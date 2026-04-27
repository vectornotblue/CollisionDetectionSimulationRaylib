#include <raylib.h>

const int WIDTH = 900;
const int HEIGHT = 900;


int main() 
{
    InitWindow(WIDTH, HEIGHT, "Collision Detection Simulation Raylib");
    SetTargetFPS(60);
    
    while (!WindowShouldClose())
    {
        
        BeginDrawing();
            ClearBackground(BLACK);
        EndDrawing();
    }
    
    CloseWindow();
}