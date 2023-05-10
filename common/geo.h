#pragma once
/** @file
  Описание шестимерных гео-координат.
  Приводятся классы (предки сообщений), по работе с шестимерными географическими координатами.
  Основное назначение классов - образование базовых классов для сообщений навигационнной системы,
  регуляторов движения и других модулей, задействованных в навигационно-управляющем комплексе.
**/

#include "common/ipc/ipc.h"  // Подключаем функционал библиотеки.
#pragma pack(push,1)         // Однобайтовое выравнивание для кроссплатформенной работы
namespace geo {              // Пространство имен всего файла

/// @defgroup GeoValid Валидность данных
/// Нумерованный список валидности какого-либо параметра.
/// @{

/// Валидность какого-либо параметра на оси координат
enum Valid
{
    None   =  -1,
    Absent =   0,
    Ok     =   1
};

/// Схема описания какого-либо параметра на оси координат
inline ipc::Schema::Enum valid_schema(ipc::Schema::Enum e)
{
    return e.add(IPC_ENUM_VALUE(None),   ipc::Debug, "---")    // Отсутствует поставщик данных => нет параметра.
            .add(IPC_ENUM_VALUE(Absent), ipc::Off,   "Нет")    // Данные отсутствуют => нет валидности.
            .add(IPC_ENUM_VALUE(Ok),     ipc::Ok,    "Есть");  // Данные присутствуют => есть валидность.
}

/// @}



/// @defgroup GeoAxes Описание осей АНПА и Земли
/// Нумерованный список принятых в навигации координатных осей АНПА и Земли
/// @{

/// Перечисление осей координат в шестимерной декартовой СК
enum Axis {
    Right,
    Forward,
    Up,
    Yaw,
    Pitch,
    Roll,
    Num
};

/// Перечисление осей скоростей
namespace velocity {
enum Enum {
    Right,
    Forward,
    Up,
    Yaw,
    Pitch,
    Roll,
    Num
};
}

/// Инкремент по осям, предпологаем что идут всегда(!) непрерывно
inline Axis& operator++(Axis &axis) {
    int tmp = static_cast<int>(axis);
    axis = static_cast<Axis>(++tmp);
    return axis;
}

/// Перечисление осей координат в Wgs84
enum Wgs {
    Lon,
    Lat
};

/// Перечисление осей координат в локальной/плоской СК
enum Flat {
    East,
    North
};

/// Перечисление осей в сферической СК
enum Sphere {
    Alpha,
    Beta
};

/// Перечисление осей в цилиндрической СК
enum Cylinder {
    Bearing,
    Distance,
    Height
};



/// Схема описания перечисления осей координат в шестимерной СК
inline ipc::Schema::Enum axis_schema(ipc::Schema::Enum e)
{
    return e.add(Right,   ipc::Ok, "Вправо/Восток/Долгота")
            .add(Forward, ipc::Ok, "Вперед/Север/Широта")
            .add(Up,      ipc::Ok, "Вверх")
            .add(Yaw,     ipc::Ok, "Курс")
            .add(Pitch,   ipc::Ok, "Дифферент")
            .add(Roll,    ipc::Ok, "Крен");
}

/// Тип оси
enum AxisType {
    Linear, ///< Линейная
    Angular ///< Вращательная
};

/// Тип оси в зависимости от её номера
inline AxisType axis_type(Axis axis) {
    return axis >= Yaw ? Angular : Linear;
}
/// @}


/// @defgroup GeoPosition Положение объектов в разных системах координат.
/// Классы служат для описания местоположения различных объектов,
/// в т.ч. АНПА, в разных системах координат.
/// @{

/// Общая структура каждой координаты
struct Coord
{
    Valid   valid;
    double  value;

    ipc::Schema schema() {return ipc::Schema(this).title("Координата")
        .add(valid_schema(IPC_ENUM(valid))        .title("Валидность"))
        .add(IPC_REAL(value)                      .title("Значение").precision(8));  //!< @todo: Проставить разные точности!!!
    }
};

/// Общая структура всех координат
struct Coords
{
    Coord  coords[Num];

    ipc::Schema schema() {
        return ipc::Schema(this).title("Координаты")
            .add(valid_schema(IPC_ENUM(coords[Lon].valid))  .title("Валидность X"         ).default_(None))
            .add             (IPC_REAL(coords[Lon].value)   .title("Координата X"         ))
            .add(valid_schema(IPC_ENUM(coords[Lat].valid))  .title("Валидность Y"         ).default_(None))
            .add             (IPC_REAL(coords[Lat].value)   .title("Координата Y"         ))
            .add(valid_schema(IPC_ENUM(coords[Up].valid))   .title("Валидность вверх"     ).default_(None))
            .add             (IPC_REAL(coords[Up].value)    .title("Координата вверх"     ))
            .add(valid_schema(IPC_ENUM(coords[Yaw].valid))  .title("Валидность курса"     ).default_(None))
            .add             (IPC_REAL(coords[Yaw].value)   .title("Координата курса"     ))
            .add(valid_schema(IPC_ENUM(coords[Pitch].valid)).title("Валидность дифферента").default_(None))
            .add             (IPC_REAL(coords[Pitch].value) .title("Координата дифферента"))
            .add(valid_schema(IPC_ENUM(coords[Roll].valid)) .title("Валидность крена"     ).default_(None))
            .add             (IPC_REAL(coords[Roll].value)  .title("Координата крена"     ))
        ;
    }
};


/// Координаты объектов в глобальных гео-кооринатах (долгота-широта, град)
struct PositionWgs: public Coords
{
    ipc::Schema schema() {
        return ipc::Schema(this)
            //.add(Coords::schema())
            .title("Глобальные координаты")
            .add(valid_schema(IPC_ENUM(coords[Lon].valid))  .title("Валидность долготы"   ).default_(None))
            .add             (IPC_REAL(coords[Lon].value)   .title("Координата долготы"   ).unit("°").precision(8).minimum(-180).maximum(180))
            .add(valid_schema(IPC_ENUM(coords[Lat].valid))  .title("Валидность широты"    ).default_(None))
            .add             (IPC_REAL(coords[Lat].value)   .title("Координата широты"    ).unit("°").precision(8).minimum( -90).maximum( 90))
            .add(valid_schema(IPC_ENUM(coords[Up].valid))   .title("Валидность вверх"     ).default_(None))
            .add             (IPC_REAL(coords[Up].value)    .title("Координата вверх"     ).unit("м").precision(2))
            .add(valid_schema(IPC_ENUM(coords[Yaw].valid))  .title("Валидность курса"     ).default_(None))
            .add             (IPC_REAL(coords[Yaw].value)   .title("Координата курса"     ).unit("°").precision(2))
            .add(valid_schema(IPC_ENUM(coords[Pitch].valid)).title("Валидность дифферента").default_(None))
            .add             (IPC_REAL(coords[Pitch].value) .title("Координата дифферента").unit("°").precision(2))
            .add(valid_schema(IPC_ENUM(coords[Roll].valid)) .title("Валидность крена"     ).default_(None))
            .add             (IPC_REAL(coords[Roll].value)  .title("Координата крена"     ).unit("°").precision(2))
        ;
    }
};


/// Координаты объектов в локальной СК (восток-север, м)
struct PositionFlat: public Coords
{
    ipc::Schema schema() {
        return ipc::Schema(this)
            //.add(Coords::schema())
            .title("Локальные координаты")
            .add(valid_schema(IPC_ENUM(coords[East].valid)) .title("Валидность к востоку"   ).default_(None))
            .add             (IPC_REAL(coords[East].value)  .title("Смещение к востоку"     ).unit("м").precision(2))
            .add(valid_schema(IPC_ENUM(coords[North].valid)).title("Валидность к северу"    ).default_(None))
            .add             (IPC_REAL(coords[North].value) .title("Смещение к северу"      ).unit("м").precision(2))
            .add(valid_schema(IPC_ENUM(coords[Up].valid))   .title("Валидность вверх"       ).default_(None))
            .add             (IPC_REAL(coords[Up].value)    .title("Смещение вверх"         ).unit("м").precision(2))
            .add(valid_schema(IPC_ENUM(coords[Yaw].valid))  .title("Валидность курса"       ).default_(None))
            .add             (IPC_REAL(coords[Yaw].value)   .title("Смещение по курсу"      ).unit("°").precision(2))
            .add(valid_schema(IPC_ENUM(coords[Pitch].valid)).title("Валидность дифферента"  ).default_(None))
            .add             (IPC_REAL(coords[Pitch].value) .title("Смещение по дифференту" ).unit("°").precision(2))
            .add(valid_schema(IPC_ENUM(coords[Roll].valid)) .title("Валидность крена"       ).default_(None))
            .add             (IPC_REAL(coords[Roll].value)  .title("Смещение по крену"      ).unit("°").precision(2))
        ;
    }
};

/// Координаты объектов в связанной с аппаратом СК (вправо-вперед, м)
struct PositionBody: public Coords
{
    ipc::Schema schema(){
        return ipc::Schema(this)
            .add(Coords::schema())
            .title("Координаты в связанной СК")
//            .add(IPC_STRUCT(coords[Right])  .title("Вправо, м"))
//            .add(IPC_STRUCT(coords[Forward]).title("Вперед, м"))
//            .add(IPC_STRUCT(coords[Up])     .title("Вверх, м"))
//            .add(IPC_STRUCT(coords[Yaw])    .title("Курс, °"))
//            .add(IPC_STRUCT(coords[Pitch])  .title("Дифферент, °"))
//            .add(IPC_STRUCT(coords[Roll])   .title("Крен, °"))
        ;
    }
};

/// Координаты объектов в связанной с аппаратом СК (вправо-вперед, м)
struct PositionSphere: public Coords
{
    ipc::Schema schema(){
        return ipc::Schema(this)
            .add(Coords::schema())
            .title("Координаты в сферической СК")
//            .add(IPC_STRUCT(coords[Alpha])   .title("Пеленг на объект, °"    ))//.unit("°").minimum(-180).maximum(  180).precision(1))
//            .add(IPC_STRUCT(coords[Beta])    .title("Тангаж на объект, °"    ))//.unit("°").minimum(-180).maximum(  180).precision(1))
//            .add(IPC_STRUCT(coords[Distance]).title("Дистанция до объекта, м"))//.unit("м").minimum(   0).maximum(50000).precision(2))
//            .add(IPC_STRUCT(coords[Yaw])     .title("Направление объекта, °" ))//.unit("°").minimum(-180).maximum(  180).precision(1))
//            .add(IPC_STRUCT(coords[Pitch])   .title("Дифферент, °"           ))//.unit("°").minimum( -80).maximum(   80).precision(1))
//            .add(IPC_STRUCT(coords[Roll])    .title("Крен, °"                ))//.unit("°").minimum( -10).maximum(   10).precision(1))
        ;
    }
};

/// Координаты объектов в связанной с аппаратом СК (вправо-вперед, м)
struct PositionCylinder: public Coords
{
    ipc::Schema schema(){
        return ipc::Schema(this)
            .add(Coords::schema())
            .title("Координаты в цилиндрической СК")
//            .add(IPC_STRUCT(coords[Height])  .title("Высота объекта, м"      ))//.unit("м").minimum(-10000).maximum(10000).precision(2))
//            .add(IPC_STRUCT(coords[Bearing]) .title("Пеленг на объект, °"    ))//.unit("°").minimum(  -180).maximum(180  ).precision(1))
//            .add(IPC_STRUCT(coords[Distance]).title("Дистанция до объекта, м"))//.unit("м").minimum(     0).maximum(50000).precision(2))
//            .add(IPC_STRUCT(coords[Yaw])     .title("Направление объекта, °" ))//.unit("°").minimum(  -180).maximum(  180).precision(1))
//            .add(IPC_STRUCT(coords[Pitch])   .title("Дифферент, °"           ))//.unit("°").minimum(   -80).maximum(   80).precision(1))
//            .add(IPC_STRUCT(coords[Roll])    .title("Крен, °"                ))//.unit("°").minimum(   -10).maximum(   10).precision(1))
        ;
    }
};

/// @}


/// @defgroup GeoVelocity Описание скорости АНПА
/// Классы служат для описания скорости АНПА в разных системах координат
/// @{

/// Скорости объектов с связанной с ними СК (вправо-вперед-вверх)
struct VelocityBody: public Coords
{
    ipc::Schema schema(){
        return ipc::Schema(this)
            .title("Скорость")
            //.add(Coords::schema())
            .add(valid_schema(IPC_ENUM(coords[Right].valid))  .title("Валидность вправо"    ).default_(None))
            .add             (IPC_REAL(coords[Right].value)   .title("Скорость вправо"      ).unit("м/с").precision(2))
            .add(valid_schema(IPC_ENUM(coords[Forward].valid)).title("Валидность вперед"    ).default_(None))
            .add             (IPC_REAL(coords[Forward].value) .title("Скорость вперед"      ).unit("м/с").precision(2))
            .add(valid_schema(IPC_ENUM(coords[Up].valid))     .title("Валидность вверх"     ).default_(None))
            .add             (IPC_REAL(coords[Up].value)      .title("Скорость вверх/-глуб" ).unit("м/с").precision(2))
            .add(valid_schema(IPC_ENUM(coords[Yaw].valid))    .title("Валидность курса"     ).default_(None))
            .add             (IPC_REAL(coords[Yaw].value)     .title("Скорость курса"       ).unit("°/с").precision(2))
            .add(valid_schema(IPC_ENUM(coords[Pitch].valid))  .title("Валидность дифферента").default_(None))
            .add             (IPC_REAL(coords[Pitch].value)   .title("Скорость дифферента"  ).unit("°/с").precision(2))
            .add(valid_schema(IPC_ENUM(coords[Roll].valid))   .title("Валидность крена"     ).default_(None))
            .add             (IPC_REAL(coords[Roll].value)    .title("Скорость крена"       ).unit("°/с").precision(2))
        ;
    }
};

/// @}


}
#pragma pack(pop)
