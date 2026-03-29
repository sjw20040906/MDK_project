/**
 * @file Dial.h
 * @author sjw
 * @brief 处理拨盘电机的控制问题
 * @version 0.1
 * @date 2023-08-14
 */

#ifndef __DIAL_H
#define __DIAL_H

#include "Shoot.h"
#include "main.h"
#include "cmsis_os.h"
#include "FuzzyPID.h"
#include "PID.h"
#include "FeedForward.h"
#include "stm32f4xx_hal.h"

#define Dail_Low_Speed 2000    // 低拨弹速度
#define Dail_High_Speed 4000   // 高拨弹速度
#define Dial_Gear_Low 0        // 低拨弹速度档位
#define Dial_Gear_High 1       // 高拨弹速度档位
#define Dial_Back_NO 0         // 不退弹
#define Dial_Back_YES 1        // 退弹
#define Dial_Motor_Speed -2000 // 拨盘电机速度

/**
 * @brief  拨弹电机的开关
 */
typedef enum
{
    Dial_Off = 0,
    Dial_On,
    Dial_Back,
} Dial_On_Off;

/**
 * @brief  发射模式
 */
typedef enum
{
    Single_Shoot = 0,
    Continuous_Shoot,
    No_Shoot,
} Shoot_Modes;

/* 拨出一个弹丸，电机需要转动的角度，映射为360°-8192 */
#define Angle_DialOneBullet_17mm 36864.0f

/* 拨盘相关参数 */
#define CHECK_INTERVAL 20          // 扭矩检测间隔(ms)
#define REVERSE_DURATION 400       // 反转持续时间(ms)
#define DIAL_REVERSE_SPEED 1000    // 反转速度
#define DIAL_TORQUE_THRESHOLD 7000 // 堵转扭矩阈值，根据实测调整

/* Dial数据接口结构体 */
#define Dial_DataGroundInit \
    {                       \
        0,                  \
        330,                \
        0,                  \
        0,                  \
        4000,               \
        Single_Shoot,       \
        Dial_Off,           \
        Dial_Gear_Low,      \
        Dial_Back_NO,       \
    }

typedef struct Dial_Data_t
{
    uint8_t Number_ToBeFired; // 需要打击的弹丸数量
    uint16_t Shoot_Interval;  // 攻击间隔
    uint32_t Time_NextShoot;  // 下次射击时间
    uint16_t Bullet_Dialed;   // 已打出的子弹
    int16_t Speed_Dial;       // 连发拨盘速度
    Shoot_Modes Shoot_Mode;   // 发射模式
    Dial_On_Off Dial_Switch;  // 拨盘开关
    int8_t Dial_Gear;         // 拨弹速度档位
    int8_t Dial_Back;         // 退
} Dial_Data_t;

void Dial_Processing(void);
void Normal_Dial(void);
void Back_Dial(void);
void Bullet_Stuck_Processing(void);
void Status_Refresh(void);

extern Dial_Data_t Dial_Data;

#endif /* __DIAL_H */
