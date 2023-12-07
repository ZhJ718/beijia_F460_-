/**
  ******************************************************************************
  * @file    beep.h
  * @author  xfw
  * @brief   This file contains definitions for BEEP
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BEEP_H
#define __BEEP_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "config.h"



//BEEPӲ������
typedef struct
{
    //BeepӲ���ӿڳ�ʼ������
    void (*beep_hw_init)(void);
    //Beep������
    void (*beep_on)(void);
    //Beep�ز���
    void (*beep_off)(void);
    //Beep��ת����
    void (*beep_toggle)(void);
} BEEP_HW_OPERATIONS;


typedef struct
{
    //ʵ��Beep����첽�����߼���ֻ��
    uint8_t		m_byIsStartBeep: 1;
    uint8_t		m_byIsPassiveBeep: 1;	//��Դ������Դ
    uint8_t		m_byRes: 6;

    uint16_t 	m_wSetCnt;	//��ʱ����ֵ
    uint16_t 	m_wNowCnt;	//�Ѿ���ʱʱ��

    //ʵ��Beep���Ӳ���������ɶ�д
    BEEP_HW_OPERATIONS m_sHwOps;
} BEEP_MAG;


/****************************************************************
@FUNCTION�����������ƹ�����ͣ
@INPUT��BEEP����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Beep_Suspend(BEEP_MAG *pMag);

/****************************************************************
@FUNCTION�����������ƹ����Ƿ���ͣ
@INPUT��BEEP����ṹ��
@OUTPUT����
@RETURN: TRUE-��ͣ FALSE-δ��ͣ
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
u32 Beep_IsSuspend(BEEP_MAG Mag);

/****************************************************************
@FUNCTION�����÷�����������ʱʱ��
@INPUT��BEEP����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Beep_SetDelayTime(uint16_t a_wCnt, BEEP_MAG *pMag);

/****************************************************************
@FUNCTION�����ӷ�����������ʱʱ��
@INPUT��BEEP����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Beep_AddDelayTime(uint16_t a_wCnt, BEEP_MAG *pMag);

/****************************************************************
@FUNCTION�����������ƹ��ָܻ�
@INPUT��BEEP����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Beep_Recovery(BEEP_MAG *pMag);


/****************************************************************
@FUNCTION��������ģʽ��ʵ�ַ��������ƹ���
@INPUT��BEEP����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ÿ�̶�ʱ��(Ŀǰ1Ms)����һ��,Ϊ�˼�����Դ��������1kHz

****************************************************************/
extern void Beep_TimedProcess(BEEP_MAG *pMag) ;


/****************************************************************
@FUNCTION��������ģʽ�·��������ƹ��ܳ�ʼ��
@INPUT��Ӳ���ӿں����ṹ�壬�Ƿ�����Դ������(TRUE-��Դ)��BEEP����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ���ݷ�����Ӳ��������غ�������ʼ��Ӳ���͹رշ���������ͣ����������
****************************************************************/
extern void Beep_LL_Init(BEEP_HW_OPERATIONS a_sHwOps, u32 a_IsPassiveBeep, BEEP_MAG *pMag);


/****************************************************************
@FUNCTION������������
@INPUT��BEEP����ṹ��ָ�룬����ģʽ�����ӳ�
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:��֧������ģʽ����Դ���������в���
****************************************************************/
extern void Beep_On(BEEP_MAG *pMag, OPT_MODE OptMode, uint16_t SetTime);

/****************************************************************
@FUNCTION���������ر�
@INPUT��BEEP����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern void Beep_Off(BEEP_MAG *pMag, OPT_MODE OptMode);


#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
