#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

#define screen_width 800
#define screen_height 800

#define MENU_SCALE 3.0f
#define BUTTON_SCALE 2.0f
#define BUTTON_SPACING 100.0f

int Playground_height = screen_height * 3 / 4;
Vector2 Playground_center = {screen_width / 2, screen_height / 2};

int main()
{
    InitWindow(screen_width, screen_height, "practice");
    SetTargetFPS(60);
    InitAudioDevice();
    Sound hitSound = LoadSound("assets/audio/jump.wav");

    // Menu background
    Texture2D menu_background = LoadTexture("assets/not-ball/start.png");

    // Menu box
    Texture2D menu[3];
    menu[0] = LoadTexture("assets/not-ball/tile_0056.png");
    menu[1] = LoadTexture("assets/not-ball/tile_0057.png");
    menu[2] = LoadTexture("assets/not-ball/tile_0058.png");

    // Start box
    Texture2D start[3];
    start[0] = LoadTexture("assets/not-ball/tile_0069.png");
    start[1] = LoadTexture("assets/not-ball/tile_0070.png");
    start[2] = LoadTexture("assets/not-ball/tile_0071.png");

    printf("%d", menu_background.width);
    while (!WindowShouldClose())
    {
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
                // Start game
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
                EndDrawing();

                for (int i = 0; i < 3; i++)
                {
                    UnloadTexture(menu[i]);
                    UnloadTexture(start[i]);
                }
                UnloadSound(hitSound);
                CloseAudioDevice();
                CloseWindow();

                return 0;
            }
        }

        BeginDrawing();
        ClearBackground(BLUE);

        // Background
        // DrawTexturePro(menu_background,
        //                (Rectangle){0, 0, menu_background.width, menu_background.height},
        //                (Rectangle){0, 0, screen_width, screen_height},
        //                Vector2Zero(), 0.0f, WHITE);

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

        char menu[] = "MENU";
        int font_size = 30;
        int text_width = MeasureText(menu, font_size);
        int text_x = menu_x + (int)((total_width - text_width) / 2.0f);
        int text_y = menu_y + (int)((piece_height - font_size) / 2.0f);
        DrawText(menu, text_x, text_y - Playground_height / 4, font_size, WHITE);

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

        EndDrawing();
    }
    for (int i = 0; i < 3; i++)
    {
        UnloadTexture(menu[i]);
        UnloadTexture(start[i]);
    }
    UnloadSound(hitSound);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}