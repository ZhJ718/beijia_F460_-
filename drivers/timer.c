/**
  ******************************************************************************
  * @file    timer.c
  * @author  xfw
  * @brief   This file contains definitions for STM32091C timer
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
#include "timer.h"

TIMER_SCHEDULE_CLOCK g_sSysScheTime;

/****************************************************************
@FUNCTION��ϵͳ����ʱ�Ӷ�ʱ�жϷ������
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��xfw
@SPECIAL:ÿ1ms��ʱ����
****************************************************************/
extern void SysScheduleProcess(void)
{
    g_sSysScheTime.TimSysTimeCount250US++;
    g_sSysScheTime.IsSysTimeUpdate_250US = TRUE;
    //gs_sSysTime.TimSysTimeCountMS++;
    //gs_sSysTime.IsSysTimeUpdate_1MS = TRUE;
    g_sSysScheTime.TimSysTimeCountTick++;

    if(g_sSysScheTime.TimSysTimeCount250US % 4 == 0)
    {
        g_sSysScheTime.IsSysTimeUpdate_1MS = TRUE;
        g_sSysScheTime.TimSysTimeCountMS++;

        if(g_sSysScheTime.TimSysTimeCountMS % 10 == 0)
            g_sSysScheTime.IsSysTimeUpdate_10MS = TRUE;

        if(g_sSysScheTime.TimSysTimeCountMS % 100 == 0)
            g_sSysScheTime.IsSysTimeUpdate_100MS = TRUE;

        if(g_sSysScheTime.TimSysTimeCountMS % 1000 == 0)
        {
            g_sSysScheTime.TimSysTimeCountMS = 0;
            g_sSysScheTime.TimSysTimeCountS++;
            g_sSysScheTime.IsSysTimeUpdate_1S = TRUE;
            #if 0

            if(g_sSysScheTime.TimSysTimeCountS % 10 == 0)
                g_sSysScheTime.IsSysTimeUpdate_10S = TRUE;

            if(g_sSysScheTime.TimSysTimeCountS % 60 == 0)
                g_sSysScheTime.IsSysTimeUpdate_1Min = TRUE;

            if(g_sSysScheTime.TimSysTimeCountS % 3600 == 0)
                g_sSysScheTime.IsSysTimeUpdate_1Hour = TRUE;

            //if(g_sSysTime.TimSysTimeCountS%86400 == 0)
            //	g_sSysTime.IsSysTimeUpdate_1Day = TRUE;
            #endif
        }

    }
}

extern void SetRand(void)
{
    srand(g_sSysScheTime.TimSysTimeCountMS);//�������������
}

/****************************************************************
@FUNCTION����ȡϵͳ��������������������
@INPUT����
@OUTPUT����
@RETURN: ����
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern uint32_t GetSysRunSecond(void)
{
    return g_sSysScheTime.TimSysTimeCountS;
}

/****************************************************************
@FUNCTION����ȡϵͳ���������������ĺ�����
@INPUT����
@OUTPUT����
@RETURN: ����
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern uint32_t GetSysRunMilliSecond(void)
{
    return g_sSysScheTime.TimSysTimeCountS * 1000 + g_sSysScheTime.TimSysTimeCountMS;
}



#if defined(AT32F4xx)




void TIM4_Int_Init(u16 arr, u16 psc)
{
    TMR_TimerBaseInitType  TIM_TimeBaseStructure;
    NVIC_InitType NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR4, ENABLE); //ʱ��ʹ��

    //��ʱ��TIM3��ʼ��
    TIM_TimeBaseStructure.TMR_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TMR_DIV = psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TMR_ClockDivision = TMR_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;  //TIM���ϼ���ģʽ
    TMR_TimeBaseInit(TMR4, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    TMR_INTConfig(TMR4, TMR_INT_Overflow, ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

    //�ж����ȼ�NVIC����
    NVIC_InitStructure.NVIC_IRQChannel = TMR4_GLOBAL_IRQn;  //TIM3�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�0��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;  //�����ȼ�3��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


    TMR_Cmd(TMR4, ENABLE);  //ʹ��TIMx
}

void TMR4_GLOBAL_IRQHandler(void)   //TIM3�ж�
{

    if (TMR_GetINTStatus(TMR4, TMR_INT_Overflow) != RESET)  //���TIM3�����жϷ������
    {

        SysScheduleProcess();
        TMR_ClearITPendingBit(TMR4, TMR_INT_Overflow); //���TIMx�����жϱ�־
    }
}

/****************************************************************
@FUNCTION��Systick�жϷ������
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��xfw
@SPECIAL:ÿ1ms��ʱ����,�ڲ�����SysScheduleProcess()����
***************************************************************
void SysTick_Handler(void)
{
		SysScheduleProcess();
}*/

/****************************************************************
@FUNCTION��ϵͳ����ʱ�Ӷ�ʱ����ʼ��
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��xfw
@SPECIAL:ʵ��250uss��ʱ�ж�
****************************************************************/
void SysScheduleTimerInit(void)
{
    uint32_t A, B;

    #if 0
    SysTick_Config(SystemCoreClock / 1000U);

    /*Configure the SysTick IRQ priority */
    NVIC_SetPriority(SysTick_IRQn, 1);
    //NVIC_EnableIRQ(SysTick_IRQn);
    #endif
    memset(&g_sSysScheTime, 0, sizeof(TIMER_SCHEDULE_CLOCK));

    A = 10;
    B = SystemCoreClock / 4000 / A;

    TIM4_Int_Init(A - 1, B - 1); // 40Khz�ļ���Ƶ�� ������10Ϊ250us ����һ���ж�
}



static u8  fac_us = 0;							//us��ʱ������
static u16 fac_ms = 0;							//ms��ʱ������,��ucos��,����ÿ�����ĵ�ms��


//��ʼ���ӳٺ���
//��ʹ��OS��ʱ��,�˺������ʼ��OS��ʱ�ӽ���
//SYSTICK��ʱ�ӹ̶�ΪHCLKʱ�ӵ�1/8
//SYSCLK:ϵͳʱ��
void Delay_Init(void)
{
    #if SYSTEM_SUPPORT_OS  							//�����Ҫ֧��OS.
    u32 reload;
    #endif
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//ѡ���ⲿʱ��  HCLK/8
    fac_us = SystemCoreClock / 8000000;				//Ϊϵͳʱ�ӵ�1/8
    #if SYSTEM_SUPPORT_OS  							//�����Ҫ֧��OS.
    reload = SystemCoreClock / 8000000;				//ÿ���ӵļ������� ��λΪK
    reload *= 1000000 / delay_ostickspersec;		//����delay_ostickspersec�趨���ʱ��
    //reloadΪ24λ�Ĵ���,���ֵ:16777216,��72M��,Լ��1.86s����
    fac_ms = 1000 / delay_ostickspersec;			//����OS������ʱ�����ٵ�λ

    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;   	//����SYSTICK�ж�
    SysTick->LOAD = reload; 						//ÿ1/delay_ostickspersec���ж�һ��
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   	//����SYSTICK

    #else
    fac_ms = (u16)fac_us * 1000;					//��OS��,����ÿ��ms��Ҫ��systickʱ����
    #endif
}

//��ʱnus
//nusΪҪ��ʱ��us��.
void Delay_Us(u32 nus)
{
    u32 temp;
    SysTick->LOAD = nus * fac_us; 					//ʱ�����
    SysTick->VAL = 0x00;        					//��ռ�����
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;	//��ʼ����

    do
    {
        temp = SysTick->CTRL;
    }
    while((temp & 0x01) && !(temp & (1 << 16)));		//�ȴ�ʱ�䵽��

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	//�رռ�����
    SysTick->VAL = 0X00;      					 //��ռ�����
}

//��ʱnms
//ע��nms�ķ�Χ
//SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK��λΪHz,nms��λΪms
//��72M������,nms<=1864
void Delay_Ms(uint32_t nms)
{
    u32 temp;
    SysTick->LOAD = (u32)nms * fac_ms;				//ʱ�����(SysTick->LOADΪ24bit)
    SysTick->VAL = 0x00;							//��ռ�����
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;	//��ʼ����

    do
    {
        temp = SysTick->CTRL;
    }
    while((temp & 0x01) && !(temp & (1 << 16)));		//�ȴ�ʱ�䵽��

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	//�رռ�����
    SysTick->VAL = 0X00;       					//��ռ�����
}






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
void TIM6_Int_Init(u16 arr, u16 psc)
{
    NVIC_InitType NVIC_InitStructure;
    TMR_TimerBaseInitType  TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR6, ENABLE);//TIM6ʱ��ʹ��

    //��ʱ��TIM7��ʼ��
    TIM_TimeBaseStructure.TMR_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TMR_DIV = psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TMR_ClockDivision = TMR_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;  //TIM���ϼ���ģʽ
    TMR_TimeBaseInit(TMR6, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
    TMR_ClearFlag(TMR6, TMR_FLAG_Update);
    TMR_INTConfig(TMR6, TMR_INT_Overflow, ENABLE ); //ʹ��ָ����TIM6�ж�,��������ж�

    /* ��ֹARRԤװ�ػ����� */
    TMR_ARPreloadConfig(TMR6, DISABLE);

    TMR_Cmd(TMR6, ENABLE); //������ʱ��6

    NVIC_InitStructure.NVIC_IRQChannel = TMR6_GLOBAL_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0 ; //��ռ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

}

/****************************************************************
@FUNCTION��TIM6΢�뼶�ж�
@INPUT���ж�us��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �ж�ʱ�������0����Ϊ0�����ж�1us
****************************************************************/
void TIM6_Sleep_Us(uint16_t nus)
{
    if(nus == 0)
    {
        TIM6_Int_Init(1, 71);
    }
    else
    {
        TIM6_Int_Init(nus - 1, 71);
    }
}


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
void TIM7_Int_Init(u16 arr, u16 psc)
{
    NVIC_InitType NVIC_InitStructure;
    TMR_TimerBaseInitType  TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR7, ENABLE);//TIM7ʱ��ʹ��

    //��ʱ��TIM7��ʼ��
    TIM_TimeBaseStructure.TMR_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TMR_DIV = psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TMR_ClockDivision = TMR_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;  //TIM���ϼ���ģʽ
    TMR_TimeBaseInit(TMR7, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
    TMR_ClearFlag(TMR7, TMR_FLAG_Update);
    TMR_INTConfig(TMR7, TMR_INT_Overflow, ENABLE ); //ʹ��ָ����TIM7�ж�,��������ж�

    /* ��ֹARRԤװ�ػ����� */
    TMR_ARPreloadConfig(TMR7, DISABLE);

    TMR_Cmd(TMR7, ENABLE); //������ʱ��7

    NVIC_InitStructure.NVIC_IRQChannel = TMR7_GLOBAL_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0 ; //��ռ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

}

/****************************************************************
@FUNCTION��TIM7΢�뼶�ж�
@INPUT���ж�us��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �ж�ʱ�������0����Ϊ0�����ж�1us
****************************************************************/
void TIM7_Sleep_Us(uint16_t nus)
{
    if(nus == 0)
    {
        TIM7_Int_Init(1, 71);
    }
    else
    {
        TIM7_Int_Init(nus - 1, 71);
    }
}

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
void TIM1_Cap_Init(u16 arr, u16 psc)
{
    GPIO_InitType GPIO_InitStructure;
    TMR_TimerBaseInitType  TIM_TimeBaseStructure;
    TMR_ICInitType  TIM1_ICInitStructure;
    NVIC_InitType NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR1, ENABLE);	//��1?��TIM2����?��
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOE, ENABLE);  //��1?��GPIOA����?��
    TMR_Reset(TMR1);
    TMR_ClearITPendingBit(TMR1, TMR_INT_Overflow );     //���TIM1�����жϱ�־

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO, ENABLE);

    //��ȫ��ӳ�� 7.4.6 ��ʱ�����ù�����ӳ��
    GPIO_PinsRemapConfig(AFIO_MAP4_TMR1_0011, ENABLE);

    GPIO_InitStructure.GPIO_Pins  = GPIO_Pins_9 | GPIO_Pins_11 | GPIO_Pins_13; //PA0 ??3y???�㨦��??
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU; //PA0 ��?��?
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz; //PA0 ��?��?
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    //3?��??��?������?��2 TMR2
    TIM_TimeBaseStructure.TMR_Period = arr; //����?��??��y?�¡�??��??����?��
    TIM_TimeBaseStructure.TMR_DIV = psc; 	//?�衤??��?��
    TIM_TimeBaseStructure.TMR_ClockDivision = TMR_CKD_DIV1; //����??����?����???:TDTS = Tck_tim
    TIM_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;  //TIM?����???��y?�꨺?
    TMR_TimeBaseInit(TMR1, &TIM_TimeBaseStructure); //?��?YTIM_TimeBaseInitStruct?D???����?2?��y3?��??��TIMx��?����???����y�̣�??

    //3?��??��TIM2��?��?2???2?��y
    TIM1_ICInitStructure.TMR_Channel = TMR_Channel_1; //CC1S=01 	????��?��??? IC1��3��?��?TI1��?
    TIM1_ICInitStructure.TMR_ICPolarity = TMR_ICPolarity_BothEdge;	//��?��y??2???
    TIM1_ICInitStructure.TMR_ICSelection = TMR_ICSelection_DirectTI; //��3��?��?TI1��?
    TIM1_ICInitStructure.TMR_ICDIV = TMR_ICDIV_DIV1;	 //????��?��?��??��,2?��??��
    TIM1_ICInitStructure.TMR_ICFilter = 0x00;//IC1F=0000 ????��?��???2��?�� 2???2��
    TMR_ICInit(TMR1, &TIM1_ICInitStructure);
    TIM1_ICInitStructure.TMR_Channel = TMR_Channel_2;
    TMR_ICInit(TMR1, &TIM1_ICInitStructure);
    TIM1_ICInitStructure.TMR_Channel = TMR_Channel_3;
    TMR_ICInit(TMR1, &TIM1_ICInitStructure);
    //?D??��?����3?��??��
    NVIC_InitStructure.NVIC_IRQChannel = TMR1_CC_IRQn;  //TMR2?D??
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //?��??��??��??2??
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 8;  //�䨮��??��??0??
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ�����̨���?��1?��
    NVIC_Init(&NVIC_InitStructure);  //?��?YNVIC_InitStruct?D???����?2?��y3?��??����a����NVIC??��??��

    TMR_INTConfig(TMR1, TMR_INT_CC1 | TMR_INT_CC2 | TMR_INT_CC3, ENABLE);

    TMR_ClearITPendingBit(TMR1, TMR_INT_CC1 | TMR_INT_CC2 | TMR_INT_CC3);

    TMR_Cmd(TMR1, ENABLE ); 	//��1?��?������?��2
}

/******************************************************************
*��������:TIM4_Cap_Init
*��������:ͨ�ö�ʱ��4 ���벶���ܳ�ʼ��
*�������:arr���Զ���װֵ��psc��ʱ��Ԥ��Ƶ��
*�������:
*����ֵ:
*����˵��:
*�޸�����    �汾��   �޸���    �޸���
******************************************************************/
void TIM4_Cap_Init(u16 arr, u16 psc)
{
    GPIO_InitType GPIO_InitStructure;
    TMR_TimerBaseInitType  TIM_TimeBaseStructure;
    TMR_ICInitType  TIM_ICInitStructure;
    NVIC_InitType NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR4, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOD, ENABLE);
    TMR_Reset(TMR4);
    TMR_ClearITPendingBit(TMR4, TMR_INT_Overflow );     //���TIM4�����жϱ�־

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO, ENABLE);

    //��ȫ��ӳ�� 7.4.6 ��ʱ�����ù�����ӳ��
    GPIO_PinsRemapConfig(AFIO_MAP4_TMR4_0001, ENABLE);

    GPIO_InitStructure.GPIO_Pins  = GPIO_Pins_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    TIM_TimeBaseStructure.TMR_Period = arr;
    TIM_TimeBaseStructure.TMR_DIV = psc;
    TIM_TimeBaseStructure.TMR_ClockDivision = TMR_CKD_DIV1;
    TIM_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;
    TMR_TimeBaseInit(TMR4, &TIM_TimeBaseStructure);

    TIM_ICInitStructure.TMR_Channel = TMR_Channel_1;
    TIM_ICInitStructure.TMR_ICPolarity = TMR_ICPolarity_BothEdge;
    TIM_ICInitStructure.TMR_ICSelection = TMR_ICSelection_DirectTI;
    TIM_ICInitStructure.TMR_ICDIV = TMR_ICDIV_DIV1;
    TIM_ICInitStructure.TMR_ICFilter = 0x00;
    TMR_ICInit(TMR4, &TIM_ICInitStructure);

    //?D??��?����3?��??��
    NVIC_InitStructure.NVIC_IRQChannel = TMR4_GLOBAL_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TMR_INTConfig(TMR4, TMR_INT_CC1, ENABLE);

    TMR_ClearITPendingBit(TMR4, TMR_INT_CC1);

    TMR_Cmd(TMR4, ENABLE );
}

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
void TIM9_Cap_Init(u16 arr, u16 psc)
{
    GPIO_InitType GPIO_InitStructure;
    TMR_TimerBaseInitType  TIM_TimeBaseStructure;
    TMR_ICInitType  TIM_ICInitStructure;
    NVIC_InitType NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR9, ENABLE);	//��1?��TIM2����?��
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOE, ENABLE);  //��1?��GPIOA����?��
    TMR_Reset(TMR9);
    TMR_ClearITPendingBit(TMR9, TMR_INT_Overflow );     //���TIM1�����жϱ�־

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO, ENABLE);

    //��ȫ��ӳ�� 7.4.6 ��ʱ�����ù�����ӳ��
    GPIO_PinsRemapConfig(AFIO_MAP3_TMR9_0010, ENABLE);

    GPIO_InitStructure.GPIO_Pins  = GPIO_Pins_5 | GPIO_Pins_6; //PA0 ??3y???�㨦��??
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU; //PA0 ��?��?
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz; //PA0 ��?��?
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    //3?��??��?������?��2 TMR2
    TIM_TimeBaseStructure.TMR_Period = arr; //����?��??��y?�¡�??��??����?��
    TIM_TimeBaseStructure.TMR_DIV = psc; 	//?�衤??��?��
    TIM_TimeBaseStructure.TMR_ClockDivision = TMR_CKD_DIV1; //����??����?����???:TDTS = Tck_tim
    TIM_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;  //TIM?����???��y?�꨺?
    TMR_TimeBaseInit(TMR9, &TIM_TimeBaseStructure); //?��?YTIM_TimeBaseInitStruct?D???����?2?��y3?��??��TIMx��?����???����y�̣�??

    //3?��??��TIM2��?��?2???2?��y
    TIM_ICInitStructure.TMR_Channel = TMR_Channel_1; //CC1S=01 	????��?��??? IC1��3��?��?TI1��?
    TIM_ICInitStructure.TMR_ICPolarity = TMR_ICPolarity_BothEdge;	//��?��y??2???
    TIM_ICInitStructure.TMR_ICSelection = TMR_ICSelection_DirectTI; //��3��?��?TI1��?
    TIM_ICInitStructure.TMR_ICDIV = TMR_ICDIV_DIV1;	 //????��?��?��??��,2?��??��
    TIM_ICInitStructure.TMR_ICFilter = 0x00;//IC1F=0000 ????��?��???2��?�� 2???2��
    TMR_ICInit(TMR9, &TIM_ICInitStructure);
    TIM_ICInitStructure.TMR_Channel = TMR_Channel_2;
    TMR_ICInit(TMR9, &TIM_ICInitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = TMR1_BRK_TMR9_IRQn;  //TMR2?D??
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //?��??��??��??2??
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;  //�䨮��??��??0??
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ�����̨���?��1?��
    NVIC_Init(&NVIC_InitStructure);  //?��?YNVIC_InitStruct?D???����?2?��y3?��??����a����NVIC??��??��

    TMR_INTConfig(TMR9, TMR_INT_CC1 | TMR_INT_CC2, ENABLE);

    TMR_ClearITPendingBit(TMR9, TMR_INT_CC1 | TMR_INT_CC2);

    TMR_Cmd(TMR9, ENABLE ); 	//��1?��?������?��2
}

/******************************************************************
*��������:TIM10_Cap_Init
*��������:ͨ�ö�ʱ��10 ���벶���ܳ�ʼ��
*�������:arr���Զ���װֵ��psc��ʱ��Ԥ��Ƶ��
*�������:
*����ֵ:
*����˵��:
*�޸�����    �汾��   �޸���    �޸���
******************************************************************/
void TIM10_Cap_Init(u16 arr, u16 psc)
{
    GPIO_InitType GPIO_InitStructure;
    TMR_TimerBaseInitType  TIM_TimeBaseStructure;
    TMR_ICInitType  TIM_ICInitStructure;
    NVIC_InitType NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR10, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);
    TMR_Reset(TMR10);
    TMR_ClearITPendingBit(TMR10, TMR_INT_Overflow );     //���TIM4�����жϱ�־

    GPIO_InitStructure.GPIO_Pins  = GPIO_Pins_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    TIM_TimeBaseStructure.TMR_Period = arr;
    TIM_TimeBaseStructure.TMR_DIV = psc;
    TIM_TimeBaseStructure.TMR_ClockDivision = TMR_CKD_DIV1;
    TIM_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;
    TMR_TimeBaseInit(TMR10, &TIM_TimeBaseStructure);

    TIM_ICInitStructure.TMR_Channel = TMR_Channel_1;
    TIM_ICInitStructure.TMR_ICPolarity = TMR_ICPolarity_BothEdge;
    TIM_ICInitStructure.TMR_ICSelection = TMR_ICSelection_DirectTI;
    TIM_ICInitStructure.TMR_ICDIV = TMR_ICDIV_DIV1;
    TIM_ICInitStructure.TMR_ICFilter = 0x00;
    TMR_ICInit(TMR10, &TIM_ICInitStructure);

    //?D??��?����3?��??��
    NVIC_InitStructure.NVIC_IRQChannel = TMR1_OV_TMR10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TMR_INTConfig(TMR10, TMR_INT_CC1, ENABLE);

    TMR_ClearITPendingBit(TMR10, TMR_INT_CC1);

    TMR_Cmd(TMR10, ENABLE );
}

/******************************************************************
*��������:TIM10_Cap_Init
*��������:ͨ�ö�ʱ��11 ���벶���ܳ�ʼ��
*�������:arr���Զ���װֵ��psc��ʱ��Ԥ��Ƶ��
*�������:
*����ֵ:
*����˵��:
*�޸�����    �汾��   �޸���    �޸���
******************************************************************/
void TIM11_Cap_Init(u16 arr, u16 psc)
{
    GPIO_InitType GPIO_InitStructure;
    TMR_TimerBaseInitType  TIM_TimeBaseStructure;
    TMR_ICInitType  TIM_ICInitStructure;
    NVIC_InitType NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR11, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);
    TMR_Reset(TMR11);
    TMR_ClearITPendingBit(TMR11, TMR_INT_Overflow );     //���TIM4�����жϱ�־

    GPIO_InitStructure.GPIO_Pins  = GPIO_Pins_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    TIM_TimeBaseStructure.TMR_Period = arr;
    TIM_TimeBaseStructure.TMR_DIV = psc;
    TIM_TimeBaseStructure.TMR_ClockDivision = TMR_CKD_DIV1;
    TIM_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;
    TMR_TimeBaseInit(TMR11, &TIM_TimeBaseStructure);

    TIM_ICInitStructure.TMR_Channel = TMR_Channel_1;
    TIM_ICInitStructure.TMR_ICPolarity = TMR_ICPolarity_BothEdge;
    TIM_ICInitStructure.TMR_ICSelection = TMR_ICSelection_DirectTI;
    TIM_ICInitStructure.TMR_ICDIV = TMR_ICDIV_DIV1;
    TIM_ICInitStructure.TMR_ICFilter = 0x00;
    TMR_ICInit(TMR11, &TIM_ICInitStructure);

    //?D??��?����3?��??��
    NVIC_InitStructure.NVIC_IRQChannel = TMR1_TRG_COM_TMR11_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TMR_INTConfig(TMR11, TMR_INT_CC1, ENABLE);

    TMR_ClearITPendingBit(TMR11, TMR_INT_CC1);

    TMR_Cmd(TMR11, ENABLE );
}



#elif defined(HC32F46x)

/********************************************************************************
 ** \brief  Initialize the system clock for the sample
 **
 ** \param  None
 **
 ** \return None
 ******************************************************************************/
void SysClkIni(void)
{
    stc_clk_xtal_cfg_t   stcXtalCfg;
    stc_clk_sysclk_cfg_t    stcSysClkCfg;
    stc_clk_mpll_cfg_t      stcMpllCfg;
    //		stc_clk_freq_t          stcClkTmp;

    MEM_ZERO_STRUCT(stcSysClkCfg);
    MEM_ZERO_STRUCT(stcMpllCfg);

    /* Set bus clk div. */
    stcSysClkCfg.enHclkDiv = ClkSysclkDiv1;   // Max 168MHz
    stcSysClkCfg.enExclkDiv = ClkSysclkDiv2;  // Max 84MHz
    stcSysClkCfg.enPclk0Div = ClkSysclkDiv1;  // Max 168MHz ClkSysclkDiv1;//
    stcSysClkCfg.enPclk1Div = ClkSysclkDiv2;  /* Max 84MHz */ //ClkSysclkDiv4;  // Max 42MHz //
    stcSysClkCfg.enPclk2Div = ClkSysclkDiv4;  // Max 60MHz
    stcSysClkCfg.enPclk3Div = ClkSysclkDiv4;  // Max 42MHz
    stcSysClkCfg.enPclk4Div = ClkSysclkDiv2;  // Max 84MHz
    CLK_SysClkConfig(&stcSysClkCfg);

    /* Switch system clock source to MPLL. */
    CLK_HrcCmd(Enable);

    /* Switch system clock source to MPLL.   */
    /* Use Xtal as MPLL source.
    stcXtalCfg.enMode = ClkXtalModeOsc;
    stcXtalCfg.enDrv = ClkXtalLowDrv;
    stcXtalCfg.enFastStartup = Enable;
    CLK_XtalConfig(&stcXtalCfg);
    CLK_XtalCmd(Enable);*/

    /* MPLL Config. */
    /*system clk = 168M, pclk1 = 84M, pclk3 = 42M*/
    stcMpllCfg.pllmDiv = 1u;//1u;
    stcMpllCfg.plln = 42u;//21u;// 8 * 42 = 336MHz
    stcMpllCfg.PllpDiv = 2u;//1u;//
    stcMpllCfg.PllqDiv = 2u;
    stcMpllCfg.PllrDiv = 2u;

    CLK_SetPllSource(ClkPllSrcXTAL); //�ⲿ����8M
    CLK_MpllConfig(&stcMpllCfg);

    /* flash read wait cycle setting */
    EFM_Unlock();
    EFM_SetLatency(EFM_LATENCY_5);
    EFM_Lock();

    /* Enable MPLL. */
    CLK_MpllCmd(Enable);

    /* Wait MPLL ready. */
    while(Set != CLK_GetFlagStatus(ClkFlagMPLLRdy))
    {
        ;
    }

    /* Switch system clock source to MPLL. */
    CLK_SetSysClkSource(CLKSysSrcMPLL);

}

void Delay_Init(void)
{
    SysTick->LOAD  = (uint32_t)(SystemCoreClock / 1000);              /* set reload register 1KHz */

    SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */

    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                     SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick IRQ and SysTick Timer */
}
//us
void Delay_Us(uint32_t u32Cnt)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD; //sysTick load  1ms??

    ticks = u32Cnt * (SystemCoreClock / 1000000); //???1us???????
    told = SysTick->VAL;

    while(1)
    {
        tnow = SysTick->VAL;

        if(tnow != told)
        {

            if(tnow < told)
                tcnt += told - tnow;
            else
                tcnt += reload - tnow + told;

            told = tnow;

            if(tcnt >= ticks)
                break;
        }
    }
}
//ms
void Delay_Ms(uint32_t u32Cnt)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD; //sysTick load  1ms??

    ticks = u32Cnt * (SystemCoreClock / 1000); //???1ms???????
    told = SysTick->VAL;

    while(1)
    {
        tnow = SysTick->VAL;

        if(tnow != told)
        {

            if(tnow < told)
                tcnt += told - tnow;
            else
                tcnt += reload - tnow + told;

            told = tnow;

            if(tcnt >= ticks)
                break;
        }
    }
}

//���жϺ�
static void IrqRegistration(en_int_src_t enIntSrc, IRQn_Type enIRQn)
{
    stc_intc_sel_field_t *stcIntSel = NULL;

    /* IRQ032~127 whether out of range */
    if ((enIRQn >= 32) && ((((enIntSrc / 32) * 6 + 32) > enIRQn) || (((enIntSrc / 32) * 6 + 37) < enIRQn)))
    {
        return;
    }
    else
    {
        stcIntSel = (stc_intc_sel_field_t *)((uint32_t)(&M4_INTC->SEL0)  +  (4u * enIRQn));
        //if (0x1FFu == stcIntSel->INTSEL)
        {
            stcIntSel->INTSEL = enIntSrc;
        }
    }
}

//ϵͳ���ĵ��ȶ�ʱ����ʼ��������250us��ʱ���
void SysScheduleTimerInit(void)
{
    stc_timer4_cnt_init_t stcCntInit;    /* Clear structures */

    MEM_ZERO_STRUCT(stcCntInit);

    /* Enable peripheral clock */
    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIM41, Enable);

    /* Timer4-CNT : Initialize CNT configuration structure */
    stcCntInit.enBufferCmd = Enable;
    stcCntInit.enClk = Timer4CntPclk;
    stcCntInit.enClkDiv = Timer4CntPclkDiv4;    /* Timer4-CNT clock divide */
    stcCntInit.u16Cycle = 5250 - 1; /* Timer4-CNT cycle */
    stcCntInit.enCntMode = Timer4CntSawtoothWave;
    stcCntInit.enZeroIntCmd = Disable;    /* Enable zero match interrupt */
    stcCntInit.enPeakIntCmd = Enable;   /* Disable peak match interrupt */
    stcCntInit.enZeroIntMsk = Timer4CntIntMask0;
    stcCntInit.enPeakIntMsk = Timer4CntIntMask0;
    TIMER4_CNT_Init(M4_TMR41, &stcCntInit); /* Initialize Timer4-CNT */

    IrqRegistration(INT_TMR41_GOVF, Int005_IRQn);
    NVIC_SetPriority(Int005_IRQn, 5);
    NVIC_ClearPendingIRQ(Int005_IRQn);
    NVIC_EnableIRQ(Int005_IRQn);

    /* Start Timer4-CNT */
    TIMER4_CNT_ClearCountVal(M4_TMR41);
    TIMER4_CNT_Start(M4_TMR41);
}

void IRQ005_Handler(void)
{
    if(TIMER4_CNT_GetIrqFlag(M4_TMR41, Timer4CntPeakMatchInt))
    {
        SysScheduleProcess();
        /* Clear Timer4-CNT Peak interrupt flag */
        TIMER4_CNT_ClearIrqFlag(M4_TMR41, Timer4CntPeakMatchInt);
    }



}
#endif








static SoftTimer gSoftTimers[TIMER_NUM];

extern void SoftTimerInit(void)
{
    uint32_t i;

    for(i = 0; i < TIMER_NUM; i++)
    {
        SoftTimerStop(i);
    }
}

extern uint32_t SoftTimerIsStart(uint32_t TimerID)
{
    if (TimerID >= TIMER_NUM)
    {
        return FALSE;
    }

    return gSoftTimers[TimerID].TimerEnable;
}
extern void SoftTimerStop(uint32_t TimerID)
{
    if (TimerID >= TIMER_NUM)
    {
        return;
    }

    SOFT_Timer_Disable;
    gSoftTimers[TimerID].TimerCnt = 0;
    gSoftTimers[TimerID].TimerOut = 0;
    gSoftTimers[TimerID].pFunc = NULL;
    gSoftTimers[TimerID].pFuncParam = NULL;
    gSoftTimers[TimerID].TimerEnable = FALSE;
    SOFT_Timer_Enable;

    return;
}

extern void SoftTimerStart(uint32_t TimerID, uint32_t n100Ms, ENUM_SOFT_TIMER_MODE Mode, PTimeOutCallBack pTimeOutCallBack, void *pHandle)
{
    if (TimerID >= TIMER_NUM)
    {
        return;
    }

    if(TRUE == SoftTimerIsStart(TimerID))
        SoftTimerStop(TimerID);

    SOFT_Timer_Disable;
    gSoftTimers[TimerID].TimerCnt = 0;
    gSoftTimers[TimerID].TimerOut = n100Ms;
    gSoftTimers[TimerID].TimerMode = Mode;
    gSoftTimers[TimerID].pFunc = pTimeOutCallBack;
    gSoftTimers[TimerID].pFuncParam = pHandle;
    gSoftTimers[TimerID].TimerEnable = TRUE;

    SOFT_Timer_Enable;

    return;
}

extern uint32_t SoftTimerIsOver(uint32_t TimerID)
{
    uint32_t bRet = FALSE;


    if (TimerID >= TIMER_NUM)
    {
        return FALSE;
    }

    SOFT_Timer_Disable;

    if (gSoftTimers[TimerID].TimerEnable)
    {
        if (gSoftTimers[TimerID].TimerCnt > gSoftTimers[TimerID].TimerOut)
        {

            gSoftTimers[TimerID].TimerEnable = FALSE;
            bRet = TRUE;


        }
    }

    SOFT_Timer_Enable;

    return bRet;
}

extern void SoftTimerIntervalProcess(void)
{
    uint32_t i;
    PTimeOutCallBack pTimeOutCallBack = NULL;
    void *pHandle = NULL;

    for(i = 0; i < TIMER_NUM; i++)
    {
        if(gSoftTimers[i].TimerEnable == TRUE)
        {
            gSoftTimers[i].TimerCnt++;

            if ((gSoftTimers[i].TimerCnt > gSoftTimers[i].TimerOut) && (gSoftTimers[i].TimerMode == MODE_AUTO))
            {
                gSoftTimers[i].TimerEnable = FALSE;
                pTimeOutCallBack = gSoftTimers[i].pFunc;
                pHandle = gSoftTimers[i].pFuncParam;

                if(pTimeOutCallBack != NULL)
                {
                    (*pTimeOutCallBack)(pHandle);
                }
            }
        }
    }
}


#if 0


//������뷢��TIM����
void TIM6_U1_CHA(void)
{
    stc_timer6_basecnt_cfg_t         stcTIM6BaseCntCfg;

    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIM61, Enable);

    stcTIM6BaseCntCfg.enCntMode   = Timer6CntSawtoothMode;              //Sawtooth wave mode
    stcTIM6BaseCntCfg.enCntDir    = Timer6CntDirUp;                     //Counter counting up
    stcTIM6BaseCntCfg.enCntClkDiv = Timer6PclkDiv8;                     //Count clock: pclk0
    Timer6_Init(M4_TMR61, &stcTIM6BaseCntCfg);                          //timer6 PWM frequency, count mode and clk config

    /* Enable timer61 GOVF interrupt */
    Timer6_ConfigIrq(M4_TMR61, Timer6INTENOVF, true);

    IrqRegistration(INT_TMR61_GOVF, Int026_IRQn);
    NVIC_SetPriority(Int026_IRQn, 1);
    NVIC_ClearPendingIRQ(Int026_IRQn);
    NVIC_EnableIRQ(Int026_IRQn);

    M4_TMR61->GCONR_f.START = 0;
}


#endif






/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
