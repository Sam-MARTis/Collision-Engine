#pragma once
#define PARTICLE_RADIUS 1.5e-3f
#define PARTICLE_COUNT 55000

#define WINDOW_SIZE_X 1000
#define WINDOW_SIZE_Y 1000

// Computational constants
#define RESERVE_UNITS_PER_COLLISION_GRID_CELL 0
#define COMPUTE_PER_RENDER 3

#define COLLISIO_GLOBAL_NUM_ITERATIONS 3
#define COLLISION_CELL_NUM_ITERATIONS 3

// Physics constants
#define GRAVITY (1e-1f)
#define DT (1e-4)




// Debug constants
#define CHECK_GRID_UPDATE_PARTICLE_INDICES_FOR_BOUNDS