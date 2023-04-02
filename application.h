#pragma once

#include <iostream>
#include <memory>

#include "gamepad.h"

#include <SDL.h>
#undef main

class Application
{
public:
    Application();
    ~Application();

    void Run();

private:
    Gamepad* gamepad;
};
