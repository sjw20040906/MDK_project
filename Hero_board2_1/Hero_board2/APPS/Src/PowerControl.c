#include "PowerControl.h"
void PowerControl_MsgSend(void);
void PowerControl_MsgRec(Can_Export_Data_t RxMessage);

struct TxData PowerTxData;
RxData PowerRxData;

 
PowerControl_Fun_t PowerControl_Fun = PowerControl_FunGroundInit;
#undef PowerControl_FunGroundInit

void PowerControl_MsgSend(void)
{
    PowerTxData.enableDCDC = 1;
    PowerTxData.systemRestart = 0;
    PowerTxData.feedbackRefereePowerLimit = g_referee.robot_status_.chassis_power_limit;
    PowerTxData.feedbackRefereeEnergyBuffer = g_referee.power_heat_.buffer_energy;
    CAN_SendData(CAN_SendHandle, &hcan1, CAN_ID_STD, 0x61, (uint8_t*)&PowerTxData);
}

void PowerControl_MsgRec(Can_Export_Data_t RxMessage)
{
    PowerRxData.errorCode = RxMessage.CANx_Export_RxMessage[0];
    uint32_t temp = (RxMessage.CANx_Export_RxMessage[4] << 24) | (RxMessage.CANx_Export_RxMessage[3] << 16) | (RxMessage.CANx_Export_RxMessage[2] << 8) | (RxMessage.CANx_Export_RxMessage[1]);
    PowerRxData.chassisPower = *((float *)&temp);
    PowerRxData.chassisPowerLimit = (uint16_t) (RxMessage.CANx_Export_RxMessage[5]) | (RxMessage.CANx_Export_RxMessage[6]);
    PowerRxData.capEnergy = RxMessage.CANx_Export_RxMessage[7];
}
