#include "humidity.h"
#include "LCD.h"

void GPIO_Init_Mode(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIOMode_TypeDef mode) {
      GPIO_InitTypeDef GPIO_InitStructure;
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
      GPIO_InitStructure.GPIO_Mode = mode;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(GPIOx, &GPIO_InitStructure);
}

void humidity_init(void)
{
	GPIO_Init_Mode(GPIOC,GPIO_Pin_10,GPIO_Mode_IN_FLOATING);
}

int readcapacitance(void)
{
	int i;
	int j;

	int lastmeasurement;
	j=0;
	for (i=0;i<10000;i++)
{	
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_10)!=lastmeasurement)
	{
		lastmeasurement=!lastmeasurement;
		j++;
	}
}
	return j;
}
