# AUV Control Driver

**AUV Control Driver** - кросплатформенный драйвер для управления автономным необитаемым аппаратом на поверхности воды с использованием джойстика.
В коплекте с драйвером поставляется утилита для привязки
команд управления к кнопкам контроллера.

Интерфейс утилиты:

![Интерфейс утилиты](/utility-interface.png)

## Содержание

- [Предварительные требования](#предварительные-требования)
- [Сборка и установка](#сборка-и-установка)
  - [Linux](#linux)
  - [Windows](#windows)
- [Запуск](#запуск)
- [Что делать, если подключенный джойстик не поддерживается библиотекой SDL?](#что-делать-геймпад-не-поддерживается-в-sdl)
 
## Предварительные требования

Операционная система:

- Windows 7 и моложе
- AstraLinux CommonEdition 2.12, другой Debian based дистрибутив

Перед сборкой и запуском проекта убедитесь, что у вас установлено следующее:

- Qt 5.12
- IPC library

## Сборка и установка

### Linux

Для сборки **драйвера** на Linux выполните следующие шаги:

1. Откройте файл `.pro` и укажите пути к библиотекам SDL и IPC, используя переменные `SDL2_PATH` и `IPC_LIB_PATH`.
2. Откройте терминал и перейдите в директорию проекта.
3. Выполните следующие команды:
   ```bash
   qmake
   make
   make install

Для сборки **утилиты** на Linux выполните следующие шаги:

1. Откройте файл `.pro` и укажите пути к библиотекам SDL, используя переменную `SDL2_PATH`.
2. Откройте терминал и перейдите в директорию проекта.
3. Выполните следующие команды:
   ```bash
   qmake
   make

### Windows

Для сборки **драйвера** на Windows следуйте этим инструкциям:

1. Откройте файл `.pro` и укажите пути к библиотекам SDL и IPC, используя переменные `SDL2_PATH` и `IPC_LIB_PATH`.
2. Откройте проект в Qt Creator.
3. Перейдите в раздел "Проекты" (CTRL+5).
4. Выберите текущую конфигурацию сборки и нажмите кнопку "Запуск".
5. "Развертывание, добавить этап" -> "Сборка", и введите "install" в поле параметров make.
6. Соберите проект, нажав "Сборка" -> "Запустить qmake", а затем "Сборка" -> "Собрать проект" и "Сборка" -> "Развернуть".

Если указанные выше инструкции не работают, вы можете попробовать особую сборку:

1. "Развертывание, добавить этап" -> "Особый этап обработки".
2. В поле команда укажите: mingw32-make.exe, в поле параметры: install.
3. Далее выполните последний пункт из предыдущей инструкции.

Для сборки **утилиты** на Windows следуйте этим инструкциям:

1. Откройте файл `.pro` и укажите пути к библиотекам SDL, используя переменную `SDL2_PATH`.
2. Откройте проект в Qt Creator.
3. Соберите проект, нажав "Сборка" -> "Запустить qmake", а затем "Сборка" -> "Собрать проект".

## Запуск

Для запуска драйвера в веб-интерфейсе IPC запустите скрипт *run_with_viewer.cmd* на Windows, *run_with_viewer.sh* на Linux. Для завершения работы драйвера
на Linux запустите скрипт *stop_all.sh*.

## Что делать, если подключенный джойстик не поддерживается?  <a name="что-делать-геймпад-не-поддерживается-в-sdl"></a>

1. Установите программу [SDL2 Gamepad Tool](https://generalarcade.com/gamepadtool/) для вашей операционной системы.
2. Ознакомьтесь с инструкцией использования данного ПО.
3. Выполните настройку вашего устройства.
4. Добавьте полученный *mapping*  в файл *gamecontrollerdb.txt*, который находится в папке с установленным драйвером.

