#include "application.h"
#include <vector>

#include <unordered_map>
#include <fstream>

#include "motion.h"

std::unordered_map<std::string, std::string> loadConfig(std::string fileName) {
    std::ifstream config(fileName);

    std::unordered_map<std::string, std::string> input_to_action;

    std::string action;
    while (std::getline(config, action, ':')) {
        std::string inputName;
        config >> inputName;
        input_to_action[inputName] = action;
        config.get();
    }

    return input_to_action;
}

void getGamepadBindings(
        std::unordered_map<std::string, std::string>& for_buttons,
        std::unordered_map<std::string, std::string>& for_axes
) {
    for_buttons = loadConfig("button_config.txt");
    for_axes = loadConfig("axes_config.txt");
}

void fillDefault(ipc::Sender<motion::Control>& control) {
    control._.parameters[geo::Right]  .value = 0;
    control._.parameters[geo::Right]  .type  = motion::ControlType::Force;
    control._.parameters[geo::Right]  .frame = scene::Absent;

    control._.parameters[geo::Forward].value = 0;
    control._.parameters[geo::Forward].type  = motion::ControlType::Force;
    control._.parameters[geo::Forward].frame = scene::Absent;

    control._.parameters[geo::Up]     .value = 0;
    control._.parameters[geo::Up]     .type  = motion::ControlType::Position;
    control._.parameters[geo::Up]     .frame = scene::Zero;

    control._.parameters[geo::Yaw]    .value = 0;
    control._.parameters[geo::Yaw]    .type  = motion::ControlType::Force;
    control._.parameters[geo::Yaw]    .frame = scene::Absent;

    control._.parameters[geo::Pitch]  .value = 0;
    control._.parameters[geo::Pitch]  .type  = motion::ControlType::Force;
    control._.parameters[geo::Pitch]  .frame = scene::Absent;

    control._.parameters[geo::Roll]   .value = 0;
    control._.parameters[geo::Roll]   .type  = motion::ControlType::Force;
    control._.parameters[geo::Roll]   .frame = scene::Absent;
}

void initializeSenderForControl(ipc::Sender<motion::Control>& control) {
    control._.is_compensation = false;
    control._.priority        = motion::Priority::Highest;
    control._.time_limit      = 2;
    fillDefault(control);
}

Application::Application()
{
    if (SDL_Init(SDL_INIT_GAMECONTROLLER) != 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        exit(1);
    }
    gamepad = Gamepad::Open();
    if (gamepad == nullptr) {
        std::cerr << "Failed to open game controller: " << SDL_GetError() << std::endl;
        exit(1);
    }
}

Application::~Application()
{
    delete gamepad;
    gamepad = nullptr;
}

void Application::Run(int argc, char *argv[])
{
    bool isControlAvailable = false;

    // Описание приложения
    ipc::Core::Description description;
    description._title       = "Тест";
    description._version     = "1.1";
    description._description = "Тестовый модуль";
    // Заводим ядро обмена сообщениями
    ipc::Core core(argc, argv, description);
    // Загружаем настройки модуля
    ipc::Loader<Message::Init>      init(core);

    std::unordered_map<std::string, std::string> button_to_action;
    std::unordered_map<std::string, std::string> axes_to_action;


    getGamepadBindings(button_to_action, axes_to_action);

    ipc::Sender<Message::State> programStateLogger(core);
    programStateLogger._.settings = init._;
    programStateLogger._.send_regime = true;

    ipc::Sender<Message::GamepadState> gamepadLogSender(core, ipc::RegisterEnable);

    for (int i = 0; i < Gamepad::ButtonCount; i++) {
        gamepadLogSender._.buttonStates[i].name = SDL_GameControllerGetStringForButton(SDL_GameControllerButton(i));
    }
    for (int i = 0; i < Gamepad::AxisCount; i++) {
        gamepadLogSender._.axesState[i].name = SDL_GameControllerGetStringForAxis(SDL_GameControllerAxis(i));
    }

    ipc::Sender<motion::Control>    control(core);
    initializeSenderForControl(control);

    // Регистрация таймера с ручным запуском
    ipc::Timer sendTimer(core);
    sendTimer.start(init._.send_timer);

    ipc::Timer tmr_state(core, init._.state_timer);

    SDL_Event event;
    while (core.receive()) {
        gamepad->Update(event);

        if (tmr_state.received()) {
            programStateLogger.send();
            // Только одно событие в итерации цикла!
            continue;
        }
        if (sendTimer.received()) {
            gamepad->ProcessPendingKeyEvents(gamepadLogSender);

            for (int i = 0; i < Gamepad::AxisCount; i++) {
                gamepadLogSender._.axesState[i].value = gamepad->GetValueForAxis(Axis(i));
            }

            gamepadLogSender.send();

            for (int i = 0; i < Gamepad::ButtonCount; i++) {
                if (gamepad->WasKeyPressed(i)) {
                    if (button_to_action.count(gamepadLogSender._.buttonStates[i].name)) {
                        std::string action = button_to_action[gamepadLogSender._.buttonStates[i].name];

                        if (action == "control_on") {
                            gamepad->ConsumeKey(i);
                            isControlAvailable = true;
                            std::cout << "control on" << std::endl;
                        }
                        if (action == "control_off") {
                            gamepad->ConsumeKey(i);
                            isControlAvailable = false;
                            std::cout << "control off" << std::endl;
                        }
                    }
                }
            }


            if (!isControlAvailable) continue;

            bool has_changes = false;

            for (int i = 0; i < Gamepad::AxisCount; i++) {
                if (!gamepad->HasValueForAxis(i)) {
                    continue;
                }

                double force_up         = 25;
                double force_pitch      = 7.5;

                control._.parameters[geo::Up]     .value = force_up;
                control._.parameters[geo::Up]     .type  = motion::ControlType::Force;
                control._.parameters[geo::Up]     .frame = scene::Absent;

                control._.parameters[geo::Pitch]  .value = force_pitch;
                control._.parameters[geo::Pitch]  .type  = motion::ControlType::Force;
                control._.parameters[geo::Pitch]  .frame = scene::Absent;

                std::string action = axes_to_action[gamepadLogSender._.axesState[i].name];
                if (action == "move_forward") {
                    double velocity_forvard = -gamepadLogSender._.axesState[i].value * 10;

                    control._.parameters[geo::Forward].value = velocity_forvard;
                    control._.parameters[geo::Forward].type  = motion::ControlType::Velocity;
                    control._.parameters[geo::Forward].frame = scene::Absent;

                    std::cout << "move_vertical" << std::endl;
                    has_changes = true;
                }
                if (action == "move_horizontal") {
                    double velocity_yaw     = gamepadLogSender._.axesState[i].value * 10;

                    control._.parameters[geo::Yaw]    .value = velocity_yaw;
                    control._.parameters[geo::Yaw]    .type  = motion::ControlType::Velocity;
                    control._.parameters[geo::Yaw]    .frame = scene::Absent;

                    std::cout << "move_horizontal" << std::endl;
                    has_changes = true;
                }
            }

            if (has_changes) {
                control.send();
                fillDefault(control);
            }
        }
    }
}
