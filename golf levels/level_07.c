#include "raylib.h"
#include "raymath.h"


#define width 800
#define height 800
int Playground_width=width/4;
int Playground_height=height*3/4;
Vector2 Playground_center = {width/2,height/2};
Vector2 pot = {width/2,height/5};
#define max_speed 400
int radius_pot=height/60;
int radius_ball=height/75;

Vector2 ball_l7 = {width/2,4*height/5};
Vector2 speed_l7 = {0,0};
# define obstacle_width_l7 133.33
# define obstacle_height_l7 15
//#define obstacle_edge 50
//Rectangle obstacle = {(width/2-obstacle_edge/2),(height/2-obstacle_edge/2),50,50};

int main()
{   
    Rectangle Playground = {(Playground_center.x - Playground_width/2),(Playground_center.y - Playground_height/2),Playground_width,Playground_height};
    Rectangle obstacle_top_l7 = {(Playground_center.x - Playground_width/2 +3),  (Playground_center.y - Playground_height/2 + Playground_height/3), obstacle_width_l7, obstacle_height_l7};
    Rectangle obstacle_bottom_l7 = {(Playground_center.x - Playground_width/2 + Playground_width/3 -1),  (Playground_center.y - Playground_height/2 + Playground_height*2/3), obstacle_width_l7, obstacle_height_l7};


    InitWindow(width,height,"practice");
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {   
        //wall bounce
        float dt = GetFrameTime();
        ball_l7 = Vector2Add(ball_l7,Vector2Scale(speed_l7,dt));

        
        if (ball_l7.x<(Playground_center.x - Playground_width/2 + radius_ball) || ball_l7.x >(Playground_center.x + Playground_width/2 - radius_ball)) 
        {
            speed_l7.x = - speed_l7.x;
            if (ball_l7.x <(Playground_center.x - Playground_width/2 + radius_ball)) ball_l7.x = (Playground_center.x - Playground_width/2 + radius_ball);
            if (ball_l7.x >(Playground_center.x + Playground_width/2 - radius_ball)) ball_l7.x = (Playground_center.x + Playground_width/2 - radius_ball);
        } 
        if (ball_l7.y<(Playground_center.y - Playground_height/2 + radius_ball) || ball_l7.y >(Playground_center.y + Playground_height/2 - radius_ball)) 
        {
            speed_l7.y = - speed_l7.y;
            if (ball_l7.y< (Playground_center.y - Playground_height/2 + radius_ball)) ball_l7.y = (Playground_center.y - Playground_height/2 + radius_ball);
            if (ball_l7.y >(Playground_center.y + Playground_height/2 - radius_ball)) ball_l7.y = (Playground_center.y + Playground_height/2 - radius_ball);
        } 
        


        //collision
        if (CheckCollisionCircleRec(ball_l7,radius_ball,obstacle_top_l7))
        {
            Vector2 collision_point;
            collision_point.x = Clamp(ball_l7.x,(Playground_center.x - Playground_width/2 +3),(Playground_center.x - Playground_width/2 + 3 + obstacle_width_l7));
            collision_point.y = Clamp(ball_l7.y,(Playground_center.y - Playground_height/2 + Playground_height/3),(Playground_center.y - Playground_height/2 + Playground_height/3 + obstacle_height_l7));
            Vector2 normal = Vector2Normalize(Vector2Subtract(ball_l7,collision_point));
            speed_l7 = Vector2Reflect(speed_l7,normal);
            ball_l7 = Vector2Add(collision_point,Vector2Scale(normal,radius_ball));
        }

        if (CheckCollisionCircleRec(ball_l7,radius_ball,obstacle_bottom_l7))
        {
            Vector2 collision_point;
            collision_point.x = Clamp(ball_l7.x,(Playground_center.x - Playground_width/2 + Playground_width/3 -1),(Playground_center.x - Playground_width/2 + Playground_width/3 -1 + obstacle_width_l7));
            collision_point.y = Clamp(ball_l7.y,(Playground_center.y - Playground_height/2 + Playground_height*2/3),(Playground_center.y - Playground_height/2 + Playground_height*2/3 + obstacle_height_l7));
            Vector2 normal = Vector2Normalize(Vector2Subtract(ball_l7,collision_point));
            speed_l7 = Vector2Reflect(speed_l7,normal);
            ball_l7 = Vector2Add(collision_point,Vector2Scale(normal,radius_ball));
        }

        //proportional deceleration
        float deceleration=100.00;
        if (fabsf(speed_l7.x)>=fabsf(speed_l7.y) && speed_l7.x!=0)
        {
            if (speed_l7.y>0) speed_l7.y-=deceleration*dt*fabsf(speed_l7.y/speed_l7.x);
            if (speed_l7.y<0) speed_l7.y+=deceleration*dt*fabsf(speed_l7.y/speed_l7.x);
            if (speed_l7.x>0) speed_l7.x-=deceleration*dt;
            if (speed_l7.x<0) speed_l7.x+=deceleration*dt;
        }
        else if (fabsf(speed_l7.x)<fabsf(speed_l7.y) && speed_l7.y!=0)
        {
            if (speed_l7.x>0) speed_l7.x-=deceleration*dt*fabsf(speed_l7.x/speed_l7.y);
            if (speed_l7.x<0) speed_l7.x+=deceleration*dt*fabsf(speed_l7.x/speed_l7.y);
            if (speed_l7.y>0) speed_l7.y-=deceleration*dt;
            if (speed_l7.y<0) speed_l7.y+=deceleration*dt;
        }

        if (speed_l7.x<5 && speed_l7.x>-5) speed_l7.x=0;
        if (speed_l7.y<5 && speed_l7.y>-5) speed_l7.y=0;
        
        //shooting
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && speed_l7.x==0 && speed_l7.y==0)
        {
            int mouse_x=GetMouseX();
            int mouse_y=GetMouseY();
            int drag_x=ball_l7.x-mouse_x;
            int drag_y=ball_l7.y-mouse_y;
            speed_l7.x=drag_x*2;
            if (fabsf(speed_l7.x)>max_speed) speed_l7.x = speed_l7.x / fabsf(speed_l7.x) * max_speed;
            speed_l7.y=drag_y*2;
            if (fabsf(speed_l7.y)>max_speed) speed_l7.y = speed_l7.y / fabsf(speed_l7.y) * max_speed;
        }
        
        //score
        if ((ball_l7.x>pot.x-3*radius_pot/4) && (ball_l7.x<pot.x+3*radius_pot/4) && (ball_l7.y>pot.y-3*radius_pot/4) && (ball_l7.y<pot.y+3*radius_pot/4))
        {   
            ball_l7.x=width/2;
            ball_l7.y=4*height/5;
            speed_l7.x=0;
            speed_l7.y= 0;
        }


        BeginDrawing();
        ClearBackground(SKYBLUE);
        
        DrawRectangleRec(Playground,GetColor(0x00F000FF));
        DrawCircle(pot.x,pot.y,radius_pot,BLACK);
        DrawCircle(ball_l7.x,ball_l7.y,radius_ball,WHITE);
        DrawRectangleLinesEx(Playground,5,BLACK);
        DrawCircleLines(pot.x,pot.y,radius_pot+1,WHITE);
        DrawCircleLines(ball_l7.x,ball_l7.y,radius_ball+1,BLACK);
        //DrawRectangleRec(obstacle,RED);
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && speed_l7.x==0 && speed_l7.y==0)
        {
            DrawLine(ball_l7.x, ball_l7.y, GetMouseX(), GetMouseY(), BLACK);
        }
        //DrawRectangleLines(width/2-obstacle_edge/2,height/2-obstacle_edge/2,obstacle_edge,obstacle_edge,BLACK);
                       
        DrawRectangleRec(obstacle_top_l7,RED);
        DrawRectangleRec(obstacle_bottom_l7,RED);
        DrawRectangleLinesEx(obstacle_top_l7,3,BLACK);
        DrawRectangleLinesEx(obstacle_bottom_l7,3,BLACK);
        EndDrawing();
    }
    CloseWindow();


    return 0;
}