#include "raylib.h"
#include "raymath.h"

#define screen_width 800
#define screen_height 800

int Playground_width = screen_width / 2;
int Playground_height = screen_height * 3 / 4;

Vector2 Playground_center = {screen_width / 2, screen_height / 2};
Vector2 pot = {screen_width / 2, screen_height / 5};
Vector2 ball = {screen_width / 2, 3.8 * screen_height / 5};
Vector2 speed = {0, 0};
Vector2 obstacle = {screen_width / 2, screen_height / 2};

int radius_pot = screen_height / 50;
int radius_ball = screen_height / 60;
int radius_obstackle = 25;

int main()
{
    InitWindow(screen_width, screen_height, "practice");
    SetTargetFPS(60);
    InitAudioDevice();

    Sound hitSound = LoadSound("assets/audio/jump.wav");
    Texture2D ball_texture = LoadTexture("assets/ball-items/ball-blue.png");
    Texture2D background = LoadTexture("assets/not-ball/background.png");
    Texture2D playground = LoadTexture("assets/not-ball/playground.png");

    while (!WindowShouldClose())
    {
        // wall bounce
        float dt = GetFrameTime();
        ball = Vector2Add(ball, Vector2Scale(speed, dt));

        int left_wall = Playground_center.x - Playground_width / 2 + radius_ball;
        int right_wall = Playground_center.x + Playground_width / 2 - radius_ball;
        int lower_wall = Playground_center.y + Playground_height / 2 - radius_ball;
        int upper_wall = Playground_center.y - Playground_height / 2 + radius_ball;

        if (ball.x < (left_wall) || ball.x > (right_wall))
        {
            speed.x = -speed.x;
            if (ball.x < (left_wall))
                ball.x = (left_wall);
            if (ball.x > (right_wall))
                ball.x = (right_wall);
        }
        if (ball.y > (lower_wall) || ball.y < (upper_wall))
        {
            speed.y = -speed.y;
            if (ball.y > (lower_wall))
                ball.y = (lower_wall);
            if (ball.y < (upper_wall))
                ball.y = (upper_wall);
        }

        // collision
        if (CheckCollisionCircles(ball, radius_ball, obstacle, radius_obstackle))
        {
            Vector2 unit_along_shootal = Vector2Normalize(Vector2Subtract(ball, obstacle));
            speed = Vector2Reflect(speed, unit_along_shootal);
        }

        // proportional deceleration
        float deceleration = 100.00;
        if (fabsf(speed.x) >= fabsf(speed.y) && speed.x != 0)
        {
            if (speed.y > 0)
                speed.y -= deceleration * dt * fabsf(speed.y / speed.x);
            if (speed.y < 0)
                speed.y += deceleration * dt * fabsf(speed.y / speed.x);
            if (speed.x > 0)
                speed.x -= deceleration * dt;
            if (speed.x < 0)
                speed.x += deceleration * dt;
        }
        else if (fabsf(speed.x) < fabsf(speed.y) && speed.y != 0)
        {
            if (speed.x > 0)
                speed.x -= deceleration * dt * fabsf(speed.x / speed.y);
            if (speed.x < 0)
                speed.x += deceleration * dt * fabsf(speed.x / speed.y);
            if (speed.y > 0)
                speed.y -= deceleration * dt;
            if (speed.y < 0)
                speed.y += deceleration * dt;
        }

        if (speed.x < 5 && speed.x > -5)
            speed.x = 0;
        if (speed.y < 5 && speed.y > -5)
            speed.y = 0;

        // shooting
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && speed.x == 0 && speed.y == 0)
        {
            int mouse_x = GetMouseX();
            int mouse_y = GetMouseY();
            int drag_x = ball.x - mouse_x;
            int drag_y = ball.y - mouse_y;
            speed.x = drag_x * 2;
            speed.y = drag_y * 2;
        }

        // score
        if ((ball.x > pot.x - 3 * radius_pot / 4) && (ball.x < pot.x + 3 * radius_pot / 4) && (ball.y > pot.y - 3 * radius_pot / 4) && (ball.y < pot.y + 3 * radius_pot / 4))
        { // score
            ball.x = screen_width / 2;
            ball.y = 4 * screen_height / 5;
            speed.x = 0;
            speed.y = 0;
            PlaySound(hitSound);
        }

        BeginDrawing();
        ClearBackground(SKYBLUE);

        Rectangle Playground = {(Playground_center.x - Playground_width / 2), (Playground_center.y - Playground_height / 2), Playground_width, Playground_height};
        // DrawRectangleRec(Playground, GetColor(0x00F000FF));
        DrawTexturePro(
            background, (Rectangle){0, 0, background.width, background.height},
            (Rectangle){0, 0, screen_width, screen_height},
            Vector2Zero(), 0.0f, WHITE);

        DrawTexturePro(
            playground, (Rectangle){0, 0, playground.width, playground.height},
            (Rectangle){Playground_center.x - Playground_width, Playground_center.y + Playground_height, Playground_width, Playground_height},
            Vector2Zero(), 0.0f, WHITE);

        DrawCircle(pot.x, pot.y, radius_pot, BLACK);
        DrawRectangleLinesEx(Playground, 5, BLACK);
        DrawCircleLines(pot.x, pot.y, radius_pot, YELLOW);
        DrawCircle(obstacle.x, obstacle.y, radius_obstackle, RED);

        DrawTexturePro(
            ball_texture, (Rectangle){0, 0, ball_texture.width, ball_texture.height},
            (Rectangle){ball.x - radius_ball * 4, ball.y - radius_ball * 4, radius_ball * 8, radius_ball * 8},
            Vector2Zero(), 0.0f, WHITE);

        // projection direction
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && speed.x == 0 && speed.y == 0)
        {
            Vector2 mouse = {GetMouseX(), GetMouseY()};
            Vector2 shoot_direction = Vector2Normalize(Vector2Subtract(ball, mouse));

            int pixel_size = 30;
            int dash_count = Vector2Distance(mouse, ball) / pixel_size;

            for (int i = 0; i < dash_count; i += 2)
            {
                Vector2 start = {ball.x + shoot_direction.x * (radius_ball + i * pixel_size), ball.y + shoot_direction.y * (radius_ball + i * pixel_size)};
                Vector2 end = {ball.x + shoot_direction.x * (radius_ball + (i + 1) * pixel_size), ball.y + shoot_direction.y * (radius_ball + (i + 1) * pixel_size)};
                DrawLineV(start, end, BLACK);
            }
        }
        EndDrawing();
    }
    UnloadSound(hitSound);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}