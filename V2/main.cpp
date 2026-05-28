#include <SFML/Graphics.hpp>
#include "particle-system.hpp"
#include "constants.hpp"
#include "display-functions.hpp"
#include <random>
#include <omp.h>
#include <iostream>

int run_mode = 0;
int main(int argc, char *argv[])
{
    std::string reference_path;
    std::string cache_path;
    if (argc > 1)
    {
        std::string mode_arg = argv[1];
        if (mode_arg == "1")
        {
            run_mode = 1;
            argc > 2 ? cache_path = argv[2] : cache_path = DEFAULT_IDS_COLOUR_CACHE_PATH;
        }
        else if (mode_arg == "2")
        {
            run_mode = 2;
            argc > 2 ? reference_path = argv[2] : reference_path = DEFAULT_IMAGE_REFERENCE_PATH;
            argc > 3 ? cache_path = argv[3] : cache_path = DEFAULT_IDS_COLOUR_CACHE_PATH;
        }
        else if (mode_arg == "0")
        {
            run_mode = 0;
        }
        else
        {
            std::cout << "Invalid mode argument, use 0 for normal run, 1 for writing to cache, 2 for reading from cache\n";
            return 1;
        }
    }
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
    // if(run_mode==0){
    ParticleSystem particles(PARTICLE_COUNT, run_mode, cache_path, reference_path);
    // }else{
    //     ParticleSystem particles(PARTICLE_COUNT, run_mode, reference_path);
    // // }
    // ParticleSystem 1==1? particles()
    // ParticleSystem particles = run_mode==0? ParticleSystem({PARTICLE_COUNT, run_mode}): run_mode==1? ParticleSystem({PARTICLE_COUNT, run_mode, reference_path}): ParticleSystem({PARTICLE_COUNT, run_mode, reference_path, cache_path});
    // particles.setScale()
    const float fraction_of_window_covered_by_particle_system = 0.8f;
    particles.setScale(sf::Vector2f({WINDOW_SIZE_X * fraction_of_window_covered_by_particle_system, -WINDOW_SIZE_Y * fraction_of_window_covered_by_particle_system}));
    particles.setOrigin(sf::Vector2f({-(1 - fraction_of_window_covered_by_particle_system) / 2, (3 - fraction_of_window_covered_by_particle_system) / 2}));
    particles.resetParticlesRandom();
    particles.setupRendering();
    float sim_time = 0;
    float add_time = 0;
    const float time_to_add_particles = 3 * PARTICLE_RADIUS / PARTICLE_ADDING_VELOCITY_X;
    bool saved_image = false;
    while (window.isOpen())
    {

        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            if (event->is<sf::Event::MouseButtonPressed>())
            {
                std::cout << "Number of particles before adding: " << particles.getNumParticles() << "\n";
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
            {
                if (!saved_image)
                {
                    std::cout << "Space pressed" << "\n";
                    particles.ComputeColourIdMapping();
                    // saved_image = true;
                }
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
            if (sim_time > 0.1)
            {
                add_time += DT;
            }
        }

        if ((add_time > time_to_add_particles) && (particles.getNumParticles() < MAX_PARTICLES))
        {

            std::vector<sf::Vector2f> poss;
            std::vector<sf::Vector2f> vels;
            poss.reserve(NUM_PARTICLES_TO_ADD_EVERY_TIME);
            vels.reserve(NUM_PARTICLES_TO_ADD_EVERY_TIME);

            for (int i = 0; i < NUM_PARTICLES_TO_ADD_EVERY_TIME; i++)
            {

                poss.push_back(sf::Vector2f({4 * PARTICLE_RADIUS, 1.0f - (i + 2) * 2.4f * PARTICLE_RADIUS}));
                vels.push_back(sf::Vector2f({PARTICLE_ADDING_VELOCITY_X, 0.0f}));
                // particles.addParticle(sf::Vector2f({0.1f, 0.7f + i* 2.4f*PARTICLE_RADIUS}), sf::Vector2f({PARTICLE_ADDING_VELOCITY_X, 0.0f}));
                // particles.addParticle(sf::Vector2f({0.1f, 0.82f}), sf::Vector2f({6.1f, 0.0f}));
                // particles.addParticle(sf::Vector2f({0.1f, 0.78f}), sf::Vector2f({6.1f, 0.0f}));
            }
            particles.addParticles(poss, vels);
            add_time -= time_to_add_particles;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        {
            std::cout << "A pressed, adding particles\n";
            std::vector<sf::Vector2f> poss;
            std::vector<sf::Vector2f> vels;
            poss.reserve(NUM_PARTICLES_TO_ADD_EVERY_TIME);
            vels.reserve(NUM_PARTICLES_TO_ADD_EVERY_TIME);

            for (int i = 0; i < NUM_PARTICLES_TO_ADD_EVERY_TIME; i++)
            {

                poss.push_back(sf::Vector2f({4 * PARTICLE_RADIUS, 1.0f - (i + 2) * 2.4f * PARTICLE_RADIUS}));
                vels.push_back(sf::Vector2f({PARTICLE_ADDING_VELOCITY_X, 0.0f}));
                // particles.addParticle(sf::Vector2f({0.1f, 0.7f + i* 2.4f*PARTICLE_RADIUS}), sf::Vector2f({PARTICLE_ADDING_VELOCITY_X, 0.0f}));
                // particles.addParticle(sf::Vector2f({0.1f, 0.82f}), sf::Vector2f({6.1f, 0.0f}));
                // particles.addParticle(sf::Vector2f({0.1f, 0.78f}), sf::Vector2f({6.1f, 0.0f}));
            }
            particles.addParticles(poss, vels);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        {
            std::cout << "S pressed, saving colour id mapping cache\n";
            particles.cacheColourIdMapping();
        }
        // std::cout << "Sim time: " << sim_time << ", Add time: " << add_time << "\n";
        particles.updateVerticesPositionFromCache();
    }
}