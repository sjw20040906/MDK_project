/**
 * @file Gimbal.c
 * @author SJW
 * @brief 云台
 * @version 1.0
 * @date 2026-01-27
 * @copyright
 */

#include "Gimbal.h"

Gimbal_t Gimbal;

float world_target_speed = 0;
float chassis_real_speed = 0;
float Angle_Cloud = 0; 
float Setup_Angleoffset = 56907; 
/************云台PID***********/
pid_advanced_t M6020s_YawOPID;
pid_advanced_t AutoAim_M6020s_YawOPID;
positionpid_t J8006s_YawOPID;
/************云台PID END***********/

/**
 * @brief 云台初始化
 * @param  void
 * @retval void
 */
void Gimbal_Init(void)
{
    /****************** (pid_t*) ******** Kp ***** Kd ****** Ki ***** Kf ***** MaxOutput ******* IntegralLimit*******/
    Gimbal.Target_Yaw = IMU_Angle.Yaw / 360.0f * 65536.0f + DM_Array[Gimbal_Motor].angleInit;
    PID_Advanced_Init(&M6020s_YawOPID,   0.0015f, 0.00004f,  0.000001f, 0.002f,  20.0f,  0.5f);
    PID_Advanced_Init(&AutoAim_M6020s_YawOPID,   0.8f,   0.4f,  0.000002f, 0.3f,  10.0f,  10.0f);
}



/**
 * @brief Yaw运动控制
 * @param  void
 * @retval void
 */
void Yaw_Angle_Set(void)
{
    if (DM_Array[Gimbal_Motor].InfoUpdateFrame <= 30)
    {
        Gimbal.Target_Yaw = IMU_Angle.Yaw / 360.0f * 65536.0f + DM_Array[Gimbal_Motor].angleInit;
    }
    /*********************** 目标角度归一化到 0-65535 范围 ***********************/
    while (Gimbal.Target_Yaw > 65536.0f)
        Gimbal.Target_Yaw -= 65536.0f;
    while (Gimbal.Target_Yaw < 0.0f)
        Gimbal.Target_Yaw += 65536.0f;
    /*********************** 坐标系转换 ***********************/
    float Angle_Yaw_Chassis = IMU_Angle.Yaw / 360.0f * 65536.0f;
    float Angle_Yaw_Cloud = DM_Array[Gimbal_Motor].angleInit + Angle_Yaw_Chassis;
    if (Angle_Yaw_Cloud > 65535.0f)
    {
        Angle_Yaw_Cloud -= 65536.0f;
    }
    else if (Angle_Yaw_Cloud < -65536.0f)
    {
        Angle_Yaw_Cloud += 65536.0f;
    }
    ControlMes.yaw_realAngle = Angle_Yaw_Cloud;
    /*********************** PID控制逻辑 ***********************/
    chassis_real_speed = IMU_Angle.Z_Vel* (65536.0f / 360.0f);
    // 手动遥控模式
    if (ControlMes.AutoAimFlag == 0)
    {
        world_target_speed = PID_Advanced_Angle_Calc_Positional(&M6020s_YawOPID, Gimbal.Target_Yaw, Angle_Yaw_Cloud, 65534.0f);
        DM_Array[Gimbal_Motor].outSpeed = world_target_speed - chassis_real_speed;
    }
    // 自动瞄准模式
    else if (ControlMes.AutoAimFlag == 1)
    {
        world_target_speed = PID_Advanced_Angle_Calc_Positional(&AutoAim_M6020s_YawOPID, Gimbal.Target_Yaw, Angle_Yaw_Cloud, 65534.0f);
        DM_Array[Gimbal_Motor].outSpeed = world_target_speed - chassis_real_speed;
    }
}

/**
 * @brief 云台控制函数
 * @param  void
 * @retval void
 */
void Gimbal_motion_Control(void)
{
    Yaw_Angle_Set();
    /********** 云台角度反馈 **********/
    Angle_Cloud = DM_Array[Gimbal_Motor].angleInit+ Setup_Angleoffset;
    if (Angle_Cloud > 32768)
        Angle_Cloud -= 65536;
    else if (Angle_Cloud < -32768)
        Angle_Cloud += 65536;
    steer_getangle(-1*Angle_Cloud/65536.0f*360.0f);
}
