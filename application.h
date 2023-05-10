#pragma once

#include <iostream>

#include "gamepad.h"
#include "messages.h"

#include "SDL2/SDL.h"
#undef main

class Application
{
public:
    Application();
    ~Application();

    void Run(int argc, char *argv[]);

private:
    Gamepad* gamepad;
};
