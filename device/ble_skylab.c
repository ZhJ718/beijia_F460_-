/**
  ******************************************************************************
  * @file    ble_skylab.c
  * @author  xfw
  * @brief   This file contains definitions for skylab's ble module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 Beijing HuaRu Tech Co.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of HuaRu nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ble_skylab.h"

void HexToAscii(uint8_t *old, uint8_t len, uint8_t *new)
{
    uint8_t cnt;

    for(cnt = 0; cnt < len; cnt++)
    {
        if(((*old >> 4) & 0xf) > 9)
            *new = 0x41 + ((*old >> 4) & 0xf) - 10;
        else
            *new = 0x30 + ((*old >> 4) & 0xf);

        new++;

        if((*old & 0xf) > 9)
            *new = 0x41 + (*old & 0xf) - 10;
        else
            *new = 0x30 + (*old & 0xf);

        new++;
        old++;
    }
}

//֧�ִ�д
void AsciiToHex(uint8_t *old, uint8_t len, uint8_t *new)
{
    uint8_t i = 0, cnt;

    for(cnt = 0; cnt < len;)
    {

        if((old[cnt] >= '0') && (old[cnt] <= '9'))
        {
            new[i] = old[cnt] - 0x30;
        }
        else if((old[cnt] >= 'a') && (old[cnt] <= 'f'))
        {
            new[i] = old[cnt] - 0x61 + 10;
        }
        else if ((old[cnt] >= 'A') && (old[cnt] <= 'F'))
        {
            new[i] = old[cnt] - 0x41 + 10;
        }

        cnt++;
        new[i] *= 0x10;

        if((old[cnt] >= '0') && (old[cnt] <= '9'))
        {
            new[i] += old[cnt] - 0x30;
        }
        else if((old[cnt] >= 'a') && (old[cnt] <= 'f'))
        {
            new[i] += old[cnt] - 0x61 + 10;
        }
        else if ((old[cnt] >= 'A') && (old[cnt] <= 'F'))
        {
            new[i] += old[cnt] - 0x41 + 10;
        }

        //old++;
        cnt++;
        i++;
    }
}

//�õ�AT��Ӧ�ַ�������
uint32_t Ble_ATStringLen(uint8_t *pBuf, uint16_t *pLen)
{
    uint16_t i;

    i = 0;

    while(i < *pLen)
    {
        if(pBuf[i] == 0x0D && pBuf[i + 1] == 0x0A)
        {
            *pLen = i + 2;
            return TRUE;
        }

        i++;
    }

    return FALSE;
}


//�����ذ��Ƿ�����ATָ��֡
uint8_t Ble_FindATFrame(uint8_t *pBuf, uint16_t *pLen, uint32_t *pIsCutted)
{

    uint16_t len;

    if((pBuf != NULL) && (pLen != NULL))
    {
        if(pBuf[0] == '[')
        {

            len = *pLen;

            if(TRUE  == Ble_ATStringLen(pBuf, &len)) //֡��
            {
                *pLen = len;

                if(pIsCutted != NULL)
                {
                    *pIsCutted = FALSE;
                }

                return TRUE;
            }
            else
            {
                if(pIsCutted != NULL)
                {
                    *pIsCutted = TRUE;
                }

                return	TRUE;
            }
        }
    }

    if (pIsCutted != NULL)
    {
        *pIsCutted	= FALSE;
    }

    return FALSE;
}

/****************************************************************
@FUNCTION���칤�������ģ��ײ�Ӳ����ʼ��
@INPUT��Ӳ���ӿں����ṹ�壬BLE����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �󶨵ײ�Ӳ��������������ʼ�����ϵ磬��ס��λ����
****************************************************************/
extern void Ble_LowLevelInit(BLE_SKYLAB_HW_OPERATIONS a_sHwOps, BLE_SKYLAB_LOWLEVEL_MAG *pMag)
{
    assert_param(pMag != NULL);

    memset(pMag, 0, sizeof(BLE_SKYLAB_LOWLEVEL_MAG));

    pMag->m_sHwOps.hw_init = a_sHwOps.hw_init;
    pMag->m_sHwOps.gpio_power_on = a_sHwOps.gpio_power_on;
    pMag->m_sHwOps.gpio_power_off = a_sHwOps.gpio_power_off;
    pMag->m_sHwOps.gpio_reset = a_sHwOps.gpio_reset;
    pMag->m_sHwOps.gpio_set = a_sHwOps.gpio_set;

    pMag->m_sHwOps.read = a_sHwOps.read;
    pMag->m_sHwOps.write = a_sHwOps.write;

    #if 0
    //��ȡ����MACȡĬ��
    pMag->m_byIsGetMac = BLE_SKYLAB_GETMAC_DEFAULT;

    //�Ƿ���Ҫ�����豸����
    if(a_strName != NULL)
    {
        strncpy((void *)pMag->m_strDevName, (void *)a_strName, MIN(sizeof(pMag->m_strDevName), strlen((void *)a_strName)));
        pMag->m_byIsSetDevName = TRUE;
    }

    //�Ƿ���Ҫ���÷��书��
    pMag->m_enumTxPwr = a_enumTxPwr;
    pMag->m_byIsSetTxPwr = (pMag->m_enumTxPwr == BLE_TXPWR_4DB ? FALSE : TRUE);
    #endif

    //IO��ʼ��
    if(pMag->m_sHwOps.hw_init != NULL)
    {
        pMag->m_sHwOps.hw_init();
    }

    //�ϵ�
    if(pMag->m_sHwOps.gpio_power_on != NULL)
    {
        pMag->m_sHwOps.gpio_power_on();
    }

    //��λ
    if(pMag->m_sHwOps.gpio_reset != NULL)
    {
        pMag->m_sHwOps.gpio_reset();
    }
}

/****************************************************************
@FUNCTION���칤�������ģ�鸴λ
@INPUT��BLE�ײ����ṹ��ָ�룬��λ������>0��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��λʱ��=Ble_LowLevelTimedProcess���ȼ�� * ��λ����
		�˺�����λ����������Ч������λ����Ϊ0����ģ�鸴λһ����λ�ĵ��ȼ��
****************************************************************/
void Ble_Reset(BLE_SKYLAB_LOWLEVEL_MAG *pMag, uint8_t Timer)
{
    pMag->m_sHwOps.gpio_reset();
    pMag->m_byResetSetCnt = Timer;
    pMag->m_byResetNowCnt = 0;
    pMag->m_byIsReset = TRUE;
}

/****************************************************************
@FUNCTION���칤�������ģ�鸴λ������ͣ
@INPUT��BLE�ײ����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �˺���������Ч
****************************************************************/
void Ble_ResetSuspend(BLE_SKYLAB_LOWLEVEL_MAG *pMag)
{
    pMag->m_sHwOps.gpio_set();
    pMag->m_byIsReset = FALSE;
}

/****************************************************************
@FUNCTION���칤�������ģ�鸴λ�����ָ�
@INPUT��BLE�ײ����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ����ʱ������λ�����Ѿ�����������Ӱ��
****************************************************************/
void Ble_ResetRecovery(BLE_SKYLAB_LOWLEVEL_MAG *pMag)
{
    if((pMag->m_byIsReset == FALSE) && (pMag->m_byResetSetCnt > pMag->m_byResetNowCnt))
    {
        Ble_Reset(pMag, pMag->m_byResetSetCnt - pMag->m_byResetNowCnt);
    }

}

/****************************************************************
@FUNCTION���칤�������ģ���ȡ����MAC
@INPUT��BLE�ײ����ṹ��ָ��,����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��
****************************************************************/
void Ble_GetMac(BLE_SKYLAB_LOWLEVEL_MAG *pMag, uint8_t *a_sRes1, uint16_t a_wRes2)
{
    uint8_t ATBuf[50] = BLE_ATCMD_GETMAC;
    uint16_t ATLen = strlen(BLE_ATCMD_GETMAC);

    if(pMag->m_sHwOps.write != NULL)
    {
        pMag->m_sHwOps.write(ATBuf, ATLen);
    }

}

/****************************************************************
@FUNCTION���칤�������ģ�������豸����
@INPUT��BLE�ײ����ṹ��ָ��,�豸����,�豸���Ƴ���
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:����У��
****************************************************************/
void Ble_SetDevName(BLE_SKYLAB_LOWLEVEL_MAG *pMag, uint8_t *a_strName, uint16_t NameLen)
{
    uint8_t ATBuf[50] = BLE_ATCMD_DEVNAME;
    uint16_t ATLen = strlen(BLE_ATCMD_DEVNAME);

    if(a_strName == NULL)
        return;

    if(NameLen == 0 || NameLen > 20)
        return;

    memcpy((void *)&ATBuf[ATLen], a_strName, NameLen);
    ATLen += NameLen;
    ATBuf[ATLen++] = '\r';
    ATBuf[ATLen++] = '\n';

    if(pMag->m_sHwOps.write != NULL)
    {
        pMag->m_sHwOps.write(ATBuf, ATLen);
    }

}

/****************************************************************
@FUNCTION���칤�������ģ�����÷��书��
@INPUT��BLE�ײ����ṹ��ָ��,���书��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Ble_SetTxPwr(BLE_SKYLAB_LOWLEVEL_MAG *pMag, ENUM_BLE_TXPWR a_enumTxPwr)
{
    uint8_t ATBuf[50] = BLE_ATCMD_TXPWR;
    uint16_t ATLen = strlen(BLE_ATCMD_TXPWR);

    switch(a_enumTxPwr)
    {
        case BLE_TXPWR_0DB:
            memcpy((void *)&ATBuf[ATLen], "0", 1);
            ATLen += 1;
            break;

        case BLE_TXPWR_N4DB:
            memcpy((void *)&ATBuf[ATLen], "-4", 2);
            ATLen += 2;
            break;

        case BLE_TXPWR_N8DB:
            memcpy((void *)&ATBuf[ATLen], "-8", 2);
            ATLen += 2;
            break;

        case BLE_TXPWR_N12DB:
            memcpy((void *)&ATBuf[ATLen], "-12", 3);
            ATLen += 3;
            break;

        case BLE_TXPWR_N16DB:
            memcpy((void *)&ATBuf[ATLen], "-16", 3);
            ATLen += 3;
            break;

        case BLE_TXPWR_N20DB:
            memcpy((void *)&ATBuf[ATLen], "-20", 3);
            ATLen += 3;
            break;

        case BLE_TXPWR_N30DB:
            memcpy((void *)&ATBuf[ATLen], "-30", 3);
            ATLen += 3;
            break;

        default:
            memcpy((void *)&ATBuf[ATLen], "4", 1);
            ATLen += 1;
            break;

    }

    ATBuf[ATLen++] = '\r';
    ATBuf[ATLen++] = '\n';

    if(pMag->m_sHwOps.write != NULL)
    {
        pMag->m_sHwOps.write(ATBuf, ATLen);
    }

}



/****************************************************************
@FUNCTION���칤�������ģ������ָ��MAC��ַ�豸
@INPUT��BLE�ײ����ṹ��ָ��,MAC��ַ(Hex��ʽ)��MAC��ַ��ռ�ռ�
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: MAC��ַ��ռ�ռ���Ҫ����6�ֽ�
****************************************************************/
void Ble_ConnectMac(BLE_SKYLAB_LOWLEVEL_MAG *pMag, uint8_t *a_sPeerMacHex, uint16_t a_wPeerMacHexLen)
{
    uint8_t ATBuf[50] = BLE_ATCMD_CONNECT_MAC;
    uint16_t ATLen = strlen(BLE_ATCMD_CONNECT_MAC);
    uint8_t i;

    if(a_wPeerMacHexLen < 6)
        return;

    for(i = 0; i < 6; i++)
    {
        HexToAscii(a_sPeerMacHex + i, 1, &ATBuf[i * 3 + 11]);
    }

    if(pMag->m_sHwOps.write != NULL)
    {
        pMag->m_sHwOps.write(ATBuf, ATLen);
    }

}

/****************************************************************
@FUNCTION���칤�������ģ��Ͽ�����
@INPUT��BLE�ײ����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Ble_DisConnect(BLE_SKYLAB_LOWLEVEL_MAG *pMag, uint8_t *a_sRes1, uint16_t a_wRes2)
{
    uint8_t ATBuf[50] = BLE_ATCMD_DISCONNECT;
    uint16_t ATLen = strlen(BLE_ATCMD_DISCONNECT);

    if(pMag->m_sHwOps.write != NULL)
    {
        pMag->m_sHwOps.write(ATBuf, ATLen);
    }

}

/****************************************************************
@FUNCTION���칤�������ģ��ɨ���ܱ������豸
@INPUT��BLE�ײ����ṹ��ָ��,����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��
****************************************************************/
void Ble_Scan(BLE_SKYLAB_LOWLEVEL_MAG *pMag, uint8_t *a_sRes1, uint16_t a_wRes2)
{
    uint8_t ATBuf[50] = BLE_ATCMD_SCAN;
    uint16_t ATLen = strlen(BLE_ATCMD_SCAN);

    if(pMag->m_sHwOps.write != NULL)
    {
        pMag->m_sHwOps.write(ATBuf, ATLen);
    }

}

/****************************************************************
@FUNCTION���칤�������ģ��MAC��ַת��(�ַ�����ʽ->����)
@INPUT��MAC��ַ�ַ�����ʽ�����ȣ�ת�������ִ��λ�úʹ�С
@OUTPUT��ת����������ʽ��С
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ����"12:34:56:78:9A:BC"-> 0x12 0x34 0x56 0x78 0x9A 0xBC
****************************************************************/
void Ble_MacCharToHex(uint8_t *a_strMac, uint16_t a_wMacLen, uint8_t *a_aMacHex, uint16_t *a_wMacHexLen)
{
    uint8_t i;
    uint8_t strTemp[2] = {0};

    assert_param(a_strMac != NULL);
    assert_param(a_aMacHex != NULL);

    if(a_wMacLen >= 17 && *a_wMacHexLen >= 6)
    {
        for(i = 0; i < 6; i++)
        {
            strTemp[0] = a_strMac[3 * i];
            strTemp[1] = a_strMac[3 * i + 1];
            AsciiToHex(strTemp, 2, &a_aMacHex[i]);
        }

        *a_wMacHexLen = i;
    }
}

/****************************************************************
@FUNCTION���칤�������ģ��MAC��ַת��(����->�ַ�����ʽ)
@INPUT��MAC��ַ������ʽ�����ȣ�ת�����ַ������λ�úʹ�С
@OUTPUT��ת�����ַ�����ʽ��С
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ����0x12 0x34 0x56 0x78 0x9A 0xBC -> "12:34:56:78:9A:BC"
****************************************************************/
void Ble_MacHexToChar(uint8_t *a_aMacHex, uint16_t MacHexLen, uint8_t *a_strMac, uint16_t *a_wMacLen)
{
    uint8_t i;

    assert_param(a_strMac != NULL);
    assert_param(a_aMacHex != NULL);

    if(*a_wMacLen >= 17 && MacHexLen >= 6)
    {
        for(i = 0; i < 6; i++)
        {
            HexToAscii(&a_aMacHex[i], 1, &a_strMac[i * 3]);

            if(i < 5)
            {
                a_strMac[i * 3 + 2] = ':';
            }
        }

        *a_wMacLen = 17;
    }
}

/****************************************************************
@FUNCTION���칤�������ģ��ײ㶨ʱ���Ƚ���
@INPUT��BLE�ײ����ṹ��ָ��
@OUTPUT����
@RETURN: TRUE-����ִ�еײ㶨ʱ���ȣ� FALSE-�޵ײ㶨ʱ���ȣ���ִ���ϲ�Ӧ�ö�ʱ����
@AUTHOR��xfw
@SPECIAL: ʵ��ģ�鸴λ
****************************************************************/
extern uint32_t Ble_LowLevelTimedProcess(BLE_SKYLAB_LOWLEVEL_MAG *pMag)
{
    assert_param(pMag != NULL);

    if(pMag->m_byIsReset == TRUE)
    {
        if(pMag->m_byResetNowCnt == pMag->m_byResetSetCnt)
        {
            pMag->m_sHwOps.gpio_set();
            pMag->m_byResetNowCnt++;
        }
        else if(pMag->m_byResetNowCnt < (pMag->m_byResetSetCnt + 1))
        {
            pMag->m_byResetNowCnt++;	//��ߵ�+1 ������ģ��һ����ʼ��ʱ��,����λһ��ɾͷ�ָ��᲻��
        }
        else
        {
            pMag->m_byIsReset = FALSE;
        }

        return TRUE;
    }

    return FALSE;
}

/****************************************************************
@FUNCTION���칤�������ģ��ײ���մ���
@INPUT��BLE�ײ����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ȡ������������Ԥ�������޷�ʶ���򻺴�
***************************************************************
extern uint32_t Ble_LowLevelRecvProcess(BLE_SKYLAB_LOWLEVEL_MAG *pMag)
{
	assert_param(pMag!=NULL);

	uint8_t Buf[100]={0};
	uint16_t i;
	uint16_t len = sizeof(Buf);
	u32 IsCutted;

	if(TRUE == pMag->m_sHwOps.read(&Buf,&len))
	{
		for(i=0;i<len;i++)
		{

		}
		if((pMag->m_byIsSetDevName == TRUE)
			&&())
		if(TRUE == Ble_FindATFrame(Buf,&len,&IsCutted)
			&& IsCutted == FALSE)
		{

		}
	}


	return FALSE;
}
*/


//���Դ��룬��������PB5
#if 0
void Ble_GpioPowerOn(void)
{
    GPIO_WriteBit(GPIOB, GPIO_Pins_5, Bit_SET);
}

void Ble_GpioPowerOff(void)
{
    GPIO_WriteBit(GPIOB, GPIO_Pins_5, Bit_RESET);
}

void Ble_GpioReset(void)
{
    GPIO_WriteBit(GPIOA, GPIO_Pins_4, Bit_RESET);
}

void Ble_GpioSet(void)
{
    GPIO_WriteBit(GPIOA, GPIO_Pins_4, Bit_SET);
}

uint32_t Ble_Write(uint8_t *pBuf, uint16_t Len)
{
    #ifdef BLE_LOG_OUTPUT
    LOG("BLE(%d)->", Len);
    printf_with_data(pBuf, Len);
    //printf_with_data(pUartMag->m_byRecvBuf,pUartMag->m_wRecvLen);
    #endif

    return Uart_SendBuf(&g_sUartSetMag.Com[COM_BLE], pBuf, Len);
}

uint32_t Ble_Read(uint8_t *pBuf, uint16_t *pLen)
{
    UART_MANAGER *pUartMag = &g_sUartSetMag.Com[COM_BLE];

    /**/
    if(TRUE == pUartMag->m_bIsComRecv)
    {
        if(pUartMag->m_wRecvLen == 0)
        {
            //pUartMag->m_bIsComRecv = FALSE;
            return FALSE;
        }

        #ifdef BLE_LOG_OUTPUT
        LOG("BLE(%d)<-", pUartMag->m_wRecvLen);
        printf_with_data(pUartMag->m_byRecvBuf, pUartMag->m_wRecvLen);
        #endif

        if(*pLen >= pUartMag->m_wRecvLen)
        {
            memcpy((void *)pBuf, (void *)pUartMag->m_byRecvBuf, pUartMag->m_wRecvLen);
            *pLen = pUartMag->m_wRecvLen;
        }
        else
        {
            memcpy((void *)pBuf, (void *)pUartMag->m_byRecvBuf, *pLen);
        }

        pUartMag->m_wRecvLen = 0;
        pUartMag->m_bIsComRecv = FALSE;

        return TRUE
    }

    return FALSE;
}



#endif

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
