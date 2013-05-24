#include "humidity.h"
#include "LCD.h"

#define SCALINGFACTOR 15 //I just made this number up.
#define OFFSET -42000;

float lookup[17][12]={
//  LESS     5,   10,   15,   20,   25,   30,   35,   40,   45,   50,  MORE,
	{99999,99999,99999,99999,99999,99999,99999,99999,99999,99999,99999,99999},  //LESS
	{20000,20000,14000, 9000, 7200, 5000, 3600, 2500, 1800, 1300, 1100, 1100},  //20
	{ 9900, 9900, 6900, 4600, 3200, 2300, 1700, 1100,  920,  600,  520,  520},  //25
	{ 4400, 4400, 3100, 2000, 1500,  920,  770,  530,  430,  280,  250,  250},  //30
	{ 1900, 1900, 1400,  900,  670,  450,  360,  250,  210,  140,  114,  114},  //35
	{  810,  810,  600,  430,  310,  220,  170,  130,   96,   74,   61,   61},  //40
	{  420,  420,  300,  220,  160,  120,   90,   71,   55,   43,   35,   35},  //45
	{  211,  211,  150,  110,   83,   66,   51,   40,   31,   25,   20,   20},  //50
	{  109,  109,   83,   62,   48,   37,   29,   23,   19,   15,   13,   13},  //55
	{   63,   63,   48,   37,   29,   23,   18,   15,   12, 10.2,  8.7,  8.7},  //60
	{   37,   37,   28,   22,   18,   14,   12,   10,  8.1,  6.9,  5.9,  5.9},  //65
	{   22,   22,   17,   14,   12,  9.6,    8,  6.8,  5.8,  4.9,  4.3,  4.3},  //70
	{   14,   14,   12,  9.4,  7.8,  6.5,  5.5,  4.7,  4.1,  3.4,    3,    3},  //75
	{    9,    9,  7.3,    6,    5,  4.2,  3.8,  3.3,  2.9,  2.4,    2,    2},  //80
	{    6,    6,  4.8,  3.9,  3.3,  2.8,  2.5,  2.2,    2,  1.7,  1.4,  1.4},  //85
	{    4,    4,  3.2,  2.7,  2.2,  1.9,  1.7,  1.5,  1.4,  1.2,  1.1,  1.1},  //90
	{    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0}}; //MORE

int readhumidity(int temperature)
{
	int i;
	float scaledreading;
	int left;
	float propright;
	int leftrh;
	int rightrh;
	scaledreading = SCALINGFACTOR * readcapacitance() +OFFSET;
	
	if (temperature <  5){temperature=5; }
	if (temperature > 50){temperature=50;}
		
  left = temperature/5;
	propright = (temperature%5)/5;

	for (i=1;i<17;i++)
	{
		if (lookup[i][left] < scaledreading)
		{
			leftrh = 10+5*i; //This is the min rh, will add a smidge to it.
			leftrh+= 5* ((lookup[i-1][left]-scaledreading)/(lookup[i-1][left]-lookup[i][left]));
			break;
		}
	}

	for (i=1;i<17;i++)
	{
		if (lookup[i][left+1] < scaledreading)
		{
			rightrh = 10+5*i; //This is the min rh, will add a smidge to it.
			rightrh+= 5* ((lookup[i-1][left+1]-scaledreading)/(lookup[i-1][left+1]-lookup[i][left+1]));
			break;
		}
	}
	
	return (leftrh*(1-propright))+(rightrh*propright);
}

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
