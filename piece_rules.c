#include "piece_rules.h"

void append(piece *piece_ptr, int file, int rank) {
    (piece_ptr->listMoves)[piece_ptr->listMovesLength][0] = file;
    (piece_ptr->listMoves)[piece_ptr->listMovesLength][1] = rank;
    (piece_ptr->listMovesLength) ++;
}

void listPawnMoves(position *position_ptr, piece *piece_ptr) {
    int pieceFile = (*piece_ptr).coords[0];
    int pieceRank = (*piece_ptr).coords[1];

    // black pawns move backwards
    int colorModifier = -1;

    if (piece_ptr->color == 'w') {
        colorModifier = 1;
    }

    if (position_ptr->board[pieceFile][pieceRank + colorModifier] == ' ') {// If square in front is empty
        // move possible: one forward
        append(piece_ptr, pieceFile, pieceRank + colorModifier);

        if (((7 - 5 * colorModifier) / 2) == pieceRank) { // if on second rank
            if (position_ptr->board[pieceFile][pieceRank + 2 * colorModifier] == ' ') {// If square 2 in front is empty
                // move possible: two forward
                append(piece_ptr, pieceFile, pieceRank + 2 * colorModifier);
            }
        }
    }
    // if enemy piece is on the diagonals

    // capture diagonally if piece there is of opposite color, and not on edge
    char testSquare;
    if (pieceFile != 7) {

        testSquare = position_ptr->board[pieceFile + 1][pieceRank + colorModifier];

        if ((getColor(testSquare) != piece_ptr->color) && testSquare != ' ') {
            append(piece_ptr, pieceFile + 1, pieceRank + colorModifier);
        }

        // capture diagonally if en passant

        if (pieceFile + 1 == position_ptr->enPassant[0] && pieceRank + colorModifier == position_ptr->enPassant[1]) {
            append(piece_ptr, pieceFile + 1, pieceRank + colorModifier);
        }
    }
    if (pieceFile != 0)
    {
        testSquare = position_ptr->board[pieceFile - 1][pieceRank + colorModifier];

        if ((getColor(testSquare) != piece_ptr->color) && testSquare != ' ') {
            append(piece_ptr, pieceFile - 1, pieceRank + colorModifier);
        }

        // capture diagonally if en passant

        if (pieceFile - 1 == position_ptr->enPassant[0] && pieceRank + colorModifier == position_ptr->enPassant[1]) {
            append(piece_ptr, pieceFile - 1, pieceRank + colorModifier);
        }
    }
}

void listKnightMoves(position *position_ptr, piece *piece_ptr) {
    int pieceFile = (*piece_ptr).coords[0];
    int pieceRank = (*piece_ptr).coords[1];

    int knightMoves[8][2] = {{1, 2},  {2, 1},  {-1, 2},  {-2, 1},
                            {1, -2}, {2, -1}, {-1, -2}, {-2, -1}};

    for (int i = 0; i < 8; i++) {
        int newFile = pieceFile + knightMoves[i][0];
        int newRank = pieceRank + knightMoves[i][1];
        if (newFile >= 0 && newRank >= 0) {
            if (newFile < 8 && newRank < 8) {
                // can't be the same color piece
                if (getColor(position_ptr->board[newFile][newRank]) != piece_ptr->color) {
                    append(piece_ptr, newFile, newRank);
                }
            }
        }
    }
}

void listKingMoves(position *position_ptr, piece *piece_ptr) {
    int pieceFile = piece_ptr->coords[0];
    int pieceRank = piece_ptr->coords[1];

    int kingMoves[8][2] = {{1, 0},  {1, 1},  {1, -1},
                             {0, 1},  {0, -1},
                             {-1, 0}, {-1, 1}, {-1, -1}};

    for (int i = 0; i < 8; i++) {
        int newFile = pieceFile + kingMoves[i][0];
        int newRank = pieceRank + kingMoves[i][1];
        if (newFile >= 0 && newRank >= 0) {
            if (newFile < 8 && newRank < 8) {
                // can't be the same color piece
                if (getColor(position_ptr->board[newFile][newRank]) != piece_ptr->color) {
                    append(piece_ptr, newFile, newRank);
                }
            }
        }
    }

    // castling

    if (position_ptr->castlingRights[0] == 1 && piece_ptr->color == 'w') {
        int newFile = pieceFile + 2;
        int newRank = pieceRank;
        if (position_ptr->board[newFile][newRank] == ' ' && position_ptr->board[newFile - 1][newRank] == ' ') {
            append(piece_ptr, newFile, newRank);
        }
    }
    if (position_ptr->castlingRights[1] == 1 && piece_ptr->color == 'w') {
        int newFile = pieceFile - 2;
        int newRank = pieceRank;
        if (position_ptr->board[newFile][newRank] == ' ' && position_ptr->board[newFile + 1][newRank] == ' ' && position_ptr->board[newFile - 1][newRank] == ' ') {
            append(piece_ptr, newFile, newRank);
        }
    }
    if (position_ptr->castlingRights[2] == 1 && piece_ptr->color == 'b') {
        int newFile = pieceFile + 2;
        int newRank = pieceRank;
        if (position_ptr->board[newFile][newRank] == ' ' && position_ptr->board[newFile - 1][newRank] == ' ') {
            append(piece_ptr, newFile, newRank);
        }
    }
    if (position_ptr->castlingRights[3] == 1 && piece_ptr->color == 'b') {
        int newFile = pieceFile - 2;
        int newRank = pieceRank;
        if (position_ptr->board[newFile][newRank] == ' ' && position_ptr->board[newFile + 1][newRank] == ' ' && position_ptr->board[newFile - 1][newRank] == ' ') {
            append(piece_ptr, newFile, newRank);
        }
    }

}

void listRookMoves(position *position_ptr, piece *piece_ptr) {
    int pieceFile = piece_ptr->coords[0];
    int pieceRank = piece_ptr->coords[1];

    int rookMoves[4][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

    for (int i = 0; i < 4; i++) {
        int j = 0;
        int newFile = pieceFile;
        int newRank = pieceRank;
        while (j < 7) {

            newFile += rookMoves[i][0];
            newRank += rookMoves[i][1];

            if (newFile >= 0 && newRank >= 0) {
                if (newFile < 8 && newRank < 8) {
                    // can't be the same color piece
                    if (getColor(position_ptr->board[newFile][newRank]) != piece_ptr->color) {
                        append(piece_ptr, newFile, newRank);
                    }
                    if (position_ptr->board[newFile][newRank] != ' ') {
                        j = 10;
                    }
                }
                else {
                    j = 10;
                }
            }
            else {
                j = 10;
            }

            j++;
        }
    }
}

void listBishopMoves(position *position_ptr, piece *piece_ptr) {
    int pieceFile = piece_ptr->coords[0];
    int pieceRank = piece_ptr->coords[1];

    int bishopMoves[4][2] = {{1, 1}, {-1, 1}, {-1, -1}, {1, -1}};

    for (int i = 0; i < 4; i++) {
        int j = 0;
        int newFile = pieceFile;
        int newRank = pieceRank;
        while (j < 7) {

            newFile += bishopMoves[i][0];
            newRank += bishopMoves[i][1];

            if (newFile >= 0 && newRank >= 0) {
                if (newFile < 8 && newRank < 8) {
                    // can't be the same color piece
                    if (getColor(position_ptr->board[newFile][newRank]) != piece_ptr->color) {
                        append(piece_ptr, newFile, newRank);
                    }
                    if ((*position_ptr).board[newFile][newRank] != ' ') {
                        j = 10;
                    }
                }
                else {
                    j = 10;
                }
            }
            else {
                j = 10;
            }

            j++;
        }
    }
}

char getColor(char testPiece) {
    if (testPiece >= 'A' && testPiece <= 'Z') {
        return 'w';
    }
    if (testPiece >= 'a' && testPiece <= 'z') {
        return 'b';
    }
    return ' ';
}
