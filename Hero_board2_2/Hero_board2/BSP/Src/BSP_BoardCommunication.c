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

// void Board2_2_To_Board2_1(void)
// {
//   uint8_t data[8] = {0};
//   uint8_t data2_Fun[8] = {0};
//   // 数据发送
//   CAN_SendData(CAN_SendHandle, &hcan2, CAN_ID_STD, CAN_ID_CHASSIS, data);
//   // 数据发送
//   CAN_SendData(CAN_SendHandle, &hcan2, CAN_ID_STD, CAN_ID_GIMBAL, data2_Fun);
// }

void Board2_2_getTrackInfo(Can_Export_Data_t RxMessage)
{
  switch (RxMessage.CAN_RxHeader.StdId)
  {
  case CAN_ID_B2_TRACK_DATA:
    ControlMes_board2.LF_track = (uint16_t)(RxMessage.CANx_Export_RxMessage[0] << 8 | RxMessage.CANx_Export_RxMessage[1]);
    ControlMes_board2.LR_track = (uint16_t)(RxMessage.CANx_Export_RxMessage[2] << 8 | RxMessage.CANx_Export_RxMessage[3]);
    ControlMes_board2.RR_track = (uint16_t)(RxMessage.CANx_Export_RxMessage[4] << 8 | RxMessage.CANx_Export_RxMessage[5]);
    ControlMes_board2.RF_track = (uint16_t)(RxMessage.CANx_Export_RxMessage[6] << 8 | RxMessage.CANx_Export_RxMessage[7]);
    break;
  case CAN_ID_B2_FLAG_DATA: // 解析标志位 (0x251)
    ControlMes_board2.upstairsFlag = RxMessage.CANx_Export_RxMessage[0];
    break;
  default:
    break;
  }
}
