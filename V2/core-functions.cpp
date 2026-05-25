#include <SFML/Graphics.hpp>

#include "particle-system.hpp"
#include "constants.hpp"
#include <random>


ParticleSystem::ParticleSystem(unsigned int count) : particle_vertices(sf::PrimitiveType::Triangles, 6 * count), particle_dynamics(count), particle_count(count), particle_texture("circle.png") {
};

void ParticleSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const{
    states.transform *= getTransform();
    states.texture = &particle_texture;
    target.draw(particle_vertices, states);
}

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
void ParticleSystem::setupRendering()
{
    auto [tw, th] = particle_texture.getSize();
    const float ftw = (float)tw;
    const float fth = (float)th;
    const float r = PARTICLE_RADIUS;

    for (int i = 0; i < particle_count; i++)
    {
        const float x = particle_dynamics[i].pos.x;
        const float y = particle_dynamics[i].pos.y;
        particle_vertices[6 * i + 0] = {{x - r, y - r}, sf::Color::White, {0.f, 0.f}}; // Top left
        particle_vertices[6 * i + 1] = {{x - r, y + r}, sf::Color::White, {0.f, fth}}; // Bottom left
        particle_vertices[6 * i + 2] = {{x + r, y - r}, sf::Color::White, {ftw, 0.f}}; // Top right

        // Triangle 2
        particle_vertices[6 * i + 3] = {{x - r, y + r}, sf::Color::White, {0.f, fth}}; // Bottom left
        particle_vertices[6 * i + 4] = {{x + r, y + r}, sf::Color::White, {ftw, fth}}; // bottom right
        particle_vertices[6 * i + 5] = {{x + r, y - r}, sf::Color::White, {ftw, 0.f}}; // Top right
    }
};
void ParticleSystem::updateVerticesPositionFromCache()
{
    // REMEMBER TO CALL THIS BEFORE DRAW. This is going to cause headaches in the future
    const float r = PARTICLE_RADIUS;
    for (int i = 0; i < particle_count; i++)
    {
        const float x = particle_dynamics[i].pos.x;
        const float y = particle_dynamics[i].pos.y;
        particle_vertices[6 * i + 0].position = {x - r, y - r};
        particle_vertices[6 * i + 1].position = {x - r, y + r};
        particle_vertices[6 * i + 2].position = {x + r, y - r};

        // Triangle 2
        particle_vertices[6 * i + 3].position = {x - r, y + r};
        particle_vertices[6 * i + 4].position = {x + r, y + r};
        particle_vertices[6 * i + 5].position = {x + r, y - r};
    }
};