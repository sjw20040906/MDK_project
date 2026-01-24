#ifndef _SBUS_H_
#define _SBUS_H_

#include "main.h"
#include "usart.h"
#include "string.h"
#include <stdint.h>
#include "BSP_BoardCommunication.h"

#define SBUS_RX_LEN 25 // 25

#define StartByte 0x0f
#define EndByte 0x00
// SBUS原始数据范围
#define SBUS_MIN 353
#define SBUS_MAX 1695
// 中间阈值
#define MID_VALUE 1024
// 档位判断容差
#define TOLERANCE 50

typedef struct
{
	uint8_t Start;
	uint16_t Ch1;
	uint16_t Ch2;
	uint16_t Ch3;
	uint16_t Ch4;
	uint16_t Ch5;
	uint16_t Ch6;
	uint16_t Ch7;
	uint16_t Ch8;
	uint16_t Ch9;
	uint16_t Ch10;
	uint16_t Ch11;
	uint16_t Ch12;
	uint16_t Ch13;
	uint16_t Ch14;
	uint16_t Ch15;
	uint16_t Ch16;
	uint8_t Flag;
	uint8_t End;
} SBUS_Buffer;

// 定义转换后的数据结构体
typedef struct
{
	int16_t Ch1;
	int16_t Ch2;
	int16_t Ch3;
	int16_t Ch4;
	int16_t Ch5;
	int16_t Ch6;
	int16_t Ch7;
	int16_t Ch8;
	int16_t Ch9;
	int16_t Ch10;
} MappedData;

extern uint8_t SBUS_RX_Finish;
extern uint8_t SBUS_RXBuffer[SBUS_RX_LEN];
extern uint8_t SBUS_Rx_Data[25];
extern MappedData mappedData;

void SBUS_Init(void);
void SBUS_Handle(void);

#endif
