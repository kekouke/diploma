#pragma once

#include <iostream>
#include <set>

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
    void ToggleInputControl(bool value);
    bool IsControlEnable();

private:
    void OnJoystickConnected(int deviceIndex);
    void OnJoystickDisconnected();

    Gamepad* gamepad;
    bool isControlEnable = false;
    bool isGamepadAvailable = false;
};
