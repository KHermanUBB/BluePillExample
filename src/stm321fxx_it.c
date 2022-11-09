
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "HardwareInit.h"

extern TIM_HandleTypeDef htmr;

void TIM4_IRQHandler(void){

  if (__HAL_TIM_GET_FLAG(&htmr, TIM_IT_UPDATE) != 0x00u)
  {
    
      HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9);

    __HAL_TIM_CLEAR_IT(&htmr, TIM_IT_UPDATE); // Clears The Interrupt Flag
  }

}


void SysTick_Handler(void)
{
  HAL_IncTick();
}

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  while (1) {}
}


void MemManage_Handler(void)
{
  while (1) {}
}

void BusFault_Handler(void)
{
  while (1) {}
}

void UsageFault_Handler(void)
{
  while (1) {}
}

void SVC_Handler(void)
{
}


void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

