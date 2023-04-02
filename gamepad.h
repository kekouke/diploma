#pragma once

#include <SDL.h>
#include <vector>
#include <iostream>

struct KeyState
{
    bool CurrentState;
    bool PreviousState;
    int PressedTimeInMilliseconds;
};

class Gamepad
{
public:
    static Gamepad* Open();
    ~Gamepad();

    void Update(SDL_Event&);
    bool IsKeyHeldDown(SDL_GameControllerButton);
    bool WasKeyClicked(SDL_GameControllerButton);

private:
    const int HOLD_THRESHOLD_MS = 300;

    Gamepad(SDL_GameController*);
    int GetKeyHoldTime(SDL_GameControllerButton);

    SDL_GameController* gameController;
    std::vector<KeyState> keys;
};
