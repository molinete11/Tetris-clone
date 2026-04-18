
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#include "board.h"
#include "tetromino.h"

#define BLOCK_SIZE  40
#define WIDTH_B  10
#define HEIGHT_B  20
#define LEFT_SPACE (BLOCK_SIZE * 7)
#define BOARD_WIDTH (BLOCK_SIZE * WIDTH_B)


struct Board{ 
	int grid[200];
	float dropV;
	int level;
	int linesCleared;
	struct Tetrominos currentTetr;
	enum TetrominosType hold;
	enum TetrominosType queue[4];
	char holding;
	char swap;
};


/*
	0 -> Spawn state
	R -> State after a clockwise rotation
	L -> State after a counterclockwise rotation
	2 -> State after two rotations in either direction
*/

// Special offsets for  rotation kicks

const struct Vec2d rotationOffsets[4][5] = {
	{{.x = 0, .y = 0}, {.x = 0, .y = 0}, {.x = 0, .y = 0}, {.x = 0, .y = 0}, {.x = 0, .y = 0}},
	{{.x = 0, .y = 0}, {.x = 1, .y = 0}, {.x = 1, .y = -1}, {.x = 0, .y = 2}, {.x = 1, .y = 2}},
	{{.x = 0, .y = 0}, {.x = 0, .y = 0}, {.x = 0, .y = 0}, {.x = 0, .y = 0}, {.x = 0, .y = 0}},
	{{.x = 0, .y = 0}, {.x = -1, .y = 0}, {.x = -1, .y = -1}, {.x = 0, .y = 2}, {.x = -1, .y = 2}},
};

const struct Vec2d rotationOffsetsI[4][5] = {
	{{.x = 0, .y = 0}, {.x = -1, .y = 0}, {.x = 2, .y = 0}, {.x = -1, .y = 0}, {.x = 2, .y = 0}},
	{{.x = -1, .y = 0}, {.x = 0, .y = 0}, {.x = 0, .y = 0}, {.x = 0, .y = 1}, {.x = 0, .y = -2}},
	{{.x = -1, .y = 1}, {.x = 1, .y = 1}, {.x = -2, .y = 1}, {.x = 1, .y = 0}, {.x = -2, .y = 0}},
	{{.x = 0, .y = 1}, {.x = 0, .y = 1}, {.x = 0, .y = 1}, {.x = 0, .y = -1}, {.x = 0, .y = 2}},
};


static struct Board board = {};

bool checkNextPosition(struct Vec2d nextPos);
void initPieceQueue(TetrominosType queue[4]);
float getDropVelocity(int level);

void initBoard()
{
	for(int i = 0; i < 200; i++){
		board.grid[i] = BG;
	}

	initBag();

	board.holding = 0;
	board.swap = 0;
	board.level = 1;
	//board.dropV = getG(board.level);

	board.currentTetr.piece = getPiece();
	board.currentTetr.pieceGrid = getPieceGrid(board.currentTetr.piece);
	board.currentTetr.pos = (struct Vec2d){.x = 3, .y = -2};
	board.currentTetr.autoShiftDownClock = clock();
	board.currentTetr.softLockTimer = clock();
	board.currentTetr.rotationIdx = 0;
	board.linesCleared = 0;
	board.currentTetr.startSoftLockTimer = false;

	initPieceQueue(board.queue);
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
				.x = col*BLOCK_SIZE + 1 + LEFT_SPACE,	.y = row*BLOCK_SIZE + 1,
				.width = BLOCK_SIZE - 2,	.height = BLOCK_SIZE - 2};
			DrawRectangleRec(rec,color);
		}
	}

	color =  Colors[board.currentTetr.piece];
	
	for(int sq = 0; sq < 4; sq++){
		const struct Vec2d piece = board.currentTetr.pieceGrid[sq];
		
		Rectangle rec = {
			.x = (piece.x + board.currentTetr.pos.x)*BLOCK_SIZE + 1 + LEFT_SPACE,	.y = (piece.y + board.currentTetr.pos.y)*BLOCK_SIZE + 1,
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

		for(int row = cube.y + board.currentTetr.pos.y; row < HEIGHT_B; row++){
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
	}
	
	color =  GhostColors[board.currentTetr.piece - 1];
	
	if(board.currentTetr.piece  != I && board.currentTetr.piece != O && smallerY != 19){
		for(char sq = 0; sq < 4; sq++){
			const struct Vec2d piece = board.currentTetr.pieceGrid[sq];
			int y = smallerY + piece.y - 3 - offset;
			int x = piece.x + board.currentTetr.pos.x;

			if(board.grid[(y+1) * WIDTH_B + x]){
				break;
			}
			if(sq != 3){
				continue;
			}

			offset -= 1;

			if(board.currentTetr.piece != L && board.currentTetr.piece != J){
				break;
			}	

			if(board.currentTetr.piece == L && board.currentTetr.rotationIdx == 3 && !board.grid[(y+2) * WIDTH_B + x]){
				offset -= 1;
				break;
			}

			if(board.currentTetr.piece == J && board.currentTetr.rotationIdx == 1 && !board.grid[(y+2) * WIDTH_B + x]){
				offset -= 1;
				break;
			}
		}
	}

	for(int sq = 0; sq < 4; sq++){
		const struct Vec2d piece = board.currentTetr.pieceGrid[sq];
		int y = smallerY + piece.y - 3 - offset;

		Rectangle rec = {
			.x = (piece.x + board.currentTetr.pos.x)*BLOCK_SIZE + 1 + LEFT_SPACE, .y = y*BLOCK_SIZE + 1,
			.width = BLOCK_SIZE - 2,	.height = BLOCK_SIZE - 2};
		DrawRectangleRec(rec,color);
	}
}

void drawHoldPiece(){

	const int x = BLOCK_SIZE * 2;
	const int y = BLOCK_SIZE * 2;


	DrawRectangleLines(x, y, BLOCK_SIZE * 4, BLOCK_SIZE * 4, (Color){.a = 255, .r = 255, .g = 0, .b = 0});

	if(board.holding){

		const struct Vec2d* tetromino = getPieceGrid(board.hold);
		const Color c = Colors[board.hold];

		for(int sq = 0; sq < 4; sq++){
			const struct Vec2d piece = tetromino[sq];

			Rectangle rec = {
				.x = (piece.x * BLOCK_SIZE + x), .y = piece.y * BLOCK_SIZE + y,
				.width = BLOCK_SIZE - 2,	.height = BLOCK_SIZE - 2};


			DrawRectangleRec(rec, c);
		}
	}
}

void storePiece()
{
	if(!board.holding){
		board.hold = board.currentTetr.piece;
		getNextQueuePiece();
		board.holding = 1;
	}else if(board.swap){
		enum TetrominosType tmp = board.currentTetr.piece;
		board.currentTetr.piece = board.hold;
		board.hold = tmp;
		board.currentTetr.pieceGrid = getPieceGrid(board.currentTetr.piece);
		board.currentTetr.pos.y = 0;
		board.currentTetr.pos.x = 3;
		board.currentTetr.rotationIdx = 0;
		board.swap = 0;
	}
}

char updateBoard()
{	
	int autoShiftTimer = (clock() - board.currentTetr.autoShiftDownClock) * 0.001;

	
	if(autoShiftTimer >= getDropVelocity(board.level) * 60){
		shiftDown();
		board.currentTetr.autoShiftDownClock = clock();
	}
	
	char filledRows = checkFilledRows();

	board.linesCleared += filledRows;

	if(board.linesCleared >= board.level * 10){
		board.level++;
	}

	return filledRows;
}

void shiftDown()
{
	struct Vec2d nextPos = {board.currentTetr.pos.x,board.currentTetr.pos.y + 1};
	for(char sq = 0; sq < 4; sq++){
		struct Vec2d tmp = {board.currentTetr.pieceGrid[sq].x + nextPos.x, board.currentTetr.pieceGrid[sq].y + nextPos.y};
		if(tmp.y >= HEIGHT_B || checkNextPosition(tmp)){
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
			board.swap = 1;
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
	//addTetromino();
	getNextQueuePiece();
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

void rotateTetrominoCW() // 0 -> R, R -> 2, 2 -> L, L -> 0
{	
	if(board.currentTetr.piece != I){
		const char testCase = board.currentTetr.rotationIdx;
		char nextRotationIdx = (board.currentTetr.rotationIdx + 1) % 4;
		const struct Vec2d pos = board.currentTetr.pos;
		const struct Vec2d *grid = getRotatedPieceGrid(board.currentTetr.piece, nextRotationIdx);

		struct Vec2d offsetsTest[5];

		for(int i = 0; i < 5; i++){
			offsetsTest[i] = (struct Vec2d){
				.x = rotationOffsets[testCase][i].x - rotationOffsets[nextRotationIdx][i].x,
				.y = rotationOffsets[testCase][i].y - rotationOffsets[nextRotationIdx][i].y,
			};
		}

		for(int i = 0; i < 5; i++){
			char success = 1;
			for(int j = 0; j < 4; j++){
				const int nX = grid[j].x + pos.x + offsetsTest[i].x;
				const int nY = grid[j].y + pos.y + offsetsTest[i].y;
				if((nX < 0 || nX > 9) || (nY >= HEIGHT_B)){
					success = 0;
					break;
				}

				if(board.grid[nY * WIDTH_B + nX]){
					success = 0;
					break;
				}
			}

			if(success){
				board.currentTetr.pos.x += offsetsTest[i].x;
				board.currentTetr.pos.y += offsetsTest[i].y;
				board.currentTetr.rotationIdx = nextRotationIdx;
				board.currentTetr.pieceGrid = grid;
				break;
			}
		}
	}else{
		const char testCase = board.currentTetr.rotationIdx;
		char nextRotationIdx = (board.currentTetr.rotationIdx + 1) % 4;
		const struct Vec2d pos = board.currentTetr.pos;
		const struct Vec2d *grid = getRotatedPieceGrid(board.currentTetr.piece, nextRotationIdx);

		struct Vec2d offsetsTest[5];

		for(int i = 0; i < 5; i++){
			offsetsTest[i] = (struct Vec2d){
				.x = rotationOffsetsI[testCase][i].x - rotationOffsetsI[nextRotationIdx][i].x,
				.y = rotationOffsetsI[testCase][i].y - rotationOffsetsI[nextRotationIdx][i].y,
			};
		}

		for(int i = 0; i < 5; i++){
			char success = 1;
			for(int j = 0; j < 4; j++){
				const int nX = grid[j].x + pos.x + offsetsTest[i].x;
				const int nY = grid[j].y + pos.y + offsetsTest[i].y;
				if((nX < 0 || nX > 9) || (nY >= HEIGHT_B)){
					success = 0;
					break;
				}

				if(board.grid[nY * WIDTH_B + nX]){
					success = 0;
					break;
				}
			}

			if(success){
				board.currentTetr.pos.x += offsetsTest[i].x;
				board.currentTetr.pos.y += offsetsTest[i].y;
				board.currentTetr.rotationIdx = nextRotationIdx;
				board.currentTetr.pieceGrid = grid;
				break;
			}
		}
	}
}

void rotateTetrominoCWW() // 0 -> L, L -> 2, 2 -> R, R -> 0
{
	if(board.currentTetr.piece != I){
		const char testCase = board.currentTetr.rotationIdx;
		char nextRotationIdx = (board.currentTetr.rotationIdx - 1) % 4;
		nextRotationIdx = (nextRotationIdx < 0) ? 3 : nextRotationIdx;
		const struct Vec2d pos = board.currentTetr.pos;
		const struct Vec2d *grid = getRotatedPieceGrid(board.currentTetr.piece, nextRotationIdx);

		struct Vec2d offsetsTest[5];

		for(int i = 0; i < 5; i++){
			offsetsTest[i] = (struct Vec2d){
				.x = rotationOffsets[testCase][i].x - rotationOffsets[nextRotationIdx][i].x,
				.y = rotationOffsets[testCase][i].y - rotationOffsets[nextRotationIdx][i].y,
			};
		}

		for(int i = 0; i < 5; i++){
			char success = 1;
			for(int j = 0; j < 4; j++){
				const int nX = grid[j].x + pos.x + offsetsTest[i].x;
				const int nY = grid[j].y + pos.y + offsetsTest[i].y;
				if((nX < 0 || nX > 9) || (nY >= HEIGHT_B)){
					success = 0;
					break;
				}

				if(board.grid[nY * WIDTH_B + nX]){
					success = 0;
					break;
				}
			}

			if(success){
				board.currentTetr.pos.x += offsetsTest[i].x;
				board.currentTetr.pos.y += offsetsTest[i].y;
				board.currentTetr.rotationIdx = nextRotationIdx;
				board.currentTetr.pieceGrid = grid;
				break;
			}
		}
	}else{
		const char testCase = board.currentTetr.rotationIdx;
		char nextRotationIdx = (board.currentTetr.rotationIdx - 1) % 4;
		nextRotationIdx = (nextRotationIdx < 0) ? 3 : nextRotationIdx;
		const struct Vec2d pos = board.currentTetr.pos;
		const struct Vec2d *grid = getRotatedPieceGrid(board.currentTetr.piece, nextRotationIdx);

		struct Vec2d offsetsTest[5];

		for(int i = 0; i < 5; i++){
			offsetsTest[i] = (struct Vec2d){
				.x = rotationOffsetsI[testCase][i].x - rotationOffsetsI[nextRotationIdx][i].x,
				.y = rotationOffsetsI[testCase][i].y - rotationOffsetsI[nextRotationIdx][i].y,
			};
		}

		for(int i = 0; i < 5; i++){
			char success = 1;
			for(int j = 0; j < 4; j++){
				const int nX = grid[j].x + pos.x + offsetsTest[i].x;
				const int nY = grid[j].y + pos.y + offsetsTest[i].y;
				if((nX < 0 || nX > 9) || (nY >= HEIGHT_B)){
					success = 0;
					break;
				}

				if(board.grid[nY * WIDTH_B + nX]){
					success = 0;
					break;
				}
			}

			if(success){
				board.currentTetr.pos.x += offsetsTest[i].x;
				board.currentTetr.pos.y += offsetsTest[i].y;
				board.currentTetr.rotationIdx = nextRotationIdx;
				board.currentTetr.pieceGrid = grid;
				break;
			}
		}
	}
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

char checkFilledRows()
{
	char nFilledRows = 0;
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
				nFilledRows++;
				clearRow(row);
				sqFilledInARow = 0;
			}
		}
	}

	return nFilledRows;
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

void instantLock()
{
	bool locked = false;
	struct Vec2d nextPos = board.currentTetr.pos;
	while(!locked){
		nextPos.y++;
		for(char sq = 0; sq < 4; sq++){
			struct Vec2d tmp = {board.currentTetr.pieceGrid[sq].x + nextPos.x, board.currentTetr.pieceGrid[sq].y + nextPos.y};
			if(checkNextPosition(tmp) || tmp.y >= HEIGHT_B){
				nextPos.y--;
				board.currentTetr.pos = nextPos;
				board.swap = 1;
				lockPiece();
				locked = true;
				break;
			}
		}
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

void drawPieceQueue()
{
	const int x = LEFT_SPACE + BOARD_WIDTH + BLOCK_SIZE;
	const int y = BLOCK_SIZE * 2;

	DrawRectangleLines(x, y, BLOCK_SIZE * 4, BLOCK_SIZE * 4, (Color){.a = 255, .r = 255, .g = 0, .b = 0});

	TetrominosType nextPiece = board.queue[0];

	const struct Vec2d* tetromino = getPieceGrid(nextPiece);
	Color c = Colors[nextPiece];

	for(int sq = 0; sq < 4; sq++){
		const struct Vec2d piece = tetromino[sq];

		Rectangle rec = {
			.x = (piece.x * BLOCK_SIZE + x), .y = piece.y * BLOCK_SIZE + y,
			.width = BLOCK_SIZE - 2,	.height = BLOCK_SIZE - 2};


		DrawRectangleRec(rec, c);
	}
	
	const int nextPiecesY = BLOCK_SIZE * 4 + y + BLOCK_SIZE;
	
	DrawRectangleLines(x, nextPiecesY, BLOCK_SIZE * 4, (BLOCK_SIZE * 4) * 3, (Color){.a = 255, .r = 255, .g = 0, .b = 0});

	for(int i = 0; i < 3; i++){
		nextPiece = board.queue[i + 1];
		c = Colors[nextPiece];
		int offset = (BLOCK_SIZE * 4) * i;
		const struct Vec2d* nTetromino = getPieceGrid(nextPiece);

		for(int sq = 0; sq < 4; sq++){
			const struct Vec2d piece = nTetromino[sq];

			Rectangle rec = {
				.x = (piece.x * BLOCK_SIZE + x), .y = piece.y * BLOCK_SIZE + nextPiecesY + offset,
				.width = BLOCK_SIZE - 2,	.height = BLOCK_SIZE - 2};


			DrawRectangleRec(rec, c);
		}
	}
}

void getNextQueuePiece()
{
	board.currentTetr.piece = board.queue[0];
	board.currentTetr.pieceGrid = getPieceGrid(board.currentTetr.piece);
	board.currentTetr.pos = (struct Vec2d){.x = 3, .y = 0};
	board.currentTetr.autoShiftDownClock = clock();
	board.currentTetr.softLockTimer = clock();
	board.currentTetr.rotationIdx = 0;
	board.currentTetr.startSoftLockTimer = false;

	board.queue[0] = board.queue[1];
	board.queue[1] = board.queue[2];
	board.queue[2] = board.queue[3];
	board.queue[3] = getPiece();
}

void initPieceQueue(TetrominosType queue[4])
{
	for(int i = 0; i < 4; i++){
		queue[i] = getPiece();
	}
}

float getDropVelocity(int level)
{
    return powf((0.8f - ((level - 1.f) * 0.007f)), (level - 1));
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
