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
#include "Chassis.h"
#include "Gimbal.h"

// CAN报文的标识符和数据长度
#define CAN_ID_CHASSIS 0x10f	   // 报文底盘数据ID为0x10f
#define CAN_ID_GIMBAL 0x11f		   // 云台数据ID为0x11f
#define CAN_ID_B1_TRACK_DATA 0x250 // 发送4个履带数据

#define model_Normal 0
#define model_Record 1
#define model_Follow 2

// 定义FDCAN报文的结构体
typedef struct
{
	int16_t x_velocity;
	int16_t y_velocity;
	int16_t z_rotation_velocity;
	int16_t pitch_velocity;
	int16_t yaw_velocity;
	int16_t yaw_position; // 自瞄使用时，yaw轴应该在的绝对位置
	uint8_t AutoAimFlag;  // 自瞄开关
	uint8_t shoot_state;
	int16_t yaw_realAngle; // 下板传上来的yaw轴角度信息
	float Speed_Bullet;	   // 裁判系统传来的弹速
	int16_t heat_remain;   // 裁判系统传来的剩余热量
	uint8_t modelFlag;	   // 比赛、检录，地盘跟随标模式
	uint8_t shoot_Speed;   // 射速
	uint8_t change_Flag;   // 变速
	uint8_t fric_Flag;	   // 摩擦轮
	uint8_t reset_Flag;	   // 复位标志
	uint8_t tnndcolor;	   // 己方颜色，1为红，2为蓝
	uint8_t redial;
	int16_t Gimbal_Chassis_Pitch_Angle; // 底盘正方向的pitch轴角度
	uint8_t jump_Flag;
	uint16_t Blood_Volume; // 机器人当前血量
	uint16_t game_start;   // 比赛开始标志（未开始为1，准备、自检、比赛中为1）
} ControlMessge;

typedef struct
{
	int16_t LF_track; // 左前履带
	int16_t LR_track; // 左后履带
	int16_t RR_track; // 右后履带
	int16_t RF_track; // 右前履带
} ControlMessge_betweenBoard2;

void Board2_1_To_Board2_2(void);
void Board2_1_getChassisInfo(Can_Export_Data_t RxMessage);
void Board2_1_getGimbalInfo(Can_Export_Data_t RxMessage);

extern ControlMessge ControlMes;
extern ControlMessge_betweenBoard2 ControlMes_board2;

#endif
