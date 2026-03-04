/**
 * @file BSP_BoardCommunication.c
 * @author SJW
 * @brief
 * @version 1.0
 * @date 2026-1-15
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "BSP_BoardCommunication.h"

ControlMessge ControlMes;
ControlMessge_betweenBoard2 ControlMes_board2;

void Board2_1_To_Board2_2(void)
{
  uint8_t data_track[8] = {0};

  data_track[0] = ControlMes_board2.LF_track >> 8;
  data_track[1] = ControlMes_board2.LF_track;
  data_track[2] = ControlMes_board2.LR_track >> 8;
  data_track[3] = ControlMes_board2.LR_track;
  data_track[4] = ControlMes_board2.RR_track >> 8;
  data_track[5] = ControlMes_board2.RR_track;
  data_track[6] = ControlMes_board2.RF_track >> 8;
  data_track[7] = ControlMes_board2.RF_track;
  CAN_SendData(CAN_SendHandle, &hcan2, CAN_ID_STD, CAN_ID_B1_TRACK_DATA, data_track);
}

/**
 * @brief 从Board2_1接收底盘速度信息
 * @param RxMessage 从Board2_1接收的CAN数据
 * @retval None
 */
void Board2_1_getChassisInfo(Can_Export_Data_t RxMessage)
{
  float vx = -(int16_t)(RxMessage.CANx_Export_RxMessage[0] << 8 | RxMessage.CANx_Export_RxMessage[1]);
  float vy = (int16_t)(RxMessage.CANx_Export_RxMessage[2] << 8 | RxMessage.CANx_Export_RxMessage[3]);
  float vw = (int16_t)(RxMessage.CANx_Export_RxMessage[4] << 8 | RxMessage.CANx_Export_RxMessage[5]);
  ControlMes.yaw_velocity = -(int16_t)(RxMessage.CANx_Export_RxMessage[6] << 8 | RxMessage.CANx_Export_RxMessage[7]);
  chassis_control.Speed_ToCloud.vx = vx;
  chassis_control.Speed_ToCloud.vy = vy;
  chassis_control.Speed_ToCloud.wz = -1 * vw;
  if (!ControlMes.AutoAimFlag)
  {
    Gimbal.Target_Yaw += -1 * ControlMes.yaw_velocity * 0.06f;
  }
}

/**
 * @brief 从Board2_1接收云台速度信息
 * @param RxMessage 从Board2_1接收的CAN数据
 * @retval None
 */
void Board2_1_getGimbalInfo(Can_Export_Data_t RxMessage)
{
  static float AutoAim_Offset = 0;
  float yaw_position = (int16_t)(RxMessage.CANx_Export_RxMessage[0] << 8 | RxMessage.CANx_Export_RxMessage[1]);
  ControlMes.shoot_Speed = (uint8_t)RxMessage.CANx_Export_RxMessage[2];
  ControlMes.shoot_Speed /= 2;
  ControlMes.fric_Flag = (uint8_t)(RxMessage.CANx_Export_RxMessage[3] >> 0) & 0x01;
  ControlMes.AutoAimFlag = (uint8_t)(RxMessage.CANx_Export_RxMessage[3] >> 1) & 0x01;
  ControlMes.change_Flag = (uint8_t)(RxMessage.CANx_Export_RxMessage[3] >> 2) & 0x01;
  ControlMes.modelFlag = (uint8_t)(RxMessage.CANx_Export_RxMessage[3] >> 3) & 0x01;

  if (ControlMes.AutoAimFlag == 1)
  {
    if (yaw_position == 0.0f)
    {
      yaw_position = Gimbal.Target_Yaw;
    }
    AutoAim_Offset = -1 * ControlMes.yaw_velocity * 0.03f;
    Gimbal.Target_Yaw = yaw_position + AutoAim_Offset;
  }
  else
  {
    AutoAim_Offset = 0;
  }
}
