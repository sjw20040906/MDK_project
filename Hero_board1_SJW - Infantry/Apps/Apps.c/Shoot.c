/**
 * @file Shoot.c
 * @author Why
 * @brief 综合摩擦轮和拨盘电机的处理，并处理热量、射速、卡弹等问题
 * @version 0.1
 * @date 2023-08-14
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "Shoot.h"

/****************接口定义******************/
Shoot_Data_t Shoot_Data = Shoot_DataGroundInit;
#undef Shoot_DataGroundInit

incrementalpid_t M3508_FricL_Pid; // 左摩擦轮电机pid
incrementalpid_t M3508_FricR_Pid; // 右摩擦轮电机pid
positionpid_t M2006_DialV_Pid;	  // 拨盘电机速度pid
incrementalpid_t M2006_DialI_Pid; // 拨盘电机电流pid

/**
 * @brief  射击总处理函数
 * @param  void
 * @retval void
 * @attention
 */
void Shoot_Processing()
{
	Fric_Processing();
	Shoot_Update_Status();
	Dial_Processing();
}

/**
 * @brief  射击相关各个状态位的更新
 * @param  void
 * @retval void
 * @attention
 */
void Shoot_Update_Status()
{
	/* 根据射击开关更新拨盘电机状态 */
	if (ControlMes.AutoAimFlag == TURN_OFF)
	{
		if (Shoot_Data.Shoot_Switch == Shoot_On)
		{
			Dial_Data.Dial_Switch = Dial_On;
		}
		else if (Shoot_Data.Shoot_Switch == Shoot_Off)
		{
			Dial_Data.Dial_Switch = Dial_Off;
		}
	}

	else if (ControlMes.AutoAimFlag == TURN_ON)
	{
		if (Shoot_Data.Shoot_Switch == Shoot_On && Fire_Flag)
		{
			Dial_Data.Dial_Switch = Dial_On;
		}
		else
			Dial_Data.Dial_Switch = Dial_Off;
	}
}
