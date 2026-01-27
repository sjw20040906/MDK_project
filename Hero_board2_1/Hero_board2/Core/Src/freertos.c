/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "typedef.h"
#include "Task_DM_onlineCheck.h"
#include "mecanum_wheel.h"
#include "Task_RemoteControl.h"
#include "Chassis.h"
#include "Gimbal.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/***********Queues************/
QueueHandle_t CAN1_ReceiveHandle; // can1接收队列
QueueHandle_t CAN2_ReceiveHandle; // can2接收队列
QueueHandle_t CAN_SendHandle;     // can发送队列

/***********Tasks************/
osThreadId Task_Can1MsgRecHandle;    // can1消息接收任务句柄
osThreadId Task_Can2MsgRecHandle;    // can2消息接收任务句柄
osThreadId Task_CanSendHandle;       // can发送任务句柄
osThreadId Task_DMOnlineCheckHandle; // DM电机掉线检测任务句柄
osThreadId Robot_Control_Handle;     // 机器人控制任务句柄
osThreadId RemoteControl_Handle;     // 遥控器处理任务句柄
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId StartTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
extern void Can1Receives(void const *argument);
extern void Can2Receives(void const *argument);
extern void AllCanSend(void const *argument);
extern void DM_onlineCheck(void const *argument);
extern void Robot_Control(void const *argument);
extern void RemoteControl_Processing(void const *argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const *argument);
void ALL_Init(void const *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize);

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* definition and creation of CAN1_Receive */
  CAN1_ReceiveHandle = xQueueCreate(16, sizeof(Can_Export_Data_t));

  /* definition and creation of CAN2_Receive */
  CAN2_ReceiveHandle = xQueueCreate(16, sizeof(Can_Export_Data_t));

  /* definition and creation of CAN_Send */
  CAN_SendHandle = xQueueCreate(32, sizeof(Can_Send_Data_t));
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityIdle, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of StartTask */
  osThreadDef(StartTask, ALL_Init, osPriorityRealtime, 0, 128);
  StartTaskHandle = osThreadCreate(osThread(StartTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* definition and creation of Can1ReceiveTask */
  osThreadDef(Can1_ReceiveTask, Can1Receives, osPriorityAboveNormal, 0, 128);
  Task_Can1MsgRecHandle = osThreadCreate(osThread(Can1_ReceiveTask), NULL);

  /* definition and creation of Can1ReceiveTask */
  osThreadDef(Can2_ReceiveTask, Can2Receives, osPriorityAboveNormal, 0, 128);
  Task_Can2MsgRecHandle = osThreadCreate(osThread(Can2_ReceiveTask), NULL);

  /* definition and creation of CanSendTask */
  osThreadDef(Can_SendTask, AllCanSend, osPriorityHigh, 0, 256);
  Task_CanSendHandle = osThreadCreate(osThread(Can_SendTask), NULL);

  /* definition and creation of DMOnlineCheckTask */
  osThreadDef(DM_OnlineCheckTask, DM_onlineCheck, osPriorityRealtime, 0, 128);
  Task_DMOnlineCheckHandle = osThreadCreate(osThread(DM_OnlineCheckTask), NULL);

  /* definition and creation of Robot_ControlTask */
  osThreadDef(Robot_ControlTask, Robot_Control, osPriorityHigh, 0, 512);
  Robot_Control_Handle = osThreadCreate(osThread(Robot_ControlTask), NULL);

  /* definition and creation of RemoteControlTask */
  osThreadDef(RemoteControlTask, RemoteControl_Processing, osPriorityRealtime, 0, 128);
  RemoteControl_Handle = osThreadCreate(osThread(RemoteControlTask), NULL);
  /* USER CODE END RTOS_THREADS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_ALL_Init */
/**
 * @brief Function implementing the StartTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_ALL_Init */
void ALL_Init(void const *argument)
{
  /* USER CODE BEGIN ALL_Init */
  /* Infinite loop */
  for (;;)
  {
    taskENTER_CRITICAL();
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_10, GPIO_PIN_SET);
    /*********初始化两个CAN控制协议，使用中断模式*********/
    CAN_IT_Init(&hcan1, Can1_Type);
    CAN_IT_Init(&hcan2, Can2_Type);
    /*********初始化IMU*********/
    IMU_Init();
    /**********遥控器初始化*********/
    SBUS_Init();
    /**********云台初始化*********/
    Gimbal_Init();
    /**********底盘初始化*********/
    Chassis_Init();
    vTaskDelete(StartTaskHandle);
    taskEXIT_CRITICAL();
    osDelay(1);
  }
  /* USER CODE END ALL_Init */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
