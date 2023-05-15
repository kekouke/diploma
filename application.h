#pragma once

#include <iostream>
#include <set>

#include "gamepad.h"
#include "messages.h"
#include "motion.h"

#include "SDL2/SDL.h"
#undef main

class Application
{
public:
    Application();
    ~Application();

    void Run();
    void Initialize(int argc, char *argv[], ipc::Core::Description description);
    void ToggleInputControl(bool value);
    bool IsControlEnable();

private:
    void OnJoystickConnected(int deviceIndex);
    void OnJoystickDisconnected();

    void PollEvents(SDL_Event& event);
    void ProcessCommands(ipc::Sender<motion::Control>*);

    void LoadGamepadMapping(ipc::Core*);

    Gamepad* gamepad;
    ipc::Core* core;
    ipc::Sender<motion::Control>* control;
    ipc::Sender<Message::State>* programStateLogger;
    ipc::Sender<Message::GamepadState>* gamepadStateLogger;

    double sendStateInterval;
    double sendDataInterval;

    bool isControlEnable = false;
    bool isGamepadAvailable = false;
};
