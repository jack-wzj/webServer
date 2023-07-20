#pragma once
#include <array>
#include <vector>
#include <string>

using namespace std;

class Connection;

// 格子状态
enum GridType {
    WATER = 0,
    SHIP,
    MISS,
    HIT
};

// 棋盘
class GameRoom {
public:
    static const size_t BOARD_SIZE = 8;
    
    GameRoom(Connection *, Connection *);
    ~GameRoom();
    void initRandomBoard(int);

    bool isValidCoord(int, int);
    bool isWin();
    bool hit(int, int);
    void switchPlayer();

    int execute(int, int);

    int getCurrentPlayer() { return currentPlayer; }
    int getOtherPlayer() { return otherPlayer; }

    

    vector<string> getBoard(int);

private:
    // 申请两个10*10的array，用于存储两个玩家的棋盘
    // 0表示空，1表示有，2表示炸空，3表示炸中
    array<array<int, BOARD_SIZE>, BOARD_SIZE> chessBoard[2];
    Connection *player[2];
    int leftShipNum[2];
    int currentPlayer;
    int otherPlayer;
    
};