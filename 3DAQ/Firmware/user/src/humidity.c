#include "humidity.h"
#include "LCD.h"

//static __INLINE void __enable_irq()               { __ASM volatile ("cpsie i"); };
//static __INLINE void __disable_irq()              { __ASM volatile ("cpsid i"); };

int readcapacitance(void)
{
	int i;
	i=0;
	
	GPIO_ResetBits(GPIOC, GPIO_Pin_9);
	delayMicroseconds(100);
//	__disable_irq();

	GPIO_SetBits(GPIOC, GPIO_Pin_9);

//	while (!DIGITALREADWHATEVER())
//	{
//		i++;
//	}
	
//	__enable_irq();
	
	GPIO_ResetBits(GPIOC,GPIO_Pin_9);
	
	return i;
}
