
#include "particle-system.hpp"
#include "constants.hpp"
#include<SFML/Graphics.hpp>

void display_test(sf::RenderWindow& window){
    sf::Texture texture("circle.png");
    auto [tw, th] = texture.getSize();
    float ftw = (float)tw;
    float fth = (float)th;

    sf::VertexArray va(sf::PrimitiveType::Triangles, 12); // 6 vertices * 2 quads
    float pos[4] = {200.0f, 200.0f, 400.0f, 400.0f};
    const float r = 100.0f;

    for(int i = 0; i < 2; i++){
        float x = pos[2*i];
        float y = pos[2*i + 1];

        // Triangle 1
        va[6*i + 0] = {{x-r, y-r}, sf::Color::White, {0.f,  0.f }}; // Top left
        va[6*i + 1] = {{x-r, y+r}, sf::Color::White, {0.f,  fth }}; // Bottom left
        va[6*i + 2] = {{x+r, y-r}, sf::Color::White, {ftw,  0.f }}; // Top right

        // Triangle 2
        va[6*i + 3] = {{x-r, y+r}, sf::Color::White, {0.f,  fth }}; // Bottom left
        va[6*i + 4] = {{x+r, y+r}, sf::Color::White, {ftw,  fth }}; // bottom right
        va[6*i + 5] = {{x+r, y-r}, sf::Color::White, {ftw,  0.f }}; // Top right
    }

    sf::RenderStates states;
    states.texture = &texture;
    window.draw(va, states);
    window.display();
}
// void display_test_02(sf::RenderWindow& window){
//     ParticleSystem particles(100);
//     particles.setScale(sf::Vector2f({100, 100}));
//     particles.resetParticlesRandom();
//     particles.setupRendering();
//     window.draw(particles);
// }

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
void ParticleSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const{
    states.transform *= getTransform();
    states.texture = &particle_texture;
    target.draw(particle_vertices, states);
}

