#include "raylib.h"
#include <stdio.h>
#include <math.h>


long double G = 6.67430e10-11;
struct object
{
    Vector2 position;
    long long mass;
    Vector2 velocity;
    Vector2 force;
    int radius;
};
void drawObject(struct object *obj);
void updateObject(struct object *obj, float deltaTime);
void calculateForces(struct object *obj1, struct object *obj2);

int main(void)
{


InitWindow(1800, 1450, "Simple Screen Example");

// Set the game to run at 60 frames per second
SetTargetFPS(60);
struct object objects[3];
objects[0].force = (Vector2){0,0};

objects[0].position = (Vector2){850, 100};
objects[0].mass =50;
objects[0].velocity = (Vector2){400, 10};
objects[1].force = (Vector2){0, 0};
objects[0].radius = 20;
objects[1].position = (Vector2){800, 725};
objects[1].mass = 3000;
objects[1].velocity = (Vector2){0, 0};
objects[1].radius = 200;

objects[2].force = (Vector2){0, 0};

objects[2].position = (Vector2){820, 110};
objects[2].mass = 2;
objects[2].velocity = (Vector2){20, 100};
objects[2].radius = 10;
// Main game loop
while (!WindowShouldClose()) // Detect window close button or ESC key
{
    
    BeginDrawing();

    // Set background color
    ClearBackground(BLACK);

    for (int i = 0; i < 3; i++) {
        for (int h =0 ; h < 3; h++) {
            if (i != h){
                objects[i].force = (Vector2){0,0};
                calculateForces(&objects[i], &objects[h]);
                updateObject(&objects[i], GetFrameTime());
                drawObject(&objects[i]);
            }
            
        };
    };
    printf("Object 1 Position: (%f, %f)\n", objects[1].position.x, objects[1].position.y);
    

    
    // End drawing
    EndDrawing();
}

// Close the window and cleanup
CloseWindow();

return 0;
}



void drawObject(struct object *obj) {
    DrawCircle(obj->position.x , obj->position.y, obj->radius, WHITE);
};

void updateObject(struct object *obj, float deltaTime) {
    
    int ax = obj->force.x / obj->mass;
    int ay = obj->force.y / obj->mass;
    
    obj->velocity.x += ax * deltaTime;
    obj->velocity.y += ay * deltaTime;
    
    obj->position.x += obj->velocity.x * deltaTime;
    obj->position.y += obj->velocity.y * deltaTime;
    
    
};

void calculateForces(struct object *obj1, struct object *obj2) {
    double distance = hypot(obj2->position.x - obj1->position.x, obj2->position.y - obj1->position.y);
    
    if (distance == 0) return; 
    Vector2 direction = {(obj2->position.x - obj1->position.x) / distance, (obj2->position.y - obj1->position.y) / distance};
    obj1->force.x += ((G * obj1->mass * obj2->mass) / ((distance * 1000) * (distance * 1000)) * direction.x);
    
    obj1->force.y += ((G * obj1->mass * obj2->mass) / ((distance * 1000) * (distance * 1000)) * direction.y);
   // printf("Force on obj1 from obj2: (%f, %f)\n", obj1->force.x, obj1->force.y);
    if (distance - (obj1->radius + obj2->radius) < 0.1) {
        obj1->force.x = -obj1->force.x;
        obj1->force.y = -obj1->force.y;
    }
};