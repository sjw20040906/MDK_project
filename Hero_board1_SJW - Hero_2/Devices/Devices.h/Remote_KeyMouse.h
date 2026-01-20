/**
 * @file Remote_KeyMouse.h
 * @brief 键鼠遥控数据协议解析头文件 (适配 0x0304 协议)
 * @version 2.0
 * @date 2026-01-19
 */

#ifndef __REMOTE_KEYMOUSE_H__
#define __REMOTE_KEYMOUSE_H__

#include "main.h" 
#include "stdint.h"
#include "string.h"
#include "BSP_Usart.h"
#include "DT7.h"

// ==================== 宏定义 ====================

// 接收缓冲区大小
#define KM_RX_BUF_SIZE 256

// 按键长短按时间阈值 (单位: ms)
#define PRESS_THRESHOLD 5        // 消抖时间，超过这个时间才认为按下
#define SHORT_PRESS_THRESHOLD 100 // 松开时小于这个时间视为短按
#define LONG_PRESS_THRESHOLD 800  // 超过这个时间视为长按

// 协议常量
#define PROTOCOL_SOF 0xA5
#define PROTOCOL_CMD_ID 0x0304

// 开关状态定义
#ifndef TURN_ON
#define TURN_ON 1
#define TURN_OFF 0
#endif

// ==================== 结构体定义 ====================

/**
 * @brief 按键状态机结构体 (用于消抖和检测长短按)
 */
typedef struct
{
    uint8_t key_state;   // 当前按键物理状态 (0/1)
    uint8_t last_state;  // 上一次按键物理状态
    uint32_t press_time; // 按下时刻的时间戳

    uint8_t press_flag;        // 当前是否处于按下状态 (消抖后)
    uint8_t short_press_flag;  // 短按触发标志 (松开瞬间置1，需手动清除或下一帧自动清除)
    uint8_t long_press_flag;   // 长按触发标志 (持续按下达到阈值)
    uint8_t long_press_report; // 长按上报锁 (防止长按期间重复触发单次逻辑)
} KeyDetect_t;

/**
 * @brief 键盘位域辅助结构体 (用于解包 16bit 数据)
 */
typedef struct
{
    uint8_t W;
    uint8_t S;
    uint8_t A;
    uint8_t D;
    uint8_t Shift;
    uint8_t Ctrl;
    uint8_t Q;
    uint8_t E;
    uint8_t R;
    uint8_t F;
    uint8_t G;
    uint8_t Z;
    uint8_t X;
    uint8_t C;
    uint8_t V;
    uint8_t B;
} VTM_KeyboardBits;

/**
 * @brief 原始协议数据包结构体 (对应协议文档 0x0304)
 */
typedef struct
{
    int16_t mouse_x;            // 鼠标 X轴速度
    int16_t mouse_y;            // 鼠标 Y轴速度
    int16_t mouse_z;            // 鼠标 滚轮速度
    int8_t  left_button_down;   // 鼠标 左键 (0/1)
    int8_t  right_button_down;  // 鼠标 右键 (0/1)
    uint16_t keyboard_value;    // 键盘 16位 键值掩码
    uint16_t reserved;          // 保留位
    uint8_t InfoUpdataFlag; // 数据更新标志位
} ext_robot_keycommand_t;


/**
 * @brief 键鼠应用层总控结构体
 * 包含所有按键的检测实例和鼠标位移
 */
typedef struct
{
    // 鼠标位移
    int16_t dx;
    int16_t dy;
    int16_t dz;

    // 鼠标按键
    KeyDetect_t mouse_l;
    KeyDetect_t mouse_r;
    // KeyDetect_t mouse_m; // 协议未定义滚轮中键，如需要可自行映射

    // 键盘按键 (覆盖所有 16 bits)
    KeyDetect_t W;
    KeyDetect_t S;
    KeyDetect_t A;
    KeyDetect_t D;
    KeyDetect_t Shift;
    KeyDetect_t Ctrl;
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

} KeyMouseDetect_t;

// ==================== 外部变量声明 ====================

extern uint8_t KM_RX_Buf[KM_RX_BUF_SIZE];    // 接收缓冲
extern uint8_t KM_RX_Finish;                 // 接收完成中断标志
extern ext_robot_keycommand_t Robot_KeyData; // 协议数据
extern KeyMouseDetect_t KeyMouseDetect;      // 按键检测数据

// ==================== 函数声明 ====================

/**
 * @brief 初始化键鼠接收
 */
void Remote_KeyMouse_Init(void);

/**
 * @brief 键鼠协议处理主循环
 * @note  建议在主循环或定时器中调用
 */
void KeyMouse_Handle_Wrapper(void);

/**
 * @brief 更新单个按键的状态机
 * @param key 按键结构体指针
 * @param current_state 当前物理电平(0或1)
 */
void KeyDetect_Update(KeyDetect_t *key, uint8_t current_state);

/**
 * @brief 统一更新所有键鼠状态
 */
void KeyMouse_Update(KeyMouseDetect_t *km, const ext_robot_keycommand_t *raw_data);

#endif /* __REMOTE_KEYMOUSE_H__ */
