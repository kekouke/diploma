#include "application.h"

int main(int argc, char *argv[])
{
    Application app;

    // Описание приложения
    ipc::Core::Description description;
    description._title       = "Тест";
    description._version     = "1.1";
    description._description = "Тестовый модуль";

    app.Initialize(argc, argv, description);
    app.Run();

    return 0;
}
