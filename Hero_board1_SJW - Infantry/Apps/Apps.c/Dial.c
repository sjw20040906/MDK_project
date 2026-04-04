/**
 * @file Dial.c
 * @author sjw
 * @brief 
 * @version 0.1
 * @date 2026-03-22
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "Dial.h"

/**************数据定义****************/
static uint32_t last_check_time = 0;    // 上一次检测时间
static int32_t last_angle = 0;          // 上一次角度
static uint8_t is_reversing = 0;        // 反转标志
static uint32_t reverse_start_time = 0; // 反转开始时间

float current_heat_copy = 0;
uint32_t cooling_ticks_copy = 0;
uint32_t delta_time = 0;
float delta_ticks = 0;

/****************函数结构体声明******************/
Dial_Data_t Dial_Data = Dial_DataGroundInit;
#undef Dial_DataGroundInit
Heat_Data_t Heat_Data = Heat_Data_Init_Burst_First_Level1;
#undef Heat_Data_Init_Burst_First_Level1

/**
 * @brief  拨弹处理函数
 * @attention
 */
void Dial_Processing(void)
{
    if (Dial_Data.Shoot_Mode == Continuous_Shoot && Dial_Data.Dial_Switch == Dial_On)
    {
        Normal_Dial();             // 正常拨弹执行
        Bullet_Stuck_Processing(); // 卡弹检测
    }
    else
    {
        Status_Refresh();
    }
}

/**
 * @brief  正常拨弹执行
 * @attention 
 */
void Normal_Dial(void)
{
    Heat_Data.current_time = HAL_GetTick();

    if (Heat_Data.last_normal_angle == 0)
    {
        Heat_Data.last_normal_angle = M2006_Array[Dial_Motor].totalAngle;
    }

    /***********************执行拨弹**************************************** */

    if (!is_reversing) // 只要没有正在反转解卡，就一直拨弹
    {
        M2006_Array[Dial_Motor].targetSpeed = Dial_Data.Speed_Dial;
        M2006_Array[Dial_Motor].outCurrent = PID_Model4_Update(&M2006_DialI_Pid, &fuzzy_pid_bullet_v, M2006_Array[Dial_Motor].targetSpeed, M2006_Array[Dial_Motor].realSpeed);

        int32_t current_angle = M2006_Array[Dial_Motor].totalAngle;
        Heat_Data.last_normal_angle = current_angle;
    }
}

/**
 * @brief  卡弹检测
 * @attention 【修改】判断条件中去掉了 !Heat_Data.overheat
 */
void Bullet_Stuck_Processing(void)
{
    // 定时检测角度变化
    if (Heat_Data.current_time - last_check_time >= CHECK_INTERVAL)
    {
        int32_t angle_change = abs(M2006_Array[Dial_Motor].totalAngle - last_angle);

        if (angle_change < ANGLE_CHANGE_THRESHOLD && !is_reversing)
        {
            is_reversing = 1;
            reverse_start_time = Heat_Data.current_time;
            // 立即开始反转
            M2006_Array[Dial_Motor].targetSpeed = 1000;
            M2006_Array[Dial_Motor].outCurrent = PID_Model4_Update(&M2006_DialI_Pid, &fuzzy_pid_bullet_v, M2006_Array[Dial_Motor].targetSpeed, M2006_Array[Dial_Motor].realSpeed);
        }

        last_check_time = Heat_Data.current_time;
        last_angle = M2006_Array[Dial_Motor].totalAngle;
    }

    /**********************拨弹盘卡弹反转执行***************************************/
    if (is_reversing)
    {
        if (Heat_Data.current_time - reverse_start_time >= REVERSE_DURATION)
        {
            is_reversing = 0; // 取消反转
            Heat_Data.last_normal_angle = M2006_Array[Dial_Motor].totalAngle;
        }
        else
        {
            M2006_Array[Dial_Motor].targetSpeed = 1000;
            M2006_Array[Dial_Motor].outCurrent = PID_Model4_Update(&M2006_DialI_Pid, &fuzzy_pid_bullet_v, M2006_Array[Dial_Motor].targetSpeed, M2006_Array[Dial_Motor].realSpeed);
            return;
        }
    }
}

/**
 * @brief  状态刷新
 */
void Status_Refresh(void)
{
    M2006_Array[Dial_Motor].outCurrent = PID_Model4_Update(&M2006_DialI_Pid, &fuzzy_pid_bullet_v, 0, M2006_Array[Dial_Motor].realSpeed);
    Heat_Data.overheat = 0;
    Heat_Data.last_normal_angle = M2006_Array[Dial_Motor].totalAngle;
    Dial_Data.Speed_Dial = Dial_Data.Dial_Gear == Dial_Gear_Low ? Dail_Low_Speed : Dail_High_Speed;
    if(Dial_Data.Dial_Gear == Dial_Gear_Low)
    {
        ControlMes.shoot_Speed = 1;
    }
    else
    {
        ControlMes.shoot_Speed = 2;
    }   
}
