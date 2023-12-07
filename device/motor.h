/**
  ******************************************************************************
  * @file    motor.h
  * @author  xfw
  * @brief   This file contains definitions for MOTOR
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
#ifndef __MOTOR_H
#define __MOTOR_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "config.h"

//MOTORӲ������
typedef struct
{
    //MotorӲ���ӿڳ�ʼ������
    void (*motor_hw_init)(void);
    //Motor������
    void (*motor_on)(void);
    //Motor�ز���
    void (*motor_off)(void);
} MOTOR_HW_OPERATIONS;

typedef struct
{
    //ʵ��Motor����첽�����߼���ֻ��
    uint8_t		m_byIsStartMotor: 1;
    uint8_t		m_byRes: 7;
    uint16_t 	m_wSetCnt;	//��ʱ����ֵ
    uint16_t 	m_wNowCnt;	//�Ѿ���ʱʱ��
    //ʵ��Motor���Ӳ���������ɶ�д
    MOTOR_HW_OPERATIONS m_sHwOps;
} MOTOR_MAG;


/****************************************************************
@FUNCTION��������ģʽ�������ƹ�����ͣ
@INPUT��MOTOR����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:��ͣ�ָ����������񶯹涨��ʣ��ʱ��
****************************************************************/
void Motor_Suspend(MOTOR_MAG *pMag);

/****************************************************************
@FUNCTION��������ģʽ�������ƹ����Ƿ���ͣ
@INPUT��MOTOR����ṹ��
@OUTPUT����
@RETURN: TRUE-��ͣ FALSE-δ��ͣ
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
u32 Motor_IsSuspend(MOTOR_MAG Mag);

/****************************************************************
@FUNCTION��������ģʽ�������ƹ��ָܻ�
@INPUT��MOTOR����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Motor_Recovery(MOTOR_MAG *pMag);


/****************************************************************
@FUNCTION��������ģʽ�������ƹ����߼�
@INPUT��MOTOR����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ÿ�̶�ʱ��(Ŀǰ100Ms)����һ��

****************************************************************/
extern void Motor_TimedProcess(MOTOR_MAG *pMag) ;


/****************************************************************
@FUNCTION������ģʽ�������ƹ��ܳ�ʼ��
@INPUT��Ӳ���ӿڳ�ʼ���������ӿڴ򿪺������ӿڹرպ�����MOTOR����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �������Ӳ��������غ�������ʼ��Ӳ���͹ر�����ͣ��﹦��
****************************************************************/
extern void Motor_LL_Init(MOTOR_HW_OPERATIONS a_sHwOps, MOTOR_MAG *pMag);

/****************************************************************
@FUNCTION�������
@INPUT��MOTOR����ṹ��ָ�룬����ģʽ�����ӳ�
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:������ģʽ�£�ʵ����������ӳ�(0-��ʱ���ȼ����)
****************************************************************/
extern void Motor_On(MOTOR_MAG *pMag, OPT_MODE OptMode, uint16_t SetTime);

/****************************************************************
@FUNCTION�����ر�
@INPUT��MOTOR����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern void Motor_Off(MOTOR_MAG *pMag, OPT_MODE OptMode);


#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
