/**
 * @file Gimbal.h
 * @author SJW
 * @brief 云台
 * @version 1.0
 * @date 2026-01-27
 * @copyright
 */

#ifndef GIMBAL_H
#define GIMBAL_H

#include "DM_Motor.h"
#include "wit_imu.h"
#include "pid.h"

/* 云台目标角度结构体(仅针对YAW) */
typedef struct
{
    float Yaw_Raw;          // yaw的原始数据
    float Target_Yaw;       // 云台目标yaw值
    float Vision_Yaw_Delta; // 视觉Yaw偏差(增量)
} Gimbal_t;

void Gimbal_Init(void);
void Gimbal_motion_Control(void);

#endif
