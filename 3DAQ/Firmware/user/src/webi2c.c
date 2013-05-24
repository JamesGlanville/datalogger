#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_rcc.h"
#include "webi2c.h"
#include "UART.h"
#include "LEDs.h"
#include "LCD.h"
#include "timer.h"

/***************************************************************************//**
 * Global variables, private define, macro and typedef
 ******************************************************************************/
GPIO_InitTypeDef  GPIO_InitStructure;
USART_InitTypeDef USART_InitStructure;

#define EEPROM_WriteAddress1    0
#define EEPROM_ReadAddress1     0

#define BufferSize1             (countof(Tx1_Buffer)-1)
#define BufferSize2             (countof(Tx2_Buffer)-1)
#define EEPROM_WriteAddress2    (EEPROM_WriteAddress1 + BufferSize1)
#define EEPROM_ReadAddress2     (EEPROM_ReadAddress1 + BufferSize1)

#define I2C_EE             I2C1
#define I2C_EE_CLK         RCC_APB1Periph_I2C1
#define I2C_EE_GPIO        GPIOB
#define I2C_EE_GPIO_CLK    RCC_APB2Periph_GPIOB
#define I2C_EE_SCL         GPIO_Pin_6
#define I2C_EE_SDA         GPIO_Pin_7

#define I2C_Speed              50000
#define I2C_SLAVE_ADDRESS7     0xA0
#define I2C_FLASH_PAGESIZE     32
#define EEPROM_HW_ADDRESS      0xA0   /* E0 = E1 = E2 = 0 */
#define ACCEL_HW_ADDRESS			 0x98

#define   X_OUT       0x00
#define   Y_OUT       0x01
#define   Z_OUT       0x02
#define   INTSU       0x06
#define   MODE        0x07
#define   SR          0x08

#define EEPROM_BYTES	0x7D00 //32000
#define CONFIGLENGTH	0x0010 //16
#define ZERO					0x00

/* Private macro -------------------------------------------------------------*/
#define countof(a) (sizeof(a) / sizeof(*(a)))

int currentByte;
uint8_t Config[CONFIGLENGTH];

/*
Config data:
first two bits are log length, msb first
third byte is sample period in seconds

*/

/* Private variables ---------------------------------------------------------*/
uint16_t EEPROM_ADDRESS;
uint8_t Tx1_Buffer[] = "/* STM32F10x I2C Firmware ";
uint8_t Test_Buffer[] = "This is written EEPROM data :)";
uint8_t Rx1_Buffer[150];
volatile TestStatus TransferStatus1 = FAILED;
extern volatile unsigned int LEDbyte;
unsigned int LEDGripple[] = {0x001,0x004,0x010,0x040,0x100,0x100,0x040,0x010,0x004,0x001};
unsigned int LEDRripple[] = {0x002,0x008,0x020,0x080,0x200,0x200,0x080,0x020,0x008,0x002};

void I2C_EE_Upload(void)
{
	int percentage;
	uint8_t byte;
	unsigned int LEDbackup;
	int i;
	
	LEDbackup=LEDbyte;
	clear();
	write('S');
	write('e');
	write('n');
	write('d');
	write('i');
	write('n');
	write('g');
	
	for (i=CONFIGLENGTH;i<EEPROM_BYTES;i++)
	{
		if((i%100)==0){
		setCursor(0,1);
		percentage = (100*i)/(EEPROM_BYTES-CONFIGLENGTH);
			writenumber(percentage);
//		write('0'+(i/(EEPROM_BYTES-CONFIGLENGTH)*100)/100);
//		write('0'+((i/(EEPROM_BYTES-CONFIGLENGTH)*100)/10)%10);
//		write('0'+(i/(EEPROM_BYTES-CONFIGLENGTH)*100)%10);
		write('%');
		write(' ');
		write(' ');}
		LEDbyte = LEDGripple[(i/80)%10];
		setLEDS();
		I2C_EE_BufferRead(&byte, i, 1);
		UART_send_byte(byte);
	}
		clear();
	write('C');
	write('O');
	write('M');
	write('P');
	write('L');
	write('E');
	write('T');
	write('E');
	delay_ms(2000);
	clear();

	LEDbyte=LEDbackup;
	setLEDS();
}

void I2C_EE_StartLog(void)
{
	currentByte=CONFIGLENGTH;
}

void I2C_EE_FinishLog(void)
{
//	int loglength = ((int)Config[0])<<8 + Config[1];
	int loglength;
	loglength = (currentByte-CONFIGLENGTH)/ENTRYBYTES;
	Config[0] = loglength>>8;
	Config[1] = loglength % 0x100;
	I2C_EE_WriteConfig();
	clear();
	write('S');
	write('t');
	write('o');
	write('p');
	write('p');
	write('i');
	write('n');
	write('g');
	delay_ms(2000);
	clear();
	write('S');
	write('T');
	write('A');
	write('N');
	write('D');
	write('B');
	write('Y');
	
}

void I2C_EE_Log(uint8_t* LogData)
{
	if (currentByte >=EEPROM_BYTES) {I2C_EE_FinishLog();}
	else
	{
	I2C_EE_BufferWrite(LogData, currentByte, CONFIGLENGTH);
	
	currentByte+=ENTRYBYTES;
	}
}

void I2C_EE_Erase(void)
{
	int percentage;
	unsigned int LEDbackup;
	int i;
	
	clear();
	write('E');
	write('R');
	write('A');
	write('S');
	write('I');
	write('N');
	write('G');
	
	for (i=CONFIGLENGTH;i<EEPROM_BYTES;i++)
	{
		I2C_EE_ByteWrite(ZERO, i);	
		
		if((i%100)==0){
		setCursor(0,1);
		percentage = (100*i)/(EEPROM_BYTES-CONFIGLENGTH);
		writenumber(percentage);
		write('%');
		write(' ');
		write(' ');}
		LEDbyte = LEDRripple[(i/80)%10];
		setLEDS();
	}
		clear();
	write('C');
	write('O');
	write('M');
	write('P');
	write('L');
	write('E');
	write('T');
	write('E');
	delay_ms(2000);
	clear();

	LEDbyte=LEDbackup;
	setLEDS();

}
void I2C_EE_LoadConfig(void)
{
I2C_EE_BufferRead(Config, 0, CONFIGLENGTH);
}

void I2C_EE_WriteConfig(void)
{
I2C_EE_BufferWrite(Config, 0, CONFIGLENGTH);
}

void I2C_ACCEL_READ(void)
{
	int NumByteToRead=3;
  unsigned char XYZ[3];

    while(I2C_GetFlagStatus(I2C_EE, I2C_FLAG_BUSY));

    I2C_GenerateSTART(I2C_EE, ENABLE);

    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C_EE, ACCEL_HW_ADDRESS, I2C_Direction_Transmitter);

    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C_EE, 0x00);

    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTART(I2C_EE, ENABLE);

    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C_EE, ACCEL_HW_ADDRESS, I2C_Direction_Receiver);

    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    while(NumByteToRead)
    {
        if(NumByteToRead == 1)
        {
            /* Disable Acknowledgement */
            I2C_AcknowledgeConfig(I2C_EE, DISABLE);

            /* Send STOP Condition */
            I2C_GenerateSTOP(I2C_EE, ENABLE);
        }

    /* Test on EV7 and clear it */
        if(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_RECEIVED))
        {
            /* Read a byte from the EEPROM */
            XYZ[3-NumByteToRead] = I2C_ReceiveData(I2C_EE);

            /* Point to the next location where the byte read will be saved */
            //pBuffer++;

            /* Decrement the read bytes counter */
            NumByteToRead--;
        }
    }

    /* Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(I2C_EE, ENABLE);
}

void I2C_ACCEL_INIT(void)
{
   I2C_GenerateSTART(I2C_EE, ENABLE);
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C_EE, ACCEL_HW_ADDRESS, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    I2C_SendData(I2C_EE, MODE);
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_SendData(I2C_EE, 0x00);
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_GenerateSTOP(I2C_EE, ENABLE);
	
   I2C_GenerateSTART(I2C_EE, ENABLE);
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C_EE, ACCEL_HW_ADDRESS, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    I2C_SendData(I2C_EE, INTSU);
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_SendData(I2C_EE, 0x10);
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_GenerateSTOP(I2C_EE, ENABLE);
	
   I2C_GenerateSTART(I2C_EE, ENABLE);
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C_EE, ACCEL_HW_ADDRESS, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    I2C_SendData(I2C_EE, SR);
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_SendData(I2C_EE, 0x00);
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_GenerateSTOP(I2C_EE, ENABLE);
		
   I2C_GenerateSTART(I2C_EE, ENABLE);
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C_EE, ACCEL_HW_ADDRESS, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    I2C_SendData(I2C_EE, MODE);
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_SendData(I2C_EE, 0x01);
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_GenerateSTOP(I2C_EE, ENABLE);

		I2C_ACCEL_READ(); //Dummy read, first one is always zeros.
}

/***************************************************************************//**
 * @brief  First, the content of Tx1_Buffer is written to the EEPROM_WriteAddress1
 *         and the written data are read. The written and the read buffers data are
 *         then compared. Following the read operation, the program wait that the
 *         EEPROM reverts to its Standby state. A second write operation is, then,
 *         performed and this time, Tx2_Buffer is written to EEPROM_WriteAddress2,
 *         which represents the address just after the last written one in the first
 *         write. After completion of the second write operation, the written data
 *         are read. The contents of the written and the read buffers are compared.
 ******************************************************************************/
void I2C_EEPROM(void)
{
    /* System clocks configuration */
    RCC_Configuration();
    /* USART configuration */
    /* Initialize the I2C EEPROM driver */
    I2C_EE_Init();

    /* First write in the memory followed by a read of the written data */
    /* Write on I2C EEPROM from EEPROM_WriteAddress1 */
 //   I2C_EE_BufferWrite(Test_Buffer, EEPROM_WriteAddress1, 100);
//	while(1);
    /* Read from I2C EEPROM from EEPROM_ReadAddress1 */
 //   I2C_EE_BufferRead(Rx1_Buffer, EEPROM_ReadAddress1, 100);

    /* Check if the data written to the memory is read correctly */
 //   TransferStatus1 = Buffercmp(Tx1_Buffer, Rx1_Buffer, BufferSize1);   
}

/***************************************************************************//**
 * @brief  Configures the different system clocks.
 ******************************************************************************/
void RCC_Configuration(void)
{
    /* Setup the microcontroller system. Initialize the Embedded Flash Interface,
       initialize the PLL and update the SystemFrequency variable. */
    SystemInit();
}

void GPIO_Configuration(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    //GPIO_PinRemapConfig(GPIO_Remap_I2C1,ENABLE);

    /* Configure I2C_EE pins: SCL and SDA */
    GPIO_InitStructure.GPIO_Pin =  I2C_EE_SCL | I2C_EE_SDA;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(I2C_EE_GPIO, &GPIO_InitStructure);
}

/***************************************************************************//**
 * @brief  Configure USART2
 ******************************************************************************/


/***************************************************************************//**
 *  @brief  I2C Configuration
 ******************************************************************************/
void I2C_Configuration(void)
{
    I2C_InitTypeDef  I2C_InitStructure;

    /* I2C configuration */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = I2C_SLAVE_ADDRESS7;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;

    /* I2C Peripheral Enable */
    I2C_Cmd(I2C_EE, ENABLE);
    /* Apply I2C configuration after enabling it */
    I2C_Init(I2C_EE, &I2C_InitStructure);
}

/***************************************************************************//**
 * @brief  Initializes peripherals used by the I2C EEPROM driver.
 ******************************************************************************/
void I2C_EE_Init()
{
    /* I2C Periph clock enable */
    RCC_APB1PeriphClockCmd(I2C_EE_CLK, ENABLE);

    /* GPIO Periph clock enable */
    RCC_APB2PeriphClockCmd(I2C_EE_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);

    /* GPIO configuration */
    GPIO_Configuration();

    /* I2C configuration */
    I2C_Configuration();

    /* Select the EEPROM address according to the state of E0, E1, E2 pins */
    EEPROM_ADDRESS = EEPROM_HW_ADDRESS;
}

/***************************************************************************//**
 * @brief      Writes one byte to the I2C EEPROM.
 * @param[in]  pBuffer   : pointer to the buffer  containing the data to be
 *                         written to the EEPROM.
 * @param[in]  WriteAddr : EEPROM's internal address to write to.
 * @return     None
 ******************************************************************************/
void I2C_EE_ByteWrite(uint8_t* pBuffer, uint16_t WriteAddr)
{
    /* Send STRAT condition */
    I2C_GenerateSTART(I2C_EE, ENABLE);

    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT));

    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2C_EE, EEPROM_ADDRESS, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    /* Send the EEPROM's internal address to write to : MSB of the address first */
    I2C_SendData(I2C_EE, (uint8_t)((WriteAddr & 0xFF00) >> 8));

    /* Test on EV8 and clear it */
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* Send the EEPROM's internal address to write to : LSB of the address */
    I2C_SendData(I2C_EE, (uint8_t)(WriteAddr & 0x00FF));

    /* Test on EV8 and clear it */
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* Send the byte to be written */
    I2C_SendData(I2C_EE, *pBuffer);

    /* Test on EV8 and clear it */
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* Send STOP condition */
    I2C_GenerateSTOP(I2C_EE, ENABLE);
}

/***************************************************************************//**
 * @brief      Reads a block of data from the EEPROM.
 * @param[in]  pBuffer : pointer to the buffer that receives the data read
 *                       from the EEPROM.
 * @param[in]  ReadAddr : EEPROM's internal address to read from.
 * @param[in]  NumByteToRead : number of bytes to read from the EEPROM.
 * @return     None
 ******************************************************************************/
void I2C_EE_BufferRead(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead)
{
    /* While the bus is busy */
    while(I2C_GetFlagStatus(I2C_EE, I2C_FLAG_BUSY));

    /* Send START condition */
    I2C_GenerateSTART(I2C_EE, ENABLE);

    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT));

    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2C_EE, EEPROM_ADDRESS, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    /* Send the EEPROM's internal address to read from: MSB of the address first */
    I2C_SendData(I2C_EE, (uint8_t)((ReadAddr & 0xFF00) >> 8));

    /* Test on EV8 and clear it */
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* Send the EEPROM's internal address to read from: LSB of the address */
    I2C_SendData(I2C_EE, (uint8_t)(ReadAddr & 0x00FF));

    /* Test on EV8 and clear it */
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* Send STRAT condition a second time */
    I2C_GenerateSTART(I2C_EE, ENABLE);

    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT));

    /* Send EEPROM address for read */
    I2C_Send7bitAddress(I2C_EE, EEPROM_ADDRESS, I2C_Direction_Receiver);

    /* Test on EV6 and clear it */
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    /* While there is data to be read */
    while(NumByteToRead)
    {
        if(NumByteToRead == 1)
        {
            /* Disable Acknowledgement */
            I2C_AcknowledgeConfig(I2C_EE, DISABLE);

            /* Send STOP Condition */
            I2C_GenerateSTOP(I2C_EE, ENABLE);
        }

    /* Test on EV7 and clear it */
        if(I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_RECEIVED))
        {
            /* Read a byte from the EEPROM */
            *pBuffer = I2C_ReceiveData(I2C_EE);

            /* Point to the next location where the byte read will be saved */
            pBuffer++;

            /* Decrement the read bytes counter */
            NumByteToRead--;
        }
    }

    /* Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(I2C_EE, ENABLE);
}

/***************************************************************************//**
 * @brief      Writes buffer of data to the I2C EEPROM.
 * @param[in]  pBuffer : pointer to the buffer  containing the data to be
 *                       written to the EEPROM.
 * @param[in]  WriteAddr : EEPROM's internal address to write to.
 * @param[in]  NumByteToWrite : number of bytes to write to the EEPROM.
 * @return     None
 ******************************************************************************/
void I2C_EE_BufferWrite(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite)
{
    uint8_t NumOfPage = 0, NumOfSingle = 0, count = 0;
    uint16_t Addr = 0;

    Addr = WriteAddr % I2C_FLASH_PAGESIZE;
    count = I2C_FLASH_PAGESIZE - Addr;
    NumOfPage =  NumByteToWrite / I2C_FLASH_PAGESIZE;
    NumOfSingle = NumByteToWrite % I2C_FLASH_PAGESIZE;

    /* If WriteAddr is I2C_FLASH_PAGESIZE aligned  */
    if(Addr == 0)
    {
        /* If NumByteToWrite < I2C_FLASH_PAGESIZE */
        if(NumOfPage == 0)
        {
            I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
            I2C_EE_WaitEepromStandbyState();
        }
        /* If NumByteToWrite > I2C_FLASH_PAGESIZE */
        else
        {
            while(NumOfPage--)
            {
                I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_FLASH_PAGESIZE);
                I2C_EE_WaitEepromStandbyState();
                WriteAddr +=  I2C_FLASH_PAGESIZE;
                pBuffer += I2C_FLASH_PAGESIZE;
            }

            if(NumOfSingle!=0)
            {
                I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
                I2C_EE_WaitEepromStandbyState();
            }
        }
    }
    /* If WriteAddr is not I2C_FLASH_PAGESIZE aligned  */
    else
    {
        /* If NumByteToWrite < I2C_FLASH_PAGESIZE */
        if(NumOfPage== 0)
        {
            /* If the number of data to be written is more than the remaining space
              in the current page: */
            if (NumByteToWrite > count)
            {
                /* Write the data conained in same page */
                I2C_EE_PageWrite(pBuffer, WriteAddr, count);
                I2C_EE_WaitEepromStandbyState();

                /* Write the remaining data in the following page */
                I2C_EE_PageWrite((uint8_t*)(pBuffer + count), (WriteAddr + count), (NumByteToWrite - count));
                I2C_EE_WaitEepromStandbyState();
            }
            else
            {
                I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
                I2C_EE_WaitEepromStandbyState();
            }
        }
        /* If NumByteToWrite > I2C_FLASH_PAGESIZE */
        else
        {
            NumByteToWrite -= count;
            NumOfPage =  NumByteToWrite / I2C_FLASH_PAGESIZE;
            NumOfSingle = NumByteToWrite % I2C_FLASH_PAGESIZE;

            if(count != 0)
            {
                I2C_EE_PageWrite(pBuffer, WriteAddr, count);
                I2C_EE_WaitEepromStandbyState();
                WriteAddr += count;
                pBuffer += count;
            }

            while(NumOfPage--)
            {
                I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_FLASH_PAGESIZE);
                I2C_EE_WaitEepromStandbyState();
                WriteAddr +=  I2C_FLASH_PAGESIZE;
                pBuffer += I2C_FLASH_PAGESIZE;
            }
            if(NumOfSingle != 0)
            {
                I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
                I2C_EE_WaitEepromStandbyState();
            }
        }
    }
}

/***************************************************************************//**
 * @brief      Writes more than one byte to the EEPROM with a single WRITE cycle.
 *             Note: The number of byte can't exceed the EEPROM page size.
 * @param[in]  pBuffer : pointer to the buffer containing the data to be
 *                       written to the EEPROM.
 * @param[in]  WriteAddr : EEPROM's internal address to write to.
 * @param[in]  NumByteToWrite : number of bytes to write to the EEPROM.
 * @return     None
 ******************************************************************************/
void I2C_EE_PageWrite(uint8_t* pBuffer, uint16_t WriteAddr, uint8_t NumByteToWrite)
{
    /* While the bus is busy */
    while(I2C_GetFlagStatus(I2C_EE, I2C_FLAG_BUSY));

    /* Send START condition */
    I2C_GenerateSTART(I2C_EE, ENABLE);

    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT));

    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2C_EE, EEPROM_ADDRESS, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    /* Send the EEPROM's internal address to write to : MSB of the address first */
    I2C_SendData(I2C_EE, (uint8_t)((WriteAddr & 0xFF00) >> 8));

    /* Test on EV8 and clear it */
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* Send the EEPROM's internal address to write to : LSB of the address */
    I2C_SendData(I2C_EE, (uint8_t)(WriteAddr & 0x00FF));

    /* Test on EV8 and clear it */
    while(! I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* While there is data to be written */
    while(NumByteToWrite--)
    {
        /* Send the current byte */
        I2C_SendData(I2C_EE, *pBuffer);

        /* Point to the next byte to be written */
        pBuffer++;

        /* Test on EV8 and clear it */
        while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    }

    /* Send STOP condition */
    I2C_GenerateSTOP(I2C_EE, ENABLE);
}

/***************************************************************************//**
 * @brief  Wait for EEPROM Standby state
 ******************************************************************************/
void I2C_EE_WaitEepromStandbyState(void)
{
    __IO uint16_t SR1_Tmp = 0;

    do
    {
        /* Send START condition */
        I2C_GenerateSTART(I2C_EE, ENABLE);

        /* Read I2C_EE SR1 register to clear pending flags */
        SR1_Tmp = I2C_ReadRegister(I2C_EE, I2C_Register_SR1);

        /* Send EEPROM address for write */
        I2C_Send7bitAddress(I2C_EE, EEPROM_ADDRESS, I2C_Direction_Transmitter);
    }while(!(I2C_ReadRegister(I2C_EE, I2C_Register_SR1) & 0x0002));

    /* Clear AF flag */
    I2C_ClearFlag(I2C_EE, I2C_FLAG_AF);

    /* STOP condition */
    I2C_GenerateSTOP(I2C_EE, ENABLE);
}

/**************************************************************************//**
 * @brief    : Compare two buffers.
 * @param[in]  pBuffer  : buffer to be compared
 * @param[in]  PBuffer1 : buffer to be compared
 * @param[in]  BufferLength : buffer's length
 * @return   :
 *      @li PASSED : pBuffer identical to pBuffer1
 *      @li FAILED : pBuffer differs from pBuffer1
 ******************************************************************************/
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
    while(BufferLength--)
    {
        if(*pBuffer1 != *pBuffer2)
        {
            return FAILED;
        }
        pBuffer1++;
        pBuffer2++;
    }
    return PASSED;
}
