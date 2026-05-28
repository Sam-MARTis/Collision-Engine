#include <SFML/Graphics.hpp>
#include <math.h>
#include "particle-system.hpp"
#include "constants.hpp"
#include "aux.hpp"
#include <random>
#include <iostream>
#include <omp.h>
#include <fstream>
#include <string>

ParticleSystem::ParticleSystem(unsigned int count, int run_mode) : particle_vertices(sf::PrimitiveType::Triangles, 6 * count), particle_dynamics(count), particle_count(count), particle_texture("circle.png"), run_mode(run_mode)
{
    assert(run_mode ==0);
    dt = DT;
    grid_cols = ceil(1.0f / (2.0f * PARTICLE_RADIUS)) + 4; // +1 for covering all non-divisible cases, +2 for ghost cell padding
    grid_rows = ceil(1.0f / (2.0f * PARTICLE_RADIUS)) + 4; // Square for now
    collision_grid.assign(grid_cols * grid_rows, std::vector<unsigned int>());
    for (auto &cell : collision_grid)
    {
        cell.reserve((size_t)RESERVE_UNITS_PER_COLLISION_GRID_CELL);
    }
    particle_dynamics.reserve(MAX_PARTICLES);
};
ParticleSystem::ParticleSystem(unsigned int count, int run_mode, std::string& reference_path): particle_vertices(sf::PrimitiveType::Triangles, 6 * count), particle_dynamics(count), particle_count(count), particle_texture("circle.png"), run_mode(run_mode)
{
    assert(run_mode == 1 || run_mode == 2);
    dt = DT;
    grid_cols = ceil(1.0f / (2.0f * PARTICLE_RADIUS)) + 4; // +1 for covering all non-divisible cases, +2 for ghost cell padding
    grid_rows = ceil(1.0f / (2.0f * PARTICLE_RADIUS)) + 4; // Square for now
    collision_grid.assign(grid_cols * grid_rows, std::vector<unsigned int>());
    for (auto &cell : collision_grid)
    {
        cell.reserve((size_t)RESERVE_UNITS_PER_COLLISION_GRID_CELL);
    }
    if(run_mode == 1){
        ids_colour_cache_path = reference_path;
        reference_image_path = DEFAULT_IMAGE_REFERENCE_PATH;
        std::ifstream file("input.txt");
    if (!file.is_open())
        throw std::runtime_error("Could not open file for reading");

    float radius_of_cached_data;
    file >> num_colour_ids_in_cache >> radius_of_cached_data;
    if(radius_of_cached_data != PARTICLE_RADIUS){
        std::cout << "Warning, radius of cached data does not match current particle radius, this might cause issues\n";
        std::cout << "Radius of cached data: " << radius_of_cached_data << ", current particle radius: " << PARTICLE_RADIUS << "\n";
    }
    colour_id_mapping.reserve(num_colour_ids_in_cache);
    for(int i=0; i<num_colour_ids_in_cache; i++){
        int r, g, b;
        file >> r >> g >> b;
        colour_id_mapping.push_back(sf::Color(r, g, b));
    }


    }else if(run_mode == 2){

        reference_image_path = reference_path;
        ids_colour_cache_path = DEFAULT_IDS_COLOUR_CACHE_PATH;
    }
    particle_dynamics.reserve(MAX_PARTICLES);
};



sf::Vector2i ParticleSystem::obtainIndexCoordsFromPosition(sf::Vector2f pos)
{
    const int x_coord = ceil((pos.x) / (2.0f * PARTICLE_RADIUS)) + 2;
    const int y_coord = ceil((pos.y) / (2.0f * PARTICLE_RADIUS)) + 2;
    return sf::Vector2i({x_coord, y_coord});
}
void ParticleSystem::updateParticlesIndicesInCollisionGrid()
{
    resetCollisionGrid();
    for (int i = 0; i < particle_count; i++)
    {
        sf::Vector2i particle_indices = obtainIndexCoordsFromPosition(particle_dynamics[i].pos);
#ifdef CHECK_GRID_UPDATE_PARTICLE_INDICES_FOR_BOUNDS
        const bool too_right = particle_indices.x > grid_cols - 2;
        const bool too_left = particle_indices.x < 1;
        const bool too_up = particle_indices.y > grid_rows - 2;
        const bool too_down = particle_indices.y < 1;
        if (too_left + too_right + too_up + too_down > 0)
        {
            std::cout << "Error in bounds during updateParticlesIndicesInCollisionGrid bounds check\n";
            std::cout << "Particle indices:\n"
                      << i << ", pos: " << particle_dynamics[i].pos.x << ", " << particle_dynamics[i].pos.y << "\n";
            std::cout << "indices: " << particle_indices.x << ", " << particle_indices.y << "\n";
            std::cout << "Grid size: " << grid_cols << "(cols), " << grid_rows << "(rows)\n";
            std::cout << "Checks(right, left, up, down respectively): " << too_right << ", " << too_left << ", " << too_up << ", " << too_down << "\n";
        }
        // if((particle_indices.x >= grid_cols-1) || (particle_indices.y >= ))
#endif
        collision_grid[flattenCoords(particle_indices.x, particle_indices.y)].push_back(i);
    }
}

void ParticleSystem::handleCollisionsFromUpdatedGrid(const int &num_global_iterations, const int &num_cell_iterations)
{
    bool something_done_in_this_global_iteration = true;
    for (int global_iter_count = 0; (global_iter_count < num_global_iterations) && something_done_in_this_global_iteration; global_iter_count++)
    {
        bool something_done_in_this_cell_iteration = false;
        for (int i = 1; i < grid_cols - 1; i++)
        {
            for (int j = 1; j < grid_rows - 1; j++)
            {
                const int grid_cell_index = flattenCoords(i, j);
                const std::vector<unsigned int> &particles_in_this_cell = collision_grid[grid_cell_index];
                if (particles_in_this_cell.size() == 0)
                    continue;
                int num_surrounding_particles = 0;
                for (int di = -1; di <= 1; di++)
                {
                    for (int dj = -1; dj <= 1; dj++)
                    {
                        if ((dj != 0) || (di != 0))
                        {
                            const int idx = flattenCoords(i + di, j + dj);
                            num_surrounding_particles += collision_grid[idx].size();
                        }
                    }
                }
                std::vector<unsigned int> surrounding_particles;
                surrounding_particles.reserve(num_surrounding_particles);
                for (int di = -1; di <= 1; di++)
                {
                    for (int dj = -1; dj <= 1; dj++)
                    {
                        if ((dj != 0) || (di != 0))
                        {
                            const std::vector<unsigned int> &particles_here = collision_grid[flattenCoords(i + di, j + dj)];
                            surrounding_particles.insert(surrounding_particles.end(), particles_here.begin(), particles_here.end());
                        }
                    }
                }
                std::vector<unsigned int> all_particles;
                all_particles.reserve(num_surrounding_particles + particles_in_this_cell.size());
                all_particles.insert(all_particles.end(), surrounding_particles.begin(), surrounding_particles.end());
                all_particles.insert(all_particles.end(), particles_in_this_cell.begin(), particles_in_this_cell.end());
                const float diam_sq = PARTICLE_RADIUS * PARTICLE_RADIUS * 4.0f;
                bool something_done_in_this_cell_iteration = true;
                for (int cell_iter_count = 0; (cell_iter_count < num_cell_iterations) && something_done_in_this_cell_iteration; cell_iter_count++)
                {
                    something_done_in_this_cell_iteration = false;
                    for (const unsigned int &id_self : particles_in_this_cell)
                    {
                        ParticleKinematics &this_particle = particle_dynamics[id_self];
                        for (unsigned int &id_other : all_particles)
                        {
                            if (id_self != id_other)
                            {
                                ParticleKinematics &other_particle = particle_dynamics[id_other];
                                const sf::Vector2f dr = other_particle.pos - this_particle.pos;
                                const float ds_sq = sfVectorNormSq(dr);
                                if (ds_sq < diam_sq)
                                {
                                    something_done_in_this_cell_iteration = true;
                                    const float ds = sqrtf(ds_sq);
                                    const float inv_ds = 1.0f / (ds);
                                    const float overlap_each_particle = PARTICLE_RADIUS - ds * 0.5f;
                                    const sf::Vector2f delta = dr * (overlap_each_particle * inv_ds);
                                    this_particle.pos -= delta;
                                    other_particle.pos += delta;
                                }
                            }
                        }
                    }
                }
                if (something_done_in_this_cell_iteration)
                    something_done_in_this_global_iteration = true;
            }
        }
    }
}

void ParticleSystem::handleCollisionsFromUpdatedGridParallel(const int &num_global_iterations, const int &num_cell_iterations)
{
    bool something_done_in_this_global_iteration = true;
    const int num_threads = omp_get_max_threads();
    const int num_cols_each_thread = ceil(0.5f * grid_cols / num_threads);
    for (int global_iter_count = 0; (global_iter_count < num_global_iterations) && something_done_in_this_global_iteration; global_iter_count++)
    {
        something_done_in_this_global_iteration = false;
        bool something_done_in_this_global_iteration_array[num_threads] = {false};
        bool something_done_in_this_cell_iteration = false;
        for (int type_iter = 0; type_iter < 2; type_iter++)
        {
// const int thread_id = ;
#pragma omp parallel
            {
                std::vector<unsigned int> all_particles;
                all_particles.reserve(RESERVE_SURROUNDING_PARTCILES + RESERVE_CURRENT_CELL_PARTICLES);
                const int starting_point = 1 + ((2 * omp_get_thread_num() + type_iter) * num_cols_each_thread);
                if (starting_point <= grid_cols - 2)
                {
                    const int ending_point = minOfTwoInts(starting_point + num_cols_each_thread, grid_cols - 1);
                    for (int i = starting_point; i < ending_point; i++)
                    {
                        for (int j = 1; j < grid_rows - 1; j++)
                        {
                            all_particles.clear();
                            const int grid_cell_index = flattenCoords(i, j);
                            const std::vector<unsigned int> &particles_in_this_cell = collision_grid[grid_cell_index];
                            if (particles_in_this_cell.size() == 0)
                                continue;
                            int num_surrounding_particles = 0;
                            for (int di = -1; di <= 1; di++)
                            {
                                for (int dj = -1; dj <= 1; dj++)
                                {
                                    // if ((dj != 0) || (di != 0))
                                    // {
                                    //     const int idx = flattenCoords(i + di, j + dj);
                                    //     num_surrounding_particles += collision_grid[idx].size();
                                    // }

                                    for (const int &id : collision_grid[flattenCoords(i + di, j + dj)])
                                    {
                                        all_particles.push_back(id);
                                    }
                                }
                            }
                            if (all_particles.size() <= 1)
                                continue;

                            // std::vector<unsigned int> surrounding_particles;
                            // surrounding_particles.reserve(num_surrounding_particles);
                            // for (int di = -1; di <= 1; di++)
                            // {
                            //     for (int dj = -1; dj <= 1; dj++)
                            //     {
                            //         if ((dj != 0) || (di != 0))
                            //         {
                            //             const std::vector<unsigned int> &particles_here = collision_grid[flattenCoords(i + di, j + dj)];
                            //             surrounding_particles.insert(surrounding_particles.end(), particles_here.begin(), particles_here.end());
                            //         }
                            //     }
                            // }
                            // std::vector<unsigned int> all_particles;
                            // all_particles.reserve(num_surrounding_particles + particles_in_this_cell.size());
                            // all_particles.insert(all_particles.end(), surrounding_particles.begin(), surrounding_particles.end());
                            // all_particles.insert(all_particles.end(), particles_in_this_cell.begin(), particles_in_this_cell.end());
                            const float diam_sq = PARTICLE_RADIUS * PARTICLE_RADIUS * 4.0f;
                            bool something_done_in_this_cell_iteration = true;
                            for (int cell_iter_count = 0; (cell_iter_count < num_cell_iterations) && something_done_in_this_cell_iteration; cell_iter_count++)
                            {
                                something_done_in_this_cell_iteration = false;
                                for (const unsigned int &id_self : particles_in_this_cell)
                                {
                                    ParticleKinematics &this_particle = particle_dynamics[id_self];
                                    for (unsigned int &id_other : all_particles)
                                    {
                                        if (id_self != id_other)
                                        {
                                            ParticleKinematics &other_particle = particle_dynamics[id_other];
                                            const sf::Vector2f dr = other_particle.pos - this_particle.pos;
                                            const float ds_sq = sfVectorNormSq(dr);
                                            if (ds_sq < diam_sq)
                                            {
                                                something_done_in_this_cell_iteration = true;
                                                const float ds = sqrtf(ds_sq);
                                                const float inv_ds = 1.0f / (ds);
                                                const float overlap_each_particle = PARTICLE_RADIUS - ds * 0.5f;
                                                const sf::Vector2f delta = dr * (overlap_each_particle * inv_ds);
                                                this_particle.pos -= delta;
                                                other_particle.pos += delta;
                                            }
                                        }
                                    }
                                }
                            }
                            if (something_done_in_this_cell_iteration)
                            {
                                something_done_in_this_global_iteration_array[omp_get_thread_num()] = true;
                            }
                        }
                    }
                }
            }
        }
        for(int l=0; l<num_threads; l++){
            if(something_done_in_this_global_iteration_array[l]){
                something_done_in_this_global_iteration = true;
                break;
            }
        }
    }
}

void ParticleSystem::solveCollisions(const int collision_num_global_iterations, const int collision_num_cell_iterations)
{
    updateParticlesIndicesInCollisionGrid();
    handleCollisionsFromUpdatedGridParallel(collision_num_global_iterations, collision_num_cell_iterations);
}


void ParticleSystem::addGravitationalAcceleration()
{
    for (int i = 0; i < particle_count; i++)
    {
        particle_dynamics[i].acc.y += GRAVITY;
    }
}
void ParticleSystem::stepForwardTime()
{
    for (int i = 0; i < particle_count; i++)
    {
        ParticleKinematics &particle = particle_dynamics[i];
        const sf::Vector2f new_pos = 2.0f * particle.pos - particle.prev_pos + particle.acc * dt * dt;
        const bool cross_right_boundary = new_pos.x > (1.0f - PARTICLE_RADIUS);
        const bool cross_left_boundary = new_pos.x < PARTICLE_RADIUS;
        const bool cross_top_boundary = new_pos.y > (1.0f - PARTICLE_RADIUS);
        const bool cross_bottom_boundary = new_pos.y < PARTICLE_RADIUS;
        particle.prev_pos.x = particle.pos.x;
        particle.prev_pos.y = particle.pos.y;
        particle.pos.x = new_pos.x;
        particle.pos.y = new_pos.y;
        // if((cross_bottom_boundary + cross_left_boundary + cross_right_boundary + cross_top_boundary)==false){
        // }else
        if (cross_bottom_boundary)
        {
            particle.pos.y = particle.prev_pos.y = PARTICLE_RADIUS;
        }
        if (cross_top_boundary)
        {
            particle.pos.y = particle.prev_pos.y = 1.0f - PARTICLE_RADIUS;
        }
        if (cross_left_boundary)
        {
            particle.pos.x = particle.prev_pos.x = PARTICLE_RADIUS;
        }
        if (cross_right_boundary)
        {
            particle.pos.x = particle.prev_pos.x = 1.0f - PARTICLE_RADIUS;
        }
    }
}