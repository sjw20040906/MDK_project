/**
 * @file J3519_Motor.h
 * @author MOON
 * @brief
 * @version 0.1
 * @date 2025-9-27
 *
 * @copyright Copyright (c)
 *
 */
#ifndef __J3519_MOTOR_H
#define __J3519_MOTOR_H

#include "can.h"
#include "main.h"
#include "typedef.h"
#include "Task_CanReceive.h"
#include "BSP_Can.h"

#define J3519_READID 0x01
#define J3519_SENDID 0x001
#define J3519_P_MAX 3.1415926535f
#define J3519_V_MAX 200.0f
#define J3519_T_MAX 10.0f
#define J3519_ReductionRatio 10
#define J3519_FIRSTANGLE 3800
#define J3519_mAngleRatio 22.7527f /
#define J3519_getRoundAngle(rAngle) rAngle / J3519_mAngleRatio

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
  int32_t turnCount;
  float totalAngle;
  int8_t outKp;
  int8_t outKd;
  uint8_t InfoUpdateFlag;
  uint32_t InfoUpdateFrame;
  uint8_t OffLineFlag;
} J3519s_t;

extern J3519s_t J3519_Array[1];

typedef enum
{
  J3519_Dail_Wheel = 0,
} J3519Name_e;

void J3519_setParameter(float uq1, float uq2, float uq3, float uq4, float uq5, uint8_t *data);
void J3519_Enable(CAN_HandleTypeDef *hcanx, uint32_t id);
void J3519_Save_Pos_Zero(void);
void J3519_getInfo(Can_Export_Data_t RxMessage);
void J3519_Reset(J3519s_t *J3519);
void Check_J3519(void);

#endif
