/**
 ******************************************************************************
 * @file    wit_imu.c
 * @author  SJW
 * @version V1.0.0
 * @date    2026-01-15
 * @brief   IMU数据处理模块
 *          该文件提供IMU数据处理和初始化函数的实现
 ******************************************************************************
 */

#include "wit_imu.h"

/* 全局变量 */
uint8_t IMU_RxRawBuffer[64];
IMU_Data_t imu_data;
IMU_Angle_t IMU_Angle;

/**
 * @brief  初始化IMU UART DMA接收
 * @note   该函数配置UART1使用DMA空闲模式接收数据
 *         接收到的数据将存储在IMU_RxRawBuffer中
 * @param  无
 * @retval 无
 */
void IMU_Init(void)
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, IMU_RxRawBuffer, sizeof(IMU_RxRawBuffer));
}

/**
 * @brief  处理IMU接收到的数据
 * @note   该函数解析原始IMU数据包并更新imu_data结构体
 *         数据包格式为：0x55 + 数据类型 + 数据(6字节) + 校验和
 * @param  pData: 指向接收数据缓冲区的指针
 * @param  Size: 接收数据的大小
 * @retval 无
 */
void IMU_ProcessData(uint8_t *pData, uint16_t Size)
{
    uint16_t i = 0;

    while (Size - i >= 11)
    {
        if (pData[i] == 0x55)
        {
            /* 计算校验和 */
            uint8_t sum = 0;
            for (uint8_t j = 0; j < 10; j++)
            {
                sum += pData[i + j];
            }

            if (sum == pData[i + 10])
            {
                int16_t raw[3];
                /* 将原始数据从大端转换为小端 */
                raw[0] = (int16_t)((pData[i + 3] << 8) | pData[i + 2]);
                raw[1] = (int16_t)((pData[i + 5] << 8) | pData[i + 4]);
                raw[2] = (int16_t)((pData[i + 7] << 8) | pData[i + 6]);

                /* 根据数据类型解析数据 */
                switch (pData[i + 1])
                {
                case WIT_ACC:
                    for (int j = 0; j < 3; j++)
                    {
                        imu_data.acc[j] = raw[j] * (16.0f / 32768.0f);
                    }
                    break;
                case WIT_GYRO:
                    for (int j = 0; j < 3; j++)
                    {
                        imu_data.gyro[j] = raw[j] * (2000.0f / 32768.0f);
                    }
                    IMU_Angle.X_Vel = imu_data.gyro[0];
                    IMU_Angle.Y_Vel = imu_data.gyro[1];
                    IMU_Angle.Z_Vel = -0.0f * imu_data.gyro[2];
                    break;
                case WIT_ANGLE:
                    /* 将角度数据转换为deg单位 (范围: ±180deg) */
                    for (int j = 0; j < 3; j++)
                    {
                        imu_data.angle[j] = raw[j] * (180.0f / 32768.0f);
                    }
                    IMU_Angle.RoLL = imu_data.angle[0];
                    IMU_Angle.Pitch = imu_data.angle[1];
                    IMU_Angle.Yaw = -(360 - (imu_data.angle[2] + 180.0f)); // 将yaw调整到0-360度范围
                    break;
                }
                i += 11; /* 移动到下一个数据包 */
            }
            else
            {
                i++; /* 校验和无效，移动到下一个字节 */
            }
        }
        else
        {
            i++; /* 不是有效的数据包头部，移动到下一个字节 */
        }
    }
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, IMU_RxRawBuffer, sizeof(IMU_RxRawBuffer));
}
