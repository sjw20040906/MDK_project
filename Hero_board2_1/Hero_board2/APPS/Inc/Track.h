#ifndef __TRACK_H__
#define __TRACK_H__

#include "BSP_BoardCommunication.h"

#define TRACK_MAX_ANGLE 60
#define TRACK_MIN_ANGLE -60

/**
 * @brief 履带运动控制
 *
 * @param None
 * @return None
 */
void Track_motion_control(void);

/**
 * @brief 履带初始化
 * @param None
 * @return None
 */
void Track_Init(void);

#endif
