#include "score.h"

#include <stdlib.h>
#include <stdio.h>

#include "../vendor/raylib.h"


static unsigned int score = 0;

void drawScore()
{
    // TODO: draw the score 

    char n[MAX_SCORE_LENGHT];

    sprintf(n, "%u", score);

    DrawText(n, 0, 0, SCORE_SIZE, RAYWHITE);

}

void updateScore(char nRows)
{
    score += nRows * 100;
}
