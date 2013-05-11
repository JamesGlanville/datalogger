
#include "stm32F10x.h"
#include "UART.h"

#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"

#include "LEDs.h"

extern volatile int command_flag;
extern volatile int value;
extern volatile int value_received;

//Prvate functions
void GPIO_Config(void);
void NVIC_Config(void);

//Function to intialise the UART to allow transmission to PC and enable interrupts to detect messages receievd from PC
void UART_init(void)
{
	USART_InitTypeDef USART_InitStructure;
	
	//Enable periph clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 
	
	//Configure required pins as UART TX/RX
	GPIO_Config();
	
	//NVIC_Config();
	
	//USART configuration values
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//8 bit bytes
  USART_InitStructure.USART_StopBits = USART_StopBits_1;	//one stop bit
  USART_InitStructure.USART_Parity = USART_Parity_No;	//no parity bit
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//No hardware flow control
  //USART_InitStructure.USART_Mode = USART_Mode_Tx;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//Enable USART for transmit and receive
	
	USART_Init(USART1, &USART_InitStructure);
	
	//Enable interrupts for RX
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  //USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	
	//Conigure Nested Vectored INterrupt Controller to accept USART inerrupts
	NVIC_Config();
	
	//Clear TXE flag (which indicates when transmit buffer is empty)
	USART_ClearFlag(USART1, USART_FLAG_TXE);
	
	//Enable UART
	USART_Cmd(USART1, ENABLE);
}

//Function to send a single byte to the PC over UART
void UART_send_byte(uint8_t byte)
{
	int i;

	//Wait for previous transmission to finish
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE != SET));
	
	//Send byte
	USART_SendData(USART1, (uint16_t)byte);
	
	//Wait 2.5us
	for (i=0; i<6000; i++);
}

//Function to wait for next byte from PC and read it in
uint8_t UART_receive_next_byte(void)
{
	uint8_t byte;
	
	//clear rx flag
	USART_ClearFlag(USART1, USART_FLAG_RXNE);
	
	//wait for new rx byte
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == 0);
	
	//get rx byte
	byte = USART_ReceiveData(USART1);
	
	return byte;
}

void GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	//Enable GPIO periph clock and alternate function periph clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	
	//Configure PA9 as TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	//Configure PA10 as RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  // Configure the NVIC Preemption Priority Bits
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
  // Enable the USART1 Interrupt
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	//Select the interrupt we're configuring (USART1 Interrupt ReQuest)
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	//Choose priority (lower number is higher priority)
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//Interrupt is to be enabled
  NVIC_Init(&NVIC_InitStructure);	//COnfigure NVIC with above parameters
}

//-----------------------------------------------------------------------------------------------

//Interrupt handler for USART1 interrupt
void USART1_IRQHandler(void)
{
	//Test source of interrupt
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == 1)	//RNXE - Receive buffer not empty (ie we have an RX byte)
	{
		//Clear interrupt flag
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		
		//Check that this is a new command and not a component of a pervious command
		if (command_flag == 0)
		{
			//This is a new command
			
			//Get command
			command_flag = USART_ReceiveData(USART1);
			//Get value
			value = USART_ReceiveData(USART1);

			value_received = 0;
		}
		else
		{
			//This is a continuation of a previous command
			
			//Get value
			value = USART_ReceiveData(USART1);
			value_received = 1;
		}

	}
	
}

