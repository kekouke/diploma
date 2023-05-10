#pragma once                    // Только один раз подключаем заголовочник
#include "common/geo.h"         // Общепринятые geo-сообщения
#pragma pack(push,1)            // Выставляем однобайтовое выравнивание!!!

namespace scene {


/// @defgroup SceneObject Идентификаторы объектов сцены и их описание.
/// @{

/// Идентификаторы всех объектов на сцене.
enum Object {
    // Базовые объекты:
    Absent = -1,        ///< Псевдо-объект "Отсутствия объекта" для управления в связанной с аппаратом СК.
    Zero   =  0,        ///< Объект привязки локальной СК, всегда в нуле.
    MissionNorth,       ///< Точка старта миссии. Объект с нулевыми глубиной/курсом/креном/дифферентом в координатах старта АНПА.
    MissionDirect,      ///< Вектор старта миссии. Объект со всеми известными координатами АНПА во время старта.
    TaskNorth,          ///< Объект "начало галса", который фиксируется при получении новой задачи (с нулевой высотой, курсом, креном, дифферентом).
    TaskDirect,         ///< Объект "начало галса", который фиксируется при получении нового таска

    // СПИСОК ВСЕХ ОБЪЕКТОВ НА СЦЕНЕ:
    Seabed,             ///< Объект "морское дно" для хождения по высоте.
    Ice,                ///< лёд
    PingerDock,

    // НОВЫЕ ОБЪЕКТЫ ДОБАВЛЯТЬ ПЕРЕД ЭТОЙ СТРОЧКОЙ!
    OBJECTS             ///< Количество объектов (ВСЕГДА В КОНЦЕ, НЕ МЕНЯТЬ И НЕ ПЕРЕМЕЩАТЬ!)
};

/// Описание всех объектов.
inline ipc::Schema::Enum object_schema(ipc::Schema::Enum e)
{
    return e.title("Объект")
            // Базовые объекты
            .add(Absent,        ipc::Off,       "СК Аппарата")
            .add(Zero,          ipc::Off,       "Нулевая точка")
            .add(MissionNorth,  ipc::On,        "Точка старта")
            .add(MissionDirect, ipc::On,        "Напр. старта")
            .add(TaskNorth,     ipc::On,        "Точка задачи")
            .add(TaskDirect,    ipc::On,        "Напр. задачи")

            // СПИСОК ВСЕХ ОБЪЕКТОВ НА СЦЕНЕ:
            .add(Seabed,        ipc::Ok,        "Морское дно")
            .add(Ice,           ipc::Ok,        "Лед")
            .add(PingerDock,    ipc::Ok,        "Пингер дока")

            // НОВЫЕ ОБЪЕКТЫ ДОБАВЛЯТЬ ПЕРЕД ЭТОЙ СТРОЧКОЙ!
    ;
}

/// @}


/// @defgroup SceneInput Входящие сообщения.
/// @{

struct ObjectWgs: public geo::PositionWgs
{
    Object  object;
    ipc::Schema schema() {return ipc::Schema(this).type(ipc::NotFiltered)
        .add(object_schema(IPC_ENUM(object)))
        .add(geo::PositionWgs::schema())   // Наследование
        .no_age()
        .title("Объект в глобальной СК")
    ;}
};

struct ObjectLocal: public geo::PositionFlat
{
    Object  object;
    ipc::Schema schema() {return ipc::Schema(this).type(ipc::NotFiltered)
        .add(object_schema(IPC_ENUM(object)))
        .add(geo::PositionFlat::schema())   // Наследование
        .no_age()
        .title("Объект в локальной СК")
    ;}
};

struct ObjectBody: public geo::PositionBody
{
    Object  object;
    ipc::Schema schema() {return ipc::Schema(this).type(ipc::NotFiltered)
        .add(object_schema(IPC_ENUM(object)))
        .add(geo::PositionBody::schema())   // Наследование
        .no_age()
        .title("Объект в связанной СК")
    ;}
};

struct ObjectSphere: public geo::PositionSphere
{
    Object  object;
    ipc::Schema schema() {return ipc::Schema(this).type(ipc::NotFiltered)
        .add(object_schema(IPC_ENUM(object)))
        .add(geo::PositionSphere::schema())   // Наследование
        .no_age()
        .title("Объект в сферической СК")
    ;}
};

struct ObjectCylinder: public geo::PositionCylinder
{
    Object  object;
    ipc::Schema schema() {return ipc::Schema(this).type(ipc::NotFiltered)
        .add(object_schema(IPC_ENUM(object)))
        .add(geo::PositionCylinder::schema())   // Наследование
        .no_age()
        .title("Объект в цилиндрической СК")
    ;}
};

/// @}
///

// Исходящие сообщения //

// Карты объектов

/// Карта всех объектов в локальной СК
struct OutLocal {
    ObjectLocal  objects[OBJECTS];
    ipc::Schema schema() {return ipc::Schema(this).title("Объекты в локальной СК")
        .add(IPC_STRUCTS(objects).title("Объекты").element_title(""))
    ;}
};

/// Карта всех объектов в глобальной СК
struct OutWgs {
    ObjectWgs  objects[OBJECTS];
    ipc::Schema schema() {return ipc::Schema(this).title("Объекты в глобальной СК")
        .add(IPC_STRUCTS(objects).title("Объекты").element_title(""))
    ;}
};

// Избранные объекты

/// Избранный объект в локальной СК
struct OutSelectedLocal: public ObjectLocal {
    ipc::Schema schema() {return ipc::Schema(this)
        .add(ObjectLocal::schema())
        .title("Избранный (счисление)")
    ;}
};

/// Избранный объект в глобальной СК
struct OutSelectedWgs: public ObjectWgs {
    ipc::Schema schema() {return ipc::Schema(this)
        .add(ObjectWgs::schema())
        .title("Избранный (геоположение)")
    ;}
};


///// Избранный объект в локальной СК
//struct OutSelectedLocal {
//    ObjectLocal  object;
//    ipc::Schema schema() {return ipc::Schema(this).title("Избранный (счисление)")
//        .add(IPC_STRUCT(object).title("Избранный объект"))
//    ;}
//};

///// Избранный объект в глобальной СК
//struct OutSelectedWgs {
//    ObjectWgs  object;
//    ipc::Schema schema() {
//        return ipc::Schema(this).title("Избранный (геоположение)")
//        .add(IPC_STRUCT(object).title("Избранный объект"))
//    ;}
//};


}

#pragma pack(pop)
