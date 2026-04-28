#include <raylib.h>
#include <vector>
#include <math.h>

const int WIDTH = 900;
const int HEIGHT = 900;
bool isPaused = true;
float ballMaxVelocity = 100.0f;
int ballCount = 10;
int ballSize = 20;


class Ball{
public:
        Vector2 position, velocity;
        float size, mass;
        Color color;
    Ball(Vector2 position, Vector2 velocity, float size, Color color){
        this->position = position;
        this->velocity = velocity;
        this->size = size;
        mass = pow(size,2);
        this->color = color;
    }
    void draw_ball(){
        DrawCircle(position.x, position.y, size, color);
    }
    void move_ball(float deltaTime){
        check_Wall_Collision();
        position = (Vector2){velocity.x*deltaTime+position.x, velocity.y*deltaTime+position.y};
    }
    void check_Wall_Collision(){
        if((position.x-size)<=0 || (position.x+size)>=WIDTH){
            velocity.x = -velocity.x;
        }
        if((position.y-size)<=0 || (position.y+size)>=HEIGHT){
            velocity.y = -velocity.y;
        }
    }
    Vector2 get_Position() { return position;}
    Vector2 get_Velocity() { return velocity;}
    float get_Size() { return size;}
    void set_Velocity(Vector2 newVelocity){
        velocity = newVelocity;
    }
    void set_Position(Vector2 newPosition){
        position = newPosition;
    }
    
    
};

std::vector<Ball> balls;

void SpawnBalls(){
    balls.reserve(ballCount);
    for(int i = 0; i<ballCount; i++){
        Vector2 randomPos = (Vector2){(float)GetRandomValue(ballSize, WIDTH-ballSize), (float)GetRandomValue(ballSize,HEIGHT-ballSize)};
        Vector2 randomVel = (Vector2){(float)GetRandomValue(-ballMaxVelocity,ballMaxVelocity), (float)GetRandomValue(-ballMaxVelocity,ballMaxVelocity)};
        balls.emplace_back(randomPos, randomVel, ballSize, BLUE);
    }
}

void CollideBalls(int i, int j, float overlap){

    Vector2 vel1 = balls[i].get_Velocity();//beginning velocity
    Vector2 vel2 = balls[j].get_Velocity();
    Vector2 pos1 = balls[i].get_Position();
    Vector2 pos2 = balls[j].get_Position();
    float mass1 = pow(balls[i].get_Size(),2);
    float mass2 = pow(balls[j].get_Size(),2);
    float mn = sqrt(pow(pos2.x-pos1.x,2) + pow(pos2.y-pos1.y,2)); //magnitude of normal
    Vector2 un = (Vector2){(pos2.x-pos1.x)/mn,(pos2.y-pos1.y)/mn}; //unified normal
    Vector2 ut = (Vector2){-un.y, un.x}; //unified tangent;
    
    //Bottom part implemented by following Chad Berchek's 2-Dimensional Elastic Collisions without Trigonometry solution
    //change velocity
    float vvel1n = un.x*vel1.x + un.y*vel1.y; // normal before scallar
    float vvel1t = ut.x*vel1.x + ut.y*vel1.y; //tangent components don't change
    float vvel2n = un.x*vel2.x + un.y*vel2.y;
    float vvel2t = ut.x*vel2.x + ut.y*vel2.y;
    float vvel1nf = (vvel1n*(mass1-mass2) + 2*mass2*vvel2n)/(mass1 + mass2); //normal final scallar
    float vvel2nf = (vvel2n*(mass2-mass1) + 2*mass1*vvel1n)/(mass1 + mass2); 
    Vector2 vel1n = (Vector2){un.x*vvel1nf, un.y*vvel1nf};//normal final vector
    Vector2 vel2n = (Vector2){un.x*vvel2nf, un.y*vvel2nf};
    Vector2 vel1t = (Vector2){ut.x*vvel1t, ut.y*vvel1t};//tangent final vector
    Vector2 vel2t = (Vector2){ut.x*vvel2t, ut.y*vvel2t};
    Vector2 vel1f = (Vector2){vel1n.x+vel1t.x, vel1n.y+vel1t.y};//final vector = final normal + final tangent components
    Vector2 vel2f = (Vector2){vel2n.x+vel2t.x, vel2n.y+vel2t.y};
    balls[i].set_Velocity(vel1f);
    balls[j].set_Velocity(vel2f);
    //fix overlap
    float pos1fix = -1*overlap*(mass2/(mass1+mass2));
    float pos2fix = overlap*(mass1/(mass1+mass2));
    balls[i].set_Position((Vector2){pos1.x+pos1fix*un.x, pos1.y+pos1fix*un.y});
    balls[j].set_Position((Vector2){pos2.x+pos2fix*un.x, pos2.y+pos2fix*un.y});
}

void CollisionsCheck(){
    for(int i = 0; i < ballCount; i++){
        for (int j = 1; j < ballCount; j++){
            if(i == j){
                continue;
            }
            Vector2 ball1pos = balls[i].get_Position(); 
            float ball1size = balls[i].get_Size();
            Vector2 ball2pos = balls[j].get_Position();
            float ball2size = balls[j].get_Size();
            float overlap = (ball1size+ball2size) - sqrt(pow(ball1pos.x-ball2pos.x, 2)+pow(ball1pos.y-ball2pos.y, 2));
            if(overlap>= 0){
                CollideBalls(i, j, overlap);
            } 
        }
    }
}

int main() 
{
    InitWindow(WIDTH, HEIGHT, "Collision Detection Simulation Raylib");
    SetTargetFPS(60);

    SpawnBalls();
    
    while (!WindowShouldClose())
    {
        if(IsKeyPressed(KEY_SPACE)){
            isPaused = !isPaused;
        }
        if(IsKeyPressed(KEY_R)){
            balls.clear();
            SpawnBalls();
        }
        
        float deltaTime = GetFrameTime();
        BeginDrawing();
            ClearBackground(BLACK);
            DrawText("SPACE - Pause/Resume    R - Restart", 10,10,30,BLUE);
            
            for(auto& ball : balls){
                if(!isPaused){
                    ball.move_ball(deltaTime);
                }
                ball.draw_ball();
            }
            CollisionsCheck();
        EndDrawing();
    }
    
    CloseWindow();
}