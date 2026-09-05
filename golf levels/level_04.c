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


Vector2 ball_l4 = {width/2,4*height/5};
Vector2 speed_l4 = {0,0};
#define obstacle_edge_l4 50
Rectangle obstacle_l4 = {(width/2-obstacle_edge_l4/2),(height/2-obstacle_edge_l4/2),50,50};
int accelerated_area_width_l4 = 60;
int accelerated_area_center_l4;
int accelerated_area_acceleration_l4 = 1000;

int main()
{   
    InitWindow(width,height,"practice");
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {   
        //wall bounce
        float dt = GetFrameTime();
        ball_l4 = Vector2Add(ball_l4,Vector2Scale(speed_l4,dt));
        if (ball_l4.x<(Playground_center.x - Playground_width/2 + radius_ball) || ball_l4.x >(Playground_center.x + Playground_width/2 - radius_ball)) 
        {
            speed_l4.x = - speed_l4.x;
            if (ball_l4.x <(Playground_center.x - Playground_width/2 + radius_ball)) ball_l4.x = (Playground_center.x - Playground_width/2 + radius_ball);
            if (ball_l4.x >(Playground_center.x + Playground_width/2 - radius_ball)) ball_l4.x = (Playground_center.x + Playground_width/2 - radius_ball);
        } 
        if (ball_l4.y<(Playground_center.y - Playground_height/2 + radius_ball) || ball_l4.y >(Playground_center.y + Playground_height/2 - radius_ball)) 
        {
            speed_l4.y = - speed_l4.y;
            if (ball_l4.y< (Playground_center.y - Playground_height/2 + radius_ball)) ball_l4.y = (Playground_center.y - Playground_height/2 + radius_ball);
            if (ball_l4.y >(Playground_center.y + Playground_height/2 - radius_ball)) ball_l4.y = (Playground_center.y + Playground_height/2 - radius_ball);
        } 
        


        //collision
        if (CheckCollisionCircleRec(ball_l4,radius_ball,obstacle_l4))
        {
            Vector2 collision_point;
            collision_point.x = Clamp(ball_l4.x,width/2-obstacle_edge_l4/2,width/2+obstacle_edge_l4/2);
            collision_point.y = Clamp(ball_l4.y,height/2-obstacle_edge_l4/2,height/2+obstacle_edge_l4/2);
            Vector2 normal = Vector2Normalize(Vector2Subtract(ball_l4,collision_point));
            speed_l4 = Vector2Reflect(speed_l4,normal);
            ball_l4 = Vector2Add(collision_point,Vector2Scale(normal,radius_ball));
        }

        //proportional deceleration
        float deceleration=100.00;
        if (fabsf(speed_l4.x)>=fabsf(speed_l4.y) && speed_l4.x!=0)
        {
            if (speed_l4.y>0) speed_l4.y-=deceleration*dt*fabsf(speed_l4.y/speed_l4.x);
            if (speed_l4.y<0) speed_l4.y+=deceleration*dt*fabsf(speed_l4.y/speed_l4.x);
            if (speed_l4.x>0) speed_l4.x-=deceleration*dt;
            if (speed_l4.x<0) speed_l4.x+=deceleration*dt;
        }
        else if (fabsf(speed_l4.x)<fabsf(speed_l4.y) && speed_l4.y!=0)
        {
            if (speed_l4.x>0) speed_l4.x-=deceleration*dt*fabsf(speed_l4.x/speed_l4.y);
            if (speed_l4.x<0) speed_l4.x+=deceleration*dt*fabsf(speed_l4.x/speed_l4.y);
            if (speed_l4.y>0) speed_l4.y-=deceleration*dt;
            if (speed_l4.y<0) speed_l4.y+=deceleration*dt;
        }

        if (speed_l4.x<5 && speed_l4.x>-5) speed_l4.x=0;
        if (speed_l4.y<5 && speed_l4.y>-5) speed_l4.y=0;

        //areal acceleration
        accelerated_area_center_l4 = Playground_center.y + Playground_height/6;
        if (ball_l4.y > accelerated_area_center_l4 - accelerated_area_width_l4/2 && ball_l4.y < accelerated_area_center_l4 + accelerated_area_width_l4/2)
        speed_l4.y = speed_l4.y - accelerated_area_acceleration_l4 * dt;
        
        //shooting
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && speed_l4.x==0 && speed_l4.y==0)
        {
            int mouse_x=GetMouseX();
            int mouse_y=GetMouseY();
            int drag_x=ball_l4.x-mouse_x;
            int drag_y=ball_l4.y-mouse_y;
            speed_l4.x=drag_x*2;
            if (fabsf(speed_l4.x)>max_speed) speed_l4.x = speed_l4.x / fabsf(speed_l4.x) * max_speed;
            speed_l4.y=drag_y*2;
            if (fabsf(speed_l4.y)>max_speed) speed_l4.y = speed_l4.y / fabsf(speed_l4.y) * max_speed;
        }
        
        //score
        if ((ball_l4.x>pot.x-3*radius_pot/4) && (ball_l4.x<pot.x+3*radius_pot/4) && (ball_l4.y>pot.y-3*radius_pot/4) && (ball_l4.y<pot.y+3*radius_pot/4))
        {   
            ball_l4.x=width/2;
            ball_l4.y=4*height/5;
            speed_l4.x=0;
            speed_l4.y= 0;
        }


        BeginDrawing();
        ClearBackground(SKYBLUE);

        Rectangle Playground = {(Playground_center.x - Playground_width/2),(Playground_center.y - Playground_height/2),Playground_width,Playground_height};
        DrawRectangleRec(Playground,GetColor(0x00F000FF));
        DrawRectangle(Playground_center.x - Playground_width/2, accelerated_area_center_l4 - accelerated_area_width_l4/2, Playground_width, accelerated_area_width_l4, RED);
        DrawRectangleLines(Playground_center.x - Playground_width/2, accelerated_area_center_l4 - accelerated_area_width_l4/2, Playground_width, accelerated_area_width_l4, BLACK);
        Vector2 arrow1 = {Playground_center.x - Playground_width/2 + Playground_width/4 , accelerated_area_center_l4};
        Vector2 arrow2 = {Playground_center.x - Playground_width/2 + 2*Playground_width/4 , accelerated_area_center_l4};
        Vector2 arrow3 = {Playground_center.x - Playground_width/2 + 3*Playground_width/4 , accelerated_area_center_l4};
        DrawPoly( arrow1 , 3 , 7, 270, BLACK);
        DrawPoly( arrow2 , 3 , 7, 270, BLACK);
        DrawPoly( arrow3 , 3 , 7, 270, BLACK);
        DrawCircle(pot.x,pot.y,radius_pot,BLACK);
        DrawCircle(ball_l4.x,ball_l4.y,radius_ball,WHITE);
        DrawRectangleLinesEx(Playground,5,BLACK);
        DrawCircleLines(pot.x,pot.y,radius_pot+1,WHITE);
        DrawCircleLines(ball_l4.x,ball_l4.y,radius_ball+1,BLACK);
        DrawRectangleRec(obstacle_l4,RED);
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && speed_l4.x==0 && speed_l4.y==0)
        {
            DrawLine(ball_l4.x, ball_l4.y, GetMouseX(), GetMouseY(), BLACK);
        }
        DrawRectangleLines(width/2-obstacle_edge_l4/2,height/2-obstacle_edge_l4/2,obstacle_edge_l4,obstacle_edge_l4,BLACK);

        EndDrawing();
    }
    CloseWindow();


    return 0;
}