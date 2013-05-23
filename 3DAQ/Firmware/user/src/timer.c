#include "timer.h"

void delay_init( void ) 
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
  TIM_TimeBaseStructure.TIM_Prescaler = (SYS_CLK / DELAY_TIM_FREQUENCY) - 1;
  TIM_TimeBaseStructure.TIM_Period = UINT16_MAX; 
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	TIM_PrescalerConfig(TIM4, 0, TIM_PSCReloadMode_Immediate);
	
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
  TIM_Cmd(TIM4, ENABLE);
}

void delay_ms( uint16_t mSecs ) 
{
	TIM4->CNT=0;
	while(TIM4->CNT < mSecs);
}
