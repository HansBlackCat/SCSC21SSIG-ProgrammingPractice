public class RBTreeMain {
    public static void main(String... args) {
        int[] arr = {15, 22, 6, 1, 17, 27, 15, 13, 8, 25, 11, 17};
        int[] searchArr = {8, 12, 17, 9, 25, 2, 15};
        int[] delArr1 = {1, 11, 15, 12, 27, 13, 2};
        int[] delArr2 = {17, 9, 17, 22, 15, 8, 6, 25};

        RBTree<Integer> tree = new RBTree<>();
        System.out.println("----- insert -----");
        for (int e : arr)
            tree.insert(e);
        tree.preorderPrint();
        
        System.out.println("----- search -----");
        for (int e : searchArr)
            System.out.println(tree.contains(e));
        System.out.println();
        
        System.out.println("----- delete 1 -----");
        for (int e : delArr1) {
            tree.delete(e);
            tree.preorderPrint();
        }

        System.out.println("----- rebuild -----");
        tree = new RBTree<>();
        for (int e : arr) tree.insert(e);
        tree.preorderPrint();

        System.out.println("----- delete 2 -----");
        for (int e : delArr2) {
            tree.delete(e);
            tree.preorderPrint();
        }

        System.out.println("----- rebuild++ -----");
        tree = new RBTree<>();
        for (int e : delArr1) tree.insert(e);
        for (int e : delArr2) tree.insert(e);
        tree.preorderPrint();
        
        System.out.println("----- delete++ -----");
        tree.delete(6);
        tree.delete(2);
        tree.preorderPrint();
        tree.delete(11);
        tree.preorderPrint();
    }
}
