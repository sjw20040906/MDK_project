/**
 * @file Chassis.c
 * @author SJW
 * @brief
 * @version 1.0
 * @date 2026-01-24
 * @copyright Copyright (c) 2026
 */

#include "Chassis.h"

float Delta_F = 0;
float Delta_R = 0;

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
    Position_PIDInit(&PID_Wheel0, 25.0f, 0.000f, 0.0f, 0.0f, 16384, 30000, 6000);
    Position_PIDInit(&PID_Wheel1, 25.0f, 0.000f, 0.0f, 0.0f, 16384, 30000, 6000);
    Position_PIDInit(&PID_Wheel2, 25.0f, 0.000f, 0.0f, 0.0f, 16384, 30000, 6000);
    Position_PIDInit(&PID_Wheel3, 25.0f, 0.000f, 0.0f, 0.0f, 16384, 30000, 6000);
}

/**
 * @brief 底盘运动控制
 * @param  void
 * @retval void
 */
void Chassis_motion_control(void)
{
    /***************履带控制策略*****************/
    if (mappedData.Ch5 == 1)
    {
        Delta_F = mappedData.Ch10 * 0.002f;
    }
    else if (mappedData.Ch5 == 3)
    {
        Delta_R = mappedData.Ch10 * 0.002f;
    }
    if (Delta_F > 10)
        Delta_F = 10;
    else if (Delta_F < -10)
        Delta_F = -10;
    if (Delta_R > 10)
        Delta_R = 10;
    else if (Delta_R < -10)
        Delta_R = -10;
    ControlMes_board2.LF_track += Delta_F;
    ControlMes_board2.LR_track += Delta_R;
    ControlMes_board2.RR_track += Delta_R;
    ControlMes_board2.RF_track += Delta_F;
    if (ControlMes_board2.LF_track > 70)
        ControlMes_board2.LF_track = 70;
    else if (ControlMes_board2.LF_track < -70)
        ControlMes_board2.LF_track = -70;
    if (ControlMes_board2.LR_track > 70)
        ControlMes_board2.LR_track = 70;
    else if (ControlMes_board2.LR_track < -70)
        ControlMes_board2.LR_track = -70;
    if (ControlMes_board2.RR_track > 70)
        ControlMes_board2.RR_track = 70;
    else if (ControlMes_board2.RR_track < -70)
        ControlMes_board2.RR_track = -70;
    if (ControlMes_board2.RF_track > 70)
        ControlMes_board2.RF_track = 70;
    else if (ControlMes_board2.RF_track < -70)
        ControlMes_board2.RF_track = -70;
    if (mappedData.Ch3 < 10 && mappedData.Ch3 > -10)
        mappedData.Ch3 = 0;
    if (mappedData.Ch4 < 10 && mappedData.Ch4 > -10)
        mappedData.Ch4 = 0;

    /***************麦克纳姆轮控制策略*****************/
    Velocity.vx = mappedData.Ch3 * 0.8;
    Velocity.vy = -mappedData.Ch4 * 0.8;
    if (mappedData.Ch5 == 2)
        Velocity.wz = -mappedData.Ch10 * 0.8;
    else
        Velocity.wz = 0;
    mecanum_forward_kinematics(&Velocity, &Config_Mecanum, &Mecanum_Speeds);
    M3508_Array[Chassis_Left_Front].outCurrent = Position_PID(&PID_Wheel0, Mecanum_Speeds.wheel0, M3508_Array[Chassis_Left_Front].realSpeed);
    M3508_Array[Chassis_Left_Back].outCurrent = Position_PID(&PID_Wheel1, Mecanum_Speeds.wheel1, M3508_Array[Chassis_Left_Back].realSpeed);
    M3508_Array[Chassis_Right_Back].outCurrent = Position_PID(&PID_Wheel2, -Mecanum_Speeds.wheel2, M3508_Array[Chassis_Right_Back].realSpeed);
    M3508_Array[Chassis_Right_Front].outCurrent = Position_PID(&PID_Wheel3, -Mecanum_Speeds.wheel3, M3508_Array[Chassis_Right_Front].realSpeed);
}
