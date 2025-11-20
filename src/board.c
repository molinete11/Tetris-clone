
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
	board.currentTetr.startSoftLockTimer = false;
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
	
	for(int sq = 0; sq < 4; sq++){
		const struct Vec2d piece = board.currentTetr.pieceGrid[sq];
		
		Rectangle rec = {
			.x = (piece.x + board.currentTetr.pos.x)*BLOCK_SIZE + 1,	.y = (piece.y + board.currentTetr.pos.y)*BLOCK_SIZE + 1,
			.width = BLOCK_SIZE - 2,	.height = BLOCK_SIZE - 2};
		DrawRectangleRec(rec,color);
	}
	
	color =  GhostColors[board.currentTetr.piece - 1];
	int smallerY = 19;
	int offset = -99999;
	for(int sq = 0; sq < 4; sq++){
		bool validCube = true;
		const struct Vec2d cube = board.currentTetr.pieceGrid[sq];

		for(int i = sq + 1; i < 4; i++){
			if(board.currentTetr.pieceGrid[i].x == cube.x){
				validCube = false;
				break;
			}
		}

		if(!validCube){ // filtro de cubos que no merecen ser iterados
			continue;
		}

		for(int row = cube.y + board.currentTetr.pos.y; row <= HEIGHT_B; row++){
			if(board.grid[row * WIDTH_B + cube.x + board.currentTetr.pos.x]){
				if(row - 1 < smallerY){
					smallerY = row - 1;
					break;
				}
			}
		}

		int y = cube.y - 3;
		if(y > offset){
			offset = y;
		}

		printf("offset %i\n",offset);
	}
	
	color =  GhostColors[board.currentTetr.piece - 1];

	if(board.currentTetr.piece  != I || board.currentTetr.piece != O){
		for(char sq = 0; sq < 4; sq++){
			const struct Vec2d piece = board.currentTetr.pieceGrid[sq];
			int y = smallerY + piece.y - 3 - offset;

			printf("y = %i\n",y);
			if(board.grid[(y+1) * WIDTH_B + piece.x + board.currentTetr.pos.x] || smallerY == 19){
				break;
			}

			if(sq == 3){
				offset -= 1;
				break;
			}
		}
	}

	for(int sq = 0; sq < 4; sq++){
		const struct Vec2d piece = board.currentTetr.pieceGrid[sq];
		int y = smallerY + piece.y - 3 - offset;
		//printf("Block y %i, offset %i, smaller y %i\n",sq, offset, smallerY);
		printf("Block pos y %i, offset %i, smaller y %i\n",piece.y, offset, smallerY);
		Rectangle rec = {
			.x = (piece.x + board.currentTetr.pos.x)*BLOCK_SIZE + 1, .y = y*BLOCK_SIZE + 1,
			.width = BLOCK_SIZE - 2,	.height = BLOCK_SIZE - 2};
		DrawRectangleRec(rec,color);
	}
}

void updateBoard()
{
	int autoShiftTimer = (clock() - board.currentTetr.autoShiftDownClock) * 0.001;
	
	if(autoShiftTimer >= 50){
		shiftDown();
		board.currentTetr.autoShiftDownClock = clock();
	}

	checkFilledRows();
}

void shiftDown()
{
	struct Vec2d nextPos = {board.currentTetr.pos.x,board.currentTetr.pos.y + 1};
	for(char sq = 0; sq < 4; sq++){
		struct Vec2d tmp = {board.currentTetr.pieceGrid[sq].x + nextPos.x, board.currentTetr.pieceGrid[sq].y + nextPos.y};
		if(checkNextPosition(tmp) || tmp.y >= HEIGHT_B){
			board.currentTetr.startSoftLockTimer = true;
			break;
		}
		board.currentTetr.startSoftLockTimer = false;
	}

	if(!board.currentTetr.startSoftLockTimer){
		board.currentTetr.softLockTimer = clock();
	}else{
		int softLockTimer = (clock() - board.currentTetr.softLockTimer) * 0.001;

		if(softLockTimer >= 140){
			lockPiece();
		}
		return;
	}

	board.currentTetr.pos = nextPos;
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
	board.currentTetr.startSoftLockTimer = false;
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

void checkFilledRows()
{
	char sqFilledInARow = 0;
	for(int row = 0; row < HEIGHT_B; row++){
		sqFilledInARow = 0;
		for(int col = 0; col < WIDTH_B; col++){
			

			if(board.grid[row * WIDTH_B + col]){
					sqFilledInARow++;
			}else{
				break;
			}

			if(sqFilledInARow == WIDTH_B){
				clearRow(row);
				sqFilledInARow = 0;
			}
		}
	}
}

void printBoard()
{
	printf("-----------------------------\n");
	for(int row = 0; row < HEIGHT_B; row++){
		for(int col = 0; col < WIDTH_B; col++){
			printf("%i",board.grid[row * WIDTH_B + col]);
		}
		printf("\n");
	}
}

void clearRow(int row)
{

	while(row > 1){
		for(int col = 0; col < 10; col++){
			setSquare(row, col, board.grid[(row - 1) * WIDTH_B + col]);
		}
		row--;
	}

	for(int col = 0; col < 10; col++){
		setSquare(row,col,BG);
	}
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
