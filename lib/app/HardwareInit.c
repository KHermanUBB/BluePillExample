#include "HardwareInit.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

TIM_HandleTypeDef htmr;

void HardwareInit(){

    HAL_Init();

/*-------------------- PC13 Configuration begins -----------------------------*/

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); 

    GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); 


    Init_TMR4();
}

void Init_TMR4(void){

  __HAL_RCC_TIM4_CLK_ENABLE();

htmr.Instance = TIM4;
htmr.Init.Prescaler = 360;
htmr.Init.CounterMode = TIM_COUNTERMODE_UP;
htmr.Init.Period = 50000; // auto reload register
htmr.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
htmr.Init.RepetitionCounter = 0;
htmr.Instance->CCR1 = 1000;
HAL_TIM_Base_Init(&htmr);

HAL_NVIC_SetPriority(TIM4_IRQn, 0, 0);
HAL_NVIC_EnableIRQ(TIM4_IRQn);
__HAL_TIM_ENABLE_IT(&htmr, TIM_IT_UPDATE);
 
HAL_TIM_Base_Start_IT(&htmr);

}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);  
}


void myprintf(const char *fmt, ...) {
  static char buffer[256];
  __va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  int len = strlen(buffer);
 // HAL_UART_Transmit(&huart1, (uint8_t*)buffer, len, -1);
}



