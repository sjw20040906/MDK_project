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

ControlMessge_betweenBoard2 ControlMes_board2;

void Board2_2_getTrackInfo(Can_Export_Data_t RxMessage)
{
  ControlMes_board2.LF_track = (int16_t)(RxMessage.CANx_Export_RxMessage[0] << 8 | RxMessage.CANx_Export_RxMessage[1]);
  ControlMes_board2.LR_track = (int16_t)(RxMessage.CANx_Export_RxMessage[2] << 8 | RxMessage.CANx_Export_RxMessage[3]);
  ControlMes_board2.RR_track = (int16_t)(RxMessage.CANx_Export_RxMessage[4] << 8 | RxMessage.CANx_Export_RxMessage[5]);
  ControlMes_board2.RF_track = (int16_t)(RxMessage.CANx_Export_RxMessage[6] << 8 | RxMessage.CANx_Export_RxMessage[7]);
}
