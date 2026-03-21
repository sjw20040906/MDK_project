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
        IMU_ProcessData(IMU_RxRawBuffer, Size);
        /* 重新启动UART DMA接收 */
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, IMU_RxRawBuffer, sizeof(IMU_RxRawBuffer));
    }
}
