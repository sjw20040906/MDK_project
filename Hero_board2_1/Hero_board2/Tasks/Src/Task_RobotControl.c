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
    uint8_t data_1[8], data_2[8] = {0};
    /*********** 云台底盘运动控制 ************/
    Gimbal_motion_Control();
    Chassis_motion_control();
    /************* 电机参数设定 **************/
    M3508_setCurrent(M3508_Array[Chassis_Left_Front].outCurrent, M3508_Array[Chassis_Left_Back].outCurrent, M3508_Array[Chassis_Right_Back].outCurrent, M3508_Array[Chassis_Right_Front].outCurrent, data_1);
    DM_setParameter(0, DM_Array[Gimbal_Motor].outSpeed, 0, 10, 0, data_2);
    /************* 电机参数发送 **************/
    CAN_SendData(CAN_SendHandle, &hcan1, CAN_ID_STD, M3508_ID_Chassis, data_1);
    CAN_SendData(CAN_SendHandle, &hcan1, CAN_ID_STD, DM_SENDID_1, data_2);
    vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
  }
}
