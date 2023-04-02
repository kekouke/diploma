#include "application.h"

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

void Application::Run()
{
    SDL_Event event;
    while (true) {
        gamepad->Update(event);
        for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
            if (gamepad->WasKeyClicked(static_cast<SDL_GameControllerButton>(i))) {
                std::cout << "Button " << i << " clicked" << std::endl;
            }
            else if (gamepad->IsKeyHeldDown(static_cast<SDL_GameControllerButton>(i))) {
                 std::cout << "Button " << i << " hold" << std::endl;
            }
        }
    }
}
