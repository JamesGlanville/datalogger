#include "humidity.h"
#include "LCD.h"

//static __INLINE void __enable_irq()               { __ASM volatile ("cpsie i"); };
//static __INLINE void __disable_irq()              { __ASM volatile ("cpsid i"); };

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
//	GPIO_Init_Mode(GPIOC,GPIO_Pin_10,GPIO_Mode_Out_PP);
//	GPIO_Init_Mode(GPIOC,GPIO_Pin_11,GPIO_Mode_IPD);
}

int readcapacitance(void)
{
	int i;
	int j;
	int k;

	int lastmeasurement;
		i=0;
	j=0;
	for (i=0;i<100;i++)
{	
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_10)!=lastmeasurement)
	{
		lastmeasurement=!lastmeasurement;
		j++;
	}

	
	
}

return j;
//		__disable_irq();

//for (k=0;k<100;k++){
	//GPIO_ResetBits(GPIOC, GPIO_Pin_10);
//	delayMicroseconds(100);

	//GPIO_SetBits(GPIOC, GPIO_Pin_10);
/*	j=GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11);
	j=GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11);
	j=GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11);
	j=GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11);
	j=GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11);
	j=GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11);
	j=GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11);
	j=GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11);
	j=GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11);
	j=GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11);
	j=GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11);
	j=GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11);
	j=GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11);
	j=GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11);
	j=GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11);
	j=GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11);*/
	/*while (!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11))
	{
		i++;
	}
		GPIO_ResetBits(GPIOC,GPIO_Pin_10);
	j+=i;
}*/
//j=j/100;
//	__enable_irq();
	
	
	
	
	return j;
}
