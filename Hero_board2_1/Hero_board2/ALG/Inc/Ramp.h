#ifndef __RAMP_H__   
#define __RAMP_H__   

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    int32_t startValue;   // 初始值
    int32_t endValue;     // 终值
    uint32_t duration;    // 持续时间 (ms)
    uint32_t startTime;   // 开始时刻 (Tick)
    bool isRunning;       // 运行状态标志
    int32_t currentValue; // 当前计算出的值
} Ramp_Handle_t;

extern Ramp_Handle_t myRamp;

void Ramp_Start(Ramp_Handle_t *ramp, int32_t start, int32_t end, uint32_t time_ms);
int32_t Ramp_Update(Ramp_Handle_t *ramp);

#endif /* __RAMP_H__ */
