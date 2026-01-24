#include "SBUS.h"

SBUS_Buffer SBUS;
MappedData mappedData;
uint8_t SBUS_RX_Finish = 0;
uint8_t SBUS_RXBuffer[SBUS_RX_LEN] = {0};
uint8_t SBUS_Rx_Data[25];

/**
 * @brief  SBUS初始化，开启接收空闲中断
 * @param  void
 * @retval void
 */
void SBUS_Init()
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart3, SBUS_Rx_Data, sizeof(SBUS_Rx_Data));
}

/**
 * @brief 线性映射函数：将353~1695映射到-1024~1024
 * @param sbus_val: SBUS原始通道值
 * @retval 映射后的数值
 */
static int16_t map_to_1024(int16_t sbus_val)
{
    // 限制输入范围，防止异常值
    if (sbus_val < SBUS_MIN)
        sbus_val = SBUS_MIN;
    if (sbus_val > SBUS_MAX)
        sbus_val = SBUS_MAX;

    // 线性映射公式：y = (x - x_min) * (y_max - y_min)/(x_max - x_min) + y_min
    return (int16_t)((sbus_val - SBUS_MIN) * 2048.0f / (SBUS_MAX - SBUS_MIN) - 1024);
}

/**
 * @brief 通道档位转换：353→1，1024→2，1695→3
 * @param sbus_val: SBUS原始通道值
 * @retval 1/2/3档位
 */
static uint8_t map_to_3levels(int16_t sbus_val)
{
    if (sbus_val <= SBUS_MIN + TOLERANCE)
    {
        return 1;
    }
    else if (sbus_val >= SBUS_MAX - TOLERANCE)
    {
        return 3;
    }
    else if (sbus_val >= MID_VALUE - TOLERANCE && sbus_val <= MID_VALUE + TOLERANCE)
    {
        return 2;
    }
    return 0;
}

/**
 * @brief 通道档位转换：353→1，1695→2
 * @param sbus_val: SBUS原始通道值
 * @retval 1/2档位
 */
static uint8_t map_to_2levels(int16_t sbus_val)
{
    if (sbus_val <= SBUS_MIN + TOLERANCE)
    {
        return 1;
    }
    else if (sbus_val >= SBUS_MAX - TOLERANCE)
    {
        return 2;
    }
    return 0;
}

/**
 * @brief 解析遥控器数据并完成通道映射
 * @param  void
 * @retval void
 */
void SBUS_Handle()
{
    if (SBUS_RX_Finish == 1)
    {
        SBUS_RX_Finish = 0;

        // 帧头帧尾校验
        if (SBUS_RXBuffer[0] == 0x0F && SBUS_RXBuffer[24] == 0x00)
        {
            // 1. 解析原始通道数据（保持原解析逻辑）
            SBUS.Ch1 = ((uint16_t)SBUS_RXBuffer[1]) | ((uint16_t)((SBUS_RXBuffer[2] & 0x07) << 8));
            SBUS.Ch2 = ((uint16_t)((SBUS_RXBuffer[2] & 0xf8) >> 3)) | (((uint16_t)(SBUS_RXBuffer[3] & 0x3f)) << 5);
            SBUS.Ch3 = ((uint16_t)((SBUS_RXBuffer[3] & 0xc0) >> 6)) | ((((uint16_t)SBUS_RXBuffer[4]) << 2)) | (((uint16_t)(SBUS_RXBuffer[5] & 0x01)) << 10);
            SBUS.Ch4 = ((uint16_t)((SBUS_RXBuffer[5] & 0xfe) >> 1)) | (((uint16_t)(SBUS_RXBuffer[6] & 0x0f)) << 7);
            SBUS.Ch5 = ((uint16_t)((SBUS_RXBuffer[6] & 0xf0) >> 4)) | (((uint16_t)(SBUS_RXBuffer[7] & 0x7f)) << 4);
            SBUS.Ch6 = ((uint16_t)((SBUS_RXBuffer[7] & 0x80) >> 7)) | (((uint16_t)SBUS_RXBuffer[8]) << 1) | (((uint16_t)(SBUS_RXBuffer[9] & 0x03)) << 9);
            SBUS.Ch7 = ((uint16_t)((SBUS_RXBuffer[9] & 0xfc) >> 2)) | (((uint16_t)(SBUS_RXBuffer[10] & 0x1f)) << 6);
            SBUS.Ch8 = ((uint16_t)((SBUS_RXBuffer[10] & 0xe0) >> 5)) | (((uint16_t)(SBUS_RXBuffer[11])) << 3);
            SBUS.Ch9 = ((uint16_t)SBUS_RXBuffer[12]) | (((uint16_t)(SBUS_RXBuffer[13] & 0x07)) << 8);
            SBUS.Ch10 = ((uint16_t)((SBUS_RXBuffer[13] & 0xf8) >> 3)) | (((uint16_t)(SBUS_RXBuffer[14] & 0x3f)) << 5);
            SBUS.Ch11 = ((uint16_t)((SBUS_RXBuffer[14] & 0xc0) >> 6)) | (((uint16_t)SBUS_RXBuffer[15]) << 2) | (((uint16_t)(SBUS_RXBuffer[16] & 0x01)) << 10);
            SBUS.Ch12 = ((uint16_t)((SBUS_RXBuffer[16] & 0xfe) >> 1)) | (((uint16_t)(SBUS_RXBuffer[17] & 0x0f)) << 7);
            SBUS.Ch13 = ((uint16_t)((SBUS_RXBuffer[17] & 0xf0) >> 4)) | (((uint16_t)(SBUS_RXBuffer[18] & 0x7f)) << 4);
            SBUS.Ch14 = ((uint16_t)((SBUS_RXBuffer[18] & 0x80) >> 7)) | (((uint16_t)SBUS_RXBuffer[19]) << 1) | (((uint16_t)(SBUS_RXBuffer[20] & 0x03)) << 9);
            SBUS.Ch15 = ((uint16_t)((SBUS_RXBuffer[20] & 0xfc) >> 2)) | (((uint16_t)(SBUS_RXBuffer[21] & 0x1f)) << 6);
            SBUS.Ch16 = ((uint16_t)((SBUS_RXBuffer[21] & 0xe0) >> 5)) | (((uint16_t)SBUS_RXBuffer[22]) << 3);

            // 2. 通道数据映射
            mappedData.Ch1 = map_to_1024(SBUS.Ch1);
            mappedData.Ch2 = map_to_1024(SBUS.Ch2);
            mappedData.Ch3 = map_to_1024(SBUS.Ch3);
            mappedData.Ch4 = map_to_1024(SBUS.Ch4);
            mappedData.Ch5 = map_to_3levels(SBUS.Ch5);
            mappedData.Ch6 = map_to_3levels(SBUS.Ch6);
            mappedData.Ch7 = map_to_2levels(SBUS.Ch7);
            mappedData.Ch8 = map_to_2levels(SBUS.Ch8);
            mappedData.Ch9 = map_to_1024(SBUS.Ch9);
            mappedData.Ch10 = map_to_1024(SBUS.Ch10);
        }
    }

    Board2_1_To_Board2_2();
}
