/**
 ******************************************************************************
 * @file    imu760.c
 * @author  sjw
 * @version V1.0.0
 * @date    2026-03-25
 * @brief   IMU760数据处理模块
 *          该文件提供IMU760初始化、数据解析和串口配置函数实现
 ******************************************************************************
 */

#include "imu760.h"

/* 全局变量 */
IMU760_Data_t imu760_data;

/* 模块内部接收缓冲区 */
uint8_t imu760_rx_raw_buffer[IMU760_RX_RAW_BUFFER_SIZE];
static uint8_t imu760_frame_buffer[IMU760_FRAME_BUFFER_SIZE];
static uint16_t imu760_frame_length = 0U;

/* 模块内部函数声明 */
static uint16_t IMU760_MinU16(uint16_t a, uint16_t b);
static uint16_t IMU760_ReadU16LE(const uint8_t *pData);
static int16_t IMU760_ReadS16LE(const uint8_t *pData);
static uint32_t IMU760_ReadU32LE(const uint8_t *pData);
static int32_t IMU760_ReadS32LE(const uint8_t *pData);
static void IMU760_CheckSum(const uint8_t *pData, uint16_t Length, uint8_t *ck1, uint8_t *ck2);
static void IMU760_ParseMessage(const uint8_t *pMessage, uint8_t Length);
static HAL_StatusTypeDef IMU760_SendCommand(uint8_t class_id, uint8_t op, const uint8_t *pData, uint16_t Length);

/**
 * @brief  初始化IMU760 UART DMA接收
 * @note   使用UART空闲中断 + DMA方式接收连续输出数据
 * @param  无
 * @retval 无
 */
void IMU760_Init(void)
{
    memset(&imu760_data, 0, sizeof(imu760_data));
    memset(imu760_rx_raw_buffer, 0, sizeof(imu760_rx_raw_buffer));
    memset(imu760_frame_buffer, 0, sizeof(imu760_frame_buffer));
    imu760_frame_length = 0U;

    HAL_UARTEx_ReceiveToIdle_DMA(&IMU760_UART_HANDLE, imu760_rx_raw_buffer, sizeof(imu760_rx_raw_buffer));

    if (IMU760_UART_HANDLE.hdmarx != NULL)
    {
        __HAL_DMA_DISABLE_IT(IMU760_UART_HANDLE.hdmarx, DMA_IT_HT);
    }
}

/**
 * @brief  清除数据更新标志
 * @param  无
 * @retval 无
 */
void IMU760_ClearUpdateFlag(void)
{
    imu760_data.update_flag = 0U;
}

/**
 * @brief  处理IMU760接收到的数据
 * @note   该函数会自动拼帧，并解析输出协议：
 *         Header(0x59 0x53) + TID(2B) + LEN(1B) + MESSAGE + CK1 + CK2
 * @param  pData: 指向接收数据缓冲区的指针
 * @param  Size : 接收数据长度
 * @retval 无
 */
void IMU760_ProcessData(uint8_t *pData, uint16_t Size)
{
    uint16_t copy_size;
    uint16_t frame_total_length;
    uint8_t ck1;
    uint8_t ck2;
    uint8_t message_length;

    if ((pData == NULL) || (Size == 0U))
    {
        return;
    }

    /* 若缓存空间不足，直接清空，避免旧数据污染 */
    if ((uint32_t)imu760_frame_length + Size > IMU760_FRAME_BUFFER_SIZE)
    {
        imu760_frame_length = 0U;
        memset(imu760_frame_buffer, 0, sizeof(imu760_frame_buffer));
    }

    copy_size = IMU760_MinU16((uint16_t)(IMU760_FRAME_BUFFER_SIZE - imu760_frame_length), Size);
    memcpy(&imu760_frame_buffer[imu760_frame_length], pData, copy_size);
    imu760_frame_length = (uint16_t)(imu760_frame_length + copy_size);

    while (imu760_frame_length >= 7U)
    {
        /* 查找帧头 */
        if ((imu760_frame_buffer[0] != 0x59U) || (imu760_frame_buffer[1] != 0x53U))
        {
            memmove(&imu760_frame_buffer[0], &imu760_frame_buffer[1], (size_t)(imu760_frame_length - 1U));
            imu760_frame_length--;
            continue;
        }

        message_length = imu760_frame_buffer[4];
        frame_total_length = (uint16_t)(2U + 2U + 1U + message_length + 2U);

        if (imu760_frame_length < frame_total_length)
        {
            break;
        }

        IMU760_CheckSum(&imu760_frame_buffer[2], (uint16_t)(3U + message_length), &ck1, &ck2);

        if ((ck1 == imu760_frame_buffer[5U + message_length]) &&
            (ck2 == imu760_frame_buffer[6U + message_length]))
        {
            imu760_data.tid = IMU760_ReadU16LE(&imu760_frame_buffer[2]);
            IMU760_ParseMessage(&imu760_frame_buffer[5], message_length);
            imu760_data.update_tick = HAL_GetTick();

            memmove(&imu760_frame_buffer[0], &imu760_frame_buffer[frame_total_length], (size_t)(imu760_frame_length - frame_total_length));
            imu760_frame_length = (uint16_t)(imu760_frame_length - frame_total_length);
        }
        else
        {
            /* 校验失败，右移1字节重新找帧头 */
            memmove(&imu760_frame_buffer[0], &imu760_frame_buffer[1], (size_t)(imu760_frame_length - 1U));
            imu760_frame_length--;
        }
    }
    IMU_Angle.X_Vel = imu760_data.gyro[0];
    IMU_Angle.Y_Vel = imu760_data.gyro[1];
    IMU_Angle.Z_Vel = imu760_data.gyro[2];
    IMU_Angle.RoLL = imu760_data.euler[0];
    IMU_Angle.Pitch = imu760_data.euler[1];
    IMU_Angle.Yaw = imu760_data.euler[2];
}

/**
 * @brief  设置输出频率
 * @param  rate: 输出频率代码，使用IMU760_RATE_xxHZ宏
 * @param  save_mode: IMU760_OP_RAM 或 IMU760_OP_FLASH
 * @retval HAL状态
 */
HAL_StatusTypeDef IMU760_SetOutputRate(uint8_t rate, uint8_t save_mode)
{
    return IMU760_SendCommand(IMU760_CLASS_OUTPUT_RATE, save_mode, &rate, 1U);
}

/**
 * @brief  设置输出内容掩码
 * @param  mask: 输出内容位掩码，使用IMU760_OUTPUT_xxx宏组合
 * @param  save_mode: IMU760_OP_RAM 或 IMU760_OP_FLASH
 * @retval HAL状态
 */
HAL_StatusTypeDef IMU760_SetOutputMask(uint16_t mask, uint8_t save_mode)
{
    uint8_t payload[2];

    payload[0] = (uint8_t)(mask & 0xFFU);
    payload[1] = (uint8_t)((mask >> 8) & 0xFFU);

    return IMU760_SendCommand(IMU760_CLASS_OUTPUT_CONTENT, save_mode, payload, 2U);
}

/**
 * @brief  设置算法模式
 * @param  mode: IMU760_MODE_AHRS / IMU760_MODE_VRU / IMU760_MODE_IMU
 * @param  save_mode: IMU760_OP_RAM 或 IMU760_OP_FLASH
 * @retval HAL状态
 */
HAL_StatusTypeDef IMU760_SetAlgorithmMode(uint8_t mode, uint8_t save_mode)
{
    uint8_t payload[2];

    payload[0] = 0x02U; /* 子类型：算法模式切换 */
    payload[1] = mode;

    return IMU760_SendCommand(IMU760_CLASS_FUNCTION_MODE, save_mode, payload, 2U);
}

/**
 * @brief  解析一帧中的MESSAGE数据域
 * @param  pMessage: 数据域指针
 * @param  Length  : 数据域长度
 * @retval 无
 */
static void IMU760_ParseMessage(const uint8_t *pMessage, uint8_t Length)
{
    uint8_t offset = 0U;
    uint8_t data_id;
    uint8_t data_len;

    while ((uint16_t)offset + 2U <= Length)
    {
        data_id = pMessage[offset];
        data_len = pMessage[offset + 1U];

        if ((uint16_t)offset + 2U + data_len > Length)
        {
            break;
        }

        switch (data_id)
        {
        case IMU760_ID_TEMP:
            if (data_len == 2U)
            {
                imu760_data.temperature = (float)IMU760_ReadS16LE(&pMessage[offset + 2U]) * 0.01f;
                imu760_data.update_flag |= IMU760_UPDATE_TEMP;
            }
            break;

        case IMU760_ID_ACC:
            if (data_len == 12U)
            {
                imu760_data.acc[0] = (float)IMU760_ReadS32LE(&pMessage[offset + 2U]) * 0.000001f;
                imu760_data.acc[1] = (float)IMU760_ReadS32LE(&pMessage[offset + 6U]) * 0.000001f;
                imu760_data.acc[2] = (float)IMU760_ReadS32LE(&pMessage[offset + 10U]) * 0.000001f;
                imu760_data.update_flag |= IMU760_UPDATE_ACC;
            }
            break;

        case IMU760_ID_GYRO:
            if (data_len == 12U)
            {
                imu760_data.gyro[0] = (float)IMU760_ReadS32LE(&pMessage[offset + 2U]) * 0.000001f;
                imu760_data.gyro[1] = (float)IMU760_ReadS32LE(&pMessage[offset + 6U]) * 0.000001f;
                imu760_data.gyro[2] = (float)IMU760_ReadS32LE(&pMessage[offset + 10U]) * 0.000001f;
                imu760_data.update_flag |= IMU760_UPDATE_GYRO;
            }
            break;

        case IMU760_ID_MAG_NORM:
            if (data_len == 12U)
            {
                imu760_data.mag_norm[0] = (float)IMU760_ReadS32LE(&pMessage[offset + 2U]) * 0.000001f;
                imu760_data.mag_norm[1] = (float)IMU760_ReadS32LE(&pMessage[offset + 6U]) * 0.000001f;
                imu760_data.mag_norm[2] = (float)IMU760_ReadS32LE(&pMessage[offset + 10U]) * 0.000001f;
                imu760_data.update_flag |= IMU760_UPDATE_MAG_NORM;
            }
            break;

        case IMU760_ID_MAG_RAW:
            if (data_len == 12U)
            {
                imu760_data.mag_raw[0] = (float)IMU760_ReadS32LE(&pMessage[offset + 2U]) * 0.001f;
                imu760_data.mag_raw[1] = (float)IMU760_ReadS32LE(&pMessage[offset + 6U]) * 0.001f;
                imu760_data.mag_raw[2] = (float)IMU760_ReadS32LE(&pMessage[offset + 10U]) * 0.001f;
                imu760_data.update_flag |= IMU760_UPDATE_MAG_RAW;
            }
            break;

        case IMU760_ID_EULER:
            if (data_len == 12U)
            {
                imu760_data.euler[0] = (float)IMU760_ReadS32LE(&pMessage[offset + 2U]) * 0.000001f;  /* pitch */
                imu760_data.euler[1] = (float)IMU760_ReadS32LE(&pMessage[offset + 6U]) * 0.000001f;  /* roll  */
                imu760_data.euler[2] = (float)IMU760_ReadS32LE(&pMessage[offset + 10U]) * 0.000001f; /* yaw   */
                imu760_data.update_flag |= IMU760_UPDATE_EULER;
            }
            break;

        case IMU760_ID_QUAT:
            if (data_len == 16U)
            {
                imu760_data.quat[0] = (float)IMU760_ReadS32LE(&pMessage[offset + 2U]) * 0.000001f;
                imu760_data.quat[1] = (float)IMU760_ReadS32LE(&pMessage[offset + 6U]) * 0.000001f;
                imu760_data.quat[2] = (float)IMU760_ReadS32LE(&pMessage[offset + 10U]) * 0.000001f;
                imu760_data.quat[3] = (float)IMU760_ReadS32LE(&pMessage[offset + 14U]) * 0.000001f;
                imu760_data.update_flag |= IMU760_UPDATE_QUAT;
            }
            break;

        case IMU760_ID_SAMPLE_TIMESTAMP:
            if (data_len == 4U)
            {
                imu760_data.sample_timestamp_us = IMU760_ReadU32LE(&pMessage[offset + 2U]);
                imu760_data.update_flag |= IMU760_UPDATE_SAMPLE_TS;
            }
            break;

        case IMU760_ID_DATAREADY_TIMESTAMP:
            if (data_len == 4U)
            {
                imu760_data.dataready_timestamp_us = IMU760_ReadU32LE(&pMessage[offset + 2U]);
                imu760_data.update_flag |= IMU760_UPDATE_DATAREADY_TS;
            }
            break;

        default:
            /* 其他数据ID暂不处理，直接跳过 */
            break;
        }

        offset = (uint8_t)(offset + 2U + data_len);
    }
}

/**
 * @brief  发送交互协议命令
 * @note   交互协议格式：59 53 + 数据类(1B) + [长度13bit|操作符3bit](2B, 小端) + 数据域 + CK1 + CK2
 * @param  class_id: 数据类
 * @param  op      : 操作符，IMU760_OP_QUERY / IMU760_OP_RAM / IMU760_OP_FLASH
 * @param  pData   : 数据域指针
 * @param  Length  : 数据域长度
 * @retval HAL状态
 */
static HAL_StatusTypeDef IMU760_SendCommand(uint8_t class_id, uint8_t op, const uint8_t *pData, uint16_t Length)
{
    uint8_t frame[32];
    uint16_t field;
    uint8_t ck1;
    uint8_t ck2;

    if (Length > 24U)
    {
        return HAL_ERROR;
    }

    frame[0] = 0x59U;
    frame[1] = 0x53U;
    frame[2] = class_id;

    field = (uint16_t)(((Length & 0x1FFFU) << 3) | (op & 0x07U));
    frame[3] = (uint8_t)(field & 0xFFU);
    frame[4] = (uint8_t)((field >> 8) & 0xFFU);

    if ((pData != NULL) && (Length > 0U))
    {
        memcpy(&frame[5], pData, Length);
    }

    IMU760_CheckSum(&frame[2], (uint16_t)(3U + Length), &ck1, &ck2);
    frame[5U + Length] = ck1;
    frame[6U + Length] = ck2;

    return HAL_UART_Transmit(&IMU760_UART_HANDLE, frame, (uint16_t)(7U + Length), 100U);
}

/**
 * @brief  计算CK1/CK2校验和
 * @param  pData : 校验起始地址
 * @param  Length: 校验长度
 * @param  ck1   : CK1输出地址
 * @param  ck2   : CK2输出地址
 * @retval 无
 */
static void IMU760_CheckSum(const uint8_t *pData, uint16_t Length, uint8_t *ck1, uint8_t *ck2)
{
    uint16_t i;
    uint8_t sum1 = 0U;
    uint8_t sum2 = 0U;

    for (i = 0U; i < Length; i++)
    {
        sum1 = (uint8_t)(sum1 + pData[i]);
        sum2 = (uint8_t)(sum2 + sum1);
    }

    *ck1 = sum1;
    *ck2 = sum2;
}

/**
 * @brief  取两个uint16_t中的较小值
 * @param  a: 参数a
 * @param  b: 参数b
 * @retval 较小值
 */
static uint16_t IMU760_MinU16(uint16_t a, uint16_t b)
{
    return (a < b) ? a : b;
}

/**
 * @brief  读取小端uint16_t数据
 * @param  pData: 数据地址
 * @retval 转换结果
 */
static uint16_t IMU760_ReadU16LE(const uint8_t *pData)
{
    return (uint16_t)((uint16_t)pData[0] | ((uint16_t)pData[1] << 8));
}

/**
 * @brief  读取小端int16_t数据
 * @param  pData: 数据地址
 * @retval 转换结果
 */
static int16_t IMU760_ReadS16LE(const uint8_t *pData)
{
    return (int16_t)((uint16_t)pData[0] | ((uint16_t)pData[1] << 8));
}

/**
 * @brief  读取小端uint32_t数据
 * @param  pData: 数据地址
 * @retval 转换结果
 */
static uint32_t IMU760_ReadU32LE(const uint8_t *pData)
{
    return ((uint32_t)pData[0]) | ((uint32_t)pData[1] << 8) | ((uint32_t)pData[2] << 16) | ((uint32_t)pData[3] << 24);
}

/**
 * @brief  读取小端int32_t数据
 * @param  pData: 数据地址
 * @retval 转换结果
 */
static int32_t IMU760_ReadS32LE(const uint8_t *pData)
{
    return (int32_t)IMU760_ReadU32LE(pData);
}
