/**
 * @file Dial.c
 * @author Why,ZS,SJW
 * @brief 拨弹盘处理总函数
 * @version 0.1
 * @date 2023-08-14
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "Dial.h"

/**************相关函数定义****************/
Dial_Data_t Dial_Data = Dial_DataGroundInit;
#undef Dial_DataGroundInit

/**
 * @brief  拨弹处理函数2
 * @param  void
 * @retval void
 * @attention
 */
void Dial_Processing(void)
{

	static uint32_t last_dial_time = 0;
	uint32_t current_time = HAL_GetTick();

	/****************拨弹*****************/
	if (ControlMes.AutoAimFlag == 1) // 自瞄模式下，需要上位机开火开关
	{
		if (Dial_Data.Shoot_Mode == Single_Shoot && Dial_Data.Dial_Switch == Dial_On && Fire_Flag)
		{
			// 检查是否到达2秒间隔
			if (current_time - last_dial_time >= 2000)
			{
				Dial_OneBullet();
				last_dial_time = current_time;
			}
			J3519_Array[J3519_Dail_Wheel].outSpeed = Position_PID(&J3519_DialV_Pid, J3519_Array[J3519_Dail_Wheel].outPosition, J3519_Array[J3519_Dail_Wheel].totalAngle);
		}
		else
		{
			J3519_Array[J3519_Dail_Wheel].outSpeed = 0;
		}
	}

	else // 非自瞄模式下，不需要上位机开火开关
	{
		if (Dial_Data.Shoot_Mode == Single_Shoot && Dial_Data.Dial_Switch == Dial_On)
		{
			// 检查是否到达2秒间隔
			if (current_time - last_dial_time >= 2000)
			{
				Dial_OneBullet();
				last_dial_time = current_time;
			}
			J3519_Array[J3519_Dail_Wheel].outSpeed = Position_PID(&J3519_DialV_Pid, J3519_Array[J3519_Dail_Wheel].outPosition, J3519_Array[J3519_Dail_Wheel].totalAngle);
		}
		else
		{
			J3519_Array[J3519_Dail_Wheel].outSpeed = 0;
		}
	}
}


/**
 * @brief  单发
 * @param  void
 * @retval void
 * @attention
 */
void Dial_OneBullet()
{
	J3519_Array[J3519_Dail_Wheel].outPosition -= (float)Angle_DialOneBullet_42mm; 
}

/**
 * @brief  更新拨盘电机的角度值
 * @param  void
 * @retval void
 * @attention
 */
void Dial_Update_Angel(bool Fric_ReadyOrNot)
{
	// 摩擦轮转速满足射定速度
	if (Fric_ReadyOrNot)
	{
		Dial_Processing();
		Dial_Data.Bullet_Dialed++;
	}
	else
	{
		J3519_Array[J3519_Dail_Wheel].outPosition = J3519_Array[J3519_Dail_Wheel].outPosition;
	}
}

