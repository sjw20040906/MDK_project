/**
 * @file FeedForward.c
 * @author Why
 * @brief 前馈控制,取实测量的百分之八十
 * @version 0.1
 * @date 2023-08-28
 * @attention 一定要在PID算出电流之后再重新引用这个函数，因为是直接加在输出电流上了
				这里只是放这个文件，里边的东西都是没有用到的旧版本的另一个板子的东西
 *
 * @copyright
 *
 */

#include "FeedForward.h"

/**
 * @brief   给摩擦轮发送前馈
 * @param   M3508的外部接口
 * @retval  void
 */
void FeedForward_Fric()
{
	/* 平衡摩擦力 */
	if (M3508_Array[Fric_Left].realSpeed < 1000)
		M3508_Array[Fric_Left].outCurrent +=
			(212 - 0.124f * (float)M3508_Array[Fric_Left].realSpeed);
	else
		M3508_Array[Fric_Left].outCurrent +=
			(88 - 0.01f * (float)(M3508_Array[Fric_Left].realSpeed - 1000));

	if (M3508_Array[Fric_Right].realSpeed < 1000)
		M3508_Array[Fric_Right].outCurrent +=
			(212 - 0.124f * (float)M3508_Array[Fric_Right].realSpeed);
	else
		M3508_Array[Fric_Right].outCurrent +=
			(88 - 0.01f * (float)(M3508_Array[Fric_Right].realSpeed - 1000));

	/* 平衡弹丸进摩擦轮的巨大阻力，减少掉速 */
	M3508_Array[Fric_Left].outCurrent += 5000;
	M3508_Array[Fric_Right].outCurrent += 5000;
}
