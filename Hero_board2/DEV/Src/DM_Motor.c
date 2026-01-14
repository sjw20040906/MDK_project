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
  uint16_t pos_tmp, vel_tmp, kp_tmp, kd_tmp, tor_tmp;

  pos_tmp = float_to_uint(uq1, -DM_P_MAX, DM_P_MAX, 16);
  vel_tmp = float_to_uint(uq2, -DM_V_MAX, DM_V_MAX, 12);
  tor_tmp = float_to_uint(uq5, -DM_T_MAX, DM_T_MAX, 12);
  kp_tmp = float_to_uint(uq3, 0, 500, 12);
  kd_tmp = float_to_uint(uq4, 0, 5, 12);
  data[0] = (pos_tmp >> 8);
  data[1] = pos_tmp;
  data[2] = (vel_tmp >> 4);
  data[3] = ((vel_tmp & 0xF) << 4) | (kp_tmp >> 8);
  data[4] = kp_tmp;
  data[5] = (kd_tmp >> 4);
  data[6] = ((kd_tmp & 0xF) << 4) | (tor_tmp >> 8);
  data[7] = tor_tmp;
}

/**
 * @brief  使能DM电机
 * @param
 * @retval None
 */
void DM_Enable(CAN_HandleTypeDef *hcanx, uint32_t id)
{
  Can_Send_Data_t Can_Send_Data;

  Can_Send_Data.CAN_TxHeader.StdId = id;
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

  CAN_SendData(CAN_SendHandle, hcanx, CAN_ID_STD, id, Can_Send_Data.CANx_Send_RxMessage);
}
/**
 * @brief  重新设置DM电机零点
 * @param
 * @retval None
 */
void DM_Save_Pos_Zero(CAN_HandleTypeDef *hcanx, uint32_t id)
{
  Can_Send_Data_t Can_Send_Data;

  Can_Send_Data.CAN_TxHeader.StdId = id;
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

  CAN_SendData(CAN_SendHandle, hcanx, CAN_ID_STD, id, Can_Send_Data.CANx_Send_RxMessage);
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
  StdId = (int32_t)RxMessage.CAN_RxHeader.StdId - DM_READID_1;

  DM_Array[StdId].lastAngle = DM_Array[StdId].angleInit;
  DM_Array[StdId].state = RxMessage.CANx_Export_RxMessage[0] >> 4;
  DM_Array[StdId].angleInit = ((RxMessage.CANx_Export_RxMessage[1] << 8) | RxMessage.CANx_Export_RxMessage[2]);
  DM_Array[StdId].speedInit = ((RxMessage.CANx_Export_RxMessage[3] << 4) | (RxMessage.CANx_Export_RxMessage[4] >> 4));
  DM_Array[StdId].torqueInit = ((RxMessage.CANx_Export_RxMessage[4] & 0xF << 8) | RxMessage.CANx_Export_RxMessage[5]);
  uint32_t delta = DM_Array[StdId].angleInit - DM_Array[StdId].lastAngle;
  if (delta < -(1 << 15))
  {
    DM_Array[StdId].turnCount++;
  }
  else if (delta > (1 << 15))
  {
    DM_Array[StdId].turnCount--;
  }
  DM_Array[StdId].realAngle = uint_to_float(DM_Array[StdId].angleInit, -DM_P_MAX, DM_P_MAX, 16);
  DM_Array[StdId].realSpeed = uint_to_float(DM_Array[StdId].speedInit, -DM_V_MAX, DM_V_MAX, 12);
  DM_Array[StdId].torque = uint_to_float(DM_Array[StdId].torqueInit, -DM_T_MAX, DM_T_MAX, 12);
  DM_Array[StdId].temperatureMOS = (RxMessage.CANx_Export_RxMessage[6]);
  DM_Array[StdId].temperatureRotor = (RxMessage.CANx_Export_RxMessage[7]);
  DM_Array[StdId].totalAngle = DM_Array[StdId].realAngle + (2 * DM_P_MAX * DM_Array[StdId].turnCount);
  // 帧率统计，数据更新标志位
  DM_Array[StdId].InfoUpdateFrame++;
  DM_Array[StdId].InfoUpdateFlag = 1;
}

/*
 *@brief  设定DM电机的目标角度
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
  DM->lastAngle = DM->realAngle;
  DM->totalAngle = DM->realAngle;
  DM->turnCount = 0;
}
