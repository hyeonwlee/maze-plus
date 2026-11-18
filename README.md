# Maze Plus


## 1. 소개

C++와 openFrameworks를 활용하여 랜덤 완전미로를 생성하고, 사용자의 직접 탐색과 DFS·BFS 경로 탐색 기능을 구현한 프로젝트입니다.


## 2. 사용 기술

- 언어: C++17
- 프레임워크: openFrameworks
- 개발 환경: Visual Studio
- 메뉴 UI: ofxWinMenu


## 3. 주요 내용

미로 생성부터 사용자의 직접 탐색, 알고리즘을 이용한 경로 확인까지 다음 기능을 구현했습니다.
- 미로 생성: Eller 알고리즘을 이용한 랜덤 완전미로 생성 및 5가지 크기 선택
- 직접 탐색: 방향키를 이용한 미로 이동 및 탈출
- 경로 탐색: DFS와 BFS의 탐색 과정 및 탈출 경로 시각화
- 힌트: DFS로 구한 정답 경로를 바탕으로 다음 이동 위치 안내
- 기타 기능: 미로 재생성 및 탈출 시도 종료


## 4. 실행 방법

openFrameworks 개발 환경에서 Visual Studio 솔루션 파일(`comsil1_project_maze_plus.sln`)을 열어 빌드 및 실행합니다. 실행 시 화면 표시용 폰트 파일 `verdana.ttf`가 필요합니다.
