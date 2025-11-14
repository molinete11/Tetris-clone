#include "tetromino.h"
#include <stdio.h>
#include <stdlib.h>

static const struct Vec2d Figures[7][4][4] = {
	{ // I
		{0,1,1,1,2,1,3,1}, // spawn
		{2,0,2,1,2,2,2,3}, // rotation 1
		{0,2,1,2,2,2,3,2}, // rotation 2
		{1,0,1,1,1,2,1,3}, // rotation 3
	},
	{ // J
		{0,0,0,1,1,1,2,1}, // spawn
		{1,0,2,0,1,1,2,3}, // rotation 1
		{0,1,1,1,2,1,2,2}, // rotation 2
		{1,0,1,1,1,2,0,2}, // rotation 3
	},
	{ // L
		{0,1,1,1,2,1,2,0}, // spawn
		{1,0,1,1,1,2,2,2}, // rotation 1
		{0,1,1,1,2,1,0,2}, // rotation 2
		{0,0,1,0,1,1,1,2}, // rotation 3
	},
	{ // O
		{1,0,2,0,1,1,2,1}, // spawn
		{1,0,2,0,1,1,2,1}, // spawn
		{1,0,2,0,1,1,2,1}, // spawn
		{1,0,2,0,1,1,2,1}, // spawn
	},
	{ // S
		{1,0,2,0,0,1,1,1}, // spawn
		{1,0,1,1,2,1,2,2}, // rotation 1
		{1,1,2,1,0,2,1,2}, // rotation 2
		{0,0,0,1,1,1,1,2}, // rotation 3
	},
	{ // Z
		{0,0,1,0,1,1,2,1}, // spawn
		{2,0,1,1,2,1,1,2}, // rotation 1
		{0,1,1,1,1,2,2,2}, // rotation 2
		{1,0,0,1,1,1,0,2}, // rotation 3
	},
		{ // T
		{1,0,0,1,1,1,2,1}, // spawn
		{0,1,2,1,1,1,1,2}, // rotation 1
		{0,1,2,1,1,1,1,2}, // rotation 2
		{1,0,0,1,1,1,1,2}, // rotation 3
	}
};


static TetrominosType bag[7];
static TetrominosType usedPieces[7];
static int usedPieceCount = 0;
static int piecesAvaiable = 7;


void initBag()
{
    TetrominosType opt[] = {I,J,L,O,S,Z,T};
    char avaiableOpts = 7;
    srand(time(NULL));

    for(char i = 0; i < 7; i++){
        char selected = (rand() % avaiableOpts);
        bag[i] = opt[selected];
        opt[selected] = opt[--avaiableOpts];
    }
}

TetrominosType getPiece()
{
    if(!piecesAvaiable){
		refillBag();
	}
	int selected = rand() % piecesAvaiable;
	TetrominosType tetr = bag[selected];
	bag[selected] = bag[--piecesAvaiable];
	usedPieces[usedPieceCount++] = tetr;
	return tetr;
}

const struct Vec2d* getPieceGrid(TetrominosType tetr)
{
    return Figures[tetr - 1][0];
}

static inline void refillBag()
{
	usedPieceCount = 0;
	piecesAvaiable = 7;
	for(char i = 0; i < 7; i++){
		bag[i] = usedPieces[i];
	}
}

const struct Vec2d *getRotatedPieceGrid(TetrominosType tetr,char idx)
{
    return Figures[tetr - 1][idx];
}
