#include "raylib.h"
#include "raymath.h"

#define width 800
#define height 800
int Playground_width = width / 4;
int Playground_height = height * 3 / 4;
Vector2 Playground_center = {width / 2, height / 2};
Vector2 pot = {width / 2, height / 5};
#define max_speed 400
int radius_pot = height / 60;
int radius_ball = height / 75;

Vector2 ball_l10 = {width / 2, 4 * height / 5};
Vector2 speed_l10 = {0, 0};
#define obstacle_width_l10 133.33
#define obstacle_height_l10 15
float obstacle_position_x_l10 = (width / 2 - width / 8 + 3);
int obstacle_speed_l10 = 50;
int deccelerated_area_width_l10 = 60;
int deccelerated_area_center_l10;
int deccelerated_area_decceleration_l10 = 1000;

int main()
{
    Rectangle Playground = {(Playground_center.x - Playground_width / 2), (Playground_center.y - Playground_height / 2), Playground_width, Playground_height};
    Rectangle obstacle_top_l10 = {obstacle_position_x_l10, (Playground_center.y - Playground_height / 2 + Playground_height / 3), obstacle_width_l10, obstacle_height_l10};
    // Rectangle obstacle_bottom = {(Playground_center.x - Playground_width/2 + Playground_width/3 -1),  (Playground_center.y - Playground_height/2 + Playground_height*2/3), obstacle_width_l10, obstacle_height_l10};

    InitWindow(width, height, "practice");
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        ball_l10 = Vector2Add(ball_l10, Vector2Scale(speed_l10, dt));

        // wall bounce
        if (ball_l10.x < (Playground_center.x - Playground_width / 2 + radius_ball) || ball_l10.x > (Playground_center.x + Playground_width / 2 - radius_ball))
        {
            speed_l10.x = -speed_l10.x;
            if (ball_l10.x < (Playground_center.x - Playground_width / 2 + radius_ball))
                ball_l10.x = (Playground_center.x - Playground_width / 2 + radius_ball);
            if (ball_l10.x > (Playground_center.x + Playground_width / 2 - radius_ball))
                ball_l10.x = (Playground_center.x + Playground_width / 2 - radius_ball);
        }
        if (ball_l10.y < (Playground_center.y - Playground_height / 2 + radius_ball) || ball_l10.y > (Playground_center.y + Playground_height / 2 - radius_ball))
        {
            speed_l10.y = -speed_l10.y;
            if (ball_l10.y < (Playground_center.y - Playground_height / 2 + radius_ball))
                ball_l10.y = (Playground_center.y - Playground_height / 2 + radius_ball);
            if (ball_l10.y > (Playground_center.y + Playground_height / 2 - radius_ball))
                ball_l10.y = (Playground_center.y + Playground_height / 2 - radius_ball);
        }

        // collision
        if (CheckCollisionCircleRec(ball_l10, radius_ball, obstacle_top_l10))
        {
            Vector2 collision_point;
            collision_point.x = Clamp(ball_l10.x, obstacle_position_x_l10, (obstacle_position_x_l10 + obstacle_width_l10));
            collision_point.y = Clamp(ball_l10.y, (Playground_center.y - Playground_height / 2 + Playground_height / 3), (Playground_center.y - Playground_height / 2 + Playground_height / 3 + obstacle_height_l10));
            Vector2 normal = Vector2Normalize(Vector2Subtract(ball_l10, collision_point));
            speed_l10 = Vector2Reflect(speed_l10, normal);
            ball_l10 = Vector2Add(collision_point, Vector2Scale(normal, radius_ball));
        }

        // if (CheckCollisionCircleRec(ball_l10,radius_ball,obstacle_bottom))
        // {
        //     Vector2 collision_point;
        //     collision_point.x = Clamp(ball_l10.x,(Playground_center.x - Playground_width/2 + Playground_width/3 -1),(Playground_center.x - Playground_width/2 + Playground_width/3 -1 + obstacle_width_l10));
        //     collision_point.y = Clamp(ball_l10.y,(Playground_center.y - Playground_height/2 + Playground_height*2/3),(Playground_center.y - Playground_height/2 + Playground_height*2/3 + obstacle_height_l10));
        //     Vector2 normal = Vector2Normalize(Vector2Subtract(ball_l10,collision_point));
        //     speed_l10 = Vector2Reflect(speed_l10,normal);
        //     ball_l10 = Vector2Add(collision_point,Vector2Scale(normal,radius_ball));
        // }

        // obstacle moving
        obstacle_position_x_l10 = obstacle_position_x_l10 + obstacle_speed_l10 * dt;
        if (obstacle_position_x_l10 >= (Playground_center.x + Playground_width / 2 - obstacle_width_l10 - 1) || obstacle_position_x_l10 < (Playground_center.x - Playground_width / 2))
            obstacle_speed_l10 = -obstacle_speed_l10;
        obstacle_top_l10.x = obstacle_position_x_l10;

        // proportional deceleration
        float deceleration = 100.00;
        if (fabsf(speed_l10.x) >= fabsf(speed_l10.y) && speed_l10.x != 0)
        {
            if (speed_l10.y > 0)
                speed_l10.y -= deceleration * dt * fabsf(speed_l10.y / speed_l10.x);
            if (speed_l10.y < 0)
                speed_l10.y += deceleration * dt * fabsf(speed_l10.y / speed_l10.x);
            if (speed_l10.x > 0)
                speed_l10.x -= deceleration * dt;
            if (speed_l10.x < 0)
                speed_l10.x += deceleration * dt;
        }
        else if (fabsf(speed_l10.x) < fabsf(speed_l10.y) && speed_l10.y != 0)
        {
            if (speed_l10.x > 0)
                speed_l10.x -= deceleration * dt * fabsf(speed_l10.x / speed_l10.y);
            if (speed_l10.x < 0)
                speed_l10.x += deceleration * dt * fabsf(speed_l10.x / speed_l10.y);
            if (speed_l10.y > 0)
                speed_l10.y -= deceleration * dt;
            if (speed_l10.y < 0)
                speed_l10.y += deceleration * dt;
        }

        if (speed_l10.x < 5 && speed_l10.x > -5)
            speed_l10.x = 0;
        if (speed_l10.y < 5 && speed_l10.y > -5)
            speed_l10.y = 0;

        // areal decceleration
        deccelerated_area_center_l10 = Playground_center.y + Playground_height / 6;
        if (ball_l10.y > deccelerated_area_center_l10 - deccelerated_area_width_l10 / 2 && ball_l10.y < deccelerated_area_center_l10 + deccelerated_area_width_l10 / 2)
            speed_l10.y = speed_l10.y + deccelerated_area_decceleration_l10 * dt;

        // shooting
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && speed_l10.x == 0 && speed_l10.y == 0)
        {
            int mouse_x = GetMouseX();
            int mouse_y = GetMouseY();
            int drag_x = ball_l10.x - mouse_x;
            int drag_y = ball_l10.y - mouse_y;
            speed_l10.x = drag_x * 2;
            if (fabsf(speed_l10.x) > max_speed)
                speed_l10.x = speed_l10.x / fabsf(speed_l10.x) * max_speed;
            speed_l10.y = drag_y * 2;
            if (fabsf(speed_l10.y) > max_speed)
                speed_l10.y = speed_l10.y / fabsf(speed_l10.y) * max_speed;
        }

        // score
        if ((ball_l10.x > pot.x - 3 * radius_pot / 4) && (ball_l10.x < pot.x + 3 * radius_pot / 4) && (ball_l10.y > pot.y - 3 * radius_pot / 4) && (ball_l10.y < pot.y + 3 * radius_pot / 4))
        {
            ball_l10.x = width / 2;
            ball_l10.y = 4 * height / 5;
            speed_l10.x = 0;
            speed_l10.y = 0;
        }

        BeginDrawing();
        ClearBackground(SKYBLUE);

        DrawRectangleRec(Playground, GetColor(0x00F000FF));
        DrawRectangleLinesEx(Playground, 5, BLACK);
        DrawRectangle(Playground_center.x - Playground_width / 2 + 4, deccelerated_area_center_l10 - deccelerated_area_width_l10 / 2, Playground_width - 8, deccelerated_area_width_l10, RED);
        DrawRectangleLines(Playground_center.x - Playground_width / 2 + 4, deccelerated_area_center_l10 - deccelerated_area_width_l10 / 2, Playground_width - 8, deccelerated_area_width_l10, BLACK);
        Vector2 arrowdown1 = {Playground_center.x - Playground_width / 2 + Playground_width / 4, deccelerated_area_center_l10};
        Vector2 arrowdown2 = {Playground_center.x - Playground_width / 2 + 2 * Playground_width / 4, deccelerated_area_center_l10};
        Vector2 arrowdown3 = {Playground_center.x - Playground_width / 2 + 3 * Playground_width / 4, deccelerated_area_center_l10};
        DrawPoly(arrowdown1, 3, 7, 90, BLACK);
        DrawPoly(arrowdown2, 3, 7, 90, BLACK);
        DrawPoly(arrowdown3, 3, 7, 90, BLACK);
        DrawCircle(pot.x, pot.y, radius_pot, BLACK);
        DrawCircle(ball_l10.x, ball_l10.y, radius_ball, WHITE);
        DrawCircleLines(pot.x, pot.y, radius_pot + 1, WHITE);
        DrawCircleLines(ball_l10.x, ball_l10.y, radius_ball + 1, BLACK);
        // DrawRectangleRec(obstacle,RED);
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && speed_l10.x == 0 && speed_l10.y == 0)
        {
            Vector2 mouse = {GetMouseX(), GetMouseY()};
            Vector2 shoot_direction = Vector2Normalize(Vector2Subtract(ball_l10, mouse));
            int pixel_size = 30;
            int dash_count = Vector2Distance(mouse, ball_l10) / pixel_size;

            for (int i = 0; i < dash_count; i += 2)
            {
                Vector2 start = {ball_l10.x + shoot_direction.x * (radius_ball + i * pixel_size), ball_l10.y + shoot_direction.y * (radius_ball + i * pixel_size)};
                Vector2 end = {ball_l10.x + shoot_direction.x * (radius_ball + (i + 1) * pixel_size), ball_l10.y + shoot_direction.y * (radius_ball + (i + 1) * pixel_size)};
                DrawLineV(start, end, BLACK);
            }
        }
        // DrawRectangleLines(width/2-obstacle_edge/2,height/2-obstacle_edge/2,obstacle_edge,obstacle_edge,BLACK);

        DrawRectangleRec(obstacle_top_l10, RED);
        // DrawRectangleRec(obstacle_bottom,RED);
        DrawRectangleLinesEx(obstacle_top_l10, 3, BLACK);
        // DrawRectangleLinesEx(obstacle_bottom,3,BLACK);
        EndDrawing();
    }
    CloseWindow();

    return 0;
}