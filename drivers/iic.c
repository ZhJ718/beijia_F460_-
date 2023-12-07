#include "iic.h"


/**
 ******************************************************************************
 ** \brief  Send start or restart condition
 **
 ** \param  none
 **
 ** \retval Process result
 **         - I2C_RET_ERROR  Send start or restart failed
 **         - I2C_RET_OK     Send start or restart success
 ******************************************************************************/
extern uint8_t Master_StartOrRestart(M4_I2C_TypeDef *pI2C,uint8_t u8Start)
{
    uint32_t u32TimeOut = TIMEOUT;
    en_flag_status_t enFlagBusy = Reset;
    en_flag_status_t enFlagStartf = Reset;

    /* generate start or restart signal */
    //if(GENERATE_START == u8Start)
    if(!u8Start)                                                //start
    {
        /* Wait I2C bus idle */
        while(Set == I2C_GetStatus(pI2C, I2C_SR_BUSY))        // i2c总线是否空闲，0:空闲状态，1:占有状态
        {	
					IWDG_Feed();
            if(0ul == (u32TimeOut--))
            {
                return I2C_RET_ERROR;
            }
        }

        I2C_GenerateStart(pI2C , Enable);                     //生成起始条件
    }
    else                                                        //restart
    {
        /* Clear start status flag */
        I2C_ClearStatus(pI2C, I2C_CLR_STARTFCLR);             //清除开始标志
        /* Send restart condition */
        I2C_GenerateReStart(pI2C , Enable);                   //重复生成起始条件
    }

    /* Judge if start success*/
    u32TimeOut = TIMEOUT;
    while(1)
    {
			IWDG_Feed();
        enFlagBusy = I2C_GetStatus(pI2C, I2C_SR_BUSY);        //等待总线被占有
        enFlagStartf = I2C_GetStatus(pI2C, I2C_SR_STARTF);    //等待起始信号为1
        if(enFlagBusy && enFlagStartf)
        {
            break;
        }
        if(0ul == (u32TimeOut--))
        {
            return I2C_RET_ERROR;
        }
    }

    return I2C_RET_OK;
}

/**
 ******************************************************************************
 ** \brief  Send slave address
 **
 ** \param  u16Adr  The slave address
 **
 ** \retval Process result
 **         - I2C_RET_ERROR  Send failed
 **         - I2C_RET_OK     Send success
 ******************************************************************************/
extern uint8_t Master_SendAdr(M4_I2C_TypeDef *pI2C,uint8_t u8Adr)
{
    uint32_t u32TimeOut = TIMEOUT;

    /* Wait tx buffer empty */
    while(Reset == I2C_GetStatus(pI2C, I2C_SR_TEMPTYF))       //等待发送数据寄存器为空
    {
        IWDG_Feed();
        if(0ul == (u32TimeOut--))
        {
            return I2C_RET_ERROR;
        }
    }

    /* Send I2C address */
    I2C_SendData(pI2C, u8Adr);                                //把发送数据寄存器写到数据移位寄存器

    //if(E2_ADDRESS_W == (u8Adr & 0x01u))
    if(!(u8Adr & 0x01u))     /*  C-STAT MISRAC2004-13.7 */      //因为最后一位0是写
    {
        /* If in master transfer process, Need wait transfer end*/
        uint32_t u32TimeOut = TIMEOUT;
        while(Reset == I2C_GetStatus(pI2C, I2C_SR_TENDF))         //等待发送数据结束标志
        {
            if(0ul == (u32TimeOut--))
            {
                return I2C_RET_ERROR;
            }
        }
    }

    /* Check ACK */
    u32TimeOut = TIMEOUT;
    while(Set == I2C_GetStatus(pI2C, I2C_SR_NACKDETECTF))     //接收ack标志：0接收到ack，1：接收到nack
    {
        if(0ul == (u32TimeOut--))
        {
            return I2C_RET_ERROR;
        }
    }

    return I2C_RET_OK;
}


/**
 ******************************************************************************
 ** \brief  Send data to slave
 **
 ** \param  pTxData  Pointer to the data buffer
 ** \param  u32Size  Data size
 **
 ** \retval Process result
 **         - I2C_RET_ERROR  Send failed
 **         - I2C_RET_OK     Send success
 ******************************************************************************/
extern uint8_t Master_WriteData(M4_I2C_TypeDef *pI2C,uint8_t *pTxData, uint32_t u32Size)
{
    uint32_t u32TimeOut = TIMEOUT;

    while(u32Size--)
    {
			IWDG_Feed();
        /* Wait tx buffer empty */
        u32TimeOut = TIMEOUT;
        while(Reset == I2C_GetStatus(pI2C, I2C_SR_TEMPTYF))   //检测发送数据寄存器为空
        {
            if(0ul == (u32TimeOut--))
            {
                return I2C_RET_ERROR;
            }
        }

        /* Send one byte data */
        I2C_SendData(pI2C, *pTxData++);                       //将发送数据放到寄存器

        /* Wait transfer end*/
        u32TimeOut = TIMEOUT;
        while(Reset == I2C_GetStatus(pI2C, I2C_SR_TENDF))     //等待发送数据结束标志
        {
				
            if(0ul == (u32TimeOut--))
            {
                return I2C_RET_ERROR;
            }
        }

        /* Check ACK */
        u32TimeOut = TIMEOUT;
        while(Set == I2C_GetStatus(pI2C, I2C_SR_NACKDETECTF)) //判断ack到来
        {
            if(0ul == (u32TimeOut--))
            {
                return I2C_RET_ERROR;
            }
        }
    }

    return I2C_RET_OK;
}

/**
 ******************************************************************************
 ** \brief  Receive data from slave
 **
 ** \param  pTxData  Pointer to the data buffer
 ** \param  u32Size  Data size
 **
 ** \retval Process result
 **         - I2C_RET_ERROR  Process failed
 **         - I2C_RET_OK     Process success
 ******************************************************************************/
extern uint8_t Master_RevData(M4_I2C_TypeDef* pI2C,uint8_t *pRxData, uint32_t u32Size)
{
    uint32_t u32TimeOut = TIMEOUT;

    for(uint32_t i=0ul; i<u32Size; i++)
    {
        /* if the last byte receive, need config NACK*/
        if(i == (u32Size - 1ul))                //接收数据是最后一个
        {
            I2C_NackConfig(pI2C, Enable);     //0：应答发送ack，1：应答发送nack
        }

        /* Wait receive full flag*/
        u32TimeOut = TIMEOUT;
        while(Reset == I2C_GetStatus(pI2C, I2C_SR_RFULLF))  //判断接收数据寄存器是否为满  
        {
            if(0ul == (u32TimeOut--))
            {
                return I2C_RET_ERROR;
            }
        }

        /* read data from register*/
        *pRxData++ = I2C_ReadData(pI2C);          //将接收到的数据从寄存器读出
    }
    return I2C_RET_OK;
}

/**
 ******************************************************************************
 ** \brief  General stop condition to slave
 **
 ** \param  None
 **
 ** \retval Process result
 **         - I2C_RET_ERROR  Send failed
 **         - I2C_RET_OK     Send success
 ******************************************************************************/
uint8_t Master_Stop(M4_I2C_TypeDef* pI2C)
{
    uint32_t u32TimeOut;

    /* Wait I2C bus busy */
    u32TimeOut = TIMEOUT;
    while(Reset == I2C_GetStatus(pI2C, I2C_SR_BUSY))  //等待 0：总线空闲，1：总线占有
    {
			  
        if(0ul == (u32TimeOut--))
        {
            return I2C_RET_ERROR;
        }
    }

    I2C_GenerateStop(pI2C, Enable);   //i2c产生停止信号

    /* Wait STOPF */
    u32TimeOut = TIMEOUT;
    while(Reset == I2C_GetStatus(pI2C, I2C_SR_STOPF)) //判断总线是否检测到停止信号
    { 
        if(0ul == (u32TimeOut--))
        {
            return I2C_RET_ERROR;
        }
    }
    return I2C_RET_OK;
}




