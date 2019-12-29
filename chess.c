// chess.c by Ilya Sherstyuk

#include <ctype.h>
#include <ncurses.h>
#include <curses.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <unistd.h>

#include "piece_tables.h"
#include "gui.h"
#include "position.h"
#include "piece_rules.h"

extern const char pieces_ascii[7][7];
extern const char pieces_index[7];

// color pair names

#define WHITEPAIR 1
#define BLACKPAIR 2
#define BLUEPAIR 3
#define BOARDPAIR 4

void playMove(position *position_ptr, int moveFrom[], int moveTo[]);
void deselectPiece(void);
bool checkLegalMove(position *position_ptr, int moveFrom[], int moveTo[]);

// void listPawnMoves(position *position_ptr, piece *piece_ptr);
// void listKnightMoves(position *position_ptr, piece *piece_ptr);
// void listKingMoves(position *position_ptr, piece *piece_ptr);
// void listRookMoves(position *position_ptr, piece *piece_ptr);
// void listBishopMoves(position *position_ptr, piece *piece_ptr);

// void append(piece *piece_ptr, int file, int rank);
// char getColor(char testPiece);
void listAllMoves(position *position_ptr);
int evaluate(position *position_ptr);
ply findBestMove(position *position_ptr, int depth, int alpha, int beta);
int charToUnicode(char piece);

int difficulty;
int highlightedDifficulty;

// the actual board state
position realPosition;

// full computer evaluation
int computer_eval;

int highlightedPiece[2];
int selectedPiece[2];
int inputPiece[2];
// [0]: file (a-h)
// [1]: rank (1-8)

// for ncurses
int top;
int left;

// finally
int main(void) {
    char initialBoard[8][8] = {{'R', 'P', ' ', ' ', ' ', ' ', 'p', 'r'} ,
                              {'N', 'P', ' ', ' ', ' ', ' ', 'p', 'n'} ,
                              {'B', 'P', ' ', ' ', ' ', ' ', 'p', 'b'} ,
                              {'Q', 'P', ' ', ' ', ' ', ' ', 'p', 'q'} ,
                              {'K', 'P', ' ', ' ', ' ', ' ', 'p', 'k'} ,
                              {'B', 'P', ' ', ' ', ' ', ' ', 'p', 'b'} ,
                              {'N', 'P', ' ', ' ', ' ', ' ', 'p', 'n'} ,
                              {'R', 'P', ' ', ' ', ' ', ' ', 'p', 'r'}};



    // initialize real Position
    // inputs the initial board into the real Position

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            realPosition.board[i][j] = initialBoard[i][j];
        }
    }
    realPosition.turn = 'w';
    realPosition.enPassant[0] = -1;
    realPosition.enPassant[1] = -1;

    // set up castling rights
    for (int i = 0; i < 4; i++) {
        realPosition.castlingRights[i] = 1;
    }

    // initialize ncurses
    // if (initscr() == NULL)
    // {
    //     return false;
    // }
    // if (noecho() == ERR)
    // {
    //     endwin();
    //     return false;
    // }
    // if (raw() == ERR)
    // {
    //     endwin();
    //     return false;
    // }
    // if (keypad(stdscr, true) == ERR)
    // {
    //     endwin();
    //     return false;
    // }
    top = 3;
    left = 3;

    // drawboard variables
    computer_eval = 0;
    difficulty = 4;
    highlightedDifficulty = 4;
    deselectPiece();

    // initialize color pairs
    // start_color();
    // init_color(COLOR_BLACK, 1000, 0, 0);
    // init_pair(WHITEPAIR, COLOR_WHITE, COLOR_WHITE);
    // init_pair(BLACKPAIR, COLOR_WHITE, COLOR_BLACK);
    // init_pair(BLUEPAIR, COLOR_WHITE, COLOR_CYAN);
    // init_pair(BOARDPAIR, COLOR_WHITE, COLOR_YELLOW);


    highlightedPiece[0] = 0;
    highlightedPiece[1] = 0;

    drawBoard(realPosition.board);

    // main loop
    while (true) {

        // create pointer for realPosition

        position * realPosition_ptr = &realPosition;


        // computer makes black move
        if (realPosition.turn == 'b') {
            ply thisPly = findBestMove(realPosition_ptr, difficulty, -999999, 999999);

            computer_eval = thisPly.evaluation;

            playMove(realPosition_ptr, thisPly.move[0], thisPly.move[1]);

        }
        else {
            // get player move
            int ch;
            while (realPosition.turn == 'w') {
                // get user's input
                refresh();
                ch = getch();

                // capitalize input to simplify cases
                ch = toupper(ch);

                switch (ch) {

                    // quit game
                    case 'Q':
                        endwin();
                        return 0;
                        break;

                    // skip your turn (for testing purposes)
                    case 'N':
                        realPosition.turn = 'b';
                        break;

                    // user moves the cursor with arrows
                    case KEY_UP:
                        highlightedPiece[1] += 1;
                        clearBoard();
                        drawBoard(realPosition.board);
                        break;
                    case KEY_DOWN:
                        highlightedPiece[1] -= 1;
                        clearBoard();
                        drawBoard(realPosition.board);
                        break;
                    case KEY_LEFT:
                        highlightedPiece[0] -= 1;
                        clearBoard();
                        drawBoard(realPosition.board);
                        break;
                    case KEY_RIGHT:
                        highlightedPiece[0] += 1;
                        clearBoard();
                        drawBoard(realPosition.board);
                        break;

                    // enter key, user makes a selection
                    case 10:
                        // if the user is changing the difficulty, not moving pieces
                        if (highlightedPiece[1] == 8) {
                            difficulty = highlightedDifficulty;
                            break;
                        }

                        // if the user is moving pieces
                        inputPiece[0] = highlightedPiece[0];
                        inputPiece[1] = highlightedPiece[1];


                        // if player selected a valid square (square is on the board)
                        // this should always be true, but just in case

                        if (inputPiece[0] > -1 && inputPiece[0] < 8 && inputPiece[1] > -1 && inputPiece[1] < 8) {

                            // if no piece is previously selected

                            if (selectedPiece[0] == -1) {
                                // checks that the square is not empty

                                if (realPosition.board[inputPiece[0]][inputPiece[1]] != ' ') {
                                    selectedPiece[0] = inputPiece[0];
                                    selectedPiece[1] = inputPiece[1];
                                }
                            }
                            else {
                            // a piece is already selected
                                // check if user manually deselected by selecting same piece again
                                if (!((selectedPiece[0] == inputPiece[0]) && (selectedPiece[1] == inputPiece[1]))) {
                                    // user did not deselect

                                    //try to move the piece
                                    if (checkLegalMove(realPosition_ptr, selectedPiece, inputPiece)) {
                                        playMove(realPosition_ptr, selectedPiece, inputPiece);
                                    }
                                }
                                deselectPiece();
                            }
                        }
                        else {
                            deselectPiece();
                        }
                        drawBoard(realPosition.board);
                        break;
                }
            }
        }
        clearBoard();
        drawBoard(realPosition.board);
    }
}

// recursive function to find the best move given a certain search depth
// uses minimax with alpha-beta pruning

ply findBestMove(position *position_ptr, int depth, int alpha, int beta) {
    // thisPly contains the best move in the input position and its evaluation
    // maybe a better name is "bestmove"
    ply thisPly;

    // evaluate board if reached end of search tree

    if (depth == 0) {
        thisPly.evaluation = evaluate(position_ptr);
        return thisPly;
    }

    // make default evaluation low

    thisPly.move[0][0] = -1;
    thisPly.evaluation = -100000000;
    if (position_ptr->turn == 'b') {
        thisPly.evaluation = 100000000;
    }

    bool continueSearch = true;

    listAllMoves(position_ptr);

    // iterate over every possible move in input position
    for (int i = 0; i < (*position_ptr).movesLength; i ++) {
        if (continueSearch) {

            // creates new position in which a new move is played

            position newPosition = (*position_ptr);
            position * newPosition_ptr = &newPosition;
            playMove(newPosition_ptr, (*position_ptr).moves[i][0], (*position_ptr).moves[i][1]);

            // if the king was captured, that was the best choice and do not look further

            if  (abs(evaluate(newPosition_ptr)) > 10000) {
                continueSearch = false;
                thisPly.evaluation = evaluate(newPosition_ptr);
                thisPly.move[0][0] = (*position_ptr).moves[i][0][0];
                thisPly.move[0][1] = (*position_ptr).moves[i][0][1];
                thisPly.move[1][0] = (*position_ptr).moves[i][1][0];
                thisPly.move[1][1] = (*position_ptr).moves[i][1][1];
                (*position_ptr).bestMove = i;
            }
            else {

                // newPly is the best move of the new position
                ply newPly = findBestMove(newPosition_ptr, depth - 1, alpha, beta);

                if (position_ptr->turn == 'w') {
                    if (newPly.evaluation > thisPly.evaluation) {
                        thisPly.evaluation = newPly.evaluation;
                        thisPly.move[0][0] = (*position_ptr).moves[i][0][0];
                        thisPly.move[0][1] = (*position_ptr).moves[i][0][1];
                        thisPly.move[1][0] = (*position_ptr).moves[i][1][0];
                        thisPly.move[1][1] = (*position_ptr).moves[i][1][1];
                        (*position_ptr).bestMove = i;

                        if (thisPly.evaluation > beta) {
                            continueSearch = false;
                        }

                        alpha = thisPly.evaluation;
                    }
                }
                else {
                    if (newPly.evaluation < thisPly.evaluation) {
                        thisPly.evaluation = newPly.evaluation;
                        thisPly.move[0][0] = position_ptr->moves[i][0][0];
                        thisPly.move[0][1] = position_ptr->moves[i][0][1];
                        thisPly.move[1][0] = position_ptr->moves[i][1][0];
                        thisPly.move[1][1] = position_ptr->moves[i][1][1];
                        (*position_ptr).bestMove = i;

                        if (thisPly.evaluation < alpha) {
                            continueSearch = false;
                        }

                        beta = thisPly.evaluation;
                    }
                }
            }
        }
    }

    return thisPly;
}

int evaluate(position *position_ptr) {
    position testPosition = *position_ptr;
    int evaluation = 0;

    // adds the material value of each piece
    // also takes into account where the piece is on the board
    // some pieces are more valuable in the center, while the king is best in the corner
    for (int i = 0; i < 8; i ++) {
        for (int j = 0; j < 8; j ++) {
            char testPiece = testPosition.board[i][j];

            switch (testPiece) {
                case 'P':
                    evaluation += 100 + pawnPositions[7 - j][i];
                    break;
                case 'K':
                    evaluation += 100000 + kingPositionsMid[7 - j][i];
                    break;
                case 'C':
                    evaluation += 100000;
                    break;
                case 'X':
                    evaluation += 100500 + rookPositions[7 - j][i];
                    break;
                case 'R':
                    evaluation += 500 + rookPositions[7 - j][i];
                    break;
                case 'N':
                    evaluation += 300 + knightPositions[7 - j][i];
                    break;
                case 'B':
                    evaluation += 310 + bishopPositions[7 - j][i];
                    break;
                case 'Q':
                    evaluation += 900 + queenPositions[7 - j][i];
                    break;
                case 'p':
                    evaluation -= 100 + pawnPositions[7 - j][i];
                    break;
                case 'k':
                    evaluation -= 100000 + kingPositionsMid[7 - j][i];
                    break;
                case 'c':
                    evaluation -= 100000;
                    break;
                case 'x':
                    evaluation -= 100500 + rookPositions[7 - j][i];
                    break;
                case 'r':
                    evaluation -= 500 + rookPositions[7 - j][i];
                    break;
                case 'n':
                    evaluation -= 300 + knightPositions[7 - j][i];
                    break;
                case 'b':
                    evaluation -= 310 + bishopPositions[7 - j][i];
                    break;
                case 'q':
                    evaluation -= 900 + queenPositions[7 - j][i];
                    break;
                default:
                    break;
            }
        }
    }

    // bonus points if you can still castle
    // discourages throwing away castling rights
    // position boost from castling should overpower this

    if (testPosition.castlingRights[0] == 1) {
        evaluation += 30;
    }
    if (testPosition.castlingRights[2] == 1) {
        evaluation -= 30;
    }

    // prevents you from castling in check and through check

    if (testPosition.castlingRights[0] == -1) {
        evaluation -= 200000;
    }
    if (testPosition.castlingRights[2] == -1) {
        evaluation += 200000;
    }

    // more possible moves = better
    // I excluded this because it makes the program a lot slower
    // although I suspect is still has merit, so I left it commented
    // just in case I want to include it later
    // "2" is just a modifier, could be between 1 and 10 or possibly even more

    // testPosition.turn = 'w';
    // listAllMoves(&testPosition);
    // evaluation += 2 * testPosition.movesLength;
    // testPosition.turn = 'b';
    // listAllMoves(&testPosition);
    // evaluation -= 2 * testPosition.movesLength;

    return evaluation;
}

void listAllMoves(position *position_ptr) {
    position_ptr->movesLength = 0;
    for (int i = 0; i < 8; i ++) {
        for (int j = 0; j < 8; j ++) {
            // if the piece is the correct color
            if (getColor((*position_ptr).board[i][j]) == (*position_ptr).turn ) {
                piece testPiece;
                testPiece.type = position_ptr->board[i][j];
                testPiece.color = position_ptr->turn;
                testPiece.coords[0] = i;
                testPiece.coords[1] = j;
                testPiece.listMovesLength = 0;

                // create pointer to the piece
                piece * piece_ptr = &testPiece;

                switch (testPiece.type) {
                    case 'P':
                    case 'p':
                        listPawnMoves(position_ptr, piece_ptr);
                        break;
                    case 'K':
                    case 'k':
                        listKingMoves(position_ptr, piece_ptr);
                        break;
                    case 'R':
                    case 'r':
                        listRookMoves(position_ptr, piece_ptr);
                        break;
                    case 'N':
                    case 'n':
                        listPawnMoves(position_ptr, piece_ptr);
                        break;
                    case 'B':
                    case 'b':
                        listBishopMoves(position_ptr, piece_ptr);
                        break;
                    case 'Q':
                    case 'q':
                        listBishopMoves(position_ptr, piece_ptr);
                        listRookMoves(position_ptr, piece_ptr);
                        break;
                    default:
                        break;
                }

                // transcribes the moves from the piece struct to the position struct
                for (int k = 0; k < testPiece.listMovesLength; k ++)
                {
                    position_ptr->moves[position_ptr->movesLength][0][0] = i;
                    position_ptr->moves[position_ptr->movesLength][0][1] = j;
                    position_ptr->moves[position_ptr->movesLength][1][0] = testPiece.listMoves[k][0];
                    position_ptr->moves[position_ptr->movesLength][1][1] = testPiece.listMoves[k][1];
                    position_ptr->movesLength ++;
                }
            }
        }
    }
}

void playMove(position *position_ptr, int moveFrom[], int moveTo[])
{
    char pieceType = (*position_ptr).board[moveFrom[0]][moveFrom[1]];

    // move the piece
    position_ptr->board[moveTo[0]][moveTo[1]] = pieceType;

    // clear the spot where the piece was
    position_ptr->board[moveFrom[0]][moveFrom[1]] = ' ';

    // pawn promotion
    // will only promote to queen
    if ((pieceType == 'P' && moveTo[1] == 7) || (pieceType == 'p' && moveTo[1] == 0)) {
        position_ptr->board[moveTo[0]][moveTo[1]] ++;
    }


    // if captured en Passant, remove piece
    if (pieceType == 'P' || pieceType == 'p') {
        // set en passant square
        if (abs(moveTo[1] - moveFrom[1]) == 2) {
            position_ptr->enPassant[0] = moveFrom[0];
            position_ptr->enPassant[1] = (moveFrom[1] + moveTo[1]) / 2;
        }
        // check if en passant capture took place
        else if (moveTo[0] == (*position_ptr).enPassant[0] && (*position_ptr).enPassant[1] == moveTo[1]) {
            // black pawn captured
            if (moveTo[1] == 5) {
                position_ptr->board[moveTo[0]][4] = ' ';
            }
            else {
            // white pawn captured
                position_ptr->board[moveTo[0]][3] = ' ';
            }

            // clear en passant
            position_ptr->enPassant[0] = -1;
        }
        else {
        // clear en passant
            position_ptr->enPassant[0] = -1;
        }
    }
    else {
        position_ptr->enPassant[0] = -1;
    }

    // if white castled on the previous move
    // reset pieces to normal
    if (position_ptr->castlingRights[0] == -1)
    {
        if (position_ptr->board[5][0] == 'X') {
            position_ptr->board[5][0] = 'R';
        }
        if (position_ptr->board[4][0] == 'C') {
            position_ptr->board[4][0] = ' ';
        }
        if (position_ptr->board[3][0] == 'X') {
            position_ptr->board[3][0] = 'R';
        }
        position_ptr->castlingRights[0] = 0;
    }

    // if black castled on the previous move
    // reset pieces to normal
    if ((*position_ptr).castlingRights[2] == -1)
    {
        if (position_ptr->board[5][7] == 'x') {
            position_ptr->board[5][7] = 'r';
        }
        if (position_ptr->board[4][7] == 'c') {
            (*position_ptr).board[4][7] = ' ';
        }
        if (position_ptr->board[3][7] == 'x') {
            position_ptr->board[3][7] = 'r';
        }
        position_ptr->castlingRights[2] = 0;
    }

    if (pieceType == 'K') {
        // forfeir castling rights
        position_ptr->castlingRights[0] = 0;
        position_ptr->castlingRights[1] = 0;

        // if castled kingside
        if (moveTo[0] == 6 && moveFrom[0] == 4) {
            position_ptr->board[7][0] = ' ';
            position_ptr->board[5][0] = 'X';
            position_ptr->board[4][0] = 'C';
            position_ptr->castlingRights[0] = -1;
        }

        // if castled queenside
        if (moveTo[0] == 2 && moveFrom[0] == 4) {
            position_ptr->board[0][0] = ' ';
            position_ptr->board[3][0] = 'X';
            position_ptr->board[4][0] = 'C';
            position_ptr->castlingRights[0] = -1;
        }
    }

    if (pieceType == 'k') {
        // forfeit castling rights
        position_ptr->castlingRights[2] = 0;
        position_ptr->castlingRights[3] = 0;

        // if castled kingside
        if (moveTo[0] == 6 && moveFrom[0] == 4) {
            position_ptr->board[7][7] = ' ';
            position_ptr->board[5][7] = 'x';
            position_ptr->board[4][7] = 'c';
            position_ptr->castlingRights[2] = -1;
        }

        // if castled queenside
        if (moveTo[0] == 2 && moveFrom[0] == 4) {
            position_ptr->board[0][7] = ' ';
            position_ptr->board[3][7] = 'x';
            position_ptr->board[4][7] = 'c';
            position_ptr->castlingRights[2] = -1;
        }
    }

    // forfeit castling rights if you moved the appropriate rook

    if (pieceType == 'R' && moveFrom[0] == 7 && moveFrom[1] == 0) {
        position_ptr->castlingRights[0] = 0;
    }
    if (pieceType == 'R' && moveFrom[0] == 0 && moveFrom[1] == 0) {
        position_ptr->castlingRights[1] = 0;
    }
    if (pieceType == 'r' && moveFrom[0] == 7 && moveFrom[1] == 7) {
        position_ptr->castlingRights[2] = 0;
    }
    if (pieceType == 'r' && moveFrom[0] == 0 && moveFrom[1] == 7) {
        position_ptr->castlingRights[3] = 0;
    }

    // switch whose turn it is

    if (position_ptr->turn == 'w') {
        position_ptr->turn = 'b';
    }
    else {
        position_ptr->turn = 'w';
    }
}

bool checkLegalMove(position *position_ptr, int moveFrom[], int moveTo[]) {
    // create type piece that is the selected piece

    piece testPiece;
    testPiece.type = position_ptr->board[moveFrom[0]][moveFrom[1]];
    testPiece.coords[0] = moveFrom[0];
    testPiece.coords[1] = moveFrom[1];
    testPiece.listMovesLength = 0;

    // create pointer to the piece

    piece * piece_ptr = &testPiece;

    testPiece.color = getColor(testPiece.type);

    if (position_ptr->turn != testPiece.color) {
        return false;
    }

    switch (testPiece.type) {
        case 'P':
        case 'p':
            listPawnMoves(position_ptr, piece_ptr);
            break;
        case 'K':
        case 'k':
            listKingMoves(position_ptr, piece_ptr);
            break;
        case 'R':
        case 'r':
            listRookMoves(position_ptr, piece_ptr);
            break;
        case 'N':
        case 'n':
            listPawnMoves(position_ptr, piece_ptr);
            break;
        case 'B':
        case 'b':
            listBishopMoves(position_ptr, piece_ptr);
            break;
        case 'Q':
        case 'q':
            listBishopMoves(position_ptr, piece_ptr);
            listRookMoves(position_ptr, piece_ptr);
            break;
        default:
            break;
    }

    // look through generated list of moves to see if any of them is the test move

    for (int i = 0; i < testPiece.listMovesLength; i++) {
        if (testPiece.listMoves[i][0] == moveTo[0]) {
            if (testPiece.listMoves[i][1] == moveTo[1]) {
                return true;
            }
        }
    }
    return false;
}

// void listPawnMoves(position *position_ptr, piece *piece_ptr) {
//     int pieceFile = (*piece_ptr).coords[0];
//     int pieceRank = (*piece_ptr).coords[1];
//
//     // black pawns move backwards
//     int colorModifier = -1;
//
//     if (piece_ptr->color == 'w') {
//         colorModifier = 1;
//     }
//
//     if (position_ptr->board[pieceFile][pieceRank + colorModifier] == ' ') {// If square in front is empty
//         // move possible: one forward
//         append(piece_ptr, pieceFile, pieceRank + colorModifier);
//
//         if (((7 - 5 * colorModifier) / 2) == pieceRank) { // if on second rank
//             if (position_ptr->board[pieceFile][pieceRank + 2 * colorModifier] == ' ') {// If square 2 in front is empty
//                 // move possible: two forward
//                 append(piece_ptr, pieceFile, pieceRank + 2 * colorModifier);
//             }
//         }
//     }
//     // if enemy piece is on the diagonals
//
//     // capture diagonally if piece there is of opposite color, and not on edge
//     char testSquare;
//     if (pieceFile != 7) {
//
//         testSquare = position_ptr->board[pieceFile + 1][pieceRank + colorModifier];
//
//         if ((getColor(testSquare) != piece_ptr->color) && testSquare != ' ') {
//             append(piece_ptr, pieceFile + 1, pieceRank + colorModifier);
//         }
//
//         // capture diagonally if en passant
//
//         if (pieceFile + 1 == position_ptr->enPassant[0] && pieceRank + colorModifier == position_ptr->enPassant[1]) {
//             append(piece_ptr, pieceFile + 1, pieceRank + colorModifier);
//         }
//     }
//     if (pieceFile != 0)
//     {
//         testSquare = position_ptr->board[pieceFile - 1][pieceRank + colorModifier];
//
//         if ((getColor(testSquare) != piece_ptr->color) && testSquare != ' ') {
//             append(piece_ptr, pieceFile - 1, pieceRank + colorModifier);
//         }
//
//         // capture diagonally if en passant
//
//         if (pieceFile - 1 == position_ptr->enPassant[0] && pieceRank + colorModifier == position_ptr->enPassant[1]) {
//             append(piece_ptr, pieceFile - 1, pieceRank + colorModifier);
//         }
//     }
// }
//
// void listKnightMoves(position *position_ptr, piece *piece_ptr) {
//     int pieceFile = (*piece_ptr).coords[0];
//     int pieceRank = (*piece_ptr).coords[1];
//
//     int knightMoves[8][2] = {{1, 2},  {2, 1},  {-1, 2},  {-2, 1},
//                             {1, -2}, {2, -1}, {-1, -2}, {-2, -1}};
//
//     for (int i = 0; i < 8; i++) {
//         int newFile = pieceFile + knightMoves[i][0];
//         int newRank = pieceRank + knightMoves[i][1];
//         if (newFile >= 0 && newRank >= 0) {
//             if (newFile < 8 && newRank < 8) {
//                 // can't be the same color piece
//                 if (getColor(position_ptr->board[newFile][newRank]) != piece_ptr->color) {
//                     append(piece_ptr, newFile, newRank);
//                 }
//             }
//         }
//     }
// }
//
// void listKingMoves(position *position_ptr, piece *piece_ptr) {
//     int pieceFile = piece_ptr->coords[0];
//     int pieceRank = piece_ptr->coords[1];
//
//     int kingMoves[8][2] = {{1, 0},  {1, 1},  {1, -1},
//                              {0, 1},  {0, -1},
//                              {-1, 0}, {-1, 1}, {-1, -1}};
//
//     for (int i = 0; i < 8; i++) {
//         int newFile = pieceFile + kingMoves[i][0];
//         int newRank = pieceRank + kingMoves[i][1];
//         if (newFile >= 0 && newRank >= 0) {
//             if (newFile < 8 && newRank < 8) {
//                 // can't be the same color piece
//                 if (getColor(position_ptr->board[newFile][newRank]) != piece_ptr->color) {
//                     append(piece_ptr, newFile, newRank);
//                 }
//             }
//         }
//     }
//
//     // castling
//
//     if (position_ptr->castlingRights[0] == 1 && piece_ptr->color == 'w') {
//         int newFile = pieceFile + 2;
//         int newRank = pieceRank;
//         if (position_ptr->board[newFile][newRank] == ' ' && position_ptr->board[newFile - 1][newRank] == ' ') {
//             append(piece_ptr, newFile, newRank);
//         }
//     }
//     if (position_ptr->castlingRights[1] == 1 && piece_ptr->color == 'w') {
//         int newFile = pieceFile - 2;
//         int newRank = pieceRank;
//         if (position_ptr->board[newFile][newRank] == ' ' && position_ptr->board[newFile + 1][newRank] == ' ' && position_ptr->board[newFile - 1][newRank] == ' ') {
//             append(piece_ptr, newFile, newRank);
//         }
//     }
//     if (position_ptr->castlingRights[2] == 1 && piece_ptr->color == 'b') {
//         int newFile = pieceFile + 2;
//         int newRank = pieceRank;
//         if (position_ptr->board[newFile][newRank] == ' ' && position_ptr->board[newFile - 1][newRank] == ' ') {
//             append(piece_ptr, newFile, newRank);
//         }
//     }
//     if (position_ptr->castlingRights[3] == 1 && piece_ptr->color == 'b') {
//         int newFile = pieceFile - 2;
//         int newRank = pieceRank;
//         if (position_ptr->board[newFile][newRank] == ' ' && position_ptr->board[newFile + 1][newRank] == ' ' && position_ptr->board[newFile - 1][newRank] == ' ') {
//             append(piece_ptr, newFile, newRank);
//         }
//     }
//
// }
//
// void listRookMoves(position *position_ptr, piece *piece_ptr) {
//     int pieceFile = piece_ptr->coords[0];
//     int pieceRank = piece_ptr->coords[1];
//
//     int rookMoves[4][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
//
//     for (int i = 0; i < 4; i++) {
//         int j = 0;
//         int newFile = pieceFile;
//         int newRank = pieceRank;
//         while (j < 7) {
//
//             newFile += rookMoves[i][0];
//             newRank += rookMoves[i][1];
//
//             if (newFile >= 0 && newRank >= 0) {
//                 if (newFile < 8 && newRank < 8) {
//                     // can't be the same color piece
//                     if (getColor(position_ptr->board[newFile][newRank]) != piece_ptr->color) {
//                         append(piece_ptr, newFile, newRank);
//                     }
//                     if (position_ptr->board[newFile][newRank] != ' ') {
//                         j = 10;
//                     }
//                 }
//                 else {
//                     j = 10;
//                 }
//             }
//             else {
//                 j = 10;
//             }
//
//             j++;
//         }
//     }
// }
//
// void listBishopMoves(position *position_ptr, piece *piece_ptr) {
//     int pieceFile = piece_ptr->coords[0];
//     int pieceRank = piece_ptr->coords[1];
//
//     int bishopMoves[4][2] = {{1, 1}, {-1, 1}, {-1, -1}, {1, -1}};
//
//     for (int i = 0; i < 4; i++) {
//         int j = 0;
//         int newFile = pieceFile;
//         int newRank = pieceRank;
//         while (j < 7) {
//
//             newFile += bishopMoves[i][0];
//             newRank += bishopMoves[i][1];
//
//             if (newFile >= 0 && newRank >= 0) {
//                 if (newFile < 8 && newRank < 8) {
//                     // can't be the same color piece
//                     if (getColor(position_ptr->board[newFile][newRank]) != piece_ptr->color) {
//                         append(piece_ptr, newFile, newRank);
//                     }
//                     if ((*position_ptr).board[newFile][newRank] != ' ') {
//                         j = 10;
//                     }
//                 }
//                 else {
//                     j = 10;
//                 }
//             }
//             else {
//                 j = 10;
//             }
//
//             j++;
//         }
//     }
// }

void deselectPiece(void) {
    selectedPiece[0] = -1;
    selectedPiece[1] = -1;
}

// void append(piece *piece_ptr, int file, int rank) {
//     (piece_ptr->listMoves)[piece_ptr->listMovesLength][0] = file;
//     (piece_ptr->listMoves)[piece_ptr->listMovesLength][1] = rank;
//     (piece_ptr->listMovesLength) ++;
// }
