#include "game.h"
#include "board.h"

const int WIDTH =  800;
const int HEIGHT =  800;


void Draw();
void Update();
void handelInput();

void InitGame()
{
	InitWindow(WIDTH,HEIGHT,"Tetris");
	SetExitKey(KEY_NULL); // Disable default Esc key exit
	SetTargetFPS(60);
	initBoard();
	SetWindowIcon(LoadImage("assets/Logo.png"));
}

void RunGame()
{
	while(!WindowShouldClose())
	{
		BeginDrawing();
			ClearBackground(GRAY);
			Draw();
		EndDrawing();


		handelInput();
		Update();
	}
}

void CleanGame()
{
	CloseWindow();
}


void Draw()
{
  drawBoard();
}

void Update()
{
	updateBoard();
}

void handelInput()
{
	if(IsKeyPressed(KEY_UP)){
		rotateTetrominoCW();
	}
	if(IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_LEFT_SHIFT)){
		rotateTetrominoCWW();
	}
	if(IsKeyPressed(KEY_A)){
		shiftLeft();
	}
	if(IsKeyPressed(KEY_D)){
		shiftRight();
	}
	if(IsKeyPressed(KEY_SPACE)){
		instantLock();
	}
}
