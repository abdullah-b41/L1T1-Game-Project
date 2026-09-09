#include "raylib.h"
#include "raymath.h"
#include <math.h>

#define width 800
#define height 800
int Playground_width = width / 4;
int Playground_height = height * 3 / 4;
Vector2 Playground_center = {width / 2, height / 2};
Vector2 pot = {width / 2, height / 5};
#define max_speed 400
int radius_pot = height / 60;
int radius_ball = height / 75;

Vector2 ball_l6 = {width / 2, 4 * height / 5};
Vector2 speed_l6 = {0, 0};
#define obstacle_edge_l6 50
Rectangle obstacle_l6 = {width / 2, height / 2, obstacle_edge_l6, obstacle_edge_l6};
Vector2 origin_l6 = {obstacle_edge_l6 / 2, obstacle_edge_l6 / 2};
int deccelerated_area_width_l6 = 60;
int deccelerated_area_center_l6;
int deccelerated_area_decceleration_l6 = 1000;
int accelerated_area_width_l6 = 60;
int accelerated_area_center_l6;
int accelerated_area_acceleration_L6 = 1000;

int main()
{
    InitWindow(width, height, "practice");
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        // wall bounce
        float dt = GetFrameTime();
        ball_l6 = Vector2Add(ball_l6, Vector2Scale(speed_l6, dt));
        if (ball_l6.x < (Playground_center.x - Playground_width / 2 + radius_ball) || ball_l6.x > (Playground_center.x + Playground_width / 2 - radius_ball))
        {
            speed_l6.x = -speed_l6.x;
            if (ball_l6.x < (Playground_center.x - Playground_width / 2 + radius_ball))
                ball_l6.x = (Playground_center.x - Playground_width / 2 + radius_ball);
            if (ball_l6.x > (Playground_center.x + Playground_width / 2 - radius_ball))
                ball_l6.x = (Playground_center.x + Playground_width / 2 - radius_ball);
        }
        if (ball_l6.y < (Playground_center.y - Playground_height / 2 + radius_ball) || ball_l6.y > (Playground_center.y + Playground_height / 2 - radius_ball))
        {
            speed_l6.y = -speed_l6.y;
            if (ball_l6.y < (Playground_center.y - Playground_height / 2 + radius_ball))
                ball_l6.y = (Playground_center.y - Playground_height / 2 + radius_ball);
            if (ball_l6.y > (Playground_center.y + Playground_height / 2 - radius_ball))
                ball_l6.y = (Playground_center.y + Playground_height / 2 - radius_ball);
        }

        // collision
        //  if (CheckCollisionCircleRec(ball_l6,radius_ball,obstacle_l6))
        //  {
        //      Vector2 collision_point;
        //      collision_point.x = Clamp(ball_l6.x,width/2-obstacle_edge_l6/2,width/2+obstacle_edge_l6/2);
        //      collision_point.y = Clamp(ball_l6.y,height/2-obstacle_edge_l6/2,height/2+obstacle_edge_l6/2);
        //      Vector2 normal = Vector2Normalize(Vector2Subtract(ball_l6,collision_point));
        //      speed_l6 = Vector2Reflect(speed_l6,normal);
        //      ball_l6 = Vector2Add(collision_point,Vector2Scale(normal,radius_ball));
        //  }

        // collision
        Vector2 normal;
        if (ball_l6.x >= width / 2 - (obstacle_edge_l6 / sqrt(2)) - radius_ball && ball_l6.x <= width / 2 + (obstacle_edge_l6 / sqrt(2)) + radius_ball)
        {
            float drift = obstacle_edge_l6 / sqrt(2) + radius_ball - fabsf(width / 2 - ball_l6.x);
            if (ball_l6.y >= height / 2 - drift && ball_l6.y <= height / 2 + drift)
            {
                if (ball_l6.x <= width / 2 && ball_l6.y <= height / 2)
                {
                    normal.x = -1 / sqrt(2);
                    normal.y = -1 / sqrt(2);
                }
                else if (ball_l6.x >= width / 2 && ball_l6.y <= height / 2)
                {
                    normal.x = 1 / sqrt(2);
                    normal.y = -1 / sqrt(2);
                }
                else if (ball_l6.x <= width / 2 && ball_l6.y >= height / 2)
                {
                    normal.x = -1 / sqrt(2);
                    normal.y = 1 / sqrt(2);
                }
                else if (ball_l6.x >= width / 2 && ball_l6.y >= height / 2)
                {
                    normal.x = 1 / sqrt(2);
                    normal.y = 1 / sqrt(2);
                }
                speed_l6 = Vector2Reflect(speed_l6, normal);
            }
        }

        // proportional deceleration
        float deceleration = 100.00;
        if (fabsf(speed_l6.x) >= fabsf(speed_l6.y) && speed_l6.x != 0)
        {
            if (speed_l6.y > 0)
                speed_l6.y -= deceleration * dt * fabsf(speed_l6.y / speed_l6.x);
            if (speed_l6.y < 0)
                speed_l6.y += deceleration * dt * fabsf(speed_l6.y / speed_l6.x);
            if (speed_l6.x > 0)
                speed_l6.x -= deceleration * dt;
            if (speed_l6.x < 0)
                speed_l6.x += deceleration * dt;
        }
        else if (fabsf(speed_l6.x) < fabsf(speed_l6.y) && speed_l6.y != 0)
        {
            if (speed_l6.x > 0)
                speed_l6.x -= deceleration * dt * fabsf(speed_l6.x / speed_l6.y);
            if (speed_l6.x < 0)
                speed_l6.x += deceleration * dt * fabsf(speed_l6.x / speed_l6.y);
            if (speed_l6.y > 0)
                speed_l6.y -= deceleration * dt;
            if (speed_l6.y < 0)
                speed_l6.y += deceleration * dt;
        }

        if (speed_l6.x < 5 && speed_l6.x > -5)
            speed_l6.x = 0;
        if (speed_l6.y < 5 && speed_l6.y > -5)
            speed_l6.y = 0;

        // areal decceleration
        deccelerated_area_center_l6 = Playground_center.y - Playground_height / 6;
        if (ball_l6.y > deccelerated_area_center_l6 - deccelerated_area_width_l6 / 2 && ball_l6.y < deccelerated_area_center_l6 + deccelerated_area_width_l6 / 2)
            speed_l6.y = speed_l6.y + deccelerated_area_decceleration_l6 * dt;

        // areal acceleration
        accelerated_area_center_l6 = Playground_center.y + Playground_height / 6;
        if (ball_l6.y > accelerated_area_center_l6 - accelerated_area_width_l6 / 2 && ball_l6.y < accelerated_area_center_l6 + accelerated_area_width_l6 / 2)
            speed_l6.y = speed_l6.y - accelerated_area_acceleration_L6 * dt;

        // shooting
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && speed_l6.x == 0 && speed_l6.y == 0)
        {
            int mouse_x = GetMouseX();
            int mouse_y = GetMouseY();
            int drag_x = ball_l6.x - mouse_x;
            int drag_y = ball_l6.y - mouse_y;
            speed_l6.x = drag_x * 2;
            if (fabsf(speed_l6.x) > max_speed)
                speed_l6.x = speed_l6.x / fabsf(speed_l6.x) * max_speed;
            speed_l6.y = drag_y * 2;
            if (fabsf(speed_l6.y) > max_speed)
                speed_l6.y = speed_l6.y / fabsf(speed_l6.y) * max_speed;
        }

        // score
        if ((ball_l6.x > pot.x - 3 * radius_pot / 4) && (ball_l6.x < pot.x + 3 * radius_pot / 4) && (ball_l6.y > pot.y - 3 * radius_pot / 4) && (ball_l6.y < pot.y + 3 * radius_pot / 4))
        {
            ball_l6.x = width / 2;
            ball_l6.y = 4 * height / 5;
            speed_l6.x = 0;
            speed_l6.y = 0;
        }

        BeginDrawing();
        ClearBackground(SKYBLUE);

        Rectangle Playground = {(Playground_center.x - Playground_width / 2), (Playground_center.y - Playground_height / 2), Playground_width, Playground_height};
        DrawRectangleRec(Playground, GetColor(0x00F000FF));
        DrawRectangle(Playground_center.x - Playground_width / 2, deccelerated_area_center_l6 - deccelerated_area_width_l6 / 2, Playground_width, deccelerated_area_width_l6, RED);
        DrawRectangleLines(Playground_center.x - Playground_width / 2, deccelerated_area_center_l6 - deccelerated_area_width_l6 / 2, Playground_width, deccelerated_area_width_l6, BLACK);
        Vector2 arrowdown1 = {Playground_center.x - Playground_width / 2 + Playground_width / 4, deccelerated_area_center_l6};
        Vector2 arrowdown2 = {Playground_center.x - Playground_width / 2 + 2 * Playground_width / 4, deccelerated_area_center_l6};
        Vector2 arrowdown3 = {Playground_center.x - Playground_width / 2 + 3 * Playground_width / 4, deccelerated_area_center_l6};
        DrawPoly(arrowdown1, 3, 7, 90, BLACK);
        DrawPoly(arrowdown2, 3, 7, 90, BLACK);
        DrawPoly(arrowdown3, 3, 7, 90, BLACK);
        DrawRectangle(Playground_center.x - Playground_width / 2, accelerated_area_center_l6 - accelerated_area_width_l6 / 2, Playground_width, accelerated_area_width_l6, RED);
        DrawRectangleLines(Playground_center.x - Playground_width / 2, accelerated_area_center_l6 - accelerated_area_width_l6 / 2, Playground_width, accelerated_area_width_l6, BLACK);
        Vector2 arrow1 = {Playground_center.x - Playground_width / 2 + Playground_width / 4, accelerated_area_center_l6};
        Vector2 arrow2 = {Playground_center.x - Playground_width / 2 + 2 * Playground_width / 4, accelerated_area_center_l6};
        Vector2 arrow3 = {Playground_center.x - Playground_width / 2 + 3 * Playground_width / 4, accelerated_area_center_l6};
        DrawPoly(arrow1, 3, 7, 270, BLACK);
        DrawPoly(arrow2, 3, 7, 270, BLACK);
        DrawPoly(arrow3, 3, 7, 270, BLACK);
        DrawCircle(pot.x, pot.y, radius_pot, BLACK);
        DrawCircle(ball_l6.x, ball_l6.y, radius_ball, WHITE);
        DrawRectangleLinesEx(Playground, 5, BLACK);
        DrawCircleLines(pot.x, pot.y, radius_pot + 1, WHITE);
        DrawCircleLines(ball_l6.x, ball_l6.y, radius_ball + 1, BLACK);
        // DrawRectangleRec(obstacle_l6,RED);
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && speed_l6.x == 0 && speed_l6.y == 0)
        {
            Vector2 mouse = {GetMouseX(), GetMouseY()};
            Vector2 shoot_direction = Vector2Normalize(Vector2Subtract(ball_l6, mouse));
            int pixel_size = 30;
            int dash_count = Vector2Distance(mouse, ball_l6) / pixel_size;

            for (int i = 0; i < dash_count; i += 2)
            {
                Vector2 start = {ball_l6.x + shoot_direction.x * (radius_ball + i * pixel_size), ball_l6.y + shoot_direction.y * (radius_ball + i * pixel_size)};
                Vector2 end = {ball_l6.x + shoot_direction.x * (radius_ball + (i + 1) * pixel_size), ball_l6.y + shoot_direction.y * (radius_ball + (i + 1) * pixel_size)};
                DrawLineV(start, end, BLACK);
            }
        }
        // DrawRectangleLines(width/2-obstacle_edge_l6/2,height/2-obstacle_edge_l6/2,obstacle_edge_l6,obstacle_edge_l6,BLACK);
        DrawRectanglePro(obstacle_l6, origin_l6, 45.0, BLACK);
        EndDrawing();
    }
    CloseWindow();

    return 0;
}