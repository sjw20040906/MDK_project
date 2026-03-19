/**
 * @file Track_Motion.h
 * @author SJW
 * @brief
 * @version 1.0
 * @date 2026-03-16
 *
 * @copyright
 *
 */

#ifndef __TRACK_MOTION_H__
#define __TRACK_MOTION_H__

#include "BSP_BoardCommunication.h"
#include "SBUS.h"

#define TRACK_MAX_ANGLE 100
#define TRACK_MIN_ANGLE -100

void Track_Motion(void);
void Track_Init(void);

#endif
