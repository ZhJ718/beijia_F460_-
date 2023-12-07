/**
  ******************************************************************************
  * @file    timer.h
  * @author  xfw
  * @brief   This file contains definitions for device timer
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIMER_H
#define __TIMER_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/

#include "config.h"



typedef struct
{
    uint8_t	IsSysTimeUpdate_250US: 1;		// 250us��ʱ�����,��Ҫ�����0
    uint8_t	IsSysTimeUpdate_1MS: 1;		// 1ms��ʱ�����,��Ҫ�����0
    uint8_t	IsSysTimeUpdate_10MS: 1;		//10ms��ʱ�����,��Ҫ�����0
    uint8_t	IsSysTimeUpdate_100MS: 1;	//100ms��ʱ�����,��Ҫ�����0
    uint8_t	IsSysTimeUpdate_1S: 1;		// 1s��ʱ�����,��Ҫ�����0
    uint8_t	IsSysTimeUpdate_10S: 1;		// 10s��ʱ�����,��Ҫ�����0
    uint8_t	IsSysTimeUpdate_1Min: 1;	// 1min��ʱ�����,��Ҫ�����0
    uint8_t	IsSysTimeUpdate_1Hour: 1;	// hour��ʱ�����,��Ҫ�����0

    uint8_t isRes: 8;
    uint16_t TimSysTimeCount250US;
    uint16_t TimSysTimeCountMS;			//��ǰϵͳ����ʱ��,��λ:ms
    uint32_t TimSysTimeCountS;			//��ǰϵͳ����ʱ��,��λ:s
    uint32_t TimSysTimeCountTick;			//��ǰϵͳ����ʱ�ӵ��ȴ���
} TIMER_SCHEDULE_CLOCK;


extern TIMER_SCHEDULE_CLOCK g_sSysScheTime;


/****************************************************************
@FUNCTION��ϵͳ����ʱ�Ӷ�ʱ����ʼ��
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��xfw
@SPECIAL:ʵ��250us��ʱ�ж�
****************************************************************/
void SysScheduleTimerInit(void);

/****************************************************************
@FUNCTION����ȡϵͳ��������������������
@INPUT����
@OUTPUT����
@RETURN: ����
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern uint32_t GetSysRunSecond(void);

/****************************************************************
@FUNCTION����ȡϵͳ���������������ĺ�����
@INPUT����
@OUTPUT����
@RETURN: ����
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern uint32_t GetSysRunMilliSecond(void);

/****************************************************************
@FUNCTION���������������
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:���ݵ�ǰϵͳʱ������
****************************************************************/
extern void SetRand(void);

//��ʼ���ӳٺ���
//��ʹ��OS��ʱ��,�˺������ʼ��OS��ʱ�ӽ���
//SYSTICK��ʱ�ӹ̶�ΪHCLKʱ�ӵ�1/8
//SYSCLK:ϵͳʱ��
void Delay_Init(void);


//��ʱnus
//nusΪҪ��ʱ��us��.
void Delay_Us(uint32_t nus);


//��ʱnms
//ע��nms�ķ�Χ
//SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK��λΪHz,nms��λΪms
//��72M������,nms<=1864
void Delay_Ms(uint32_t nms);

#if defined( AT32F4xx)

/******************************************************************
*��������:TIM6_Int_Init
*��������:��ʱ��6�жϳ�ʼ��
*�������:arr���Զ���װֵ��psc��ʱ��Ԥ��Ƶ��
*�������:
*����ֵ:
*����˵��: �������ģʽ
*�޸�����    �汾��   �޸���    �޸�����
*---------------------------------------------------
*                              ���
******************************************************************/
void TIM6_Int_Init(u16 arr, u16 psc);

/****************************************************************
@FUNCTION��TIM6΢�뼶�ж�
@INPUT���ж�us��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �ж�ʱ�������0����Ϊ0�����ж�1us
****************************************************************/
void TIM6_Sleep_Us(uint16_t nus);



/******************************************************************
*��������:TIM7_Int_Init
*��������:��ʱ��7�жϳ�ʼ��
*�������:arr���Զ���װֵ��psc��ʱ��Ԥ��Ƶ��
*�������:
*����ֵ:
*����˵��:
*�޸�����    �汾��   �޸���    �޸�����
*---------------------------------------------------
*                              ���
******************************************************************/
void TIM7_Int_Init(u16 arr, u16 psc);

/****************************************************************
@FUNCTION��TIM7΢�뼶�ж�
@INPUT���ж�us��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �ж�ʱ�������0����Ϊ0�����ж�1us
****************************************************************/
void TIM7_Sleep_Us(uint16_t nus);

/******************************************************************
*��������:TIM1_Cap_Init
*��������:�߼���ʱ��1���벶���ܳ�ʼ��
*�������:arr���Զ���װֵ��psc��ʱ��Ԥ��Ƶ��
*�������:
*����ֵ:
*����˵��:
*�޸�����    �汾��   �޸���    �޸�����
*---------------------------------------------------
*                              ���
******************************************************************/
void TIM1_Cap_Init(u16 arr, u16 psc);

/******************************************************************
*��������:TIM4_Cap_Init
*��������:ͨ�ö�ʱ��4 ���벶���ܳ�ʼ��
*�������:arr���Զ���װֵ��psc��ʱ��Ԥ��Ƶ��
*�������:
*����ֵ:
*����˵��:
*�޸�����    �汾��   �޸���    �޸���
******************************************************************/
void TIM4_Cap_Init(u16 arr, u16 psc);

/******************************************************************
*��������:TIM9_Cap_Init
*��������:ͨ�ö�ʱ��9 ���벶���ܳ�ʼ��
*�������:arr���Զ���װֵ��psc��ʱ��Ԥ��Ƶ��
*�������:
*����ֵ:
*����˵��:
*�޸�����    �汾��   �޸���    �޸�����
*---------------------------------------------------
*                              ���
******************************************************************/
void TIM9_Cap_Init(u16 arr, u16 psc);

/******************************************************************
*��������:TIM10_Cap_Init
*��������:ͨ�ö�ʱ��10 ���벶���ܳ�ʼ��
*�������:arr���Զ���װֵ��psc��ʱ��Ԥ��Ƶ��
*�������:
*����ֵ:
*����˵��:
*�޸�����    �汾��   �޸���    �޸�����
*---------------------------------------------------
*                              ���
******************************************************************/
void TIM10_Cap_Init(u16 arr, u16 psc);

/******************************************************************
*��������:TIM11_Cap_Init
*��������:ͨ�ö�ʱ��11 ���벶���ܳ�ʼ��
*�������:arr���Զ���װֵ��psc��ʱ��Ԥ��Ƶ��
*�������:
*����ֵ:
*����˵��:
*�޸�����    �汾��   �޸���    �޸�����
*---------------------------------------------------
*                              ���
******************************************************************/
void TIM11_Cap_Init(u16 arr, u16 psc);

#elif defined(HC32F46x)


#include "hc32f46x_timer4_cnt.h"

/********************************************************************************
 ** \brief  Initialize the system clock for the sample
 **
 ** \param  None
 **
 ** \return None
 ******************************************************************************/
void SysClkIni(void);






#endif

typedef void (*PTimeOutCallBack)(void *pHandle);

typedef enum
{
    MODE_NORMAL = 0,//0����ģʽ,��Ҫ�Լ��ж϶�ʱ���Ƿ�ʱ,��ʱ�����������
    MODE_AUTO,		// 1��ʱ���Զ���ʱ����ģʽ,��ʱ�ж�ʱ�жϳ�ʱ����ó�ʱ�ص�����
} ENUM_SOFT_TIMER_MODE;

typedef struct
{
    uint32_t TimerEnable;
    uint32_t TimerOut;
    uint32_t TimerCnt;
    // 0����ģʽ,��Ҫ�Լ��ж϶�ʱ���Ƿ�ʱ,��ʱ�����������
    // 1��ʱ���Զ���ʱ����ģʽ,��ʱ�ж�ʱ�жϳ�ʱ����ó�ʱ�ص�����
    ENUM_SOFT_TIMER_MODE TimerMode;
    PTimeOutCallBack pFunc;
    void *pFuncParam;
} SoftTimer;

typedef  enum
{
    TIMER_TEST,                     //ϵͳ���Զ�ʱ��
    TIMER_CAR_SPEED_MONITOR,                     //С���ٶȼ��
    TIMER_NUM						//��ʱ������
} ENUM_TIMER;

#define SOFT_Timer_Disable 0//TIMER_DisableInt(TIMER3)
#define SOFT_Timer_Enable 0//TIMER_EnableInt(TIMER3)
#define SOFT_Timer_Interval_MS 100		//��ʱ����С��ʱ���Ϊ100ms
//extern SoftTimer SoftTimers[TIMER_NUM];

extern void SoftTimerInit(void);
extern void SoftTimerStart(uint32_t TimerID, uint32_t n100Ms, ENUM_SOFT_TIMER_MODE Mode, PTimeOutCallBack pTimeOutCallBack, void *pHandle);
extern void SoftTimerStop(uint32_t TimerID);
extern uint32_t SoftTimerIsOver(uint32_t TimerID);
extern uint32_t SoftTimerIsStart(uint32_t TimerID);
extern void SoftTimerIntervalProcess(void);


#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
