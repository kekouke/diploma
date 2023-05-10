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
    keys = std::vector<ButtonState>(SDL_CONTROLLER_BUTTON_MAX);
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
                buttonEventQueue.push_back({SDL_GameControllerButton(event.cbutton.button), true});
                break;
            }
            case SDL_CONTROLLERBUTTONUP: {
                buttonEventQueue.push_back({SDL_GameControllerButton(event.cbutton.button), false});
                break;
            }
        }
    }

    for (int i = 0; i < Gamepad::AxisCount; i++) {
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

const std::vector<ButtonState>& Gamepad::GetKeys()
{
    return keys;
}

const std::vector<double>& Gamepad::GetAxes()
{
    return axes;
}

bool Gamepad::WasKeyPressed(int i)
{
    return keys[i].CurrentState && !keys[i].PreviousState;
}

void Gamepad::ConsumeKey(int i)
{
    keys[i].PreviousState = keys[i].CurrentState;
}

void Gamepad::ProcessPendingKeyEvents(ipc::Sender<Message::GamepadState> &sender)
{
   std::vector<ButtonEvent> processedQueue;
   std::vector<bool> used(Gamepad::ButtonCount);
   for (const auto& key : buttonEventQueue) {
       if (used[key.Button]) {
           processedQueue.push_back(key);
           continue;
       }
       SetKeyState(key.Button, key.State);
       sender._.buttonStates[key.Button].isPressed = key.State;
       used[key.Button] = true;
   }
   buttonEventQueue = processedQueue;
}

void Gamepad::SetKeyState(int i, bool state)
{
    keys[i].PreviousState = keys[i].CurrentState;
    keys[i].CurrentState = state;
}
