#include "BSP_Usart.h"

/***************用户数据声明****************/
/******************接口声明*****************/
Usart_Data_t Usart_Data = Usart_DataGroundInit;
#undef Usart_DataGroundInit

/**
 * @brief  接收空闲回调
 * @param  void
 * @retval void
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART3)
    {
        {
            memcpy(SBUS_RXBuffer, SBUS_Rx_Data, sizeof(SBUS_Rx_Data));
            SBUS_RX_Finish = 1;
        }
    }
    if (huart->Instance == USART1)
    {
        IMU760_ProcessData(imu760_rx_raw_buffer, Size);

        HAL_UARTEx_ReceiveToIdle_DMA(&IMU760_UART_HANDLE, imu760_rx_raw_buffer, sizeof(imu760_rx_raw_buffer));

        if (IMU760_UART_HANDLE.hdmarx != NULL)
        {
            __HAL_DMA_DISABLE_IT(IMU760_UART_HANDLE.hdmarx, DMA_IT_HT);
        }
    }
    if(huart->Instance == USART6)
    {
        JudgeSystem_Handler(&huart6);
    }
}
