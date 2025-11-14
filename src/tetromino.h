#ifndef TETROMINOS_H
#define TETROMINOS_H
#include <time.h>


typedef enum TetrominosType
{
	BG = 0,
	I,
	J,
	L,
	O,
	S,
	Z,
	T
}TetrominosType;

struct Vec2d{
	char x, y;
};

struct Tetrominos{
	clock_t autoShiftDownClock;
	clock_t softLockTimer;
	const struct Vec2d* pieceGrid;
	TetrominosType piece;
	struct Vec2d pos;
	char rotationIdx;
};


void initBag();
TetrominosType getPiece();
const struct Vec2d* getPieceGrid(TetrominosType tetr);
static inline  void refillBag();
const struct Vec2d* getRotatedPieceGrid(TetrominosType tetr, char idx); 

#endif