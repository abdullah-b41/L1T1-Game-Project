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

Vector2 ball_l11 = {width/2,4*height/5};
Vector2 speed_l11 = {0,0};
# define obstacle_width_l11 90
# define obstacle_height_l11 15
int obstacle_top_speed_l11 = 70;
int obstacle_bottom_speed_l11 = -70;
float obstacle_top_position_x_l11 = (width/2 - width/8 +3);
float obstacle_bottom_position_x_l11 = (width/2 + width/8 -obstacle_width_l11 -3);



int main()
{   
    Rectangle Playground = {(Playground_center.x - Playground_width/2),(Playground_center.y - Playground_height/2),Playground_width,Playground_height};
    Rectangle obstacle_top_l11 = {obstacle_top_position_x_l11,  (Playground_center.y - Playground_height/2 + Playground_height/3), obstacle_width_l11, obstacle_height_l11};
    Rectangle obstacle_bottom_l11 = {obstacle_bottom_position_x_l11,  (Playground_center.y + Playground_height/2 - Playground_height/3), obstacle_width_l11, obstacle_height_l11};


    InitWindow(width,height,"practice");
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {   
        float dt = GetFrameTime();
        ball_l11 = Vector2Add(ball_l11,Vector2Scale(speed_l11,dt));

        //wall bounce
        if (ball_l11.x<(Playground_center.x - Playground_width/2 + radius_ball) || ball_l11.x >(Playground_center.x + Playground_width/2 - radius_ball)) 
        {
            speed_l11.x = - speed_l11.x;
            if (ball_l11.x <(Playground_center.x - Playground_width/2 + radius_ball)) ball_l11.x = (Playground_center.x - Playground_width/2 + radius_ball);
            if (ball_l11.x >(Playground_center.x + Playground_width/2 - radius_ball)) ball_l11.x = (Playground_center.x + Playground_width/2 - radius_ball);
        } 
        if (ball_l11.y<(Playground_center.y - Playground_height/2 + radius_ball) || ball_l11.y >(Playground_center.y + Playground_height/2 - radius_ball)) 
        {
            speed_l11.y = - speed_l11.y;
            if (ball_l11.y< (Playground_center.y - Playground_height/2 + radius_ball)) ball_l11.y = (Playground_center.y - Playground_height/2 + radius_ball);
            if (ball_l11.y >(Playground_center.y + Playground_height/2 - radius_ball)) ball_l11.y = (Playground_center.y + Playground_height/2 - radius_ball);
        } 
        


        //collision
        if (CheckCollisionCircleRec(ball_l11,radius_ball,obstacle_top_l11))
        {
            Vector2 collision_point;
            collision_point.x = Clamp(ball_l11.x,obstacle_top_position_x_l11,(obstacle_top_position_x_l11 + obstacle_width_l11));
            collision_point.y = Clamp(ball_l11.y,(Playground_center.y - Playground_height/2 + Playground_height/3),(Playground_center.y - Playground_height/2 + Playground_height/3 + obstacle_height_l11));
            Vector2 normal = Vector2Normalize(Vector2Subtract(ball_l11,collision_point));
            speed_l11 = Vector2Reflect(speed_l11,normal);
            ball_l11 = Vector2Add(collision_point,Vector2Scale(normal,radius_ball));
        }

        if (CheckCollisionCircleRec(ball_l11,radius_ball,obstacle_bottom_l11))
        {
            Vector2 collision_point;
            collision_point.x = Clamp(ball_l11.x,obstacle_bottom_position_x_l11,(obstacle_bottom_position_x_l11 + obstacle_width_l11));
            collision_point.y = Clamp(ball_l11.y,(Playground_center.y - Playground_height/2 + Playground_height*2/3),(Playground_center.y - Playground_height/2 + Playground_height*2/3 + obstacle_height_l11));
            Vector2 normal = Vector2Normalize(Vector2Subtract(ball_l11,collision_point));
            speed_l11 = Vector2Reflect(speed_l11,normal);
            ball_l11 = Vector2Add(collision_point,Vector2Scale(normal,radius_ball));
        }

        //obstacle moving
        obstacle_top_position_x_l11 = obstacle_top_position_x_l11 + obstacle_top_speed_l11*dt;
        if (obstacle_top_position_x_l11 >= (Playground_center.x + Playground_width/2 - obstacle_width_l11 -1) || obstacle_top_position_x_l11 < (Playground_center.x - Playground_width/2)) obstacle_top_speed_l11 = -obstacle_top_speed_l11;        obstacle_top_l11.x = obstacle_top_position_x_l11;

        obstacle_bottom_position_x_l11 = obstacle_bottom_position_x_l11 + obstacle_bottom_speed_l11*dt;
        if (obstacle_bottom_position_x_l11 >= (Playground_center.x + Playground_width/2 - obstacle_width_l11 -1) || obstacle_bottom_position_x_l11 < (Playground_center.x - Playground_width/2)) obstacle_bottom_speed_l11 = -obstacle_bottom_speed_l11;        obstacle_bottom_l11.x = obstacle_bottom_position_x_l11;


        //proportional deceleration
        float deceleration=100.00;
        if (fabsf(speed_l11.x)>=fabsf(speed_l11.y) && speed_l11.x!=0)
        {
            if (speed_l11.y>0) speed_l11.y-=deceleration*dt*fabsf(speed_l11.y/speed_l11.x);
            if (speed_l11.y<0) speed_l11.y+=deceleration*dt*fabsf(speed_l11.y/speed_l11.x);
            if (speed_l11.x>0) speed_l11.x-=deceleration*dt;
            if (speed_l11.x<0) speed_l11.x+=deceleration*dt;
        }
        else if (fabsf(speed_l11.x)<fabsf(speed_l11.y) && speed_l11.y!=0)
        {
            if (speed_l11.x>0) speed_l11.x-=deceleration*dt*fabsf(speed_l11.x/speed_l11.y);
            if (speed_l11.x<0) speed_l11.x+=deceleration*dt*fabsf(speed_l11.x/speed_l11.y);
            if (speed_l11.y>0) speed_l11.y-=deceleration*dt;
            if (speed_l11.y<0) speed_l11.y+=deceleration*dt;
        }

        if (speed_l11.x<5 && speed_l11.x>-5) speed_l11.x=0;
        if (speed_l11.y<5 && speed_l11.y>-5) speed_l11.y=0;
        
        //shooting
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && speed_l11.x==0 && speed_l11.y==0)
        {
            int mouse_x=GetMouseX();
            int mouse_y=GetMouseY();
            int drag_x=ball_l11.x-mouse_x;
            int drag_y=ball_l11.y-mouse_y;
            speed_l11.x=drag_x*2;
            if (fabsf(speed_l11.x)>max_speed) speed_l11.x = speed_l11.x / fabsf(speed_l11.x) * max_speed;
            speed_l11.y=drag_y*2;
            if (fabsf(speed_l11.y)>max_speed) speed_l11.y = speed_l11.y / fabsf(speed_l11.y) * max_speed;
        }
        
        //score
        if ((ball_l11.x>pot.x-3*radius_pot/4) && (ball_l11.x<pot.x+3*radius_pot/4) && (ball_l11.y>pot.y-3*radius_pot/4) && (ball_l11.y<pot.y+3*radius_pot/4))
        {   
            ball_l11.x=width/2;
            ball_l11.y=4*height/5;
            speed_l11.x=0;
            speed_l11.y= 0;
        }


        BeginDrawing();
        ClearBackground(SKYBLUE);
        
        DrawRectangleRec(Playground,GetColor(0x00F000FF));
        DrawCircle(pot.x,pot.y,radius_pot,BLACK);
        DrawCircle(ball_l11.x,ball_l11.y,radius_ball,WHITE);
        DrawRectangleLinesEx(Playground,5,BLACK);
        DrawCircleLines(pot.x,pot.y,radius_pot+1,WHITE);
        DrawCircleLines(ball_l11.x,ball_l11.y,radius_ball+1,BLACK);
        //DrawRectangleRec(obstacle,RED);
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && speed_l11.x==0 && speed_l11.y==0)
        {
            DrawLine(ball_l11.x, ball_l11.y, GetMouseX(), GetMouseY(), BLACK);
        }
        //DrawRectangleLines(width/2-obstacle_edge/2,height/2-obstacle_edge/2,obstacle_edge,obstacle_edge,BLACK);
                       
        DrawRectangleRec(obstacle_top_l11,RED);
        DrawRectangleRec(obstacle_bottom_l11,RED);
        DrawRectangleLinesEx(obstacle_top_l11,3,BLACK);
        DrawRectangleLinesEx(obstacle_bottom_l11,3,BLACK);

        EndDrawing();
    }
    CloseWindow();


    return 0;
}