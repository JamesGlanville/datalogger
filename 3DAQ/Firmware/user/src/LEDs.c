#include "stm32F10x.h"
#include "LEDs.h"
#include "stm32f10x_gpio.h"
#include "LEDs.h"

int LED;
extern volatile unsigned int LEDbyte;


void setLEDS(void)
{
	//LSB = green furthest right. LSB->MSB:
	if (LEDbyte&0x001){GPIO_SetBits(GPIOC,GPIO_Pin_13);}else{GPIO_ResetBits(GPIOC,GPIO_Pin_13);}
	if (LEDbyte&0x002){GPIO_SetBits(GPIOC,GPIO_Pin_1);}else{GPIO_ResetBits(GPIOC,GPIO_Pin_1);}
	if (LEDbyte&0x004){GPIO_SetBits(GPIOA,GPIO_Pin_4);}else{GPIO_ResetBits(GPIOA,GPIO_Pin_4);} //DONE
	if (LEDbyte&0x008){GPIO_SetBits(GPIOA,GPIO_Pin_2);}else{GPIO_ResetBits(GPIOA,GPIO_Pin_2);} //DONE
	if (LEDbyte&0x010){GPIO_SetBits(GPIOC,GPIO_Pin_3);}else{GPIO_ResetBits(GPIOC,GPIO_Pin_3);} //DONE
	if (LEDbyte&0x020){GPIO_SetBits(GPIOB,GPIO_Pin_1);}else{GPIO_ResetBits(GPIOB,GPIO_Pin_1);} 
	if (LEDbyte&0x040){GPIO_SetBits(GPIOC,GPIO_Pin_2);}else{GPIO_ResetBits(GPIOC,GPIO_Pin_2);} //DONE
	if (LEDbyte&0x080){GPIO_SetBits(GPIOA,GPIO_Pin_3);}else{GPIO_ResetBits(GPIOA,GPIO_Pin_3);} //DONE
	if (LEDbyte&0x100){GPIO_SetBits(GPIOA,GPIO_Pin_6);}else{GPIO_ResetBits(GPIOA,GPIO_Pin_6);}//DONE
	if (LEDbyte&0x200){GPIO_SetBits(GPIOA,GPIO_Pin_5);}else{GPIO_ResetBits(GPIOA,GPIO_Pin_5);} //DONE
}

void init_GPIO_pins()
{
	GPIO_InitTypeDef  GPIO_InitStructureA;	//Structure that holds configuration values for GPIO (General Purpose In/Out) pins
	GPIO_InitTypeDef  GPIO_InitStructureB;	//Structure that holds configuration values for GPIO (General Purpose In/Out) pins
	GPIO_InitTypeDef  GPIO_InitStructureC;	//Structure that holds configuration values for GPIO (General Purpose In/Out) pins
	
	//start GPIO clocks
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructureA.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_6|GPIO_Pin_5; //DB6|ENABLE|RS
	GPIO_InitStructureA.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructureA.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructureA);

	GPIO_InitStructureB.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1; //DB5
	GPIO_InitStructureB.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructureB.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructureB);

	GPIO_InitStructureC.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_2|GPIO_Pin_9|GPIO_Pin_3|GPIO_Pin_13|GPIO_Pin_1; //DB4|DB7|LEDx?|BOARDLED
	GPIO_InitStructureC.GPIO_Mode = GPIO_Mode_Out_PP;	//Choose mode (output, push-pull)
	GPIO_InitStructureC.GPIO_Speed = GPIO_Speed_10MHz;	//Choose max speed (slow is fine here)
	GPIO_Init(GPIOC, &GPIO_InitStructureC);	//Call the GPIO init function from the std periph lib to initialise GPIOC with these parameters
}

//Function to turn LED off
void LED_off()
{
	//Send PC9 low (using std periph lib function)
	GPIO_ResetBits(GPIOC, GPIO_Pin_9);
	//Set flag to remember current state of LED
	LED = 0;
}

void LED_on()
{
	//Send PC9 high (using std periph lib function)
	GPIO_SetBits(GPIOC, GPIO_Pin_9);
	//Set flag to remember current state of LED
	LED = 1;
}


