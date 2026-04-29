#include <raylib.h>
#include <vector>
#include <math.h>
#include <algorithm>

const int WIDTH = 1200;
const int HEIGHT = 850;
const int OFFSET = 100;

bool isPaused = true;
bool isGizmoOn = true;

Color ballColor = BLUE;
Color collisionColor = WHITE;
float ballMaxVelocity = 100.0f;
float ballMaxSize = 30.0f;
float ballMinSize = 10.0f;
int ballCount = 100;
int ballSize = 20;
int collisionChecks = 0;

enum OptimisationAlgorithm{ //length is hardcoded somewhere
    NONE, 
    SAP, //SWEEP AND PRUNE
    UGRID, // UNIFORM GRID
    QUADTREE,
    KDTREE
};

int currentOptAlg = NONE;

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
        DrawCircle(position.x, position.y+OFFSET, size, color);
        color = ballColor;
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
    void set_Color(Color newColor){
        color = newColor;
    }
    
    
};

std::vector<Ball> balls;

void SpawnBalls(){
    balls.reserve(ballCount);
    for(int i = 0; i<ballCount; i++){
        float randomSize = (float)GetRandomValue(ballMinSize*10, ballMaxSize*10)/10.0;
        Vector2 randomPos = (Vector2){(float)GetRandomValue(ballSize, WIDTH-ballSize), (float)GetRandomValue(ballSize,HEIGHT-ballSize)};
        Vector2 randomVel = (Vector2){(float)GetRandomValue(-ballMaxVelocity,ballMaxVelocity), (float)GetRandomValue(-ballMaxVelocity,ballMaxVelocity)};
        balls.emplace_back(randomPos, randomVel, randomSize, ballColor);
    }
}

void CollideBalls(Ball& ball1, Ball& ball2, float overlap){

    Vector2 vel1 = ball1.get_Velocity();//beginning velocity
    Vector2 vel2 = ball2.get_Velocity();
    Vector2 pos1 = ball1.get_Position();
    Vector2 pos2 = ball2.get_Position();
    float mass1 = pow(ball1.get_Size(),2); 
    float mass2 = pow(ball2.get_Size(),2);
    float mn = sqrt(pow(pos2.x-pos1.x,2) + pow(pos2.y-pos1.y,2)); //magnitude of normal
    if(mn == 0) return; 
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
    ball1.set_Velocity(vel1f);
    ball2.set_Velocity(vel2f);
    //fix overlap
    float pos1fix = -1*overlap*(mass2/(mass1+mass2));
    float pos2fix = overlap*(mass1/(mass1+mass2));
    ball1.set_Position((Vector2){pos1.x+pos1fix*un.x, pos1.y+pos1fix*un.y});
    ball2.set_Position((Vector2){pos2.x+pos2fix*un.x, pos2.y+pos2fix*un.y});
}

void CheckOverlap(Ball& ball1, Ball& ball2){
    collisionChecks += 1;
    Vector2 ball1pos = ball1.get_Position(); 
    float ball1size = ball1.get_Size();
    Vector2 ball2pos = ball2.get_Position();
    float ball2size = ball2.get_Size();
    if(isGizmoOn){
        DrawLineEx({ball1pos.x,ball1pos.y+OFFSET}, {ball2pos.x, ball2pos.y+OFFSET}, 1, RED);
    }
    float overlap = (ball1size+ball2size) - sqrt(pow(ball1pos.x-ball2pos.x, 2)+pow(ball1pos.y-ball2pos.y, 2));
    if(overlap>= 0){
        ball1.set_Color(collisionColor);
        ball2.set_Color(collisionColor);
        CollideBalls(ball1, ball2, overlap);
    }
}

void CollisionsCheck(){
    for(int i = 0; i < ballCount; i++){
        for (int j = i+1; j < ballCount; j++){
            if(i == j){
                continue;
            }
            CheckOverlap(balls[i], balls[j]);
            
        }
    }
}

bool isX1lessThanX2(Ball& ball1, Ball& ball2){
    return ball1.get_Position().x < ball2.get_Position().x;  
}
void CollisionsCheckSAP(){
    std::sort(balls.begin(), balls.end(), isX1lessThanX2);
    for (int i = 0; i < ballCount; i++){
        for(int j = 1+i; j < ballCount; j++){
            float size1 = balls[i].get_Size();
            float size2 = balls[j].get_Size();
            float pos1 = balls[i].get_Position().x;
            float pos2 = balls[j].get_Position().x;
            if((pos2-size2)>(pos1+size1)){
                break;
            }

            CheckOverlap(balls[i], balls[j]);
        }
    }
}






int main() 
{
    InitWindow(WIDTH, HEIGHT+OFFSET, "Collision Detection Simulation Raylib");
    SetTargetFPS(60);

    SpawnBalls();
    
    while (!WindowShouldClose())
    {
        if(IsKeyPressed(KEY_SPACE)){
            isPaused = !isPaused;
        }
        if(IsKeyPressed(KEY_O)){
            currentOptAlg = (currentOptAlg+1)%5; //////////////////////////////////////////// HARDCODED ENUM LENGHT
        }
         if(IsKeyPressed(KEY_G)){
            isGizmoOn = !isGizmoOn;
        }
        if(IsKeyPressed(KEY_R)){
            balls.clear();
            SpawnBalls();
        }
        
        float deltaTime = GetFrameTime();
        BeginDrawing();
            ClearBackground(BLACK);
            DrawText(TextFormat("SPACE - Pause/Resume    R - Restart       G - Gizmos      CPF - %d", collisionChecks), 10,10,30,BLUE);
            collisionChecks = 0;
            switch (currentOptAlg)
            {
            case NONE:
                DrawText(TextFormat("O - OPTIMISATION ALGORITHM : NONE"), 10,60,30,BLUE);
                CollisionsCheck();
                break;
            case SAP: 
                DrawText(TextFormat("O - OPTIMISATION ALGORITHM : SWEEP AND PRUNE"), 10,60,30,BLUE);
                CollisionsCheckSAP();
                break;
            case UGRID:
                DrawText(TextFormat("O - OPTIMISATION ALGORITHM : UNIFORM GRID"), 10,60,30,BLUE);
                CollisionsCheck();
                break;
            case QUADTREE:
                DrawText(TextFormat("O - OPTIMISATION ALGORITHM : QUADTREE"), 10,60,30,BLUE);
                CollisionsCheck();
                break;
            case KDTREE:
                DrawText(TextFormat("O - OPTIMISATION ALGORITHM : K-D TREE"), 10,60,30,BLUE);
                CollisionsCheck();
                break;
            
            default:
                CollisionsCheck();
                break;
            }

            DrawLineEx({0, OFFSET}, {WIDTH, OFFSET}, 2, BLUE);
            
            
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