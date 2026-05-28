
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

class ParticleSystem : public sf::Drawable, public sf::Transformable
{
private:
    struct ParticleKinematics
    {
        sf::Vector2f pos;
        sf::Vector2f prev_pos;
        sf::Vector2f acc;
    };

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
    void resetCollisionGrid();
    sf::Vector2i obtainIndexCoordsFromPosition(sf::Vector2f pos);

    std::vector<ParticleKinematics> particle_dynamics;
    sf::VertexArray particle_vertices;
    sf::Texture particle_texture;
    unsigned int particle_count;
    float dt;
    unsigned int grid_rows;
    unsigned int grid_cols;
    std::vector<std::vector<unsigned int>> collision_grid;
    inline int flattenCoords(int i, int j);
    inline float sfVectorNormSq(const sf::Vector2f& vec);
public:
    ParticleSystem(unsigned int count);
    void resetParticlesRandom();
    void updateVerticesPositionFromCache();
    void setupRendering();
    void addGravitationalAcceleration();
    void stepForwardTime();
    void setDt(float deltat);
    void updateParticlesIndicesInCollisionGrid(); //Note, this will call reset on its own
    void handleCollisions(int num_global_iterations, int num_cell_iterations);
};