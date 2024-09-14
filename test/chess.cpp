#include "chess.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

namespace chess {

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
    virtual bool legalCmd(const BoardElem(board)[64], const Cmd& cmd, Change changes[5]) const = 0;
};

class PawnRule : public AbstractPieceRule {
   public:
    bool legalCmd(const BoardElem(board)[64], const Cmd& cmd, Change changes[5]) const override {
        int oldIndex = vec2toIndex(cmd.x_old, cmd.y_old);
        int newIndex = vec2toIndex(cmd.x_new, cmd.y_new);

        if (board[oldIndex].isEmpty) {
            return false;
        }

        int rightDirect = board[oldIndex].piece.team == Team::White ? 1 : -1;

        if ((cmd.y_new - cmd.y_old) / abs(cmd.y_new - cmd.y_old) != rightDirect) {
            return false;
        }

        if (abs(cmd.y_new - cmd.y_old) == 1 && (cmd.x_new - cmd.x_old) == 0) {
            if (!board[vec2toIndex(cmd.x_new, cmd.y_new)].isEmpty) {
                return false;
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
        }

        if (abs(cmd.y_new - cmd.y_old) == 1 && abs(cmd.x_new - cmd.x_old) == 1) {
            if (board[vec2toIndex(cmd.x_new, cmd.y_new)].isEmpty) {
                return false;
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
        }

        // TODO: for enpasst
        if (abs(cmd.y_new - cmd.y_old) == 2 && abs(cmd.x_new - cmd.x_old) == 1) {
            if (board[vec2toIndex(cmd.x_new, cmd.y_new)].isEmpty) {
                return false;
            }
        }
        return false;
    }
};

class KingRule : public AbstractPieceRule {
   public:
    bool legalCmd(const BoardElem(board)[64], const Cmd& cmd, Change changes[5]) const override {
        int oldIndex = vec2toIndex(cmd.x_old, cmd.y_old);

        int newIndex = vec2toIndex(cmd.x_new, cmd.y_new);

        if (board[oldIndex].isEmpty) {
            return false;
        }

        if (abs(cmd.x_old - cmd.x_new) * abs(cmd.y_old - cmd.y_new) != 1) {
            return false;
        }

        if (!board[newIndex].isEmpty && board[newIndex].piece.team == board[oldIndex].piece.team) {
            return false;
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

        return true;
    };
};

class KnightRule : public AbstractPieceRule {
   public:
    bool legalCmd(const BoardElem(board)[64], const Cmd& cmd, Change changes[5]) const override {
        int oldIndex = vec2toIndex(cmd.x_old, cmd.y_old);
        int newIndex = vec2toIndex(cmd.x_new, cmd.y_new);
        if (board[oldIndex].isEmpty) {
            return false;
        }

        if (abs(cmd.x_old - cmd.x_new) * abs(cmd.y_old - cmd.y_new) != 2) {
            return false;
        }
        if (!board[newIndex].isEmpty && board[newIndex].piece.team == board[oldIndex].piece.team) {
            return false;
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

        return true;
    }
};

class BishopRule : public AbstractPieceRule {
   public:
    bool legalCmd(const BoardElem(board)[64], const Cmd& cmd, Change changes[5]) const override {
        int oldIndex = vec2toIndex(cmd.x_old, cmd.y_old);
        int newIndex = vec2toIndex(cmd.x_new, cmd.y_new);
        if (board[oldIndex].isEmpty) {
            return false;
        }

        if (abs(cmd.x_old - cmd.x_new) != abs(cmd.y_old - cmd.y_new) || cmd.x_old == cmd.x_new) {
            return false;
        }

        for (int i = 0; i < 64; i++) {
            if ((i % 8 > cmd.x_new && i % 8 < cmd.x_old) || (i % 8 < cmd.x_new && i % 8 > cmd.x_old)) {
                if ((i / 8 > cmd.y_new && i / 8 < cmd.y_old) || (i / 8 < cmd.y_new && i / 8 > cmd.y_old)) {
                    if (!board[i].isEmpty) {
                        return false;
                    }
                }
            }
        }

        if (!board[newIndex].isEmpty && board[newIndex].piece.team == board[oldIndex].piece.team) {
            return false;
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

        return true;
    }
};

class RookRule : public AbstractPieceRule {
   public:
    bool legalCmd(const BoardElem(board)[64], const Cmd& cmd, Change changes[5]) const override {
        int oldIndex = vec2toIndex(cmd.x_old, cmd.y_old);
        int newIndex = vec2toIndex(cmd.x_new, cmd.y_new);
        if (board[oldIndex].isEmpty) {
            return false;
        }

        if (abs(cmd.x_old - cmd.x_new) * abs(cmd.y_old - cmd.y_new) != 0 ||
            abs(cmd.x_old - cmd.x_new) + abs(cmd.y_old - cmd.y_new) == 0) {
            return false;
        }

        for (int i = 0; i < 64; i++) {
            if ((i % 8 > cmd.x_new && i % 8 < cmd.x_old) || (i % 8 < cmd.x_new && i % 8 > cmd.x_old)) {
                if (!board[i].isEmpty) {
                    return false;
                }
            }

            if ((i / 8 > cmd.y_new && i / 8 < cmd.y_old) || (i / 8 < cmd.y_new && i / 8 > cmd.y_old)) {
                if (!board[i].isEmpty) {
                    return false;
                }
            }
        }

        if (!board[newIndex].isEmpty && board[newIndex].piece.team == board[oldIndex].piece.team) {
            return false;
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
        return true;
    }
};

class QueenRule : public AbstractPieceRule {
   public:
    bool legalCmd(const BoardElem(board)[64], const Cmd& cmd, Change changes[5]) const override {
        RookRule rookrule;
        if (rookrule.legalCmd(board, cmd, changes)) return true;

        BishopRule bishoprule;
        if (bishoprule.legalCmd(board, cmd, changes)) return true;

        return false;
    }
};

const AbstractPieceRule* selectRule(PieceRole r) {
    static PawnRule pawnRule;
    static KingRule KingRule;

    switch (r) {
    case PieceRole::Pawn:
        return &pawnRule;
    case PieceRole::King:
        return &KingRule;
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
        bool legal = rule->legalCmd(board, cmd, changes);
        if (!legal) {
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
        if (rule->legalCmd(board, cmd, nullptr)) {
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

GameState::GameState() : round(1), isFinished(false), winner(Team::None), turn(Team::White) {
    for (int i = 0; i < 64; i++) this->board[i].isEmpty = true;

    for (int i = 0; i < 8; i++) {
        this->board[i + 8].isEmpty = false;
        this->board[i + 8].piece.moveCount = 0;
        this->board[i + 8].piece.team = Team::White;
        this->board[i + 8].piece.role = PieceRole::Pawn;
    }

    for (int i = 0; i < 8; i++) {
        this->board[i + 48].isEmpty = false;
        this->board[i + 48].piece.moveCount = 0;
        this->board[i + 48].piece.team = Team::Black;
        this->board[i + 48].piece.role = PieceRole::Pawn;
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

    const AbstractPieceRule* rule = selectRule(role);

    Change changes[5];
    if (!rule->legalCmd(this->board, cmd, changes)) {
        return Response::ErrPieceRule;
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
