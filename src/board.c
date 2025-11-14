
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include "board.h"
#include "tetromino.h"

#define BLOCK_SIZE  40
#define WIDTH_B  10
#define HEIGHT_B  20


struct Board{ 
	int grid[200];
	struct Tetrominos currentTetr;
};

static struct Board board = {};

bool checkNextPosition(struct Vec2d nextPos);

void initBoard()
{
	for(int i = 0; i < 200; i++){
		board.grid[i] = BG;
	}

	initBag();

	board.currentTetr.piece = getPiece();
	board.currentTetr.pieceGrid = getPieceGrid(board.currentTetr.piece);
	board.currentTetr.pos = (struct Vec2d){.x = 3, .y = -2};
	board.currentTetr.autoShiftDownClock = clock();
	board.currentTetr.softLockTimer = clock();
	board.currentTetr.rotationIdx = 0;
	printf("%i\n",board.currentTetr.piece);
}

void drawBoard()
{
	Color color;
	for(int row = 0; row < HEIGHT_B; row++)
	{
		for(int col = 0; col < WIDTH_B; col++)
		{
			color =  Colors[board.grid[row * WIDTH_B + col]];
			Rectangle rec = {
				.x = col*BLOCK_SIZE + 1,	.y = row*BLOCK_SIZE + 1,
				.width = BLOCK_SIZE - 2,	.height = BLOCK_SIZE - 2};
			DrawRectangleRec(rec,color);
		}
	}
	color =  Colors[board.currentTetr.piece];

	for(char sq = 0; sq < 4; sq++){
		const struct Vec2d piece = board.currentTetr.pieceGrid[sq];
		Rectangle rec = {
			.x = (piece.x + board.currentTetr.pos.x)*BLOCK_SIZE + 1,	.y = (piece.y + board.currentTetr.pos.y)*BLOCK_SIZE + 1,
			.width = BLOCK_SIZE - 2,	.height = BLOCK_SIZE - 2};
		DrawRectangleRec(rec,color);
	}
}

void updateBoard()
{
	int autoShiftTimer = (clock() - board.currentTetr.autoShiftDownClock) * 0.001;
	
	if(autoShiftTimer >= 167){
		shiftDown();
		board.currentTetr.autoShiftDownClock = clock();
	}

}

void shiftDown()
{
	struct Vec2d lowestBlocks[4];

	for(char sq = 0; sq < 4; sq++){
		struct Vec2d nextBlock = board.currentTetr.pieceGrid[sq];
		for(char i = 0; i < 3 - sq; i++){
		}
	}
	
}

void lockPiece()
{
	for(char sq = 0; sq < 4; sq++){
			const struct Vec2d piece = board.currentTetr.pieceGrid[sq];
			setSquare(piece.y + board.currentTetr.pos.y, piece.x + board.currentTetr.pos.x, board.currentTetr.piece);	
	}
	addTetromino();
}

void addTetromino()
{
	board.currentTetr.piece = getPiece();
	board.currentTetr.pieceGrid = getPieceGrid(board.currentTetr.piece);
	board.currentTetr.pos = (struct Vec2d){.x = 3, .y = 0};
	board.currentTetr.autoShiftDownClock = clock();
	board.currentTetr.softLockTimer = clock();
	board.currentTetr.rotationIdx = 0;
	printf("%i\n",board.currentTetr.piece);
}

void rotateTetrominoCW()
{
	board.currentTetr.rotationIdx = (board.currentTetr.rotationIdx + 1) % 4;
	board.currentTetr.pieceGrid = getRotatedPieceGrid(board.currentTetr.piece,board.currentTetr.rotationIdx);
}

void rotateTetrominoCWW()
{
	board.currentTetr.rotationIdx = (board.currentTetr.rotationIdx - 1) % 4;
	if(board.currentTetr.rotationIdx < 0){
		board.currentTetr.rotationIdx = 3;
	}
	board.currentTetr.pieceGrid = getRotatedPieceGrid(board.currentTetr.piece,board.currentTetr.rotationIdx);
}

void shiftLeft()
{
	struct Vec2d nextPos = {board.currentTetr.pos.x-1,board.currentTetr.pos.y};
	for(char sq = 0; sq < 4; sq++){
		struct Vec2d tmp = {board.currentTetr.pieceGrid[sq].x + nextPos.x, board.currentTetr.pieceGrid[sq].y + nextPos.y};
		if(checkNextPosition(tmp) || tmp.x < 0){
			return;
		}
	}
	board.currentTetr.pos.x = nextPos.x;
}

void shiftRight()
{
	struct Vec2d nextPos = {board.currentTetr.pos.x+1,board.currentTetr.pos.y};
	for(char sq = 0; sq < 4; sq++){
		struct Vec2d tmp = {board.currentTetr.pieceGrid[sq].x + nextPos.x, board.currentTetr.pieceGrid[sq].y + nextPos.y};
		if(checkNextPosition(tmp) || tmp.x >= WIDTH_B){
			return;
		}
	}
	board.currentTetr.pos.x = nextPos.x;
}

bool checkNextPosition(struct Vec2d nextPos){
	if(board.grid[nextPos.y * WIDTH_B + nextPos.x]){
		return true;
	}

	return false;
}

static inline void setSquare(int row, int col, int value)
{
  if(board.grid[row * WIDTH_B + col] < 0 ||	board.grid[row * WIDTH_B + col]> 7 )
    return;
  board.grid[row * WIDTH_B + col] = value;
}
