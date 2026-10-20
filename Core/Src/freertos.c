/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
#include "usart.h"
#include "pca9538.h"
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define KBRD_ADDR 0xE2
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osMessageQId xInputQueueHandle;
/* USER CODE END Variables */
osThreadId Task_LED1Handle;
osThreadId Task_UARTHandle;
osThreadId Task_LED2Handle;
osMessageQId myQueue01Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void StartTaskInput(void const * argument);
void StartTaskGame(void const * argument);
/* USER CODE END FunctionPrototypes */


void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{

}

__weak unsigned long getRunTimeCounterValue(void)
{
return 0;
}
/* USER CODE END 1 */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
  
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
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
void MX_FREERTOS_Init(void) {
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

  /* Create the queue(s) */
  /* definition and creation of myQueue01 */
  osMessageQDef(myQueue01, 16, uint16_t);
  myQueue01Handle = osMessageCreate(osMessageQ(myQueue01), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  osMessageQDef(InputQueue, 10, uint8_t);
  xInputQueueHandle = osMessageCreate(osMessageQ(InputQueue), NULL);
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of Task_LED1 */

  /* USER CODE BEGIN RTOS_THREADS */
  vQueueAddToRegistry(myQueue01Handle, "queue1");
  osThreadDef(InputTask, StartTaskInput, osPriorityNormal, 0, 256); // 256 words стека
  osThreadCreate(osThread(InputTask), NULL);

  osThreadDef(GameTask, StartTaskGame, osPriorityAboveNormal, 0, 512);
  osThreadCreate(osThread(GameTask), NULL);
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartTaskLED1 */
/**
  * @brief  Function implementing the Task_LED1 thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartTaskLED1 */
void StartTaskLED1(void const * argument)
{
  /* USER CODE BEGIN StartTaskLED1 */
  /* Infinite loop */
  uint32_t state;
  for(;;)
  {
	  HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
	  if (HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_15) == GPIO_PIN_SET) {
		  state = 0x01;
		  osMessagePut(myQueue01Handle, state, 100);
	  } else {
		  state = 0x00;
		  osMessagePut(myQueue01Handle, state, 100);
	  }
	  osDelay(500);
  }
  /* USER CODE END StartTaskLED1 */
}

/* USER CODE BEGIN Header_StartTaskUART */
/**
* @brief Function implementing the Task_UART thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskUART */
void StartTaskUART(void const * argument)
{
  /* USER CODE BEGIN StartTaskUART */
  /* Infinite loop */
	osEvent event;
	char *state;
	for(;;)
	  {
		event = osMessageGet(myQueue01Handle, 100);
		if (event.status == osEventMessage) {
			state = event.value.v == 0x00 ? "reset\n\r" : "set\n\r";
			HAL_UART_Transmit(&huart1, (uint8_t *)state, strlen(state), 100);
		}
		osDelay(1);
	  }
  /* USER CODE END StartTaskUART */
}

/* USER CODE BEGIN Header_StartTasLED2 */
/**
* @brief Function implementing the Task_LED2 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTasLED2 */
void StartTasLED2(void const * argument)
{
  /* USER CODE BEGIN StartTasLED2 */
  /* Infinite loop */
  for(;;)
  {
	  HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
	  osDelay(400);
  }
  /* USER CODE END StartTasLED2 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
#include "pca9538.h"
#include "tetris.h"

void StartTaskInput(void const * arg) {
    uint8_t cfg, in, prev_key = 0;
    uint8_t buf = 0;

    PCA9538_Write_Register(KBRD_ADDR, POLARITY_INVERSION, &buf);
    PCA9538_Write_Register(KBRD_ADDR, OUTPUT_PORT, &buf);

    for(;;) {
        uint8_t key = 0;
        for (int r = 0; r < 4; r++) {
            cfg = ~(1 << r);
            PCA9538_Write_Register(KBRD_ADDR, CONFIG, &cfg);
            PCA9538_Read_Inputs(KBRD_ADDR, &in);
            in = (~in) & 0x70;
            if (in & 0x10) { key = (r << 4) | 0x01; break; }
            if (in & 0x20) { key = (r << 4) | 0x02; break; }
            if (in & 0x40) { key = (r << 4) | 0x04; break; }
        }
        if (key != 0 && (key != prev_key || key == 0x11)) {
            osMessagePut(xInputQueueHandle, key, 10);
        }
        prev_key = key;
        osDelay(30);
    }
}


void StartTaskGame(void const * arg) {
    TetrisState state;
    Tetris_Init(&state);

    for(;;) {
        osEvent ev = osMessageGet(xInputQueueHandle, state.speed);

        if (ev.status == osEventMessage) {
            switch(ev.value.v) {
                case 0x01: Tetris_Move(&state, -1, 0); break; // Row 0, Col 1 (Left)
                case 0x02: Tetris_Rotate(&state); break;      // Row 0, Col 2 (Rotate)
                case 0x04: Tetris_Move(&state, 1, 0); break;  // Row 0, Col 3 (Right)
                case 0x11: Tetris_Move(&state, 0, 1); break;  // Row 1, Col 1 (Down)
                case 0x12: Tetris_Drop(&state); Tetris_Step(&state); break;
            }
        } else {
            if (Tetris_Step(&state)) {
                Tetris_Init(&state);
            }
        }
        Tetris_Render(&state);
    }
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
