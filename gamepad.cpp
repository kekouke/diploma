#include "gamepad.h"

Gamepad* Gamepad::Open()
{
    SDL_GameController* controller;
    controller = SDL_GameControllerOpen(0);
    if (controller == nullptr) {
        return nullptr;
    }
    return new Gamepad(controller);
}

Gamepad::Gamepad(SDL_GameController* controller)
{
    gameController = controller;
    keys = std::vector<KeyState>(SDL_CONTROLLER_BUTTON_MAX);
}

Gamepad::~Gamepad()
{
    if (gameController != nullptr) {
        SDL_GameControllerClose(gameController);
        gameController = nullptr;
    }
}

void Gamepad::Update(SDL_Event& event)
{
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_CONTROLLERBUTTONDOWN: {
                int buttonIndex = event.cbutton.button;
                keys[buttonIndex].PressedTimeInMilliseconds = SDL_GetTicks();
                std::cout << "Button " << buttonIndex << " pressed" << std::endl;
                break;
            }
            case SDL_CONTROLLERBUTTONUP: {
                int buttonIndex = event.cbutton.button;
                std::cout << "Button " << buttonIndex << " released" << std::endl;
                break;
            }
        }
    }
    for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
        KeyState& key = keys[i];
        key.PreviousState = key.CurrentState;
        key.CurrentState = SDL_GameControllerGetButton(
                    gameController,
                    static_cast<SDL_GameControllerButton>(i)
        );
    }
}

bool Gamepad::IsKeyHeldDown(SDL_GameControllerButton key)
{
    return keys[key].CurrentState && GetKeyHoldTime(key) >= HOLD_THRESHOLD_MS;
}

bool Gamepad::WasKeyClicked(SDL_GameControllerButton key)
{
   return !keys[key].CurrentState &&
           keys[key].PreviousState &&
           GetKeyHoldTime(key) < HOLD_THRESHOLD_MS;
}

int Gamepad::GetKeyHoldTime(SDL_GameControllerButton key)
{
    return SDL_GetTicks() - keys[key].PressedTimeInMilliseconds;
}
