#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <cstdlib>
#include <ctime>

#include "maze_make.h"


using namespace std;

vector<vector<room>> maze;

void makeMaze(int n, int m) {
    //n: width, m: height

    srand(time(0));

    maze.clear();
    maze.resize(m + 1, vector<room>(n + 1));
    map<int, vector<int>> setInfo;
    //각 행 별로. (집합번호, (해당집합개수,해당집합수직연결개수))

    int connectFlag;
    int setNum = n + 1;
    int preSet, curSet;


    //첫 번째 줄 초기화, 수평 랜덤 연결
    for (int i = 1; i <= n; i++) {
        maze[1][i].s = i;
        setInfo.insert({ i,{1,0} });
    }
    for (int i = 2; i <= n; i++) {
        connectFlag = rand() % 2;
        if (connectFlag) {
            preSet = maze[1][i - 1].s;
            curSet = maze[1][i].s;

            maze[1][i - 1].right = 1;

            maze[1][i].s = preSet;

            setInfo[preSet][0] += 1;
            if (setInfo[curSet][0] != 0) setInfo[curSet][0] -= 1;
        }
    }

    //
    for (int i = 2; i <= m; i++) {

        //랜덤 수직 연결
        for (int j = 1; j <= n; j++) {
            connectFlag = rand() % 2;
            preSet = maze[i - 1][j].s;
            if (setInfo[preSet][0] == 1 && setInfo[preSet][1] == 0) {
                maze[i - 1][j].bottom = 1;

                maze[i][j].s = preSet;
            }
            else if (connectFlag) {
                maze[i - 1][j].bottom = 1;

                maze[i][j].s = preSet;

                setInfo[preSet][1] += 1;
            }
            else {
                maze[i][j].s = setNum++;
            }

            if (setInfo[preSet][0] != 0) setInfo[preSet][0] -= 1;
        }

        if (i == m) break;

        //랜덤 수평 연결
        connectFlag = 0;
        for (int j = 2; j <= n; j++) {

            connectFlag = rand() % 2;
            preSet = maze[i][j - 1].s;
            curSet = maze[i][j].s;


            if (connectFlag && preSet != curSet) {
                maze[i][j - 1].right = 1;

                int minSet = min(preSet, curSet);
                int maxSet = max(preSet, curSet);
                for (int k = 1; k <= n; k++) {
                    if (maze[i][k].s == maxSet)
                        maze[i][k].s = minSet;
                }

            }

        }

        //새로운 행에 대한 정보 갱신
        setInfo.clear();
        for (int j = 1; j <= n; j++) {
            if (setInfo.find(maze[i][j].s) == setInfo.end()) setInfo.insert({ maze[i][j].s, { 1,0 } });
            else setInfo[maze[i][j].s][0] += 1;
        }

    }

    //마지막 줄 처리
    for (int i = 2; i <= n; i++) {

        preSet = maze[m][i - 1].s;
        curSet = maze[m][i].s;
        int mergeSetNum = min(preSet, curSet);

        if (preSet != curSet) {
            maze[m][i - 1].right = 1;

            int minSet = min(preSet, curSet);
            int maxSet = max(preSet, curSet);

            for (int k = 1; k <= n; k++) {
                if (maze[m][k].s == maxSet) {
                    maze[m][k].s = minSet;
                }
            }
        }
    }

}