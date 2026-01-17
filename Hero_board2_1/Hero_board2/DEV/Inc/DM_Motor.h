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

#define DM_READID_1 0x01
#define DM_SENDID_1 0x001
#define DM_P_MAX 3.1415926535f
#define DM_V_MAX 200.0f
#define DM_T_MAX 10.0f
#define Pi 3.14159265f

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

  int32_t targetSpeed;
  int32_t targetAngle;

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


extern DM_Motor_t DM_Array[1];

#endif /* __DM_MOTOR_H */
