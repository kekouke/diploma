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
    keys = std::vector<KeyInfo>(SDL_CONTROLLER_BUTTON_MAX);
    axes = std::vector<double>(SDL_CONTROLLER_AXIS_MAX);

    InitializeKeys();
}

void Gamepad::InitializeKeys()
{
    int i = 0;
    for (auto& key : keys) {
        key.Button = SDL_GameControllerButton(i++);
    }
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
                keyInfoQueue.push_back(KeyInfo{true, SDL_GameControllerButton(event.cbutton.button)});
                int buttonIndex = event.cbutton.button;
                std::cout << "Button " << buttonIndex << " pressed" << std::endl;
                break;
            }
            case SDL_CONTROLLERBUTTONUP: {
                keyInfoQueue.push_back(KeyInfo{false, SDL_GameControllerButton(event.cbutton.button)});
                int buttonIndex = event.cbutton.button;
                std::cout << "Button " << buttonIndex << " released" << std::endl;
                break;
            }
        }
    }

    for (int i = 0; i < SDL_CONTROLLER_AXIS_MAX; i++) {
        axes[i] = std::clamp(
                    SDL_GameControllerGetAxis(
                       gameController,
                       static_cast<SDL_GameControllerAxis>(i)
                    ) / 32767.f,
                    -1.f,
                    1.f
        );
        axes[i] = abs(axes[i]) >= DEADZONE ? axes[i] : 0.0;
    }
}

const std::vector<KeyInfo>& Gamepad::GetKeys()
{
    return keys;
}

const std::vector<double>& Gamepad::GetAxes()
{
    return axes;
}

std::vector<KeyInfo> &Gamepad::GetKeyEvents()
{
    return keyInfoQueue;
}
