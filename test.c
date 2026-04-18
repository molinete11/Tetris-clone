#include <math.h>
#include <stdio.h>

#define FRAME_CAP 60

double fn(int level){
    return powf((0.8-((level-1)*0.007)), (level - 1));
}


int main(){
    for(int i = 1; i <= 20; i++){
        double g = 1 / (fn(i) * FRAME_CAP);
        printf("G = %lf\n", g);

        printf("Frames per ROW %lf\n", 1 / g);
    }
}