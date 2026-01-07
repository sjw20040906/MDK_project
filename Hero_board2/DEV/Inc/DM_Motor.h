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

#define J4310_READID_PITCH 0x01
#define J4310_SENDID_Pitch 0x001
#define J4310_MaxV 200
#define J4310_MaxT 7
#define J4310_ReductionRatio 10

#define J4310_FIRSTANGLE 3800

#define J4310_mAngleRatio 22.7527f /
#define Pi 3.14159265f

#define J4310_getRoundAngle(rAngle) rAngle / J4310_mAngleRatio

#define DM_FunGroundInit  \
  {                          \
      &DM_setParameter,   \
      &DM_Enable,         \
      &DM_Save_Pos_Zero,  \
      &DM_getInfo,        \
      &DM_setTargetAngle, \
      &DM_Reset,          \
      &Check_DM,          \
  }

typedef struct
{
  int16_t state;
  float realAngle;
  float realSpeed;
  uint8_t temperatureMOS;
  uint8_t temperatureRotor;
  float torqueInit;
  float torque;
  float angleInit;
  float speedInit;

  uint16_t lastAngle;

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


typedef enum
{
  J4310_PITCH = 0,
} J4310Name_e;

typedef struct
{
  void (*DM_setParameter)(float uq1, float uq2, float uq3, float uq4, float uq5, uint8_t *data);
  void (*DM_Enable)(void);
  void (*DM_Save_Pos_Zero)(void);
  void (*DM_getInfo)(Can_Export_Data_t RxMessage);
  void (*DM_setTargetAngle)(DM_Motor_t *DM, int32_t angle);
  void (*DM_Reset)(DM_Motor_t *DM);
  void (*Check_DM)(void);
} DM_Fun_t;

extern DM_Motor_t DM_Array[6];
extern DM_Fun_t DM_Fun;

#endif /* __DM_MOTOR_H */
