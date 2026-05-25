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
        const bool cross_right_boundary = new_pos.x>1.0f;
        const bool cross_left_boundary = new_pos.x < 0.0f;
        const bool cross_top_boundary = new_pos.y > 1.0f;
        const bool cross_bottom_boundary = new_pos.y < 0.0f;
        particle.prev_pos.x = particle.pos.x;
        particle.prev_pos.y = particle.pos.y;
        particle.pos.x = new_pos.x;
        particle.pos.y = new_pos.y;
        // if((cross_bottom_boundary + cross_left_boundary + cross_right_boundary + cross_top_boundary)==false){
        // }else 
        if (cross_bottom_boundary)
        {
            particle.pos.y = particle.prev_pos.y = 0.0f;
        }
        if (cross_top_boundary)
        {
            particle.pos.y = particle.prev_pos.y = 1.0f;
        }
        if (cross_left_boundary)
        {
            particle.pos.x = particle.prev_pos.x = 0.0f;
        }
        if (cross_right_boundary)
        {
            particle.pos.x = particle.prev_pos.x = 1.0f;
        }
        
    }
}