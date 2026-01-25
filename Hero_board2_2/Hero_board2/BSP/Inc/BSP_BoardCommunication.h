/**
 * @file BSP_BoardCommunication.h
 * @author lxr(784457420@qq.com)
 * @brief
 * @version 1.0
 * @date 2023-9-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BSP_BOARDCOMMUNICATION_H
#define BSP_BOARDCOMMUNICATION_H

#include "main.h"
#include "BSP_Can.h"
#include "Extern_Handles.h"
#include "queue.h"

// CAN报文的ID
#define CAN_ID_B2_TRACK_DATA 0x250 // 履带数据

#define model_Normal 0
#define model_Record 1
#define model_Follow 2

typedef struct
{
    int16_t LF_track;    // 左前履带
    int16_t LR_track;    // 左后履带
    int16_t RR_track;    // 右后履带
    int16_t RF_track;    // 右前履带
} ControlMessge_betweenBoard2;

void Board2_2_getTrackInfo(Can_Export_Data_t RxMessage);

extern ControlMessge_betweenBoard2 ControlMes_board2;

#endif
