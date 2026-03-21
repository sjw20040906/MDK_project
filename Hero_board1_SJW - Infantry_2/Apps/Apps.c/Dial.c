#include "Dial.h"

/**************数据定义****************/
static uint32_t last_check_time = 0;     // 上一次检测时间
static uint8_t is_reversing = 0;         // 反转标志
static uint32_t reverse_start_time = 0;  // 反转开始时间

/****************函数结构体声明******************/
Dial_Data_t Dial_Data = Dial_DataGroundInit;
#undef Dial_DataGroundInit

/**
 * @brief 拨弹处理函数
 * @note 只进行正常拨弹和卡弹处理
 */
void Dial_Processing(void)
{
    if (Dial_Data.Shoot_Mode == Continuous_Shoot && Dial_Data.Dial_Switch == Dial_On)
    {
        Bullet_Stuck_Processing();

        if (!is_reversing)
        {
            Normal_Dial();
        }
    }
    else
    {
        Status_Refresh();
    }
}

/**
 * @brief 正常拨弹执行
 */
void Normal_Dial(void)
{
    M2006_Array[Dial_Motor].targetSpeed = Dial_Data.Speed_Dial;
    M2006_Array[Dial_Motor].outCurrent = PID_Model4_Update(&M2006_DialI_Pid,&fuzzy_pid_bullet_v,M2006_Array[Dial_Motor].targetSpeed,M2006_Array[Dial_Motor].realSpeed);
}

/**
 * @brief 卡弹检测与反转处理
 * @note 通过检测真实扭矩判断是否堵转
 */
void Bullet_Stuck_Processing(void)
{
    uint32_t now = HAL_GetTick();

    /* 正在反转，保持反转直到时间结束 */
    if (is_reversing)
    {
        if (now - reverse_start_time >= REVERSE_DURATION)
        {
            is_reversing = 0;
        }
        else
        {
            M2006_Array[Dial_Motor].targetSpeed = DIAL_REVERSE_SPEED;
            M2006_Array[Dial_Motor].outCurrent = PID_Model4_Update(&M2006_DialI_Pid, &fuzzy_pid_bullet_v, M2006_Array[Dial_Motor].targetSpeed, M2006_Array[Dial_Motor].realSpeed);
            return;
        }
    }

    /* 定时检测真实扭矩 */
    if (now - last_check_time >= CHECK_INTERVAL)
    {
        last_check_time = now;
        if (abs(M2006_Array[Dial_Motor].realTorque) > DIAL_TORQUE_THRESHOLD)
        {
            is_reversing = 1;
            reverse_start_time = now;

            M2006_Array[Dial_Motor].targetSpeed = DIAL_REVERSE_SPEED;
            M2006_Array[Dial_Motor].outCurrent = PID_Model4_Update(&M2006_DialI_Pid,  &fuzzy_pid_bullet_v,  M2006_Array[Dial_Motor].targetSpeed,  M2006_Array[Dial_Motor].realSpeed);
        }
    }
}

/**
 * @brief 状态刷新
 */
void Status_Refresh(void)
{
    is_reversing = 0;
    M2006_Array[Dial_Motor].targetSpeed = 0;
    M2006_Array[Dial_Motor].outCurrent =  PID_Model4_Update(&M2006_DialI_Pid,  &fuzzy_pid_bullet_v,  0,  M2006_Array[Dial_Motor].realSpeed);
}
