/**
 * @file J3519_Motor.c
 * @author SJW
 * @brief
 * @version 0.1
 * @date 2025-9-26
 *
 * @copyright Copyright (c)
 *
 */
#include "J3519_Motor.h"

// 直接声明对应的电机的结构体而不用数组，直观便于后期调试观察数据使用。
J3519s_t J3519_Array[1] = {0};
void J3519_setParameter(float uq1, float uq2, float uq3, float uq4, float uq5, uint8_t *data);
void J3519_Enable(CAN_HandleTypeDef *hcanx, uint32_t id);
void J3519_Save_Pos_Zero(void);
void J3519_getInfo(Can_Export_Data_t RxMessage);
void J3519_Reset(J3519s_t *J3519);
void Check_J3519(void);

J3519_Fun_t J3519_Fun = J3519_FunGroundInit;
#undef J3519_FunGroundInit

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
 * @brief 将浮点数映射到整型
 *
 * @param x 浮点数
 * @param Float_1 浮点数1
 * @param Float_2 浮点数2
 * @param Int_1 整型1
 * @param Int_2 整型2
 * @return int32_t 整型
 */
int32_t Math_Float_To_Int(float x, float Float_1, float Float_2, int32_t Int_1, int32_t Int_2)
{
  float tmp = (x - Float_1) / (Float_2 - Float_1);
  int32_t out = tmp * (float)(Int_2 - Int_1) + Int_1;
  return (out);
}

/**
 * @brief 将整型映射到浮点数
 *
 * @param x 整型
 * @param Int_1 整型1
 * @param Int_2 整型2
 * @param Float_1 浮点数1
 * @param Float_2 浮点数2
 * @return float 浮点数
 */
float Math_Int_To_Float(int32_t x, int32_t Int_1, int32_t Int_2, float Float_1, float Float_2)
{
  float tmp = (float)(x - Int_1) / (float)(Int_2 - Int_1);
  float out = tmp * (Float_2 - Float_1) + Float_1;
  return (out);
}

/**
 * @brief 16位大小端转换
 *
 * @param Source 源数据地址
 * @param Destination 目标存储地址
 * @return uint16_t 结果
 */
uint16_t Math_Endian_Reverse_16(void *Source, void *Destination)
{
  uint8_t *temp_address_8 = (uint8_t *)Source;
  uint16_t temp_address_16;
  temp_address_16 = temp_address_8[0] << 8 | temp_address_8[1];

  if (Destination != NULL)
  {
    uint8_t *temp_source, *temp_destination;
    temp_source = (uint8_t *)Source;
    temp_destination = (uint8_t *)Destination;

    temp_destination[0] = temp_source[1];
    temp_destination[1] = temp_source[0];
  }

  return temp_address_16;
}

/**
 * @brief  设置J3519电机参数（id号为8）
 * @param  uq1：角度  uq2：转速  uq3：Kp  uq4：Kd  uq5：转矩
 * @retval None
 */
void J3519_setParameter(float uq1, float uq2, float uq3, float uq4, float uq5, uint8_t *data)
{
  uint16_t pos_tmp, vel_tmp, kp_tmp, kd_tmp, tor_tmp;

  pos_tmp = float_to_uint(uq1, -J3519_P_MAX, J3519_P_MAX, 16);
  vel_tmp = float_to_uint(uq2, -J3519_V_MAX, J3519_V_MAX, 12);
  tor_tmp = float_to_uint(uq5, -J3519_T_MAX, J3519_T_MAX, 12);
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
 * @brief  使能J3519电机
 * @param
 * @retval None
 */
void J3519_Enable(CAN_HandleTypeDef *hcanx, uint32_t id)
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

  Can_Fun.CAN_SendData(CAN_SendHandle, hcanx, CAN_ID_STD, id, Can_Send_Data.CANx_Send_RxMessage);
}
/**
 * @brief  重新设置3519电机零点
 * @param
 * @retval None
 */
void J3519_Save_Pos_Zero(void)
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

  Can_Fun.CAN_SendData(CAN_SendHandle, &hcan1, CAN_ID_STD, 0x001, Can_Send_Data.CANx_Send_RxMessage);
}

/**
 * @brief  从CAN报文中获取J3519电机信息
 * @param  RxMessage 	CAN报文接收结构体
 * @retval None
 */

void J3519_getInfo(Can_Export_Data_t RxMessage)
{
  int32_t StdId;
  StdId = (RxMessage.CANx_Export_RxMessage[0]) & 0x0F;
  StdId = (int32_t)RxMessage.CAN_RxHeader.StdId - J3519_READID_1; // 由0开始

  J3519_Array[StdId].lastAngle = J3519_Array[StdId].angleInit;
  J3519_Array[StdId].state = RxMessage.CANx_Export_RxMessage[0] >> 4;
  J3519_Array[StdId].angleInit = ((RxMessage.CANx_Export_RxMessage[1] << 8) | RxMessage.CANx_Export_RxMessage[2]);
  J3519_Array[StdId].speedInit = ((RxMessage.CANx_Export_RxMessage[3] << 4) | (RxMessage.CANx_Export_RxMessage[4] >> 4));
  J3519_Array[StdId].torqueInit = ((RxMessage.CANx_Export_RxMessage[4] & 0xF << 8) | RxMessage.CANx_Export_RxMessage[5]);
  float delta = J3519_Array[StdId].angleInit - J3519_Array[StdId].lastAngle;
  if (delta < -(1 << 15))
  {
    J3519_Array[StdId].turnCount++;
  }
  else if (delta > (1 << 15))
  {
    J3519_Array[StdId].turnCount--;
  }
  J3519_Array[StdId].realAngle = uint_to_float(J3519_Array[StdId].angleInit, -J3519_P_MAX, J3519_P_MAX, 16);
  J3519_Array[StdId].realSpeed = uint_to_float(J3519_Array[StdId].speedInit, -J3519_V_MAX, J3519_V_MAX, 12);
  J3519_Array[StdId].torque = uint_to_float(J3519_Array[StdId].torqueInit, -J3519_T_MAX, J3519_T_MAX, 12);
  J3519_Array[StdId].temperatureMOS = (RxMessage.CANx_Export_RxMessage[6]);
  J3519_Array[StdId].temperatureRotor = (RxMessage.CANx_Export_RxMessage[7]);
  J3519_Array[StdId].totalAngle = J3519_Array[StdId].realAngle + (2 * J3519_P_MAX * J3519_Array[StdId].turnCount);
  // 帧率统计，数据更新标志位
  J3519_Array[StdId].InfoUpdateFrame++;
  J3519_Array[StdId].InfoUpdateFlag = 1;
}


/*************************************
 * Method:    J3519_OverflowReset
 * Returns:   void
 * 说明：调运此函数以解决totalAngle 等溢出的问题。
 ************************************/
void J3519_Reset(J3519s_t *J3519)
{
  // 解包数据，数据格式详见C620电调说明书P33
  J3519->lastAngle = J3519->realAngle;
  J3519->totalAngle = J3519->realAngle;
  J3519->turnCount = 0;
}

/**
 * @brief 3519检测
 *
 */
void Check_J3519(void)
{
#if (USING_BOARD == BOARD2)
  // J3159检测
  for (int i = 0; i < 1; i++)
  {
    if (J3519_Array[i].InfoUpdateFrame < 1)
    {
      J3519_Array[i].OffLineFlag = 1;
    }
    else
    {
      J3519_Array[i].OffLineFlag = 0;
    }
    J3519_Array[i].InfoUpdateFrame = 0;
  }
#else
  for (int i = 0; i < 1; i++)
  {
    if (J3519_Array[i].InfoUpdateFrame < 1)
    {
      J3519_Array[i].OffLineFlag = 1;
    }
    else
    {
      J3519_Array[i].OffLineFlag = 0;
    }
    J3519_Array[i].InfoUpdateFrame = 0;
  }
#endif
}
