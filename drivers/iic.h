#ifndef __IIC_H
#define __IIC_H

#include "config.h"

#define TIMEOUT                         ((uint32_t)0x10000)	//((uint32_t)0x800)

#define I2C_RET_OK                      0u
#define I2C_RET_ERROR                   1u

#define GENERATE_START                  0x00u
#define GENERATE_RESTART                0x01u

#define ADDRESS_W                       0x00u
#define ADDRESS_R                       0x01u

/* Define Write and read data length for the example */
#define TEST_DATA_LEN                   256u
/* Define i2c baudrate */
#define I2C_BAUDRATE                    100000ul


extern uint8_t Master_StartOrRestart(M4_I2C_TypeDef* pI2C,uint8_t u8Start);
extern uint8_t Master_SendAdr(M4_I2C_TypeDef* pI2C,uint8_t u8Adr);
extern uint8_t Master_WriteData(M4_I2C_TypeDef* pI2C,uint8_t *pTxData, uint32_t u32Size);
extern uint8_t Master_RevData(M4_I2C_TypeDef* pI2C,uint8_t *pRxData, uint32_t u32Size);
extern void JudgeResult(uint8_t u8Result);
uint8_t Master_Stop(M4_I2C_TypeDef* pI2C);

#endif
