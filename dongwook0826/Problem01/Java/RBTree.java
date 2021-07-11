public final class RBTree<E extends Comparable<E>> {

    private static final RBNode<?> NIL = new RBNode<>(
            null, NodeColor.BLACK, null, null
    );

    private RBNode<E> root = NIL();

    static <E extends Comparable<E>> RBNode<E> NIL() {
        @SuppressWarnings("unchecked")
        RBNode<E> typedNIL = (RBNode<E>) NIL;
        return typedNIL;
    }

    public boolean isEmpty() {
        return root == NIL;
    }

    public void preorderPrint() {
        preorderPrint(root, "");
        System.out.println();
    }

    private void preorderPrint(RBNode<E> node, String depth) {
        if (node == NIL) return;
        System.out.printf("%s %s (%s)\n", depth, node.element.toString(), node.color);
        preorderPrint(node.left, depth + '-');
        preorderPrint(node.right, depth + '+');
    }

    // ----- search -----

    public boolean contains(E target) {
        return search(target) != NIL;
    }

    private RBNode<E> search(E target) {
        int comp;
        for (RBNode<E> node = root; node != NIL; ) {
            comp = target.compareTo(node.element);
            if (comp == 0)     return node;
            else if (comp < 0) node = node.left;
            else               node = node.right;
        } return NIL();
    }

    // ----- insert -----

    public void insert(E newElem) {
        if (isEmpty()) { // insert as the root node
            root = new RBNode<>(newElem);
            root.setBlack();
            return;
        }
        RBNode<E> probe;
        boolean onLeft;
        {
            RBNode<E> child = root;
            do {
                probe = child;
                child = (onLeft = newElem.compareTo(probe.element) < 0)
                        ? probe.left
                        : probe.right;
            } while (child != NIL);
        }
        RBNode<E> newNode = new RBNode<>(newElem);
        connect(probe, newNode, onLeft);
        // problem occurs when nodes of depth over 2 are inserted
        modifyInsert(newNode);
    }

    private void modifyInsert(RBNode<E> node) {
        RBNode<E> parent, grand, uncle;
        boolean onLeft;
        while (true) {
            if (node == root) {
                node.setBlack();
                return;
            }
            parent = node.parent;
            if (parent.isBlack()) return;
            grand = parent.parent; // always black
            onLeft = parent == grand.left;
            uncle = onLeft ? grand.right
                    : grand.left;
            if (uncle.isBlack()) break;
            parent.setBlack();
            uncle.setBlack();
            grand.setRed();
            node = grand;
        }
        if (onLeft != node.isOnLeft())
            rotate(parent, onLeft);
        rotate(grand, !onLeft);
        grand.setRed();
        grand.parent.setBlack();
    }

    // ----- delete -----

    public void delete(E delElem) {
        RBNode<E> delNode = search(delElem);
        if (delNode == NIL) return; // target not found
        boolean delOnLeft = delNode.isOnLeft();
        if (delNode.left == delNode.right /* == NIL */) {
            if (delNode == root) {
                root = NIL();
            } else {
                connect(delNode.parent, NIL(), delOnLeft);
                if (delNode.isBlack()) // black height unbalanced
                    modifyDelete(delNode.parent, delOnLeft);
            }
        } else if (delNode.right == NIL) {
            connect(delNode.parent, delNode.left, delOnLeft);
            delNode.left.setBlack();
        } else if (delNode.left == NIL) {
            connect(delNode.parent, delNode.right, delOnLeft);
            delNode.right.setBlack();
        } else { // both children are not NIL
            hookSuccessor(delNode, delOnLeft);
        }
    }

    private void modifyDelete(RBNode<E> parent, boolean delOnLeft) {
        RBNode<E> sibling, closeNep, distNep;
        int breakFlag;
        do {
            if (delOnLeft) {
                sibling = parent.right;
                closeNep = sibling.left;
                distNep = sibling.right;
            } else {
                sibling = parent.left;
                closeNep = sibling.right;
                distNep = sibling.left;
            }
            if (!sibling.isBlack()) {
                breakFlag = 1;
            } else {
                boolean distBlack = distNep.isBlack();
                if (closeNep.isBlack() && distBlack)
                    breakFlag = parent.isBlack() ? 0 : 2;
                else
                    breakFlag = distBlack ? 3 : 4;
            }
            if (breakFlag != 0) break;
            sibling.setRed();
            delOnLeft = parent.isOnLeft();
            parent = parent.parent;
        } while (parent != NIL);
        if (breakFlag == 0) return;
        if (breakFlag == 1) {
            rotate(parent, delOnLeft);
            parent.setRed();
            sibling.setBlack();
            sibling = closeNep;
            if (delOnLeft) {
                closeNep = sibling.left;
                distNep = sibling.right;
            } else {
                closeNep = sibling.right;
                distNep = sibling.left;
            }
            boolean distBlack = distNep.isBlack();
            if (closeNep.isBlack() && distBlack)
                breakFlag = 2;
            else
                breakFlag = distBlack ? 3 : 4;
        }
        switch (breakFlag) {
            case 2:
                parent.setBlack();
                sibling.setRed();
                break;
            case 3:
                rotate(sibling, !delOnLeft);
                (distNep = sibling).setRed();
                (sibling = closeNep).setBlack();
            case 4:
                rotate(parent, delOnLeft);
                sibling.color = parent.color;
                distNep.setBlack();
                parent.setBlack();
        }
    }

    private void hookSuccessor(RBNode<E> delNode, boolean delOnLeft) {
        RBNode<E> succNode = delNode.right;
        while (succNode.left != NIL)
            succNode = succNode.left;
        RBNode<E> succParent = succNode.parent;
        connect(delNode.parent, succNode, delOnLeft);
        connect(succNode, delNode.left, true);
        boolean succIsRed = succNode.isRed();
        succNode.color = delNode.color;
        if (delNode.right == succNode) {
            if (succIsRed) return;
            if (succNode.right != NIL) {
                succNode.right.setBlack();
                return;
            }
            modifyDelete(succNode, false);
        } else {
            connect(succParent, succNode.right, true);
            connect(succNode, delNode.right, false);
            if (!succIsRed) {
                if (succParent.left.isRed()) {
                    succParent.left.setBlack();
                    return;
                }
                modifyDelete(succParent, true);
            }
        }
    }

    // ----- private util methods -----

    private void connect(RBNode<E> parent, RBNode<E> child, boolean onLeft) {
        if (parent == NIL) {
            child.parent = NIL();
            root = child;
            return;
        }
        if (onLeft) parent.left = child;
        else        parent.right = child;
        if (child != NIL)
            child.parent = parent;
    }

    private void rotate(RBNode<E> node, boolean toLeft) {
        RBNode<E> parent = node.parent, child;
        boolean nodeOnLeft = parent.left == node;
        if (toLeft) {
            child = node.right;
            connect(node, child.left, false);
            connect(child, node, true);
        } else {
            child = node.left;
            connect(node, child.right, true);
            connect(child, node, false);
        }
        connect(parent, child, nodeOnLeft);
    }
}

final class RBNode<E extends Comparable<E>> {
    E element;
    NodeColor color;
    RBNode<E> parent = null;
    RBNode<E> left, right;

    RBNode(E element) {
        this.element = element;
        color = NodeColor.RED;
        parent = left = right = RBTree.NIL();
    }

    RBNode(E element, NodeColor color, RBNode<E> left, RBNode<E> right) {
        this.element = element;
        this.color = color;
        this.left = left;
        this.right = right;
    }

    boolean isBlack() {
        return color == NodeColor.BLACK;
    }

    boolean isRed() {
        return color == NodeColor.RED;
    }

    void setBlack() {
        color = NodeColor.BLACK;
    }

    void setRed() {
        color = NodeColor.RED;
    }

    boolean isOnLeft() {
        return this == parent.left;
    }
}

enum NodeColor {
    RED, BLACK
}