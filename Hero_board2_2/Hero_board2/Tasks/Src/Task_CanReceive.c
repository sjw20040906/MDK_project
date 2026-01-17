/**
 * @file Task_CanMsg.c
 * @author SJW
 * @brief
 * @version 0.1
 * @date 2021-03-30
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "Task_CanReceive.h"

int16_t DM_RxID;
/**
 * @Data   2021-03-28
 * @brief  can1接收任务
 * @param  void
 * @retval void
 */
void Can1Receives(void const *argument)
{
  Can_Export_Data_t Can_Export_Data;
  DM_RxID = (Can_Export_Data.CANx_Export_RxMessage[0]) & 0x0F;
  for (;;)
  {
    xQueueReceive(CAN1_ReceiveHandle, &Can_Export_Data, portMAX_DELAY);
    if (DM_RxID == DM_LF_ReadID || DM_RxID == DM_LR_ReadID || DM_RxID == DM_RR_ReadID || DM_RxID == DM_RF_ReadID)
    {
      DM_getInfo(Can_Export_Data);
    }
  }
}

/**
 * @Data   2021-03-28
 * @brief  can2接收任务
 * @param  void
 * @retval void
 */
void Can2Receives(void const *argument)
{
  Can_Export_Data_t Can_Export_Data;
  uint32_t ID;
  for (;;)
  {
    xQueueReceive(CAN2_ReceiveHandle, &Can_Export_Data, portMAX_DELAY);
    ID = Can_Export_Data.CAN_RxHeader.StdId;
    DM_RxID = (Can_Export_Data.CANx_Export_RxMessage[0]) & 0x0F;
    if (ID == CAN_ID_B2_TRACK_DATA || ID == CAN_ID_B2_FLAG_DATA)
    {
      Board2_2_getTrackInfo(Can_Export_Data);
    }
  }
}
