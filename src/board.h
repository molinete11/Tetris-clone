#ifndef BOARD_H
#define BOARD_H

#include "../vendor/raylib.h"


static const Color Colors[] =
{
	DARKGRAY,						// back
	{.r=173,.g=216,.b=230,.a=255},	// I piece
	{.r=0,.g=0,.b=139,.a=255}, 	// J piece
	{.r=255,.g=165,.b=0,.a=255},	// L piece
	{.r=255,.g=255,.b=0,.a=255},	// O piece
	{.r=0,.g=255,.b=0,.a=255},	// S piece
	{.r=255,.g=0,.b=0,.a=255},	// Z piece
	{.r=255,.g=0,.b=255,.a=255},	// T piece
};

static const Color GhostColors[] =
{
	{.r=173,.g=216,.b=230,.a=175},	// I piece
	{.r=0,.g=0,.b=139,.a=175}, 	// J piece
	{.r=255,.g=165,.b=0,.a=175},	// L piece
	{.r=255,.g=255,.b=0,.a=175},	// O piece
	{.r=0,.g=255,.b=0,.a=175},	// S piece
	{.r=255,.g=0,.b=0,.a=175},	// Z piece
	{.r=255,.g=0,.b=255,.a=175},	// T piece
};


void initBoard();
void drawBoard();
void drawHoldPiece();
void storePiece();
char updateBoard();
void shiftDown();
void lockPiece();
void addTetromino();
void rotateTetrominoCW();
void rotateTetrominoCWW();
void shiftLeft();
void shiftRight();
char checkFilledRows();
void printBoard();
void instantLock();
void clearRow(int row);
void drawPieceQueue();
void getNextQueuePiece();



static inline void setSquare(int row, int col, int value);

#endif
