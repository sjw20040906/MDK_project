/**
 ******************************************************************************
 * @file    imu760.h
 * @author  OpenAI
 * @version V1.0.0
 * @date    2026-03-25
 * @brief   IMU760数据处理模块头文件
 *          该文件包含IMU760数据结构定义、宏定义和数据处理/配置函数声明
 ******************************************************************************
 */

#ifndef __IMU760_H
#define __IMU760_H

#include "main.h"
#include "usart.h"
#include <string.h>
#include "wit_imu.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 用户可根据工程实际情况修改此宏 */
#define IMU760_UART_HANDLE              huart1

/* 接收缓冲区大小 */
#define IMU760_RX_RAW_BUFFER_SIZE       256U
#define IMU760_FRAME_BUFFER_SIZE        512U

/* IMU760输出协议数据ID */
#define IMU760_ID_TEMP                  0x01U
#define IMU760_ID_ACC                   0x10U
#define IMU760_ID_GYRO                  0x20U
#define IMU760_ID_MAG_NORM              0x30U
#define IMU760_ID_MAG_RAW               0x31U
#define IMU760_ID_EULER                 0x40U
#define IMU760_ID_QUAT                  0x41U
#define IMU760_ID_SAMPLE_TIMESTAMP      0x51U
#define IMU760_ID_DATAREADY_TIMESTAMP   0x52U

/* IMU760交互协议：操作符 */
#define IMU760_OP_QUERY                 0x00U
#define IMU760_OP_RAM                   0x01U
#define IMU760_OP_FLASH                 0x02U

/* IMU760交互协议：数据类 */
#define IMU760_CLASS_BAUDRATE           0x02U
#define IMU760_CLASS_OUTPUT_RATE        0x03U
#define IMU760_CLASS_OUTPUT_CONTENT     0x04U
#define IMU760_CLASS_FUNCTION_MODE      0x4DU

/* 输出频率枚举 */
#define IMU760_RATE_1HZ                 0x01U
#define IMU760_RATE_2HZ                 0x02U
#define IMU760_RATE_5HZ                 0x03U
#define IMU760_RATE_10HZ                0x04U
#define IMU760_RATE_20HZ                0x05U
#define IMU760_RATE_25HZ                0x06U
#define IMU760_RATE_50HZ                0x07U
#define IMU760_RATE_100HZ               0x08U
#define IMU760_RATE_200HZ               0x09U
#define IMU760_RATE_400HZ               0x0AU

/* 算法模式 */
#define IMU760_MODE_AHRS                0x01U
#define IMU760_MODE_VRU                 0x02U
#define IMU760_MODE_IMU                 0x03U

/* 输出内容位定义 */
#define IMU760_OUTPUT_TIMESTAMP         (1U << 15)
#define IMU760_OUTPUT_STATUS            (1U << 11)
#define IMU760_OUTPUT_TEMP              (1U << 10)
#define IMU760_OUTPUT_ACC               (1U << 7)
#define IMU760_OUTPUT_GYRO              (1U << 6)
#define IMU760_OUTPUT_MAG_RAW           (1U << 5)
#define IMU760_OUTPUT_EULER             (1U << 4)
#define IMU760_OUTPUT_QUAT              (1U << 3)
#define IMU760_OUTPUT_UTC               (1U << 2)
#define IMU760_OUTPUT_POSITION          (1U << 1)
#define IMU760_OUTPUT_VELOCITY          (1U << 0)

/* 数据更新标志位 */
#define IMU760_UPDATE_TEMP              (1UL << 0)
#define IMU760_UPDATE_ACC               (1UL << 1)
#define IMU760_UPDATE_GYRO              (1UL << 2)
#define IMU760_UPDATE_MAG_NORM          (1UL << 3)
#define IMU760_UPDATE_MAG_RAW           (1UL << 4)
#define IMU760_UPDATE_EULER             (1UL << 5)
#define IMU760_UPDATE_QUAT              (1UL << 6)
#define IMU760_UPDATE_SAMPLE_TS         (1UL << 7)
#define IMU760_UPDATE_DATAREADY_TS      (1UL << 8)

/**
 * @brief  IMU760数据结构体
 * @note   欧拉角顺序与协议保持一致：pitch, roll, yaw
 */
typedef struct
{
    float temperature;              /* IMU温度，单位：℃ */
    float acc[3];                   /* 加速度，单位：m/s^2 */
    float gyro[3];                  /* 角速度，单位：deg/s */
    float mag_norm[3];              /* 磁场归一化值，无量纲 */
    float mag_raw[3];               /* 磁场强度，单位：mGauss */
    float euler[3];                 /* 欧拉角，顺序：pitch/roll/yaw，单位：deg */
    float quat[4];                  /* 四元数，顺序：q0/q1/q2/q3 */
    uint32_t sample_timestamp_us;   /* 采样时间戳，单位：us */
    uint32_t dataready_timestamp_us;/* Dataready时间戳，单位：us */
    uint16_t tid;                   /* 最近一帧帧序号 */
    uint32_t update_flag;           /* 数据更新标志 */
    uint32_t update_tick;           /* 最近一次收到有效数据的HAL_GetTick()时间 */
} IMU760_Data_t;

/* 导出变量 */
extern IMU760_Data_t imu760_data;
extern uint8_t imu760_rx_raw_buffer[IMU760_RX_RAW_BUFFER_SIZE];

/* 导出函数声明 */
void IMU760_Init(void);
void IMU760_ProcessData(uint8_t *pData, uint16_t Size);
void IMU760_ClearUpdateFlag(void);

HAL_StatusTypeDef IMU760_SetOutputRate(uint8_t rate, uint8_t save_mode);
HAL_StatusTypeDef IMU760_SetOutputMask(uint16_t mask, uint8_t save_mode);
HAL_StatusTypeDef IMU760_SetAlgorithmMode(uint8_t mode, uint8_t save_mode);

#ifdef __cplusplus
}
#endif

#endif /* __IMU760_H */
