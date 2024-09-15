#include "chess.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

namespace chess {

const char* responseTostr(Response r) {
    switch (r) {
    case Response::Success:
        return "Success";
    case Response::ErrParseCmd:
        return "ErrParseCmd";
    case Response::ErrNoPieceThere:
        return "ErrNoPieceThere";
    case Response::ErrWrongPos:
        return "ErrWrongPos";
    case Response::ErrAlreadyFinish:
        return "ErrAlreadyFinish";
    case Response::ErrNotYourTurn:
        return "ErrNotYourTurn";
    case Response::ErrPieceRule:
        return "ErrPieceRule";
    case Response::ErrPawnMove:
        return "ErrPawnMove";
    case Response::ErrKingMove:
        return "ErrKingMove";
    case Response::ErrBishopMove:
        return "ErrBishopMove";
    case Response::ErrKnightMove:
        return "ErrKnightMove";
    case Response::ErrQueenMove:
        return "ErrQueenMove";
    case Response::ErrRookMove:
        return "ErrRookMove";
    case Response::ErrBlocked:
        return "ErrBlocked";
    case Response::ErrKillSame:
        return "ErrKillSame";
    case Response::ErrSucide:
        return "ErrSucide";
    }
    return "WrongTypeResponse";
}

enum class ChangeType : char {
    NoChange,
    Add,
    Delete,
};

struct Change {
    ChangeType type;
    Piece role;
    int x;
    int y;
};

enum class CmdType : char {
    None,
    Move,
    Surrender,
    Promotion,
};

struct Cmd {
    CmdType type;
    int x_old;
    int y_old;

    int x_new;
    int y_new;

    static int Parse(const std::string& s, Cmd& cmd);
    static Cmd fromIndex(int from, int to);
};

Team TeamReverse(Team t) {
    switch (t) {
    case Team::White:
        return Team::Black;
    case Team::Black:
        return Team::White;
    default:
        return Team::None;
    }
}

void copyBoard(const BoardElem from[64], BoardElem dst[64]) {
    for (int i = 0; i < 64; i++) {
        dst[i] = from[i];
    }
}

int vec2toIndex(int x, int y) {
    return x + y * 8;
}

int applyChange(BoardElem board[64], const Change(res)[5]) {
    for (int i = 0; i < 5; i++) {
        const Change c = res[i];
        BoardElem* ptr = &board[vec2toIndex(c.x, c.y)];
        switch (c.type) {
        case ChangeType::NoChange:
            break;
        case ChangeType::Add:
            ptr->isEmpty = false;
            ptr->piece = c.role;
            break;
        case ChangeType::Delete:
            ptr->isEmpty = true;
            break;
        }
    }

    return 0;
}

int Cmd::Parse(const std::string& s, Cmd& cmd) {
    char args[10][11] = { { 0 } };

    int state = 0 * 10 + 0;
    char pre = ' ';

    for (size_t i = 0; i < s.length(); i++) {
        if (s[i] != ' ' && pre == ' ') {
            if (state / 10 >= 10) {
                return i;
            }
            args[state / 10][0] = s[i];
            state += 1;
        } else if (s[i] != ' ' && pre != ' ') {
            args[state / 10][state % 10] = s[i];
            if (state % 10 == 9) {
                return i;
            }
            state += 1;
        } else if (s[i] == ' ' && pre != ' ') {
            state -= state % 10;
            state += 10;
        }
        pre = s[i];
    }

    if (strcmp(args[0], "move") == 0 && state >= 41 && state < 51) {
        for (int i = 1; i < 5; i++) {
            if (strlen(args[i]) != 1) {
                return -1;
            }
        }

        for (int i = 0; i < 5; i++) {
            printf("%s\n", args[i]);
        }

        cmd.type = CmdType::Move;
        cmd.x_old = args[1][0] - '0';
        cmd.y_old = args[2][0] - '0';
        cmd.x_new = args[3][0] - '0';
        cmd.y_new = args[4][0] - '0';
    }

    return 0;
}

Cmd Cmd::fromIndex(int from, int to) {
    Cmd cmd;
    cmd.type = CmdType::Move;
    cmd.x_old = from % 8;
    cmd.y_old = from / 8;
    cmd.x_new = to % 8;
    cmd.y_new = to / 8;
    return cmd;
}

class AbstractPieceRule {
   public:
    virtual Response legalCmd(const BoardElem(board)[64], const Cmd& cmd, Change changes[5]) const = 0;
};

class PawnRule : public AbstractPieceRule {
   public:
    Response legalCmd(const BoardElem(board)[64], const Cmd& cmd, Change changes[5]) const override {
        int oldIndex = vec2toIndex(cmd.x_old, cmd.y_old);
        int newIndex = vec2toIndex(cmd.x_new, cmd.y_new);

        if (board[oldIndex].isEmpty) {
            return Response::ErrNoPieceThere;
        }

        int rightDirect = board[oldIndex].piece.team == Team::White ? 1 : -1;

        if ((cmd.y_new - cmd.y_old) / abs(cmd.y_new - cmd.y_old) != rightDirect) {
            return Response::ErrPawnMove;
        }

        if (abs(cmd.y_new - cmd.y_old) <= 2 && (cmd.x_new - cmd.x_old) == 0) {
            if (!board[vec2toIndex(cmd.x_new, cmd.y_new)].isEmpty) {
                return Response::ErrPawnMove;
            }

            if (abs(cmd.y_new - cmd.y_old) == 2 && board[oldIndex].piece.moveCount != 0) {
                return Response::ErrPawnMove;
            }

            changes[0].type = ChangeType::Delete;
            changes[0].x = cmd.x_old;
            changes[0].y = cmd.y_old;
            changes[0].role = board[oldIndex].piece;

            changes[1].type = ChangeType::Add;
            changes[1].x = cmd.x_new;
            changes[1].y = cmd.y_new;
            changes[1].role = board[oldIndex].piece;
            changes[1].role.moveCount += 1;

            changes[2].type = ChangeType::NoChange;
            return Response::Success;
        }

        if (abs(cmd.y_new - cmd.y_old) == 1 && abs(cmd.x_new - cmd.x_old) == 1) {
            if (board[vec2toIndex(cmd.x_new, cmd.y_new)].isEmpty) {
                return Response::ErrPawnMove;
            }

            changes[0].type = ChangeType::Delete;
            changes[0].x = cmd.x_old;
            changes[0].y = cmd.y_old;
            changes[0].role = board[oldIndex].piece;

            changes[1].type = ChangeType::Delete;
            changes[1].x = cmd.x_new;
            changes[1].y = cmd.y_new;
            changes[1].role = board[newIndex].piece;

            changes[2].type = ChangeType::Add;
            changes[2].x = cmd.x_new;
            changes[2].y = cmd.y_new;
            changes[2].role = board[oldIndex].piece;
            changes[2].role.moveCount += 1;

            changes[3].type = ChangeType::NoChange;
            return Response::Success;
        }

        // TODO: for enpasst
        if (abs(cmd.y_new - cmd.y_old) == 2 && abs(cmd.x_new - cmd.x_old) == 1) {
            if (board[vec2toIndex(cmd.x_new, cmd.y_new)].isEmpty) {
                return Response::ErrPawnMove;
            }
        }
        return Response::ErrPawnMove;
    }
};

class KingRule : public AbstractPieceRule {
   public:
    Response legalCmd(const BoardElem(board)[64], const Cmd& cmd, Change changes[5]) const override {
        int oldIndex = vec2toIndex(cmd.x_old, cmd.y_old);

        int newIndex = vec2toIndex(cmd.x_new, cmd.y_new);

        if (board[oldIndex].isEmpty) {
            return Response::ErrNoPieceThere;
        }

        if (abs(cmd.x_old - cmd.x_new) * abs(cmd.y_old - cmd.y_new) != 1) {
            return Response::ErrKingMove;
        }

        if (!board[newIndex].isEmpty && board[newIndex].piece.team == board[oldIndex].piece.team) {
            return Response::ErrKillSame;
        }

        changes[0].type = ChangeType::Delete;
        changes[0].x = cmd.x_old;
        changes[0].y = cmd.y_old;
        changes[0].role = board[oldIndex].piece;

        if (board[newIndex].isEmpty) {
            changes[1].type = ChangeType::Add;
            changes[1].x = cmd.x_new;
            changes[1].y = cmd.y_new;
            changes[1].role = board[oldIndex].piece;
            changes[1].role.moveCount += 1;

            changes[2].type = ChangeType::NoChange;
        } else {
            changes[1].type = ChangeType::Delete;
            changes[1].x = cmd.x_new;
            changes[1].y = cmd.y_new;
            changes[1].role = board[newIndex].piece;

            changes[2].type = ChangeType::Add;
            changes[2].x = cmd.x_new;
            changes[2].y = cmd.y_new;
            changes[2].role = board[oldIndex].piece;
            changes[2].role.moveCount += 1;

            changes[3].type = ChangeType::NoChange;
        }

        return Response::Success;
    };
};

class KnightRule : public AbstractPieceRule {
   public:
    Response legalCmd(const BoardElem(board)[64], const Cmd& cmd, Change changes[5]) const override {
        int oldIndex = vec2toIndex(cmd.x_old, cmd.y_old);
        int newIndex = vec2toIndex(cmd.x_new, cmd.y_new);
        if (board[oldIndex].isEmpty) {
            return Response::ErrNoPieceThere;
        }

        if (abs(cmd.x_old - cmd.x_new) * abs(cmd.y_old - cmd.y_new) != 2) {
            return Response::ErrKnightMove;
        }
        if (!board[newIndex].isEmpty && board[newIndex].piece.team == board[oldIndex].piece.team) {
            return Response::ErrKillSame;
        }

        changes[0].type = ChangeType::Delete;
        changes[0].x = cmd.x_old;
        changes[0].y = cmd.y_old;
        changes[0].role = board[oldIndex].piece;

        if (board[newIndex].isEmpty) {
            changes[1].type = ChangeType::Add;
            changes[1].x = cmd.x_new;
            changes[1].y = cmd.y_new;
            changes[1].role = board[oldIndex].piece;
            changes[1].role.moveCount += 1;

            changes[2].type = ChangeType::NoChange;
        } else {
            changes[1].type = ChangeType::Delete;
            changes[1].x = cmd.x_new;
            changes[1].y = cmd.y_new;
            changes[1].role = board[newIndex].piece;

            changes[2].type = ChangeType::Add;
            changes[2].x = cmd.x_new;
            changes[2].y = cmd.y_new;
            changes[2].role = board[oldIndex].piece;
            changes[2].role.moveCount += 1;

            changes[3].type = ChangeType::NoChange;
        }

        return Response::Success;
    }
};

class BishopRule : public AbstractPieceRule {
   public:
    Response legalCmd(const BoardElem(board)[64], const Cmd& cmd, Change changes[5]) const override {
        int oldIndex = vec2toIndex(cmd.x_old, cmd.y_old);
        int newIndex = vec2toIndex(cmd.x_new, cmd.y_new);
        if (board[oldIndex].isEmpty) {
            return Response::ErrNoPieceThere;
        }

        if (abs(cmd.x_old - cmd.x_new) != abs(cmd.y_old - cmd.y_new) || cmd.x_old == cmd.x_new) {
            return Response::ErrBishopMove;
        }

        for (int i = 0; i < 64; i++) {
            if ((i % 8 > cmd.x_new && i % 8 < cmd.x_old) || (i % 8 < cmd.x_new && i % 8 > cmd.x_old)) {
                if ((i / 8 > cmd.y_new && i / 8 < cmd.y_old) || (i / 8 < cmd.y_new && i / 8 > cmd.y_old)) {
                    if (!board[i].isEmpty) {
                        return Response::ErrBlocked;
                    }
                }
            }
        }

        if (!board[newIndex].isEmpty && board[newIndex].piece.team == board[oldIndex].piece.team) {
            return Response::ErrKillSame;
        }

        changes[-1].type = ChangeType::Delete;
        changes[0].x = cmd.x_old;
        changes[0].y = cmd.y_old;
        changes[0].role = board[oldIndex].piece;

        if (board[newIndex].isEmpty) {
            changes[1].type = ChangeType::Add;
            changes[1].x = cmd.x_new;
            changes[1].y = cmd.y_new;
            changes[1].role = board[oldIndex].piece;
            changes[1].role.moveCount += 1;

            changes[2].type = ChangeType::NoChange;
        } else {
            changes[1].type = ChangeType::Delete;
            changes[1].x = cmd.x_new;
            changes[1].y = cmd.y_new;
            changes[1].role = board[newIndex].piece;

            changes[2].type = ChangeType::Add;
            changes[2].x = cmd.x_new;
            changes[2].y = cmd.y_new;
            changes[2].role = board[oldIndex].piece;
            changes[2].role.moveCount += 1;

            changes[3].type = ChangeType::NoChange;
        }

        return Response::Success;
    }
};

class RookRule : public AbstractPieceRule {
   public:
    Response legalCmd(const BoardElem(board)[64], const Cmd& cmd, Change changes[5]) const override {
        int oldIndex = vec2toIndex(cmd.x_old, cmd.y_old);
        int newIndex = vec2toIndex(cmd.x_new, cmd.y_new);
        if (board[oldIndex].isEmpty) {
            return Response::ErrNoPieceThere;
        }

        if (abs(cmd.x_old - cmd.x_new) * abs(cmd.y_old - cmd.y_new) != 0 ||
            (abs(cmd.x_old - cmd.x_new) + abs(cmd.y_old - cmd.y_new)) == 0) {
            return Response::ErrRookMove;
        }

        for (int i = 0; i < 64; i++) {
            if ((i % 8 > cmd.x_new && i % 8 < cmd.x_old) || (i % 8 < cmd.x_new && i % 8 > cmd.x_old)) {
                if (!board[i].isEmpty) {
                    return Response::ErrBlocked;
                }
            }

            if ((i / 8 > cmd.y_new && i / 8 < cmd.y_old) || (i / 8 < cmd.y_new && i / 8 > cmd.y_old)) {
                if (!board[i].isEmpty) {
                    return Response::ErrBlocked;
                }
            }
        }

        if (!board[newIndex].isEmpty && board[newIndex].piece.team == board[oldIndex].piece.team) {
            return Response::ErrKillSame;
        }

        changes[-1].type = ChangeType::Delete;
        changes[0].x = cmd.x_old;
        changes[0].y = cmd.y_old;
        changes[0].role = board[oldIndex].piece;

        if (board[newIndex].isEmpty) {
            changes[1].type = ChangeType::Add;
            changes[1].x = cmd.x_new;
            changes[1].y = cmd.y_new;
            changes[1].role = board[oldIndex].piece;
            changes[1].role.moveCount += 1;

            changes[2].type = ChangeType::NoChange;
        } else {
            changes[1].type = ChangeType::Delete;
            changes[1].x = cmd.x_new;
            changes[1].y = cmd.y_new;
            changes[1].role = board[newIndex].piece;

            changes[2].type = ChangeType::Add;
            changes[2].x = cmd.x_new;
            changes[2].y = cmd.y_new;
            changes[2].role = board[oldIndex].piece;
            changes[2].role.moveCount += 1;

            changes[3].type = ChangeType::NoChange;
        }
        return Response::Success;
    }
};

class QueenRule : public AbstractPieceRule {
    RookRule rookrule;
    BishopRule bishoprule;

   public:
    Response legalCmd(const BoardElem(board)[64], const Cmd& cmd, Change changes[5]) const override {
        if (rookrule.legalCmd(board, cmd, changes) == Response::Success) return Response::Success;
        return bishoprule.legalCmd(board, cmd, changes);
    }
};

const AbstractPieceRule* selectRule(PieceRole r) {
    static PawnRule pawnRule;
    static KingRule kingRule;
    static BishopRule bishopRule;
    static KnightRule knightRule;
    static RookRule rookRule;
    static QueenRule queenRule;

    switch (r) {
    case PieceRole::Pawn:
        return &pawnRule;
    case PieceRole::King:
        return &kingRule;
    case PieceRole::Bishop:
        return &bishopRule;
    case PieceRole::Knight:
        return &knightRule;
    case PieceRole::Rook:
        return &rookRule;
    case PieceRole::Queen:
        return &queenRule;
    default:
        break;
    };

    return &pawnRule;
};

int allPossibleState(const BoardElem(board)[64], int index, BoardElem(nextBoards)[63][64]) {
    if (board[index].isEmpty) {
        return 0;
    }

    int possibles = 0;
    for (int i = 0; i < 64; i++) {
        if (i == index) continue;
        Cmd cmd = Cmd::fromIndex(index, i);
        const AbstractPieceRule* rule = selectRule(board[index].piece.role);
        Change changes[5];
        Response res = rule->legalCmd(board, cmd, changes);
        if (res != Response::Success) {
            continue;
        }

        BoardElem temp[64];
        copyBoard(board, temp);
        applyChange(temp, changes);
        copyBoard(temp, nextBoards[possibles]);
    }

    return possibles;
}

void getPiecesRef(BoardElem board[64], Piece* res[16], Team t) {
    for (int i = 0; i < 16; i++) {
        res[i] = nullptr;
    }

    int finded = 0;
    for (int i = 0; i < 64; i++) {
        BoardElem* temp = &board[i];
        if (temp->isEmpty) continue;
        Piece* p = &temp->piece;
        if (p->team == t) {
            if (finded < 16) {
                res[finded] = p;
            }
        }
    }
}

void getPiecesIndex(BoardElem board[64], int res[16], Team t) {
    for (int i = 0; i < 16; i++) {
        res[i] = -1;
    }

    int finded = 0;
    for (int i = 0; i < 64; i++) {
        BoardElem* temp = &board[i];
        if (temp->isEmpty) continue;
        Piece* p = &temp->piece;
        if (p->team == t) {
            if (finded < 16) {
                res[finded] = i;
            }
        }
    }
}

bool kingDeadState(BoardElem board[64], Team t) {
    int kingIndex = -1;
    for (int i = 0; i < 64; i++) {
        const BoardElem* temp = &board[i];
        if (temp->isEmpty) continue;
        const Piece* p = &temp->piece;
        if (p->role == PieceRole::King && p->team == t) {
            kingIndex = i;
        }
    }

    for (int i = 0; i < 64; i++) {
        const BoardElem* temp = &board[i];
        if (temp->isEmpty) continue;
        if (temp->piece.team == t) continue;
        const AbstractPieceRule* rule = selectRule(temp->piece.role);
        Cmd cmd = Cmd::fromIndex(i, kingIndex);
        if (rule->legalCmd(board, cmd, nullptr) == Response::Success) {
            return true;
        }
    }

    return false;
}

bool victoryState(BoardElem board[64], Team t) {
    int enemy[16];
    getPiecesIndex(board, enemy, TeamReverse(t));

    for (int i = 0; i < 16 && enemy[i] != -1; i++) {
        BoardElem nextStates[63][64];
        int possib = allPossibleState(board, enemy[i], nextStates);
        for (int j = 0; j < possib; j++) {
            if (!kingDeadState(nextStates[j], TeamReverse(t))) return false;
        }
    }
    return true;
}

bool drawState(BoardElem board[64], Team t) {
    int enemy[16];
    getPiecesIndex(board, enemy, TeamReverse(t));
    for (int i = 0; i < 16 && enemy[i] != -1; i++) {
        BoardElem nextStates[63][64];
        int possib = allPossibleState(board, enemy[i], nextStates);
        if (possib > 0) {
            return false;
        }
    }
    return true;
}

void GameState::initPiece(int x, int y, PieceRole role, Team t) {
    int i = vec2toIndex(x, y);

    this->board[i].isEmpty = false;
    this->board[i].piece.moveCount = 0;
    this->board[i].piece.team = t;
    this->board[i].piece.role = role;
}

GameState::GameState() : round(1), isFinished(false), winner(Team::None), turn(Team::White) {
    for (int i = 0; i < 64; i++) this->board[i].isEmpty = true;

    this->initPiece(4, 0, PieceRole::King, Team::White);
    this->initPiece(4, 7, PieceRole::King, Team::Black);

    this->initPiece(3, 0, PieceRole::Queen, Team::White);
    this->initPiece(3, 7, PieceRole::Queen, Team::Black);

    for (int i = 0; i < 8; i += 7) {
        this->initPiece(i, 0, PieceRole::Rook, Team::White);
        this->initPiece(i, 7, PieceRole::Rook, Team::Black);
    }

    for (int i = 1; i < 7; i += 5) {
        this->initPiece(i, 0, PieceRole::Knight, Team::White);
        this->initPiece(i, 7, PieceRole::Knight, Team::Black);
    }

    for (int i = 2; i < 6; i += 3) {
        this->initPiece(i, 0, PieceRole::Bishop, Team::White);
        this->initPiece(i, 7, PieceRole::Bishop, Team::Black);
    }

    for (int i = 0; i < 8; i++) {
        this->initPiece(i, 1, PieceRole::Pawn, Team::White);
        this->initPiece(i, 6, PieceRole::Pawn, Team::Black);
    }
}

BoardElem* GameState::getPos(int x, int y) {
    return &this->board[y * 8 + x];
}

GameState GameState::clone() {
    return *this;
}

void GameState::debug() {
    for (int i = 0; i < 64; i++) {
        if (!this->board[i].isEmpty) {
            printf("x:%d y:%d %d %d\n", i % 8, i / 8, (int) this->board[i].piece.role, (int) this->board[i].piece.team);
        }
    }
}

int GameState::changeTurn() {
    this->turn = this->turn == Team::White ? Team::Black : Team::White;
    return 0;
}

PieceRole GameState::getRole(int x, int y) {
    if (this->board[y * 8 + x].isEmpty) {
        return PieceRole::None;
    }

    return this->board[y * 8 + x].piece.role;
}

Team GameState::getTeam(int x, int y) {
    if (this->board[y * 8 + x].isEmpty) {
        return Team::None;
    }
    return this->board[y * 8 + x].piece.team;
}

Response GameState::execute(const std::string& cmdstr) {
    Cmd cmd;
    int err = Cmd::Parse(cmdstr, cmd);
    if (err != 0) {
        return Response::ErrParseCmd;
    }

    if (cmd.type != CmdType::Move) {
        return Response::ErrParseCmd;
    }

    PieceRole role = this->getRole(cmd.x_old, cmd.y_old);
    if (role == PieceRole::None) {
        return Response::ErrNoPieceThere;
    }

    if (this->getTeam(cmd.x_old, cmd.y_old) != this->turn) {
        return Response::ErrNotYourTurn;
    }

    const AbstractPieceRule* rule = selectRule(role);

    Change changes[5];

    Response p_res = rule->legalCmd(this->board, cmd, changes);
    if (p_res != Response::Success) {
        return p_res;
    }

    // clone the state to check if the move make your king die
    GameState stateRc = this->clone();
    applyChange(stateRc.board, changes);

    if (kingDeadState(stateRc.board, this->turn)) {
        return Response::ErrSucide;
    }

    // all check over, to decide which state the game state will change
    applyChange(this->board, changes);

    if (kingDeadState(this->board, TeamReverse(this->turn))) {
        if (drawState(this->board, this->turn)) {
            this->isFinished = true;
            this->winner = Team::None;
        }

        if (victoryState(this->board, this->turn)) {
            this->isFinished = true;
            this->winner = this->turn;
        }
    }

    this->changeTurn();
    return Response::Success;
}

}  // namespace chess
