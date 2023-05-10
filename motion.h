/** @file
  Интерфейс сообщений программы регулирования движения подводного аппарата.
**/

#pragma once             /// Один раз подключаем файл.
#include "ipc.h"
#include "common/geo.h"  /// Схемы описания гео-координат.
#include "core/scene.h"  /// Описание объектов сцены.
#pragma pack(push,1)     /// Однобайтовое выравнивание для кроссплатформенной работы.

namespace motion {


// СЛУЖЕБНЫЕ СТРУКТУРЫ //


/// Приоритеты команд управления.
namespace Priority {
enum Enum {
    NoControl   =  -1,  ///< Нет управления
    Lowest      =   0,  ///< Наинизший приоритет управления (поддержание)
    Mission     =   1,  ///< Приоритет миссии
    Submission  =   2,  ///< Приоритет подмиссии
    Telecommand =  10,  ///< Приоритет телекоманд
    Alarm       =  20,  ///< Приоритет КАС
    Highest     =  30   ///< Наивысший приоритет
};
inline ipc::Schema::Enum schema(ipc::Schema::Enum e)
{
    return e.title("Приоритет управления")
        .add(IPC_ENUM_VALUE(NoControl),   ipc::Off, "Нет управления")
        .add(IPC_ENUM_VALUE(Lowest),      ipc::On,  "Низший")
        .add(IPC_ENUM_VALUE(Mission),     ipc::On,  "Миссия")
        .add(IPC_ENUM_VALUE(Submission),  ipc::On,  "Подмиссия")
        .add(IPC_ENUM_VALUE(Telecommand), ipc::On,  "Телеуправление")
        .add(IPC_ENUM_VALUE(Alarm),       ipc::On,  "КАС")
        .add(IPC_ENUM_VALUE(Highest),     ipc::On,  "Наивысший")
    ;
}
}


/// Типы управления степенями свобод
namespace ControlType {
enum Enum {
     None     = -1,    ///< Нет управления
     Force    =  0,    ///< Управление по упору
     Velocity =  1,    ///< Управление по скорости
     Position =  2,    ///< Управление по координате
     Num
};
inline ipc::Schema::Enum schema(ipc::Schema::Enum e)
{
    return e.add(None,     ipc::Off, "Нет управления")
            .add(Force,    ipc::On,  "Упор|момент, H[*м]")
            .add(Velocity, ipc::On,  "Скорость, м|°/c")
            .add(Position, ipc::On,  "Координата, м|°");
}
}


/// Команда управления выделенной осью
struct ControlParameter {
    double            value; ///< Целевая величина управления
    ControlType::Enum type;  ///< Тип управления
    scene::Object     frame; ///< Система координат, в которой происходит движение

    ipc::Schema schema(){return ipc::Schema(this).title("Параметр движения")
        .add(IPC_REAL(value).title("Целевая величина").default_(0.0))
        .add(ControlType::schema(IPC_ENUM(type)).title("Тип управления").default_(ControlType::None))
        .add(scene::object_schema(IPC_ENUM(frame)).title("Система координат").default_(scene::Absent))
    ;}
};


// СООБЩЕНИЯ //


/// Низкоуровневая команда управления аппаратом
/// Мы исходим из предположения что все оси аппарата управляются независимо
struct Control {
    double           time_limit;            ///< Лимит времени на отработку целевых данных
    Priority::Enum   priority;              ///< Приоритет команды управления
    bool             is_compensation;       ///< Включены ли компенсирующие силы
    ControlParameter parameters[geo::Num];  ///< Парамеры управления для всех степеней свобод

    ipc::Schema schema() {return ipc::Schema(this).title("Команда движения").no_age()
        .add(IPC_REAL(time_limit).title("Лимит времени").maximum(10.0))
        .add(Priority::schema(IPC_ENUM(priority)).title("Приоритет").default_(ControlType::None))
        .add(IPC_BOOL(is_compensation).title("Компенсирующие силы").false_(ipc::Ok, "Выключены")
                                                                    .true_(ipc::Ok, "Включены").default_(true))
        .add(IPC_ARRAY(parameters).title("Параметры управления")
            .add(IPC_STRUCT(parameters[geo::Right]  ).title("На восток"    ))
            .add(IPC_STRUCT(parameters[geo::Forward]).title("На север"     ))
            .add(IPC_STRUCT(parameters[geo::Up]     ).title("Вверх"        ))
            .add(IPC_STRUCT(parameters[geo::Yaw]    ).title("По курсу"     ))
            .add(IPC_STRUCT(parameters[geo::Pitch]  ).title("По дифференту"))
            .add(IPC_STRUCT(parameters[geo::Roll]   ).title("По крену"     ))
        )
    ;}
};


/// Сигнализация о процессе выполнения команды управления.
struct Work
{
    Priority::Enum  priority;
    ipc::Schema schema() {return ipc::Schema(this).title("Состояние управления")
        .add(Priority::schema(IPC_ENUM(priority)).title("Статус").default_(Priority::NoControl))
    ;}
};

/// Сформированные силы для релазиации заданных целевых
struct Forces
{
    double  limit  [geo::Num];
    double  unlimit[geo::Num];

    ipc::Schema schema() {return ipc::Schema(this)
        .title("Управление ДРК").age(1.0).decimation_off()
        .add(IPC_ARRAY(limit).title("Силы с ограничениями")
            .add(IPC_REAL(limit[geo::Right])  .title("Упор вправо")   .unit("Н")  .minimum(-1000).maximum(1000))
            .add(IPC_REAL(limit[geo::Forward]).title("Упор вперед")   .unit("Н")  .minimum(-1000).maximum(1000))
            .add(IPC_REAL(limit[geo::Up])     .title("Упор вверх")    .unit("Н")  .minimum(-1000).maximum(1000))
            .add(IPC_REAL(limit[geo::Yaw])    .title("Момент курса")  .unit("Н*м").minimum(-1000).maximum(1000))
            .add(IPC_REAL(limit[geo::Pitch])  .title("Момент диффер.").unit("Н*м").minimum(-1000).maximum(1000))
            .add(IPC_REAL(limit[geo::Roll])   .title("Момент крена")  .unit("Н*м").minimum(-1000).maximum(1000))
        )
        .add(IPC_ARRAY(unlimit).title("Силы без ограничений")
            .add(IPC_REAL(unlimit[geo::Right])  .title("Упор вправо")   .unit("Н")  )
            .add(IPC_REAL(unlimit[geo::Forward]).title("Упор вперед")   .unit("Н")  )
            .add(IPC_REAL(unlimit[geo::Up])     .title("Упор вверх")    .unit("Н")  )
            .add(IPC_REAL(unlimit[geo::Yaw])    .title("Момент курса")  .unit("Н*м"))
            .add(IPC_REAL(unlimit[geo::Pitch])  .title("Момент диффер.").unit("Н*м"))
            .add(IPC_REAL(unlimit[geo::Roll])   .title("Момент крена")  .unit("Н*м"))
        )
    ;}
};

}
#pragma pack(pop)
