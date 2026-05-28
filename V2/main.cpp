#include<SFML/Graphics.hpp>
#include "particle-system.hpp"
#include"constants.hpp"
#include"display-functions.hpp"


int main(){
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 16;
    sf::RenderWindow window = sf::RenderWindow(
        sf::VideoMode({WINDOW_SIZE_X, WINDOW_SIZE_Y}), "Particle collision sim"
    );
    window.setFramerateLimit(60);
    

    ParticleSystem particles(PARTICLE_COUNT);
    // particles.setScale()
    particles.setScale(sf::Vector2f({WINDOW_SIZE_X/2, -WINDOW_SIZE_Y/2}));
    particles.setOrigin(sf::Vector2f({-0.5f, 1.5f}));
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
        particles.addGravitationalAcceleration();
        particles.stepForwardTime();
        particles.solveCollisions(10, 100);
        particles.updateVerticesPositionFromCache();

    }
}