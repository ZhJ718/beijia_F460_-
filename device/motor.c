/**
  ******************************************************************************
  * @file    motor.c
  * @author  xfw
  * @brief   This file contains definitions for motor
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
#include "motor.h"



/****************************************************************
@FUNCTION��������ģʽ�������ƹ�����ͣ
@INPUT��MOTOR����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:��ͣ�ָ����������񶯹涨��ʣ��ʱ��
****************************************************************/
void Motor_Suspend(MOTOR_MAG *pMag)
{
    pMag->m_byIsStartMotor = FALSE;
}

/****************************************************************
@FUNCTION��������ģʽ�������ƹ����Ƿ���ͣ
@INPUT��MOTOR����ṹ��
@OUTPUT����
@RETURN: TRUE-��ͣ FALSE-δ��ͣ
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
u32 Motor_IsSuspend(MOTOR_MAG Mag)
{
    return (Mag.m_byIsStartMotor == FALSE ? TRUE : FALSE);
}

/****************************************************************
@FUNCTION��������ģʽ��������������ʱʱ��
@INPUT��MOTOR����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Motor_SetDelayTime(uint16_t a_wCnt, MOTOR_MAG *pMag)
{
    pMag->m_wSetCnt = a_wCnt;
}

/****************************************************************
@FUNCTION��������ģʽ��������������ʱʱ��
@INPUT��MOTOR����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Motor_AddDelayTime(uint16_t a_wCnt, MOTOR_MAG *pMag)
{
    pMag->m_wSetCnt += a_wCnt;
}

/****************************************************************
@FUNCTION��������ģʽ�������ƹ��ָܻ�
@INPUT��MOTOR����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Motor_Recovery(MOTOR_MAG *pMag)
{
    pMag->m_byIsStartMotor = TRUE;
}

/****************************************************************
@FUNCTION��ʵ�ַ�����ģʽ�������ƹ���
@INPUT��MOTOR����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ÿ�̶�ʱ��(Ŀǰ100Ms)����һ��

****************************************************************/
extern void Motor_TimedProcess(MOTOR_MAG *pMag)
{
    DDL_ASSERT(pMag != NULL);

    if(TRUE == Motor_IsSuspend(*pMag))
        return;

    if(pMag->m_wNowCnt <= pMag->m_wSetCnt)
    {
        pMag->m_sHwOps.motor_on();
        pMag->m_wNowCnt++;
    }
    else
    {
        Motor_Suspend(pMag);
        pMag->m_sHwOps.motor_off();
    }
}

/****************************************************************
@FUNCTION��������ṹ���ʼ��
@INPUT��MOTOR����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ���Ӳ����������ǰ����
			@@ Motor_GpioInit
			@@ Motor_GpioOn
			@@ Motor_GpioOff
****************************************************************/
void Motor_StructInit(PTRFUNC a_pGpioInitFunc, PTRFUNC a_pGpioOnFunc, PTRFUNC a_pGpioOffFunc, MOTOR_MAG *pMag)
{
    DDL_ASSERT(pMag != NULL);

    memset(pMag, 0, sizeof(MOTOR_MAG));

    pMag->m_sHwOps.motor_hw_init = a_pGpioInitFunc;
    pMag->m_sHwOps.motor_on = a_pGpioOnFunc;
    pMag->m_sHwOps.motor_off = a_pGpioOffFunc;

}


/****************************************************************
@FUNCTION������ģʽ�������ƹ��ܳ�ʼ��
@INPUT��Ӳ���ӿڳ�ʼ���������ӿڴ򿪺������ӿڹرպ�����MOTOR����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �������Ӳ��������غ�������ʼ��Ӳ���͹ر�����ͣ��﹦��
****************************************************************/
extern void Motor_LL_Init(MOTOR_HW_OPERATIONS a_sHwOps, MOTOR_MAG *pMag)
{
    DDL_ASSERT(pMag != NULL);

    memset(pMag, 0, sizeof(MOTOR_MAG));

    pMag->m_sHwOps.motor_hw_init = a_sHwOps.motor_hw_init;
    pMag->m_sHwOps.motor_on = a_sHwOps.motor_on;
    pMag->m_sHwOps.motor_off = a_sHwOps.motor_off;

    Motor_Suspend(pMag);

    if(pMag->m_sHwOps.motor_hw_init != NULL)
    {
        pMag->m_sHwOps.motor_hw_init();
    }

    if(pMag->m_sHwOps.motor_off != NULL)
    {
        pMag->m_sHwOps.motor_off();
    }
}


/****************************************************************
@FUNCTION�������
@INPUT��MOTOR����ṹ��ָ�룬����ģʽ�����ӳ�
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:������ģʽ�£�ʵ����������ӳ�(0-��ʱ���ȼ����),ָ����ʱ�������ر����
****************************************************************/
extern void Motor_On(MOTOR_MAG *pMag, OPT_MODE OptMode, uint16_t SetTime)
{
    if(OptMode == OPT_MODE_BLOCK)
    {
        pMag->m_sHwOps.motor_on();
    }
    else
    {
        if(TRUE == Motor_IsSuspend(*pMag))
        {
            Motor_SetDelayTime(SetTime, pMag);
            pMag->m_wNowCnt = 0;
        }
        else
        {
            Motor_AddDelayTime(SetTime, pMag);
        }

        Motor_Recovery(pMag);
    }
}

/****************************************************************
@FUNCTION�����ر�
@INPUT��MOTOR����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern void Motor_Off(MOTOR_MAG *pMag, OPT_MODE OptMode)
{
    pMag->m_sHwOps.motor_off();

    if(OptMode != OPT_MODE_BLOCK)
    {
        Motor_Suspend(pMag);
    }

}


/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
