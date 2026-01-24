/**
 * @file DM_Motor.h
 * @author SJW
 * @brief
 * @version 0.1
 * @date 2024-12-20
 *
 * @copyright Copyright (c)
 *
 */
#ifndef __DM_MOTOR_H
#define __DM_MOTOR_H

#include "typedef.h"
#include "Task_CanReceive.h"
#include "stm32f4xx_hal_can.h"

enum DM_Motor
{
  LF = 0,
  LR = 1,
  RR = 2,
  RF = 3,
  totalMotor = 4,
};

enum DM_Motor_ID
{
  DM_LF = 0x001,
  DM_LR = 0x002,
  DM_RR = 0x003,
  DM_RF = 0x004,
};

enum DM_Motor_ReadID
{
  DM_LF_ReadID = 0x01,
  DM_LR_ReadID = 0x02,
  DM_RR_ReadID = 0x03,
  DM_RF_ReadID = 0x04,
};

#define DM_P_MAX 3.14159f
#define DM_V_MAX 200.0f
#define DM_T_MAX 10.0f
#define Pi 3.14159265f
#define DM_KP 20.0f          // 范围0-500
#define DM_KD 1.0f           // 范围0-5
#define DM_Torque 3.0f       // 额定扭矩
#define DM_Speed 15.0f       // 速度
#define DM_Offset -0.87f     // 偏移量
#define DM_Offset_temp -2.0f  // 临时偏移量

typedef struct
{
  int16_t state;
  float realAngle;
  float realSpeed;
  uint8_t temperatureMOS;
  uint8_t temperatureRotor;
  float torqueInit;
  float torque;
  uint32_t angleInit;
  float speedInit;

  uint32_t lastAngle;

  float outPosition;
  float outSpeed;
  float outTorque;

  int16_t turnCount;
  float totalAngle;

  int8_t outKp;
  int8_t outKd;

  uint8_t InfoUpdateFlag;
  uint16_t InfoUpdateFrame;
  uint8_t OffLineFlag;
} DM_Motor_t;

void DM_setParameter(float uq1, float uq2, float uq3, float uq4, float uq5, uint8_t *data);
void DM_Enable(CAN_HandleTypeDef *hcanx, uint32_t id);
void DM_Save_Pos_Zero(CAN_HandleTypeDef *hcanx, uint32_t id);
void DM_getInfo(Can_Export_Data_t RxMessage);
void DM_Reset(DM_Motor_t *DM);

extern DM_Motor_t DM_Array[totalMotor];

#endif
