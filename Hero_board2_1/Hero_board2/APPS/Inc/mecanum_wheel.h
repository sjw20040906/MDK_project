/**
 * @file mecanum_wheel.h
 * @brief 麦克纳姆轮运动学解算头文件
 * @author SJW
 * @date 2026-01-12
 */

#ifndef MECANUM_WHEEL_H
#define MECANUM_WHEEL_H

#include <stddef.h>

// 麦克纳姆轮配置结构体
typedef struct {
    float wheel_radius;    // 轮子半径 (m)
    float wheel_base_x;    // 轮子与中心X轴 (m) 
    float wheel_base_y;    // 轮子与中心Y轴 (m)
} MecanumConfig;

// 速度向量结构体
typedef struct {
    float vx;    // X轴速度 (m/s)
    float vy;    // Y轴速度 (m/s)
    float wz;    // 角速度 (rad/s)
} VelocityVector;

// 轮子速度结构体
typedef struct {
    float wheel0;    // 左前轮速度 (rad/s)
    float wheel1;    // 左后轮速度 (rad/s)
    float wheel2;    // 右后轮速度 (rad/s)
    float wheel3;    // 右前轮速度 (rad/s)
} WheelSpeeds;


void mecanum_init(MecanumConfig *config, float wheel_radius, float wheel_base_x, float wheel_base_y);
void mecanum_forward_kinematics(const VelocityVector *velocity, const MecanumConfig *config, WheelSpeeds *speeds);
void mecanum_inverse_kinematics(const WheelSpeeds *speeds, const MecanumConfig *config, VelocityVector *velocity);
void mecanum_limit_speeds(WheelSpeeds *speeds, float max_speed);
void mecanum_calculate_wheel_speeds(const VelocityVector *velocity, const MecanumConfig *config, float max_speed, WheelSpeeds *speeds);

#endif // MECANUM_WHEEL_H
