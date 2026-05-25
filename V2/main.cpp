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
    

    ParticleSystem particles(100);
    particles.setScale(sf::Vector2f({WINDOW_SIZE_X/2, WINDOW_SIZE_Y/2}));
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
        
    }
}