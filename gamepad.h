#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <algorithm>

struct KeyInfo
{
    bool IsPressed;
    SDL_GameControllerButton Button;
};

// Adapted from SDL - see SDL_GameControllerAxis(https://wiki.libsdl.org/SDL2/SDL_GameControllerAxis)
enum class Axis
{
    LeftStickHorizontal,
    LeftStickVertical,
    RightStickHorizontal,
    RightStickVertical,
    Count
};

class Gamepad
{
public:
    static Gamepad* Open();
    ~Gamepad();

    void Update(SDL_Event& event);
    const std::vector<KeyInfo>& GetKeys();
    const std::vector<double>& GetAxes();
    std::vector<KeyInfo>& GetKeyEvents();
    double GetValueForAxis(Axis axis) {
        return axes[(int)axis];
    }

private:
    const int HOLD_THRESHOLD_MS = 300;
    const float DEADZONE = 0.2f;

    Gamepad(SDL_GameController*);

    void InitializeKeys();

    SDL_GameController* gameController;
    std::vector<KeyInfo> keys;
    std::vector<double> axes;
    std::vector<KeyInfo> keyInfoQueue;
};
