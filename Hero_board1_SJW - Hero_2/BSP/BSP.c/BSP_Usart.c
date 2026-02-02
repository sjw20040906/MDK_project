#include "BSP_Usart.h"

/***************用户数据声明****************/
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

#if (RemoteControlMethod == DT7)
        // DT7遥控器
        DT7_RX_Finish = 1; // 已接受完一包数据
#endif
    }
    if (huart->Instance == USART6)
    {
#if (RemoteControlMethod == DJI_VT13)
        // DJI_VT13遥控器
        DJI_VT13_RX_Finish = 1; // 已接受完一包数据
#endif
		}
}

