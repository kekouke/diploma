#pragma once                  // Только один раз подключаем заголовочник
#include "ipc.h"
#pragma pack(push,1)          // Выставляем однобайтовое выравнивание!!!

namespace Message {
    struct ButtonState {
        ipc::String<10> name;
        bool isPressed;
        ipc::Schema schema() {
            return ipc::Schema(this).title("Состояние кнопки")
                   .add(IPC_STRING(name).title("Имя кнопки").default_("Invalid name"))
                   .add(IPC_BOOL(isPressed).title("Состояние: ")
                                           .false_(ipc::Off, "Отпущена")
                                           .true_(ipc::On, "Нажата")
                                           .default_(false))
                   ;
        }
    };

    struct AxisState {
        ipc::String<10> name;
        double value;
        ipc::Schema schema() {
            return ipc::Schema(this).title("Состояние оси")
                   .add(IPC_STRING(name).title("Наименование оси").default_("Invalid name"))
                   .add(IPC_REAL(value).title("Значение").default_(0.0))
                   ;
        }
    };

    struct GamepadState {
        ButtonState buttonStates[21];
        AxisState axesState[4];
        ipc::Schema schema() {
            return ipc::Schema(this).title("Состояние геймпада")
                   .add(IPC_STRUCTS(buttonStates).title("Состояние кнопок")
                                                 .element_title("Состояние кнопки"))
                   .add(IPC_STRUCTS(axesState).title("Состояние осей")
                                                 .element_title("Состояние оси"))
                   ;
        }
    };

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

    struct GamepadMapping {
        ipc::String<10> start_control;
        ipc::String<10> stop_control;
        ipc::String<10> set_min_speed;
        ipc::String<10> set_slow_speed;
        ipc::String<10> set_max_speed;
        ipc::String<10> set_zero_speed;
        ipc::String<10> move_forward;
        ipc::String<10> move_right;
        ipc::Schema schema() {
            return ipc::Schema(this).title("Привязка")
                    .add(IPC_STRING(start_control).title("Включение управления"))
                    .add(IPC_STRING(stop_control).title("Отключение управления"))
                    .add(IPC_STRING(set_min_speed).title("Минимальная скорость"))
                    .add(IPC_STRING(set_slow_speed).title("Медленная скорость"))
                    .add(IPC_STRING(set_max_speed).title("Максимальная скорость"))
                    .add(IPC_STRING(set_zero_speed).title("Нулевая скорость"))
                    .add(IPC_STRING(move_forward).title("Движение прямо"))
                    .add(IPC_STRING(move_right).title("Движение вправо"))
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
}
#pragma pack(pop)
