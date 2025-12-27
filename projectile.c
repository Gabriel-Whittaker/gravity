#include "raylib.h"
#include <stdio.h>
#include <math.h>

double g = 9.81;
int number= 2;
bool simulate = false;
double restitution = 0.8;

struct object
{
    Vector2 position;
    long mass;
    Vector2 velocity;
    Vector2 force;
    int radius;
};
void drawObject(struct object *obj);
void updateObject(struct object *obj, float deltaTime);
void calculateForces(struct object *obj);
void zero(struct object *obj);

int main(void)
{

    InitWindow(1800, 1450, "Simple Screen Example");

    // Set the game to run at 60 frames per second
    SetTargetFPS(60);
    struct object objects[number];
    for (int i = 0; i < number; i++)
    {
        zero(&objects[i]);
    }
    objects[0].position = (Vector2){400,400};
    objects[0].velocity = (Vector2){40, 0};
    objects[1].position = (Vector2){800, 400};
    objects[1].velocity.y = -10;

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {

        BeginDrawing();

        // Set background color
        ClearBackground(BLACK);

        
            for (int i = 0; i < number; i++)
            {
                if (simulate)
                {
                    objects[i].force = (Vector2){0, 0};
                    calculateForces(&objects[i]);
                    updateObject(&objects[i], GetFrameTime());
                    
                };
                drawObject(&objects[i]);
            };
        
        if (IsKeyDown(KEY_W) && !simulate)
        {
            objects[0].velocity.y -= 50 * GetFrameTime();

        }
        if (IsKeyPressed(' '))
        {
            simulate = true;
        }
       

        // End drawing
        EndDrawing();
    }

    // Close the window and cleanup
    CloseWindow();

    return 0;
}

void drawObject(struct object *obj)
{
    DrawCircle(obj->position.x, obj->position.y, obj->radius, WHITE);
};

void updateObject(struct object *obj, float deltaTime)
{

    int ax = obj->force.x / obj->mass;
    int ay = obj->force.y / obj->mass;

    obj->velocity.x += ax * deltaTime;
    obj->velocity.y += ay * deltaTime;

    obj->position.x += obj->velocity.x * deltaTime;
    obj->position.y += obj->velocity.y * deltaTime;
};

void calculateForces(struct object *obj)
{
    obj->force.y += obj->mass * g;
    if (obj->position.y > 1000)
    {
        obj->velocity.y = -1 * obj->velocity.y * restitution; 
    }
    if (obj->position.x > 900)
    {
        obj->velocity.x = -1 * obj->velocity.x * restitution;
    }
    if (obj->position.x < 300)
    {
        obj->velocity.x = -1 * obj->velocity.x * restitution;
    }
};


void zero(struct object *obj)
{
    obj->force = (Vector2){0,0};
    obj->position = (Vector2){0, 0};
    obj->velocity = (Vector2){0, 0};
    obj->radius = 10;
    obj-> mass = 1;
};