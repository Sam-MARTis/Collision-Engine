#include "particle-system.hpp"

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