/**
 ******************************************************************************
 * @file    wit_imu.h
 * @author  SJW
 * @version V1.0.0
 * @date    2026-01-15
 * @brief   IMU数据处理模块头文件
 *          该文件包含IMU数据结构定义、宏定义和IMU数据处理与初始化的函数声明
 ******************************************************************************
 */

#ifndef __WIT_IMU_H
#define __WIT_IMU_H

#include "main.h"
#include "usart.h"

/* IMU数据类型定义 */
#define WIT_ACC 0x51   /* 加速度数据类型 */
#define WIT_GYRO 0x52  /* 陀螺仪数据类型 */
#define WIT_ANGLE 0x53 /* 角度数据类型 */

typedef struct
{
    float RoLL;
    float Pitch;
    float Yaw;
    float X_Vel;
    float Y_Vel;
    float Z_Vel;
} IMU_Angle_t;

/**
 * @brief  IMU数据结构体
 * @note   该结构体包含加速度、陀螺仪和角度数据
 */
typedef struct
{
    float acc[3];   /* 加速度数据 (x, y, z) 单位：g */
    float gyro[3];  /* 陀螺仪数据 (x, y, z) 单位：deg/s */
    float angle[3]; /* 角度数据 (roll, pitch, yaw) 单位：deg */
} IMU_Data_t;

/* 导出变量 */
extern IMU_Data_t imu_data;
extern uint8_t IMU_RxRawBuffer[64];
extern IMU_Angle_t IMU_Angle;

/* 导出函数声明 */
void IMU_Init(void);
void IMU_ProcessData(uint8_t *pData, uint16_t Size);

#endif /* __WIT_IMU_H */
