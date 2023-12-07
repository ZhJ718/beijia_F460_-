/**
  ******************************************************************************
  * @file    hmi_lt268b.c
  * @author  xfw
  * @brief   This file contains definitions for LT268B human machine interface
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2019 Beijing HuaRu Tech Co.</center></h2>
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
#include "hmi_lt268b.h"

static HMI268B_LOWLEVEL_MAG Hmi268bLLMag;


HMI268B_LOWLEVEL_MAG *Hmi268b_GetLLMagHandle(void)
{
    return &Hmi268bLLMag;
}


/****************************************************************
@FUNCTION��LT268B �ײ��ʼ��
@INPUT��Ӳ���ӿں����ṹ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �󶨵ײ�Ӳ��������������ʼ��
****************************************************************/
extern void Hmi268b_LowLevelInit(COM_HW_OPERATIONS a_sHwOps)
{
    HMI268B_LOWLEVEL_MAG *pMag = Hmi268b_GetLLMagHandle();

    assert_param(pMag != NULL);

    memset(pMag, 0, sizeof(HMI268B_LOWLEVEL_MAG));

    pMag->m_sHwOps.hw_init = a_sHwOps.hw_init;
    pMag->m_sHwOps.gpio_power_on = a_sHwOps.gpio_power_on;
    pMag->m_sHwOps.gpio_power_off = a_sHwOps.gpio_power_off;

    pMag->m_sHwOps.read = a_sHwOps.read;
    pMag->m_sHwOps.write = a_sHwOps.write;

    //IO��ʼ��
    if(pMag->m_sHwOps.hw_init != NULL)
    {
        pMag->m_sHwOps.hw_init();
    }

}




/****************************************************************
@FUNCTION��LT268B  �ײ㶨ʱ���Ƚ���
@INPUT��
@OUTPUT����
@RETURN: TRUE-����ִ�еײ㶨ʱ����,  FALSE-�޵ײ㶨ʱ����, ��ִ���ϲ�Ӧ�ö�ʱ����
@AUTHOR��xfw
@SPECIAL: ʵ��ģ���ϵ�
****************************************************************/
extern uint32_t Hmi268b_LowLevelTimedProcess(void)
{
    HMI268B_LOWLEVEL_MAG *pMag = Hmi268b_GetLLMagHandle();

    assert_param(pMag != NULL);



    return FALSE;
}



#if 0


//���͸�HMI
void SendToHmi(uint8_t Cmd, uint8_t Widget, uint8_t *Buf, uint8_t BufLen/*�ı��ؼ�Ϊ�����ȣ�ͼƬ�ؼ�д0����ť�ؼ�д0���������ؼ�д1,*/, HMI_MAG *pMag)
{
    /*****@���ı��ؼ���
        AA C0 01 31 33 B4 E4 1B 11 EE
        AA	֡ͷ
        C0	ָ���ʾ�ֿ�1����
        01	�ı��ؼ����
        31	��ʾ�ַ� ��1��
        33 B4  CRCУ�飬C0 01 31��CRCΪ33 B4
        E4 1B 11 EE  ֡β
    *****/

    /*****@��ͼƬ�ؼ���
        ��������AA 80 01 0B B9 E4 1B 11 EE ��ʾ1��ͼƬ�ؼ�
        AA 80 04 5B 1C E4 1B 11 EE  ��ʾ4��ͼƬ�ؼ�
    *****/

    /*****@���������ؼ���
        �������� AA B0 00 00 3E 26 4C E4 1B 11 EE
    *****/
    unsigned int chkSum = 0;

    uint8_t SendBuf[HMIBUFLEN] = {0xAA, Cmd, Widget, 0x00};

    if(Cmd == HMI_PICCMD && BufLen == 0) //ͼƬ�ؼ�
    {
        BufLen += 2;
        chkSum = Rx_CRC_CCITT(&SendBuf[1], BufLen);
        BufLen = 3;

        SendBuf[BufLen++] = (chkSum >> 8) & 0xFF;
        SendBuf[BufLen++] = chkSum & 0xFF;
        SendBuf[BufLen++] = 0xE4;
        SendBuf[BufLen++] = 0x1B;
        SendBuf[BufLen++] = 0x11;
        SendBuf[BufLen++] = 0xEE;
        pMag->m_sHwOps.write((uint8_t*)SendBuf, BufLen);
    }
    else if((Cmd == HMI_BUTTONCMD_SHOW || Cmd == HMI_BUTTONCMD_HIDE) && BufLen == 0)
    {
        BufLen += 2;
        chkSum = Rx_CRC_CCITT(&SendBuf[1], BufLen);
        BufLen = 3;

        SendBuf[BufLen++] = (chkSum >> 8) & 0xFF;
        SendBuf[BufLen++] = chkSum & 0xFF;
        SendBuf[BufLen++] = 0xE4;
        SendBuf[BufLen++] = 0x1B;
        SendBuf[BufLen++] = 0x11;
        SendBuf[BufLen++] = 0xEE;
        pMag->m_sHwOps.write((uint8_t*)SendBuf, BufLen);
    }
    else if(Cmd == HMI_BARCMD && BufLen == 1) //�������ؼ�
    {
        BufLen += 3;
        SendBuf[BufLen++] = Buf[0];     //��������Χ00 00 ~~ 00 3E��
        chkSum = Rx_CRC_CCITT(&SendBuf[1], 4);

        SendBuf[BufLen++] = (chkSum >> 8) & 0xFF;
        SendBuf[BufLen++] = chkSum & 0xFF;
        SendBuf[BufLen++] = 0xE4;
        SendBuf[BufLen++] = 0x1B;
        SendBuf[BufLen++] = 0x11;
        SendBuf[BufLen++] = 0xEE;
        pMag->m_sHwOps.write((uint8_t*)SendBuf, BufLen);
    }
    else
    {
        memcpy(&SendBuf[3], Buf, strlen((const char*)Buf));

        BufLen = BufLen + 3;
        chkSum = Rx_CRC_CCITT(&SendBuf[1], BufLen - 1);

        SendBuf[BufLen++] = (chkSum >> 8) & 0xFF;
        SendBuf[BufLen++] = chkSum & 0xFF;
        SendBuf[BufLen++] = 0xE4;
        SendBuf[BufLen++] = 0x1B;
        SendBuf[BufLen++] = 0x11;
        SendBuf[BufLen++] = 0xEE;

        pMag->m_sHwOps.write((uint8_t*)SendBuf, BufLen);
    }

    if(0x39 == Widget)
    {
        pMag->m_GreenSolderReplyFlg = TRUE;
    }

}

#endif




/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
