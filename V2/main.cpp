#include <SFML/Graphics.hpp>
#include "particle-system.hpp"
#include "constants.hpp"
#include "display-functions.hpp"
#include <random>
#include <omp.h>
#include <iostream>

int main()
{
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 16;
    sf::RenderWindow window = sf::RenderWindow(
        sf::VideoMode({WINDOW_SIZE_X, WINDOW_SIZE_Y}), "Particle collision sim");
    const int max_threads = omp_get_max_threads();
    omp_set_num_threads(max_threads);
    const int current_threads = omp_get_num_threads();
    std::cout << "Max threads: " << max_threads << "\n";
#pragma omp parallel
    {
#pragma omp single // print only once
        std::cout << "Threads in parallel region: " << omp_get_num_threads() << "\n";
    }
    window.setFramerateLimit(144);

    ParticleSystem particles(PARTICLE_COUNT);
    // particles.setScale()
    const float fraction_of_window_covered_by_particle_system = 0.8f;
    particles.setScale(sf::Vector2f({WINDOW_SIZE_X * fraction_of_window_covered_by_particle_system, -WINDOW_SIZE_Y * fraction_of_window_covered_by_particle_system}));
    particles.setOrigin(sf::Vector2f({-(1 - fraction_of_window_covered_by_particle_system) / 2, (3 - fraction_of_window_covered_by_particle_system) / 2}));
    particles.resetParticlesRandom();
    particles.setupRendering();
    float sim_time = 0;
    float add_time = 0;
    const float time_to_add_particles = 3*PARTICLE_RADIUS / PARTICLE_ADDING_VELOCITY_X;
    while (window.isOpen())
    {

        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            if (event->is<sf::Event::MouseButtonPressed>()){
                std::cout << "Number of particles before adding: " << particles.getNumParticles() << "\n";
            }
        }
        window.clear();
        // display_test(window);
        window.draw(particles);
        window.display();
        for (int i = 0; i < COMPUTE_PER_RENDER; i++)
        {
            particles.addGravitationalAcceleration();
            particles.stepForwardTime();
            particles.solveCollisions(COLLISIO_GLOBAL_NUM_ITERATIONS, COLLISION_CELL_NUM_ITERATIONS);
            sim_time += DT;
            if(sim_time>0.1){
                add_time += DT;
            }
        }
        
        if((add_time>time_to_add_particles) && (particles.getNumParticles() < MAX_PARTICLES)){
   
            std::vector<sf::Vector2f> poss;
            std::vector<sf::Vector2f> vels;
            poss.reserve(NUM_PARTICLES_TO_ADD_EVERY_TIME);
            vels.reserve(NUM_PARTICLES_TO_ADD_EVERY_TIME);
            
            for(int i = 0; i < NUM_PARTICLES_TO_ADD_EVERY_TIME; i++){
                
                poss.push_back(sf::Vector2f({4*PARTICLE_RADIUS, 1.0f -(i+2)* 2.4f*PARTICLE_RADIUS}));
                vels.push_back(sf::Vector2f({PARTICLE_ADDING_VELOCITY_X, 0.0f}));
                // particles.addParticle(sf::Vector2f({0.1f, 0.7f + i* 2.4f*PARTICLE_RADIUS}), sf::Vector2f({PARTICLE_ADDING_VELOCITY_X, 0.0f}));
                // particles.addParticle(sf::Vector2f({0.1f, 0.82f}), sf::Vector2f({6.1f, 0.0f}));
                // particles.addParticle(sf::Vector2f({0.1f, 0.78f}), sf::Vector2f({6.1f, 0.0f}));
                
            }
            particles.addParticles(poss, vels);
            add_time -= time_to_add_particles;
        }
        // std::cout << "Sim time: " << sim_time << ", Add time: " << add_time << "\n";
        particles.updateVerticesPositionFromCache();
    }
}