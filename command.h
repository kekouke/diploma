#pragma once
#include "gamepad.h"

#include <SDL2/SDL.h>

#include <functional>

struct KeyCommand {
    SDL_GameControllerButton mappedKey;
};

struct AxisCommand {
    SDL_GameControllerAxis mappedAxis;
};
