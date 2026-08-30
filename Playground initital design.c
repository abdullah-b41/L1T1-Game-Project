#include "raylib.h"

#define width 800
#define height 800

int Playground_width=width/4;
int Playground_height=height*3/4;

Vector2 Playground_center = {width/2,height/2};
Vector2 pot = {width/2,height/5};
Vector2 ball = {width/2,4*height/5};
int radius_pot=height/50;
int radius_ball=height/60;

int main()
{   
    InitWindow(width,height,"practice");
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(SKYBLUE);

        Rectangle Playground = {(Playground_center.x - Playground_width/2),(Playground_center.y - Playground_height/2),Playground_width,Playground_height};
        DrawRectangleRec(Playground,GetColor(0x00F000FF));
        DrawCircle(pot.x,pot.y,radius_pot,BLACK);
        DrawCircle(ball.x,ball.y,radius_ball,WHITE);
        DrawRectangleLinesEx(Playground,5,BLACK);
        DrawCircleLines(pot.x,pot.y,radius_pot+1,YELLOW);
        DrawCircleLines(ball.x,ball.y,radius_ball+1,BLACK);

        EndDrawing();
    }
    CloseWindow();


    return 0;
}