/**
 * @file DJI_VT13.h
 * @author MOON
 * @brief
 * @version 1.0
 * @date 2025-10-23
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef DJI_VT13_H
#define DJI_VT13_H

#include <stdint.h>
#include <stdbool.h>
#include "usart.h"
#include "BSP_BoardCommunication.h"
#include "Cloud_Control.h"
#include "Protocol_UpperComputer.h"
#include "Dial.h"
#include "Shoot.h"
#include "SpeedRamp.h"
#include "DT7.h"

#define VTM_FRAME_LEN 21u
#define VTM_HEADER0 0xA9
#define VTM_HEADER1 0x53
#define PRESS_flag_YES 1
#define PRESS_flag_NO 0
#define TURN_ON 1
#define TURN_OFF 0

#define PRESS_THRESHOLD 10        // 按键阈值
#define SHORT_PRESS_THRESHOLD 300 // 短按阈值
#define LONG_PRESS_THRESHOLD 800 // 长按阈值
typedef struct
{
    uint8_t key_state;         // 当前状态
    uint8_t last_state;        // 上一次状态
    uint32_t press_time;       // 按下时刻
    uint8_t short_press_flag;  // 短按标志位
    uint8_t long_press_flag;   // 长按事件标志
    uint8_t long_press_report; // 防止长按重复触发
    uint8_t press_flag;        // 按下标志位
} KeyDetect_t;

typedef enum
{
    VTM_MODE_C = 0, // C 档
    VTM_MODE_N = 1, // N 档
    VTM_MODE_S = 2  // S 档
} VTM_ModeSwitch;

/* 键盘位图（bit0..bit15） */
typedef struct
{
    uint8_t W : 1, S : 1, A : 1, D : 1, Shift : 1, Ctrl : 1, Q : 1, E : 1;
    uint8_t R : 1, F : 1, G : 1, Z : 1, X : 1, C : 1, V : 1, B : 1;
} VTM_KeyboardBits;

/* 完整解析结果 */
typedef struct
{
    /* 11-bit 通道：范围 364..1684，居中 1024 */
    int16_t ch[4]; // ch0..ch3

    /* 挡位、按键 */
    VTM_ModeSwitch mode; // 0=C,1=N,2=S
    uint8_t pause;       // 0/1
    uint8_t btn_left;    // 自定义左键 0/1
    uint8_t btn_right;   // 自定义右键 0/1

    /* 拨轮 11-bit */
    int16_t wheel; // 364..1684

    /* 扳机 */
    uint8_t trigger; // 0/1

    /* 鼠标移动（有符号16位；速度/增量） */
    int16_t mouse_x;
    int16_t mouse_y;
    int16_t mouse_z;

    /* 鼠标键（表定义为2bit，但取值仍为0/1） */
    uint8_t mouse_l; // 0/1
    uint8_t mouse_r; // 0/1
    uint8_t mouse_m; // 0/1

    /* 键盘位图 */
    uint16_t keyboard_bits; // 原始 16bit
    VTM_KeyboardBits kb;    // 便捷访问

    /* 帧头校验通过标志 */
    bool header_ok;
} VTM_Frame;

/* 键鼠状态结构体 */
typedef struct
{
    // 鼠标移动
    int16_t dx;
    int16_t dy;
    int16_t dz;

    // 鼠标按键
    KeyDetect_t mouse_l;
    KeyDetect_t mouse_r;
    KeyDetect_t mouse_m;

    // 键盘按键
    KeyDetect_t W;
    KeyDetect_t S;
    KeyDetect_t A;
    KeyDetect_t D;
    KeyDetect_t Q;
    KeyDetect_t E;
    KeyDetect_t R;
    KeyDetect_t F;
    KeyDetect_t G;
    KeyDetect_t Z;
    KeyDetect_t X;
    KeyDetect_t C;
    KeyDetect_t V;
    KeyDetect_t B;
    KeyDetect_t Shift;
    KeyDetect_t Ctrl;

} KeyMouseDetect_t;

void DJI_VT13_Init(void);
void DJI_VT13_Handle(void);

extern VTM_Frame DJI_VT13_Data;
extern uint8_t DJI_VT13_RX_Buf[VTM_FRAME_LEN];
extern uint8_t DJI_VT13_RX_Finish;

#endif /* VTM_FRAME_H */
