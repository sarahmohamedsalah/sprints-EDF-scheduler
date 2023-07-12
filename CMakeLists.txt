cmake_minimum_required(VERSION 3.25)
project(shared C)

set(CMAKE_C_STANDARD 11)

include_directories(Obstacle_Avoidance_Car_Project/ECUAL/dcm)
include_directories(Obstacle_Avoidance_Car_Project/ECUAL/delay)
include_directories(Obstacle_Avoidance_Car_Project/ECUAL/icu)
include_directories(Obstacle_Avoidance_Car_Project/ECUAL/kpd)
include_directories(Obstacle_Avoidance_Car_Project/ECUAL/lcd)
include_directories(Obstacle_Avoidance_Car_Project/ECUAL/pwm)
include_directories(Obstacle_Avoidance_Car_Project/ECUAL/us)
include_directories(Obstacle_Avoidance_Car_Project/LIB)
include_directories(Obstacle_Avoidance_Car_Project/MCAL/dio)
include_directories(Obstacle_Avoidance_Car_Project/MCAL/exi)
include_directories(Obstacle_Avoidance_Car_Project/MCAL/timer)

add_executable(shared
        Obstacle_Avoidance_Car_Project/ECUAL/dcm/dcm_cfg.c
        Obstacle_Avoidance_Car_Project/ECUAL/dcm/dcm_cfg.h
        Obstacle_Avoidance_Car_Project/ECUAL/dcm/dcm_interface.h
        Obstacle_Avoidance_Car_Project/ECUAL/dcm/dcm_program.c
        Obstacle_Avoidance_Car_Project/ECUAL/delay/delay_interface.h
        Obstacle_Avoidance_Car_Project/ECUAL/icu/icu_interface.h
        Obstacle_Avoidance_Car_Project/ECUAL/kpd/keypad_interface.h
        Obstacle_Avoidance_Car_Project/ECUAL/lcd/lcd_interface.h
        Obstacle_Avoidance_Car_Project/ECUAL/pwm/pwm_interface.h
        Obstacle_Avoidance_Car_Project/ECUAL/us/us_interface.h
        Obstacle_Avoidance_Car_Project/LIB/bit_math.h
        Obstacle_Avoidance_Car_Project/LIB/std.h
        Obstacle_Avoidance_Car_Project/MCAL/dio/dio_interface.h
        Obstacle_Avoidance_Car_Project/MCAL/exi/exi_interface.h
        Obstacle_Avoidance_Car_Project/MCAL/timer/timer_cfg.c
        Obstacle_Avoidance_Car_Project/MCAL/timer/timer_cfg.h
        Obstacle_Avoidance_Car_Project/MCAL/timer/timer_interface.h
        Obstacle_Avoidance_Car_Project/MCAL/timer/timer_private.h
        Obstacle_Avoidance_Car_Project/MCAL/timer/timer_program.c
        Obstacle_Avoidance_Car_Project/main.c Obstacle_Avoidance_Car_Project/ECUAL/icu/icu_program.c Obstacle_Avoidance_Car_Project/ECUAL/icu/icu_cfg.c Obstacle_Avoidance_Car_Project/ECUAL/us/us_program.c Obstacle_Avoidance_Car_Project/ECUAL/us/us_cfg.h Obstacle_Avoidance_Car_Project/ECUAL/us/us_cfg.c Obstacle_Avoidance_Car_Project/ECUAL/btn/button.c Obstacle_Avoidance_Car_Project/ECUAL/kpd/keypad.c Obstacle_Avoidance_Car_Project/APP/app.h Obstacle_Avoidance_Car_Project/APP/app.c Obstacle_Avoidance_Car_Project/ECUAL/lcd/lcd.c Obstacle_Avoidance_Car_Project/ECUAL/delay/delay_program.c Obstacle_Avoidance_Car_Project/MCAL/dio/dio_program.c Obstacle_Avoidance_Car_Project/ECUAL/lcd/lcd_config.c Obstacle_Avoidance_Car_Project/MCAL/exi/exi_program.c Obstacle_Avoidance_Car_Project/MCAL/exi/exi_cfg.c Obstacle_Avoidance_Car_Project/ECUAL/pwm/pwm_program.c)
