#ifndef __CHASSIS_H__
#define __CHASSIS_H__


#include "pid.h"
#include "mecanum_wheel.h"
#include "M3508_Motor.h"
#include "SBUS.h"
#include "Task_RemoteControl.h"


#define pi 3.14159f

// 底盘数据结构体
typedef struct
{
    struct
    {
        float vx;
        float vy;
        float wz;
    } Speed_ToCloud;

    struct
    {
        float vx;
        float vy;
        float wz;
    } Speed_ToChassis;

    float Angle_ChassisToCloud;
    float max_speed;
    float chassis_r;
    float chassis_s;
    int16_t motor_omega[4];
    float motor_location[4];
} CHASSIS_Manage_Object;

extern CHASSIS_Manage_Object chassis_control;
/**
 * @brief 底盘初始化
 * @param  void
 * @retval void
 */
void Chassis_Init(void);
/**
 * @brief 底盘运动控制
 * @param  void
 * @retval void
 */
void Chassis_motion_control(void);

#endif
