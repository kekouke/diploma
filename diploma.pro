TARGET = diploma

TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle # Не собирать маковский архив
CONFIG -= qt
CONFIG += debug

QT -= gui
QT -= core


SDL2_PATH = "C:\libs\SDL2-2.26.4\i686-w64-mingw32"
IPC_LIB_PATH = "C:\libs\ipc_lib\usr"

INCLUDEPATH += \
    $${SDL2_PATH}\include \
    $${IPC_LIB_PATH}\include
LIBS += \
    -L$${SDL2_PATH}\lib -lSDL2 -lSDL2main \
    -L$${IPC_LIB_PATH}\lib -lipc

SOURCES += \
    gamepad.cpp \
    main.cpp \
    application.cpp


HEADERS += \
    messages.h \
    application.h \
    gamepad.h \
    motion.h

DESTDIR = $$PWD/bin
OBJECTS_DIR = $$PWD/build            # Путь объектников
MOC_DIR = $$PWD/build                # Путь моков
RCC_DIR = $$PWD/build                # Путь ресорцов

# ИНСТРУКЦИЯ ПО УСТАНОВКЕ ipc-lib
# Для установки всех необходимых dll в windows через qtcreator необходимо добавить этап установки:
# Заходим в "Проекты"(CTRL+5), для текущей сборки жмём кнопку "Запустить",
# 1) "Установка, добавить этап" -> "Сборка", в поле параметры make пишем "install"
# 2) Делаем "Сборка" -> "Запустить qmake", "Сборка" -> "Установить всё"
# Если этот вариант не сработал, то делаем особую сборку:
# 1) "Установка, добавить этап" -> "Особый", в поле команда указываем:
#    mingw32-make.exe, в поле параметры: install
# 2) как и в предыдущей инструкции

INSTALL.path    = $$DESTDIR                   # Куда копируем
INSTALL.files  += $${IPC_LIB_PATH}/lib/*      # Что копируем
INSTALL.files  += $$PWD/load                  # Директория с настройками
INSTALL.files  += $$PWD/run_with_viewer.cmd   # Скрипт запуска приложения
INSTALLS       += INSTALL                     # Добавляем к установке свои копирования
