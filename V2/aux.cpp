#include "particle-system.hpp"

void ParticleSystem::setDt(float deltat){
    dt = deltat;
};
inline int ParticleSystem::flattenCoords(int i, int j){
    return i + j*grid_cols;
};

inline float ParticleSystem::sfVectorNormSq(const sf::Vector2f& vec){
    return (vec.x*vec.x + vec.y*vec.y);
};