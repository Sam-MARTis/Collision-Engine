#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <cstdlib>
#include <cstring>

#define DEFAULT_PARTICLE_COUNT 4000
#define DIMENSIONS 2
#define INTERNAL_DAMPING 0.0f
#define WALL_DAMPING 0.1f
#define RADIUS 5
#define RADIUS_SQUARED_TIMES_FOUR 4*RADIUS*RADIUS
#define STARTING_VELOCITY_RANGE 10
#define GRID_OVERLAP_TOLERANCE 5
#define NEIGHBOURHOOD_RADIUS 1

#define BOUNDARY_X 800.0f
#define BOUNDARY_Y 800.0f
#define PADDING 50.0f
#define TIME_STEP 0.0166f
#define ITERATIONS_PER_DRAW 20
#define GRID_CALCULATIONS_PER_ITER 5




#define DEBUG_MODE true

unsigned int particle_count;
const float G = 9.81;
// const int GRID_CELLS_COUNT_X = ceil(BOUNDARY_X / RADIUS);
// const int GRID_CELLS_COUNT_Y = ceil(BOUNDARY_Y / RADIUS);
const int GRID_CELLS_COUNT_Y = 80;
const int GRID_CELLS_COUNT_X = 80;
const float GRID_CELL_DX = BOUNDARY_X / ((float)GRID_CELLS_COUNT_X);
const float GRID_CELL_DY = BOUNDARY_Y / ((float)GRID_CELLS_COUNT_Y);

float randf(){
    return (float)rand()/RAND_MAX;
}

void initializeParticles(float *pos, float *vel, const int& count)
{
    for (int i = 0; i < count; i++)
    {
        pos[i * 2] = PADDING + randf() * (BOUNDARY_X - 2*PADDING);
        pos[i * 2 + 1] = PADDING + randf() * (BOUNDARY_Y - 2*PADDING);
        vel[i * 2] = STARTING_VELOCITY_RANGE * (randf() * 2.0f - 1.0f);
        vel[i * 2 + 1] = STARTING_VELOCITY_RANGE * (randf() * 2.0f - 1.0f);
    }
}


void stepParticles(float *pos, float *vel, const int& count, const float& dt)
{
    for (int i = 0; i < count; i++)
    {
        // pos[i * 2] += vel[i * 2] * dt;
        // pos[i * 2 + 1] += 0.5 * vel[i * 2 + 1] * dt;
        // vel[i * 2 + 1] += G * dt;
        // pos[i * 2 + 1] += 0.5 * vel[i * 2 + 1] * dt;

        pos[i * 2] += vel[i * 2] * dt;
        pos[i * 2 + 1] += vel[i * 2 + 1] * dt + 0.5f *G * dt*dt;
        vel[i * 2 + 1] += G * dt;


        if(pos[i*2+1]>BOUNDARY_Y-2*RADIUS - PADDING){
            vel[i*2+1] *= -WALL_DAMPING;
            pos[i*2+1] = BOUNDARY_Y-2*RADIUS - PADDING;
        }
        if(pos[i*2+1]<RADIUS + PADDING){
            vel[i*2+1] *= -WALL_DAMPING;
            pos[i*2+1] = RADIUS + PADDING;
        }

        if((pos[i*2]>BOUNDARY_X-2*RADIUS - PADDING)){
            vel[i*2] *= -WALL_DAMPING;
            pos[i*2] = BOUNDARY_X-2*RADIUS - PADDING;

        } 
        if(pos[i*2]< RADIUS + PADDING){
            pos[i*2] = RADIUS + PADDING;
            vel[i*2] *= -WALL_DAMPING;
        }
        
    }

}
void updateGrid(const float *pos, unsigned int *grid, const int& p_count, const int& countx, const int& county, const float& cell_dx, const float& cell_dy)
{
    memset(grid, 0, sizeof(float) * countx * county*(GRID_OVERLAP_TOLERANCE+1));

    #ifdef DEBUG_MODE
        for (int i = 0; i < p_count; i++)
        {
            const int x_idx = floorf(pos[i * 2] / cell_dx);
            const int y_idx = floorf(pos[i * 2 + 1] / cell_dy);
            const int idx = (x_idx + y_idx*countx)*(GRID_OVERLAP_TOLERANCE+1);
            unsigned int& cells_occupied = grid[idx];
            if(cells_occupied==GRID_OVERLAP_TOLERANCE){
                fprintf(stderr, "Error. Grid Tolerance exceeded at index (x: %d, y: %d)", x_idx, y_idx );
                continue;
                // exit(1);
            }


            // if(grid[x_idx+y_idx*countx] != 0){
            //     fprintf(stderr, "Error. Grid collision detected at index (x: %d, y: %d)", x_idx, y_idx );
            //     exit(1);
            // }


            cells_occupied ++;
            grid[idx + cells_occupied] = i;
            // grid[x_idx + y_idx * countx] = i + 1;
        }
    
    #else
        for (int i = 0; i < p_count; i++)
        {
            const int x_idx = floorf(pos[i * 2] / cell_dx);
            const int y_idx = floorf(pos[i * 2 + 1] / cell_dy);
            const int idx = (x_idx + y_idx*countx)*(GRID_OVERLAP_TOLERANCE+1);
            unsigned int& cells_occupied = grid[idx];
            // if(cells_occupied==GRID_OVERLAP_TOLERANCE){
            //     fprintf(stderr, "Error. Grid Tolerance exceeded at index (x: %d, y: %d)", x_idx, y_idx );
            //     exit(1);
            // }


            // if(grid[x_idx+y_idx*countx] != 0){
            //     fprintf(stderr, "Error. Grid collision detected at index (x: %d, y: %d)", x_idx, y_idx );
            //     exit(1);
            // }


            cells_occupied ++;
            grid[idx + cells_occupied] = i+1;
            // grid[x_idx + y_idx * countx] = i + 1;
        }
    #endif
}

void handle_collision(float *pos, float *vel, const int& id1, const int& id2){
    const int p1 = (id1)*2;
    const int p2 = (id2)*2;
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
    const float x_push = dot_product * nx * 0.5f*(1+INTERNAL_DAMPING);
    const float y_push = dot_product * ny * 0.5f*(1+INTERNAL_DAMPING);
    const float dR = RADIUS - 0.5f*distance;
    vel[p1] -= x_push;
    vel[p1+1] -= y_push;
    vel[p2] += x_push;
    vel[p2+1] += y_push;
    pos[p1] -= (nx * dR);
    pos[p1+1] -= (ny * dR);
    pos[p2] += (nx * dR);
    pos[p2+1] += (ny * dR);
}

void handle_collision_grid(float *pos, float* vel, const unsigned int* grid, const int& countx, const int& county, const int& x_idx, const int& y_idx, const int& p_id){
    for(int i= -NEIGHBOURHOOD_RADIUS; i<=NEIGHBOURHOOD_RADIUS; i++){
        for(int j= -NEIGHBOURHOOD_RADIUS; j<=NEIGHBOURHOOD_RADIUS; j++){
            const int idx_to_check = ((x_idx+i)+(y_idx+j)*countx)*(GRID_OVERLAP_TOLERANCE+1);
            const int& cell_count = grid[idx_to_check];
            if(cell_count==0) continue;
            for(int k=1; k<=cell_count; k++){
                handle_collision(pos, vel, p_id, grid[idx_to_check+k]);
            }
            // const int p2 = grid[(x_idx+i) + (y_idx+j)*countx];
            // if((p2 != 0) && (p2 != p_id) ){
            //     handle_collision(pos, vel, p_id, p2);
            // }
        }
    }
}


void grid_collision_solve(float *pos, float* vel, const unsigned int* grid, const int& countx, const int& county, const int& p_count, const float& cell_dx, const float& cell_dy){

    for(int i=0; i<p_count; i++){
        const int x_idx = floorf(pos[i * 2] / cell_dx);
        const int y_idx = floorf(pos[i * 2 + 1] / cell_dy);
        handle_collision_grid(pos, vel, grid, countx, county, x_idx, y_idx, i);
    }
}
void n_square_collision_solve(float *pos, float *vel, const int& p_count, const int& iterations){
    for(int iteration =0; iteration< iterations; iteration++){
        for(int i = 0; i<p_count-1; i++){
            for(int j=i; j<p_count; j++){
                handle_collision(pos, vel, i, j);
            }
        }
    }
}
void renderParticles(sf::RenderWindow& window, sf::CircleShape *points,  const float* pos, const int& particle_count){
    for(int i=0; i<particle_count; i++){
        points[i].setPosition(pos[2*i], pos[2*i +1]);
        window.draw(points[i]);
    }

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

    float ppos[2 * particle_count];
    float pvel[2 * particle_count];
    sf::CircleShape pshapes[particle_count];
    for(int i=0; i<particle_count; i++){
        pshapes[i] = sf::CircleShape((float)RADIUS);
    }
    
    // std::cout<<ppos[0]<<" "<<ppos[1]<<std::endl;
    initializeParticles(ppos, pvel, particle_count);
    // std::cout<<ppos[0]<<" "<<ppos[1]<<std::endl;
    
    n_square_collision_solve(ppos, pvel, particle_count, 5);
    stepParticles(ppos, pvel, particle_count, TIME_STEP);
    unsigned int spatial_grid[GRID_CELLS_COUNT_X * GRID_CELLS_COUNT_Y*(GRID_OVERLAP_TOLERANCE+1)];
    updateGrid(ppos, spatial_grid, particle_count, GRID_CELLS_COUNT_X, GRID_CELLS_COUNT_Y, GRID_CELL_DX, GRID_CELL_DY);

    /* REMEMBER: Fix collisions BEFORE putting stuff into grid. Wait wtf.
    Ok so... the FIRST time you initialize particles, you have to inefficiently sort collisions.
    Why? Because theres a chance of strong overlap in the beggining. 
    Once sorted manually, the grid can take over and, assuming dt is small, there should be no collisions. 
    */


    sf::ContextSettings settings;
    settings.antialiasingLevel = 16;
    sf::RenderWindow window(sf::VideoMode((int)round(BOUNDARY_X), (int)round(BOUNDARY_Y)), "First window", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);

    while (window.isOpen())
    {
        window.clear();
        renderParticles(window, pshapes, ppos, particle_count);
        for(int i =0; i<ITERATIONS_PER_DRAW; i++){
        stepParticles(ppos, pvel, particle_count, TIME_STEP);
        for(int j = 0; j< GRID_CALCULATIONS_PER_ITER; j++){ grid_collision_solve(ppos, pvel, spatial_grid, GRID_CELLS_COUNT_X, GRID_CELLS_COUNT_Y, particle_count, GRID_CELL_DX, GRID_CELL_DY);
        // std::cout<<"Preparing to update grid";
        updateGrid(ppos, spatial_grid, particle_count, GRID_CELLS_COUNT_X, GRID_CELLS_COUNT_Y, GRID_CELL_DX, GRID_CELL_DY);
        }
        // std::cout<<"Updated grid";
        }
        // std::cout<<ppos[0]<<" "<<ppos[1];


        
        
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