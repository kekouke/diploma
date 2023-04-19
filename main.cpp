#include <cstdlib>          // rand()
#include "messages.h"       // Подключаем свои сообщения (и библиотеку IPC заодно)
#include "application.h"


int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++) {
        std::string args(argv[1]);
        if (args == "--controller") {
            std::cout << "SDL APPLICATION" << std::endl;
            Application app;
            app.Run();
            return 0;
        }
    }

    // Описание приложения
    ipc::Core::Description description;
    description._title       = "Тест";
    description._version     = "1.1";
    description._description = "Тестовый модуль";
    // Заводим ядро обмена сообщениями
    ipc::Core core(argc, argv, description);
    // Загружаем настройки модуля
    ipc::Loader<test::Init>      init(core);
    // Получаем режим работы
    // В режиме sender будем отправлять данные по таймеру, в режиме receiver - получать
    bool sender = true;
    for (int val = 1; val < argc; ++val) {
        if (std::string(argv[val]) == "--receiver") {
            sender = false;
            break;
        }
    }
    // Инициализируем псевдо-случайную последовательность
    srand(ipc::DateTime().Seconds());
    // Регистрируем публикацию состояния для всех режимов работы
    ipc::Sender<test::State> pub_state(core);
    // Регистрируем прием сообщения по событию для режима receiver
    ipc::Receiver<test::OutDataSimple> rec_data_simple(core,sender?ipc::RegisterDisable:
                                                                   ipc::RegisterEnable);
    // Регистрируем прием сообщения в буфер для режима receiver
    ipc::Reader<test::OutDataFull> read_data_full(core,sender?ipc::RegisterDisable:
                                                              ipc::RegisterEnable);
    // Регистрируем отправку сообщений для режима sender
    ipc::Sender<test::OutDataSimple> pub_data_simple(core,sender?ipc::RegisterEnable:
                                                                 ipc::RegisterDisable);
    ipc::Sender<test::OutDataFull> pub_data_full(core,sender?ipc::RegisterEnable:
                                                                 ipc::RegisterDisable);

    // Внесем настройки в публикуемое состояние (они меняться не будут)
    pub_state._.settings = init._;
    pub_state._.send_regime = sender;
    // Зарегистрируем таймеры
    // Регистрация таймера с автоматическим запуском
    ipc::Timer tmr_state(core,init._.state_timer);
    // Регистрация таймера с ручным запуском
    ipc::Timer tmr_send_read(core);
    if (sender)
        tmr_send_read.start(init._.send_timer);
    else
        tmr_send_read.start(init._.read_timer);
    // Переход в цикл обработки и генерации событий
    while (core.receive()) {
        // Получили событие (срабатывание) таймера публикации состояния
        if (tmr_state.received()) {
            pub_state.send();
            // Только одно событие в итерации цикла!
            continue;
        }
        // Получили событие (срабатывание) таймера чтения\публикации данных
        if (tmr_send_read.received()) {
            if (sender) {
                // В режиме сендера - отправляем данные
                // В текст запишем текущую дату
                pub_data_simple._.text = ipc::DateTime().to_string();
                pub_data_simple._.random = rand()/100.0;
                pub_data_simple._.counter++;
                pub_data_simple._.toggle = !pub_data_simple._.toggle;
                // Отправляем данные (простые)
                pub_data_simple.send();
                // Заполним массив со сложными данными
                pub_data_full._.text = ipc::DateTime().to_string();
                pub_data_full._.random = rand()/100.0;
                pub_data_full._.counter++;
                pub_data_full._.toggle = !pub_data_full._.toggle;
                // Отправляем данные (сложные)
                pub_data_full.send();
            } else {
                // В режиме ресивера - по таймеру читаем сообщения из буфера
                // и выведем в лог (ipc)
                read_data_full.read(); // Обновляем данные в переменной последними данными
                core.log(ipc::Ok,"(READ) Текст: %s; Ранд: %f; Счетчик: %d;",
                         read_data_full._.text.to_std_string().c_str(),
                         read_data_full._.random, read_data_full._.counter);
            }
            continue;
        }
        // Получили событие прихода конкретного сообщения (ipc::Receiver)
        if (rec_data_simple.received()) {
            // Данные в структуре обновляются при вызове received() автоматически
            core.log(ipc::Ok,"(RECEIVE) Текст: %s; Счетчик: %d; Ключ: %s",
                     rec_data_simple._.text.to_std_string().c_str(),
                     rec_data_simple._.counter, rec_data_simple._.toggle?"true":"false");
            continue;
        }
    }
    // Здесь мы окажемся, когда будет отправлен сигнал KILL программе
    // Например, по CTRL+C в консоли
    return 0;
}
