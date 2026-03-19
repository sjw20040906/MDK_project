/**
 * @file Track_Motion.c
 * @author SJW
 * @brief
 * @version 1.0
 * @date 2026-03-16
 *
 * @copyright
 *
 */
#include "Track.h"

/**
 * @brief  履带初始化
 * @param  None
 * @retval None
 */
void Track_Init(void)
{
    ControlMes.F_Track_Angle = 0;
    ControlMes.R_Track_Angle = 0;
}

/**
 * @brief  履带运动控制
 * @param  None
 * @retval None
 */
void Track_Motion(void)
{

    if (mappedData.Ch7 == SBUS_RC_UP)
    {
        ControlMes.F_Track_Angle = mappedData.Ch10;
    }
    else
    {
        ControlMes.F_Track_Angle = 0;
    }
    if (mappedData.Ch7 == SBUS_RC_DOWN)
    {
        ControlMes.R_Track_Angle = mappedData.Ch10;
    }
    else
    {
        ControlMes.R_Track_Angle = 0;
    }
}
