#include "application.h"
#include "messages.h"       // Подключаем сообщения (и библиотеку IPC заодно)
#include <vector>

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
    // Описание приложения
    ipc::Core::Description description;
    description._title       = "Тест";
    description._version     = "1.1";
    description._description = "Тестовый модуль";
    // Заводим ядро обмена сообщениями
    ipc::Core core(argc, argv, description);
    // Загружаем настройки модуля
    ipc::Loader<Message::Init>      init(core);
    // Получаем режим работы
    // В режиме sender будем отправлять данные по таймеру, в режиме receiver - получать
    bool isSender = false;
    for (int val = 1; val < argc; ++val) {
        if (std::string(argv[val]) == "--sender") {
            isSender = true;
            break;
        }
    }

    ipc::Sender<Message::State> pub_state(core);
    pub_state._.settings = init._;
    pub_state._.send_regime = isSender;

    ipc::Sender<Message::GamepadState> sender(core, isSender ? ipc::RegisterEnable: ipc::RegisterDisable);
    ipc::Receiver<Message::GamepadState> receiver(core, isSender ? ipc::RegisterDisable : ipc::RegisterEnable);

    int i = 0;
    for (const auto& key : gamepad->GetKeys()) {
        sender._.buttonStates[i++].name = SDL_GameControllerGetStringForButton(key.Button);
    }

    // Регистрация таймера с ручным запуском
    ipc::Timer tmr_send_read(core);
    double interval = isSender ? init._.send_timer : init._.read_timer;
    tmr_send_read.start(interval);

    ipc::Timer tmr_state(core, init._.state_timer);

    SDL_Event event;
    while (core.receive()) {
        if (isSender) {
            gamepad->Update(event);
        }
        if (tmr_state.received()) {
            pub_state.send();
            // Только одно событие в итерации цикла!
            continue;
        }
        if (tmr_send_read.received()) {
            if (isSender) {
                ProcessPendingKeyEvents(sender);
                sender._.leftStick.x = gamepad->GetValueForAxis(Axis::LeftStickHorizontal);
                sender._.leftStick.y = gamepad->GetValueForAxis(Axis::LeftStickVertical);
                sender._.rightStick.x = gamepad->GetValueForAxis(Axis::RightStickHorizontal);
                sender._.rightStick.y = gamepad->GetValueForAxis(Axis::RightStickVertical);
                sender.send();
            }
            continue;
        }
        if (receiver.received()) {
            for (int i = 0; i < 1; i++) {
                core.log(
                     ipc::Ok,
                     "(RECEIVE) Кнопка '%s': %s",
                     receiver._.buttonStates[i].name.to_std_string().c_str(),
                     receiver._.buttonStates[i].isPressed ? "нажата" : "отпущена");
            }
            core.log(ipc::Ok, "(RECEIVE) Left stick: X = %lf Y = %lf", receiver._.leftStick.x, receiver._.leftStick.y);
            core.log(ipc::Ok, "(RECEIVE) Right stick: X = %lf Y = %lf", receiver._.rightStick.x, receiver._.rightStick.y);
        }
    }
}

void Application::ProcessPendingKeyEvents(ipc::Sender<Message::GamepadState>& sender)
{
    std::vector<KeyInfo> processedQueue;
    std::vector<bool> used(21);
    for (const auto& key : gamepad->GetKeyEvents()) {
        if (used[key.Button]) {
            processedQueue.push_back(key);
            continue;
        }
        sender._.buttonStates[key.Button].isPressed = key.IsPressed;
        used[key.Button] = true;
    }
    gamepad->GetKeyEvents() = processedQueue;
}
