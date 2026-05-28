#include<SFML/Graphics.hpp>
#include "particle-system.hpp"
#include"constants.hpp"
#include"display-functions.hpp"
#include<random>


int main(){
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 16;
    sf::RenderWindow window = sf::RenderWindow(
        sf::VideoMode({WINDOW_SIZE_X, WINDOW_SIZE_Y}), "Particle collision sim"
    );
    window.setFramerateLimit(60);
    

    ParticleSystem particles(PARTICLE_COUNT);
    // particles.setScale()
    const float fraction_of_window_covered_by_particle_system = 0.8f;
    particles.setScale(sf::Vector2f({WINDOW_SIZE_X * fraction_of_window_covered_by_particle_system, -WINDOW_SIZE_Y * fraction_of_window_covered_by_particle_system}));
    particles.setOrigin(sf::Vector2f({-(1-fraction_of_window_covered_by_particle_system)/2, (3-fraction_of_window_covered_by_particle_system)/2}));
    particles.resetParticlesRandom();
    particles.setupRendering();
    while(window.isOpen()){
        
        while(auto event = window.pollEvent()){
            if(event->is<sf::Event::Closed>()){
                window.close();
            }
        }
        window.clear();
        // display_test(window);
        window.draw(particles);
        window.display();
        for(int i=0; i<COMPUTE_PER_RENDER; i++){
            particles.addGravitationalAcceleration();
            particles.stepForwardTime();
            particles.solveCollisions(COLLISIO_GLOBAL_NUM_ITERATIONS, COLLISION_CELL_NUM_ITERATIONS);
        }
        particles.updateVerticesPositionFromCache();

    }
}