#pragma once
#define PARTICLE_RADIUS 3e-3f
#define PARTICLE_COUNT 0
#define MAX_PARTICLES 30000

#define WINDOW_SIZE_X 1000
#define WINDOW_SIZE_Y 1000

#define PARTICLE_ADDING_VELOCITY_X 7.0f
#define NUM_PARTICLES_TO_ADD_EVERY_TIME 20
// Computational constants
#define RESERVE_UNITS_PER_COLLISION_GRID_CELL 1
#define COMPUTE_PER_RENDER 20

#define COLLISIO_GLOBAL_NUM_ITERATIONS 3
#define COLLISION_CELL_NUM_ITERATIONS 5

#define RESERVE_SURROUNDING_PARTCILES 12
#define RESERVE_CURRENT_CELL_PARTICLES 2

// Physics constants
#define GRAVITY (-1e-1f)
#define DT (1e-4)


// Image rendering constants
#define DEFAULT_IMAGE_REFERENCE_PATH "reference_image.png"
#define DEFAULT_IDS_COLOUR_CACHE_PATH "ids_colour_cache.png"
// Also, use in mode 0 for not doing anything, mode 1 for writing to cache, mode 2 for reading from cache

#define DEFAULT_COLOUR_FOR_UNASSIGNED_ID sf::Color::Blue


// Debug constants
// #define CHECK_GRID_UPDATE_PARTICLE_INDICES_FOR_BOUNDS