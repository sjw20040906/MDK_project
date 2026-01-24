/**
 * @file Task_RobotControl.c
 * @author rm_control_team
 * @brief
 * @version 0.1
 * @date 2023-08-30
 * @copyright Copyright (c) 2021
 */

#include "Task_RobotControl.h"

void Robot_Control(void const *argument)
{
  portTickType xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  const TickType_t TimeIncrement = pdMS_TO_TICKS(2);
  for (;;)
  {
    uint8_t data_1[8] = {0};
    Chassis_motion_control();
    M3508_setCurrent(M3508_Array[Chassis_Left_Front].outCurrent, M3508_Array[Chassis_Left_Back].outCurrent, M3508_Array[Chassis_Right_Back].outCurrent, M3508_Array[Chassis_Right_Front].outCurrent, data_1);
    CAN_SendData(CAN_SendHandle, &hcan1, CAN_ID_STD, M3508_ID_Chassis, data_1);
    vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
  }
}
