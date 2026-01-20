/**
 * @file Remote_KeyMouse.c
 * @brief 适配新协议(0x0304)的键鼠解析代码，仿照原DJI_VT13风格
 * @version 2.0
 * @date 2026-01-19
 */

#include "Remote_KeyMouse.h" // 假设你的结构体定义在这里，需要根据下方说明修改.h

// 底盘遥控前后斜坡
static SpeedRamp_t ChassisRamp_ForwardBack_test = ForwardBackGroundInit;

// 底盘遥控左右斜坡
static SpeedRamp_t ChassisRamp_LeftRight_test = LeftRightGroundInit;

// 底盘遥控左右转斜坡
static SpeedRamp_t ChassisRamp_Rotate_test = RotateGroundInit;

extern uint32_t HAL_GetTick(void);

// -------- 全局变量定义 --------
ext_robot_keycommand_t Robot_KeyData; // 接收到的原始数据结构体
KeyMouseDetect_t KeyMouseDetect;      // 处理后的按键状态结构体
uint8_t KM_RX_Buf[256];               // 接收缓冲区
uint8_t KM_RX_Finish;                 // 接收完成标志

// -------- 逻辑控制变量 --------
static int RampRate_ForwardBack = 80; // 斜坡增量
static int RampRate_LeftRight = 80;
static int F_Lock = 0;
static int8_t spin_gear = 0;
static uint8_t q_combo_lock = 0;
static uint8_t e_combo_lock = 0;

// -------- 协议常量 --------
#define PROTOCOL_SOF 0xA5
#define PROTOCOL_CMD_ID 0x0304

/**
 * @brief 初始化，开启空闲中断
 */
void Remote_KeyMouse_Init(void)
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart6, KM_RX_Buf, 256);
}

/**
 * @brief 按键状态机更新 (完全保留你的原逻辑)
 */
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
        if (duration >= PRESS_THRESHOLD)
        {
            key->press_flag = 1;
        }
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
        if (duration < SHORT_PRESS_THRESHOLD)
        {
            key->short_press_flag = 1;
        }
    }

    key->last_state = current_state;
    key->key_state = current_state;
}

/**
 * @brief 解包键盘的16位数据 (适配新协议位定义)
 * @param raw 16位的 keyboard_value
 * @param kb  目标按键位结构体
 */
static void unpack_keyboard_bits(uint16_t raw, VTM_KeyboardBits *kb)
{
    // 根据图片 bit 0 - 15 的定义
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
 * @brief 键鼠逻辑处理核心函数
 */
void KeyMouse_Update(KeyMouseDetect_t *km, const ext_robot_keycommand_t *raw_data)
{
    // 1. 解包键盘位
    VTM_KeyboardBits temp_kb_bits;
    unpack_keyboard_bits(raw_data->keyboard_value, &temp_kb_bits);

    // 2. 更新鼠标物理量
    km->dx = raw_data->mouse_x;
    km->dy = raw_data->mouse_y;
    km->dz = raw_data->mouse_z;

    KeyDetect_Update(&km->mouse_l, raw_data->left_button_down);
    KeyDetect_Update(&km->mouse_r, raw_data->right_button_down);
    KeyDetect_Update(&km->W, temp_kb_bits.W);
    KeyDetect_Update(&km->S, temp_kb_bits.S);
    KeyDetect_Update(&km->A, temp_kb_bits.A);
    KeyDetect_Update(&km->D, temp_kb_bits.D);
    KeyDetect_Update(&km->Shift, temp_kb_bits.Shift);
    KeyDetect_Update(&km->Ctrl, temp_kb_bits.Ctrl);
    KeyDetect_Update(&km->Q, temp_kb_bits.Q);
    KeyDetect_Update(&km->E, temp_kb_bits.E);
    KeyDetect_Update(&km->R, temp_kb_bits.R);
    KeyDetect_Update(&km->F, temp_kb_bits.F);
    KeyDetect_Update(&km->G, temp_kb_bits.G);
    KeyDetect_Update(&km->Z, temp_kb_bits.Z);
    KeyDetect_Update(&km->X, temp_kb_bits.X);
    KeyDetect_Update(&km->C, temp_kb_bits.C);
    KeyDetect_Update(&km->V, temp_kb_bits.V);
    KeyDetect_Update(&km->B, temp_kb_bits.B);

    // ================== 业务逻辑区 ==================

    // ---- 云台控制 (鼠标移动) ----
    ControlMes.yaw_velocity = km->dx * 20;
    ControlMes.pitch_velocity = km->dy * 12;

    // ---- R键：重置状态 ----
    if (km->R.press_flag)
    {
        ControlMes.z_rotation_velocity = 0;
        q_combo_lock = 0;
        e_combo_lock = 0;
        spin_gear = 0;
        ControlMes.change_Flag = 0;
    }

    // ---- 鼠标左键：射击 ----
    if ((km->mouse_l.press_flag) && ControlMes.fric_Flag == TURN_ON)
    {
        Dial_Data.Shoot_Mode = Single_Shoot;
        Shoot_Data.Shoot_Switch = TURN_ON;
        Dial_Data.Dial_Switch = Dial_On;
    }
    else
    {
        Dial_Data.Shoot_Mode = No_Shoot;
        Shoot_Data.Shoot_Switch = TURN_OFF;
        Dial_Data.Dial_Switch = Dial_Off;
    }

    // ---- 鼠标右键：自瞄开关 ----
    if (km->mouse_r.short_press_flag)
    {
        ControlMes.AutoAimFlag = !ControlMes.AutoAimFlag; // 简化的取反写法
    }
    if (ControlMes.AutoAimFlag == 1)
    {
        ControlMes.yaw_position = Auto_Aim_Yaw;
        Cloud.AutoAim_Pitch = Auto_Aim_Pitch;
    }

    // ---- 底盘前后 (W/S) ----
    if (km->W.press_flag)
    {
        ChassisRamp_ForwardBack_test.rate = RampRate_ForwardBack;
    }
    else if (km->S.press_flag)
    {
        ChassisRamp_ForwardBack_test.rate = -RampRate_ForwardBack;
    }
    else
    {
        CountReset(&ChassisRamp_ForwardBack_test);
        ChassisRamp_ForwardBack_test.rate = 0;
    }
    ControlMes.x_velocity = -SpeedRampCalc(&ChassisRamp_ForwardBack_test);

    // ---- 底盘左右 (A/D) ----
    if (km->A.press_flag)
    {
        ChassisRamp_LeftRight_test.rate = RampRate_LeftRight;
    }
    else if (km->D.press_flag)
    {
        ChassisRamp_LeftRight_test.rate = -RampRate_LeftRight;
    }
    else
    {
        CountReset(&ChassisRamp_LeftRight_test);
        ChassisRamp_LeftRight_test.rate = 0;
    }
    ControlMes.y_velocity = SpeedRampCalc(&ChassisRamp_LeftRight_test);

    // ---- 小陀螺 / 旋转控制 (Shift + Q/E) ----
    // 复用你的逻辑，保留不变
    float pos_max = (float)ChassisRamp_Rotate_test.maxcount;
    float neg_min = (float)ChassisRamp_Rotate_test.mincount;
    float abs_cap = pos_max;
    if (-neg_min < abs_cap)
        abs_cap = -neg_min;
    float step = abs_cap / 3.0f;

    uint8_t q_combo = (km->Shift.press_flag) && (km->Q.press_flag);
    if (q_combo && !q_combo_lock)
    {
        if (spin_gear < 3)
            spin_gear++;
        q_combo_lock = 1;
    }
    else if (!q_combo)
        q_combo_lock = 0;

    uint8_t e_combo = (km->Shift.press_flag) && (km->E.press_flag);
    if (e_combo && !e_combo_lock)
    {
        if (spin_gear > -3)
            spin_gear--;
        e_combo_lock = 1;
    }
    else if (!e_combo)
        e_combo_lock = 0;

    float target_wz = (float)spin_gear * step;
    const float rate_per_tick = 30.0f;

    // 简单的旋转斜坡跟随
    if (ControlMes.z_rotation_velocity < target_wz)
    {
        ControlMes.z_rotation_velocity += rate_per_tick;
        if (ControlMes.z_rotation_velocity > target_wz)
            ControlMes.z_rotation_velocity = target_wz;
    }
    else if (ControlMes.z_rotation_velocity > target_wz)
    {
        ControlMes.z_rotation_velocity -= rate_per_tick;
        if (ControlMes.z_rotation_velocity < target_wz)
            ControlMes.z_rotation_velocity = target_wz;
    }

    // ---- F键：摩擦轮开关 ----
    if (km->F.press_flag)
    {
        if (F_Lock == 0)
        {
            ControlMes.fric_Flag = (ControlMes.fric_Flag == TURN_OFF) ? TURN_ON : TURN_OFF;
            F_Lock = 1;
        }
    }
    else
    {
        F_Lock = 0;
    }

    // ---- G键逻辑 (示例) ----
    if (km->G.short_press_flag)
    {
        // 可以在这里添加 G 键短按的功能，比如开启能量盾
    }
    if (km->G.long_press_flag)
    {
        // G 键长按功能
    }

    // ---- Z/X/C/V/B 键逻辑 (预留位置) ----
    if (km->Z.short_press_flag)
    { /* 你的逻辑 */
    }
    if (km->X.short_press_flag)
    { /* 你的逻辑 */
    }
    if (km->C.short_press_flag)
    { /* 你的逻辑 */
    }
    if (km->V.short_press_flag)
    { /* 你的逻辑 */
    }
    if (km->B.short_press_flag)
    { /* 你的逻辑 */
    }

    // ---- Ctrl键：模式切换 ----
    if (km->Ctrl.short_press_flag)
    {
        if (ControlMes.modelFlag == 2)
            ControlMes.modelFlag = 0;
        else if (ControlMes.modelFlag == 0)
            ControlMes.modelFlag = 2;
    }
}

void KeyMouse_Handle_Wrapper(void)
{

    if (KM_RX_Finish == 0)
        return;

    // 清除标志
    KM_RX_Finish = 0;

    for (int i = 0; i < 200; i++)
    {
        // 1. 找到帧头 A5
        if (KM_RX_Buf[i] == PROTOCOL_SOF)
        {
            // 2. 校验 Command ID (防止正好有个数据是 A5 但不是帧头)
            // Cmd ID 在帧头(Offset 0)后的第 5, 6 字节 -> 即 i+5, i+6
            volatile uint8_t id_low = KM_RX_Buf[i + 5];
            volatile uint8_t id_high = KM_RX_Buf[i + 6];
            uint16_t current_cmd = (uint16_t)(id_low | (id_high << 8));

            if (current_cmd == PROTOCOL_CMD_ID)
            {
                uint8_t *p = &KM_RX_Buf[i + 7];

                // 解析数据
                Robot_KeyData.mouse_x = (int16_t)(p[0] | (p[1] << 8));
                Robot_KeyData.mouse_y = (int16_t)(p[2] | (p[3] << 8));
                Robot_KeyData.mouse_z = (int16_t)(p[4] | (p[5] << 8));

                Robot_KeyData.left_button_down = p[6];
                Robot_KeyData.right_button_down = p[7];

                Robot_KeyData.keyboard_value = (uint16_t)(p[8] | (p[9] << 8));
                Robot_KeyData.reserved = (uint16_t)(p[10] | (p[11] << 8));

                Robot_KeyData.InfoUpdataFlag = 1;

                KeyMouse_Update(&KeyMouseDetect, &Robot_KeyData);
                Board1_To_2();

                return;
            }
        }
    }
}
