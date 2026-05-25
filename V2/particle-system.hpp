
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

class ParticleSystem : public sf::Drawable, public sf::Transformable
{
private:
    struct ParticleKinematics {
        sf::Vector2f pos;
        sf::Vector2f prev_pos;
        sf::Vector2f acc;
    };

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    

    std::vector<ParticleKinematics> particle_dynamics;
    sf::VertexArray         particle_vertices;
    sf::Texture                     particle_texture;
    unsigned int                    particle_count;
    float                  dt;
public:
    ParticleSystem(unsigned int count);
    void resetParticlesRandom();
    void updateVerticesPositionFromCache();
    void setupRendering();
    void addGravitationalAcceleration();
    void stepForwardTime();
    void setDt(float deltat);
};