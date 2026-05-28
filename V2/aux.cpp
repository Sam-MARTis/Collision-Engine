#include "particle-system.hpp"
#include "constants.hpp"
void ParticleSystem::setDt(float deltat){
    dt = deltat;
};
void ParticleSystem::resetCollisionGrid()
{
    for (auto &cell : collision_grid)
    {
        cell.clear();
    }
};
void ParticleSystem::resetParticlesRandom()
{
    // static std::random_device rd;
    static std::mt19937 rng(42);

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

void ParticleSystem::addParticle(sf::Vector2f pos, sf::Vector2f vel){
    ParticleKinematics new_particle;
    new_particle.pos = pos;
    new_particle.prev_pos = pos - vel*dt;
    new_particle.acc = sf::Vector2f({0.0f, 0.0f});
    particle_dynamics.push_back(new_particle);
    particle_count++;

    particle_vertices.resize(6*particle_count);
    const int i = particle_count-1;
    


    // void ParticleSystem::setupRendering()
    auto [tw, th] = particle_texture.getSize();
    const float ftw = (float)tw;
    const float fth = (float)th;
    const float r = PARTICLE_RADIUS;


    const float x = particle_dynamics[i].pos.x;
    const float y = particle_dynamics[i].pos.y;
    particle_vertices[6 * i + 0] = {{x - r, y - r}, sf::Color::White, {0.f, 0.f}}; // Top left
    particle_vertices[6 * i + 1] = {{x - r, y + r}, sf::Color::White, {0.f, fth}}; // Bottom left
    particle_vertices[6 * i + 2] = {{x + r, y - r}, sf::Color::White, {ftw, 0.f}}; // Top right

    // Triangle 2
    particle_vertices[6 * i + 3] = {{x - r, y + r}, sf::Color::White, {0.f, fth}}; // Bottom left
    particle_vertices[6 * i + 4] = {{x + r, y + r}, sf::Color::White, {ftw, fth}}; // bottom right
    particle_vertices[6 * i + 5] = {{x + r, y - r}, sf::Color::White, {ftw, 0.f}}; // Top right

};

void ParticleSystem::addParticles(std::vector<sf::Vector2f> poss, std::vector<sf::Vector2f> vels){
    assert(poss.size() == vels.size());
    const float new_particle_count = particle_count + poss.size();
    particle_vertices.resize(6*new_particle_count);
    auto [tw, th] = particle_texture.getSize();
    const float ftw = (float)tw;
    const float fth = (float)th;
    const float r = PARTICLE_RADIUS;
    
    for(int i = particle_count; i < particle_count+poss.size(); i++){
        ParticleKinematics new_particle;
        new_particle.pos = poss[i-particle_count];
        new_particle.prev_pos = poss[i-particle_count] - vels[i-particle_count]*dt;
        new_particle.acc = sf::Vector2f({0.0f, 0.0f});
        particle_dynamics.push_back(new_particle);
    
    
        const float x = particle_dynamics[i].pos.x;
        const float y = particle_dynamics[i].pos.y;
        particle_vertices[6 * i + 0] = {{x - r, y - r}, sf::Color::White, {0.f, 0.f}}; // Top left
        particle_vertices[6 * i + 1] = {{x - r, y + r}, sf::Color::White, {0.f, fth}}; // Bottom left
        particle_vertices[6 * i + 2] = {{x + r, y - r}, sf::Color::White, {ftw, 0.f}}; // Top right
    
        // Triangle 2
        particle_vertices[6 * i + 3] = {{x - r, y + r}, sf::Color::White, {0.f, fth}}; // Bottom left
        particle_vertices[6 * i + 4] = {{x + r, y + r}, sf::Color::White, {ftw, fth}}; // bottom right
        particle_vertices[6 * i + 5] = {{x + r, y - r}, sf::Color::White, {ftw, 0.f}}; // Top right
    // particle_count++;
    }

    particle_count = new_particle_count;

    // const int i = particle_count-1;
    


    // void ParticleSystem::setupRendering()

};

// void 
