#include <stdio.h>

const int BOARD_DIM = 5;

// Function to check if a move is outside the chessboard
int isMoveOutsideBoard(int posX, int posY) {
    // Using ternary operator, returns true if the move is out of bounds
    return (posX < 0 || posY < 0 || posX >= BOARD_DIM || posY >= BOARD_DIM) ? 1 : 0;
}

// Function to explore the Knight's Tour using backtracking
void knightsTour(int board[BOARD_DIM][BOARD_DIM], int posX, int posY, int stepCount) {
    
    // Present location on the board
    board[posX][posY] = stepCount;
    

    // 8 possible moves for the knight in each direction
    int yMoves[] = {1, 2, 2, 1, -1, -2, -2, -1};
    int xMoves[] = {2, 1, -1, -2, -2, -1, 1, 2};
    

    // Check if the tour is complete
    if(stepCount >= BOARD_DIM * BOARD_DIM) {
        // Prints the path the knight took
        for (int step = 0; step < BOARD_DIM * BOARD_DIM; step++) {
            for (int x = 0; x < BOARD_DIM; x++) {
                for (int y = 0; y < BOARD_DIM; y++) {
                    if(board[x][y] == step) {
                        printf("%d,", (x * BOARD_DIM) + y + 1);
                    }
                }
            }
        }

        // Prints the final position the knight is on and makes a new line for the next tour
        for (int x = 0; x < BOARD_DIM; x++) {
            for (int y = 0; y < BOARD_DIM; y++) {
                if(board[x][y] == BOARD_DIM * BOARD_DIM) {
                    printf("%d\n", (x * BOARD_DIM) + y + 1);
                }
            }
        }

        // Reset to explore remaning tours
        board[posX][posY] = 0;
        return;
    }

    // Try all eight possible moves a knight can make
    for(int moveNum = 0; moveNum < 8; moveNum++) {
        int newX = posX + xMoves[moveNum];
        int newY = posY + yMoves[moveNum];

        // Check if the move is valid and has not been visted already
        if (!isMoveOutsideBoard(newX, newY) && board[newX][newY] == 0) {
            knightsTour(board, newX, newY, stepCount + 1);
        }
    }

    // Reset the position for backtracking
    board[posX][posY] = 0;
}

int main() {
    int board[BOARD_DIM][BOARD_DIM];
    int x, y;

    // Initialize the chess board
    for(x = 0; x < BOARD_DIM; x++) {
        for(y = 0; y < BOARD_DIM; y++) {
            board[x][y] = 0;
        }
    }

    // Start from each position on the board
    for(x = 0; x < BOARD_DIM; x++) {
        for(y = 0; y < BOARD_DIM; y++) {
            knightsTour(board, x, y, 1);
        }
    }

    return 0;
}