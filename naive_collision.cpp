#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <cstdlib>
#include <cstring>

#define DEFAULT_PARTICLE_COUNT 1000
#define DIMENSIONS 2
#define INTERNAL_DAMPING 1.0f
#define RADIUS 5
#define RADIUS_SQUARED_TIMES_FOUR 100
#define STARTING_VELOCITY_RANGE 10
#define GRID_OVERLAP_TOLERANCE 5
#define BOUNDARY_X 800.0f
#define BOUNDARY_Y 800.0f
#define TIME_STEP 0.0001f


#define DEBUG_MODE true

unsigned int particle_count;
const float G = -9.81;
const int GRID_CELLS_COUNT_X = ceil(BOUNDARY_X / RADIUS);
const int GRID_CELLS_COUNT_Y = ceil(BOUNDARY_Y / RADIUS);
const float GRID_CELL_DX = BOUNDARY_X / ((float)GRID_CELLS_COUNT_X);
const float GRID_CELL_DY = BOUNDARY_Y / ((float)GRID_CELLS_COUNT_Y);

void initializeParticles(float *pos, float *vel, int count)
{
    for (int i = 0; i < count; i++)
    {
        pos[i * 2] = rand() * BOUNDARY_X;
        pos[i * 2 + 1] = rand() * BOUNDARY_Y;
        vel[i * 2] = STARTING_VELOCITY_RANGE * (rand() * 2 - 1);
        vel[i * 2 + 1] = STARTING_VELOCITY_RANGE * (rand() * 2 - 1);
    }
}

void stepParticles(float *pos, float *vel, const int& count, const float& dt)
{
    for (int i = 0; i < count; i++)
    {
        pos[i * 2] += vel[i * 2] * dt;
        pos[i * 2 + 1] += 0.5 * vel[i * 2 + 1] * dt;
        vel[i * 2 + 1] += G * dt;
        pos[i * 2 + 1] += 0.5 * vel[i * 2 + 1] * dt;
    }
}

void handle_collision(float *pos, float *vel, const int& id1, const int& id2){
    const int p1 = (id1-1)*2;
    const int p2 = (id2-1)*2;
    const float dx = pos[p2] - pos[p1];
    const float dy = pos[p2+1] - pos[p1+1];
    const float distance_squared = pow(dx, 2) + pow(dy, 2);
    if(distance_squared> RADIUS_SQUARED_TIMES_FOUR){
        return;
    }
    const float distance = sqrtf(distance_squared);
    if (distance < 1e-6f) return;
    const float nx = dx/distance;
    const float ny = dy/distance;
    const float dot_product = nx* (vel[p1] - vel[p2]) + ny*(vel[p1+1] - vel[p2+1]);
    const float x_push = dot_product * nx *INTERNAL_DAMPING;
    const float y_push =  dot_product * ny *INTERNAL_DAMPING;
    const float dR = (float)RADIUS-distance;
    vel[p1] -= x_push;
    vel[p1+1] -= y_push;
    vel[p2] += x_push;
    vel[p2+1] += y_push;
    pos[p1] -= (nx * dR);
    pos[p1+1] -= (ny * dR);
    pos[p2] += (nx * dR);
    pos[p2+1] += (ny * dR);
}

void n_square_collision_solve(float *pos, float *vel, const int& p_count, const int& iterations){
    for(int iteration =0; iteration< iterations; iteration++){
        for(int i = 0; i<p_count-1; i++){
            for(int j=0; j<p_count; j++){
                handle_collision(pos, vel, i+1, j+1);
            }
        }
    }
}

void renderParticles(sf::RenderWindow& window, sf::CircleShape *points,  float* pos, const int& particle_count){
    for(int i=0; i<particle_count; i++){
        points[i].setPosition(pos[2*i], pos[2*i +1]);
        window.draw(points[i]);
    }

}


// void initializeRenderingParticles(sf::CircleShape* circles, const float& radius, const int& particle_count){
//     for(int i=0; i<particle_count; i++){
//         circles[i] = sf::CircleShape()
//     }
    
// }

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        particle_count = DEFAULT_PARTICLE_COUNT;
    }
    else
    {
        if (argc == 2)
        {
            particle_count = strtol(argv[2], NULL, 10);
        }
        else
        {
            printf("Unknown arguement count. Only one extra arguement expected");
            return 1;
        }
    }

    srand(40);

    float ppos[DIMENSIONS * particle_count];
    float pvel[DIMENSIONS * particle_count];
    sf::CircleShape pshapes[particle_count];
    for(int i=0; i<particle_count; i++){
        pshapes[i] = sf::CircleShape((float)RADIUS);
    }
    

    initializeParticles(ppos, pvel, particle_count);
    n_square_collision_solve(ppos, pvel, particle_count, 1);
    stepParticles(ppos, pvel, particle_count, TIME_STEP);


    // unsigned int spatial_grid[GRID_CELLS_COUNT_X * GRID_CELLS_COUNT_Y];
    /* REMEMBER: Fix collisions BEFORE putting stuff into grid. Wait wtf.
    Ok so... the FIRST time you initialize particles, you have to inefficiently sort collisions.
    Why? Because theres a chance of strong overlap in the beggining. 
    Once sorted manually, the grid can take over and, assuming dt is small, there should be no collisions. 
    */




    sf::ContextSettings settings;
    settings.antialiasingLevel = 16;
    sf::RenderWindow window(sf::VideoMode(1600, 1600), "First window", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);

    while (window.isOpen())
    {
        window.clear();
        renderParticles(window, pshapes, ppos, particle_count);
        stepParticles(ppos, pvel, particle_count, TIME_STEP);
        n_square_collision_solve(ppos, pvel, particle_count, 1);


        
        
        window.display();
        sf::Event event;


        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            { 
                window.close();
            }
        }
    }
    
    

    return 0;
}