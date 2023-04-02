#pragma once

#include <SDL.h>
#include <vector>
#include <iostream>
#include <algorithm>

struct KeyState
{
    bool CurrentState;
    bool PreviousState;
    int PressedTimeInMilliseconds;
};

struct AxisState {
    bool CurrentState;
    float Value;
};

class Gamepad
{
public:
    static Gamepad* Open();
    ~Gamepad();

    void Update(SDL_Event&);
    bool IsKeyHeldDown(SDL_GameControllerButton);
    bool WasKeyClicked(SDL_GameControllerButton);
    bool IsAxisMotion(SDL_GameControllerAxis);
    float GetAxisValue(SDL_GameControllerAxis);

private:
    const int HOLD_THRESHOLD_MS = 300;
    const float DEADZONE = 0.1f;

    Gamepad(SDL_GameController*);
    int GetKeyHoldTime(SDL_GameControllerButton);

    SDL_GameController* gameController;
    std::vector<KeyState> keys;
    std::vector<AxisState> axes;
};
