/**
 * @file Cloud_control.h
 * @author MOON
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

typedef struct
{
  float Yaw_Raw;       
  float Pitch_Raw;     
  float Target_Yaw;    
  float Target_Pitch;  
  float AutoAim_Pitch; 
} Cloud_t;

void Cloud_Init(void);
void Cloud_Sport_Out(void);
void Remote_Change(void);

extern Cloud_t Cloud;


#endif /* __CLOUD_CONTROL_H */
