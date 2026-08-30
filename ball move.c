#include "raylib.h"
#include "raymath.h"

#define width 800
#define height 800
#define speed_1D 100
int speed_x = speed_1D;
int speed_y = speed_1D;

int Playground_width=width/4;
int Playground_height=height*3/4;

Vector2 Playground_center = {width/2,height/2};
Vector2 pot = {width/2,height/5};
Vector2 ball = {width/2,4*height/5};
Vector2 speed = {0,0};
int radius_pot=height/50;
int radius_ball=height/60;


int main()
{   
    InitWindow(width,height,"practice");
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {   
        float dt = GetFrameTime();
        ball = Vector2Add(ball,Vector2Scale(speed,dt));
        if (ball.x<(Playground_center.x - Playground_width/2 + radius_ball) || ball.x >(Playground_center.x + Playground_width/2 - radius_ball)) 
        speed_x = - speed_x;
        if (ball.y<(Playground_center.y - Playground_height/2 + radius_ball) || ball.y >(Playground_center.y + Playground_height/2 - radius_ball)) 
        speed_y = - speed_y;
        if (IsKeyDown(KEY_UP)) speed.y = -speed_y;
        else if (IsKeyDown(KEY_DOWN)) speed.y = speed_y;
        else speed.y =0;
        if (IsKeyDown(KEY_RIGHT)) speed.x = speed_x;
        else if (IsKeyDown(KEY_LEFT)) speed.x = -speed_x;
        else speed.x =0;
        if ((ball.x>pot.x-radius_pot/2) && (ball.x<pot.x+radius_pot/2) && (ball.y>pot.y-radius_pot/2) && (ball.y<pot.y+radius_pot/2))
        {
            ball.x=width/2;
            ball.y=4*height/5;
            speed.x=0;
            speed.y= 0;
        }


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