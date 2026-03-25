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
  uint32_t ID;
  for (;;)
  {
    xQueueReceive(CAN1_ReceiveHandle, &Can_Export_Data, portMAX_DELAY);
    DM_RxID = (Can_Export_Data.CANx_Export_RxMessage[0]) & 0x0F;
    ID = Can_Export_Data.CAN_RxHeader.StdId;
    if (ID >= M3508_READID_START && ID <= M3508_READID_END)
    {
      M3508_getInfo(Can_Export_Data);
    }
    else if (DM_RxID == DM_READID_1)
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
    if (ID == CAN_ID_CHASSIS)
    {
      Board2_1_getChassisInfo(Can_Export_Data);
    }
    else if(ID == CAN_ID_GIMBAL)
    {
      Board2_1_getGimbalInfo(Can_Export_Data);
    }

  }
}
