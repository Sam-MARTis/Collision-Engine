
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
    std::vector<sf::Color> colour_id_mapping;
    int num_colour_ids_in_cache = 0;  
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
    void resetCollisionGrid();
    sf::Vector2i obtainIndexCoordsFromPosition(sf::Vector2f pos);

    std::vector<ParticleKinematics> particle_dynamics;
    sf::VertexArray particle_vertices;
    sf::Texture particle_texture;
    unsigned int particle_count;
    float dt;
    const int run_mode; 
    unsigned int grid_rows;
    unsigned int grid_cols;
    std::string reference_image_path;
    std::string ids_colour_cache_path;
    std::vector<std::vector<unsigned int>> collision_grid;
    inline int flattenCoords(int i, int j);
    inline float sfVectorNormSq(const sf::Vector2f& vec);
    inline int minOfTwoInts(int a, int b){
        return (a<b)?a:b;
    }
    void updateParticlesIndicesInCollisionGrid(); //Note, this will call reset on its own
    void handleCollisionsFromUpdatedGrid(const int& num_global_iterations, const int& num_cell_iterations);
    void handleCollisionsFromUpdatedGridParallel(const int& num_global_iterations, const int& num_cell_iterations);
public:
    ParticleSystem(unsigned int count, int run_mode);
    ParticleSystem(unsigned int count, int run_mode, std::string& reference_path);
    void resetParticlesRandom();
    void updateVerticesPositionFromCache();
    void setupRendering();
    void addGravitationalAcceleration();
    void stepForwardTime();
    void solveCollisions(const int collision_num_global_iterations, const int collision_num_cell_iterations);
    void setDt(float deltat);
    void addParticles(std::vector<sf::Vector2f> poss, std::vector<sf::Vector2f> vels);

    float getNumParticles(){
        return particle_count;
    }
    
};