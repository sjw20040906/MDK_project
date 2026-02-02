/**
 * @file Cloud_control.c
 * @author Cyx,SJW
 * @brief
 * @version 0.1
 * @date 2023-08-15
 *
 * @copyright
 *
 */
#ifndef __CLOUD_CONTROL_H
#define __CLOUD_CONTROL_H

#include "PID.h"
#include "kalman_filter.h"
#include "shoot.h"
#include "DT7.h"
#include "typedef.h"
#include "J4310_Motor.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "FeedForward.h"
#include "M3508_Motor.h"
#include "BSP_Can.h"
#include "Extern_Handles.h"
#include "FuzzyPID.h"

#define Cloud_Pitch_level 1.24f
/* ????????? */
typedef struct
{
  float Yaw_Raw;       // yaw?????
  float Pitch_Raw;     // pitch?????
  float Target_Yaw;    // ????yaw?
  float Target_Pitch;  // ????pitch?
  float AutoAim_Pitch; // ?????pithc???
} Cloud_t;

typedef struct
{
  void (*Cloud_Init)(void);            // ?????????
  void (*Cloud_Sport_Out)(void);       // ??????????
  void (*Cloud_Pitch_Angle_Set)(void); // Pitch?????????
  void (*Remote_Change)(void);         // ?????????????

} Cloud_FUN_t;

void Cloud_Init(void);

extern Cloud_t Cloud;
extern Cloud_FUN_t Cloud_FUN;

#define Cloud_FUNGroundInit {&Cloud_Init, &Cloud_Sport_Out, &Cloud_Pitch_Angle_Set, &Remote_Change}

#endif /* __CLOUD_CONTROL_H */
