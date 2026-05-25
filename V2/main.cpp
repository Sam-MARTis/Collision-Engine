#include<SFML/Graphics.hpp>

#include"constants.hpp"


int main(){
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 16;
    sf::RenderWindow window = sf::RenderWindow(
        sf::VideoMode({WINDOW_SIZE_X, WINDOW_SIZE_Y}), "Particle collision sim"
    );
    while(window.isOpen()){

        while(auto event = window.pollEvent()){
            if(event->is<sf::Event::Closed>()){
                window.close();
            }
        }
    }
}
