/**
  ******************************************************************************
  * @file    fume.c
  * @author  xfw
  * @brief   This file contains definitions for fume
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
#include "fume.h"

/****************************************************************
@FUNCTION��������ģʽ�·��̹��Ƿ�װ���
@INPUT��FUME����ṹ��
@OUTPUT����
@RETURN: TRUE-�Ѱ�װ FALSE-δ��װ
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint32_t Fume_IsInstalled(FUME_MAG Mag)
{
    if(Mag.m_sHwOps.is_installed != NULL)
    {
        return Mag.m_sHwOps.is_installed();
    }

    return 1;
}


/****************************************************************
@FUNCTION��������ģʽ�·��̹޿��ƹ�����ͣ
@INPUT��FUME����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:��ͣ�ָ��󣬷��̹޼����涨��ʣ��ʱ��
****************************************************************/
void Fume_Suspend(FUME_MAG *pMag)
{
    pMag->m_byIsStart = FALSE;
}

/****************************************************************
@FUNCTION��������ģʽ�·��̹޿��ƹ����Ƿ���ͣ
@INPUT��FUME����ṹ��
@OUTPUT����
@RETURN: TRUE-��ͣ FALSE-δ��ͣ
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
u32 Fume_IsSuspend(FUME_MAG Mag)
{
    return (Mag.m_byIsStart == FALSE ? TRUE : FALSE);
}

/****************************************************************
@FUNCTION��������ģʽ�·��̹޿���������ʱʱ��
@INPUT��FUME����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Fume_SetDelayTime(uint16_t a_wCnt, FUME_MAG *pMag)
{
    pMag->m_wSetCnt = a_wCnt;
}

/****************************************************************
@FUNCTION��������ģʽ�·��̹޿���������ʱʱ��
@INPUT��FUME����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Fume_AddDelayTime(uint16_t a_wCnt, FUME_MAG *pMag)
{
    pMag->m_wSetCnt += a_wCnt;
}

/****************************************************************
@FUNCTION��������ģʽ�·��̹޿��ƹ��ָܻ�
@INPUT��FUME����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Fume_Recovery(FUME_MAG *pMag)
{
    pMag->m_byIsStart = TRUE;
}

/****************************************************************
@FUNCTION��ʵ�ַ�����ģʽ�·��̹޿��ƹ���
@INPUT��FUME����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ÿ�̶�ʱ��(Ŀǰ100Ms)����һ��

****************************************************************/
extern void Fume_TimedProcess(FUME_MAG *pMag)
{
    assert_param(pMag != NULL);

    if(TRUE == Fume_IsSuspend(*pMag))
        return;

    if(pMag->m_wNowCnt <= pMag->m_wSetCnt)
    {
        pMag->m_sHwOps.on();
        pMag->m_wNowCnt++;
    }
    else
    {
        Fume_Suspend(pMag);
        pMag->m_sHwOps.off();
    }
}


/****************************************************************
@FUNCTION������ģʽ�·��̹޿��ƹ��ܳ�ʼ��
@INPUT��Ӳ���ӿڳ�ʼ���������ӿڴ򿪺������ӿڹرպ�����FUME����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ���ݷ��̹�Ӳ��������غ�������ʼ��Ӳ���͹رշ��̹�
****************************************************************/
extern void Fume_LL_Init(FUME_HW_OPERATIONS a_sHwOps, FUME_MAG *pMag)
{
    assert_param(pMag != NULL);

    memset(pMag, 0, sizeof(FUME_MAG));

    pMag->m_sHwOps.hw_init = a_sHwOps.hw_init;
    pMag->m_sHwOps.on = a_sHwOps.on;
    pMag->m_sHwOps.off = a_sHwOps.off;
    pMag->m_sHwOps.is_installed = a_sHwOps.is_installed;

    Fume_Suspend(pMag);

    if(pMag->m_sHwOps.hw_init != NULL)
    {
        pMag->m_sHwOps.hw_init();
    }

    if(pMag->m_sHwOps.off != NULL)
    {
        pMag->m_sHwOps.off();
    }
}


/****************************************************************
@FUNCTION�����̹޷���
@INPUT��FUME����ṹ��ָ�룬���ӳ�
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:������ģʽ�£�ʵ�ʵ������ӳ�(0-��ʱ���ȼ����),ָ����ʱ�������رյ�����
****************************************************************/
extern void Fume_On(FUME_MAG *pMag, uint16_t SetTime)
{
    if(Fume_IsInstalled(*pMag))
    {
        if(TRUE == Fume_IsSuspend(*pMag))
        {
            Fume_SetDelayTime(SetTime, pMag);
            pMag->m_wNowCnt = 0;
        }
        else
        {
            Fume_AddDelayTime(SetTime, pMag);
        }

        Fume_Recovery(pMag);
    }
}

/****************************************************************
@FUNCTION�����̹޷��̹ر�
@INPUT��FUME����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern void Fume_Off(FUME_MAG *pMag)
{
    pMag->m_sHwOps.off();


    Fume_Suspend(pMag);

}
/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
