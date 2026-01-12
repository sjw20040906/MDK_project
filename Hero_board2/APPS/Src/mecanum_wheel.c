/**
 * @file mecanum_wheel.c
 * @brief 麦克纳姆轮运动学解算源文件
 * @author SJW
 * @date 2026-01-12
 */
#include "mecanum_wheel.h"

/**
 * @brief 初始化麦克纳姆轮配置
 * @param config 麦克纳姆轮配置结构体指针
 * @param wheel_radius 轮子半径 (m)
 * @param wheel_base_x 轮子与中心X轴 (m)
 * @param wheel_base_y 轮子与中心Y轴 (m)
 */
void mecanum_init(MecanumConfig *config, float wheel_radius, float wheel_base_x, float wheel_base_y)
{
    if (config != NULL)
    {
        config->wheel_radius = wheel_radius;
        config->wheel_base_x = wheel_base_x;
        config->wheel_base_y = wheel_base_y;
    }
}

/**
 * @brief 正向解算：从速度向量计算轮子速度
 * @param velocity 速度向量结构体指针
 * @param config 麦克纳姆轮配置结构体指针
 * @param speeds 轮子速度结构体指针
 */
void mecanum_forward_kinematics(const VelocityVector *velocity, const MecanumConfig *config, WheelSpeeds *speeds)
{
    if (velocity == NULL || config == NULL || speeds == NULL)
    {
        return;
    }

    float wheel_radius = config->wheel_radius;
    float wheel_base_x = config->wheel_base_x;
    float wheel_base_y = config->wheel_base_y;

    // 麦克纳姆轮正向运动学公式
    // 轮子速度计算
    speeds->wheel0 = (velocity->vx - velocity->vy - (wheel_base_x + wheel_base_y) * velocity->wz) / wheel_radius; // 左前
    speeds->wheel1 = (velocity->vx + velocity->vy - (wheel_base_x + wheel_base_y) * velocity->wz) / wheel_radius; // 左后
    speeds->wheel2 = (velocity->vx - velocity->vy + (wheel_base_x + wheel_base_y) * velocity->wz) / wheel_radius; // 右后
    speeds->wheel3 = (velocity->vx + velocity->vy + (wheel_base_x + wheel_base_y) * velocity->wz) / wheel_radius; // 右前
}

/**
 * @brief 逆向解算：从轮子速度计算速度向量
 * @param speeds 轮子速度结构体指针
 * @param config 麦克纳姆轮配置结构体指针
 * @param velocity 速度向量结构体指针
 */
void mecanum_inverse_kinematics(const WheelSpeeds *speeds, const MecanumConfig *config, VelocityVector *velocity)
{
    if (speeds == NULL || config == NULL || velocity == NULL)
    {
        return;
    }

    float wheel_radius = config->wheel_radius;
    float wheel_base_x = config->wheel_base_x;
    float wheel_base_y = config->wheel_base_y;

    // 麦克纳姆轮逆向运动学公式
    velocity->vx = (speeds->wheel0 + speeds->wheel1 + speeds->wheel2 + speeds->wheel3) * wheel_radius / 4.0f;
    velocity->vy = (-speeds->wheel0 + speeds->wheel1 - speeds->wheel2 + speeds->wheel3) * wheel_radius / 4.0f;
    velocity->wz = (-speeds->wheel0 - speeds->wheel1 + speeds->wheel2 + speeds->wheel3) * wheel_radius / (4.0f * (wheel_base_x + wheel_base_y));
}

/**
 * @brief 限制轮子速度在最大范围内
 * @param speeds 轮子速度结构体指针
 * @param max_speed 最大速度 (rad/s)
 */
void mecanum_limit_speeds(WheelSpeeds *speeds, float max_speed)
{
    if (speeds == NULL)
    {
        return;
    }

    // 检查并限制每个轮子的速度
    if (speeds->wheel0 > max_speed)
        speeds->wheel0 = max_speed;
    if (speeds->wheel0 < -max_speed)
        speeds->wheel0 = -max_speed;

    if (speeds->wheel1 > max_speed)
        speeds->wheel1 = max_speed;
    if (speeds->wheel1 < -max_speed)
        speeds->wheel1 = -max_speed;

    if (speeds->wheel2 > max_speed)
        speeds->wheel2 = max_speed;
    if (speeds->wheel2 < -max_speed)
        speeds->wheel2 = -max_speed;

    if (speeds->wheel3 > max_speed)
        speeds->wheel3 = max_speed;
    if (speeds->wheel3 < -max_speed)
        speeds->wheel3 = -max_speed;
}

/**
 * @brief 综合函数：计算轮子速度并限制在最大范围内
 * @param velocity 速度向量结构体指针
 * @param config 麦克纳姆轮配置结构体指针
 * @param max_speed 最大速度 (rad/s)
 * @param speeds 轮子速度结构体指针
 */
void mecanum_calculate_wheel_speeds(const VelocityVector *velocity, const MecanumConfig *config, float max_speed, WheelSpeeds *speeds)
{
    if (velocity == NULL || config == NULL || speeds == NULL)
    {
        return;
    }

    // 首先计算轮子速度
    mecanum_forward_kinematics(velocity, config, speeds);

    // 然后限制轮子速度在最大范围内
    mecanum_limit_speeds(speeds, max_speed);
}
