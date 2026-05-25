

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
        va[6*i + 0] = {{x-r, y-r}, sf::Color::White, {0.f,  0.f }};
        va[6*i + 1] = {{x-r, y+r}, sf::Color::White, {0.f,  fth }};
        va[6*i + 2] = {{x+r, y-r}, sf::Color::White, {ftw,  0.f }};

        // Triangle 2
        va[6*i + 3] = {{x-r, y+r}, sf::Color::White, {0.f,  fth }};
        va[6*i + 4] = {{x+r, y+r}, sf::Color::White, {ftw,  fth }};
        va[6*i + 5] = {{x+r, y-r}, sf::Color::White, {ftw,  0.f }};
    }

    sf::RenderStates states;
    states.texture = &texture;
    window.draw(va, states);
    window.display();
}