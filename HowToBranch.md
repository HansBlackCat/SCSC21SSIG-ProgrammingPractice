# Learn *Branching* for great benefit

## If you're not familiar with git..
go to these link
- [git - the simple guide](http://rogerdudler.github.io/git-guide/index.html)
- [git - 간편 안내서](http://rogerdudler.github.io/git-guide/index.ko.html)
- [누구나 쉽게 이해할 수 있는 Git 입문](https://backlog.com/git-tutorial/kr/intro/intro1_1.html)
- [생활코딩 Git 1](https://www.youtube.com/playlist?list=PLuHgQVnccGMCNJESahrVV-uYGMNYK_vMf)

## Why we need branching

Branching 을 하는 목적은 각자 독립된 공간에서 푼 문제들을 간섭없이 올리기 위함 입니다. 


만약 Branching 을 하지 않으면 

동일한 자료 e 를 받아서 A 와 B 가 작업하게 되는 상황을 가정해 봅시다. A 와 B 는 성공적으로 과제를 끝냈고, 둘다 GitHub 에 자신이 푼 내용을 올립니다 (Github Repository 에 push). 만약 B 가 A 보다 늦게 푼 내용을 올리면, GitHub 에는 A 가 푼 것은 덮어 씌워져 사라져 버리고, B 가 푼 것만 남습니다. 

A 는 `e` 를 `e_a` 로 바꾸었고, B 는 `e` 를 `e_b` 로 바꾸었다고 하면 우리 입장에서는 `e -> e_a -> e_a 를 포함한 e_b` 이렇게 수정된다고 생각할 수 있으나, GitHub 입장에서는 먼저 A 가 `e -> e_a` 로 수정하고, B 가 `e_a -강제로 e 로 되돌림 -> e -> e_b` 로 수정했다고 생각하기 때문입니다. 

>사실 -f 명령어를 넣지 않으면 이런 일이 발생하지는 않습니다. 필요성을 설명하기 위해 조금 극단적으로 예시를 들었습니다. 아마 이런 케이스에는 아예 push 자체가 안될 겁니다.

근데 Branching 을 하게 되면 각 인원은 자기만의 `e` 를 가지게 됩니다. 그러면 나중에 제가 한번에 취합해서 `main` branch 에 취합하기 쉬워집니다.

## How to branching

1. 먼저 repository 를 clone 합시다
```bash
git clone git@github.com:HansBlackCat/SCSC21SSIG-ProgrammingPractice.git
```
> git@ 으로 시작하는 이 주소는 뭔가요? <br/>
> [여기 있는 주소 입니다](./img/gitid.png)

2. local repository 에 branch 를 만들고 그 branch 로 이동합니다.
```bash
git checkout -b <자기깃헙아이디>
```  
> 제 깃헙 아이디는 HansBlackCat 이니까<br/>
> git checkout -b HansBlackCat<br/>
> 이런식 입니다.

3. 깃 기초 자료에 나와있는 것처럼 이 안에서 수정을 하고, add, commit를 합니다.

4. 이제 다 완성이 되었으면 Remote Repository(Github) 에 push 를 합니다.
```bash
git push --set-upstream origin <자기깃헙아이디>
```
> 그냥 git push 가 아니라 뒤에 --set-upstream 이 붙는 이유는, 우리는 local 컴퓨터에 branch 를 만들어서 branch 가 생성되었다는 것을 알고 있지만, GitHub 는 우리 컴퓨터 속에서 어떤 일이 벌어졌는지 모르기 떄문입니다. GitHub 입장에서는 "잉? 이런 브랜치 명은 등록이 안되어 있는데? 제대로 입력해준게 맞아?" 라고 되물어 볼 겁니다. 이떄 --set-upstream 을 해 주면 내가 만든 branch 를 인정해줘! 이런 의미가 됩니다. 따라서 GitHub 도 "아 이건 너가 만든 branch 구나. 홈페이지에 추가해줄께" 하면서 branch 가 반영이 됩니다.

> GitHub 가 자기 branch 를 잘 추가했는지 알기 위해서는 <br/>
> [이곳을 봐서 자기가 추가한 branch 가 있는지 확인해보면 됩니다.](./img/branch.png)

이 과정이 마치면 나중에 제가 Pull Request 과정을 통해 `main` 브랜치에 다 합쳐놓겠습니다.

## 주의사항

**절때 NEVER** `main` branch 에서 작업을 하고 push 하면 안됩니다.

다른 사람이 했던 모든 것이 뒤엎어질 수 있습니다.

