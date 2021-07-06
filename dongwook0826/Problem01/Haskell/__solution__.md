# Red-Black Tree in Haskell

written by: dongwook0826

## 컴파일 및 실행 환경

Windows 10 운영체제에서 Powershell 콘솔을 통해 컴파일했습니다. 컴파일 및 실행 명령어는 다음과 같으며, GHC version 8.10.2로 컴파일하였습니다.

```Powershell
ghc RBTreeMain.hs
./RBTreeMain.exe
```

## 메인 함수 및 모듈 구성

`RBTree.hs`에 red-black tree와 키 검색, 삽입, 삭제를 포함한 각종 함수들을 모두 구현하였으며, `RBTreeMain.hs`에서 이를 `import`하여 `Integer` 타입 자료들을 차례로 삽입, 검색 및 삭제해보는 간단한 테스트를 진행합니다.

## 출력 결과

```
 15 (Black)
- 6 (Red)
-- 1 (Black)
-+ 11 (Black)
-+- 8 (Red)
-++ 13 (Red)
+ 22 (Red)
+- 17 (Black)
+-- 15 (Red)
+-+ 17 (Red)
++ 27 (Black)
++- 25 (Red)

[True,False,True,False,True,False,True]

del list 1

 15 (Black)
- 8 (Black)
-- 6 (Red)
+ 22 (Red)
+- 17 (Black)
+-+ 17 (Red)
++ 25 (Black)

del list 2

 15 (Black)
- 11 (Red)
-- 1 (Black)
-+ 13 (Black)
+ 27 (Black)

re-building...

 15 (Black)
- 11 (Black)
-- 2 (Red)
--- 1 (Black)
--+ 8 (Black)
--+- 6 (Red)
--++ 9 (Red)
-+ 12 (Black)
-++ 13 (Red)
+ 17 (Black)
+- 17 (Black)
+-- 15 (Red)
++ 25 (Black)
++- 22 (Red)
+++ 27 (Red)

```