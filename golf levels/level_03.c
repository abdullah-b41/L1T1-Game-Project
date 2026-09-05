#include "raylib.h"
#include "raymath.h"


#define width 800
#define height 800
int Playground_width=width/4;
int Playground_height=height*3/4;
Vector2 Playground_center = {width/2,height/2};
Vector2 pot = {width/2,height/5};
int radius_pot=height/60;
int radius_ball=height/75;
#define max_speed 400

Vector2 ball_l3 = {width/2,4*height/5};
Vector2 speed_l3 = {0,0};
#define obstacle_edge_l3 50
Rectangle obstacle_l3 = {(width/2-obstacle_edge_l3/2),(height/2-obstacle_edge_l3/2),50,50};

int main()
{   
    InitWindow(width,height,"practice");
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {   
        //wall bounce
        float dt = GetFrameTime();
        ball_l3 = Vector2Add(ball_l3,Vector2Scale(speed_l3,dt));
        if (ball_l3.x<(Playground_center.x - Playground_width/2 + radius_ball) || ball_l3.x >(Playground_center.x + Playground_width/2 - radius_ball)) 
        {
            speed_l3.x = - speed_l3.x;
            if (ball_l3.x <(Playground_center.x - Playground_width/2 + radius_ball)) ball_l3.x = (Playground_center.x - Playground_width/2 + radius_ball);
            if (ball_l3.x >(Playground_center.x + Playground_width/2 - radius_ball)) ball_l3.x = (Playground_center.x + Playground_width/2 - radius_ball);
        } 
        if (ball_l3.y<(Playground_center.y - Playground_height/2 + radius_ball) || ball_l3.y >(Playground_center.y + Playground_height/2 - radius_ball)) 
        {
            speed_l3.y = - speed_l3.y;
            if (ball_l3.y< (Playground_center.y - Playground_height/2 + radius_ball)) ball_l3.y = (Playground_center.y - Playground_height/2 + radius_ball);
            if (ball_l3.y >(Playground_center.y + Playground_height/2 - radius_ball)) ball_l3.y = (Playground_center.y + Playground_height/2 - radius_ball);
        } 
        


        //collision
        if (CheckCollisionCircleRec(ball_l3,radius_ball,obstacle_l3))
        {
            Vector2 collision_point;
            collision_point.x = Clamp(ball_l3.x,width/2-obstacle_edge_l3/2,width/2+obstacle_edge_l3/2);
            collision_point.y = Clamp(ball_l3.y,height/2-obstacle_edge_l3/2,height/2+obstacle_edge_l3/2);
            Vector2 normal = Vector2Normalize(Vector2Subtract(ball_l3,collision_point));
            speed_l3 = Vector2Reflect(speed_l3,normal);
            ball_l3 = Vector2Add(collision_point,Vector2Scale(normal,radius_ball));
        }

        //proportional deceleration
        float deceleration=100.00;
        if (fabsf(speed_l3.x)>=fabsf(speed_l3.y) && speed_l3.x!=0)
        {
            if (speed_l3.y>0) speed_l3.y-=deceleration*dt*fabsf(speed_l3.y/speed_l3.x);
            if (speed_l3.y<0) speed_l3.y+=deceleration*dt*fabsf(speed_l3.y/speed_l3.x);
            if (speed_l3.x>0) speed_l3.x-=deceleration*dt;
            if (speed_l3.x<0) speed_l3.x+=deceleration*dt;
        }
        else if (fabsf(speed_l3.x)<fabsf(speed_l3.y) && speed_l3.y!=0)
        {
            if (speed_l3.x>0) speed_l3.x-=deceleration*dt*fabsf(speed_l3.x/speed_l3.y);
            if (speed_l3.x<0) speed_l3.x+=deceleration*dt*fabsf(speed_l3.x/speed_l3.y);
            if (speed_l3.y>0) speed_l3.y-=deceleration*dt;
            if (speed_l3.y<0) speed_l3.y+=deceleration*dt;
        }

        if (speed_l3.x<5 && speed_l3.x>-5) speed_l3.x=0;
        if (speed_l3.y<5 && speed_l3.y>-5) speed_l3.y=0;
        
        //shooting
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && speed_l3.x==0 && speed_l3.y==0)
        {
            int mouse_x=GetMouseX();
            int mouse_y=GetMouseY();
            int drag_x=ball_l3.x-mouse_x;
            int drag_y=ball_l3.y-mouse_y;
            speed_l3.x=drag_x*2;
            if (fabsf(speed_l3.x)>max_speed) speed_l3.x = speed_l3.x / fabsf(speed_l3.x) * max_speed;
            speed_l3.y=drag_y*2;
            if (fabsf(speed_l3.y)>max_speed) speed_l3.y = speed_l3.y / fabsf(speed_l3.y) * max_speed;
        }
        
        //score
        if ((ball_l3.x>pot.x-3*radius_pot/4) && (ball_l3.x<pot.x+3*radius_pot/4) && (ball_l3.y>pot.y-3*radius_pot/4) && (ball_l3.y<pot.y+3*radius_pot/4))
        {   
            ball_l3.x=width/2;
            ball_l3.y=4*height/5;
            speed_l3.x=0;
            speed_l3.y= 0;
        }


        BeginDrawing();
        ClearBackground(SKYBLUE);

        Rectangle Playground = {(Playground_center.x - Playground_width/2),(Playground_center.y - Playground_height/2),Playground_width,Playground_height};
        DrawRectangleRec(Playground,GetColor(0x00F000FF));
        DrawCircle(pot.x,pot.y,radius_pot,BLACK);
        DrawCircle(ball_l3.x,ball_l3.y,radius_ball,WHITE);
        DrawRectangleLinesEx(Playground,5,BLACK);
        DrawCircleLines(pot.x,pot.y,radius_pot+1,WHITE);
        DrawCircleLines(ball_l3.x,ball_l3.y,radius_ball+1,BLACK);
        DrawRectangleRec(obstacle_l3,RED);
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && speed_l3.x==0 && speed_l3.y==0)
        {
            DrawLine(ball_l3.x, ball_l3.y, GetMouseX(), GetMouseY(), BLACK);
        }
        DrawRectangleLines(width/2-obstacle_edge_l3/2,height/2-obstacle_edge_l3/2,obstacle_edge_l3,obstacle_edge_l3,BLACK);

        EndDrawing();
    }
    CloseWindow();


    return 0;
}