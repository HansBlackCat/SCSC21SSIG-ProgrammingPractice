import RBTree (RBTree, contains, delete, buildRBTree)

myList :: [Integer]
myList = [15, 22, 6, 1, 17, 27, 15, 13, 8, 25, 11, 17]

mySearchList :: [Integer]
mySearchList = [8, 12, 17, 9, 25, 2, 15]

myDelList1 :: [Integer]
myDelList1 = [1, 11, 15, 12, 27, 13, 2]

myDelList2 :: [Integer]
myDelList2 = [17, 9, 17, 22, 15, 8, 6, 25]

main :: IO ()
main = let myTree = buildRBTree myList in do
    print myTree
    print $ map (myTree `contains`) mySearchList
    putStrLn "\ndel list 1\n"
    print $ foldl delete myTree myDelList1
    putStrLn "del list 2\n"
    print $ foldl delete myTree myDelList2
    putStrLn "re-building...\n"
    print $ buildRBTree $ myDelList1 ++ myDelList2
