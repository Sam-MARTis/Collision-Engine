#include <SFML/Graphics.hpp>

#include "particle-system.hpp"
#include "constants.hpp"
#include <random>


ParticleSystem::ParticleSystem(unsigned int count) : particle_vertices(sf::PrimitiveType::Triangles, 6 * count), particle_dynamics(count), particle_count(count), particle_texture("circle.png") {
};


void ParticleSystem::resetParticlesRandom()
{
    static std::random_device rd;
    static std::mt19937 rng(rd());

    for (int i = 0; i < particle_count; i++)
    {
        const float rand_val1 = std::uniform_real_distribution(0.0f, 1.0f)(rng);
        const float rand_val2 = std::uniform_real_distribution(0.0f, 1.0f)(rng);
        ParticleKinematics &particle = particle_dynamics[i];
        particle.pos.x = rand_val1;
        particle.pos.y = rand_val2;
        particle.prev_pos.x = rand_val1;
        particle.prev_pos.y = rand_val2;
    }
}
