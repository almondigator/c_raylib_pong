#include <raylib.h>
#include <raymath.h>
#include <stdio.h>


int main(void)
{
    char *w_title = "window title";
    int screen_resolution_x = 800;
    int screen_resolution_y = 500;

    int virtual_resolution_x = 180;
    int virtual_resolution_y = 90;

    float virtual_ratio = (float)screen_resolution_x/(float)screen_resolution_y;

    InitWindow(screen_resolution_x, screen_resolution_y, w_title);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Do you realize?", screen_resolution_x/2, screen_resolution_y/2, 12, BLACK);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}