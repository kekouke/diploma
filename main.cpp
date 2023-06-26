#include "application.h"

int main(int argc, char *argv[])
{
    Application app;

    // Описание приложения
    ipc::Core::Description description;
    description._title       = "Джойстик";
    description._version     = "1.1";
    description._description = "Модуль управления джойстиком";

    app.Initialize(argc, argv, description);
    app.Run();

    return 0;
}
