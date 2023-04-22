#pragma once                  // Только один раз подключаем заголовочник
#include "ipc.h"
#pragma pack(push,1)          // Выставляем однобайтовое выравнивание!!!

namespace Message {
    struct ButtonState {
        ipc::String<20> name;
        bool isPressed;
        ipc::Schema schema() {
            return ipc::Schema(this).title("Состояние кнопки")
                   .add(IPC_STRING(name).title("Имя кнопки").default_("Undefined name"))
                   .add(IPC_BOOL(isPressed).title("Состояние: ")
                                           .false_(ipc::Off, "Отпущена")
                                           .true_(ipc::On, "Нажата")
                                           .default_(false))
                   ;
        }
    };

    struct AxisState {
        double x;
        double y;
        ipc::Schema schema() {
            return ipc::Schema(this).title("Состояние кнопки")
                   .add(IPC_REAL(x).title("X").default_(0.0))
                   .add(IPC_REAL(y).title("Y").default_(0.0))
                   ;
        }
    };

    struct GamepadState {
        ButtonState buttonStates[21];
        AxisState leftStick;
        AxisState rightStick;
        ipc::Schema schema() {
            return ipc::Schema(this).title("Состояние геймпада")
                   .add(IPC_STRUCTS(buttonStates).title("Состояние кнопок")
                                                 .element_title("Состояние кнопки"))
                   .add(IPC_STRUCT(leftStick).title("Левый стик"))
                   .add(IPC_STRUCT(rightStick).title("Правый стик"))
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
