module RBTree
( RBTree
, contains
, insert
, delete
, buildRBTree
) where

data NodeColor = Black | Red deriving (Eq, Show)

data RBTree a = Nil
              | Node a NodeColor (RBTree a) (RBTree a)
              deriving Eq

showWithDepth :: Show a => RBTree a -> String -> String
showWithDepth Nil _ = ""
showWithDepth (Node e c left right) dep = -- preorder traversal
    dep ++ " " ++ show e ++ " (" ++ show c ++ ")\n"
        ++ showWithDepth left (dep ++ "-")
        ++ showWithDepth right (dep ++ "+")

instance (Show a) => Show (RBTree a) where
    show tree = showWithDepth tree ""

-- private util const's & ftn's

colorOf :: RBTree a -> NodeColor
colorOf Nil = Black
colorOf (Node _ c _ _) = c

valueOf :: RBTree a -> Maybe a
valueOf Nil = Nothing
valueOf (Node e _ _ _) = Just e

setBlack :: RBTree a -> RBTree a
setBlack Nil = Nil
setBlack (Node e _ left right) = Node e Black left right

setRed :: RBTree a -> RBTree a
setRed Nil = Nil
setRed (Node e _ left right) = Node e Red left right

-- search

contains :: Ord a => RBTree a -> a -> Bool
contains Nil _ = False
contains (Node e c left right) x
    | x == e    = True
    | x < e     = left `contains` x
    | otherwise = right `contains` x

-- insert

insert :: Ord a => RBTree a -> a -> RBTree a
insert tree = setBlack . insert' tree

insert' :: Ord a => RBTree a -> a -> RBTree a
insert' Nil x = Node x Red Nil Nil
insert' (Node e c left right) x
    | x < e     = modifyInsert $ Node e c (insert' left x) right
    | otherwise = modifyInsert $ Node e c left (insert' right x)

modifyInsert :: RBTree a -> RBTree a
modifyInsert (Node g Black
    (Node p Red (Node e Red left right) pright)
    uncleTree)
    | colorOf uncleTree == Red =
        Node g Red
            (Node p Black (Node e Red left right) pright)
            (setBlack uncleTree)
    | otherwise =
        Node p Black
            (Node e Red left right)
            (Node g Red pright uncleTree)
modifyInsert (Node g Black
    (Node p Red pleft (Node e Red left right))
    uncleTree)
    | colorOf uncleTree == Red =
        Node g Red
            (Node p Black pleft (Node e Red left right))
            (setBlack uncleTree)
    | otherwise =
        modifyInsert (Node g Black
            (Node e Red (Node p Red pleft left) right)
            uncleTree)
modifyInsert (Node g Black
    uncleTree
    (Node p Red (Node e Red left right) pright))
    | colorOf uncleTree == Red =
        Node g Red
            (setBlack uncleTree)
            (Node p Black (Node e Red left right) pright)
    | otherwise =
        modifyInsert (Node g Black
            uncleTree
            (Node e Red left (Node p Red right pright)))
modifyInsert (Node g Black
    uncleTree
    (Node p Red pleft (Node e Red left right)))
    | colorOf uncleTree == Red =
        Node g Red
            (setBlack uncleTree)
            (Node p Black pleft (Node e Red left right))
    | otherwise =
        Node p Black
            (Node g Red uncleTree pleft)
            (Node e Red left right)
modifyInsert tree = tree

buildRBTree :: Ord a => [a] -> RBTree a
buildRBTree = foldl insert Nil

-- delete

delete :: Ord a => RBTree a -> a -> RBTree a
delete Nil _ = Nil
delete (Node e c Nil Nil) x -- root
    | x == e    = Nil
    | otherwise = Node e c Nil Nil
delete tree x = fst $ delete' tree x

type Direction = Bool

toRight :: Direction
toRight = True

toLeft :: Direction
toLeft = False

delete' :: Ord a => RBTree a -> a -> (RBTree a, Bool)
delete' Nil _ = (Nil, True)
delete' (Node e c left right) x
    | x < e     = let
        (deletedTree, wellDeleted) = delete' left x
        (modifiedTree, allModified) = modifyDelete wellDeleted (Node e c deletedTree right) toLeft in 
            (modifiedTree, wellDeleted || allModified)
    | x > e     = let
        (deletedTree, wellDeleted) = delete' right x
        (modifiedTree, allModified) = modifyDelete wellDeleted (Node e c left deletedTree) toRight in
            (modifiedTree, wellDeleted || allModified)
    | otherwise {- x == e -} = case (c, left, right) of
        (_, Nil, Nil) -> (Nil, c == Red) -- if Black, modification needed
        (Black, Nil, Node y _ _ _) -> (Node y Black Nil Nil, True)
        (Black, Node y _ _ _, Nil) -> (Node y Black Nil Nil, True)
        (_, _, _) -> let
            (delMinRight, delDone, Just min) = deleteMin right in
                modifyDelete delDone (Node min c left delMinRight) toRight

deleteMin :: Ord a => RBTree a -> (RBTree a, Bool, Maybe a)
deleteMin Nil = (Nil, True, Nothing)
deleteMin (Node e c left right)
    | left == Nil = case c of
        Black -> (setBlack right, right /= Nil, Just e)
        Red   -> (Nil, True, Just e)
    | otherwise = let
        (delLeft, delDone, justMin) = deleteMin left
        (delTree, wellDeleted) = modifyDelete delDone (Node e c delLeft right) toLeft in
            (delTree, wellDeleted, justMin)

modifyDelete :: Bool -> RBTree a -> Direction -> (RBTree a, Bool)
modifyDelete True tree _ = (tree, True)
modifyDelete False (Node e c left (Node s sc sleft sright)) toLeft =
    case (c, sc, colorOf sleft, colorOf sright) of
        (Black, Black, Black, Black) ->
            (Node e c left (Node s Red sleft sright), False)
        (_, Red, _, _) ->
            (Node s Black (fst $ modifyDelete False (Node e Red left sleft) toLeft) sright, True)
        (Red, _, Black, Black) ->
            (Node e Black left $ Node s Red sleft sright, True)
        (_, Black, Red, Black) -> let Node n nc nleft nright = sleft in
            (Node n c (Node e Black left nleft) (Node s Black nright sright), True)
        (_, Black, _, Red) ->
            (Node s c (Node e Black left sleft) $ setBlack sright, True)
modifyDelete False (Node e c (Node s sc sleft sright) right) toRight =
    case (c, sc, colorOf sleft, colorOf sright) of
        (Black, Black, Black, Black) ->
            (Node e c (Node s Red sleft sright) right, False)
        (_, Red, _, _) ->
            (Node s Black sleft (fst $ modifyDelete False (Node e Red sright right) toRight), True)
        (Red, _, Black, Black) ->
            (Node e Black (Node s Red sleft sright) right, True)
        (_, Black, Black, Red) -> let Node n nc nleft nright = sright in
            (Node n c (Node s Black sleft nleft) (Node e Black nright right), True)
        (_, Black, Red, _) ->
            (Node s c (setBlack sleft) $ Node e Black sright right, True)
modifyDelete False tree _ = (tree, True)
