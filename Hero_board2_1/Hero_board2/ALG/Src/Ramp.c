#include "Ramp.h"

Ramp_Handle_t myRamp;

/**
 * @brief 启动线性变化
 * @param ramp 句柄指针
 * @param start 初值
 * @param end 终值
 * @param time_ms 变化总耗时(毫秒)
 */
void Ramp_Start(Ramp_Handle_t *ramp, int32_t start, int32_t end, uint32_t time_ms)
{
    ramp->startValue = start;
    ramp->endValue = end;
    ramp->duration = time_ms;
    ramp->startTime = HAL_GetTick(); // 获取当前系统滴答
    ramp->currentValue = start;
    ramp->isRunning = true;
}

/**
 * @brief 更新并获取当前值 (需在主循环中不断调用)
 * @param ramp 句柄指针
 * @return 当前时刻应该输出的整形值
 */
int32_t Ramp_Update(Ramp_Handle_t *ramp)
{
    // 如果没启动，直接返回当前保持的值（通常是终值或初值）
    if (!ramp->isRunning)
    {
        return ramp->currentValue;
    }

    uint32_t now = HAL_GetTick();
    uint32_t elapsed = now - ramp->startTime; // 计算已过去的时间

    // 检查是否时间已到
    if (elapsed >= ramp->duration)
    {
        ramp->currentValue = ramp->endValue;
        ramp->isRunning = false; // 标记结束
    }
    else
    {
        // 线性插值公式: y = start + (end - start) * (elapsed / total_time)
        // 使用 int64_t 避免 (end - start) * elapsed 溢出
        int64_t delta = (int64_t)ramp->endValue - (int64_t)ramp->startValue;
        ramp->currentValue = ramp->startValue + (int32_t)((delta * elapsed) / ramp->duration);
    }

    return ramp->currentValue;
}
