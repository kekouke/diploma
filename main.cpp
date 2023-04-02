#include <iostream>

#include <SDL.h>
#undef main

int main() {
    if (SDL_Init(SDL_INIT_GAMECONTROLLER) != 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        exit(1);
    }
    std::cout << "OK" << std::endl;
    return 0;
}
