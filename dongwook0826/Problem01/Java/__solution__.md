# Red-Black Tree in Java

written by: dongwook0826

## 컴파일 및 실행 환경

Windows 10 운영체제에서 Powershell 콘솔을 통해 컴파일했습니다. JDK 14.0.1 환경에서 작업하였으나 , 전체 컴파일 및 실행 명령어는 다음과 같습니다.

```Powershell
javac RBTree.java RBTreeMain.java
java RBTreeMain
```

## 메인 함수 및 모듈 구성

`RBTree.java`에 red-black tree와 키 검색, 삽입, 삭제를 포함한 각종 함수들을 모두 구현하였으며, `RBTreeMain.java`에서 `Integer` 타입 자료들을 차례로 삽입, 검색 및 삭제해보는 간단한 테스트를 진행합니다. 비교적 긴 출력을 통해 테스트하여, 출력 결과는 생략합니다.