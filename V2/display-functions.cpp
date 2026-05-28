
#include "particle-system.hpp"
#include "constants.hpp"
#include<SFML/Graphics.hpp>
#include<iostream>
#include<fstream>

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
        sf::Color particle_color = DEFAULT_COLOUR_FOR_UNASSIGNED_ID;
        if(run_mode ==1){
            if(i < num_colour_ids_in_cache){
                particle_color = colour_id_mapping[i];
            }else{
                std::cout << "Warning, particle index " << i << " does not have a corresponding colour in the cache, using default colour instead\n";
            }
        }
        particle_vertices[6 * i + 0] = {{x - r, y - r}, particle_color, {0.f, 0.f}}; // Top left
        particle_vertices[6 * i + 1] = {{x - r, y + r}, particle_color, {0.f, fth}}; // Bottom left
        particle_vertices[6 * i + 2] = {{x + r, y - r}, particle_color, {ftw, 0.f}}; // Top right

        // Triangle 2
        particle_vertices[6 * i + 3] = {{x - r, y + r}, particle_color, {0.f, fth}}; // Bottom left
        particle_vertices[6 * i + 4] = {{x + r, y + r}, particle_color, {ftw, fth}}; // bottom right
        particle_vertices[6 * i + 5] = {{x + r, y - r}, particle_color, {ftw, 0.f}}; // Top right
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


void ParticleSystem::ComputeColourIdMapping(){
    sf::Image reference_image(reference_image_path);
    if (!reference_image.getSize().x || !reference_image.getSize().y)
        throw std::runtime_error("Could not load reference image for computing colour id mapping");
    auto [img_width, img_height] = reference_image.getSize();
    const float fraction_to_fit = 0.8f;
    const float scaling_factor_x = img_width *fraction_to_fit;
    const float scaling_factor_y = img_height*fraction_to_fit;
    const float scaling_factor = scaling_factor_x < scaling_factor_y ? scaling_factor_x : scaling_factor_y;
    const float origin_x = 0.5f;
    const float origin_y = 0.5f;
    colour_id_mapping.clear();
    colour_id_mapping.reserve(particle_count);
    for(unsigned int i=0; i<particle_count; i++){

        const ParticleKinematics &particle = particle_dynamics[i];
        int image_x = (int)((particle.pos.x - origin_x)*scaling_factor + img_width/2);
        int image_y = (int)(img_height - ((particle.pos.y - origin_y)*scaling_factor + img_height/2));
        if(image_x < 0 || image_x >= img_width || image_y < 0 || image_y >= img_height){
            colour_id_mapping[i] = (DEFAULT_COLOUR_FOR_UNASSIGNED_ID);
        }else{
            sf::Vector2u image_coords({image_x, image_y});
            colour_id_mapping[i] = reference_image.getPixel(image_coords);
        }
    }
    for(unsigned int i=0; i<particle_count; i++){
        for(int j=0; j<6; j++){
             particle_vertices[6*i + j].color = colour_id_mapping[i];
        }
    }
}
void ParticleSystem::cacheColourIdMapping(){
    std::ofstream file(ids_colour_cache_path);
    /*
    THERE IS SOME BUG HERE.
    why is colour_id_mapping.size() different from particle_count? 
    The former is showing 0, wtf
    
    */
    std::cout << "Caching colour id mapping to path: " << ids_colour_cache_path << "\n";
    std::cout << "Number of colour ids being cached: " << colour_id_mapping.size() << "\n";
    if (!file.is_open())
        throw std::runtime_error("Could not open file for writing in cacheColourIdMapping");
    file << particle_count << " " << PARTICLE_RADIUS << "\n";
    // for(const sf::Color& colour : colour_id_mapping){
    //     file << (int)colour.r << " " << (int)colour.g << " " << (int)colour.b << "\n";
    // }
    for(unsigned int i=0; i<particle_count; i++){
        const sf::Color& colour = particle_vertices[6*i].color; // All 6 vertices of a particle have the same colour, so just take the first one
        file << (int)colour.r << " " << (int)colour.g << " " << (int)colour.b << "\n";
    }
}

