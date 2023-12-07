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
        while(Set == I2C_GetStatus(pI2C, I2C_SR_BUSY))        // i2c�����Ƿ���У�0:����״̬��1:ռ��״̬
        {	
					IWDG_Feed();
            if(0ul == (u32TimeOut--))
            {
                return I2C_RET_ERROR;
            }
        }

        I2C_GenerateStart(pI2C , Enable);                     //������ʼ����
    }
    else                                                        //restart
    {
        /* Clear start status flag */
        I2C_ClearStatus(pI2C, I2C_CLR_STARTFCLR);             //�����ʼ��־
        /* Send restart condition */
        I2C_GenerateReStart(pI2C , Enable);                   //�ظ�������ʼ����
    }

    /* Judge if start success*/
    u32TimeOut = TIMEOUT;
    while(1)
    {
			IWDG_Feed();
        enFlagBusy = I2C_GetStatus(pI2C, I2C_SR_BUSY);        //�ȴ����߱�ռ��
        enFlagStartf = I2C_GetStatus(pI2C, I2C_SR_STARTF);    //�ȴ���ʼ�ź�Ϊ1
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
    while(Reset == I2C_GetStatus(pI2C, I2C_SR_TEMPTYF))       //�ȴ��������ݼĴ���Ϊ��
    {
        IWDG_Feed();
        if(0ul == (u32TimeOut--))
        {
            return I2C_RET_ERROR;
        }
    }

    /* Send I2C address */
    I2C_SendData(pI2C, u8Adr);                                //�ѷ������ݼĴ���д��������λ�Ĵ���

    //if(E2_ADDRESS_W == (u8Adr & 0x01u))
    if(!(u8Adr & 0x01u))     /*  C-STAT MISRAC2004-13.7 */      //��Ϊ���һλ0��д
    {
        /* If in master transfer process, Need wait transfer end*/
        uint32_t u32TimeOut = TIMEOUT;
        while(Reset == I2C_GetStatus(pI2C, I2C_SR_TENDF))         //�ȴ��������ݽ�����־
        {
            if(0ul == (u32TimeOut--))
            {
                return I2C_RET_ERROR;
            }
        }
    }

    /* Check ACK */
    u32TimeOut = TIMEOUT;
    while(Set == I2C_GetStatus(pI2C, I2C_SR_NACKDETECTF))     //����ack��־��0���յ�ack��1�����յ�nack
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
        while(Reset == I2C_GetStatus(pI2C, I2C_SR_TEMPTYF))   //��ⷢ�����ݼĴ���Ϊ��
        {
            if(0ul == (u32TimeOut--))
            {
                return I2C_RET_ERROR;
            }
        }

        /* Send one byte data */
        I2C_SendData(pI2C, *pTxData++);                       //���������ݷŵ��Ĵ���

        /* Wait transfer end*/
        u32TimeOut = TIMEOUT;
        while(Reset == I2C_GetStatus(pI2C, I2C_SR_TENDF))     //�ȴ��������ݽ�����־
        {
				
            if(0ul == (u32TimeOut--))
            {
                return I2C_RET_ERROR;
            }
        }

        /* Check ACK */
        u32TimeOut = TIMEOUT;
        while(Set == I2C_GetStatus(pI2C, I2C_SR_NACKDETECTF)) //�ж�ack����
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
        if(i == (u32Size - 1ul))                //�������������һ��
        {
            I2C_NackConfig(pI2C, Enable);     //0��Ӧ����ack��1��Ӧ����nack
        }

        /* Wait receive full flag*/
        u32TimeOut = TIMEOUT;
        while(Reset == I2C_GetStatus(pI2C, I2C_SR_RFULLF))  //�жϽ������ݼĴ����Ƿ�Ϊ��  
        {
            if(0ul == (u32TimeOut--))
            {
                return I2C_RET_ERROR;
            }
        }

        /* read data from register*/
        *pRxData++ = I2C_ReadData(pI2C);          //�����յ������ݴӼĴ�������
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
    while(Reset == I2C_GetStatus(pI2C, I2C_SR_BUSY))  //�ȴ� 0�����߿��У�1������ռ��
    {
			  
        if(0ul == (u32TimeOut--))
        {
            return I2C_RET_ERROR;
        }
    }

    I2C_GenerateStop(pI2C, Enable);   //i2c����ֹͣ�ź�

    /* Wait STOPF */
    u32TimeOut = TIMEOUT;
    while(Reset == I2C_GetStatus(pI2C, I2C_SR_STOPF)) //�ж������Ƿ��⵽ֹͣ�ź�
    { 
        if(0ul == (u32TimeOut--))
        {
            return I2C_RET_ERROR;
        }
    }
    return I2C_RET_OK;
}




