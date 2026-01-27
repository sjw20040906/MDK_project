/**
 * @file Task_RobotControl.c
 * @author Rm_Team
 * @brief
 * @version 0.1
 * @date 2023-08-30
 *
 * @copyright Copyright (c) 2021
 *
 */ 


#include "BSP_Can.h"
#include "BSP_BoardCommunication.h"
#include "mecanum_wheel.h"
#include "M3508_Motor.h"
#include "Chassis.h"
#include "Gimbal.h"
#include "DM_Motor.h"

void Robot_Control(void const *argument);

