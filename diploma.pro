TEMPLATE = app
CONFIG -= qt
CONFIG += console c++17
CONFIG += debug

TARGET = diploma

SDL2_PATH = "C:\libs\SDL2-2.26.4"

INCLUDEPATH += $${SDL2_PATH}\include
LIBS += -L$${SDL2_PATH}\lib\x64 -lSDL2 -lSDL2main

SOURCES += \
    gamepad.cpp \
    main.cpp \
    application.cpp


HEADERS += \
    application.h \
    gamepad.h

DESTDIR = $${_PRO_FILE_PWD_}/bin
