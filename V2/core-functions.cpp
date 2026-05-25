#include <SFML/Graphics.hpp>

#include "particle-system.hpp"
#include "constants.hpp"
#include <random>


ParticleSystem::ParticleSystem(unsigned int count) : particle_vertices(sf::PrimitiveType::Triangles, 6 * count), particle_dynamics(count), particle_count(count), particle_texture("circle.png") {
    dt = DT;
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

void ParticleSystem::addGravitationalAcceleration(){
    for(int i=0; i<particle_count; i++){
        particle_dynamics[i].acc.y += GRAVITY;
    }
}
void ParticleSystem::stepForwardTime(){
    for(int i=0; i<particle_count; i++){
        ParticleKinematics& particle = particle_dynamics[i];
        const sf::Vector2f new_pos = 2.0f*particle.pos  - particle.prev_pos + particle.acc*dt*dt;
        particle.prev_pos.x = particle.pos.x;
        particle.prev_pos.y = particle.pos.y;
        particle.pos.x = new_pos.x;
        particle.pos.y = new_pos.y;
    }
}