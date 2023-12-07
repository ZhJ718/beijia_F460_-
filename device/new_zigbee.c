/**
  ******************************************************************************
  * @file    new_zigbee.c
  * @author  xfw
  * @brief   This file contains definitions for XY's new zigbee module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2023 Beijing HuaRu Tech Co.</center></h2>
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
#include "new_zigbee.h"
#include "zigcom_task.h"

static NEW_ZIGBEE_LOWLEVEL_MAG NewZigLLMag;


NEW_ZIGBEE_LOWLEVEL_MAG *NewZigbee_GetLLMagHandle(void)
{
    return &NewZigLLMag;
}
/****************************************************************
@FUNCTION����һ��zigbeeģ��ײ�Ӳ����ʼ��
@INPUT��Ӳ���ӿں����ṹ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �󶨵ײ�Ӳ��������������ʼ�����ϵ磬��ס��λ����
		�����ͺ�ΪXY-MZA66A  XY-MZA66C
****************************************************************/
extern void NewZigbee_LowLevelInit(COM_HW_OPERATIONS a_sHwOps)

{
    NEW_ZIGBEE_LOWLEVEL_MAG *pMag = NewZigbee_GetLLMagHandle();

    assert_param(pMag != NULL);

    memset(pMag, 0, sizeof(NEW_ZIGBEE_LOWLEVEL_MAG));

    pMag->m_sHwOps.hw_init = a_sHwOps.hw_init;
    pMag->m_sHwOps.gpio_power_on = a_sHwOps.gpio_power_on;
    pMag->m_sHwOps.gpio_power_off = a_sHwOps.gpio_power_off;
    pMag->m_sHwOps.gpio_reset = a_sHwOps.gpio_reset;
    pMag->m_sHwOps.gpio_set = a_sHwOps.gpio_set;

    pMag->m_sHwOps.read = a_sHwOps.read;
    pMag->m_sHwOps.write = a_sHwOps.write;

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

    //IO��ʼ��
    if(pMag->m_sHwOps.hw_init != NULL)
    {
        pMag->m_sHwOps.hw_init();
    }

}

//ģ��ײ��������
void NewZigbee_LL_PowerOn(NEW_ZIGBEE_LOWLEVEL_MAG *pMag)
{
    assert_param(pMag != NULL);

    if(pMag->m_sHwOps.gpio_power_on != NULL)
    {
        pMag->m_sHwOps.gpio_power_on();
    }
}

//ģ��ײ�����ϵ�
void NewZigbee_LL_PowerOff(NEW_ZIGBEE_LOWLEVEL_MAG *pMag)
{
    assert_param(pMag != NULL);

    if(pMag->m_sHwOps.gpio_power_off != NULL)
    {
        pMag->m_sHwOps.gpio_power_off();
    }
}

//ģ��ײ������λ����
void NewZigbee_LL_Reset(NEW_ZIGBEE_LOWLEVEL_MAG *pMag)
{
    assert_param(pMag != NULL);

    if(pMag->m_sHwOps.gpio_reset != NULL)
    {
        pMag->m_sHwOps.gpio_reset();
    }
}

//ģ��ײ���λ��λ����
void NewZigbee_LL_Set(NEW_ZIGBEE_LOWLEVEL_MAG *pMag)
{
    assert_param(pMag != NULL);

    if(pMag->m_sHwOps.gpio_set != NULL)
    {
        pMag->m_sHwOps.gpio_set();
    }
}

/****************************************************************
@FUNCTION����һ��zigbeeģ���ȡ�������
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��
****************************************************************/
void NewZigbee_LL_GetParam(NEW_ZIGBEE_LOWLEVEL_MAG *pMag)
{
    uint8_t Buf[] = {0x5A, 0x0A, 0xFC, 0x00, 0x91, 0x0F, 0x45, 0x3C, 0x1D, 0xAA};
    uint16_t Len;

    assert_param(pMag != NULL);

    Len = sizeof(Buf);

    if(pMag->m_sHwOps.write != NULL)
    {
        pMag->m_sHwOps.write(Buf, Len);
    }

}


/****************************************************************
@FUNCTION����һ��zigbeeģ���ȡ����MAC
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��
****************************************************************/
void NewZigbee_LL_SetParam(NEW_ZIGBEE_LOWLEVEL_MAG *pMag)
{
    uint8_t Chk, Buf[50];
    uint16_t i, Len;
    NEW_ZIGBEE_MOD_PARAM ZeroParam;

    assert_param(pMag != NULL);

    Len = 0;
    Buf[Len++] = 0x5A;
    Buf[Len++] = 0;
    Buf[Len++] = 0xFC;
    Buf[Len++] = 0x0C;
    Buf[Len++] = 0x91;
    Buf[Len++] = 0x0E;

    //������Ҫ���õĲ���
    memset(&ZeroParam, 0, sizeof(NEW_ZIGBEE_MOD_PARAM));

		//Uart_SendBuf(&g_sUartSetMag.Com[COM2], pMag->m_sSetParam.m_aSelfExtAddr, 8);
		//Uart_SendBuf(&g_sUartSetMag.Com[COM2], ZeroParam.m_aSelfExtAddr, 8);
		
    if(0 != memcmp((void *)pMag->m_sSetParam.m_aSelfExtAddr, (void *)ZeroParam.m_aSelfExtAddr, 8))
    {
        memcpy((void *)pMag->m_sParam.m_aSelfExtAddr, (void *)pMag->m_sSetParam.m_aSelfExtAddr, 8);
    }

    if(0 != memcmp((void *)&pMag->m_sSetParam.m_byRadioChannel, (void *)&ZeroParam.m_byRadioChannel, 1))
    {
        memcpy((void *)&pMag->m_sParam.m_byRadioChannel, (void *)&pMag->m_sSetParam.m_byRadioChannel, 1);
    }

    if(0 != memcmp((void *)&pMag->m_sSetParam.m_byTxPwr, (void *)&ZeroParam.m_byTxPwr, 1))
    {
        memcpy((void *)&pMag->m_sParam.m_byTxPwr, (void *)&pMag->m_sSetParam.m_byTxPwr, 8);
    }

    memset((void *)&pMag->m_sSetParam, 0, sizeof(NEW_ZIGBEE_MOD_PARAM));
    memcpy(&Buf[Len], (void *)&pMag->m_sParam, sizeof(NEW_ZIGBEE_MOD_PARAM));
    Len += sizeof(NEW_ZIGBEE_MOD_PARAM);

    for(Chk = 0, i = 2; i < 34; i++)
    {
        Chk += Buf[i];
    }

    Buf[Len++] = Chk;
    Buf[Len++] = 0xAA;

    Buf[1] = Len;

    if(pMag->m_sHwOps.write != NULL)
    {
        pMag->m_sHwOps.write(Buf, Len);
    }

}


/****************************************************************
@FUNCTION����һ��zigbeeģ����λ
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��
****************************************************************/
void NewZigbee_LL_SoftReset(NEW_ZIGBEE_LOWLEVEL_MAG *pMag)
{
    uint8_t Buf[] = {0x5A, 0x0A, 0xFC, 0x00, 0x91, 0x87, 0x6A, 0x35, 0xB3, 0xAA};
    uint16_t Len;

    assert_param(pMag != NULL);

    Len = sizeof(Buf);

    if(pMag->m_sHwOps.write != NULL)
    {
        pMag->m_sHwOps.write(Buf, Len);
    }

}

/****************************************************************
@FUNCTION����һ��zigbeeģ�鸴λ
@INPUT����λ������>0��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��λʱ��=��ʱ������ȼ�� * ��λ����
		�˺�����λ����������Ч������λ����Ϊ0����ģ�鸴λһ����λ�ĵ��ȼ��
****************************************************************/
void NewZigbee_Reset(uint8_t Timer)
{
    NEW_ZIGBEE_LOWLEVEL_MAG *pMag = NewZigbee_GetLLMagHandle();

    assert_param(pMag != NULL);

    pMag->m_byResetSetCnt = Timer;
    pMag->m_byResetNowCnt = 0;
    pMag->m_byIsReset = TRUE;

    NewZigbee_LL_Reset(pMag);
}


/****************************************************************
@FUNCTION����һ��zigbeeģ�鸴λ������ͣ
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �˺���������Ч
****************************************************************/
void NewZigbee_ResetSuspend(void)
{
    NEW_ZIGBEE_LOWLEVEL_MAG *pMag = NewZigbee_GetLLMagHandle();

    assert_param(pMag != NULL);

    pMag->m_byIsReset = FALSE;
    NewZigbee_LL_Set(pMag);
}

/****************************************************************
@FUNCTION����һ��zigbeeģ�鸴λ�����ָ�
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ����ʱ��λ�����Ѿ�����������Ӱ��
****************************************************************/
void NewZigbee_ResetRecovery(void)
{
    NEW_ZIGBEE_LOWLEVEL_MAG *pMag = NewZigbee_GetLLMagHandle();

    assert_param(pMag != NULL);

    if((pMag->m_byIsReset == FALSE) && (pMag->m_byResetSetCnt > pMag->m_byResetNowCnt))
    {
        NewZigbee_Reset(pMag->m_byResetSetCnt - pMag->m_byResetNowCnt);
    }

}


/****************************************************************
@FUNCTION����һ��zigbeeģ�鿪ʼ��ȡ����
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ֻ�ǿ����жϣ���һ���������ȡ����
****************************************************************/
void NewZigbee_StartGetParam(void)
{
    NEW_ZIGBEE_LOWLEVEL_MAG *pMag = NewZigbee_GetLLMagHandle();

    assert_param(pMag != NULL);

    pMag->m_byGetParamNowCnt = 0;
    pMag->m_byIsGetParam = TRUE;

}

/****************************************************************
@FUNCTION����һ��zigbeeģ��ֹͣ��ȡ����
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void NewZigbee_StopGetParam(void)
{
    NEW_ZIGBEE_LOWLEVEL_MAG *pMag = NewZigbee_GetLLMagHandle();

    assert_param(pMag != NULL);

    pMag->m_byGetParamNowCnt = 0;
    pMag->m_byIsGetParam = FALSE;

}


/****************************************************************
@FUNCTION����һ��zigbeeģ�鿪ʼ���ò���
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ����ǰ��Ҫ��ȡģ�鵱ǰ����
****************************************************************/
void NewZigbee_StartSetParam(void)
{
    NEW_ZIGBEE_LOWLEVEL_MAG *pMag = NewZigbee_GetLLMagHandle();

    assert_param(pMag != NULL);

    pMag->m_bySetParamNowCnt = 0;
    pMag->m_byIsSetParam = TRUE;

}

/****************************************************************
@FUNCTION����һ��zigbeeģ��ֹͣ���ò���
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void NewZigbee_StopSetParam(void)
{
    NEW_ZIGBEE_LOWLEVEL_MAG *pMag = NewZigbee_GetLLMagHandle();

    assert_param(pMag != NULL);

    pMag->m_bySetParamNowCnt = 0;
    pMag->m_byIsSetParam = FALSE;

}


/****************************************************************
@FUNCTION����һ��zigbeeģ�鿪ʼ��λ
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void NewZigbee_StartSoftReset(void)
{
    NEW_ZIGBEE_LOWLEVEL_MAG *pMag = NewZigbee_GetLLMagHandle();

    assert_param(pMag != NULL);

    pMag->m_bySoftResetNowCnt = 0;
    pMag->m_byIsSoftReset = TRUE;

}

/****************************************************************
@FUNCTION����һ��zigbeeģ��ֹͣ��λ
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void NewZigbee_StopSoftReset(void)
{
    NEW_ZIGBEE_LOWLEVEL_MAG *pMag = NewZigbee_GetLLMagHandle();

    assert_param(pMag != NULL);

    pMag->m_bySoftResetNowCnt = 0;
    pMag->m_byIsSoftReset = FALSE;

}

/****************************************************************
@FUNCTION����һ��zigbeeģ�鷢��ָ��
@INPUT��Ŀ�ĵص�ַ����������
@OUTPUT����
@RETURN: �����ֽ���
@AUTHOR��xfw
@SPECIAL: ��
****************************************************************/
uint16_t NewZigbee_Send(uint8_t *pDstAddr, uint8_t DstAddrLen, uint8_t *pBuf, uint16_t BufLen)
{
    uint8_t IsBroad = FALSE, Buf[250];
    uint16_t Len;
    NEW_ZIGBEE_LOWLEVEL_MAG *pMag = NewZigbee_GetLLMagHandle();

    assert_param(pMag != NULL);

    if(pDstAddr == NULL || DstAddrLen > 8
            || pBuf == NULL || BufLen > 100)
    {
        return 0;
    }

    if((DstAddrLen == 2) && (*(uint16_t *)pDstAddr == 0xFFFF))
    {
        IsBroad = TRUE;
    }

    Len = 0;
    Buf[Len++] = 0x5A;
    Buf[Len++] = 0;
    Buf[Len++] = 0xFD;
    Buf[Len++] = (IsBroad == TRUE ? 0x01 : 0x00);
    Buf[Len++] = BufLen;

    memcpy(&Buf[Len], pDstAddr, DstAddrLen);
    Len += DstAddrLen;
    memcpy(&Buf[Len], pBuf, BufLen);
    Len += BufLen;

    Buf[Len++] = 0xAA;

    Buf[1] = Len;

    if(pMag->m_sHwOps.write != NULL)
    {
        pMag->m_sHwOps.write(Buf, Len);
    }

    return BufLen;
}

/****************************************************************
@FUNCTION����һ��zigbeeģ���������ָ��
@INPUT��Ŀ�ĵص�ַ�������غɺͳ��ȣ����Ļ���ռ�
@OUTPUT�����Ļ��棬���ĳ���
@RETURN: ���ĳ���
@AUTHOR��xfw
@SPECIAL: ��
****************************************************************/
uint16_t NewZigbee_MakeSendFrame(uint8_t *pDstAddr, uint8_t DstAddrLen, uint8_t *pDat, uint16_t DatLen, uint8_t *pBuf,uint16_t *pBufLen)
{
    uint8_t IsBroad = FALSE;
    uint16_t Len;
    NEW_ZIGBEE_LOWLEVEL_MAG *pMag = NewZigbee_GetLLMagHandle();

    assert_param(pMag != NULL);

    if(pDstAddr == NULL || DstAddrLen > 8
            || pDat == NULL || DatLen > 100
            || pBuf == NULL || pBufLen == NULL )
    {
        return 0;
    }

    if(*pBufLen < DatLen+14)
    {
        return 0;
    }
    
    if((DstAddrLen == 2) && (pDstAddr[0] == 0xFF) && (pDstAddr[1] == 0xFF))
    {
        IsBroad = TRUE;
    }

    Len = 0;
    pBuf[Len++] = 0x5A;
    pBuf[Len++] = 0;
    pBuf[Len++] = 0xFD;
    pBuf[Len++] = (IsBroad == TRUE ? 0x01 : 0x00);
    pBuf[Len++] = DatLen;

    memcpy(&pBuf[Len], pDstAddr, DstAddrLen);
    Len += DstAddrLen;
    memcpy(&pBuf[Len], pDat, DatLen);
    Len += DatLen;

    pBuf[Len++] = 0xAA;

    pBuf[1] = Len;
    
    *pBufLen = Len;
		
		//Uart_SendBuf(&g_sUartSetMag.Com[COM2], pBuf, *pBufLen);   
    return Len;
}


/****************************************************************
@FUNCTION����һ��zigbeeģ����������MAC
@INPUT���µ�ַ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void NewZigbee_SetSelfAddr(uint8_t *pAddr, uint8_t AddrLen)
{
    NEW_ZIGBEE_LOWLEVEL_MAG *pMag = NewZigbee_GetLLMagHandle();

    assert_param(pMag != NULL);

    if(pAddr == NULL || AddrLen != 8)
    {
        return;
    }

    memcpy(pMag->m_sSetParam.m_aSelfExtAddr, pAddr, AddrLen);

    NewZigbee_StartSetParam();
}

/****************************************************************
@FUNCTION����һ��zigbeeģ��������Ƶ�ŵ�
@INPUT���ŵ�ѡ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:Ĭ��2405MHZ,��ģ�������
****************************************************************/
void NewZigbee_SetRadioChannel(ENUM_NEW_ZIGBEE_RADIO_CHAN a_enumChan)
{
    NEW_ZIGBEE_LOWLEVEL_MAG *pMag = NewZigbee_GetLLMagHandle();

    assert_param(pMag != NULL);

    //if(pMag->m_sSetParam.m_byRadioChannel != (uint8_t)a_enumChan)
    {
        pMag->m_sSetParam.m_byRadioChannel = (uint8_t)a_enumChan;

        NewZigbee_StartSetParam();
    }
}

/****************************************************************
@FUNCTION����һ��zigbeeģ�����÷��书��
@INPUT�����书��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ����������,����ǰ��Ҫ�Ѿ���ȡ��ģ�鵱ǰ����
****************************************************************/
void NewZigbee_SetTxPwr(ENUM_NEW_ZIGBEE_TXPWR a_enumTxPwr)
{
    NEW_ZIGBEE_LOWLEVEL_MAG *pMag = NewZigbee_GetLLMagHandle();

    assert_param(pMag != NULL);

    // if(pMag->m_sSetParam.m_byTxPwr != (uint8_t)a_enumTxPwr)
    {
        pMag->m_sSetParam.m_byTxPwr = (uint8_t)a_enumTxPwr;

        NewZigbee_StartSetParam();
    }
}


/****************************************************************
@FUNCTION����һ��zigbeeģ��ײ㶨ʱ���Ƚ���
@INPUT��
@OUTPUT����
@RETURN: TRUE-����ִ�еײ㶨ʱ����,  FALSE-�޵ײ㶨ʱ����, ��ִ���ϲ�Ӧ�ö�ʱ����
@AUTHOR��xfw
@SPECIAL: ʵ��ģ�鸴λ
****************************************************************/
extern uint32_t NewZigbee_LowLevelTimedProcess(void)
{
    NEW_ZIGBEE_LOWLEVEL_MAG *pMag = NewZigbee_GetLLMagHandle();

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
            pMag->m_byResetNowCnt++;	//��ߵ�+1 ��ģ��һ����ʼ��ʱ��,����λһ��ɾͷ�ָ��᲻��
        }
        else
        {
            pMag->m_byIsReset = FALSE;
        }

        return TRUE;
    }
    else  if(pMag->m_byIsGetParam == TRUE)
    {
        if(pMag->m_byGetParamNowCnt++ % 20 == 10)
        {
            NewZigbee_LL_GetParam(pMag);
        }

        return TRUE;
    }
    else if(pMag->m_byIsSetParam == TRUE)
    {
        if(pMag->m_bySetParamNowCnt++ % 20 == 10)
        {
            NewZigbee_LL_SetParam(pMag);
        }

        return TRUE;
    }
    else if(pMag->m_byIsSoftReset == TRUE)
    {
        if(pMag->m_bySoftResetNowCnt++ % 20 == 0)
        {
            NewZigbee_LL_SoftReset(pMag);
        }

        return TRUE;
    }


    return FALSE;
}

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
