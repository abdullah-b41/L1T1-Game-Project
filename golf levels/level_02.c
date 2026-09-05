#include "raylib.h"
#include "raymath.h"

#define width 800
#define height 800
int Playground_width=width/4;
int Playground_height=height*3/4;
Vector2 Playground_center = {width/2,height/2};
Vector2 pot = {width/2,height/5};
int radius_pot = height/60;
int radius_ball = height/75;

Vector2 ball_l2 = {width/2,4*height/5};
Vector2 speed_l2 = {0,0};
Vector2 obstacle_l2 = {width/2,height/2};
int radius_obstackle_l2 = 25;
int deccelerated_area_width_l2 = 60;
int deccelerated_area_center_l2;
int deccelerated_area_decceleration_l2 = 1000;


int main()
{   
    InitWindow(width,height,"practice");
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {   
        //wall bounce
        float dt = GetFrameTime();
        ball_l2 = Vector2Add(ball_l2,Vector2Scale(speed_l2,dt));
        if (ball_l2.x<(Playground_center.x - Playground_width/2 + radius_ball) || ball_l2.x >(Playground_center.x + Playground_width/2 - radius_ball)) 
        {
            speed_l2.x = - speed_l2.x;
            if (ball_l2.x <(Playground_center.x - Playground_width/2 + radius_ball)) ball_l2.x = (Playground_center.x - Playground_width/2 + radius_ball);
            if (ball_l2.x >(Playground_center.x + Playground_width/2 - radius_ball)) ball_l2.x = (Playground_center.x + Playground_width/2 - radius_ball);
        } 
        if (ball_l2.y<(Playground_center.y - Playground_height/2 + radius_ball) || ball_l2.y >(Playground_center.y + Playground_height/2 - radius_ball)) 
        {
            speed_l2.y = - speed_l2.y;
            if (ball_l2.y< (Playground_center.y - Playground_height/2 + radius_ball)) ball_l2.y = (Playground_center.y - Playground_height/2 + radius_ball);
            if (ball_l2.y >(Playground_center.y + Playground_height/2 - radius_ball)) ball_l2.y = (Playground_center.y + Playground_height/2 - radius_ball);
        }

        //collision
        if(CheckCollisionCircles(ball_l2,radius_ball,obstacle_l2,radius_obstackle_l2))
        {   
            Vector2 normal= Vector2Normalize(Vector2Subtract(ball_l2,obstacle_l2));
            speed_l2 = Vector2Reflect(speed_l2,normal);
        }

        //proportional deceleration
        float deceleration=100.00;
        if (fabsf(speed_l2.x)>=fabsf(speed_l2.y) && speed_l2.x!=0)
        {
            if (speed_l2.y>0) speed_l2.y-=deceleration*dt*fabsf(speed_l2.y/speed_l2.x);
            if (speed_l2.y<0) speed_l2.y+=deceleration*dt*fabsf(speed_l2.y/speed_l2.x);
            if (speed_l2.x>0) speed_l2.x-=deceleration*dt;
            if (speed_l2.x<0) speed_l2.x+=deceleration*dt;
        }
        else if (fabsf(speed_l2.x)<fabsf(speed_l2.y) && speed_l2.y!=0)
        {
            if (speed_l2.x>0) speed_l2.x-=deceleration*dt*fabsf(speed_l2.x/speed_l2.y);
            if (speed_l2.x<0) speed_l2.y-=deceleration*dt;
            if (speed_l2.y<0) speed_l2.y+=deceleration*dt;
        }

        if (speed_l2.x<5 && speed_l2.x>-5) speed_l2.x=0;
        if (speed_l2.y<5 && speed_l2.y>-5) speed_l2.y=0;


        //areal decceleration
        deccelerated_area_center_l2 = Playground_center.y - Playground_height/6;
        if (ball_l2.y > deccelerated_area_center_l2 - deccelerated_area_width_l2/2 && ball_l2.y < deccelerated_area_center_l2 + deccelerated_area_width_l2/2)
        speed_l2.y = speed_l2.y + deccelerated_area_decceleration_l2 * dt;
        
        //shooting
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && speed_l2.x==0 && speed_l2.y==0)
        {
            int mouse_x=GetMouseX();
            int mouse_y=GetMouseY();
            int drag_x=ball_l2.x-mouse_x;
            int drag_y=ball_l2.y-mouse_y;
            speed_l2.x=drag_x*2;
            speed_l2.y=drag_y*2;
        }
        
        //score
        if ((ball_l2.x>pot.x-3*radius_pot/4) && (ball_l2.x<pot.x+3*radius_pot/4) && (ball_l2.y>pot.y-3*radius_pot/4) && (ball_l2.y<pot.y+3*radius_pot/4))
        {   //score
            ball_l2.x=width/2;
            ball_l2.y=4*height/5;
            speed_l2.x=0;
            speed_l2.y= 0;
        }


        BeginDrawing();
        ClearBackground(SKYBLUE);

        Rectangle Playground = {(Playground_center.x - Playground_width/2),(Playground_center.y - Playground_height/2),Playground_width,Playground_height};
        DrawRectangleRec(Playground,GetColor(0x00F000FF));
        DrawRectangle(Playground_center.x - Playground_width/2, deccelerated_area_center_l2 - deccelerated_area_width_l2/2, Playground_width, deccelerated_area_width_l2, RED);
        DrawRectangleLines(Playground_center.x - Playground_width/2, deccelerated_area_center_l2 - deccelerated_area_width_l2/2, Playground_width, deccelerated_area_width_l2, BLACK);
        Vector2 arrowdown1 = {Playground_center.x - Playground_width/2 + Playground_width/4 , deccelerated_area_center_l2};
        Vector2 arrowdown2 = {Playground_center.x - Playground_width/2 + 2*Playground_width/4 , deccelerated_area_center_l2};
        Vector2 arrowdown3 = {Playground_center.x - Playground_width/2 + 3*Playground_width/4 , deccelerated_area_center_l2};
        DrawPoly( arrowdown1 , 3 , 7, 90, BLACK);
        DrawPoly( arrowdown2 , 3 , 7, 90, BLACK);
        DrawPoly( arrowdown3 , 3 , 7, 90, BLACK);
        DrawCircle(pot.x,pot.y,radius_pot,BLACK);
        DrawCircle(ball_l2.x,ball_l2.y,radius_ball,WHITE);
        DrawRectangleLinesEx(Playground,5,BLACK);
        DrawCircleLines(pot.x,pot.y,radius_pot+1,YELLOW);
        DrawCircleLines(ball_l2.x,ball_l2.y,radius_ball+1,BLACK);
        DrawCircle(obstacle_l2.x,obstacle_l2.y,radius_obstackle_l2,RED);
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && speed_l2.x==0 && speed_l2.y==0)
        {
            DrawLine(ball_l2.x, ball_l2.y, GetMouseX(), GetMouseY(), BLACK);
        }

        EndDrawing();
    }
    CloseWindow();


    return 0;
}