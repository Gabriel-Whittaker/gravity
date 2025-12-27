#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>


typedef struct node
{
    int index;
    struct node *next;
} Node;

void update(int i, Vector2 *vel, float time, float *densities, Vector2 *predPositions, int num, Vector2 *positions, Node *grid[]);
void resolveCollisions(Vector2 *pos, Vector2 *vel);
void drawObject(Vector2 *pos, Vector2 velocity, float density);
float calcPressure(float targetDens, float density, float pressureMultiplier);
float calcSharedPressure(float densityA, float densityB, float target, float pressureMultiplier);
float smoothing(float radius, float dst);
float smoothingderiv(float radius, float dst);
float calcDensity(Vector2 point, Vector2 *positions, int num, Node *grid[]);
void updateDensities(float *densities, int num, Vector2 *positions, Node *grid[]);
Vector2 calcPressureForce(int index, Vector2 point, Vector2 *positions, int num, float *densities, Node *grid[]);
Vector2 getMouseForce(Vector2 pos, Vector2 vel);
void getIndexes(Vector2 pos, float radius, int *indexes);
void addHead (Node **head, int i);
void add (Node *current,int i);




float gravity = 49.8;

float boxwidth = 800;
int boxheight = 800;

int main(void)
{
    
    int startwidth = 400;
    int startheight = 800;
    float radius = 50;
    int gridspace = 15;
    const int num = 1600;
    Vector2 positions[num];
    Vector2 velocity[num];
    float densities[num];
    InitWindow(1800, 1450, "Screen");

    
    SetTargetFPS(60);
    int boxsize = (int)(boxheight / radius * boxwidth / radius);
    Node *grid[boxsize];
    for (int i = 0; i < boxsize; i++)
    {
        grid[i] = NULL;
    }
    
    int current = 0;//startwidth*50;
    for (int i=0; i < num;i++)
    {
        //gridspace = round(((gridspace - 10 / 3) % 3 / 2 * 7 + 2 + gridspace) % 183);
        //printf("%i", gridspace);
        velocity[i] = (Vector2){0, 0};
        div_t pos = div(current,startwidth);
        positions[i].x = pos.rem; 
        positions[i].y = pos.quot*10;
        current += gridspace;
    }

    while (!WindowShouldClose()) 
    {

        BeginDrawing();

     
        ClearBackground(BLACK);
        DrawRectangleLines(0,0, boxwidth, boxheight,WHITE );

        Vector2 predictedpositions[num];
        //printf("check3\n");
        for (int i = 0; i < boxsize; i++)
        {
            while (grid[i] != NULL)
            {
                Node *cur = grid[i];
                grid[i] = grid[i]->next; 
                free(cur);
            }
        
        }
        //printf("check2\n");
        for (int i = 0; i < num; i++)
        {
            predictedpositions[i].x  =positions[i].x +velocity[i].x*1/ 60;
            predictedpositions[i].y = positions[i].y + velocity[i].y * 1 / 60;

            int index = (int)(predictedpositions[i].x/ radius) + (int)(predictedpositions[i].y/ radius) * (int)(boxwidth / radius);
            if (index >= boxsize || index < 0)
            {
                index = boxsize-1;
            }
            if (grid[index] == NULL)
            {
                addHead(&grid[index],i);
            }
            else
            {

                Node *cur = grid[index];
                while (cur-> next)
                {
                    cur = cur->next;
                }
                add(cur,i);
            }
        }
        //printf("check1\n");
        updateDensities(densities, num, predictedpositions, grid);
        for (int i = 0; i < num; i++)
        {
            
            //drawObject(&positions[i]);
            //Vector2 grad = calcPressureForce(i, positions[i], positions, num, densities);
            //printf("%f%f\n", grad.x,grad.y);
            //DrawLine(positions[i].x, positions[i].y, positions[i].x + grad.x, positions[i].y + grad.y, GREEN);

            update(i, &velocity[i], GetFrameTime(), densities, predictedpositions, num, positions, grid);
        }
        
        //printf("%f\n",calcDensity((Vector2){100,100}, positions, num, grid));
        EndDrawing();
    }
    


// Close the window and cleanup
CloseWindow();

return 0;

}

void drawObject(Vector2 *pos, Vector2 velocity, float density)
{
    float speed = hypot(velocity.x,velocity.y);
    float max = 100;
    float colour = fmin(speed/max,1);
    Color c;
    c.r = (unsigned char)(255 *colour);
    c.g = 0;
    c.b = (unsigned char)(255 * (1- colour));
    c.a = 255;
    // float target = 2;
    // float good  =fmax(0,1 - abs(target-density));
    // float dense = fmax(0, (density - target)/target*4);
    // float spread = fmax(0, (target - density) / (target / 8));
    // Color c;
    // c.r = (unsigned char)(fmin(255 *dense,255));
    // c.g = (unsigned char)(fmin(255 * good, 255));
    // c.b = (unsigned char)(fmin(255 * spread, 255));
    // c.a = 255;
    DrawCircle(pos->x,pos->y, 3, c);
};

void update(int i, Vector2 *vel, float time, float *densities, Vector2 *predPositions, int num, Vector2 *positions, Node *grid[])
{
    Vector2 pressure  = calcPressureForce(i,predPositions[i], predPositions, num, densities, grid);
    vel->y += pressure.y/densities[i]*time;
    vel->x += pressure.x/densities[i]*time;
    Vector2 mouseForce = getMouseForce(positions[i],*vel);
   // DrawLine(positions[i].x, positions[i].y, positions[i].x + pressure.x, positions[i].y + pressure.y, GREEN);
    vel->y += mouseForce.y * time;
    vel->x += mouseForce.x * time;

    //vel->y += gravity * time;

    positions[i].y += vel->y *time;

    positions[i].x += vel->x * time;
    resolveCollisions(&positions[i],vel);
    
    drawObject(&positions[i], *vel, densities[i]);

}

void resolveCollisions(Vector2 *pos, Vector2 *vel)
{
    float damping = 0.4;


    if (pos->x < 0.0)
    {
        pos->x = 2.0;
        vel->x *= -damping;
    }

    else if (pos->x > boxwidth)
    {
        pos->x = boxwidth-2;
        vel->x *= -damping;
    }

    if (pos->y < 0.0)
    {
        pos->y = 2.0;
        vel->y *= -damping;
    }
    else if (pos->y > boxheight)
    {
        pos->y = boxheight-2;
        vel->y *= -damping;
    }
}


float smoothing(float radius, float dst)
{
    float volume = PI * pow(radius,4)/6;
    float value = fmax(0,radius-dst);
    return value * value/ volume;
}

float smoothingderiv(float radius, float dst)
{
    if (dst>= radius){return 0;}
    float s = dst -radius;
    return 12 /PI/pow(radius,4)*s;

}

float calcDensity(Vector2 point, Vector2 *positions, int num, Node *grid[])
{
    float density = 0; 
    float radius = 50;
    float mass =500;

    int indexes[9];
    getIndexes(point, radius, indexes);

    for (int h = 0; h < 9; h++)
    {   
        int f = indexes[h];
        if (f != -1)
        {
        Node *current = grid[f];

        while(current != NULL)
        {
        int i = current->index;
        float distance = hypot(point.x-positions[i].x, point.y-positions[i].y);
        density += mass * smoothing(radius, distance);
        current = current->next;
        }}}
    return density;
}

Vector2 calcPressureForce(int index, Vector2 point, Vector2 *positions, int num, float *densities, Node *grid[])
{

    Vector2 pressureForce = {0, 0};
    float radius = 50;
    float mass = 500;
    float targetDensity = 8;
    float pressureMultiplier = 8000;
    int indexes[9];
    getIndexes(point, radius, indexes);

    for (int h = 0; h < 9; h++)
    {   
        int f = indexes[h];
        if (f != -1)
        {
        Node *current = grid[f];

        while(current != NULL)
        {
        int i = current->index;
        
        float distance = hypot(point.x - positions[i].x, point.y - positions[i].y);
        if (distance > 0.001 && distance < 51)
        {
            float sharedPressure = calcSharedPressure(densities[index], densities[i], targetDensity, pressureMultiplier);
            pressureForce.x += sharedPressure * mass * ((point.x - positions[i].x) / distance) * smoothingderiv(radius, distance) / densities[i];

            pressureForce.y += sharedPressure * mass * ((point.y - positions[i].y) / distance) * smoothingderiv(radius, distance) / densities[i];
            };
        
        current = current->next;
        }
    }
    }

        return pressureForce;
}

void updateDensities(float *densities, int num, Vector2 *positions, Node *grid[])
{
    for (int i = 0; i< num; i++)
    {
        densities[i] = calcDensity(positions[i],positions,num, grid);
    }
}


float calcPressure(float targetDens, float density, float pressureMultiplier)
{
    float pressure  = (density - targetDens) * pressureMultiplier;
    return pressure;

}

float calcSharedPressure(float densityA, float densityB, float target, float pressureMultiplier)
{
    return (calcPressure(target, densityA, pressureMultiplier) + calcPressure(target, densityA, pressureMultiplier))/2;
}

Vector2 getMouseForce(Vector2 pos, Vector2 vel)
{
    Vector2 force = {0, 0};
    
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {   
        Vector2 mouse = GetMousePosition();
        float distance = hypot(mouse.x - pos.x, mouse.y - pos.y);
        if (distance<150)
        {

            force.x = -((mouse.x - pos.x) / distance)  * (150 - distance)*10 - vel.x;
            force.y = -((mouse.y - pos.y) / distance)  * (150-distance)*10 - vel.y;
        }
    }
    return force;
}

void add (Node *current,int i)
{
    Node *n = malloc(sizeof(Node));
    n->next = NULL;
    n->index = i;
    current ->next = n;

}

void addHead (Node **head, int i)
{
    Node *n = malloc(sizeof(Node));
    n->next = NULL;
    n->index = i;
    *head = n;
}

void getIndexes(Vector2 pos, float radius, int *indexes)
{

    int gridW = (int)(boxwidth / radius);
    int gridH = (int)(boxheight / radius);

    int cellX = (int)(pos.x / radius);
    int cellY = (int)(pos.y / radius);

    int current = cellX + cellY * gridW;

    int num = 0;
    for (int dy = -1;dy<2;dy++)
    {
        for (int dx =-1;dx<2;dx++)
        {
            //printf("%inum\n",num);
            int nx = cellX  + dx;
            int ny = cellY + dy;
            int index = nx + ny * gridW;
            //printf("%i,%i\n",cellX,gridW);
            if (nx >= 0 && nx < gridW && ny >= 0 && ny < gridH)
            {
                indexes[num] = index;
                //printf("check");
            }
            else
            {
                indexes[num] = -1;
            }
            num++;

        }
    }
    
}