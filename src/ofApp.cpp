/*

	ofxWinMenu basic example - ofApp.cpp

	Example of using ofxWinMenu addon to create a menu for a Microsoft Windows application.

	Copyright (C) 2016-2017 Lynn Jarvis.

	https://github.com/leadedge

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

	03.11.16 - minor comment cleanup
	21.02.17 - rebuild for OF 0.9.8

*/
#include "ofApp.h"
#include <iostream>
#include <vector>
#include <string.h>
#include <stack>
#include <queue>
#include <utility>


using namespace std;



//--------------------------------------------------------------
void ofApp::setup() {

	ofSetWindowTitle("Maze Project"); // Set the app name on the title bar
	ofSetFrameRate(15);
	ofBackground(255,255,255);
	// Get the window size for image loading
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();
	isdfs = false;
	// Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth() - windowWidth) / 2, (ofGetScreenHeight() - windowHeight) / 2);

	// Load a font rather than the default
	myFont.loadFont("verdana.ttf", 12, true, true);
	biggerFont.loadFont("verdana.ttf", 25, true, true);
	midiumFont.loadFont("verdana.ttf", 18, true, true);

	// Load an image for the example
	//myImage.loadImage("lighthouse.jpg");

	// Window handle used for topmost function
	hWnd = WindowFromDC(wglGetCurrentDC());

	// Disable escape key exit so we can exit fullscreen with Escape (see keyPressed)
	ofSetEscapeQuitsApp(false);

	//
	// Create a menu using ofxWinMenu
	//

	// A new menu object with a pointer to this class
	menu = new ofxWinMenu(this, hWnd);

	// Register an ofApp function that is called when a menu item is selected.
	// The function can be called anything but must exist. 
	// See the example "appMenuFunction".
	menu->CreateMenuFunction(&ofApp::appMenuFunction);

	// Create a window menu
	HMENU hMenu = menu->CreateWindowMenu();

	//
	// Create a "File" popup menu
	//
	HMENU hPopup = menu->AddPopupMenu(hMenu, "File");

	//
	// Add popup items to the File menu
	//
	
	// Open an maze file
	menu->AddPopupItem(hPopup, "Renew", false, false); // Not checked and not auto-checked
	// Final File popup menu item is "Exit" - add a separator before it
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Exit", false, false);


	//
	//추가: 사용자 직접 이동 기능
	//
	hPopup = menu->AddPopupMenu(hMenu, "UserGamePlay!");
	menu->AddPopupItem(hPopup, "Start", false, false);
	menu->AddPopupItem(hPopup, "End", false, false);


	//
	// View popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "View");

	bShowInfo = true;  // screen info display on
	bShowDFS = false;
	bShowBFS = false;
	menu->AddPopupItem(hPopup, "Show DFS"); // Checked
	bTopmost = false; // app is topmost
	menu->AddPopupItem(hPopup, "Show BFS"); // Not checked (default)
	bFullscreen = false; // not fullscreen yet
	menu->AddPopupItem(hPopup, "Full screen", false, false); // Not checked and not auto-check

	//
	// Help popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "Help");
	menu->AddPopupItem(hPopup, "About", false, false); // No auto check

	// Set the menu to the window
	menu->SetWindowMenu();


} // end Setup


//
// Menu function
//
// This function is called by ofxWinMenu when an item is selected.
// The the title and state can be checked for required action.
// 
void ofApp::appMenuFunction(string title, bool bChecked) {

	ofFileDialogResult result;
	string filePath;
	size_t pos;

	
	//
	// File menu
	//
	if (title == "Renew") {
		freeMemory();//메모리 해제
		isselected = 0;//새로운 미로 가져오기 위해서
	}
	if (title == "Exit") {
		freeMemory();//메모리 해제
		ofExit();//Quit the application
	}
	
	
	//미로가 만들어져야 아래 메뉴 키들이 작동하도록.
	if (!isselected) {
		return;
	}

	//
	//추가: 사용자 직접 이동 기능
	//
	if (title == "Start") {
		isGameStart = 1;
		gameStart();
	}

	if (title == "End") {
		isGameStart = 0;
		isGameEnd = 1;
	}



	//
	// Window menu
	//
	if (title == "Show DFS") {
		if (!isGameStart) {
			//bShowInfo = bChecked;  // Flag is used elsewhere in Draw()
			DFS();
			bShowDFS = bChecked;
			bShowBFS = false;
			menu->SetPopupItem("Show DFS", bShowDFS);
			menu->SetPopupItem("Show BFS", bShowBFS);
		}
	}

	if (title == "Show BFS") {
		if (!isGameStart) {
			doTopmost(bChecked); // Use the checked value directly
			BFS();
			bShowBFS = bChecked;
			bShowDFS = false;
			menu->SetPopupItem("Show DFS", bShowDFS);
			menu->SetPopupItem("Show BFS", bShowBFS);
		}
	}

	if (title == "Full screen") {
		bFullscreen = !bFullscreen; // Not auto-checked and also used in the keyPressed function
		doFullScreen(bFullscreen); // But als take action immediately
	}

	//
	// Help menu
	//
	if (title == "About") {
		ofSystemAlertDialog("ofxWinMenu\nbasic example\n\nhttp://spout.zeal.co");
	}

} // end appMenuFunction


//--------------------------------------------------------------
void ofApp::update() {

	if (isselected && isGameStart) {
		hint.buttonRect.set(ofGetWidth() / 2 - 100, ofGetHeight() / 2 + (HEIGHT) * (cellSize + 1) / 2 + 2, 200, 40);
		hint.content = "hint";
	}

	if (hint.buttonClicked && (player.y == h.first && player.x == h.second)) {
		hint.buttonClicked = false;
	}

}


//--------------------------------------------------------------
void ofApp::draw() {

	char str[256];
	ofSetLineWidth(15);
	int i, j;


	if (bShowInfo) {
		// Show keyboard duplicates of menu functions
		sprintf(str, " comsil project");
		myFont.drawString(str, 15, ofGetHeight() - 20);
	}


	//게임 진행 중이면 위에 메시지 띄워두기, 버튼 보이기
	if (isselected && isGameStart) {
		s = "--- Game in progress ---";
		ofRectangle bounds = biggerFont.getStringBoundingBox(s, 0, 0);
		biggerFont.drawString(s, ofGetWidth() / 2 - bounds.width / 2, ofGetHeight() / 2 - (HEIGHT) * (cellSize + 1) / 2 - 15);

		ofSetColor(215, 228, 239);
		ofDrawRectangle(hint.buttonRect);

		ofSetColor(255, 255, 255);
		midiumFont.drawString(hint.content, hint.buttonRect.getX()+80, hint.buttonRect.getY()+25);
	}


	//사용자가 선택한 크기에 따라 미로 생성되도록
	//선택지 띄우기
	if (!isselected) {

		ofSetColor(0, 0, 102);
		string s = "CHOOSE MAZE SIZE (width x height)";
		ofRectangle bounds = biggerFont.getStringBoundingBox(s, 0, 0);
		biggerFont.drawString(s, ofGetWidth()/2 - bounds.width/2, ofGetHeight()/3);
		for (int i = 0; i < 5; i++) {
			s = mazesize[i];
			bounds = biggerFont.getStringBoundingBox(s, 0, 0);
			biggerFont.drawString(s, ofGetWidth()/2 - bounds.width/2, ofGetHeight()/3 + 60 + i*40);
		}

		return;
	}


	//미로 그리기
	if (maze.empty()) return;

	startX = (ofGetWidth() - WIDTH * cellSize) / 2;
	startY = (ofGetHeight() - (HEIGHT) * (cellSize+1)) / 2;
	ofSetColor(215, 228, 239);
	ofDrawRectangle(startX, startY, WIDTH * (cellSize), HEIGHT * (cellSize));

	ofSetColor(255, 255, 255);
	//왼쪽, 위 윤곽선
	for (i = 0; i < HEIGHT; i++) {
		ofDrawLine(startX, startY + i * cellSize, startX, startY + (i + 1) * cellSize);
	}
	for (j = 0; j < WIDTH; j++) {
		ofDrawLine(startX + j * cellSize, startY, startX + (j + 1) * cellSize, startY);
	}

	//미로 내부
	for (i = 1; i <= HEIGHT; i++) {
		for (j = 1; j <= WIDTH; j++) {
			int x = startX + j * cellSize;
			int y = startY + (i - 1) * cellSize;

			if (!maze[i][j].right) {
				ofDrawLine(x, y, x, y + cellSize);
			}

			if (!maze[i][j].bottom) {
				ofDrawLine(x, y + cellSize, x - cellSize, y + cellSize);
			}

		}
	}



	//bfs 및 dfs 경로
	if (bShowDFS && isdfs)
	{
		//ofSetColor(200);
		ofSetLineWidth(15);
		dfsdraw();
	}

	if (bShowBFS && isbfs)
	{
		//ofSetColor(200);
		ofSetLineWidth(15);
		bfsdraw();
	}



	//사용자 이동 경로 그리기
	if (isGameStart) {
		ofSetColor(0, 0, 102);
		ofSetLineWidth(20);
		movedraw();
	}


	//힌트 제공되면
	if (hint.buttonClicked) {
		ofPushStyle();
		ofSetColor(255, 251, 228);
		if (h.second == -1 && h.first == -1) {
			h = userpath[userpath.size() - 2];
		}
		ofDrawCircle(startX + h.second * cellSize - cellSize / 2, startY + h.first * cellSize - cellSize / 2, cellSize / 4);
		ofPopStyle();
	}


	//사용자 탈출 지점 도달했을 때
	if (isGameComplete) {
		ofSetColor(255, 251, 228, 180);
		ofSetLineWidth(10);

		ofDrawRectangle(ofGetWidth() / 2 - 250, ofGetHeight() / 2 - 100, 500, 200);

		ofNoFill();
		ofSetColor(240, 198, 31);
		ofDrawRectangle(ofGetWidth() / 2 - 250, ofGetHeight() / 2 - 100, 500, 200);
		ofFill();

		s = "SUCCESS!";
		ofRectangle bounds = biggerFont.getStringBoundingBox(s, 0, 0);
		biggerFont.drawString(s, ofGetWidth() / 2 - bounds.width / 2, ofGetHeight() / 2);

		s = "press the 'Enter' to continue";
		bounds = midiumFont.getStringBoundingBox(s, 0, 0);
		midiumFont.drawString(s, ofGetWidth() / 2 - bounds.width / 2, ofGetHeight() / 2 + 35);
	}


} // end Draw


void ofApp::doFullScreen(bool bFull)
{
	// Enter full screen
	if (bFull) {
		// Remove the menu but don't destroy it
		menu->RemoveWindowMenu();
		// hide the cursor
		ofHideCursor();
		// Set full screen
		ofSetFullscreen(true);
	}
	else {
		// return from full screen
		ofSetFullscreen(false);
		// Restore the menu
		menu->SetWindowMenu();
		// Restore the window size allowing for the menu
		ofSetWindowShape(windowWidth, windowHeight + GetSystemMetrics(SM_CYMENU));
		// Centre on the screen
		ofSetWindowPosition((ofGetScreenWidth() - ofGetWidth()) / 2, (ofGetScreenHeight() - ofGetHeight()) / 2);
		// Show the cursor again
		ofShowCursor();
		// Restore topmost state
		if (bTopmost) doTopmost(true);
	}

} // end doFullScreen


void ofApp::doTopmost(bool bTop)
{
	if (bTop) {
		// get the current top window for return
		hWndForeground = GetForegroundWindow();
		// Set this window topmost
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
	}
	else {
		SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
		// Reset the window that was topmost before
		if (GetWindowLong(hWndForeground, GWL_EXSTYLE) & WS_EX_TOPMOST)
			SetWindowPos(hWndForeground, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		else
			SetWindowPos(hWndForeground, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
} // end doTopmost


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	// Escape key exit has been disabled but it can be checked here
	if (key == VK_ESCAPE) {
		// Disable fullscreen set, otherwise quit the application as usual
		if (bFullscreen) {
			bFullscreen = false;
			doFullScreen(false);
		}
		else {
			ofExit();
		}
	}

	// Remove or show screen info
	if (key == ' ') {
		bShowInfo = !bShowInfo;
	}

	if (key == 'f') {
		bFullscreen = !bFullscreen;
		doFullScreen(bFullscreen);
		// Do not check this menu item
		// If there is no menu when you call the SetPopupItem function it will crash
	}


	//사용자가 미로 크기 선택
	if (!isselected) {
		if (key == '1') {
			WIDTH = 5;
			HEIGHT = 5;
			isselected = 1;
		}
		else if (key == '2') {
			WIDTH = 10;
			HEIGHT = 10;
			isselected = 1;
		}
		else if (key == '3') {
			WIDTH = 20;
			HEIGHT = 10;
			isselected = 1;
		}
		else if (key == '4') {
			WIDTH = 20;
			HEIGHT = 15;
			isselected = 1;
		}
		else if (key == '5') {
			WIDTH = 30;
			HEIGHT = 15;
			isselected = 1;
		}
		else {
			cout << "Try other number: '1' ~ '5' " << endl;
		}

		if (isselected) {
			makeMaze(WIDTH, HEIGHT);
			projectStart();
		}
	}


	//사용자 직접 이동
	if (key == OF_KEY_UP) move(player.y - 1, player.x);
	if (key == OF_KEY_DOWN) move(player.y + 1, player.x);
	if (key == OF_KEY_LEFT) move(player.y, player.x - 1);
	if (key == OF_KEY_RIGHT) move(player.y, player.x + 1);

	if (isGameComplete) {
		if (key == OF_KEY_RETURN) {
			freeMemory();//메모리 해제
			isselected = 0;//처음 화면으로 이동하도록
		}
	}


} // end keyPressed

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	
	if (hint.buttonRect.inside(x, y)) {
		hint.buttonClicked = true;
		h = getHint();
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}


void ofApp::freeMemory() {

	//TO DO
	// malloc한 memory를 free해주는 함수

	if (isselected) {
		for (int i = 0; i < HEIGHT + 1; i++) {
			free(visited[i]);
		}
		free(visited);
	}

	if (isselected) {
		for (int i = 0; i < HEIGHT + 1; i++) {
			free(went[i]);
		}
		free(went);
	}
}



//--------------------------------------------------------------
bool ofApp::DFS()//DFS탐색을 하는 함수
{
	//TO DO
	//DFS탐색을 하는 함수 ( 3주차)


	//초기화
	for (int i = 0; i < HEIGHT + 1; i++) {
		memset(visited[i], 0, (WIDTH + 1) * sizeof(int));
	}
	allpath.clear();
	realpath.clear();


	//dfs
	S.push({ 1,1 });
	visited[1][1] = 1;

	while (!S.empty()) {

		int y = S.top().first;
		int x = S.top().second;

		allpath.push_back({ y,x });

		if (x == WIDTH && y == HEIGHT) break;

		if (maze[y][x].right && !visited[y][x + 1]) x += 1;
		else if (maze[y][x].bottom && !visited[y + 1][x]) y += 1;
		else if (x != 1 && (maze[y][x - 1].right && !visited[y][x - 1])) x -= 1;
		else if (y != 1 && (maze[y - 1][x].bottom && !visited[y - 1][x])) y -= 1;
		else {
			S.pop();
			continue;
		}

		S.push({ y,x });
		visited[y][x] = 1;

	}

	while (!S.empty()) {
		realpath.push_back({ S.top() });
		S.pop();
	}

	isdfs = true;
	isbfs = false;

	return 0;
}

void ofApp::dfsdraw()
{
	//TO DO 
	//DFS를 수행한 결과를 그린다. (3주차 내용)

	for (int i = 1; i < allpath.size(); i++) {

		int x1 = startX + (allpath[i-1].second) * (cellSize);
		int y1 = startY + (allpath[i-1].first - 1) * (cellSize);

		int x2 = startX + (allpath[i].second) * (cellSize);
		int y2 = startY + (allpath[i].first - 1) * (cellSize);

		ofSetColor(0, 97, 77, 50);

		ofDrawLine(x1 - cellSize / 2, y1 + cellSize / 2, x2 - cellSize / 2, y2 + cellSize / 2);

	}

	for (int i = 1; i < realpath.size(); i++) {

		int x1 = startX + (realpath[i - 1].second) * (cellSize);
		int y1 = startY + (realpath[i - 1].first - 1) * (cellSize);

		int x2 = startX + (realpath[i].second) * (cellSize);
		int y2 = startY + (realpath[i].first - 1) * (cellSize);

		ofSetColor(0, 97, 77);

		ofDrawLine(x1 - cellSize / 2, y1 + cellSize / 2, x2 - cellSize / 2, y2 + cellSize / 2);

	}


}



bool ofApp::BFS() {

	//초기화
	for (int i = 0; i < HEIGHT + 1; i++) {
		memset(visited[i], 0, (WIDTH + 1) * sizeof(int));
	}
	allpath_more.clear();


	//bfs
	q.push({ 1,1 });
	visited[1][1] = 1;

	while (!q.empty()) {

		int x = q.front().second;
		int y = q.front().first;
		q.pop();

		if (x == WIDTH && y == HEIGHT) break;

		if (maze[y][x].right && !visited[y][x + 1]) {
			q.push({ y,x + 1 });
			visited[y][x + 1] = 1;

			allpath_more.push_back({ y,x });
			allpath_more.push_back({ y,x + 1 });
		}
		if (maze[y][x].bottom && !visited[y + 1][x]) {
			q.push({ y + 1,x });
			visited[y + 1][x] = 1;

			allpath_more.push_back({ y,x });
			allpath_more.push_back({ y + 1,x });
		}
		if (x != 1 && (maze[y][x - 1].right && !visited[y][x - 1])) {
			q.push({ y,x - 1 });
			visited[y][x - 1] = 1;

			allpath_more.push_back({ y,x });
			allpath_more.push_back({ y,x - 1 });
		}
		if (y != 1 && (maze[y - 1][x].bottom && !visited[y - 1][x])) {
			q.push({ y - 1,x });
			visited[y - 1][x] = 1;

			allpath_more.push_back({ y,x });
			allpath_more.push_back({ y - 1,x });
		}

	}

	while (!q.empty()) {
		q.pop();
	}

	isbfs = true;
	isdfs = false;


	return 0;

}


void ofApp::bfsdraw() {

	int realX = WIDTH;
	int realY = HEIGHT;
	for (int i = allpath_more.size() - 1; i >= 0; i -= 2) {

		int x1 = allpath_more[i].second;
		int y1 = allpath_more[i].first;

		int x2 = allpath_more[i - 1].second;
		int y2 = allpath_more[i - 1].first;

		if (x1 == realX && y1 == realY) {
			ofSetColor(0, 97, 77);
			realX = x2;
			realY = y2;
		}
		else {
			ofSetColor(0, 97, 77, 100);
		}

		ofDrawLine(startX + x1 * cellSize - cellSize / 2, startY + y1 * cellSize - cellSize / 2,
			startX + x2 * cellSize - cellSize / 2, startY + y2 * cellSize - cellSize / 2);

	}

}






//--------------------------------------------------------------
void ofApp::projectStart() {

	isdfs = false;
	isbfs = false;
	isGameStart = false;

	isGameComplete = false;
	isGameEnd = false;

	showButton = false;

	cout << "maze generated: (" << WIDTH << " x " << HEIGHT << ")" << endl;
	cout << "program start!" << endl;


	//필요한 변수 동적할당 및 초기화
	visited = (int**)malloc((HEIGHT + 1) * sizeof(int*));
	for (int i = 0; i < HEIGHT + 1; i++) {
		visited[i] = (int*)malloc((WIDTH + 1) * sizeof(int));
	}

	went = (int**)malloc((HEIGHT + 1) * sizeof(int*));
	for (int i = 0; i < HEIGHT + 1; i++) {
		went[i] = (int*)malloc((WIDTH + 1) * sizeof(int));
	}


}




//--------------------------------------------------------------
//추가: 사용자 직접 이동 기능
void ofApp::gameStart() {

	//초기화
	for (int i = 0; i < HEIGHT + 1; i++) {
		memset(went[i], 0, (WIDTH + 1) * sizeof(int));
	}
	went[1][1] = 1;

	userpath.clear();
	userpath.push_back({ 1,1 });
	player.x = 1;
	player.y = 1;

}


void ofApp::move(int nY, int nX) {

	if (!isGameStart) return;

	//현재 좌표 저장
	int preY = player.y;
	int preX = player.x;

	//이동 가능 여부
	if (nY == preY - 1) {
		if (preY == 1) return;
		if (maze[preY - 1][preX].bottom == 0) return;
	}
	if (nY == preY + 1) {
		if (preY == HEIGHT) return;
		if (maze[preY][preX].bottom == 0) return;

	}
	if (nX == preX - 1) {
		if (preX == 1) return;
		if (maze[preY][preX - 1].right == 0) return;
	}
	if (nX == preX + 1) {
		if (preX == WIDTH) return;
		if (maze[preY][preX].right == 0) return;
	}

	//경로 연결 여부 갱신
	if (went[nY][nX] == 1) {
		userpath.pop_back();
		went[preY][preX] = 0;
	}
	else {
		userpath.push_back({ nY, nX });
		went[nY][nX] = 1;
	}

	if (nY == HEIGHT && nX == WIDTH) {
		isGameComplete = 1;
	}

	//사용자 위치 갱신
	player.y = nY;
	player.x = nX;

}


void ofApp::movedraw() {

	int s = userpath.size();

	//이동 경로 그리기
	for (int i = 1; i < s; i++) {
		int preY = userpath[i-1].first;
		int preX = userpath[i-1].second;
		int curY = userpath[i].first;
		int curX = userpath[i].second;

		ofDrawLine(startX + preX * cellSize - cellSize / 2, startY + preY * cellSize - cellSize / 2,
			startX + curX * cellSize - cellSize / 2, startY + curY * cellSize - cellSize / 2);
	}

	//현재 사용자 위치 그리기
	ofDrawCircle(startX + userpath[s - 1].second * cellSize - cellSize / 2, startY + userpath[s - 1].first * cellSize - cellSize / 2, cellSize/4);

}





//--------------------------------------------------------------
pair<int,int> ofApp::getHint() {

	DFS();//정답 탈출경로 찾기 위해
	for (int i = realpath.size()-1; i >=0; i--) {
		//탈출경로 역순으로 저장된 벡터 거꾸로 순회
		
		//현재 좌표가 탈출경로에 있다면, 그 좌표를 반환
		if (realpath[i].first == player.y && realpath[i].second == player.x) {
			return realpath[i - 1];
		}
	}

	//현재 좌표가 탈출경로에 없다면, 잘못된 이동 중임을 뜻한다.
	return { -1,-1 };
	
}