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
#define BOUNDARY_X 800.0f
#define BOUNDARY_Y 800.0f
#define TIME_STEP 0.0005f


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

void stepParticles(float *pos, float *vel, int count)
{
    for (int i = 0; i < count; i++)
    {
        pos[i * 2] += vel[i * 2] * TIME_STEP;
        pos[i * 2 + 1] += 0.5 * vel[i * 2 + 1] * TIME_STEP;
        vel[i * 2 + 1] += G * TIME_STEP;
        pos[i * 2 + 1] += 0.5 * vel[i * 2 + 1] * TIME_STEP;
    }
}

void updateGrid(float *pos, unsigned int *grid, int p_count, const int countx, const int county, const float cell_dx, const float cell_dy)
{
    memset(grid, 0, sizeof(float) * countx * county);

    if (DEBUG_MODE)
    {
        for (int i = 0; i < p_count; i++)
        {
            const int x_idx = floorf(pos[i * 2] / cell_dx);
            const int y_idx = floorf(pos[i * 2 + 1] / cell_dy);
            if(grid[x_idx+y_idx*countx] != 0){
                fprintf(stderr, "Error. Grid collision detected at index (x: %d, y: %d)", x_idx, y_idx );
                exit(1);
            }
            grid[x_idx + y_idx * countx] = i + 1;
        }
    }
    else
    {
        for (int i = 0; i < p_count; i++)
        {
            const int x_idx = floorf(pos[i * 2] / cell_dx);
            const int y_idx = floorf(pos[i * 2 + 1] / cell_dy);
            grid[x_idx + y_idx * countx] = i + 1;
        }
    }
}

void handle_collision(float *pos, float *vel, int id1, int id2){
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
    vel[p1] -= 1 * dot_product * nx *INTERNAL_DAMPING;
    vel[p1+1] -= 1 * dot_product * ny *INTERNAL_DAMPING;
    vel[p2] += 1 * dot_product * nx*INTERNAL_DAMPING;
    vel[p2+1] += 1 * dot_product * ny*INTERNAL_DAMPING;
    pos[p1] -= (nx * ( RADIUS - distance));
    pos[p1+1] -= (ny * ( RADIUS - distance));
    pos[p2] += (nx * ( RADIUS - distance));
    pos[p2+1] += (ny * ( RADIUS - distance));
}

void n_square_collision_solve(){

}



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
    initializeParticles(ppos, pvel, particle_count);
    unsigned int spatial_grid[GRID_CELLS_COUNT_X * GRID_CELLS_COUNT_Y];
    /* REMEMBER: Fix collisions BEFORE putting stuff into grid. Wait wtf.
    Ok so... the FIRST time you initialize particles, you have to inefficiently sort collisions.
    Why? Because theres a chance of strong overlap in the beggining. 
    Once sorted manually, the grid can take over and, assuming dt is small, there should be no collisions. 
    */
    
    

    return 0;
}