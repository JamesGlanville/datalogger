typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

void I2C_EE_Erase(void);
void I2C_EE_WriteConfig(void);
void I2C_EE_LoadConfig(void);
void RCC_Configuration(void);
void GPIO_Configuration(void);
void USART_Configuration(void);
void I2C_Configuration(void);
void I2C_EE_Init(void);
void I2C_EE_ByteWrite(uint8_t* pBuffer, uint16_t WriteAddr);
void I2C_EE_PageWrite(uint8_t* pBuffer, uint16_t WriteAddr, uint8_t NumByteToWrite);
void I2C_EE_BufferWrite(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite);
void I2C_EE_BufferRead(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead);
void I2C_EE_WaitEepromStandbyState(void);
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
void I2C_EEPROM(void);
void I2C_ACCEL_INIT(void);
void I2C_ACCEL_READ(void);

void I2C_EE_StartLog(void);
void I2C_EE_FinishLog(void);
void I2C_EE_Log(uint8_t* LogData);
void I2C_EE_Erase(void);
void I2C_EE_LoadConfig(void);
void I2C_EE_WriteConfig(void);
