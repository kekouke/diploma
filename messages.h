#pragma once                  // Только один раз подключаем заголовочник
#include "ipc.h"
#pragma pack(push,1)          // Выставляем однобайтовое выравнивание!!!

// Пространство имен сообщений
namespace test {
    // Настройки из конфиг-файла  //
    struct Init {
        double  state_timer;
        double  read_timer;
        double  send_timer;
        ipc::Schema schema() {
            return ipc::Schema(this).title("Настройки")
                    .add(IPC_INT(state_timer).title("Период состояния").unit("c").default_(1.0))
                    .add(IPC_INT(read_timer).title("Период чтения данных").unit("c").default_(1.0))
                    .add(IPC_INT(send_timer).title("Период посылки данных").unit("c").default_(0.5))
                    ;
        }
    };

    // Состояние программы //
    struct State {
        bool send_regime;
        Init settings;
        ipc::Schema schema() {
            return ipc::Schema(this).title("Состояние")
                .add(IPC_BOOL(send_regime).title("Режим работы")
                                     .false_(ipc::Ok, "Получатель").true_(ipc::On, "Отправитель"))
                .add(IPC_STRUCT(settings) .title("Настройки"))
                ;
        }
    };

    // Простое сообщение   //
    struct OutDataSimple {
        bool            toggle;
        int             counter;
        double          random;
        ipc::String<80> text;

        ipc::Schema schema() {
            return ipc::Schema(this).title("Простое сообщение")
                    // Описание атомов
                    .add(IPC_INT(counter).title("Счетчик посылок")
                                         .unit("ед").default_(0).range(0,100))
                    .add(IPC_REAL(random).title("Случайное значение")
                                         .unit("ед").default_(0.0).range(0.0,100.0))
                    .add(IPC_BOOL(toggle).title("Переключалка")
                                         .false_(ipc::Off, "Выкл").true_(ipc::On, "Вкл").default_(true))
                    .add(IPC_STRING(text).title("Текстовое поле")
                                         .default_("No title"))
                    ;
        }
    };

    // Сложное сообщение (все поддерживаемые типы данных) //

    // Служебные структуры //
    namespace Mode {
        enum Enum {
            Service,
            Normal,
            SwitchOn,
            SwitchOff,
            Advanced,
            Critical,
            OutOfRange,
            Num
        };
        inline ipc::Schema::Enum schema(ipc::Schema::Enum n)
        {
            return n.add(IPC_ENUM_VALUE(Service),    ipc::Debug,   "Сервисный режим")
                    .add(IPC_ENUM_VALUE(Normal),     ipc::Ok,      "Параметры в норме")
                    .add(IPC_ENUM_VALUE(SwitchOn),   ipc::On,      "Вкл")
                    .add(IPC_ENUM_VALUE(SwitchOff),  ipc::Off,     "Выкл")
                    .add(IPC_ENUM_VALUE(Advanced),   ipc::Ok,      "Расширенный режим")
                    .add(IPC_ENUM_VALUE(Critical),   ipc::Warning, "Критический режим")
                    .add(IPC_ENUM_VALUE(OutOfRange), ipc::Error,   "Переполнение диапазона");
        }
    }

    struct Raw {
        int32_t  i;
        enum     {ArrayNum = 10};
        double   array[ArrayNum];

        ipc::Schema schema(){return ipc::Schema(this).title("Сырые данные")
                    .add(IPC_INT(i).title("Интовое число").unit("ед").default_(0).minimum(0).maximum(100))
                    .add(IPC_ARRAY(array).title("Массив даблов")
                        .add(IPC_REAL(array[0]).title("Ячейка №0").unit("ед").default_(0)
                                               .minimum(0).maximum(100).precision(2))
                        .add(IPC_REAL(array[1]).title("Ячейка №1").unit("ед").default_(1)
                                               .range(0,100).precision(2))
                        .add(IPC_REAL(array[2]).title("Ячейка №2").unit("ед").default_(2)
                                               .range(0,100).precision(2))
                        .add(IPC_REAL(array[3]).title("Ячейка №3").unit("ед").default_(3)
                                               .range(0,100).precision(2))
                        .add(IPC_REAL(array[4]).title("Ячейка №4").unit("ед").default_(4)
                                               .range(0,100).precision(2))
                        .add(IPC_REAL(array[5]).title("Ячейка №5").unit("ед").default_(5)
                                               .range(0,100).precision(2))
                        .add(IPC_REAL(array[6]).title("Ячейка №6").unit("ед").default_(6)
                                               .range(0,100).precision(2))
                        .add(IPC_REAL(array[7]).title("Ячейка №7").unit("ед").default_(7)
                                               .range(0,100).precision(2))
                        .add(IPC_REAL(array[8]).title("Ячейка №8").unit("ед").default_(8)
                                               .range(0,100).precision(2))
                        .add(IPC_REAL(array[9]).title("Ячейка №9").unit("ед").default_(9)
                                               .range(0,100).precision(2))
                    )
                    ;}
    };

    // Cообщение //
    struct OutDataFull {
        bool            toggle;
        int             counter;
        double          random;
        ipc::String<80> text;
        Raw             raw;
        Mode::Enum      mode;

        bool            array_bool  [Mode::Num];
        int32_t         array_int   [Mode::Num];
        double          array_double[Mode::Num];
        ipc::String<80> array_string[Mode::Num];
        Raw             array_struct[Mode::Num];
        Mode::Enum      array_enum  [Mode::Num];

        ipc::Schema schema() {
            return ipc::Schema(this).title("Полное сообщение")

                    // Описание атомов
                    .add(IPC_INT(counter).title("Счетчик посылок")   .unit("ед")
                                         .default_(0  ).minimum(0  ).maximum(100  ))
                    .add(IPC_REAL(random).title("Случайное значение").unit("ед")
                                         .default_(0.0).minimum(0.0).maximum(100.0))
                    .add(IPC_BOOL(toggle).title("Переключалка")
                                         .false_(ipc::Off, "Выкл").true_(ipc::On, "Вкл")
                                         .default_(true))
                    .add(IPC_STRING(text).title("Текстовое поле").default_("No title"))
                    .add(IPC_STRUCT(raw) .title("Сырые данные"))
                    // Описание enum-а
                    .add(Mode::schema(IPC_ENUM(mode)).title("Режим"))
                    // Описание массивов
                    .add(IPC_BOOLS(array_bool)    .title("Булы")
                                                  .element_title("Бул")   .element_default(true))
                    .add(IPC_INTS(array_int)      .title("Инты")
                                                  .element_title("Инт")   .element_unit("ед.изм."))
                    .add(IPC_REALS(array_double)  .title("Даблы")
                                                  .element_title("Дабл")  .element_unit("ед.изм."))
                    .add(IPC_STRINGS(array_string).title("Строки")
                                                  .element_title("Строка").element_default("Текст"))
                    .add(IPC_STRUCTS(array_struct).title("Структуры")
                                                  .element_titles(Mode::schema(IPC_ENUM(array_struct))))
                    .add(IPC_ENUMS(array_enum)    .title("Енамы")    .element_title("Енам")
                             .element_add(Mode::Service,    ipc::Debug,   "Сервисный режим")
                             .element_add(Mode::Normal,     ipc::Ok,      "Параметры в норме")
                             .element_add(Mode::SwitchOn,   ipc::On,      "Вкл")
                             .element_add(Mode::SwitchOff,  ipc::Off,     "Выкл")
                             .element_add(Mode::Advanced,   ipc::Ok,      "Расширенный режим")
                             .element_add(Mode::Critical,   ipc::Warning, "Критический режим")
                             .element_add(Mode::OutOfRange, ipc::Error,   "Переполнение диапазона")
                         )
                    ;
        }
    };
}
#pragma pack(pop)
