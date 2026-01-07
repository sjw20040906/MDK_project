/**
 * @file DM_Motor.c
 * @author SJW
 * @brief
 * @version 0.1
 * @date 2024-12-20
 *
 * @copyright Copyright (c)
 *
 */
#include "DM_Motor.h"

DM_Motor_t DM_Array[6] = {0};
void DM_setParameter(float uq1, float uq2, float uq3, float uq4, float uq5, uint8_t *data);
void DM_Enable(void);
void DM_Save_Pos_Zero(void);
void DM_getInfo(Can_Export_Data_t RxMessage);
void DM_setTargetAngle(DM_Motor_t *DM, int32_t angle);
void DM_Reset(DM_Motor_t *DM);
void Check_DM(void);

DM_Fun_t DM_Fun = DM_FunGroundInit;
#undef DM_FunGroundInit

/**
 * @brief  uint类型转换为float类型
 * @param
 * @retval None
 */
static float uint_to_float(int X_int, float X_min, float X_max, int Bits)
{
  float span = X_max - X_min;
  float offset = X_min;
  return ((float)X_int) * span / ((float)((1 << Bits) - 1)) + offset;
}

/**
 * @brief  float类型转换为uint类型
 * @param
 * @retval None
 */
static int float_to_uint(float X_float, float X_min, float X_max, int bits)
{
  float span = X_max - X_min;
  float offset = X_min;
  return (int)((X_float - offset) * ((float)((1 << bits) - 1)) / span);
}

/**
 * @brief  设置DM电机参数
 * @param  uq1：角度  uq2：转速  uq3：Kp  uq4：Kd  uq5：转矩
 * @retval None
 */
void DM_setParameter(float uq1, float uq2, float uq3, float uq4, float uq5, uint8_t *data)
{
  float Postion = uq1 / 8192 * 2 * Pi;
  uint16_t Postion_Tmp, Velocity_Tmp, Torque_Tmp, KP_Tmp, KD_Tmp;

  float P_MAX, V_MAX, T_MAX;
  P_MAX = 3.141593f;
  V_MAX = 200.f;
  T_MAX = 7.f;

  Postion_Tmp = float_to_uint(Postion, -P_MAX, P_MAX, 16);
  Velocity_Tmp = float_to_uint(uq2, -V_MAX, V_MAX, 12);
  Torque_Tmp = float_to_uint(uq5, -T_MAX, T_MAX, 12);
  KP_Tmp = float_to_uint(uq3, 0, 500, 12);
  KD_Tmp = float_to_uint(uq4, 0, 5, 12);

  data[0] = (uint8_t)(Postion_Tmp >> 8);
  data[1] = (uint8_t)(Postion_Tmp);
  data[2] = (uint8_t)(Velocity_Tmp >> 4);
  data[3] = (uint8_t)((Velocity_Tmp & 0x0F) << 4) | (uint8_t)(KP_Tmp >> 8);
  data[4] = (uint8_t)(KP_Tmp);
  data[5] = (uint8_t)(KD_Tmp >> 4);
  data[6] = (uint8_t)((KD_Tmp & 0x0F) << 4) | (uint8_t)(Torque_Tmp >> 8);
  data[7] = (uint8_t)(Torque_Tmp);
}

/**
 * @brief  使能DM电机
 * @param
 * @retval None
 */
void DM_Enable()
{
  Can_Send_Data_t Can_Send_Data;

  Can_Send_Data.CAN_TxHeader.StdId = 0x001;
  Can_Send_Data.CAN_TxHeader.IDE = CAN_ID_STD;             // ID类型
  Can_Send_Data.CAN_TxHeader.RTR = CAN_RTR_DATA;           // 发送的为数据
  Can_Send_Data.CAN_TxHeader.DLC = 0x08;                   // 数据长度为8字节
  Can_Send_Data.CAN_TxHeader.TransmitGlobalTime = DISABLE; // 不发送全局时间

  Can_Send_Data.CANx_Send_RxMessage[0] = 0xFF;
  Can_Send_Data.CANx_Send_RxMessage[1] = 0xFF;
  Can_Send_Data.CANx_Send_RxMessage[2] = 0xFF;
  Can_Send_Data.CANx_Send_RxMessage[3] = 0xFF;
  Can_Send_Data.CANx_Send_RxMessage[4] = 0xFF;
  Can_Send_Data.CANx_Send_RxMessage[5] = 0xFF;
  Can_Send_Data.CANx_Send_RxMessage[6] = 0xFF;
  Can_Send_Data.CANx_Send_RxMessage[7] = 0xFC;

  Can_Fun.CAN_SendData(CAN_SendHandle, &hcan2, CAN_ID_STD, 0x001, Can_Send_Data.CANx_Send_RxMessage);
}
/**
 * @brief  重新设置DM电机零点
 * @param
 * @retval None
 */
void DM_Save_Pos_Zero(void)
{
  Can_Send_Data_t Can_Send_Data;

  Can_Send_Data.CAN_TxHeader.StdId = 0x001;
  Can_Send_Data.CAN_TxHeader.IDE = CAN_ID_STD;             // ID类型
  Can_Send_Data.CAN_TxHeader.RTR = CAN_RTR_DATA;           // 发送的为数据
  Can_Send_Data.CAN_TxHeader.DLC = 0x08;                   // 数据长度为8字节
  Can_Send_Data.CAN_TxHeader.TransmitGlobalTime = DISABLE; // 不发送全局时间
  Can_Send_Data.CANx_Send_RxMessage[0] = 0xFF;
  Can_Send_Data.CANx_Send_RxMessage[1] = 0xFF;
  Can_Send_Data.CANx_Send_RxMessage[2] = 0xFF;
  Can_Send_Data.CANx_Send_RxMessage[3] = 0xFF;
  Can_Send_Data.CANx_Send_RxMessage[4] = 0xFF;
  Can_Send_Data.CANx_Send_RxMessage[5] = 0xFF;
  Can_Send_Data.CANx_Send_RxMessage[6] = 0xFF;
  Can_Send_Data.CANx_Send_RxMessage[7] = 0xFE;

  Can_Fun.CAN_SendData(CAN_SendHandle, &hcan2, CAN_ID_STD, 0x001, Can_Send_Data.CANx_Send_RxMessage);
}

/**
 * @brief  从CAN报文中获取DM电机信息
 * @param  RxMessage 	CAN报文接收结构体
 * @retval None
 */

void DM_getInfo(Can_Export_Data_t RxMessage)
{
  int32_t StdId;
  StdId = (RxMessage.CANx_Export_RxMessage[0]) & 0x0F;
  StdId = (int32_t)RxMessage.CAN_RxHeader.StdId - J4310_READID_PITCH; // 由0开始
  float P_MAX, V_MAX, T_MAX;

  P_MAX = 12.5f;
  V_MAX = 200.f;
  T_MAX = 7.f; // J4310电机数据

  DM_Array[StdId].lastAngle = DM_Array[StdId].realAngle;
  DM_Array[StdId].state = RxMessage.CANx_Export_RxMessage[0] >> 4;
  DM_Array[StdId].angleInit = (uint16_t)((RxMessage.CANx_Export_RxMessage[1] << 8) | RxMessage.CANx_Export_RxMessage[2]);
  DM_Array[StdId].speedInit = (uint16_t)((RxMessage.CANx_Export_RxMessage[3] << 4) | (RxMessage.CANx_Export_RxMessage[4] >> 4));
  DM_Array[StdId].torqueInit = (uint16_t)((RxMessage.CANx_Export_RxMessage[4] & 0xF << 8) | RxMessage.CANx_Export_RxMessage[5]);
  DM_Array[StdId].realAngle = uint_to_float(DM_Array[StdId].angleInit, -P_MAX, P_MAX, 16);
  DM_Array[StdId].realAngle = DM_Array[StdId].realAngle / 2 * 3.1415927f * 36.f;
  DM_Array[StdId].realSpeed = uint_to_float(DM_Array[StdId].speedInit, -V_MAX, V_MAX, 12);
  DM_Array[StdId].torque = uint_to_float(DM_Array[StdId].torqueInit, -T_MAX, T_MAX, 12);
  DM_Array[StdId].temperatureMOS = (float)(RxMessage.CANx_Export_RxMessage[6]);
  DM_Array[StdId].temperatureRotor = (float)(RxMessage.CANx_Export_RxMessage[7]);

  if (DM_Array[StdId].realAngle - DM_Array[StdId].lastAngle < -6500)
  {
    DM_Array[StdId].turnCount++;
  }

  if (DM_Array[StdId].lastAngle - DM_Array[StdId].realAngle < -6500)
  {
    DM_Array[StdId].turnCount--;
  }

  DM_Array[StdId].totalAngle = DM_Array[StdId].realAngle + (8192 * DM_Array[StdId].turnCount);
  // 帧率统计，数据更新标志位
  DM_Array[StdId].InfoUpdateFrame++;
  DM_Array[StdId].InfoUpdateFlag = 1;
}

/*
 *@brief  设定J4310电机的目标角度
 * @param  motorName 	电机名字 @ref M6623Name_e
 *			angle		电流值，范围 0~8191 由于设置0和8191会导致电机振荡，要做个限幅
 * @retval None
 * */
void DM_setTargetAngle(DM_Motor_t *DM, int32_t angle)
{
  DM->targetAngle = angle;
}

/*************************************
 * Method:    DM_OverflowReset
 * Returns:   void
 * 说明：调运此函数以解决totalAngle 等溢出的问题。
 ************************************/
void DM_Reset(DM_Motor_t *DM)
{
  // 解包数据，数据格式详见C620电调说明书P33
  DM->lastAngle = DM->realAngle;
  DM->totalAngle = DM->realAngle;
  DM->turnCount = 0;
}

/**
 * @brief DM检测
 *
 */
void Check_DM(void)
{
#if (USING_BOARD == BOARD2)
  for (int i = 0; i < 1; i++)
  {
    if (DM_Array[i].InfoUpdateFrame < 1)
    {
      DM_Array[i].OffLineFlag = 1;
    }
    else
    {
      DM_Array[i].OffLineFlag = 0;
    }
    DM_Array[i].InfoUpdateFrame = 0;
  }
#else
  for (int i = 1; i < 2; i++)
  {
    if (DM_Array[i]->InfoUpdateFrame < 1)
    {
      DM_Array[i]->OffLineFlag = 1;
    }
    else
    {
      DM_Array[i]->OffLineFlag = 0;
    }
    DM_Array[i]->InfoUpdateFrame = 0;
  }
#endif
}
