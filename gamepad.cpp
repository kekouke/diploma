#include "gamepad.h"

bool Gamepad::Open(int deviceIndex)
{
    gameController = SDL_GameControllerOpen(deviceIndex);
    return gameController != nullptr;
}

void Gamepad::Close()
{
    if (gameController != nullptr) {
        SDL_GameControllerClose(gameController);
        gameController = nullptr;
    }
}

Gamepad::Gamepad()
{
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
    Close();
}

void Gamepad::UpdateAxes()
{
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

void Gamepad::SetButtonState(SDL_GameControllerButton button, bool value)
{
    buttonEventQueue.push_back({button, value});
}

bool Gamepad::WasKeyPressed(int i)
{
    return keys[i].CurrentState && !keys[i].PreviousState;
}

void Gamepad::ConsumeKey(int i)
{
    keys[i].PreviousState = keys[i].CurrentState;
}

void Gamepad::ProcessPendingKeyEvents(ipc::Sender<Message::GamepadState>* sender)
{
   std::vector<ButtonEvent> processedQueue;
   std::vector<bool> used(Gamepad::ButtonCount);
   for (const auto& key : buttonEventQueue) {
       if (used[key.Button]) {
           processedQueue.push_back(key);
           continue;
       }
       SetKeyState(key.Button, key.State);
       sender->_.buttonStates[key.Button].isPressed = key.State;
       used[key.Button] = true;
   }
   buttonEventQueue = processedQueue;
}

bool Gamepad::IsAtached()
{
    if (gameController == nullptr) return false;
    return SDL_GameControllerGetAttached(gameController);
}


void Gamepad::SetKeyState(int i, bool state)
{
    keys[i].PreviousState = keys[i].CurrentState;
    keys[i].CurrentState = state;
}
