#include <raylib.h>
#include <vector>

const int WIDTH = 900;
const int HEIGHT = 900;
bool isPaused = true;
float ballMaxVelocity = 100.0f;
class Ball{
public:
        Vector2 position, velocity;
        float size;
        Color color;
    Ball(Vector2 position, Vector2 velocity, float size, Color color){
        this->position = position;
        this->velocity = velocity;
        this->size = size;
        this->color = color;
    }
    void draw_ball(){
        DrawCircle(position.x, position.y, size, color);
    }
    void move_ball(float deltaTime){
        check_Wall_Colission();
        position = (Vector2){velocity.x*deltaTime+position.x, velocity.y*deltaTime+position.y};
    }
    void check_Wall_Colission(){
        if((position.x-size)<=0 || (position.x+size)>=WIDTH){
            velocity.x = -velocity.x;
        }
        if((position.y-size)<=0 || (position.y+size)>=HEIGHT){
            velocity.y = -velocity.y;
        }
    }
    
};

int main() 
{
    InitWindow(WIDTH, HEIGHT, "Collision Detection Simulation Raylib");
    SetTargetFPS(60);
    int ballCount = 10;
    int ballSize = 20;
    std::vector<Ball> balls;
    balls.reserve(ballCount);
    for(int i = 0; i<ballCount; i++){
        Vector2 randomPos = (Vector2){(float)GetRandomValue(0,(int)WIDTH), (float)GetRandomValue(0,(int)HEIGHT)};
        Vector2 randomVel = (Vector2){(float)GetRandomValue(0,ballMaxVelocity), (float)GetRandomValue(0,ballMaxVelocity)};
        balls.emplace_back(randomPos, randomVel, ballSize, BLUE);
    }
    while (!WindowShouldClose())
    {
        if(IsKeyPressed(KEY_SPACE)){
            isPaused = !isPaused;
        }
        float deltaTime = GetFrameTime();
        BeginDrawing();
            ClearBackground(BLACK);
            for(auto& ball : balls){
                if(!isPaused){
                    ball.move_ball(deltaTime);
                }
                ball.draw_ball();
            }
        EndDrawing();
    }
    
    CloseWindow();
}