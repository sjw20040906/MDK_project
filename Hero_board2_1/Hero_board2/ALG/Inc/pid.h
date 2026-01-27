/**
 * @file PID.h
 * @author Miraggio (w1159904119@gmail)
 * @brief
 * @version 0.1
 * @date 2021-03-30
 * @copyright Copyright (c) 2021
 */

#ifndef ___PID_H
#define ___PID_H
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FuzzyPID.h"
#include "math.h"

/**********PID对外数据接口************/

// 增量式PID计算的基准参数
typedef struct incrementalpid_t
{
    float Target;         // 设定目标值
    float Measured;       // 测量值
    float err;            // 本次偏差值
    float err_last;       // 上一次偏差
    float err_beforeLast; // 上上次偏差
    float last_set_point; // 上一次目标值
    float error_target;   // 前馈控制
    float integral_error; // 误差积分累积
    float Kp;
    float Ki;
    float Kd;
    float kf; // Kp, Ki, Kd，Kf控制系数
    float p_out;
    float i_out;
    float d_out;            // 各部分输出值
    float f_out;            // 前馈f输出
    float pwm;              // pwm输出
    uint32_t MaxOutput;     // 输出限幅
    uint32_t IntegralLimit; // 积分限幅
    float (*Incremental_PID)(struct incrementalpid_t *pid_t, float target, float measured);
} incrementalpid_t;

// 位置式PID计算的基准参数
typedef struct positionpid_t
{
    float Target;         // 设定目标值
    float Measured;       // 测量值
    float err;            // 本次偏差值
    float err_last;       // 上一次偏差
    float err_change;     // 误差变化率
    float last_set_point; // 上一次目标值
    float error_target;   // 前馈控制
    float integral_error; // 误差积分累积
    float Kp;
    float Ki;
    float Kd; // Kp, Ki, Kd控制系数
    float kf;
    float p_out;
    float i_out;
    float d_out;               // 各部分输出值
    float f_out;               // 前馈f输出
    float pwm;                 // pwm输出
    float MaxOutput;           // 输出限幅
    float Integral_Separation; // 积分分离阈值
    float IntegralLimit;       // 积分限幅
    float (*Position_PID)(struct positionpid_t *pid_t, float target, float measured);
} positionpid_t;

// 模糊PID计算的基准参数
typedef struct Struct_PID_Manage_Object_Fuzzy
{
    float kp;
    float ki;
    float kd;
    float kf;

    float error;
    float last_error;
    float before_last_error;
    float integral_error;      // 误差积分累积
    float error_target;        // 前馈控制
    float err_change;          // 误差变化率
    float set_point;           // 目标值
    float now_point;           // 当前值
    float last_set_point;      // 上一次目标值
    float integral_limit;      // 积分限幅
    float output_limit;        // 输出限幅
    float Integral_Separation; // 积分分离阈值
    float deadzone;            // 死区
    float p_out;               // p输出
    float i_out;               // i输出
    float d_out;               // d输出
    float f_out;               // 前馈f输出
    float output;              // 总输出
} Struct_PID_Manage_Object_Fuzzy;

/******** (新增) 高级PID结构体 ********/
/**
 * @brief (新增) 高级增量式PID结构体 (带前馈和微分先行)
 * @note 专为高性能伺服控制优化 (如云台Yaw轴)
 */
typedef struct pid_advanced_t
{
    float Target;              // 设定目标值
    float Measured;            // 测量值
    float err;                 // e(k) 本次偏差
    float err_last;            // e(k-1) 上一次偏差
    float measured_last;       // PV(k-1)
    float measured_beforeLast; // PV(k-2)
    float target_last;         // Target(k-1)
    float Kp, Ki, Kd, Kf;
    float p_out, i_out, d_out, f_out;
    float delta_pwm;     // 本次总增量
    float pwm;           // 累计总输出
    float MaxOutput;     // 最大输出限幅
    float IntegralLimit; // 积分限幅
} pid_advanced_t;

float Incremental_PID(incrementalpid_t *pid_t, float target, float measured);
float Position_PID(positionpid_t *pid_t, float target, float measured);
void Incremental_PIDInit(incrementalpid_t *pid_t, float Kp, float Kd, float Ki, uint32_t MaxOutput, uint32_t IntegralLimit);
void Position_PIDInit(positionpid_t *pid_t, float Kp, float Kd, float Ki, float Kf, float MaxOutput, float IntegralLimit, float Integral_Separation);
float Position_PID_Pitch(positionpid_t *pid_t, FUZZYPID_Data_t *fuzzy_t, float target, float measured);
float PID_Model4_Update(incrementalpid_t *pid, FUZZYPID_Data_t *PID, float _set_point, float _now_point);
void PID_Advanced_Init(pid_advanced_t *pid_t, float Kp, float Kd, float Ki, float kf, float MaxOutput, float IntegralLimit);
float PID_Advanced_Angle_Calc_Positional(pid_advanced_t *pid_t, float target, float measured, float ecd_max);

#endif
