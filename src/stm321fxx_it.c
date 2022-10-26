
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "HardwareInit.h"



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


HAL_DMA1_IRQHandler(DMA_HandleTypeDef *hdma){

 
  if(__HAL_DMA_GET_TC_FLAG_INDEX(hdma) == DMA_FLAG_TC6){

        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
      __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_TC6);
  }


} 