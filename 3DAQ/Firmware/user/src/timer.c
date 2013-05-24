#include "timer.h"
#include "webi2c.h"

extern volatile uint8_t Config[CONFIGLENGTH];

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

void logging_timer_init( void ) 
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
  TIM_TimeBaseStructure.TIM_Prescaler = (SYS_CLK/(DELAY_TIM_FREQUENCY)) - 1;
	if ((1000*Config[2])>UINT16_MAX)
	{
		TIM_TimeBaseStructure.TIM_Period = UINT16_MAX; 
	}
	else
	{
		TIM_TimeBaseStructure.TIM_Period = 1000*Config[2]; //UINT16_MAX; 		
	}
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//TIM_PrescalerConfig(TIM3, 0, TIM_PSCReloadMode_Immediate);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  TIM_Cmd(TIM3, ENABLE);
	TIM3->CNT=0;
}

void delay_ms( uint16_t mSecs ) 
{
	TIM4->CNT=0;
	while(TIM4->CNT < mSecs);
}
