#include "command.h"

#pragma once

class Commands
{
public:
    static KeyCommand start_control;
    static KeyCommand stop_control;
    static KeyCommand set_min_speed;
    static KeyCommand set_slow_speed;
    static KeyCommand set_max_speed;
    static KeyCommand set_zero_speed;
    static AxisCommand move_forward;
    static AxisCommand move_right;
};
