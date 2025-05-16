#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <cstdlib>

#define DEFAULT_PARTICLE_COUNT 1000

unsigned int particle_count;




int main(int argc, char** argv){
    if(argc==1){
        particle_count = DEFAULT_PARTICLE_COUNT;
    }
    else{ if(argc==2){
        particle_count = strtol(argv[2], NULL, 10);
    }
    else {
        printf("Unknown arguement count. Only one extra arguement expected");
        return 1;
    }
    }

    srand(40);
    return 0;


}