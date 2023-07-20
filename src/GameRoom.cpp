#include "GameRoom.h"
#include "Connection.h"


GameRoom::GameRoom(Connection *p1, Connection *p2) {
    player[0] = p1;
    player[1] = p2;
    // 初始化
    for (int i = 0; i < 2; i++) {
        initRandomBoard(i);
        leftShipNum[i] = BOARD_SIZE;
    }
    // 当前玩家为 0
    currentPlayer = 0;
    otherPlayer = 1;
}

GameRoom::~GameRoom() {
}

void GameRoom::initRandomBoard(int player) {
    // 随机生成棋盘
    srand(time(nullptr));
    for (int i = 0; i < BOARD_SIZE; i++) {
        int x = rand() % BOARD_SIZE;
        int y = rand() % BOARD_SIZE;
        if (chessBoard[player][x][y] == 0) {
            chessBoard[player][x][y] = GridType::SHIP;
        } else {
            i--;
        }
    }
}

bool GameRoom::isValidCoord(int x, int y) {
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE && chessBoard[currentPlayer][x][y] == 0;
}

bool GameRoom::isWin() {
    return leftShipNum[otherPlayer] == 0;
}

bool GameRoom::hit(int x, int y) {
    if (chessBoard[otherPlayer][x][y] == GridType::SHIP) {
        chessBoard[otherPlayer][x][y] = GridType::HIT;
        leftShipNum[otherPlayer]--;
        return true;
    } else if (chessBoard[otherPlayer][x][y] == GridType::WATER) {
        chessBoard[otherPlayer][x][y] = GridType::MISS;
    }
    return false;
}

void GameRoom::switchPlayer() {
    currentPlayer = 1 - currentPlayer;
    otherPlayer = 1 - otherPlayer;
}

vector<string> GameRoom::getBoard(int player) {
    vector<string> ret;
    for (int i = 0; i < BOARD_SIZE; i++) {
        string line;
        for (int j = 0; j < BOARD_SIZE; j++) {
            if ( chessBoard[player][i][j] == GridType::WATER ) {
                line += "W ";
            } else if ( chessBoard[player][i][j] == GridType::SHIP ) {
                line += "S";
            } else if ( chessBoard[player][i][j] == GridType::MISS ) {
                line += "X";
            } else if ( chessBoard[player][i][j] == GridType::HIT ) {
                line += "O";
            }
        }
        ret.push_back(line);
    }
    return ret;
}
// -1: 无效坐标, 0: 未击中, 1: 击中, 2: 胜利
int GameRoom::execute(int x, int y) {
    int ret = 0;
    if (isValidCoord(x, y)) {
        if (hit(x, y)) {
            ret = isWin()? 2: 1;
        } else {
            ret = 0;
        }
        switchPlayer();
        return ret;
    }
    else {
        return -1;
    }
}