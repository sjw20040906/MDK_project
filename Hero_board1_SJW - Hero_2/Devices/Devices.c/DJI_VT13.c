/**
 * @file DJI_VT13.c
 * @author SJW
 * @brief
 * @version 1.0
 * @date 2025-10-23
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "DJI_VT13.h"

extern uint32_t HAL_GetTick(void);
VTM_Frame DJI_VT13_Data;
KeyMouseDetect_t KeyMouseDetect;
uint8_t DJI_VT13_RX_Buf[VTM_FRAME_LEN];
uint8_t DJI_VT13_RX_Finish;
static int RampRate_ForwardBack = 50; // 斜坡函数叠加值
static int RampRate_LeftRight = 50;   // 斜坡函数叠加值
static int F_Lock = 0;
//static int ctrl_Lock = 0;
static int8_t spin_gear = 0;     // 档位：-3,-2,-1,0,1,2,3
static uint8_t q_combo_lock = 0; // Shift+Q 组合按键沿检测锁
static uint8_t e_combo_lock = 0; // Shift+E 组合按键沿检测锁

KeyDetect_t btn_right_detect;
KeyDetect_t btn_left_detect;
KeyDetect_t btn_pasue_detect;

/**
 * @brief  DJI_VT13初始化，开启接收空闲中断
 * @param  void
 * @retval void
 */
void DJI_VT13_Init(void)
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart6, DJI_VT13_RX_Buf, sizeof(DJI_VT13_RX_Buf));
}

void KeyDetect_Update(KeyDetect_t *key, uint8_t current_state)
{
    uint32_t now = HAL_GetTick();

    key->short_press_flag = 0;
    key->long_press_flag = 0;
    key->press_flag = 0;

    // ---- 按下瞬间 ----
    if (current_state == 1 && key->last_state == 0)
    {
        key->press_time = now;
        key->long_press_report = 0;
    }

    // ---- 持续按下 ----
    if (current_state == 1 && key->last_state == 1)
    {
        uint32_t duration = now - key->press_time;

        // 满足有效按下条件
        if (duration >= PRESS_THRESHOLD)
        {
            key->press_flag = 1;
        }

        // 检测长按
        if (duration >= LONG_PRESS_THRESHOLD && key->long_press_report == 0)
        {
            key->long_press_flag = 1;
            key->long_press_report = 1;
        }
    }

    // ---- 松开瞬间 ----
    if (current_state == 0 && key->last_state == 1)
    {
        uint32_t duration = now - key->press_time;

        // 松开前若时间较短，视为短按
        if (duration < SHORT_PRESS_THRESHOLD)
        {
            key->short_press_flag = 1;
        }
    }

    key->last_state = current_state;
    key->key_state = current_state;
}

/* 从 bit 流中提取任意位段（小端位序：bit0 是 byte0 的 LSB） */
static uint32_t get_bits_le(const uint8_t *bytes, uint32_t bit_off, uint8_t bit_len)
{
    uint32_t v = 0;
    for (uint8_t i = 0; i < bit_len; ++i)
    {
        uint32_t bi = bit_off + i;
        uint8_t byte_idx = (uint8_t)(bi >> 3);
        uint8_t bit_idx = (uint8_t)(bi & 7u); // 0 = LSB
        uint8_t bit = (bytes[byte_idx] >> bit_idx) & 0x01u;
        v |= ((uint32_t)bit) << i; // 低位在前（LE）
    }
    return v;
}

static void unpack_keyboard_bits(uint16_t raw, VTM_KeyboardBits *kb)
{
    kb->W = (raw >> 0) & 1;
    kb->S = (raw >> 1) & 1;
    kb->A = (raw >> 2) & 1;
    kb->D = (raw >> 3) & 1;
    kb->Shift = (raw >> 4) & 1;
    kb->Ctrl = (raw >> 5) & 1;
    kb->Q = (raw >> 6) & 1;
    kb->E = (raw >> 7) & 1;
    kb->R = (raw >> 8) & 1;
    kb->F = (raw >> 9) & 1;
    kb->G = (raw >> 10) & 1;
    kb->Z = (raw >> 11) & 1;
    kb->X = (raw >> 12) & 1;
    kb->C = (raw >> 13) & 1;
    kb->V = (raw >> 14) & 1;
    kb->B = (raw >> 15) & 1;
}

/**
 * @brief 键鼠检测统一更新函数
 * @param km   键鼠检测结构体指针
 * @param data 当前解析的遥控帧数据（DJI_VT13_Data）
 */
void KeyMouse_Update(KeyMouseDetect_t *km, const VTM_Frame *data)
{
    // 更新鼠标位移
    km->dx = data->mouse_x;
    km->dy = data->mouse_y;
    km->dz = data->mouse_z;

    // 更新鼠标按键
    KeyDetect_Update(&km->mouse_l, data->mouse_l);
    KeyDetect_Update(&km->mouse_r, data->mouse_r);
    KeyDetect_Update(&km->mouse_m, data->mouse_m);

    // 更新键盘主要按键
    KeyDetect_Update(&km->W, data->kb.W);
    KeyDetect_Update(&km->S, data->kb.S);
    KeyDetect_Update(&km->A, data->kb.A);
    KeyDetect_Update(&km->D, data->kb.D);
    KeyDetect_Update(&km->Q, data->kb.Q);
    KeyDetect_Update(&km->E, data->kb.E);
    KeyDetect_Update(&km->R, data->kb.R);
    KeyDetect_Update(&km->F, data->kb.F);
    KeyDetect_Update(&km->G, data->kb.G);
    KeyDetect_Update(&km->Z, data->kb.Z);
    KeyDetect_Update(&km->X, data->kb.X);
    KeyDetect_Update(&km->C, data->kb.C);
    KeyDetect_Update(&km->V, data->kb.V);
    KeyDetect_Update(&km->B, data->kb.B);
    KeyDetect_Update(&km->Shift, data->kb.Shift);
    KeyDetect_Update(&km->Ctrl, data->kb.Ctrl);

    // 云台控制
    ControlMes.yaw_velocity = km->dx * 5;
    ControlMes.pitch_velocity = km->dy * 8;

    /*****************清空******************/
    if (km->R.press_flag) // r
    {
        ControlMes.z_rotation_velocity = 0;
        q_combo_lock = 0;
        e_combo_lock = 0;
        spin_gear = 0;
        ControlMes.change_Flag = 0;
    }

    // 鼠标短按 / 长按
    if ((km->mouse_l.press_flag) && ControlMes.fric_Flag == TURN_ON)
    {
        Dial_Data.Shoot_Mode = Single_Shoot;
        Shoot_Data.Shoot_Switch = TURN_ON;
    }
    else
    {
        Dial_Data.Shoot_Mode = No_Shoot;
        Shoot_Data.Shoot_Switch = TURN_OFF;
    }

    /*****************自瞄开关右键****************/
    if (km->mouse_r.short_press_flag)
    {
        if (ControlMes.AutoAimFlag == 0)
        {
            ControlMes.AutoAimFlag = 1;
        }
        else if (ControlMes.AutoAimFlag == 1)
        {
            ControlMes.AutoAimFlag = 0;
        }
    }
    if (ControlMes.AutoAimFlag == 1)
    {
        ControlMes.yaw_position = Auto_Aim_Yaw;
        Cloud.AutoAim_Pitch = Auto_Aim_Pitch;
    }

    /*****************RESET_FLAG *****************/
    if (km->mouse_m.short_press_flag)
    {
        ControlMes.reset_Flag = TURN_ON;
    }
    else
    {
        ControlMes.reset_Flag = TURN_OFF;
    }

    /*********************前后控制*******************/
    if (km->W.press_flag)
    {
        ChassisRamp_ForwardBack.rate = RampRate_ForwardBack;
    }
    else if (km->S.press_flag)
    {
        ChassisRamp_ForwardBack.rate = -RampRate_ForwardBack;
    }
    else
    {
        CountReset(&ChassisRamp_ForwardBack);
        ChassisRamp_ForwardBack.rate = 0;
    }
    ControlMes.x_velocity = -SpeedRampCalc(&ChassisRamp_ForwardBack);

    /********************左右控制*******************/
    if (km->A.press_flag)
    {
        ChassisRamp_LeftRight.rate = RampRate_LeftRight;
    }
    else if (km->D.press_flag)
    {
        ChassisRamp_LeftRight.rate = -RampRate_LeftRight;
    }
    else
    {
        CountReset(&ChassisRamp_LeftRight);
        ChassisRamp_LeftRight.rate = 0;
    }
    ControlMes.y_velocity = SpeedRampCalc(&ChassisRamp_LeftRight);

    /********************小陀螺控制*******************/
    float pos_max = (float)ChassisRamp_Rotate.maxcount; // 通常为正
    float neg_min = (float)ChassisRamp_Rotate.mincount; // 通常为负
    float abs_cap = pos_max;                            // 正负对称能力上限
    if (-neg_min < abs_cap)
    {
        abs_cap = -neg_min; // 取两侧较小的绝对值
    }
    float step = abs_cap / 3.0f; // 每档对应的角速度幅值

    uint8_t q_combo = (km->Shift.press_flag) && (km->Q.press_flag);
    if (q_combo && !q_combo_lock)
    {
        if (spin_gear < 3)
        {
            spin_gear++;
        } // 加一挡
        q_combo_lock = 1;
    }
    else if (!q_combo)
    {
        q_combo_lock = 0; // 松开复位，便于下次触发
    }
    // —— 挡位切换：Shift+Q 加档；Shift+E 减档（沿触发） —— //
    uint8_t e_combo = (km->Shift.press_flag) && (km->E.press_flag);
    if (e_combo && !e_combo_lock)
    {
        if (spin_gear > -3)
        {
            spin_gear--;
        } // 减一挡
        e_combo_lock = 1;
    }
    else if (!e_combo)
    {
        e_combo_lock = 0;
    }

    // —— 档位映射到目标角速度 —— //
    float target_wz = (float)spin_gear * step;

    // —— 平滑斜坡（避免瞬变）：按固定坡度逼近目标 —— //
    const float rate_per_tick = 30.0f; // 例如每周期最多变化 30（与原来每步±10、累计80步相当的平滑感）
    if (ControlMes.z_rotation_velocity < target_wz)
    {
        ControlMes.z_rotation_velocity += rate_per_tick;
        if (ControlMes.z_rotation_velocity > target_wz)
        {
            ControlMes.z_rotation_velocity = target_wz;
        }
    }
    else if (ControlMes.z_rotation_velocity > target_wz)
    {
        ControlMes.z_rotation_velocity -= rate_per_tick;
        if (ControlMes.z_rotation_velocity < target_wz)
        {
            ControlMes.z_rotation_velocity = target_wz;
        }
    }

    /********************摩擦轮控制F键*******************/
    if (km->F.press_flag)
    {
        if (F_Lock == 0)
        {
            if (ControlMes.fric_Flag == TURN_OFF)
            {
                ControlMes.fric_Flag = TURN_ON;
            }
            else if (ControlMes.fric_Flag == TURN_ON)
            {
                ControlMes.fric_Flag = TURN_OFF;
            }
            F_Lock = 1;
        }
    }
    else
    {
        F_Lock = 0;
    }

    //                                                                if (km->G.short_press_flag)

    //                                                                    if (km->G.long_press_flag)

    //                                                                        if (km->Z.short_press_flag)

    //                                                                            if (km->Z.long_press_flag)

    //                                                                                if (km->X.short_press_flag)

    //                                                                                    if (km->X.long_press_flag)

    //                                                                                        if (km->C.short_press_flag)

    //                                                                                            if (km->C.long_press_flag)

    //                                                                                                if (km->V.short_press_flag)

    //                                                                                                    if (km->V.long_press_flag)

    //                                                                                                        if (km->B.short_press_flag)

    //                                                                                                            if (km->B.long_press_flag)

    /**********************切换模式Ctrl*******************/
    if (km->Ctrl.short_press_flag)
    {
        if (ControlMes.modelFlag == 2)
        {
            ControlMes.modelFlag = 0;
        }
        else if (ControlMes.modelFlag == 0)
        {
            ControlMes.modelFlag = 2;
        }
    }
}
void DJI_VT13_Handle(void)
{

    if (DJI_VT13_RX_Finish == 1)
    {
        DJI_VT13_RX_Finish = 0;

        DJI_VT13_Data.header_ok = (DJI_VT13_RX_Buf[0] == VTM_HEADER0) && (DJI_VT13_RX_Buf[1] == VTM_HEADER1);
        if (!DJI_VT13_Data.header_ok)
            return;

        uint32_t off = 16;
        DJI_VT13_Data.ch[0] = (int16_t)get_bits_le(DJI_VT13_RX_Buf, off + 0, 11) - 1024;  // 右手左右
        DJI_VT13_Data.ch[1] = (int16_t)get_bits_le(DJI_VT13_RX_Buf, off + 11, 11) - 1024; // 右手前后
        DJI_VT13_Data.ch[2] = (int16_t)get_bits_le(DJI_VT13_RX_Buf, off + 22, 11) - 1024; // 左手前后
        DJI_VT13_Data.ch[3] = (int16_t)get_bits_le(DJI_VT13_RX_Buf, off + 33, 11) - 1024; // 左手左右

        DJI_VT13_Data.mode = (VTM_ModeSwitch)get_bits_le(DJI_VT13_RX_Buf, 60, 2);
        DJI_VT13_Data.pause = (uint8_t)get_bits_le(DJI_VT13_RX_Buf, 62, 1);
        DJI_VT13_Data.btn_left = (uint8_t)get_bits_le(DJI_VT13_RX_Buf, 63, 1);
        DJI_VT13_Data.btn_right = (uint8_t)get_bits_le(DJI_VT13_RX_Buf, 64, 1);

        DJI_VT13_Data.wheel = (int16_t)get_bits_le(DJI_VT13_RX_Buf, 65, 11) - 1024;
        DJI_VT13_Data.trigger = (uint8_t)get_bits_le(DJI_VT13_RX_Buf, 76, 1);

        // mouse x/y/z
        DJI_VT13_Data.mouse_x = (int16_t)(get_bits_le(DJI_VT13_RX_Buf, 80, 16));
        DJI_VT13_Data.mouse_y = (int16_t)(get_bits_le(DJI_VT13_RX_Buf, 96, 16));
        DJI_VT13_Data.mouse_z = (int16_t)(get_bits_le(DJI_VT13_RX_Buf, 112, 16));

        // 鼠标键
        DJI_VT13_Data.mouse_l = (uint8_t)get_bits_le(DJI_VT13_RX_Buf, 128, 2);
        DJI_VT13_Data.mouse_r = (uint8_t)get_bits_le(DJI_VT13_RX_Buf, 130, 2);
        DJI_VT13_Data.mouse_m = (uint8_t)get_bits_le(DJI_VT13_RX_Buf, 132, 2);

        // 键盘 16bit 位图
        DJI_VT13_Data.keyboard_bits = (uint16_t)get_bits_le(DJI_VT13_RX_Buf, 136, 16);
        unpack_keyboard_bits(DJI_VT13_Data.keyboard_bits, &DJI_VT13_Data.kb);

        /**************************** control code ****************************/

        /*中正常遥控；右自瞄；左键鼠*/
        // 正常遥控
        if (DJI_VT13_Data.mode == VTM_MODE_N)
        {
            /******************************遥控器数值传递******************************/
            // 底盘运动控制
            ControlMes.x_velocity = -DJI_VT13_Data.ch[2];
            ControlMes.y_velocity = -DJI_VT13_Data.ch[3];

            // 云台运动控制
            ControlMes.AutoAimFlag = TURN_OFF;
            ControlMes.pitch_velocity = DJI_VT13_Data.ch[1]*0.8;
            ControlMes.yaw_velocity = DJI_VT13_Data.ch[0];
            ControlMes.z_rotation_velocity = -DJI_VT13_Data.wheel;
            ControlMes.yaw_position = Auto_Aim_Yaw;

            KeyDetect_Update(&btn_right_detect, DJI_VT13_Data.btn_right);
            KeyDetect_Update(&btn_left_detect, DJI_VT13_Data.btn_left);
            KeyDetect_Update(&btn_pasue_detect, DJI_VT13_Data.pause);
            // 发射状态设置
            if (btn_right_detect.short_press_flag)
            {
                ControlMes.fric_Flag = !ControlMes.fric_Flag;
            }

            if (ControlMes.fric_Flag == TURN_ON)
            {
                if (DJI_VT13_Data.trigger == PRESS_flag_YES)
                {
                    Dial_Data.Shoot_Mode = Single_Shoot;
                    Dial_Data.Dial_Switch = Dial_On;
                }
                else
                {
                    Dial_Data.Shoot_Mode = Single_Shoot;
                    Dial_Data.Dial_Switch = Dial_Off;
                }
            }
            else
            {
                Shoot_Data.Shoot_Switch = TURN_OFF;
                Dial_Data.Dial_Switch = Dial_On;
            }

            // 模式设置
            if (btn_pasue_detect.short_press_flag) // 模式0正常，模式1检录，模式2跟随
            {
                ControlMes.modelFlag++;
                if (ControlMes.modelFlag > 2)
                    ControlMes.modelFlag = 0;
            }

            // 重置标志设置
            if (btn_left_detect.short_press_flag)
            {
                ControlMes.reset_Flag = TURN_ON;
            }
            else
            {
                ControlMes.reset_Flag = TURN_OFF;
            }
        }
        // 自瞄模式
        else if (DJI_VT13_Data.mode == VTM_MODE_S)
        {
            /******************************遥控器数值传递******************************/
            // 底盘运动控制
            ControlMes.x_velocity = DJI_VT13_Data.ch[2];
            ControlMes.y_velocity = -DJI_VT13_Data.ch[3];

            // 云台运动控制
            ControlMes.AutoAimFlag = TURN_ON;
            ControlMes.pitch_velocity = DJI_VT13_Data.ch[1] * 0.2;
            ControlMes.yaw_velocity = DJI_VT13_Data.ch[0];
            ControlMes.z_rotation_velocity = -DJI_VT13_Data.wheel;
            ControlMes.yaw_position = Auto_Aim_Yaw;
            Cloud.AutoAim_Pitch = Auto_Aim_Pitch;

            KeyDetect_Update(&btn_right_detect, DJI_VT13_Data.btn_right);
            KeyDetect_Update(&btn_left_detect, DJI_VT13_Data.btn_left);
            KeyDetect_Update(&btn_pasue_detect, DJI_VT13_Data.pause);
            // 发射状态设置
            if (btn_right_detect.short_press_flag)
            {
                ControlMes.fric_Flag = !ControlMes.fric_Flag;
            }

            if (ControlMes.fric_Flag == TURN_ON)
            {
                if (DJI_VT13_Data.trigger == PRESS_flag_YES)
                {
                    Dial_Data.Shoot_Mode = Single_Shoot;
                    Shoot_Data.Shoot_Switch = TURN_ON;
                }
                else
                {
                    Dial_Data.Shoot_Mode = Single_Shoot;
                    Shoot_Data.Shoot_Switch = TURN_OFF;
                }
            }
            else
            {
                Shoot_Data.Shoot_Switch = TURN_OFF;
            }

            // 模式设置
            if (btn_pasue_detect.short_press_flag) // 模式0正常，模式1检录，模式2跟随
            {
                ControlMes.modelFlag++;
                if (ControlMes.modelFlag > 2)
                    ControlMes.modelFlag = 0;
            }

            // 重置标志设置
            if (btn_left_detect.short_press_flag)
            {
                ControlMes.reset_Flag = TURN_ON;
            }
            else
            {
                ControlMes.reset_Flag = TURN_OFF;
            }
        }
        // 键鼠模式
        else if (DJI_VT13_Data.mode == VTM_MODE_C)
        {
            KeyMouse_Update(&KeyMouseDetect, &DJI_VT13_Data);
        }
        else
        {
            ControlMes.AutoAimFlag = 0;
            ControlMes.x_velocity = 0;          // 左手上下
            ControlMes.y_velocity = 0;          // 左手左右
            ControlMes.z_rotation_velocity = 0; // 右手上下
            ControlMes.yaw_velocity = 0;
            ControlMes.pitch_velocity = 0;
        }
    }
    // 用board1 CAN2发送给board2。
    Board1_To_2();
}
