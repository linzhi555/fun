#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

namespace chess {

enum class Response : char {
    Success,
    ErrParseCmd,
    ErrNoPieceThere,
    ErrWrongPos,
    ErrAlreadyFinish,
    ErrNotYourTurn,
    ErrPieceRule,
    ErrSucide,
};

enum class Team : char {
    None,
    White,
    Black,
    All,

};

enum class PieceRole : char {
    None,
    King,
    Queen,
    Rook,
    Bishop,
    Knight,
    Pawn,
};

struct Piece {
    Team team;
    int moveCount;
    PieceRole role;
};

struct BoardElem {
    bool isEmpty;
    Piece piece;
};

class GameState {
    int round;
    bool isFinished;
    Team winner;
    Team turn;
    BoardElem board[64];

   public:
    GameState();
    GameState clone();
    void debug();
    // std::string Save();
    // GameState Load(const std::string& data);
    Response execute(const std::string& cmd);

   private:
    BoardElem* getPos(int x, int y);
    PieceRole getRole(int x, int y);

    bool possibleMove(int x, int y);
    bool finishState();
    int changeTurn();
};

}  // namespace chess
