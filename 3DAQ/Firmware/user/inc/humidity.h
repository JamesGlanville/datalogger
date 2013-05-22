#ifndef humidity_h
#define humidity_h
#include "stm32f10x_gpio.h"

void GPIO_Init_Mode(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIOMode_TypeDef mode);
int readcapacitance(void);
void humidity_init(void);

#endif
