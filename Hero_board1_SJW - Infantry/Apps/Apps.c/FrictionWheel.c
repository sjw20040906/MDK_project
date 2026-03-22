/**
 * @file FrictionWheel.c
 * @author Why
 * @brief 处理摩擦轮的控制问题
 * @version 0.1
 * @date 2023-08-14
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "FrictionWheel.h"

/**************用户数据定义****************/
float PID_Model4_Update(incrementalpid_t *pid, FUZZYPID_Data_t *PID, float _set_point, float _now_point);
/****************接口定义******************/
Fric_Data_t Fric_Data = Fric_DataGroundInit;
#undef Fric_DataGroundInit

/**
 * @brief  摩擦轮控制总处理函数
 * @param  void
 * @retval void
 * @attention
 */
void Fric_Processing()
{
  /* 设定目标值 */
  Fric_Set_targetSpeed();

  M3508_Array[FricL_Wheel].targetSpeed = -1 * Fric_Data.Required_Speed;
  M3508_Array[FricR_Wheel].targetSpeed = Fric_Data.Required_Speed;

  M3508_Array[FricL_Wheel].outCurrent = PID_Model4_Update(&M3508_FricL_Pid, &fuzzy_pid_shoot_l, M3508_Array[FricL_Wheel].targetSpeed, M3508_Array[FricL_Wheel].realSpeed);
  M3508_Array[FricR_Wheel].outCurrent = PID_Model4_Update(&M3508_FricR_Pid, &fuzzy_pid_shoot_r, M3508_Array[FricR_Wheel].targetSpeed, M3508_Array[FricR_Wheel].realSpeed);
}

/**
 * @brief  设定摩擦轮的目标速度
 * @param  void
 * @retval void
 * @attention
 */
void Fric_Set_targetSpeed(void)
{

  if (ControlMes.fric_Flag == 0)
  {
    Fric_Data.Required_Speed = 0;
    return;
  }
  else if (ControlMes.fric_Flag == 1)
  {
    Fric_Data.Required_Speed = Fric_SpeedLevel1;
  }
}

/**
 * @brief  左摩擦轮的PID重置
 * @param  void
 * @retval void
 * @attention
 */
void PID_Clear_FricL(void)
{
  Incremental_PIDInit(&M3508_FricL_Pid, 3.3f, 0.035f, 0, 8000, 700);
}

/**
 * @brief  右摩擦轮的PID重置
 * @param  void
 * @retval void
 * @attention
 */
void PID_Clear_FricR(void)
{
  Incremental_PIDInit(&M3508_FricR_Pid, 3.3f, 0.035f, 0, 8000, 700);
}
