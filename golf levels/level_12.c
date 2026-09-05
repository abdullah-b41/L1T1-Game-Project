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

Vector2 ball_l12 = {width/2,4*height/5};
Vector2 speed_l12 = {0,0};
# define obstacle_width_l12 90
# define obstacle_height_l12 15
int obstacle_top_speed_l12 = 70;
int obstacle_bottom_speed_l12 = -70;
float obstacle_top_position_x_l12 = (width/2 - width/8 +3);
float obstacle_bottom_position_x_l12 = (width/2 + width/8 -obstacle_width_l12 -3);
int deccelerated_area_width_l12 = 46;
int deccelerated_area_center_l12;
int deccelerated_area_decceleration_l12 = 1000;

int main()
{   
    Rectangle Playground = {(Playground_center.x - Playground_width/2),(Playground_center.y - Playground_height/2),Playground_width,Playground_height};
    Rectangle obstacle_top_l12 = {obstacle_top_position_x_l12,  (Playground_center.y - Playground_height/2 + Playground_height/3), obstacle_width_l12, obstacle_height_l12};
    Rectangle obstacle_bottom_l12 = {obstacle_bottom_position_x_l12,  (Playground_center.y + Playground_height/2 - Playground_height/3), obstacle_width_l12, obstacle_height_l12};


    InitWindow(width,height,"practice");
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {   
        float dt = GetFrameTime();
        ball_l12 = Vector2Add(ball_l12,Vector2Scale(speed_l12,dt));

        //wall bounce
        if (ball_l12.x<(Playground_center.x - Playground_width/2 + radius_ball) || ball_l12.x >(Playground_center.x + Playground_width/2 - radius_ball)) 
        {
            speed_l12.x = - speed_l12.x;
            if (ball_l12.x <(Playground_center.x - Playground_width/2 + radius_ball)) ball_l12.x = (Playground_center.x - Playground_width/2 + radius_ball);
            if (ball_l12.x >(Playground_center.x + Playground_width/2 - radius_ball)) ball_l12.x = (Playground_center.x + Playground_width/2 - radius_ball);
        } 
        if (ball_l12.y<(Playground_center.y - Playground_height/2 + radius_ball) || ball_l12.y >(Playground_center.y + Playground_height/2 - radius_ball)) 
        {
            speed_l12.y = - speed_l12.y;
            if (ball_l12.y< (Playground_center.y - Playground_height/2 + radius_ball)) ball_l12.y = (Playground_center.y - Playground_height/2 + radius_ball);
            if (ball_l12.y >(Playground_center.y + Playground_height/2 - radius_ball)) ball_l12.y = (Playground_center.y + Playground_height/2 - radius_ball);
        } 
        


        //collision
        if (CheckCollisionCircleRec(ball_l12,radius_ball,obstacle_top_l12))
        {
            Vector2 collision_point;
            collision_point.x = Clamp(ball_l12.x,obstacle_top_position_x_l12,(obstacle_top_position_x_l12 + obstacle_width_l12));
            collision_point.y = Clamp(ball_l12.y,(Playground_center.y - Playground_height/2 + Playground_height/3),(Playground_center.y - Playground_height/2 + Playground_height/3 + obstacle_height_l12));
            Vector2 normal = Vector2Normalize(Vector2Subtract(ball_l12,collision_point));
            speed_l12 = Vector2Reflect(speed_l12,normal);
            ball_l12 = Vector2Add(collision_point,Vector2Scale(normal,radius_ball));
        }

        if (CheckCollisionCircleRec(ball_l12,radius_ball,obstacle_bottom_l12))
        {
            Vector2 collision_point;
            collision_point.x = Clamp(ball_l12.x,obstacle_bottom_position_x_l12,(obstacle_bottom_position_x_l12 + obstacle_width_l12));
            collision_point.y = Clamp(ball_l12.y,(Playground_center.y - Playground_height/2 + Playground_height*2/3),(Playground_center.y - Playground_height/2 + Playground_height*2/3 + obstacle_height_l12));
            Vector2 normal = Vector2Normalize(Vector2Subtract(ball_l12,collision_point));
            speed_l12 = Vector2Reflect(speed_l12,normal);
            ball_l12 = Vector2Add(collision_point,Vector2Scale(normal,radius_ball));
        }

        //obstacle moving
        obstacle_top_position_x_l12 = obstacle_top_position_x_l12 + obstacle_top_speed_l12*dt;
        if (obstacle_top_position_x_l12 >= (Playground_center.x + Playground_width/2 - obstacle_width_l12 -1) || obstacle_top_position_x_l12 < (Playground_center.x - Playground_width/2)) obstacle_top_speed_l12 = -obstacle_top_speed_l12;        obstacle_top_l12.x = obstacle_top_position_x_l12;

        obstacle_bottom_position_x_l12 = obstacle_bottom_position_x_l12 + obstacle_bottom_speed_l12*dt;
        if (obstacle_bottom_position_x_l12 >= (Playground_center.x + Playground_width/2 - obstacle_width_l12 -1) || obstacle_bottom_position_x_l12 < (Playground_center.x - Playground_width/2)) obstacle_bottom_speed_l12 = -obstacle_bottom_speed_l12;        obstacle_bottom_l12.x = obstacle_bottom_position_x_l12;


        //proportional deceleration
        float deceleration=100.00;
        if (fabsf(speed_l12.x)>=fabsf(speed_l12.y) && speed_l12.x!=0)
        {
            if (speed_l12.y>0) speed_l12.y-=deceleration*dt*fabsf(speed_l12.y/speed_l12.x);
            if (speed_l12.y<0) speed_l12.y+=deceleration*dt*fabsf(speed_l12.y/speed_l12.x);
            if (speed_l12.x>0) speed_l12.x-=deceleration*dt;
            if (speed_l12.x<0) speed_l12.x+=deceleration*dt;
        }
        else if (fabsf(speed_l12.x)<fabsf(speed_l12.y) && speed_l12.y!=0)
        {
            if (speed_l12.x>0) speed_l12.x-=deceleration*dt*fabsf(speed_l12.x/speed_l12.y);
            if (speed_l12.x<0) speed_l12.x+=deceleration*dt*fabsf(speed_l12.x/speed_l12.y);
            if (speed_l12.y>0) speed_l12.y-=deceleration*dt;
            if (speed_l12.y<0) speed_l12.y+=deceleration*dt;
        }

        if (speed_l12.x<5 && speed_l12.x>-5) speed_l12.x=0;
        if (speed_l12.y<5 && speed_l12.y>-5) speed_l12.y=0;

        //areal decceleration
        deccelerated_area_center_l12 = Playground_center.y;
        if (ball_l12.y > deccelerated_area_center_l12 - deccelerated_area_width_l12/2 && ball_l12.y < deccelerated_area_center_l12 + deccelerated_area_width_l12/2)
        speed_l12.y = speed_l12.y + deccelerated_area_decceleration_l12 * dt;
        
        //shooting
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && speed_l12.x==0 && speed_l12.y==0)
        {
            int mouse_x=GetMouseX();
            int mouse_y=GetMouseY();
            int drag_x=ball_l12.x-mouse_x;
            int drag_y=ball_l12.y-mouse_y;
            speed_l12.x=drag_x*2;
            if (fabsf(speed_l12.x)>max_speed) speed_l12.x = speed_l12.x / fabsf(speed_l12.x) * max_speed;
            speed_l12.y=drag_y*2;
            if (fabsf(speed_l12.y)>max_speed) speed_l12.y = speed_l12.y / fabsf(speed_l12.y) * max_speed;
        }
        
        //score
        if ((ball_l12.x>pot.x-3*radius_pot/4) && (ball_l12.x<pot.x+3*radius_pot/4) && (ball_l12.y>pot.y-3*radius_pot/4) && (ball_l12.y<pot.y+3*radius_pot/4))
        {   
            ball_l12.x=width/2;
            ball_l12.y=4*height/5;
            speed_l12.x=0;
            speed_l12.y= 0;
        }


        BeginDrawing();
        ClearBackground(SKYBLUE);
        
        DrawRectangleRec(Playground,GetColor(0x00F000FF));
        DrawRectangle(Playground_center.x - Playground_width/2 + 4, deccelerated_area_center_l12 - deccelerated_area_width_l12/2, Playground_width - 8, deccelerated_area_width_l12, RED);
        DrawRectangleLines(Playground_center.x - Playground_width/2 + 4, deccelerated_area_center_l12 - deccelerated_area_width_l12/2, Playground_width - 8, deccelerated_area_width_l12, BLACK);
        Vector2 arrowdown1 = {Playground_center.x - Playground_width/2 + Playground_width/4 , deccelerated_area_center_l12};
        Vector2 arrowdown2 = {Playground_center.x - Playground_width/2 + 2*Playground_width/4 , deccelerated_area_center_l12};
        Vector2 arrowdown3 = {Playground_center.x - Playground_width/2 + 3*Playground_width/4 , deccelerated_area_center_l12};
        DrawPoly( arrowdown1 , 3 , 7, 90, BLACK);
        DrawPoly( arrowdown2 , 3 , 7, 90, BLACK);
        DrawPoly( arrowdown3 , 3 , 7, 90, BLACK);
        DrawCircle(pot.x,pot.y,radius_pot,BLACK);
        DrawCircle(ball_l12.x,ball_l12.y,radius_ball,WHITE);
        DrawRectangleLinesEx(Playground,5,BLACK);
        DrawCircleLines(pot.x,pot.y,radius_pot+1,WHITE);
        DrawCircleLines(ball_l12.x,ball_l12.y,radius_ball+1,BLACK);
        
        //DrawRectangleRec(obstacle,RED);
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && speed_l12.x==0 && speed_l12.y==0)
        {
            DrawLine(ball_l12.x, ball_l12.y, GetMouseX(), GetMouseY(), BLACK);
        }
        //DrawRectangleLines(width/2-obstacle_edge/2,height/2-obstacle_edge/2,obstacle_edge,obstacle_edge,BLACK);
                       
        DrawRectangleRec(obstacle_top_l12,RED);
        DrawRectangleRec(obstacle_bottom_l12,RED);
        DrawRectangleLinesEx(obstacle_top_l12,3,BLACK);
        DrawRectangleLinesEx(obstacle_bottom_l12,3,BLACK);

        EndDrawing();
    }
    CloseWindow();


    return 0;
}