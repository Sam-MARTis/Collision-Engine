#include <SFML/Graphics.hpp>
#include <random>

class ParticleSystem : public sf::Drawable, public sf::Transformable
{
    private:
    struct ParticleKinematics{
        sf::Vector2f pos;
        sf::Vector2f prev_pos;
        sf::Vector2f acc;
    };
    std::vector<ParticleKinematics> particle_dynamics;
    sf::VertexArray particle_vertices;
    sf::Texture particle_texture;
    unsigned int particle_count;
    public:
    ParticleSystem(unsigned int particle_count) : particle_vertices(sf::PrimitiveType::Triangles, 6*particle_count), particle_dynamics(particle_count){

    }
    void resetParticlesRandom(){
        static std::random_device rd;
        static std::mt19937 rng(rd());

        for(int i=0; i<particle_count*2; i++){
            const float rand_val1 = std::uniform_real_distribution(0.0f, 1.0f)(rng);
            const float rand_val2 = std::uniform_real_distribution(0.0f, 1.0f)(rng);
            ParticleKinematics& particle = particle_dynamics[i];
            particle.pos.x = rand_val1;
            particle.pos.y = rand_val2;
            particle.prev_pos.x = rand_val1;
            particle.prev_pos.y = rand_val2;
        }
    }
};