/**
 * @file DT7.h
 * @author MOON
 * @brief
 * @version 2.0
 * @date 2024-3-9
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __DT7_H
#define __DT7_H

#include "main.h"
#include "usart.h"
#include "BSP_BoardCommunication.h"
#include "Cloud_Control.h"
#include "Protocol_UpperComputer.h"
#include "Dial.h"
#include "Shoot.h"
#include "Cloud_Control.h"
#include "SpeedRamp.h"


/* ----------------------- RC Channel Definition---------------------------- */
#define RC_CH_VALUE_MIN ((uint16_t)364)
#define RC_CH_VALUE_OFFSET ((uint16_t)1024)
#define RC_CH_VALUE_MAX ((uint16_t)1684)
/* ----------------------- RC Switch Definition----------------------------- */
#define RC_SW_UP ((uint16_t)1)
#define RC_SW_MID ((uint16_t)3)
#define RC_SW_DOWN ((uint16_t)2)

#define RC_FRAME_LENGTH 18u

#define KEYMOUSE_AMOUNT 18
#define IT_KEYMOUSE_AMOUNT 18
#define TIME_KeyMouse_Press 3
#define TIME_KeyMouse_LongPress 60

#define PROTOCOL_SOF 0xA5
#define PROTOCOL_CMD_ID 0x0304

#define DR16_ExportDataGroundInit \
    {                             \
        {0, 0},                   \
        {0, 0, 0, {0}},           \
        {0, 0, 0, 0, 0, 0},       \
        0,                        \
        0,                        \
    }

#define Image_Transmission_ExportDataGroundInit \
    {                                           \
        {0, 0},                                 \
        {0, 0, 0, {0}},                         \
        {0, 0, 0, 0, 0, 0},                     \
        0,                                      \
        0,                                      \
    }

#define ForwardBackGroundInit \
    {                         \
        0,                    \
        0,                    \
        -800,                 \
        800,                  \
    }

#define LeftRightGroundInit \
    {                       \
        0,                  \
        0,                  \
        -800,               \
        800,                \
    }

#define RotateGroundInit \
    {                    \
        0,               \
        0,               \
        -1100,            \
        1100,             \
    }

/* ----------------------- Data Struct ------------------------------------- */
/**
 * @brief  remote control information
 */
typedef struct
{
    struct
    {
        int16_t ch0;
        int16_t ch1;
        int16_t ch2;
        int16_t ch3;
        uint8_t s1;
        uint8_t s2;
    } rc;

    struct
    {
        int16_t x;
        int16_t y;
        int16_t z;
        uint8_t press_l;
        uint8_t press_r;
    } mouse;

    /* keyboard key information */
    union
    {
        uint16_t key_code;
        struct
        {
            uint16_t W : 1;
            uint16_t S : 1;
            uint16_t A : 1;
            uint16_t D : 1;
            uint16_t SHIFT : 1;
            uint16_t CTRL : 1;
            uint16_t Q : 1;
            uint16_t E : 1;
            uint16_t R : 1;
            uint16_t F : 1;
            uint16_t G : 1;
            uint16_t Z : 1;
            uint16_t X : 1;
            uint16_t C : 1;
            uint16_t V : 1;
            uint16_t B : 1;
        } bit;
    } key;

    int16_t wheel;

} RC_Ctl_t;

typedef enum
{
    KEY_W = 0, 
    KEY_S = 1, 
    KEY_A,     
    KEY_D,     
    KEY_SHIFT,
    KEY_CTRL, 
    KEY_Q,    
    KEY_E,    
    KEY_R,    
    KEY_F,
    KEY_G,
    KEY_Z,
    KEY_X,
    KEY_C,      
    KEY_V,      
    KEY_B,      
    MOUSE_Left, 
    MOUSE_Right 
} KeyList_e;

typedef enum
{
    KeyAction_CLICK,
    KeyAction_PRESS,
    KeyAction_LONG_PRESS
} KeyAction_e;

typedef struct
{
    struct
    {
        float x;
        float y;
    } mouse;

    struct
    {

        uint32_t Press_Flag;              
        uint32_t Click_Press_Flag;       
        uint32_t Long_Press_Flag;          
        uint8_t PressTime[KEYMOUSE_AMOUNT]; 
    } KeyMouse;                          

    struct
    {
        float Forward_Back_Value; 
        float Omega_Value;        
        float Left_Right_Value;   
        float Pitch_Value;
        float Yaw_Value;
        float Dial_Wheel;     
    } Robot_TargetValue;     
    uint16_t infoUpdateFrame; 
    uint8_t OffLineFlag;      
} DR16_Export_Data_t;      


typedef struct
{
    struct
    {
        float x;
        float y;
    } mouse;

    struct
    {

        uint32_t Press_Flag;
        uint32_t Click_Press_Flag;
        uint32_t Long_Press_Flag;
        uint8_t PressTime[KEYMOUSE_AMOUNT];
    } KeyMouse;

    struct
    {
        float Forward_Back_Value;
        float Omega_Value;
        float Left_Right_Value;
        float Pitch_Value;
        float Yaw_Value;
        float Dial_Wheel;
    } Robot_TargetValue;
    uint16_t infoUpdateFrame;
    uint8_t OffLineFlag;
} Image_Transmission_Export_Data_t;

extern RC_Ctl_t RC_CtrlData;
extern uint8_t DT7_RX_Finish;
extern uint8_t DT7_Rx_Data[RC_FRAME_LENGTH];
extern SpeedRamp_t ChassisRamp_ForwardBack;
extern SpeedRamp_t ChassisRamp_LeftRight;
extern SpeedRamp_t ChassisRamp_Rotate;

void DT7_Init(void);
void DT7_Handle(void);
#endif
