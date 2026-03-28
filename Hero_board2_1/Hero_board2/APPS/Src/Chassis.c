/**
 * @file Chassis.c
 * @author SJW
 * @brief 底盘控制任务，处理底盘运动控制
 * @version 1.0
 * @date 2026-01-24
 * @copyright Copyright (c) 2026
 */

#include "Chassis.h"

positionpid_t PID_Wheel0; // 左前轮
positionpid_t PID_Wheel1; // 左后轮
positionpid_t PID_Wheel2; // 右后轮
positionpid_t PID_Wheel3; // 右前轮

CHASSIS_Manage_Object chassis_control;

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
 * @brief 将云台坐标系下的速度转换为底盘坐标系下的速度
 * @param angle 云台相对于底盘的角度
 * @retval
 */
void v_cloud_to_chassis(float angle)
{
    float angle_hd = angle * pi / 180;
    chassis_control.Speed_ToChassis.vx = chassis_control.Speed_ToCloud.vx * cos(angle_hd) - chassis_control.Speed_ToCloud.vy * sin(angle_hd);
    chassis_control.Speed_ToChassis.vy = chassis_control.Speed_ToCloud.vx * sin(angle_hd) + chassis_control.Speed_ToCloud.vy * cos(angle_hd);
    chassis_control.Speed_ToChassis.wz = chassis_control.Speed_ToCloud.wz;
}

/**
 * @brief 底盘目标值计算函数
 * @param None
 * @retval None
 */
void chassis_target_calc(void)
{
    v_cloud_to_chassis(chassis_control.Angle_ChassisToCloud);
    // chassis_follow_mode(chassis_control.Angle_ChassisToCloud, follow);
}

/**
 * @brief 底盘运动控制
 * @param  void
 * @retval void
 */
void Chassis_motion_control(void)
{
    chassis_target_calc();
    Velocity.vx = chassis_control.Speed_ToChassis.vx;
    Velocity.vy = -chassis_control.Speed_ToChassis.vy;
    Velocity.wz = -chassis_control.Speed_ToChassis.wz;
    mecanum_forward_kinematics(&Velocity, &Config_Mecanum, &Mecanum_Speeds);
    M3508_Array[Chassis_Left_Front].outCurrent = Position_PID(&PID_Wheel0, Mecanum_Speeds.wheel0, M3508_Array[Chassis_Left_Front].realSpeed);
    M3508_Array[Chassis_Left_Back].outCurrent = Position_PID(&PID_Wheel1, Mecanum_Speeds.wheel1, M3508_Array[Chassis_Left_Back].realSpeed);
    M3508_Array[Chassis_Right_Back].outCurrent = Position_PID(&PID_Wheel2, -Mecanum_Speeds.wheel2, M3508_Array[Chassis_Right_Back].realSpeed);
    M3508_Array[Chassis_Right_Front].outCurrent = Position_PID(&PID_Wheel3, -Mecanum_Speeds.wheel3, M3508_Array[Chassis_Right_Front].realSpeed);
}

/**
 * @brief 获取底盘相对于云台的角度
 * @param  angle 云台相对于底盘的角度
 * @retval void
 */
void steer_getangle(float angle)
{
    chassis_control.Angle_ChassisToCloud = angle;
}
