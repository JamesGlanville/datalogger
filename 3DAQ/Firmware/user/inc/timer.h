#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32F10x.h"

#define SYS_CLK 24000000    /* in this example we use SPEED_HIGH = 48 MHz */
#define DELAY_TIM_FREQUENCY 1000 /* = 1kHZ -> timer runs in milliseconds */

void delay_init( void );
void logging_timer_init( void );
void delay_ms( uint16_t mSecs );
