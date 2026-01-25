/**
 * @file Chassis.c
 * @author SJW
 * @brief
 * @version 1.0
 * @date 2026-01-24
 * @copyright Copyright (c) 2026
 */

#include "Chassis.h"

positionpid_t PID_Wheel0; // 左前轮
positionpid_t PID_Wheel1; // 左后轮
positionpid_t PID_Wheel2; // 右后轮
positionpid_t PID_Wheel3; // 右前轮

/**
 * @brief 底盘初始化
 * @param  void
 * @retval void
 */
void Chassis_Init(void)
{
    mecanum_init(&Config_Mecanum, 0.077f, 0.2f, 0.1925f);
    Position_PIDInit(&PID_Wheel0, 30.0f, 0.000f, 0.0f, 0.0f, 16384, 30000, 6000);
    Position_PIDInit(&PID_Wheel1, 30.0f, 0.000f, 0.0f, 0.0f, 16384, 30000, 6000);
    Position_PIDInit(&PID_Wheel2, 30.0f, 0.000f, 0.0f, 0.0f, 16384, 30000, 6000);
    Position_PIDInit(&PID_Wheel3, 30.0f, 0.000f, 0.0f, 0.0f, 16384, 30000, 6000);
}

/**
 * @brief 底盘运动控制
 * @param  void
 * @retval void
 */
void Chassis_motion_control(void)
{
    if(mappedData.Ch3<10 && mappedData.Ch3>-10)
        mappedData.Ch3 = 0;
    if(mappedData.Ch4<10 && mappedData.Ch4>-10)
        mappedData.Ch4 = 0;
    if(mappedData.Ch10<10 && mappedData.Ch10>-10)
        mappedData.Ch10 = 0;
    Velocity.vx = mappedData.Ch3*0.8;
    Velocity.vy = -mappedData.Ch4*0.8;
    Velocity.wz = -mappedData.Ch10*0.8;
    mecanum_forward_kinematics(&Velocity, &Config_Mecanum, &Mecanum_Speeds);
    M3508_Array[Chassis_Left_Front].outCurrent = Position_PID(&PID_Wheel0, Mecanum_Speeds.wheel0, M3508_Array[Chassis_Left_Front].realSpeed);
    M3508_Array[Chassis_Left_Back].outCurrent = Position_PID(&PID_Wheel1, Mecanum_Speeds.wheel1, M3508_Array[Chassis_Left_Back].realSpeed);
    M3508_Array[Chassis_Right_Back].outCurrent = Position_PID(&PID_Wheel2, -Mecanum_Speeds.wheel2, M3508_Array[Chassis_Right_Back].realSpeed);
    M3508_Array[Chassis_Right_Front].outCurrent = Position_PID(&PID_Wheel3, -Mecanum_Speeds.wheel3, M3508_Array[Chassis_Right_Front].realSpeed);
}
