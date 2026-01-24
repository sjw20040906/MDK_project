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

void Board1_getGimbalInfo(Can_Export_Data_t RxMessage)
{
  ControlMes.yaw_realAngle = (int16_t)(RxMessage.CANx_Export_RxMessage[0] << 8 | RxMessage.CANx_Export_RxMessage[1]);
  ControlMes.Blood_Volume = (int16_t)(RxMessage.CANx_Export_RxMessage[2] << 8 | RxMessage.CANx_Export_RxMessage[3]);
  ControlMes.Speed_Bullet = (int16_t)(RxMessage.CANx_Export_RxMessage[4] << 8 | RxMessage.CANx_Export_RxMessage[5]);
  ControlMes.Speed_Bullet /= 1000;
  ControlMes.tnndcolor = (uint8_t)(RxMessage.CANx_Export_RxMessage[6] >> 0) & 0x01;
  ControlMes.game_start = (uint8_t)(RxMessage.CANx_Export_RxMessage[6] >> 1) & 0x01;
}
