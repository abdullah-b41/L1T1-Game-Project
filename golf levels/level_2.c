#include "raylib.h"
#include "raymath.h"


#define width 800
#define height 800

int Playground_width=width/4;
int Playground_height=height*3/4;

Vector2 Playground_center = {width/2,height/2};

Vector2 pot = {width/2,height/5};
Vector2 ball = {width/2,4*height/5};

#define max_speed 550
Vector2 speed = {0,0};

int radius_pot=height/60;
int radius_ball=height/75;

#define obstacle_edge 50
Rectangle obstacle = {(width/2-obstacle_edge/2),(height/2-obstacle_edge/2),50,50};

int main()
{   
    InitWindow(width,height,"practice");
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {   
        //wall bounce
        float dt = GetFrameTime();
        ball = Vector2Add(ball,Vector2Scale(speed,dt));
        if (ball.x<(Playground_center.x - Playground_width/2 + radius_ball) || ball.x >(Playground_center.x + Playground_width/2 - radius_ball)) 
        {
            speed.x = - speed.x;
            if (ball.x <(Playground_center.x - Playground_width/2 + radius_ball)) ball.x = (Playground_center.x - Playground_width/2 + radius_ball);
            if (ball.x >(Playground_center.x + Playground_width/2 - radius_ball)) ball.x = (Playground_center.x + Playground_width/2 - radius_ball);
        } 
        if (ball.y<(Playground_center.y - Playground_height/2 + radius_ball) || ball.y >(Playground_center.y + Playground_height/2 - radius_ball)) 
        {
            speed.y = - speed.y;
            if (ball.y< (Playground_center.y - Playground_height/2 + radius_ball)) ball.y = (Playground_center.y - Playground_height/2 + radius_ball);
            if (ball.y >(Playground_center.y + Playground_height/2 - radius_ball)) ball.y = (Playground_center.y + Playground_height/2 - radius_ball);
        } 
        


        //collision
        if (CheckCollisionCircleRec(ball,radius_ball,obstacle))
        {
            Vector2 collision_point;
            collision_point.x = Clamp(ball.x,width/2-obstacle_edge/2,width/2+obstacle_edge/2);
            collision_point.y = Clamp(ball.y,height/2-obstacle_edge/2,height/2+obstacle_edge/2);
            Vector2 normal = Vector2Normalize(Vector2Subtract(ball,collision_point));
            speed = Vector2Reflect(speed,normal);
            ball = Vector2Add(collision_point,Vector2Scale(normal,radius_ball));
        }

        //proportional deceleration
        float deceleration=100.00;
        if (fabsf(speed.x)>=fabsf(speed.y) && speed.x!=0)
        {
            if (speed.y>0) speed.y-=deceleration*dt*fabsf(speed.y/speed.x);
            if (speed.y<0) speed.y+=deceleration*dt*fabsf(speed.y/speed.x);
            if (speed.x>0) speed.x-=deceleration*dt;
            if (speed.x<0) speed.x+=deceleration*dt;
        }
        else if (fabsf(speed.x)<fabsf(speed.y) && speed.y!=0)
        {
            if (speed.x>0) speed.x-=deceleration*dt*fabsf(speed.x/speed.y);
            if (speed.x<0) speed.x+=deceleration*dt*fabsf(speed.x/speed.y);
            if (speed.y>0) speed.y-=deceleration*dt;
            if (speed.y<0) speed.y+=deceleration*dt;
        }

        if (speed.x<5 && speed.x>-5) speed.x=0;
        if (speed.y<5 && speed.y>-5) speed.y=0;
        
        //shooting
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && speed.x==0 && speed.y==0)
        {
            int mouse_x=GetMouseX();
            int mouse_y=GetMouseY();
            int drag_x=ball.x-mouse_x;
            int drag_y=ball.y-mouse_y;
            speed.x=drag_x*2;
            if (fabsf(speed.x)>max_speed) speed.x = speed.x / fabsf(speed.x) * max_speed;
            speed.y=drag_y*2;
            if (fabsf(speed.y)>max_speed) speed.y = speed.y / fabsf(speed.y) * max_speed;
        }
        
        //score
        if ((ball.x>pot.x-3*radius_pot/4) && (ball.x<pot.x+3*radius_pot/4) && (ball.y>pot.y-3*radius_pot/4) && (ball.y<pot.y+3*radius_pot/4))
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
        DrawCircleLines(pot.x,pot.y,radius_pot+1,WHITE);
        DrawCircleLines(ball.x,ball.y,radius_ball+1,BLACK);
        DrawRectangleRec(obstacle,RED);
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && speed.x==0 && speed.y==0)
        {
            DrawLine(ball.x, ball.y, GetMouseX(), GetMouseY(), BLACK);
        }
        DrawRectangleLines(width/2-obstacle_edge/2,height/2-obstacle_edge/2,obstacle_edge,obstacle_edge,BLACK);

        EndDrawing();
    }
    CloseWindow();


    return 0;
}