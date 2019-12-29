#ifndef PIECE_RULES_H
#define PIECE_RULES_H

#include "position.h"

void listPawnMoves(position *position_ptr, piece *piece_ptr);
void listKnightMoves(position *position_ptr, piece *piece_ptr);
void listKingMoves(position *position_ptr, piece *piece_ptr);
void listRookMoves(position *position_ptr, piece *piece_ptr);
void listBishopMoves(position *position_ptr, piece *piece_ptr);

void append(piece *piece_ptr, int file, int rank);
char getColor(char testPiece);

#endif // PIECE_RULES_H
