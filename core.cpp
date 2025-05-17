#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <cstdlib>
#include <cstring>
#include <fstream>

#define DEFAULT_PARTICLE_COUNT 10000
#define DIMENSIONS 2
#define INTERNAL_DAMPING 0.0f
#define WALL_DAMPING 0.1f
#define RADIUS 2
#define RADIUS_SQUARED_TIMES_FOUR 4*RADIUS*RADIUS
#define STARTING_VELOCITY_RANGE 10
#define GRID_OVERLAP_TOLERANCE 2
#define NEIGHBOURHOOD_RADIUS 1

#define TAGGING_RADIUS 1
#define TAG_COLOUR 200.0f
// #define CACHE_COLOURS
#define READ_CACHE_COLOURS
#define CACHE_FILE "colours_cache.txt"


#define BOUNDARY_X 500.0f
#define BOUNDARY_Y 800.0f
#define PADDING 50.0f
#define TIME_STEP 0.015f
#define ITERATIONS_PER_DRAW 2
#define GRID_CALCULATIONS_PER_ITER 1




// #define DEBUG_MODE true

unsigned int particle_count;
const float G = 9.81;
const int GRID_CELLS_COUNT_X = ceil(BOUNDARY_X / (2*RADIUS));
const int GRID_CELLS_COUNT_Y = ceil(BOUNDARY_Y / (2*RADIUS));
// const int GRID_CELLS_COUNT_Y = 120;
// const int GRID_CELLS_COUNT_X = 120;
const float GRID_CELL_DX = BOUNDARY_X / ((float)GRID_CELLS_COUNT_X);
const float GRID_CELL_DY = BOUNDARY_Y / ((float)GRID_CELLS_COUNT_Y);

float randf(){
    return (float)rand()/RAND_MAX;
}
sf::Color hueToColor(float hueDegrees) {
    float h = fmod(hueDegrees, 360.0f);
    float s = 1.0f; 
    float l = 0.5f; 

    float c = (1 - std::fabs(2 * l - 1)) * s;
    float x = c * (1 - std::fabs(fmod(h / 60.0f, 2) - 1));
    float m = l - c / 2.0f;

    float r = 0, g = 0, b = 0;

    if (0 <= h && h < 60)      { r = c; g = x; b = 0; }
    else if (60 <= h && h < 120)  { r = x; g = c; b = 0; }
    else if (120 <= h && h < 180) { r = 0; g = c; b = x; }
    else if (180 <= h && h < 240) { r = 0; g = x; b = c; }
    else if (240 <= h && h < 300) { r = x; g = 0; b = c; }
    else if (300 <= h && h < 360) { r = c; g = 0; b = x; }

    return sf::Color(
        static_cast<sf::Uint8>((r + m) * 255),
        static_cast<sf::Uint8>((g + m) * 255),
        static_cast<sf::Uint8>((b + m) * 255)
    );
}





void initializeParticles(float *pos, float *init_pos, float *col, const int& count, const float& dt)
{
    for (int i = 0; i < count; i++)
    {
        init_pos[i * 2] = PADDING + randf() * (BOUNDARY_X - 2*PADDING);
        init_pos[i * 2 + 1] = PADDING + randf() * (BOUNDARY_Y - 2*PADDING);
        pos[i * 2] = init_pos[i*2] + STARTING_VELOCITY_RANGE * (randf() * 2.0f - 1.0f)*dt;
        pos[i * 2 + 1] = init_pos[i*2+1]+ STARTING_VELOCITY_RANGE * (randf() * 2.0f - 1.0f)*dt + 0.5*G*dt*dt;
        col[i] = 120.0f;
    }

    #ifdef READ_CACHE_COLOURS
        std::ifstream fin(CACHE_FILE);
        int cached_particles;
        fin>>cached_particles;
        const int reads = cached_particles<count? cached_particles : count;
        for(int i=0; i<reads; i++){
            fin >> col[i];
        }
    #endif
}

void cacheColours(float* col, const int& particle_count, const std::string& filename){
    std::ofstream fout(filename);
    fout<<particle_count<<"\n";
    for (int i=0; i<particle_count; i++){
        fout<<col[i]<<"\n";
    }
}
void stepParticles(float *pos, float *prev_pos, const int& count, const float& dt)
{
    for (int i = 0; i < count; i++)
    {
        // pos[i * 2] += vel[i * 2] * dt;
        // pos[i * 2 + 1] += 0.5 * vel[i * 2 + 1] * dt;
        // vel[i * 2 + 1] += G * dt;
        // pos[i * 2 + 1] += 0.5 * vel[i * 2 + 1] * dt;

        // pos[i * 2] += vel[i * 2] * dt;
        // pos[i * 2 + 1] += vel[i * 2 + 1] * dt + 0.5f *G * dt*dt;
        // vel[i * 2 + 1] += G * dt;


        const float newX = 2*pos[i*2] - prev_pos[2*i];
        const float newY = 2*pos[i*2 + 1] - prev_pos[i*2 + 1] + G * dt * dt;
        prev_pos[2*i] = pos[2*i];
        prev_pos[2*i+1] = pos[2*i+1];
        pos[2*i] = newX;
        pos[2*i + 1] = newY;


        if(pos[i*2+1]>BOUNDARY_Y-2*RADIUS - PADDING){
            pos[i*2+1] = BOUNDARY_Y-2*RADIUS - PADDING;
        }
        if(pos[i*2+1]<RADIUS + PADDING){
            pos[i*2+1] = RADIUS + PADDING;
        }

        if((pos[i*2]>BOUNDARY_X-2*RADIUS - PADDING)){
            pos[i*2] = BOUNDARY_X-2*RADIUS - PADDING;

        } 
        if(pos[i*2]< RADIUS + PADDING){
            pos[i*2] = RADIUS + PADDING;
        }
        
    }

}
void updateGrid(const float *pos, unsigned int *grid, const int& p_count, const int& countx, const int& county, const float& cell_dx, const float& cell_dy)
{
    memset(grid, 0, sizeof(float) * countx * county*(GRID_OVERLAP_TOLERANCE+1));

    
        for (int i = 0; i < p_count; i++)
        {
            const int x_idx = floorf(pos[i * 2] / cell_dx);
            const int y_idx = floorf(pos[i * 2 + 1] / cell_dy);
            const int idx = (x_idx + y_idx*countx)*(GRID_OVERLAP_TOLERANCE+1);
            unsigned int& cells_occupied = grid[idx];
            if(cells_occupied==GRID_OVERLAP_TOLERANCE){
                #ifdef DEBUG_MODE
                fprintf(stderr, "Error. Grid Tolerance exceeded at index (x: %d, y: %d)", x_idx, y_idx );
                #endif
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
    
    
}

void handle_collision(float *pos, const int& id1, const int& id2){
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
    const float dR = RADIUS - 0.5f*distance;
    pos[p1] -= (nx * dR);
    pos[p1+1] -= (ny * dR);
    pos[p2] += (nx * dR);
    pos[p2+1] += (ny * dR);
}

void handle_collision_grid(float *pos, const unsigned int* grid, const int& countx, const int& county, const int& x_idx, const int& y_idx, const int& p_id){
    for(int i= -NEIGHBOURHOOD_RADIUS; i<=NEIGHBOURHOOD_RADIUS; i++){
        for(int j= -NEIGHBOURHOOD_RADIUS; j<=NEIGHBOURHOOD_RADIUS; j++){
            const int idx_to_check = ((x_idx+i)+(y_idx+j)*countx)*(GRID_OVERLAP_TOLERANCE+1);
            const int& cell_count = grid[idx_to_check];
            if(cell_count==0) continue;
            for(int k=1; k<=cell_count; k++){
                handle_collision(pos, p_id, grid[idx_to_check+k]);
            }
            // const int p2 = grid[(x_idx+i) + (y_idx+j)*countx];
            // if((p2 != 0) && (p2 != p_id) ){
            //     handle_collision(pos, vel, p_id, p2);
            // }
        }
    }
}


void grid_collision_solve(float *pos, const unsigned int* grid, const int& countx, const int& county, const int& p_count, const float& cell_dx, const float& cell_dy){

    for(int i=0; i<p_count; i++){
        const int x_idx = floorf(pos[i * 2] / cell_dx);
        const int y_idx = floorf(pos[i * 2 + 1] / cell_dy);
        handle_collision_grid(pos, grid, countx, county, x_idx, y_idx, i);
    }
}
void n_square_collision_solve(float *pos, const int& p_count, const int& iterations){
    for(int iteration =0; iteration< iterations; iteration++){
        for(int i = 0; i<p_count-1; i++){
            for(int j=i; j<p_count; j++){
                handle_collision(pos, i, j);
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
void tagParticles(const sf::Vector2i& mouse_coordinates, float* col, sf::CircleShape *points,  const unsigned int* grid, const int& countx, const int& county, const float& cell_dx, const float& cell_dy){
    const float mouseX = (float)mouse_coordinates.x;
    const float mouseY = (float)mouse_coordinates.y;
    const int x_idx = floorf(mouseX/ cell_dx);
    const int y_idx = floorf(mouseY / cell_dy);
    for(int l= -TAGGING_RADIUS; l<=TAGGING_RADIUS; l++){
            for(int m = -TAGGING_RADIUS; m<= TAGGING_RADIUS; m++){
            const int idx = ((x_idx+l) + (y_idx+m)*countx )*(GRID_OVERLAP_TOLERANCE + 1);
            if(grid[idx]==0) continue;;
            for(int i=1; i<=grid[idx]; i++){
                const int id = grid[idx+i];
                col[id]=TAG_COLOUR;
                points[id].setFillColor(hueToColor(TAG_COLOUR));
            }
        }
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
    float pcol[particle_count];
    float ppos_prev[2*particle_count];
    initializeParticles(ppos, ppos_prev, pcol, particle_count, TIME_STEP);
    
    sf::CircleShape pshapes[particle_count];
    for(int i=0; i<particle_count; i++){
        pshapes[i] = sf::CircleShape((float)RADIUS);
        pshapes[i].setFillColor(hueToColor(pcol[i]));
    }
    
    // std::cout<<ppos[0]<<" "<<ppos[1]<<std::endl;
    // std::cout<<ppos[0]<<" "<<ppos[1]<<std::endl;
    
    n_square_collision_solve(ppos, particle_count, 1);
    stepParticles(ppos, ppos_prev, particle_count, TIME_STEP);
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
    
    bool dragging = false;
    std::vector<int> selected_particles;
    while (window.isOpen())
    {
        window.clear();
        renderParticles(window, pshapes, ppos, particle_count);
        for(int i =0; i<ITERATIONS_PER_DRAW; i++){
        stepParticles(ppos, ppos_prev, particle_count, TIME_STEP);
        for(int j = 0; j< GRID_CALCULATIONS_PER_ITER; j++){ grid_collision_solve(ppos, spatial_grid, GRID_CELLS_COUNT_X, GRID_CELLS_COUNT_Y, particle_count, GRID_CELL_DX, GRID_CELL_DY);
        updateGrid(ppos, spatial_grid, particle_count, GRID_CELLS_COUNT_X, GRID_CELLS_COUNT_Y, GRID_CELL_DX, GRID_CELL_DY);
        }
        }


        
        
        window.display();
        sf::Event event;
        


        while (window.pollEvent(event))
        {
            if((event.type ==sf::Event::MouseButtonPressed) &&
            (event.mouseButton.button == sf::Mouse::Left)) dragging=true;
            if((event.type == sf::Event::MouseButtonReleased) &&
            (event.mouseButton.button == sf::Mouse::Left)) dragging=false;


            if(dragging){
                // std::cout<<"Dragging now"<<std::endl;
                sf::Vector2i mouseCoords = sf::Mouse::getPosition(window);
                tagParticles(mouseCoords, pcol, pshapes, spatial_grid,GRID_CELLS_COUNT_X, GRID_CELLS_COUNT_Y, GRID_CELL_DX, GRID_CELL_DY);
            }
            
            
            if (event.type == sf::Event::Closed)
            { 
                #ifdef CACHE_COLOURS
                    cacheColours(pcol, particle_count, CACHE_FILE);
                #endif
                window.close();
            }
        }
    }
    
    
    
    

    return 0;
}