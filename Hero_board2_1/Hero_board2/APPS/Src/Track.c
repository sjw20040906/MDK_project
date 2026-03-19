/**
 * @file Task_RobotControl.c
 * @author SJW
 * @brief
 * @version 1.0
 * @date 2026-01-30
 * @copyright Copyright (c) 2026
 */

#include "Track.h"

float Delta_F = 0;
float Delta_R = 0;
float F_Track_Angle = 0;
float R_Track_Angle = 0;



/**
 * @brief 履带初始化
 * @param None
 * @return None
 */
void Track_Init(void)
{
    Delta_F = 0;
    Delta_R = 0;
    ControlMes_board2.LF_track = 0;
    ControlMes_board2.LR_track = 0;
    ControlMes_board2.RR_track = 0;
    ControlMes_board2.RF_track = 0;
}

/**
 * @brief 履带运动控制
 *
 * @param None
 * @return None
 */
void Track_motion_control(void)
{
    Delta_F = ControlMes.F_Track_Angle * 0.0002f;
    Delta_R = ControlMes.R_Track_Angle * 0.0002f;

    if (Delta_F > 5)
        Delta_F = 5;
    else if (Delta_F < -5)
        Delta_F = -5;
    if (Delta_R > 5)
        Delta_R = 5;
    else if (Delta_R < -5)
        Delta_R = -5;
    F_Track_Angle += Delta_F;
    R_Track_Angle += Delta_R;

    if(R_Track_Angle > TRACK_MAX_ANGLE)
        R_Track_Angle = TRACK_MAX_ANGLE;
    else if(R_Track_Angle < TRACK_MIN_ANGLE)
        R_Track_Angle = TRACK_MIN_ANGLE;
    if(F_Track_Angle > TRACK_MAX_ANGLE)
        F_Track_Angle = TRACK_MAX_ANGLE;
    else if(F_Track_Angle < TRACK_MIN_ANGLE)
        F_Track_Angle = TRACK_MIN_ANGLE;

    ControlMes_board2.LF_track = F_Track_Angle;
    ControlMes_board2.LR_track = R_Track_Angle;
    ControlMes_board2.RR_track = R_Track_Angle;
    ControlMes_board2.RF_track = F_Track_Angle;
}
