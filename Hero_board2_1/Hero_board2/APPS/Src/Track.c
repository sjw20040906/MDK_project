/**
 * @file Task_RobotControl.c
 * @author SJW
 * @brief
 * @version 1.0
 * @date 2026-01-30
 * @copyright Copyright (c) 2026
 */

#include "Track.h"

/**
 * @brief 履带运动控制
 *
 * @param None
 * @return None
 */
void Track_motion_control(void)
{
    ControlMes_board2.LF_track = -50;
    ControlMes_board2.LR_track = -50;
    ControlMes_board2.RR_track = -50;
    ControlMes_board2.RF_track = -50;
}
