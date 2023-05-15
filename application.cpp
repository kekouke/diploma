#include "application.h"
#include <vector>

#include <unordered_map>
#include <fstream>

#include "motion.h"
#include "command.h"

KeyCommand start_control;
KeyCommand stop_control;
KeyCommand set_min_speed;
KeyCommand set_slow_speed;
KeyCommand set_max_speed;
KeyCommand set_zero_speed;
AxisCommand move_forward;
AxisCommand move_right;

void fillDefault(ipc::Sender<motion::Control>* control) {
    control->_.parameters[geo::Right]  .value = 0;
    control->_.parameters[geo::Right]  .type  = motion::ControlType::Force;
    control->_.parameters[geo::Right]  .frame = scene::Absent;

    control->_.parameters[geo::Forward].value = 0;
    control->_.parameters[geo::Forward].type  = motion::ControlType::Force;
    control->_.parameters[geo::Forward].frame = scene::Absent;

    control->_.parameters[geo::Up]     .value = 0;
    control->_.parameters[geo::Up]     .type  = motion::ControlType::Position;
    control->_.parameters[geo::Up]     .frame = scene::Zero;

    control->_.parameters[geo::Yaw]    .value = 0;
    control->_.parameters[geo::Yaw]    .type  = motion::ControlType::Force;
    control->_.parameters[geo::Yaw]    .frame = scene::Absent;

    control->_.parameters[geo::Pitch]  .value = 0;
    control->_.parameters[geo::Pitch]  .type  = motion::ControlType::Force;
    control->_.parameters[geo::Pitch]  .frame = scene::Absent;

    control->_.parameters[geo::Roll]   .value = 0;
    control->_.parameters[geo::Roll]   .type  = motion::ControlType::Force;
    control->_.parameters[geo::Roll]   .frame = scene::Absent;
}

void initializeSenderForControl(ipc::Sender<motion::Control>* control) {
    control->_.is_compensation = false;
    control->_.priority = motion::Priority::Highest;
    control->_.time_limit = 2;
    fillDefault(control);
}

void Application::Initialize(int argc, char *argv[], ipc::Core::Description description)
{
    core = new ipc::Core(argc, argv, description);
    gamepad = new Gamepad();

    // Загружаем настройки
    ipc::Loader<Message::Init> init(*core);
    sendStateInterval = init._.state_timer;
    sendDataInterval = init._.send_timer;

    LoadGamepadMapping(core);

    programStateLogger = new ipc::Sender<Message::State>(*core);
    programStateLogger->_.settings = init._;
    programStateLogger->_.send_regime = true;

    gamepadStateLogger = new ipc::Sender<Message::GamepadState>(*core, ipc::RegisterEnable);
    for (int i = 0; i < Gamepad::ButtonCount; i++) {
        gamepadStateLogger->_.buttonStates[i].name =
                SDL_GameControllerGetStringForButton(SDL_GameControllerButton(i));
    }
    for (int i = 0; i < Gamepad::AxisCount; i++) {
        gamepadStateLogger->_.axesState[i].name =
                SDL_GameControllerGetStringForAxis(SDL_GameControllerAxis(i));
    }

    control = new ipc::Sender<motion::Control>(*core);
    initializeSenderForControl(control);
}

void Application::LoadGamepadMapping(ipc::Core* core) {
    ipc::Loader<Message::GamepadMapping> gamepadMapping(*core);

    start_control.mappedKey = SDL_GameControllerGetButtonFromString(
                gamepadMapping._.start_control.to_std_string().c_str()
    );
    stop_control.mappedKey = SDL_GameControllerGetButtonFromString(
                gamepadMapping._.stop_control.to_std_string().c_str()
    );
    set_min_speed.mappedKey = SDL_GameControllerGetButtonFromString(
                gamepadMapping._.set_min_speed.to_std_string().c_str()
    );
    set_slow_speed.mappedKey = SDL_GameControllerGetButtonFromString(
                gamepadMapping._.set_slow_speed.to_std_string().c_str()
    );
    set_max_speed.mappedKey = SDL_GameControllerGetButtonFromString(
                gamepadMapping._.set_max_speed.to_std_string().c_str()
    );
    set_zero_speed.mappedKey = SDL_GameControllerGetButtonFromString(
                gamepadMapping._.set_zero_speed.to_std_string().c_str()
    );


    move_forward.mappedAxis = SDL_GameControllerGetAxisFromString(
                gamepadMapping._.move_forward.to_std_string().c_str()
    );
    move_right.mappedAxis = SDL_GameControllerGetAxisFromString(
                gamepadMapping._.move_right.to_std_string().c_str()
    );
}

Application::Application()
{
    if (SDL_Init(SDL_INIT_GAMECONTROLLER) != 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        exit(1);
    }
}

Application::~Application()
{
    delete core;
    delete control;
    delete gamepad;
    delete gamepadStateLogger;
    delete programStateLogger;
    core = nullptr;
    control = nullptr;
    gamepad = nullptr;
    gamepadStateLogger = nullptr;
    programStateLogger = nullptr;
}

void Application::Run()
{
    ipc::Timer sendTimer(*core);
    ipc::Timer tmr_state(*core);
    sendTimer.start(sendDataInterval);
    tmr_state.start(sendStateInterval);

    SDL_Event event;
    while (core->receive()) {
        if (tmr_state.received()) {
            programStateLogger->send();
            // Только одно событие в итерации цикла!
            continue;
        }

        PollEvents(event);

        if (!isGamepadAvailable) {
            continue;
        }

        if (sendTimer.received()) {
            gamepad->ProcessPendingKeyEvents(gamepadStateLogger);
            for (int i = 0; i < Gamepad::AxisCount; i++) {
                gamepadStateLogger->_.axesState[i].value = gamepad->GetValueForAxis(Axis(i));
            }
            gamepadStateLogger->send();
            ProcessCommands(control);
            continue;
        }
    }
}

void Application::OnJoystickConnected(int deviceIndex)
{
    if (!SDL_IsGameController(deviceIndex)) {
        return;
    }
    if (!isGamepadAvailable) {
        gamepad->Open(deviceIndex);
        isGamepadAvailable = true;
        ToggleInputControl(false);
        std::cout << "Gamepad connected." << std::endl;
    }
}

void Application::OnJoystickDisconnected()
{
    if (!gamepad->IsAtached()) {
        std::cout << "Gamepad disconnected." << std::endl;
        gamepad->Close();
        isGamepadAvailable = false;
        for (int i = 0; i < SDL_NumJoysticks(); i++) {
            if (!SDL_IsGameController(i)) {
                continue;
            }
            if (gamepad->Open(i)) {
                isGamepadAvailable = true;
                ToggleInputControl(false);
                return;
            }
        }
    }
}

void Application::PollEvents(SDL_Event &event)
{
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_CONTROLLERDEVICEADDED:
                OnJoystickConnected(event.cdevice.which);
                break;
            case SDL_CONTROLLERDEVICEREMOVED:
                OnJoystickDisconnected();
                break;
            case SDL_CONTROLLERBUTTONDOWN:
                gamepad->SetButtonState(SDL_GameControllerButton(event.cbutton.button), true);
                break;
            case SDL_CONTROLLERBUTTONUP:
                gamepad->SetButtonState(SDL_GameControllerButton(event.cbutton.button), false);
                break;
        }
    }
    gamepad->UpdateAxes(); // TODO: Use case SDL_CONTROLLERAXISMOTION
}

void Application::ProcessCommands(ipc::Sender<motion::Control>* control)
{
    if (gamepad->WasKeyPressed(start_control.mappedKey)) {
        gamepad->ConsumeKey(start_control.mappedKey);
        ToggleInputControl(true);
        std::cout << "control on" << std::endl;
    }
    if (gamepad->WasKeyPressed(stop_control.mappedKey)) {
        gamepad->ConsumeKey(stop_control.mappedKey);
        ToggleInputControl(false);
        std::cout << "control off" << std::endl;
        return;
    }

    if (!IsControlEnable()) return;

    double speed_coeff = 1;

    if (gamepad->WasKeyPressed(set_min_speed.mappedKey)) {
        speed_coeff = 0.2;
    }
    if (gamepad->WasKeyPressed(set_slow_speed.mappedKey)) {
        speed_coeff = 0.5;
    }
    if (gamepad->WasKeyPressed(set_max_speed.mappedKey)) {
        speed_coeff = 2;
    }

    double force_up = 25;
    double force_pitch = 7.5;
    bool has_changes = false;

    control->_.parameters[geo::Up].value = force_up;
    control->_.parameters[geo::Up].type  = motion::ControlType::Force;
    control->_.parameters[geo::Up].frame = scene::Absent;

    control->_.parameters[geo::Pitch].value = force_pitch;
    control->_.parameters[geo::Pitch].type  = motion::ControlType::Force;
    control->_.parameters[geo::Pitch].frame = scene::Absent;

    if (gamepad->HasValueForAxis(move_forward.mappedAxis)) {
        double velocity_forward = -gamepad->GetValueForAxis((Axis)move_forward.mappedAxis) * 5 * speed_coeff;

        control->_.parameters[geo::Forward].value = velocity_forward;
        control->_.parameters[geo::Forward].type  = motion::ControlType::Velocity;
        control->_.parameters[geo::Forward].frame = scene::Absent;

        std::cout << "move_vertical" << std::endl;
        has_changes = true;
    }
    if (gamepad->HasValueForAxis(move_right.mappedAxis)) {
        double velocity_yaw = gamepad->GetValueForAxis((Axis)move_right.mappedAxis) * 5 * speed_coeff;

        control->_.parameters[geo::Yaw].value = velocity_yaw;
        control->_.parameters[geo::Yaw].type  = motion::ControlType::Velocity;
        control->_.parameters[geo::Yaw].frame = scene::Absent;

        std::cout << "move_horizontal" << std::endl;
        has_changes = true;
    }
    if (gamepad->WasKeyPressed(set_zero_speed.mappedKey)) {
        has_changes = true;
        fillDefault(control);
    }

    if (has_changes) {
        control->send();
        fillDefault(control);
    }
}

void Application::ToggleInputControl(bool value)
{
    isControlEnable = value;
}

bool Application::IsControlEnable()
{
    return isControlEnable;
}
