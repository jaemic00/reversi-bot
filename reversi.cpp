#include <iostream>
#include <vector>
#include <climits>

#define SIZE 8

enum Player { EMPTY = 0, BLACK = 1, WHITE = 2 };

struct Move {
    int x, y;
};

Player opponent(Player player) {
    return (player == BLACK) ? WHITE : BLACK;
}

class Board {
public:
    Player board[SIZE][SIZE];

    Board() {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = EMPTY;
            }
        }
        board[3][3] = board[4][4] = BLACK;
        board[3][4] = board[4][3] = WHITE;
    }

    bool validMove(Player player, Move move) {
        // Check if the move is within the board and the destination is empty
        if (move.x < 0 || move.x >= SIZE || move.y < 0 || move.y >= SIZE || board[move.x][move.y] != EMPTY) {
            return false;
        }

        // Check all directions
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue;
                if (canFlip(player, move, dx, dy)) {
                    return true;
                }
            }
        }
        return false;
    }

    void makeMove(Player player, Move move) {
        if (!validMove(player, move)) {
            return;
        }
        board[move.x][move.y] = player;
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue;
                if (canFlip(player, move, dx, dy)) {
                    flip(player, move, dx, dy);
                }
            }
        }
    }

    bool hasValidMove(Player player) {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (validMove(player, {i, j})) {
                    return true;
                }
            }
        }
        return false;
    }

private:
    bool canFlip(Player player, Move move, int dx, int dy) {
        int x = move.x + dx;
        int y = move.y + dy;
        if (x < 0 || x >= SIZE || y < 0 || y >= SIZE || board[x][y] != opponent(player)) {
            return false;
        }
        while (true) {
            x += dx;
            y += dy;
            if (x < 0 || x >= SIZE || y < 0 || y >= SIZE || board[x][y] == EMPTY) {
                return false;
            }
            if (board[x][y] == player) {
                return true;
            }
        }
    }

    void flip(Player player, Move move, int dx, int dy) {
        int x = move.x + dx;
        int y = move.y + dy;
        while (board[x][y] == opponent(player)) {
            board[x][y] = player;
            x += dx;
            y += dy;
        }
    }
};

class Bot {
public:
    Player player;
    int maxDepth;

    Bot(Player player, int maxDepth) : player(player), maxDepth(maxDepth) {}

    Move getMove(Board board) {
        int bestScore = INT_MIN;
        Move bestMove = {-1, -1};
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Move move = {i, j};
                if (board.validMove(player, move)) {
                    Board newBoard = board;
                    newBoard.makeMove(player, move);
                    int score = minimax(newBoard, maxDepth, INT_MIN, INT_MAX, false);
                    if (score > bestScore) {
                        bestScore = score;
                        bestMove = move;
                    }
                }
            }
        }
        return bestMove;
    }

private:
    int minimax(Board board, int depth, int alpha, int beta, bool maximizingPlayer) {
        if (depth == 0 || !board.hasValidMove(player)) {
            return evaluate(board);
        }
        if (maximizingPlayer) {
            int maxEval = INT_MIN;
            for (int i = 0; i < SIZE; i++) {
                for (int j = 0; j < SIZE; j++) {
                    Move move = {i, j};
                    if (board.validMove(player, move)) {
                        Board newBoard = board;
                        newBoard.makeMove(player, move);
                        int eval = minimax(newBoard, depth - 1, alpha, beta, false);
                        maxEval = std::max(maxEval, eval);
                        alpha = std::max(alpha, eval);
                        if (beta <= alpha) {
                            break;
                        }
                    }
                }
            }
            return maxEval;
        } else {
            int minEval = INT_MAX;
            for (int i = 0; i < SIZE; i++) {
                for (int j = 0; j < SIZE; j++) {
                    Move move = {i, j};
                    if (board.validMove(opponent(player), move)) {
                        Board newBoard = board;
                        newBoard.makeMove(opponent(player), move);
                        int eval = minimax(newBoard, depth - 1, alpha, beta, true);
                        minEval = std::min(minEval, eval);
                        beta = std::min(beta, eval);
                        if (beta <= alpha) {
                            break;
                        }
                    }
                }
            }
            return minEval;
        }
    }

        int evaluate(Board board) {
            int score = 0;
            int opponent = 3 - player;
            int counter = 0;

            // Heuristic 0: Counting fields
            // for (int i = 0; i < 8; i++) {
            //     for (int j = 0; j < 8; j++) {
            //         if (board.board[i][j] == player) {
            //             score++;
            //         } else if (board.board[i][j] == opponent) {
            //             score--;
            //         }
            //     }
            // }

            // Heuristic 1: Corners
            int cornerValue = 10;
            if (board.board[0][0] == player) score += cornerValue;
            if (board.board[0][7] == player) score += cornerValue;
            if (board.board[7][0] == player) score += cornerValue;
            if (board.board[7][7] == player) score += cornerValue;

            int opponentCornerValue = -10;
            if (board.board[0][0] == opponent) score += opponentCornerValue;
            if (board.board[0][7] == opponent) score += opponentCornerValue;
            if (board.board[7][0] == opponent) score += opponentCornerValue;
            if (board.board[7][7] == opponent) score += opponentCornerValue;

            // Heuristic 2: Fields next to corners
            int badPositionValue = -10;
            if (board.board[0][0] != player && (board.board[0][1] == player || board.board[1][0] == player || board.board[1][1] == player)) score += badPositionValue;
            if (board.board[0][7] != player && (board.board[0][6] == player || board.board[1][7] == player || board.board[1][6] == player)) score += badPositionValue;
            if (board.board[7][0] != player && (board.board[7][1] == player || board.board[6][0] == player || board.board[6][1] == player)) score += badPositionValue;
            if (board.board[7][7] != player && (board.board[7][6] == player || board.board[6][7] == player || board.board[6][6] == player)) score += badPositionValue;

            // Heuristic 3: Counting fields next to our fields
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    // Heuristic 0: Counting fields
                    if (board.board[i][j] == player) {
                        score++;
                    } else if (board.board[i][j] == opponent) {
                        score--;
                    }
                    if (board.board[i][j] == 0) {
                        for (int dx = -1; dx <= 1; dx++) {
                            for (int dy = -1; dy <= 1; dy++) {
                                if (i + dx >= 0 && i + dx < 8 && j + dy >= 0 && j + dy < 8) {
                                    if (board.board[i + dx][j + dy] == player) {
                                        counter--;
                                    } else if (board.board[i + dx][j + dy] == opponent) {
                                        counter++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            score += counter;

            return score;
        }
    };

int main() {
    Board board;
    Bot bot(WHITE, 2);
    std::cout << "RDY" << std::endl;
    while (true) {
        std::string command;
        std::cin >> command;
        if (command == "UGO") {
            board = Board();
            bot = Bot(WHITE, 2);
            double moveTime, totalTime;
            std::cin >> moveTime >> totalTime;
            Move move = bot.getMove(board);
            std::cout << "IDO " << move.x << " " << move.y << std::endl;
            board.makeMove(bot.player, move);
        } else if (command == "HEDID") {
            double moveTime, totalTime;
            int x, y;
            std::cin >> moveTime >> totalTime >> x >> y;
            board.makeMove(opponent(bot.player), {x, y});
            Move move = bot.getMove(board);
            std::cout << "IDO " << move.x << " " << move.y << std::endl;
            board.makeMove(bot.player, move);
        } else if (command == "ONEMORE") {
            board = Board();
            bot = Bot(BLACK, 2);
            std::cout << "RDY" << std::endl;
        } else if (command == "BYE") {
            exit(0);
            break;
        }
    }
    return 0;
}
