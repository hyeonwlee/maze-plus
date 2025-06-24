/*

	ofxWinMenu basic example - ofApp.h

	Copyright (C) 2016-2017 Lynn Jarvis.

	http://www.spout.zeal.co

	=========================================================================
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
	=========================================================================
*/

#pragma once

#include "ofMain.h"
#include "ofxWinMenu.h" // Menu addon
#include "maze_make.h"  // maze make
#include <vector>
#include <utility>
#include <stack>
#include <string.h>



struct Player {
	int x, y;//사용자 현재 좌표
};

struct Button {
	ofRectangle buttonRect;//버튼 위치, 크기
	string content;//표시 문자열
	bool buttonClicked = false;//버튼의 클릭 여부(한 번 단위로 갱신)
};


class ofApp : public ofBaseApp {

public:

	void setup();
	void update();
	void draw();

	void keyPressed(int key); // Traps escape key if exit disabled
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	void freeMemory();
	bool DFS();
	bool BFS();
	void dfsdraw();
	void bfsdraw();
	int HEIGHT;//미로의 높이
	int WIDTH;//미로의 너비
	
	int** visited;//방문여부를 저장할 포인터
	int isdfs;//DFS함수를 실행시켰는지 판단하는 변수. 0이면 실행안했고 1이면 실행했다.
	int isbfs;//BFS함수를 실행시켰는지 판단하는 변수. 0이면 실행안했고 1이면 실행했다.
	
	stack<pair<int, int>> S;//dfs 구현 위한 스택
	vector<pair<int, int>> allpath;//dfs 탐색 모든 경로 저장
	vector<pair<int, int>> realpath;//dfs 탐색 실제 탈출 경로 저장

	queue<pair<int, int>> q;//bfs 구현 위한 큐
	vector<pair<int, int>> allpath_more;//bfs 탐색 모든 경로 저장

	// Menu
	ofxWinMenu* menu; // Menu object
	void appMenuFunction(string title, bool bChecked); // Menu return function

	float windowWidth, windowHeight;

	// Used by example app
	ofTrueTypeFont myFont;
	ofTrueTypeFont midiumFont;
	ofTrueTypeFont biggerFont;//폰트 크기 다르게 하여 세 가지로 지정
	
	HWND hWnd; // Application window
	HWND hWndForeground; // current foreground window

	// Example menu variables
	bool bShowInfo;
	bool bShowDFS;
	bool bShowBFS;
	bool bFullscreen;
	bool bTopmost;
	// Example functions
	void doFullScreen(bool bFull);
	void doTopmost(bool bTop);

	int cellSize = 50;
	int startX, startY;
	



	//사용자가 선택한 크기에 따라 미로 생성되도록 하기 위한 변수 및 함수
	string s;//화면에 문자 나타낼 때 사용한 변수
	string mazesize[5] = 
		{"1. (5 x 5)", "2. (10 x 10)", "3. (20 x 10)", "4. (20 x 15)", "5. (30 x 15)"};//선택지로 제공될 미로 크기
	void projectStart();//미로 생성되면 호출되어, 프로그램 시작
	bool isselected = 0;//미로 생성(선택) 여부


	//사용자가 직접 이동 가능하게 하기 위한 변수 및 함수
	Player player;//사용자
	bool isGameStart = 0;//사용자 움직임 시작 여부
	bool isGameComplete = 0;//사용자 탈출 성공 여부
	bool isGameEnd = 0;//end 메뉴 클릭 여부
	int** went;//사용자가 방문한 지점 기록
	vector<pair<int, int>> userpath;//사용자가 간 경로 저장
	void gameStart();
	void move(int ny, int nx);
	void movedraw();


	//힌트 버튼 구현 위한 변수 및 함수
	bool showButton = false;
	Button hint;
	pair<int, int> h;//getHint로 얻은 힌트 저장
	pair<int,int> getHint();//다음 위치에 대한 힌트 하나 제공하는 함수
};
