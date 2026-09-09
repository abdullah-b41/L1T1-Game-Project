#include "raylib.h"
#include "raymath.h"

// full global
#define width_f 800
#define height_f 800
int Playground_width = width_f / 4;
int Playground_height = height_f * 3 / 4;
Vector2 Playground_center = {width_f / 2, height_f / 2};
Rectangle Playground = {(width_f / 2 - width_f / 8), (height_f / 2 - height_f * 3 / 4 / 2), width_f / 4, height_f * 3 / 4};
Vector2 pot = {width_f / 2, height_f / 5};
int radius_pot = height_f / 60;
int radius_ball = height_f / 75;
#define max_speed 400
int stroke = 0;
int level = 1;
int score = 0;
int play = 0;

// menu
#define MENU_SCALE 3.0f
#define BUTTON_SCALE 2.0f
#define BUTTON_SPACING 100.0f

Texture2D menu[3];
Texture2D start[3];

void load_menu_assets()
{
    // Menu background
    Texture2D menu_background = LoadTexture("assets/not-ball/start.png");

    // Menu box
    menu[0] = LoadTexture("assets/not-ball/tile_0056.png");
    menu[1] = LoadTexture("assets/not-ball/tile_0057.png");
    menu[2] = LoadTexture("assets/not-ball/tile_0058.png");

    // Start box
    start[0] = LoadTexture("assets/not-ball/tile_0069.png");
    start[1] = LoadTexture("assets/not-ball/tile_0070.png");
    start[2] = LoadTexture("assets/not-ball/tile_0071.png");
}

void menu_view()
{
    // BeginDrawing();
    float piece_width = (float)menu[0].width * BUTTON_SCALE;
    float piece_height = (float)menu[0].height * BUTTON_SCALE;
    float total_width = piece_width * BUTTON_SCALE;

    float menu_x = Playground_center.x - (total_width / 2.0f);
    float menu_y = Playground_center.y - (piece_height / 2.0f);

    int mouse_x = GetMouseX();
    int mouse_y = GetMouseY();

    // Start game
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        mouse_x >= Playground_center.x - (total_width / 2.0f) &&
        mouse_x <= Playground_center.x - (total_width / 2.0f) + total_width)
    {
        if (mouse_y >= Playground_center.y - (piece_height / 2.0f) - Playground_height / 3 + 200 &&
            mouse_y <= Playground_center.y - (piece_height / 2.0f) + 200 + piece_height - Playground_height / 3)
        {
            play = 1;
            return;
        }
    }

    // Exit game
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        mouse_x >= Playground_center.x - (total_width / 2.0f) &&
        mouse_x <= Playground_center.x - (total_width / 2.0f) + total_width)
    {
        if (mouse_y >= Playground_center.y - (piece_height / 2.0f) + 200 + 100 - Playground_height / 3 &&
            mouse_y <= Playground_center.y - (piece_height / 2.0f) + 200 + 100 + piece_height - Playground_height / 3)
        {
            level = 13;
            return;
        }
    }

    // menu
    piece_width = (float)menu[0].width * MENU_SCALE;
    piece_height = (float)menu[0].height * MENU_SCALE;
    total_width = piece_width * 3.0f;
    menu_x = Playground_center.x - (total_width / 2.0f);
    menu_y = Playground_center.y - (piece_height / 2.0f);
    piece_width = (float)menu[0].width * MENU_SCALE;
    piece_height = (float)menu[0].height * MENU_SCALE;
    total_width = piece_width * MENU_SCALE;
    Rectangle start_rect = {menu_x, menu_y, total_width, piece_height};
    for (int i = 0; i < 3; i++)
    {
        Rectangle dest = {menu_x + (i * piece_width),
                          menu_y - Playground_height / 4, piece_width, piece_height};

        DrawTexturePro(
            menu[i],
            (Rectangle){0.0f, 0.0f, (float)menu[i].width, (float)menu[i].height},
            dest,
            Vector2Zero(), 0.0f, WHITE);
    }

    char show[] = "MENU";
    int font_size = 30;
    int text_width = MeasureText(menu, font_size);
    int text_x = menu_x + (int)((total_width - text_width) / 2.0f);
    int text_y = menu_y + (int)((piece_height - font_size) / 2.0f);
    DrawText(show, text_x, text_y - Playground_height / 4, font_size, WHITE);

    // start
    piece_width = (float)start[0].width * BUTTON_SCALE;
    piece_height = (float)start[0].height * BUTTON_SCALE;
    total_width = piece_width * 3.0f;
    menu_x = Playground_center.x - (total_width / 2.0f);
    menu_y = Playground_center.y - (piece_height / 2.0f) + 200;
    Rectangle start_rec = {menu_x, menu_y, total_width, piece_height};
    for (int i = 0; i < 3; i++)
    {
        Rectangle dest = {menu_x + (i * piece_width), menu_y - Playground_height / 3, piece_width, piece_height};

        DrawTexturePro(
            start[i],
            (Rectangle){0.0f, 0.0f, (float)start[i].width, (float)start[i].height},
            dest,
            Vector2Zero(), 0.0f, WHITE);
    }

    char text[] = "START";
    text_width = MeasureText(text, font_size);
    text_x = menu_x + (int)((total_width - text_width) / 2.0f);
    text_y = menu_y + (int)((piece_height - font_size) / 2.0f);
    DrawText(text, text_x, text_y - Playground_height / 3, font_size, WHITE);

    // exit
    menu_y = Playground_center.y - (piece_height / 2.0f) + 200 + 100;
    Rectangle end_rect = {menu_x, menu_y, total_width, piece_height};
    for (int i = 0; i < 3; i++)
    {
        Rectangle dest = {menu_x + (i * piece_width), menu_y - Playground_height / 3, piece_width, piece_height};

        DrawTexturePro(
            start[i],
            (Rectangle){0.0f, 0.0f, (float)start[i].width, (float)start[i].height},
            dest,
            Vector2Zero(), 0.0f, WHITE);
    }

    char exit[] = "EXIT";
    text_width = MeasureText(exit, font_size);
    text_x = menu_x + (int)((total_width - text_width) / 2.0f);
    text_y = menu_y + (int)((piece_height - font_size) / 2.0f);
    DrawText(exit, text_x, text_y - Playground_height / 3, font_size, WHITE);

    // EndDrawing();
    // for (int i = 0; i < 3; i++)
    // {
    //     UnloadTexture(menu[i]);
    //     UnloadTexture(start[i]);
    // }
}

// level 1
Vector2 ball_l1 = {width_f / 2, 4 * height_f / 5};
Vector2 speed_l1 = {0, 0};
bool dragging_l1 = false;
Vector2 obstacle_l1 = {width_f / 2, height_f / 2};
int radius_obstackle_l1 = 25;

void play_level_01()
{
    {
        // wall bounce
        float dt = GetFrameTime();
        ball_l1 = Vector2Add(ball_l1, Vector2Scale(speed_l1, dt));
        if (ball_l1.x < (Playground_center.x - Playground_width / 2 + radius_ball) || ball_l1.x > (Playground_center.x + Playground_width / 2 - radius_ball))
        {
            speed_l1.x = -speed_l1.x;
            if (ball_l1.x < (Playground_center.x - Playground_width / 2 + radius_ball))
                ball_l1.x = (Playground_center.x - Playground_width / 2 + radius_ball);
            if (ball_l1.x > (Playground_center.x + Playground_width / 2 - radius_ball))
                ball_l1.x = (Playground_center.x + Playground_width / 2 - radius_ball);
        }
        if (ball_l1.y < (Playground_center.y - Playground_height / 2 + radius_ball) || ball_l1.y > (Playground_center.y + Playground_height / 2 - radius_ball))
        {
            speed_l1.y = -speed_l1.y;
            if (ball_l1.y < (Playground_center.y - Playground_height / 2 + radius_ball))
                ball_l1.y = (Playground_center.y - Playground_height / 2 + radius_ball);
            if (ball_l1.y > (Playground_center.y + Playground_height / 2 - radius_ball))
                ball_l1.y = (Playground_center.y + Playground_height / 2 - radius_ball);
        }

        // collision
        if (CheckCollisionCircles(ball_l1, radius_ball, obstacle_l1, radius_obstackle_l1))
        {
            Vector2 normal = Vector2Normalize(Vector2Subtract(ball_l1, obstacle_l1));
            speed_l1 = Vector2Reflect(speed_l1, normal);
        }

        // proportional deceleration
        float deceleration = 100.00;
        if (fabsf(speed_l1.x) >= fabsf(speed_l1.y) && speed_l1.x != 0)
        {
            if (speed_l1.y > 0)
                speed_l1.y -= deceleration * dt * fabsf(speed_l1.y / speed_l1.x);
            if (speed_l1.y < 0)
                speed_l1.y += deceleration * dt * fabsf(speed_l1.y / speed_l1.x);
            if (speed_l1.x > 0)
                speed_l1.x -= deceleration * dt;
            if (speed_l1.x < 0)
                speed_l1.x += deceleration * dt;
        }
        else if (fabsf(speed_l1.x) < fabsf(speed_l1.y) && speed_l1.y != 0)
        {
            if (speed_l1.x > 0)
                speed_l1.x -= deceleration * dt * fabsf(speed_l1.x / speed_l1.y);
            if (speed_l1.x < 0)
                speed_l1.x += deceleration * dt * fabsf(speed_l1.x / speed_l1.y);
            if (speed_l1.y > 0)
                speed_l1.y -= deceleration * dt;
            if (speed_l1.y < 0)
                speed_l1.y += deceleration * dt;
        }

        if (speed_l1.x < 5 && speed_l1.x > -5)
            speed_l1.x = 0;
        if (speed_l1.y < 5 && speed_l1.y > -5)
            speed_l1.y = 0;

        // shooting
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && speed_l1.x == 0 && speed_l1.y == 0 &&
            CheckCollisionPointCircle((Vector2){GetMouseX(), GetMouseY()}, ball_l1, radius_ball))
        {
            dragging_l1 = true;
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && dragging_l1 && speed_l1.x == 0 && speed_l1.y == 0)
        {
            int mouse_x = GetMouseX();
            int mouse_y = GetMouseY();
            int drag_x = ball_l1.x - mouse_x;
            int drag_y = ball_l1.y - mouse_y;
            speed_l1.x = drag_x * 2;
            speed_l1.y = drag_y * 2;
            stroke++;
            dragging_l1 = false;
        }

        // score
        if ((ball_l1.x > pot.x - 3 * radius_pot / 4) && (ball_l1.x < pot.x + 3 * radius_pot / 4) && (ball_l1.y > pot.y - 3 * radius_pot / 4) && (ball_l1.y < pot.y + 3 * radius_pot / 4))
        { // score
            ball_l1.x = width_f / 2;
            ball_l1.y = 4 * height_f / 5;
            speed_l1.x = 0;
            speed_l1.y = 0;
            level++;
            return;
        }

        // BeginDrawing();

        Rectangle Playground = {(Playground_center.x - Playground_width / 2), (Playground_center.y - Playground_height / 2), Playground_width, Playground_height};
        DrawRectangleRec(Playground, GetColor(0x00F000FF));
        DrawCircle(pot.x, pot.y, radius_pot, BLACK);
        DrawCircle(ball_l1.x, ball_l1.y, radius_ball, WHITE);
        DrawRectangleLinesEx(Playground, 5, BLACK);
        DrawCircleLines(pot.x, pot.y, radius_pot + 1, YELLOW);
        DrawCircleLines(ball_l1.x, ball_l1.y, radius_ball + 1, BLACK);
        DrawCircle(obstacle_l1.x, obstacle_l1.y, radius_obstackle_l1, RED);
        if (dragging_l1 && IsMouseButtonDown(MOUSE_BUTTON_LEFT) && speed_l1.x == 0 && speed_l1.y == 0)
        {
            Vector2 mouse = {GetMouseX(), GetMouseY()};
            Vector2 shoot_direction = Vector2Normalize(Vector2Subtract(ball_l1, mouse));

            int pixel_size = 30;
            int dash_count = Vector2Distance(mouse, ball_l1) / pixel_size;

            for (int i = 0; i < dash_count; i += 2)
            {
                Vector2 start = {ball_l1.x + shoot_direction.x * (radius_ball + i * pixel_size), ball_l1.y + shoot_direction.y * (radius_ball + i * pixel_size)};
                Vector2 end = {ball_l1.x + shoot_direction.x * (radius_ball + (i + 1) * pixel_size), ball_l1.y + shoot_direction.y * (radius_ball + (i + 1) * pixel_size)};
                DrawLineV(start, end, BLACK);
            }
        }

        // EndDrawing();
    }
}

// level 2
Vector2 ball_l2 = {width_f / 2, 4 * height_f / 5};
Vector2 speed_l2 = {0, 0};
Vector2 obstacle_l2 = {width_f / 2, height_f / 2};
int radius_obstackle_l2 = 25;
int deccelerated_area_width_l2 = 60;
int deccelerated_area_center_l2;
int deccelerated_area_decceleration_l2 = 1000;

void play_level_02()
{
    // wall bounce
    float dt = GetFrameTime();
    ball_l2 = Vector2Add(ball_l2, Vector2Scale(speed_l2, dt));
    if (ball_l2.x < (Playground_center.x - Playground_width / 2 + radius_ball) || ball_l2.x > (Playground_center.x + Playground_width / 2 - radius_ball))
    {
        speed_l2.x = -speed_l2.x;
        if (ball_l2.x < (Playground_center.x - Playground_width / 2 + radius_ball))
            ball_l2.x = (Playground_center.x - Playground_width / 2 + radius_ball);
        if (ball_l2.x > (Playground_center.x + Playground_width / 2 - radius_ball))
            ball_l2.x = (Playground_center.x + Playground_width / 2 - radius_ball);
    }
    if (ball_l2.y < (Playground_center.y - Playground_height / 2 + radius_ball) || ball_l2.y > (Playground_center.y + Playground_height / 2 - radius_ball))
    {
        speed_l2.y = -speed_l2.y;
        if (ball_l2.y < (Playground_center.y - Playground_height / 2 + radius_ball))
            ball_l2.y = (Playground_center.y - Playground_height / 2 + radius_ball);
        if (ball_l2.y > (Playground_center.y + Playground_height / 2 - radius_ball))
            ball_l2.y = (Playground_center.y + Playground_height / 2 - radius_ball);
    }

    // collision
    if (CheckCollisionCircles(ball_l2, radius_ball, obstacle_l2, radius_obstackle_l2))
    {
        Vector2 normal = Vector2Normalize(Vector2Subtract(ball_l2, obstacle_l2));
        speed_l2 = Vector2Reflect(speed_l2, normal);
    }

    // proportional deceleration
    float deceleration = 100.00;
    if (fabsf(speed_l2.x) >= fabsf(speed_l2.y) && speed_l2.x != 0)
    {
        if (speed_l2.y > 0)
            speed_l2.y -= deceleration * dt * fabsf(speed_l2.y / speed_l2.x);
        if (speed_l2.y < 0)
            speed_l2.y += deceleration * dt * fabsf(speed_l2.y / speed_l2.x);
        if (speed_l2.x > 0)
            speed_l2.x -= deceleration * dt;
        if (speed_l2.x < 0)
            speed_l2.x += deceleration * dt;
    }
    else if (fabsf(speed_l2.x) < fabsf(speed_l2.y) && speed_l2.y != 0)
    {
        if (speed_l2.x > 0)
            speed_l2.x -= deceleration * dt * fabsf(speed_l2.x / speed_l2.y);
        if (speed_l2.x < 0)
            speed_l2.y -= deceleration * dt;
        if (speed_l2.y < 0)
            speed_l2.y += deceleration * dt;
    }

    if (speed_l2.x < 5 && speed_l2.x > -5)
        speed_l2.x = 0;
    if (speed_l2.y < 5 && speed_l2.y > -5)
        speed_l2.y = 0;

    // areal decceleration
    deccelerated_area_center_l2 = Playground_center.y - Playground_height / 6;
    if (ball_l2.y > deccelerated_area_center_l2 - deccelerated_area_width_l2 / 2 && ball_l2.y < deccelerated_area_center_l2 + deccelerated_area_width_l2 / 2)
        speed_l2.y = speed_l2.y + deccelerated_area_decceleration_l2 * dt;

    // shooting
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && speed_l2.x == 0 && speed_l2.y == 0)
    {
        int mouse_x = GetMouseX();
        int mouse_y = GetMouseY();
        int drag_x = ball_l2.x - mouse_x;
        int drag_y = ball_l2.y - mouse_y;
        speed_l2.x = drag_x * 2;
        speed_l2.y = drag_y * 2;
        stroke++;
    }

    // score
    if ((ball_l2.x > pot.x - 3 * radius_pot / 4) && (ball_l2.x < pot.x + 3 * radius_pot / 4) && (ball_l2.y > pot.y - 3 * radius_pot / 4) && (ball_l2.y < pot.y + 3 * radius_pot / 4))
    { // score
        ball_l2.x = width_f / 2;
        ball_l2.y = 4 * height_f / 5;
        speed_l2.x = 0;
        speed_l2.y = 0;
        level++;
        return;
    }

    // BeginDrawing();

    Rectangle Playground = {(Playground_center.x - Playground_width / 2), (Playground_center.y - Playground_height / 2), Playground_width, Playground_height};
    DrawRectangleRec(Playground, GetColor(0x00F000FF));
    DrawRectangle(Playground_center.x - Playground_width / 2, deccelerated_area_center_l2 - deccelerated_area_width_l2 / 2, Playground_width, deccelerated_area_width_l2, RED);
    DrawRectangleLines(Playground_center.x - Playground_width / 2, deccelerated_area_center_l2 - deccelerated_area_width_l2 / 2, Playground_width, deccelerated_area_width_l2, BLACK);
    Vector2 arrowdown1 = {Playground_center.x - Playground_width / 2 + Playground_width / 4, deccelerated_area_center_l2};
    Vector2 arrowdown2 = {Playground_center.x - Playground_width / 2 + 2 * Playground_width / 4, deccelerated_area_center_l2};
    Vector2 arrowdown3 = {Playground_center.x - Playground_width / 2 + 3 * Playground_width / 4, deccelerated_area_center_l2};
    DrawPoly(arrowdown1, 3, 7, 90, BLACK);
    DrawPoly(arrowdown2, 3, 7, 90, BLACK);
    DrawPoly(arrowdown3, 3, 7, 90, BLACK);
    DrawCircle(pot.x, pot.y, radius_pot, BLACK);
    DrawCircle(ball_l2.x, ball_l2.y, radius_ball, WHITE);
    DrawRectangleLinesEx(Playground, 5, BLACK);
    DrawCircleLines(pot.x, pot.y, radius_pot + 1, YELLOW);
    DrawCircleLines(ball_l2.x, ball_l2.y, radius_ball + 1, BLACK);
    DrawCircle(obstacle_l2.x, obstacle_l2.y, radius_obstackle_l2, RED);
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && speed_l2.x == 0 && speed_l2.y == 0)
    {
        Vector2 mouse = {GetMouseX(), GetMouseY()};
        Vector2 shoot_direction = Vector2Normalize(Vector2Subtract(ball_l2, mouse));

        int pixel_size = 30;
        int dash_count = Vector2Distance(mouse, ball_l2) / pixel_size;

        for (int i = 0; i < dash_count; i += 2)
        {
            Vector2 start = {ball_l2.x + shoot_direction.x * (radius_ball + i * pixel_size), ball_l2.y + shoot_direction.y * (radius_ball + i * pixel_size)};
            Vector2 end = {ball_l2.x + shoot_direction.x * (radius_ball + (i + 1) * pixel_size), ball_l2.y + shoot_direction.y * (radius_ball + (i + 1) * pixel_size)};
            DrawLineV(start, end, BLACK);
        }
    }

    // EndDrawing();
}

// level 03
Vector2 ball_l3 = {width_f / 2, 4 * height_f / 5};
Vector2 speed_l3 = {0, 0};
#define obstacle_edge_l3 50
Rectangle obstacle_l3 = {(width_f / 2 - obstacle_edge_l3 / 2), (height_f / 2 - obstacle_edge_l3 / 2), 50, 50};

void play_level_03()
{
    // wall bounce
    float dt = GetFrameTime();
    ball_l3 = Vector2Add(ball_l3, Vector2Scale(speed_l3, dt));
    if (ball_l3.x < (Playground_center.x - Playground_width / 2 + radius_ball) || ball_l3.x > (Playground_center.x + Playground_width / 2 - radius_ball))
    {
        speed_l3.x = -speed_l3.x;
        if (ball_l3.x < (Playground_center.x - Playground_width / 2 + radius_ball))
            ball_l3.x = (Playground_center.x - Playground_width / 2 + radius_ball);
        if (ball_l3.x > (Playground_center.x + Playground_width / 2 - radius_ball))
            ball_l3.x = (Playground_center.x + Playground_width / 2 - radius_ball);
    }
    if (ball_l3.y < (Playground_center.y - Playground_height / 2 + radius_ball) || ball_l3.y > (Playground_center.y + Playground_height / 2 - radius_ball))
    {
        speed_l3.y = -speed_l3.y;
        if (ball_l3.y < (Playground_center.y - Playground_height / 2 + radius_ball))
            ball_l3.y = (Playground_center.y - Playground_height / 2 + radius_ball);
        if (ball_l3.y > (Playground_center.y + Playground_height / 2 - radius_ball))
            ball_l3.y = (Playground_center.y + Playground_height / 2 - radius_ball);
    }

    // collision
    if (CheckCollisionCircleRec(ball_l3, radius_ball, obstacle_l3))
    {
        Vector2 collision_point;
        collision_point.x = Clamp(ball_l3.x, width_f / 2 - obstacle_edge_l3 / 2, width_f / 2 + obstacle_edge_l3 / 2);
        collision_point.y = Clamp(ball_l3.y, height_f / 2 - obstacle_edge_l3 / 2, height_f / 2 + obstacle_edge_l3 / 2);
        Vector2 normal = Vector2Normalize(Vector2Subtract(ball_l3, collision_point));
        speed_l3 = Vector2Reflect(speed_l3, normal);
        ball_l3 = Vector2Add(collision_point, Vector2Scale(normal, radius_ball));
    }

    // proportional deceleration
    float deceleration = 100.00;
    if (fabsf(speed_l3.x) >= fabsf(speed_l3.y) && speed_l3.x != 0)
    {
        if (speed_l3.y > 0)
            speed_l3.y -= deceleration * dt * fabsf(speed_l3.y / speed_l3.x);
        if (speed_l3.y < 0)
            speed_l3.y += deceleration * dt * fabsf(speed_l3.y / speed_l3.x);
        if (speed_l3.x > 0)
            speed_l3.x -= deceleration * dt;
        if (speed_l3.x < 0)
            speed_l3.x += deceleration * dt;
    }
    else if (fabsf(speed_l3.x) < fabsf(speed_l3.y) && speed_l3.y != 0)
    {
        if (speed_l3.x > 0)
            speed_l3.x -= deceleration * dt * fabsf(speed_l3.x / speed_l3.y);
        if (speed_l3.x < 0)
            speed_l3.x += deceleration * dt * fabsf(speed_l3.x / speed_l3.y);
        if (speed_l3.y > 0)
            speed_l3.y -= deceleration * dt;
        if (speed_l3.y < 0)
            speed_l3.y += deceleration * dt;
    }

    if (speed_l3.x < 5 && speed_l3.x > -5)
        speed_l3.x = 0;
    if (speed_l3.y < 5 && speed_l3.y > -5)
        speed_l3.y = 0;

    // shooting
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && speed_l3.x == 0 && speed_l3.y == 0)
    {
        stroke++;
        int mouse_x = GetMouseX();
        int mouse_y = GetMouseY();
        int drag_x = ball_l3.x - mouse_x;
        int drag_y = ball_l3.y - mouse_y;
        speed_l3.x = drag_x * 2;
        if (fabsf(speed_l3.x) > max_speed)
            speed_l3.x = speed_l3.x / fabsf(speed_l3.x) * max_speed;
        speed_l3.y = drag_y * 2;
        if (fabsf(speed_l3.y) > max_speed)
            speed_l3.y = speed_l3.y / fabsf(speed_l3.y) * max_speed;
    }

    // score
    if ((ball_l3.x > pot.x - 3 * radius_pot / 4) && (ball_l3.x < pot.x + 3 * radius_pot / 4) && (ball_l3.y > pot.y - 3 * radius_pot / 4) && (ball_l3.y < pot.y + 3 * radius_pot / 4))
    {
        ball_l3.x = width_f / 2;
        ball_l3.y = 4 * height_f / 5;
        speed_l3.x = 0;
        speed_l3.y = 0;
        level++;
        return;
    }

    // BeginDrawing();

    Rectangle Playground = {(Playground_center.x - Playground_width / 2), (Playground_center.y - Playground_height / 2), Playground_width, Playground_height};
    DrawRectangleRec(Playground, GetColor(0x00F000FF));
    DrawCircle(pot.x, pot.y, radius_pot, BLACK);
    DrawCircle(ball_l3.x, ball_l3.y, radius_ball, WHITE);
    DrawRectangleLinesEx(Playground, 5, BLACK);
    DrawCircleLines(pot.x, pot.y, radius_pot + 1, WHITE);
    DrawCircleLines(ball_l3.x, ball_l3.y, radius_ball + 1, BLACK);
    DrawRectangleRec(obstacle_l3, RED);
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && speed_l3.x == 0 && speed_l3.y == 0)
    {
        Vector2 mouse = {GetMouseX(), GetMouseY()};
        Vector2 shoot_direction = Vector2Normalize(Vector2Subtract(ball_l3, mouse));

        int pixel_size = 30;
        int dash_count = Vector2Distance(mouse, ball_l3) / pixel_size;

        for (int i = 0; i < dash_count; i += 2)
        {
            Vector2 start = {ball_l3.x + shoot_direction.x * (radius_ball + i * pixel_size), ball_l3.y + shoot_direction.y * (radius_ball + i * pixel_size)};
            Vector2 end = {ball_l3.x + shoot_direction.x * (radius_ball + (i + 1) * pixel_size), ball_l3.y + shoot_direction.y * (radius_ball + (i + 1) * pixel_size)};
            DrawLineV(start, end, BLACK);
        }
    }
    DrawRectangleLines(width_f / 2 - obstacle_edge_l3 / 2, height_f / 2 - obstacle_edge_l3 / 2, obstacle_edge_l3, obstacle_edge_l3, BLACK);

    // EndDrawing();
}

// level 04
Vector2 ball_l4 = {width_f / 2, 4 * height_f / 5};
Vector2 speed_l4 = {0, 0};
#define obstacle_edge_l4 50
Rectangle obstacle_l4 = {(width_f / 2 - obstacle_edge_l4 / 2), (height_f / 2 - obstacle_edge_l4 / 2), 50, 50};
int accelerated_area_width_l4 = 60;
int accelerated_area_center_l4;
int accelerated_area_acceleration_l4 = 1000;

void play_level_04()
{
    // wall bounce
    float dt = GetFrameTime();
    ball_l4 = Vector2Add(ball_l4, Vector2Scale(speed_l4, dt));
    if (ball_l4.x < (Playground_center.x - Playground_width / 2 + radius_ball) || ball_l4.x > (Playground_center.x + Playground_width / 2 - radius_ball))
    {
        speed_l4.x = -speed_l4.x;
        if (ball_l4.x < (Playground_center.x - Playground_width / 2 + radius_ball))
            ball_l4.x = (Playground_center.x - Playground_width / 2 + radius_ball);
        if (ball_l4.x > (Playground_center.x + Playground_width / 2 - radius_ball))
            ball_l4.x = (Playground_center.x + Playground_width / 2 - radius_ball);
    }
    if (ball_l4.y < (Playground_center.y - Playground_height / 2 + radius_ball) || ball_l4.y > (Playground_center.y + Playground_height / 2 - radius_ball))
    {
        speed_l4.y = -speed_l4.y;
        if (ball_l4.y < (Playground_center.y - Playground_height / 2 + radius_ball))
            ball_l4.y = (Playground_center.y - Playground_height / 2 + radius_ball);
        if (ball_l4.y > (Playground_center.y + Playground_height / 2 - radius_ball))
            ball_l4.y = (Playground_center.y + Playground_height / 2 - radius_ball);
    }

    // collision
    if (CheckCollisionCircleRec(ball_l4, radius_ball, obstacle_l4))
    {
        Vector2 collision_point;
        collision_point.x = Clamp(ball_l4.x, width_f / 2 - obstacle_edge_l4 / 2, width_f / 2 + obstacle_edge_l4 / 2);
        collision_point.y = Clamp(ball_l4.y, height_f / 2 - obstacle_edge_l4 / 2, height_f / 2 + obstacle_edge_l4 / 2);
        Vector2 normal = Vector2Normalize(Vector2Subtract(ball_l4, collision_point));
        speed_l4 = Vector2Reflect(speed_l4, normal);
        ball_l4 = Vector2Add(collision_point, Vector2Scale(normal, radius_ball));
    }

    // proportional deceleration
    float deceleration = 100.00;
    if (fabsf(speed_l4.x) >= fabsf(speed_l4.y) && speed_l4.x != 0)
    {
        if (speed_l4.y > 0)
            speed_l4.y -= deceleration * dt * fabsf(speed_l4.y / speed_l4.x);
        if (speed_l4.y < 0)
            speed_l4.y += deceleration * dt * fabsf(speed_l4.y / speed_l4.x);
        if (speed_l4.x > 0)
            speed_l4.x -= deceleration * dt;
        if (speed_l4.x < 0)
            speed_l4.x += deceleration * dt;
    }
    else if (fabsf(speed_l4.x) < fabsf(speed_l4.y) && speed_l4.y != 0)
    {
        if (speed_l4.x > 0)
            speed_l4.x -= deceleration * dt * fabsf(speed_l4.x / speed_l4.y);
        if (speed_l4.x < 0)
            speed_l4.x += deceleration * dt * fabsf(speed_l4.x / speed_l4.y);
        if (speed_l4.y > 0)
            speed_l4.y -= deceleration * dt;
        if (speed_l4.y < 0)
            speed_l4.y += deceleration * dt;
    }

    if (speed_l4.x < 5 && speed_l4.x > -5)
        speed_l4.x = 0;
    if (speed_l4.y < 5 && speed_l4.y > -5)
        speed_l4.y = 0;

    // areal acceleration
    accelerated_area_center_l4 = Playground_center.y + Playground_height / 6;
    if (ball_l4.y > accelerated_area_center_l4 - accelerated_area_width_l4 / 2 && ball_l4.y < accelerated_area_center_l4 + accelerated_area_width_l4 / 2)
        speed_l4.y = speed_l4.y - accelerated_area_acceleration_l4 * dt;

    // shooting
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && speed_l4.x == 0 && speed_l4.y == 0)
    {
        stroke++;
        int mouse_x = GetMouseX();
        int mouse_y = GetMouseY();
        int drag_x = ball_l4.x - mouse_x;
        int drag_y = ball_l4.y - mouse_y;
        speed_l4.x = drag_x * 2;
        if (fabsf(speed_l4.x) > max_speed)
            speed_l4.x = speed_l4.x / fabsf(speed_l4.x) * max_speed;
        speed_l4.y = drag_y * 2;
        if (fabsf(speed_l4.y) > max_speed)
            speed_l4.y = speed_l4.y / fabsf(speed_l4.y) * max_speed;
    }

    // score
    if ((ball_l4.x > pot.x - 3 * radius_pot / 4) && (ball_l4.x < pot.x + 3 * radius_pot / 4) && (ball_l4.y > pot.y - 3 * radius_pot / 4) && (ball_l4.y < pot.y + 3 * radius_pot / 4))
    {
        ball_l4.x = width_f / 2;
        ball_l4.y = 4 * height_f / 5;
        speed_l4.x = 0;
        speed_l4.y = 0;
        level++;
        return;
    }

    // BeginDrawing();

    Rectangle Playground = {(Playground_center.x - Playground_width / 2), (Playground_center.y - Playground_height / 2), Playground_width, Playground_height};
    DrawRectangleRec(Playground, GetColor(0x00F000FF));
    DrawRectangle(Playground_center.x - Playground_width / 2, accelerated_area_center_l4 - accelerated_area_width_l4 / 2, Playground_width, accelerated_area_width_l4, RED);
    DrawRectangleLines(Playground_center.x - Playground_width / 2, accelerated_area_center_l4 - accelerated_area_width_l4 / 2, Playground_width, accelerated_area_width_l4, BLACK);
    Vector2 arrow1 = {Playground_center.x - Playground_width / 2 + Playground_width / 4, accelerated_area_center_l4};
    Vector2 arrow2 = {Playground_center.x - Playground_width / 2 + 2 * Playground_width / 4, accelerated_area_center_l4};
    Vector2 arrow3 = {Playground_center.x - Playground_width / 2 + 3 * Playground_width / 4, accelerated_area_center_l4};
    DrawPoly(arrow1, 3, 7, 270, BLACK);
    DrawPoly(arrow2, 3, 7, 270, BLACK);
    DrawPoly(arrow3, 3, 7, 270, BLACK);
    DrawCircle(pot.x, pot.y, radius_pot, BLACK);
    DrawCircle(ball_l4.x, ball_l4.y, radius_ball, WHITE);
    DrawRectangleLinesEx(Playground, 5, BLACK);
    DrawCircleLines(pot.x, pot.y, radius_pot + 1, WHITE);
    DrawCircleLines(ball_l4.x, ball_l4.y, radius_ball + 1, BLACK);
    DrawRectangleRec(obstacle_l4, RED);
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && speed_l4.x == 0 && speed_l4.y == 0)
    {
        Vector2 mouse = {GetMouseX(), GetMouseY()};
        Vector2 shoot_direction = Vector2Normalize(Vector2Subtract(ball_l4, mouse));
        int pixel_size = 30;
        int dash_count = Vector2Distance(mouse, ball_l4) / pixel_size;

        for (int i = 0; i < dash_count; i += 2)
        {
            Vector2 start = {ball_l4.x + shoot_direction.x * (radius_ball + i * pixel_size), ball_l4.y + shoot_direction.y * (radius_ball + i * pixel_size)};
            Vector2 end = {ball_l4.x + shoot_direction.x * (radius_ball + (i + 1) * pixel_size), ball_l4.y + shoot_direction.y * (radius_ball + (i + 1) * pixel_size)};
            DrawLineV(start, end, BLACK);
        }
    }
    DrawRectangleLines(width_f / 2 - obstacle_edge_l4 / 2, height_f / 2 - obstacle_edge_l4 / 2, obstacle_edge_l4, obstacle_edge_l4, BLACK);

    // EndDrawing();
}

// level 05
Vector2 ball_l5 = {width_f / 2, 4 * height_f / 5};
Vector2 speed_l5 = {0, 0};
#define obstacle_edge_l5 50
Rectangle obstacle_l5 = {width_f / 2, height_f / 2, obstacle_edge_l5, obstacle_edge_l5};
Vector2 origin_l5 = {obstacle_edge_l5 / 2, obstacle_edge_l5 / 2};

void play_level_05()
{
    // wall bounce
    float dt = GetFrameTime();
    ball_l5 = Vector2Add(ball_l5, Vector2Scale(speed_l5, dt));
    if (ball_l5.x < (Playground_center.x - Playground_width / 2 + radius_ball) || ball_l5.x > (Playground_center.x + Playground_width / 2 - radius_ball))
    {
        speed_l5.x = -speed_l5.x;
        if (ball_l5.x < (Playground_center.x - Playground_width / 2 + radius_ball))
            ball_l5.x = (Playground_center.x - Playground_width / 2 + radius_ball);
        if (ball_l5.x > (Playground_center.x + Playground_width / 2 - radius_ball))
            ball_l5.x = (Playground_center.x + Playground_width / 2 - radius_ball);
    }
    if (ball_l5.y < (Playground_center.y - Playground_height / 2 + radius_ball) || ball_l5.y > (Playground_center.y + Playground_height / 2 - radius_ball))
    {
        speed_l5.y = -speed_l5.y;
        if (ball_l5.y < (Playground_center.y - Playground_height / 2 + radius_ball))
            ball_l5.y = (Playground_center.y - Playground_height / 2 + radius_ball);
        if (ball_l5.y > (Playground_center.y + Playground_height / 2 - radius_ball))
            ball_l5.y = (Playground_center.y + Playground_height / 2 - radius_ball);
    }

    // collision
    Vector2 normal;
    if (ball_l5.x >= width_f / 2 - (obstacle_edge_l5 / sqrt(2)) - radius_ball && ball_l5.x <= width_f / 2 + (obstacle_edge_l5 / sqrt(2)) + radius_ball)
    {
        float drift = obstacle_edge_l5 / sqrt(2) + radius_ball - fabsf(width_f / 2 - ball_l5.x);
        if (ball_l5.y >= height_f / 2 - drift && ball_l5.y <= height_f / 2 + drift)
        {
            if (ball_l5.x <= width_f / 2 && ball_l5.y <= height_f / 2)
            {
                normal.x = -1 / sqrt(2);
                normal.y = -1 / sqrt(2);
            }
            else if (ball_l5.x >= width_f / 2 && ball_l5.y <= height_f / 2)
            {
                normal.x = 1 / sqrt(2);
                normal.y = -1 / sqrt(2);
            }
            else if (ball_l5.x <= width_f / 2 && ball_l5.y >= height_f / 2)
            {
                normal.x = -1 / sqrt(2);
                normal.y = 1 / sqrt(2);
            }
            else if (ball_l5.x >= width_f / 2 && ball_l5.y >= height_f / 2)
            {
                normal.x = 1 / sqrt(2);
                normal.y = 1 / sqrt(2);
            }
            speed_l5 = Vector2Reflect(speed_l5, normal);
        }
    }

    // proportional deceleration
    float deceleration = 100.00;
    if (fabsf(speed_l5.x) >= fabsf(speed_l5.y) && speed_l5.x != 0)
    {
        if (speed_l5.y > 0)
            speed_l5.y -= deceleration * dt * fabsf(speed_l5.y / speed_l5.x);
        if (speed_l5.y < 0)
            speed_l5.y += deceleration * dt * fabsf(speed_l5.y / speed_l5.x);
        if (speed_l5.x > 0)
            speed_l5.x -= deceleration * dt;
        if (speed_l5.x < 0)
            speed_l5.x += deceleration * dt;
    }
    else if (fabsf(speed_l5.x) < fabsf(speed_l5.y) && speed_l5.y != 0)
    {
        if (speed_l5.x > 0)
            speed_l5.x -= deceleration * dt * fabsf(speed_l5.x / speed_l5.y);
        if (speed_l5.x < 0)
            speed_l5.x += deceleration * dt * fabsf(speed_l5.x / speed_l5.y);
        if (speed_l5.y > 0)
            speed_l5.y -= deceleration * dt;
        if (speed_l5.y < 0)
            speed_l5.y += deceleration * dt;
    }

    if (speed_l5.x < 5 && speed_l5.x > -5)
        speed_l5.x = 0;
    if (speed_l5.y < 5 && speed_l5.y > -5)
        speed_l5.y = 0;

    // shooting
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && speed_l5.x == 0 && speed_l5.y == 0)
    {
        stroke++;
        int mouse_x = GetMouseX();
        int mouse_y = GetMouseY();
        int drag_x = ball_l5.x - mouse_x;
        int drag_y = ball_l5.y - mouse_y;
        speed_l5.x = drag_x * 2;
        if (fabsf(speed_l5.x) > max_speed)
            speed_l5.x = speed_l5.x / fabsf(speed_l5.x) * max_speed;
        speed_l5.y = drag_y * 2;
        if (fabsf(speed_l5.y) > max_speed)
            speed_l5.y = speed_l5.y / fabsf(speed_l5.y) * max_speed;
    }

    // score
    if ((ball_l5.x > pot.x - 3 * radius_pot / 4) && (ball_l5.x < pot.x + 3 * radius_pot / 4) && (ball_l5.y > pot.y - 3 * radius_pot / 4) && (ball_l5.y < pot.y + 3 * radius_pot / 4))
    {
        ball_l5.x = width_f / 2;
        ball_l5.y = 4 * height_f / 5;
        speed_l5.x = 0;
        speed_l5.y = 0;
        level++;
        return;
    }

    // BeginDrawing();

    Rectangle Playground = {(Playground_center.x - Playground_width / 2), (Playground_center.y - Playground_height / 2), Playground_width, Playground_height};
    DrawRectangleRec(Playground, GetColor(0x00F000FF));
    DrawCircle(pot.x, pot.y, radius_pot, BLACK);
    DrawCircle(ball_l5.x, ball_l5.y, radius_ball, WHITE);
    DrawRectangleLinesEx(Playground, 5, BLACK);
    DrawCircleLines(pot.x, pot.y, radius_pot + 1, WHITE);
    DrawCircleLines(ball_l5.x, ball_l5.y, radius_ball + 1, BLACK);
    // DrawRectangleRec(obstacle_l5,RED);
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && speed_l5.x == 0 && speed_l5.y == 0)
    {
        Vector2 mouse = {GetMouseX(), GetMouseY()};
        Vector2 shoot_direction = Vector2Normalize(Vector2Subtract(ball_l5, mouse));
        int pixel_size = 30;
        int dash_count = Vector2Distance(mouse, ball_l5) / pixel_size;

        for (int i = 0; i < dash_count; i += 2)
        {
            Vector2 start = {ball_l5.x + shoot_direction.x * (radius_ball + i * pixel_size), ball_l5.y + shoot_direction.y * (radius_ball + i * pixel_size)};
            Vector2 end = {ball_l5.x + shoot_direction.x * (radius_ball + (i + 1) * pixel_size), ball_l5.y + shoot_direction.y * (radius_ball + (i + 1) * pixel_size)};
            DrawLineV(start, end, BLACK);
        }
    }
    // DrawRectangleLines(width_f/2-obstacle_edge_l5/2,height_f/2-obstacle_edge_l5/2,obstacle_edge_l5,obstacle_edge_l5,BLACK);
    DrawRectanglePro(obstacle_l5, origin_l5, 45.0, BLACK);
    // EndDrawing();
}

// level 06
Vector2 ball_l6 = {width_f / 2, 4 * height_f / 5};
Vector2 speed_l6 = {0, 0};
#define obstacle_edge_l6 50
Rectangle obstacle_l6 = {width_f / 2, height_f / 2, obstacle_edge_l6, obstacle_edge_l6};
Vector2 origin_l6 = {obstacle_edge_l6 / 2, obstacle_edge_l6 / 2};
int deccelerated_area_width_l6 = 60;
int deccelerated_area_center_l6;
int deccelerated_area_decceleration_l6 = 1000;
int accelerated_area_width_l6 = 60;
int accelerated_area_center_l6;
int accelerated_area_acceleration_L6 = 1000;

void play_level_06()
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
    Vector2 normal;
    if (ball_l6.x >= width_f / 2 - (obstacle_edge_l6 / sqrt(2)) - radius_ball && ball_l6.x <= width_f / 2 + (obstacle_edge_l6 / sqrt(2)) + radius_ball)
    {
        float drift = obstacle_edge_l6 / sqrt(2) + radius_ball - fabsf(width_f / 2 - ball_l6.x);
        if (ball_l6.y >= height_f / 2 - drift && ball_l6.y <= height_f / 2 + drift)
        {
            if (ball_l6.x <= width_f / 2 && ball_l6.y <= height_f / 2)
            {
                normal.x = -1 / sqrt(2);
                normal.y = -1 / sqrt(2);
            }
            else if (ball_l6.x >= width_f / 2 && ball_l6.y <= height_f / 2)
            {
                normal.x = 1 / sqrt(2);
                normal.y = -1 / sqrt(2);
            }
            else if (ball_l6.x <= width_f / 2 && ball_l6.y >= height_f / 2)
            {
                normal.x = -1 / sqrt(2);
                normal.y = 1 / sqrt(2);
            }
            else if (ball_l6.x >= width_f / 2 && ball_l6.y >= height_f / 2)
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
        stroke++;
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
        ball_l6.x = width_f / 2;
        ball_l6.y = 4 * height_f / 5;
        speed_l6.x = 0;
        speed_l6.y = 0;
        level++;
        return;
    }

    // BeginDrawing();
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
    // DrawRectangleLines(width_f/2-obstacle_edge_l6/2,height_f/2-obstacle_edge_l6/2,obstacle_edge_l6,obstacle_edge_l6,BLACK);
    DrawRectanglePro(obstacle_l6, origin_l6, 45.0, BLACK);
    // EndDrawing();
}

// level 07
Vector2 ball_l7 = {width_f / 2, 4 * height_f / 5};
Vector2 speed_l7 = {0, 0};
#define obstacle_width_l7 133.33
#define obstacle_height_l7 15
Rectangle obstacle_top_l7;
Rectangle obstacle_bottom_l7;

void play_level_07()
{
    // wall bounce
    float dt = GetFrameTime();
    ball_l7 = Vector2Add(ball_l7, Vector2Scale(speed_l7, dt));

    if (ball_l7.x < (Playground_center.x - Playground_width / 2 + radius_ball) || ball_l7.x > (Playground_center.x + Playground_width / 2 - radius_ball))
    {
        speed_l7.x = -speed_l7.x;
        if (ball_l7.x < (Playground_center.x - Playground_width / 2 + radius_ball))
            ball_l7.x = (Playground_center.x - Playground_width / 2 + radius_ball);
        if (ball_l7.x > (Playground_center.x + Playground_width / 2 - radius_ball))
            ball_l7.x = (Playground_center.x + Playground_width / 2 - radius_ball);
    }
    if (ball_l7.y < (Playground_center.y - Playground_height / 2 + radius_ball) || ball_l7.y > (Playground_center.y + Playground_height / 2 - radius_ball))
    {
        speed_l7.y = -speed_l7.y;
        if (ball_l7.y < (Playground_center.y - Playground_height / 2 + radius_ball))
            ball_l7.y = (Playground_center.y - Playground_height / 2 + radius_ball);
        if (ball_l7.y > (Playground_center.y + Playground_height / 2 - radius_ball))
            ball_l7.y = (Playground_center.y + Playground_height / 2 - radius_ball);
    }

    // collision
    if (CheckCollisionCircleRec(ball_l7, radius_ball, obstacle_top_l7))
    {
        Vector2 collision_point;
        collision_point.x = Clamp(ball_l7.x, (Playground_center.x - Playground_width / 2 + 3), (Playground_center.x - Playground_width / 2 + 3 + obstacle_width_l7));
        collision_point.y = Clamp(ball_l7.y, (Playground_center.y - Playground_height / 2 + Playground_height / 3), (Playground_center.y - Playground_height / 2 + Playground_height / 3 + obstacle_height_l7));
        Vector2 normal = Vector2Normalize(Vector2Subtract(ball_l7, collision_point));
        speed_l7 = Vector2Reflect(speed_l7, normal);
        ball_l7 = Vector2Add(collision_point, Vector2Scale(normal, radius_ball));
    }

    if (CheckCollisionCircleRec(ball_l7, radius_ball, obstacle_bottom_l7))
    {
        Vector2 collision_point;
        collision_point.x = Clamp(ball_l7.x, (Playground_center.x - Playground_width / 2 + Playground_width / 3 - 1), (Playground_center.x - Playground_width / 2 + Playground_width / 3 - 1 + obstacle_width_l7));
        collision_point.y = Clamp(ball_l7.y, (Playground_center.y - Playground_height / 2 + Playground_height * 2 / 3), (Playground_center.y - Playground_height / 2 + Playground_height * 2 / 3 + obstacle_height_l7));
        Vector2 normal = Vector2Normalize(Vector2Subtract(ball_l7, collision_point));
        speed_l7 = Vector2Reflect(speed_l7, normal);
        ball_l7 = Vector2Add(collision_point, Vector2Scale(normal, radius_ball));
    }

    // proportional deceleration
    float deceleration = 100.00;
    if (fabsf(speed_l7.x) >= fabsf(speed_l7.y) && speed_l7.x != 0)
    {
        if (speed_l7.y > 0)
            speed_l7.y -= deceleration * dt * fabsf(speed_l7.y / speed_l7.x);
        if (speed_l7.y < 0)
            speed_l7.y += deceleration * dt * fabsf(speed_l7.y / speed_l7.x);
        if (speed_l7.x > 0)
            speed_l7.x -= deceleration * dt;
        if (speed_l7.x < 0)
            speed_l7.x += deceleration * dt;
    }
    else if (fabsf(speed_l7.x) < fabsf(speed_l7.y) && speed_l7.y != 0)
    {
        if (speed_l7.x > 0)
            speed_l7.x -= deceleration * dt * fabsf(speed_l7.x / speed_l7.y);
        if (speed_l7.x < 0)
            speed_l7.x += deceleration * dt * fabsf(speed_l7.x / speed_l7.y);
        if (speed_l7.y > 0)
            speed_l7.y -= deceleration * dt;
        if (speed_l7.y < 0)
            speed_l7.y += deceleration * dt;
    }

    if (speed_l7.x < 5 && speed_l7.x > -5)
        speed_l7.x = 0;
    if (speed_l7.y < 5 && speed_l7.y > -5)
        speed_l7.y = 0;

    // shooting
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && speed_l7.x == 0 && speed_l7.y == 0)
    {
        stroke++;
        int mouse_x = GetMouseX();
        int mouse_y = GetMouseY();
        int drag_x = ball_l7.x - mouse_x;
        int drag_y = ball_l7.y - mouse_y;
        speed_l7.x = drag_x * 2;
        if (fabsf(speed_l7.x) > max_speed)
            speed_l7.x = speed_l7.x / fabsf(speed_l7.x) * max_speed;
        speed_l7.y = drag_y * 2;
        if (fabsf(speed_l7.y) > max_speed)
            speed_l7.y = speed_l7.y / fabsf(speed_l7.y) * max_speed;
    }

    // score
    if ((ball_l7.x > pot.x - 3 * radius_pot / 4) && (ball_l7.x < pot.x + 3 * radius_pot / 4) && (ball_l7.y > pot.y - 3 * radius_pot / 4) && (ball_l7.y < pot.y + 3 * radius_pot / 4))
    {
        ball_l7.x = width_f / 2;
        ball_l7.y = 4 * height_f / 5;
        speed_l7.x = 0;
        speed_l7.y = 0;
        level++;
        return;
    }

    // BeginDrawing();

    DrawRectangleRec(Playground, GetColor(0x00F000FF));
    DrawCircle(pot.x, pot.y, radius_pot, BLACK);
    DrawCircle(ball_l7.x, ball_l7.y, radius_ball, WHITE);
    DrawRectangleLinesEx(Playground, 5, BLACK);
    DrawCircleLines(pot.x, pot.y, radius_pot + 1, WHITE);
    DrawCircleLines(ball_l7.x, ball_l7.y, radius_ball + 1, BLACK);
    // DrawRectangleRec(obstacle,RED);
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && speed_l7.x == 0 && speed_l7.y == 0)
    {
        Vector2 mouse = {GetMouseX(), GetMouseY()};
        Vector2 shoot_direction = Vector2Normalize(Vector2Subtract(ball_l7, mouse));
        int pixel_size = 30;
        int dash_count = Vector2Distance(mouse, ball_l7) / pixel_size;

        for (int i = 0; i < dash_count; i += 2)
        {
            Vector2 start = {ball_l7.x + shoot_direction.x * (radius_ball + i * pixel_size), ball_l7.y + shoot_direction.y * (radius_ball + i * pixel_size)};
            Vector2 end = {ball_l7.x + shoot_direction.x * (radius_ball + (i + 1) * pixel_size), ball_l7.y + shoot_direction.y * (radius_ball + (i + 1) * pixel_size)};
            DrawLineV(start, end, BLACK);
        }
    }
    // DrawRectangleLines(width_f/2-obstacle_edge/2,height_f/2-obstacle_edge/2,obstacle_edge,obstacle_edge,BLACK);

    DrawRectangleRec(obstacle_top_l7, RED);
    DrawRectangleRec(obstacle_bottom_l7, RED);
    DrawRectangleLinesEx(obstacle_top_l7, 3, BLACK);
    DrawRectangleLinesEx(obstacle_bottom_l7, 3, BLACK);
    // EndDrawing();
}

// level 08
Vector2 ball_l8 = {width_f / 2, 4 * height_f / 5};
Vector2 speed_l8 = {0, 0};
#define obstacle_width_l8 133.33
#define obstacle_height_l8 15
int deccelerated_area_width_l8 = 46;
int deccelerated_area_center_l8;
int deccelerated_area_decceleration_l8 = 1000;
Rectangle obstacle_top_l8;
Rectangle obstacle_bottom_l8;

void play_level_08()
{
    // wall bounce
    float dt = GetFrameTime();
    ball_l8 = Vector2Add(ball_l8, Vector2Scale(speed_l8, dt));

    if (ball_l8.x < (Playground_center.x - Playground_width / 2 + radius_ball) || ball_l8.x > (Playground_center.x + Playground_width / 2 - radius_ball))
    {
        speed_l8.x = -speed_l8.x;
        if (ball_l8.x < (Playground_center.x - Playground_width / 2 + radius_ball))
            ball_l8.x = (Playground_center.x - Playground_width / 2 + radius_ball);
        if (ball_l8.x > (Playground_center.x + Playground_width / 2 - radius_ball))
            ball_l8.x = (Playground_center.x + Playground_width / 2 - radius_ball);
    }
    if (ball_l8.y < (Playground_center.y - Playground_height / 2 + radius_ball) || ball_l8.y > (Playground_center.y + Playground_height / 2 - radius_ball))
    {
        speed_l8.y = -speed_l8.y;
        if (ball_l8.y < (Playground_center.y - Playground_height / 2 + radius_ball))
            ball_l8.y = (Playground_center.y - Playground_height / 2 + radius_ball);
        if (ball_l8.y > (Playground_center.y + Playground_height / 2 - radius_ball))
            ball_l8.y = (Playground_center.y + Playground_height / 2 - radius_ball);
    }

    // collision
    if (CheckCollisionCircleRec(ball_l8, radius_ball, obstacle_top_l8))
    {
        Vector2 collision_point;
        collision_point.x = Clamp(ball_l8.x, (Playground_center.x - Playground_width / 2 + 3), (Playground_center.x - Playground_width / 2 + 3 + obstacle_width_l8));
        collision_point.y = Clamp(ball_l8.y, (Playground_center.y - Playground_height / 2 + Playground_height / 3), (Playground_center.y - Playground_height / 2 + Playground_height / 3 + obstacle_height_l8));
        Vector2 normal = Vector2Normalize(Vector2Subtract(ball_l8, collision_point));
        speed_l8 = Vector2Reflect(speed_l8, normal);
        ball_l8 = Vector2Add(collision_point, Vector2Scale(normal, radius_ball));
    }

    if (CheckCollisionCircleRec(ball_l8, radius_ball, obstacle_bottom_l8))
    {
        Vector2 collision_point;
        collision_point.x = Clamp(ball_l8.x, (Playground_center.x - Playground_width / 2 + Playground_width / 3 - 1), (Playground_center.x - Playground_width / 2 + Playground_width / 3 - 1 + obstacle_width_l8));
        collision_point.y = Clamp(ball_l8.y, (Playground_center.y - Playground_height / 2 + Playground_height * 2 / 3), (Playground_center.y - Playground_height / 2 + Playground_height * 2 / 3 + obstacle_height_l8));
        Vector2 normal = Vector2Normalize(Vector2Subtract(ball_l8, collision_point));
        speed_l8 = Vector2Reflect(speed_l8, normal);
        ball_l8 = Vector2Add(collision_point, Vector2Scale(normal, radius_ball));
    }

    // proportional deceleration
    float deceleration = 100.00;
    if (fabsf(speed_l8.x) >= fabsf(speed_l8.y) && speed_l8.x != 0)
    {
        if (speed_l8.y > 0)
            speed_l8.y -= deceleration * dt * fabsf(speed_l8.y / speed_l8.x);
        if (speed_l8.y < 0)
            speed_l8.y += deceleration * dt * fabsf(speed_l8.y / speed_l8.x);
        if (speed_l8.x > 0)
            speed_l8.x -= deceleration * dt;
        if (speed_l8.x < 0)
            speed_l8.x += deceleration * dt;
    }
    else if (fabsf(speed_l8.x) < fabsf(speed_l8.y) && speed_l8.y != 0)
    {
        if (speed_l8.x > 0)
            speed_l8.x -= deceleration * dt * fabsf(speed_l8.x / speed_l8.y);
        if (speed_l8.x < 0)
            speed_l8.x += deceleration * dt * fabsf(speed_l8.x / speed_l8.y);
        if (speed_l8.y > 0)
            speed_l8.y -= deceleration * dt;
        if (speed_l8.y < 0)
            speed_l8.y += deceleration * dt;
    }

    if (speed_l8.x < 5 && speed_l8.x > -5)
        speed_l8.x = 0;
    if (speed_l8.y < 5 && speed_l8.y > -5)
        speed_l8.y = 0;

    // areal decceleration
    deccelerated_area_center_l8 = Playground_center.y;
    if (ball_l8.y > deccelerated_area_center_l8 - deccelerated_area_width_l8 / 2 && ball_l8.y < deccelerated_area_center_l8 + deccelerated_area_width_l8 / 2)
        speed_l8.y = speed_l8.y + deccelerated_area_decceleration_l8 * dt;

    // shooting
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && speed_l8.x == 0 && speed_l8.y == 0)
    {
        stroke++;
        int mouse_x = GetMouseX();
        int mouse_y = GetMouseY();
        int drag_x = ball_l8.x - mouse_x;
        int drag_y = ball_l8.y - mouse_y;
        speed_l8.x = drag_x * 2;
        if (fabsf(speed_l8.x) > max_speed)
            speed_l8.x = speed_l8.x / fabsf(speed_l8.x) * max_speed;
        speed_l8.y = drag_y * 2;
        if (fabsf(speed_l8.y) > max_speed)
            speed_l8.y = speed_l8.y / fabsf(speed_l8.y) * max_speed;
    }

    // score
    if ((ball_l8.x > pot.x - 3 * radius_pot / 4) && (ball_l8.x < pot.x + 3 * radius_pot / 4) && (ball_l8.y > pot.y - 3 * radius_pot / 4) && (ball_l8.y < pot.y + 3 * radius_pot / 4))
    {
        ball_l8.x = width_f / 2;
        ball_l8.y = 4 * height_f / 5;
        speed_l8.x = 0;
        speed_l8.y = 0;
        level++;
        return;
    }

    // BeginDrawing();

    DrawRectangleRec(Playground, GetColor(0x00F000FF));
    DrawRectangleLinesEx(Playground, 5, BLACK);
    DrawRectangle(Playground_center.x - Playground_width / 2 + 4, deccelerated_area_center_l8 - deccelerated_area_width_l8 / 2, Playground_width - 8, deccelerated_area_width_l8, RED);
    DrawRectangleLines(Playground_center.x - Playground_width / 2 + 4, deccelerated_area_center_l8 - deccelerated_area_width_l8 / 2, Playground_width - 8, deccelerated_area_width_l8, BLACK);
    Vector2 arrowdown1 = {Playground_center.x - Playground_width / 2 + Playground_width / 4, deccelerated_area_center_l8};
    Vector2 arrowdown2 = {Playground_center.x - Playground_width / 2 + 2 * Playground_width / 4, deccelerated_area_center_l8};
    Vector2 arrowdown3 = {Playground_center.x - Playground_width / 2 + 3 * Playground_width / 4, deccelerated_area_center_l8};
    DrawPoly(arrowdown1, 3, 7, 90, BLACK);
    DrawPoly(arrowdown2, 3, 7, 90, BLACK);
    DrawPoly(arrowdown3, 3, 7, 90, BLACK);
    DrawCircle(pot.x, pot.y, radius_pot, BLACK);
    DrawCircle(ball_l8.x, ball_l8.y, radius_ball, WHITE);
    DrawCircleLines(pot.x, pot.y, radius_pot + 1, WHITE);
    DrawCircleLines(ball_l8.x, ball_l8.y, radius_ball + 1, BLACK);
    // DrawRectangleRec(obstacle,RED);
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && speed_l8.x == 0 && speed_l8.y == 0)
    {
        Vector2 mouse = {GetMouseX(), GetMouseY()};
        Vector2 shoot_direction = Vector2Normalize(Vector2Subtract(ball_l8, mouse));
        int pixel_size = 30;
        int dash_count = Vector2Distance(mouse, ball_l8) / pixel_size;

        for (int i = 0; i < dash_count; i += 2)
        {
            Vector2 start = {ball_l8.x + shoot_direction.x * (radius_ball + i * pixel_size), ball_l8.y + shoot_direction.y * (radius_ball + i * pixel_size)};
            Vector2 end = {ball_l8.x + shoot_direction.x * (radius_ball + (i + 1) * pixel_size), ball_l8.y + shoot_direction.y * (radius_ball + (i + 1) * pixel_size)};
            DrawLineV(start, end, BLACK);
        }
    }
    // DrawRectangleLines(width_f/2-obstacle_edge/2,height_f/2-obstacle_edge/2,obstacle_edge,obstacle_edge,BLACK);

    DrawRectangleRec(obstacle_top_l8, RED);
    DrawRectangleRec(obstacle_bottom_l8, RED);
    DrawRectangleLinesEx(obstacle_top_l8, 3, BLACK);
    DrawRectangleLinesEx(obstacle_bottom_l8, 3, BLACK);
    // EndDrawing();
}

// level 09
Vector2 ball_l9 = {width_f / 2, 4 * height_f / 5};
Vector2 speed_l9 = {0, 0};
#define obstacle_width_l9 133.33
#define obstacle_height_l9 15
float obstacle_position_x_l9 = (width_f / 2 - width_f / 8 + 3);
int obstacle_speed_l9 = 50;
Rectangle obstacle_top_l9;

void play_level_09()
{
    float dt = GetFrameTime();
    ball_l9 = Vector2Add(ball_l9, Vector2Scale(speed_l9, dt));

    // wall bounce
    if (ball_l9.x < (Playground_center.x - Playground_width / 2 + radius_ball) || ball_l9.x > (Playground_center.x + Playground_width / 2 - radius_ball))
    {
        speed_l9.x = -speed_l9.x;
        if (ball_l9.x < (Playground_center.x - Playground_width / 2 + radius_ball))
            ball_l9.x = (Playground_center.x - Playground_width / 2 + radius_ball);
        if (ball_l9.x > (Playground_center.x + Playground_width / 2 - radius_ball))
            ball_l9.x = (Playground_center.x + Playground_width / 2 - radius_ball);
    }
    if (ball_l9.y < (Playground_center.y - Playground_height / 2 + radius_ball) || ball_l9.y > (Playground_center.y + Playground_height / 2 - radius_ball))
    {
        speed_l9.y = -speed_l9.y;
        if (ball_l9.y < (Playground_center.y - Playground_height / 2 + radius_ball))
            ball_l9.y = (Playground_center.y - Playground_height / 2 + radius_ball);
        if (ball_l9.y > (Playground_center.y + Playground_height / 2 - radius_ball))
            ball_l9.y = (Playground_center.y + Playground_height / 2 - radius_ball);
    }

    // collision
    if (CheckCollisionCircleRec(ball_l9, radius_ball, obstacle_top_l9))
    {
        Vector2 collision_point;
        collision_point.x = Clamp(ball_l9.x, obstacle_position_x_l9, (obstacle_position_x_l9 + obstacle_width_l9));
        collision_point.y = Clamp(ball_l9.y, (Playground_center.y - Playground_height / 2 + Playground_height / 3), (Playground_center.y - Playground_height / 2 + Playground_height / 3 + obstacle_height_l9));
        Vector2 normal = Vector2Normalize(Vector2Subtract(ball_l9, collision_point));
        speed_l9 = Vector2Reflect(speed_l9, normal);
        ball_l9 = Vector2Add(collision_point, Vector2Scale(normal, radius_ball));
    }

    // obstacle moving
    obstacle_position_x_l9 = obstacle_position_x_l9 + obstacle_speed_l9 * dt;
    if (obstacle_position_x_l9 >= (Playground_center.x + Playground_width / 2 - obstacle_width_l9 - 1) || obstacle_position_x_l9 < (Playground_center.x - Playground_width / 2))
        obstacle_speed_l9 = -obstacle_speed_l9;
    obstacle_top_l9.x = obstacle_position_x_l9;

    // proportional deceleration
    float deceleration = 100.00;
    if (fabsf(speed_l9.x) >= fabsf(speed_l9.y) && speed_l9.x != 0)
    {
        if (speed_l9.y > 0)
            speed_l9.y -= deceleration * dt * fabsf(speed_l9.y / speed_l9.x);
        if (speed_l9.y < 0)
            speed_l9.y += deceleration * dt * fabsf(speed_l9.y / speed_l9.x);
        if (speed_l9.x > 0)
            speed_l9.x -= deceleration * dt;
        if (speed_l9.x < 0)
            speed_l9.x += deceleration * dt;
    }
    else if (fabsf(speed_l9.x) < fabsf(speed_l9.y) && speed_l9.y != 0)
    {
        if (speed_l9.x > 0)
            speed_l9.x -= deceleration * dt * fabsf(speed_l9.x / speed_l9.y);
        if (speed_l9.x < 0)
            speed_l9.x += deceleration * dt * fabsf(speed_l9.x / speed_l9.y);
        if (speed_l9.y > 0)
            speed_l9.y -= deceleration * dt;
        if (speed_l9.y < 0)
            speed_l9.y += deceleration * dt;
    }

    if (speed_l9.x < 5 && speed_l9.x > -5)
        speed_l9.x = 0;
    if (speed_l9.y < 5 && speed_l9.y > -5)
        speed_l9.y = 0;

    // shooting
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && speed_l9.x == 0 && speed_l9.y == 0)
    {
        stroke++;
        int mouse_x = GetMouseX();
        int mouse_y = GetMouseY();
        int drag_x = ball_l9.x - mouse_x;
        int drag_y = ball_l9.y - mouse_y;
        speed_l9.x = drag_x * 2;
        if (fabsf(speed_l9.x) > max_speed)
            speed_l9.x = speed_l9.x / fabsf(speed_l9.x) * max_speed;
        speed_l9.y = drag_y * 2;
        if (fabsf(speed_l9.y) > max_speed)
            speed_l9.y = speed_l9.y / fabsf(speed_l9.y) * max_speed;
    }

    // score
    if ((ball_l9.x > pot.x - 3 * radius_pot / 4) && (ball_l9.x < pot.x + 3 * radius_pot / 4) && (ball_l9.y > pot.y - 3 * radius_pot / 4) && (ball_l9.y < pot.y + 3 * radius_pot / 4))
    {
        ball_l9.x = width_f / 2;
        ball_l9.y = 4 * height_f / 5;
        speed_l9.x = 0;
        speed_l9.y = 0;
        level++;
        return;
    }

    // BeginDrawing();

    DrawRectangleRec(Playground, GetColor(0x00F000FF));
    DrawCircle(pot.x, pot.y, radius_pot, BLACK);
    DrawCircle(ball_l9.x, ball_l9.y, radius_ball, WHITE);
    DrawRectangleLinesEx(Playground, 5, BLACK);
    DrawCircleLines(pot.x, pot.y, radius_pot + 1, WHITE);
    DrawCircleLines(ball_l9.x, ball_l9.y, radius_ball + 1, BLACK);
    // DrawRectangleRec(obstacle,RED);
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && speed_l9.x == 0 && speed_l9.y == 0)
    {
        Vector2 mouse = {GetMouseX(), GetMouseY()};
        Vector2 shoot_direction = Vector2Normalize(Vector2Subtract(ball_l9, mouse));
        int pixel_size = 30;
        int dash_count = Vector2Distance(mouse, ball_l9) / pixel_size;

        for (int i = 0; i < dash_count; i += 2)
        {
            Vector2 start = {ball_l9.x + shoot_direction.x * (radius_ball + i * pixel_size), ball_l9.y + shoot_direction.y * (radius_ball + i * pixel_size)};
            Vector2 end = {ball_l9.x + shoot_direction.x * (radius_ball + (i + 1) * pixel_size), ball_l9.y + shoot_direction.y * (radius_ball + (i + 1) * pixel_size)};
            DrawLineV(start, end, BLACK);
        }
    }
    // DrawRectangleLines(width_f/2-obstacle_edge/2,height_f/2-obstacle_edge/2,obstacle_edge,obstacle_edge,BLACK);

    DrawRectangleRec(obstacle_top_l9, RED);
    // DrawRectangleRec(obstacle_bottom,RED);
    DrawRectangleLinesEx(obstacle_top_l9, 3, BLACK);
    // DrawRectangleLinesEx(obstacle_bottom,3,BLACK);
    // EndDrawing();
}

// level 10
Vector2 ball_l10 = {width_f / 2, 4 * height_f / 5};
Vector2 speed_l10 = {0, 0};
#define obstacle_width_l10 133.33
#define obstacle_height_l10 15
float obstacle_position_x_l10 = (width_f / 2 - width_f / 8 + 3);
int obstacle_speed_l10 = 50;
int deccelerated_area_width_l10 = 60;
int deccelerated_area_center_l10;
int deccelerated_area_decceleration_l10 = 1000;
Rectangle obstacle_top_l10;

void play_level_10()
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
        stroke++;
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
        ball_l10.x = width_f / 2;
        ball_l10.y = 4 * height_f / 5;
        speed_l10.x = 0;
        speed_l10.y = 0;
        level++;
        return;
    }

    // BeginDrawing();

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
    // DrawRectangleLines(width_f/2-obstacle_edge/2,height_f/2-obstacle_edge/2,obstacle_edge,obstacle_edge,BLACK);

    DrawRectangleRec(obstacle_top_l10, RED);
    // DrawRectangleRec(obstacle_bottom,RED);
    DrawRectangleLinesEx(obstacle_top_l10, 3, BLACK);
    // DrawRectangleLinesEx(obstacle_bottom,3,BLACK);
    // EndDrawing();
}

// level 11
Vector2 ball_l11 = {width_f / 2, 4 * height_f / 5};
Vector2 speed_l11 = {0, 0};
#define obstacle_width_l11 90
#define obstacle_height_l11 15
int obstacle_top_speed_l11 = 70;
int obstacle_bottom_speed_l11 = -70;
float obstacle_top_position_x_l11 = (width_f / 2 - width_f / 8 + 3);
float obstacle_bottom_position_x_l11 = (width_f / 2 + width_f / 8 - obstacle_width_l11 - 3);
Rectangle obstacle_top_l11;
Rectangle obstacle_bottom_l11;

void play_level_11()
{
    float dt = GetFrameTime();
    ball_l11 = Vector2Add(ball_l11, Vector2Scale(speed_l11, dt));

    // wall bounce
    if (ball_l11.x < (Playground_center.x - Playground_width / 2 + radius_ball) || ball_l11.x > (Playground_center.x + Playground_width / 2 - radius_ball))
    {
        speed_l11.x = -speed_l11.x;
        if (ball_l11.x < (Playground_center.x - Playground_width / 2 + radius_ball))
            ball_l11.x = (Playground_center.x - Playground_width / 2 + radius_ball);
        if (ball_l11.x > (Playground_center.x + Playground_width / 2 - radius_ball))
            ball_l11.x = (Playground_center.x + Playground_width / 2 - radius_ball);
    }
    if (ball_l11.y < (Playground_center.y - Playground_height / 2 + radius_ball) || ball_l11.y > (Playground_center.y + Playground_height / 2 - radius_ball))
    {
        speed_l11.y = -speed_l11.y;
        if (ball_l11.y < (Playground_center.y - Playground_height / 2 + radius_ball))
            ball_l11.y = (Playground_center.y - Playground_height / 2 + radius_ball);
        if (ball_l11.y > (Playground_center.y + Playground_height / 2 - radius_ball))
            ball_l11.y = (Playground_center.y + Playground_height / 2 - radius_ball);
    }

    // collision
    if (CheckCollisionCircleRec(ball_l11, radius_ball, obstacle_top_l11))
    {
        Vector2 collision_point;
        collision_point.x = Clamp(ball_l11.x, obstacle_top_position_x_l11, (obstacle_top_position_x_l11 + obstacle_width_l11));
        collision_point.y = Clamp(ball_l11.y, (Playground_center.y - Playground_height / 2 + Playground_height / 3), (Playground_center.y - Playground_height / 2 + Playground_height / 3 + obstacle_height_l11));
        Vector2 normal = Vector2Normalize(Vector2Subtract(ball_l11, collision_point));
        speed_l11 = Vector2Reflect(speed_l11, normal);
        ball_l11 = Vector2Add(collision_point, Vector2Scale(normal, radius_ball));
    }

    if (CheckCollisionCircleRec(ball_l11, radius_ball, obstacle_bottom_l11))
    {
        Vector2 collision_point;
        collision_point.x = Clamp(ball_l11.x, obstacle_bottom_position_x_l11, (obstacle_bottom_position_x_l11 + obstacle_width_l11));
        collision_point.y = Clamp(ball_l11.y, (Playground_center.y - Playground_height / 2 + Playground_height * 2 / 3), (Playground_center.y - Playground_height / 2 + Playground_height * 2 / 3 + obstacle_height_l11));
        Vector2 normal = Vector2Normalize(Vector2Subtract(ball_l11, collision_point));
        speed_l11 = Vector2Reflect(speed_l11, normal);
        ball_l11 = Vector2Add(collision_point, Vector2Scale(normal, radius_ball));
    }

    // obstacle moving
    obstacle_top_position_x_l11 = obstacle_top_position_x_l11 + obstacle_top_speed_l11 * dt;
    if (obstacle_top_position_x_l11 >= (Playground_center.x + Playground_width / 2 - obstacle_width_l11 - 1) || obstacle_top_position_x_l11 < (Playground_center.x - Playground_width / 2))
        obstacle_top_speed_l11 = -obstacle_top_speed_l11;
    obstacle_top_l11.x = obstacle_top_position_x_l11;

    obstacle_bottom_position_x_l11 = obstacle_bottom_position_x_l11 + obstacle_bottom_speed_l11 * dt;
    if (obstacle_bottom_position_x_l11 >= (Playground_center.x + Playground_width / 2 - obstacle_width_l11 - 1) || obstacle_bottom_position_x_l11 < (Playground_center.x - Playground_width / 2))
        obstacle_bottom_speed_l11 = -obstacle_bottom_speed_l11;
    obstacle_bottom_l11.x = obstacle_bottom_position_x_l11;

    // proportional deceleration
    float deceleration = 100.00;
    if (fabsf(speed_l11.x) >= fabsf(speed_l11.y) && speed_l11.x != 0)
    {
        if (speed_l11.y > 0)
            speed_l11.y -= deceleration * dt * fabsf(speed_l11.y / speed_l11.x);
        if (speed_l11.y < 0)
            speed_l11.y += deceleration * dt * fabsf(speed_l11.y / speed_l11.x);
        if (speed_l11.x > 0)
            speed_l11.x -= deceleration * dt;
        if (speed_l11.x < 0)
            speed_l11.x += deceleration * dt;
    }
    else if (fabsf(speed_l11.x) < fabsf(speed_l11.y) && speed_l11.y != 0)
    {
        if (speed_l11.x > 0)
            speed_l11.x -= deceleration * dt * fabsf(speed_l11.x / speed_l11.y);
        if (speed_l11.x < 0)
            speed_l11.x += deceleration * dt * fabsf(speed_l11.x / speed_l11.y);
        if (speed_l11.y > 0)
            speed_l11.y -= deceleration * dt;
        if (speed_l11.y < 0)
            speed_l11.y += deceleration * dt;
    }

    if (speed_l11.x < 5 && speed_l11.x > -5)
        speed_l11.x = 0;
    if (speed_l11.y < 5 && speed_l11.y > -5)
        speed_l11.y = 0;

    // shooting
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && speed_l11.x == 0 && speed_l11.y == 0)
    {
        stroke++;
        int mouse_x = GetMouseX();
        int mouse_y = GetMouseY();
        int drag_x = ball_l11.x - mouse_x;
        int drag_y = ball_l11.y - mouse_y;
        speed_l11.x = drag_x * 2;
        if (fabsf(speed_l11.x) > max_speed)
            speed_l11.x = speed_l11.x / fabsf(speed_l11.x) * max_speed;
        speed_l11.y = drag_y * 2;
        if (fabsf(speed_l11.y) > max_speed)
            speed_l11.y = speed_l11.y / fabsf(speed_l11.y) * max_speed;
    }

    // score
    if ((ball_l11.x > pot.x - 3 * radius_pot / 4) && (ball_l11.x < pot.x + 3 * radius_pot / 4) && (ball_l11.y > pot.y - 3 * radius_pot / 4) && (ball_l11.y < pot.y + 3 * radius_pot / 4))
    {
        ball_l11.x = width_f / 2;
        ball_l11.y = 4 * height_f / 5;
        speed_l11.x = 0;
        speed_l11.y = 0;
        level++;
        return;
    }

    // BeginDrawing();

    DrawRectangleRec(Playground, GetColor(0x00F000FF));
    DrawCircle(pot.x, pot.y, radius_pot, BLACK);
    DrawCircle(ball_l11.x, ball_l11.y, radius_ball, WHITE);
    DrawRectangleLinesEx(Playground, 5, BLACK);
    DrawCircleLines(pot.x, pot.y, radius_pot + 1, WHITE);
    DrawCircleLines(ball_l11.x, ball_l11.y, radius_ball + 1, BLACK);
    // DrawRectangleRec(obstacle,RED);
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && speed_l11.x == 0 && speed_l11.y == 0)
    {
        Vector2 mouse = {GetMouseX(), GetMouseY()};
        Vector2 shoot_direction = Vector2Normalize(Vector2Subtract(ball_l11, mouse));
        int pixel_size = 30;
        int dash_count = Vector2Distance(mouse, ball_l11) / pixel_size;

        for (int i = 0; i < dash_count; i += 2)
        {
            Vector2 start = {ball_l11.x + shoot_direction.x * (radius_ball + i * pixel_size), ball_l11.y + shoot_direction.y * (radius_ball + i * pixel_size)};
            Vector2 end = {ball_l11.x + shoot_direction.x * (radius_ball + (i + 1) * pixel_size), ball_l11.y + shoot_direction.y * (radius_ball + (i + 1) * pixel_size)};
            DrawLineV(start, end, BLACK);
        }
    }
    // DrawRectangleLines(width_f/2-obstacle_edge/2,height_f/2-obstacle_edge/2,obstacle_edge,obstacle_edge,BLACK);

    DrawRectangleRec(obstacle_top_l11, RED);
    DrawRectangleRec(obstacle_bottom_l11, RED);
    DrawRectangleLinesEx(obstacle_top_l11, 3, BLACK);
    DrawRectangleLinesEx(obstacle_bottom_l11, 3, BLACK);

    // EndDrawing();
}

// level 12
Vector2 ball_l12 = {width_f / 2, 4 * height_f / 5};
Vector2 speed_l12 = {0, 0};
#define obstacle_width_l12 90
#define obstacle_height_l12 15
int obstacle_top_speed_l12 = 70;
int obstacle_bottom_speed_l12 = -70;
float obstacle_top_position_x_l12 = (width_f / 2 - width_f / 8 + 3);
float obstacle_bottom_position_x_l12 = (width_f / 2 + width_f / 8 - obstacle_width_l12 - 3);
int deccelerated_area_width_l12 = 46;
int deccelerated_area_center_l12;
int deccelerated_area_decceleration_l12 = 1000;
Rectangle obstacle_top_l12;
Rectangle obstacle_bottom_l12;

void play_level_12()
{
    float dt = GetFrameTime();
    ball_l12 = Vector2Add(ball_l12, Vector2Scale(speed_l12, dt));

    // wall bounce
    if (ball_l12.x < (Playground_center.x - Playground_width / 2 + radius_ball) || ball_l12.x > (Playground_center.x + Playground_width / 2 - radius_ball))
    {
        speed_l12.x = -speed_l12.x;
        if (ball_l12.x < (Playground_center.x - Playground_width / 2 + radius_ball))
            ball_l12.x = (Playground_center.x - Playground_width / 2 + radius_ball);
        if (ball_l12.x > (Playground_center.x + Playground_width / 2 - radius_ball))
            ball_l12.x = (Playground_center.x + Playground_width / 2 - radius_ball);
    }
    if (ball_l12.y < (Playground_center.y - Playground_height / 2 + radius_ball) || ball_l12.y > (Playground_center.y + Playground_height / 2 - radius_ball))
    {
        speed_l12.y = -speed_l12.y;
        if (ball_l12.y < (Playground_center.y - Playground_height / 2 + radius_ball))
            ball_l12.y = (Playground_center.y - Playground_height / 2 + radius_ball);
        if (ball_l12.y > (Playground_center.y + Playground_height / 2 - radius_ball))
            ball_l12.y = (Playground_center.y + Playground_height / 2 - radius_ball);
    }

    // collision
    if (CheckCollisionCircleRec(ball_l12, radius_ball, obstacle_top_l12))
    {
        Vector2 collision_point;
        collision_point.x = Clamp(ball_l12.x, obstacle_top_position_x_l12, (obstacle_top_position_x_l12 + obstacle_width_l12));
        collision_point.y = Clamp(ball_l12.y, (Playground_center.y - Playground_height / 2 + Playground_height / 3), (Playground_center.y - Playground_height / 2 + Playground_height / 3 + obstacle_height_l12));
        Vector2 normal = Vector2Normalize(Vector2Subtract(ball_l12, collision_point));
        speed_l12 = Vector2Reflect(speed_l12, normal);
        ball_l12 = Vector2Add(collision_point, Vector2Scale(normal, radius_ball));
    }

    if (CheckCollisionCircleRec(ball_l12, radius_ball, obstacle_bottom_l12))
    {
        Vector2 collision_point;
        collision_point.x = Clamp(ball_l12.x, obstacle_bottom_position_x_l12, (obstacle_bottom_position_x_l12 + obstacle_width_l12));
        collision_point.y = Clamp(ball_l12.y, (Playground_center.y - Playground_height / 2 + Playground_height * 2 / 3), (Playground_center.y - Playground_height / 2 + Playground_height * 2 / 3 + obstacle_height_l12));
        Vector2 normal = Vector2Normalize(Vector2Subtract(ball_l12, collision_point));
        speed_l12 = Vector2Reflect(speed_l12, normal);
        ball_l12 = Vector2Add(collision_point, Vector2Scale(normal, radius_ball));
    }

    // obstacle moving
    obstacle_top_position_x_l12 = obstacle_top_position_x_l12 + obstacle_top_speed_l12 * dt;
    if (obstacle_top_position_x_l12 >= (Playground_center.x + Playground_width / 2 - obstacle_width_l12 - 1) || obstacle_top_position_x_l12 < (Playground_center.x - Playground_width / 2))
        obstacle_top_speed_l12 = -obstacle_top_speed_l12;
    obstacle_top_l12.x = obstacle_top_position_x_l12;

    obstacle_bottom_position_x_l12 = obstacle_bottom_position_x_l12 + obstacle_bottom_speed_l12 * dt;
    if (obstacle_bottom_position_x_l12 >= (Playground_center.x + Playground_width / 2 - obstacle_width_l12 - 1) || obstacle_bottom_position_x_l12 < (Playground_center.x - Playground_width / 2))
        obstacle_bottom_speed_l12 = -obstacle_bottom_speed_l12;
    obstacle_bottom_l12.x = obstacle_bottom_position_x_l12;

    // proportional deceleration
    float deceleration = 100.00;
    if (fabsf(speed_l12.x) >= fabsf(speed_l12.y) && speed_l12.x != 0)
    {
        if (speed_l12.y > 0)
            speed_l12.y -= deceleration * dt * fabsf(speed_l12.y / speed_l12.x);
        if (speed_l12.y < 0)
            speed_l12.y += deceleration * dt * fabsf(speed_l12.y / speed_l12.x);
        if (speed_l12.x > 0)
            speed_l12.x -= deceleration * dt;
        if (speed_l12.x < 0)
            speed_l12.x += deceleration * dt;
    }
    else if (fabsf(speed_l12.x) < fabsf(speed_l12.y) && speed_l12.y != 0)
    {
        if (speed_l12.x > 0)
            speed_l12.x -= deceleration * dt * fabsf(speed_l12.x / speed_l12.y);
        if (speed_l12.x < 0)
            speed_l12.x += deceleration * dt * fabsf(speed_l12.x / speed_l12.y);
        if (speed_l12.y > 0)
            speed_l12.y -= deceleration * dt;
        if (speed_l12.y < 0)
            speed_l12.y += deceleration * dt;
    }

    if (speed_l12.x < 5 && speed_l12.x > -5)
        speed_l12.x = 0;
    if (speed_l12.y < 5 && speed_l12.y > -5)
        speed_l12.y = 0;

    // areal decceleration
    deccelerated_area_center_l12 = Playground_center.y;
    if (ball_l12.y > deccelerated_area_center_l12 - deccelerated_area_width_l12 / 2 && ball_l12.y < deccelerated_area_center_l12 + deccelerated_area_width_l12 / 2)
        speed_l12.y = speed_l12.y + deccelerated_area_decceleration_l12 * dt;

    // shooting
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && speed_l12.x == 0 && speed_l12.y == 0)
    {
        stroke++;
        int mouse_x = GetMouseX();
        int mouse_y = GetMouseY();
        int drag_x = ball_l12.x - mouse_x;
        int drag_y = ball_l12.y - mouse_y;
        speed_l12.x = drag_x * 2;
        if (fabsf(speed_l12.x) > max_speed)
            speed_l12.x = speed_l12.x / fabsf(speed_l12.x) * max_speed;
        speed_l12.y = drag_y * 2;
        if (fabsf(speed_l12.y) > max_speed)
            speed_l12.y = speed_l12.y / fabsf(speed_l12.y) * max_speed;
    }

    // score
    if ((ball_l12.x > pot.x - 3 * radius_pot / 4) && (ball_l12.x < pot.x + 3 * radius_pot / 4) && (ball_l12.y > pot.y - 3 * radius_pot / 4) && (ball_l12.y < pot.y + 3 * radius_pot / 4))
    {
        ball_l12.x = width_f / 2;
        ball_l12.y = 4 * height_f / 5;
        speed_l12.x = 0;
        speed_l12.y = 0;
        level++;
        return;
    }

    // BeginDrawing();

    DrawRectangleRec(Playground, GetColor(0x00F000FF));
    DrawRectangle(Playground_center.x - Playground_width / 2 + 4, deccelerated_area_center_l12 - deccelerated_area_width_l12 / 2, Playground_width - 8, deccelerated_area_width_l12, RED);
    DrawRectangleLines(Playground_center.x - Playground_width / 2 + 4, deccelerated_area_center_l12 - deccelerated_area_width_l12 / 2, Playground_width - 8, deccelerated_area_width_l12, BLACK);
    Vector2 arrowdown1 = {Playground_center.x - Playground_width / 2 + Playground_width / 4, deccelerated_area_center_l12};
    Vector2 arrowdown2 = {Playground_center.x - Playground_width / 2 + 2 * Playground_width / 4, deccelerated_area_center_l12};
    Vector2 arrowdown3 = {Playground_center.x - Playground_width / 2 + 3 * Playground_width / 4, deccelerated_area_center_l12};
    DrawPoly(arrowdown1, 3, 7, 90, BLACK);
    DrawPoly(arrowdown2, 3, 7, 90, BLACK);
    DrawPoly(arrowdown3, 3, 7, 90, BLACK);
    DrawCircle(pot.x, pot.y, radius_pot, BLACK);
    DrawCircle(ball_l12.x, ball_l12.y, radius_ball, WHITE);
    DrawRectangleLinesEx(Playground, 5, BLACK);
    DrawCircleLines(pot.x, pot.y, radius_pot + 1, WHITE);
    DrawCircleLines(ball_l12.x, ball_l12.y, radius_ball + 1, BLACK);

    // DrawRectangleRec(obstacle,RED);
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && speed_l12.x == 0 && speed_l12.y == 0)
    {
        Vector2 mouse = {GetMouseX(), GetMouseY()};
        Vector2 shoot_direction = Vector2Normalize(Vector2Subtract(ball_l12, mouse));
        int pixel_size = 30;
        int dash_count = Vector2Distance(mouse, ball_l12) / pixel_size;

        for (int i = 0; i < dash_count; i += 2)
        {
            Vector2 start = {ball_l12.x + shoot_direction.x * (radius_ball + i * pixel_size), ball_l12.y + shoot_direction.y * (radius_ball + i * pixel_size)};
            Vector2 end = {ball_l12.x + shoot_direction.x * (radius_ball + (i + 1) * pixel_size), ball_l12.y + shoot_direction.y * (radius_ball + (i + 1) * pixel_size)};
            DrawLineV(start, end, BLACK);
        }
    }
    // DrawRectangleLines(width_f/2-obstacle_edge/2,height_f/2-obstacle_edge/2,obstacle_edge,obstacle_edge,BLACK);

    DrawRectangleRec(obstacle_top_l12, RED);
    DrawRectangleRec(obstacle_bottom_l12, RED);
    DrawRectangleLinesEx(obstacle_top_l12, 3, BLACK);
    DrawRectangleLinesEx(obstacle_bottom_l12, 3, BLACK);

    // EndDrawing();
}

int main()
{
    obstacle_top_l7 = (Rectangle){(Playground_center.x - Playground_width / 2 + 3), (Playground_center.y - Playground_height / 2 + Playground_height / 3), obstacle_width_l7, obstacle_height_l7};
    obstacle_bottom_l7 = (Rectangle){(Playground_center.x - Playground_width / 2 + Playground_width / 3 - 1), (Playground_center.y - Playground_height / 2 + Playground_height * 2 / 3), obstacle_width_l7, obstacle_height_l7};
    obstacle_top_l8 = (Rectangle){(Playground_center.x - Playground_width / 2 + 3), (Playground_center.y - Playground_height / 2 + Playground_height / 3), obstacle_width_l8, obstacle_height_l8};
    obstacle_bottom_l8 = (Rectangle){(Playground_center.x - Playground_width / 2 + Playground_width / 3 - 1), (Playground_center.y - Playground_height / 2 + Playground_height * 2 / 3), obstacle_width_l8, obstacle_height_l8};
    obstacle_top_l9 = (Rectangle){obstacle_position_x_l9, (Playground_center.y - Playground_height / 2 + Playground_height / 3), obstacle_width_l9, obstacle_height_l9};
    obstacle_top_l10 = (Rectangle){obstacle_position_x_l10, (Playground_center.y - Playground_height / 2 + Playground_height / 3), obstacle_width_l10, obstacle_height_l10};
    obstacle_top_l11 = (Rectangle){obstacle_top_position_x_l11, (Playground_center.y - Playground_height / 2 + Playground_height / 3), obstacle_width_l11, obstacle_height_l11};
    obstacle_bottom_l11 = (Rectangle){obstacle_bottom_position_x_l11, (Playground_center.y + Playground_height / 2 - Playground_height / 3), obstacle_width_l11, obstacle_height_l11};
    obstacle_top_l12 = (Rectangle){obstacle_top_position_x_l12, (Playground_center.y - Playground_height / 2 + Playground_height / 3), obstacle_width_l12, obstacle_height_l12};
    obstacle_bottom_l12 = (Rectangle){obstacle_bottom_position_x_l12, (Playground_center.y + Playground_height / 2 - Playground_height / 3), obstacle_width_l12, obstacle_height_l12};

    load_menu_assets();

    InitWindow(width_f, height_f, "practice");
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(SKYBLUE);
        menu_view();
        if (play)
        {
            if (level == 1)
                play_level_01();
            else if (level == 2)
                play_level_02();
            else if (level == 3)
                play_level_03();
            else if (level == 4)
                play_level_04();
            else if (level == 5)
                play_level_05();
            else if (level == 6)
                play_level_06();
            else if (level == 7)
                play_level_07();
            else if (level == 8)
                play_level_08();
            else if (level == 9)
                play_level_09();
            else if (level == 10)
                play_level_10();
            else if (level == 11)
                play_level_11();
            else if (level == 12)
                play_level_12();
        }
        else
        {
            for (int i = 0; i < 3; i++)
            {
                UnloadTexture(menu[i]);
                UnloadTexture(start[i]);
            }
        }

        // scoreboard
        score = 3 * (level - 1) - stroke;
        if (level > 12)
        {
            DrawRectangle(0, 0, width_f, height_f, SKYBLUE);
            DrawText("THE END!", width_f / 2 - 100, height_f / 2 - 30, 45, BLACK);
        }
        else if (play)
        {
            DrawRectangle(width_f / 5 - 75, height_f / 2 - 25, 150, 50, WHITE);
            DrawText(TextFormat("Level: %d", level), width_f / 5 - 75 + 15, height_f / 2 - 25 + 15, 30, BLACK);
            DrawRectangle(width_f * 4 / 5 - 75, height_f / 2 - 25, 175, 50, WHITE);
            DrawText(TextFormat("Score: %d", score), width_f * 4 / 5 - 75 + 15, height_f / 2 - 25 + 15, 30, BLACK);
        }

        EndDrawing();
    }
    CloseWindow();
}
