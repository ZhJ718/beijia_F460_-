/**
  ******************************************************************************
  * @file    led.c
  * @author  xfw
  * @brief   This file contains definitions for STM32F103 Leds
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
#include "led.h"


/******************************״̬��N�����㷨***********************
��N�����ֳ�5sһѭ��,�ٽ���ѭ���ֳ����ɽ׶Ρ�
һ����:2���׶Σ���һ�׶�500ms���̵������ڶ��׶�500ms�ĵ���
������:4���׶Σ���һ�׶�500ms���̵������ڶ��׶�500ms�ĵ���
			  �������׶�500ms���̵��������Ľ׶�3500ms�ĵ���
������:6���׶Σ���һ�׶�500ms���̵������ڶ��׶�500ms�ĵ���
			  �������׶�500ms���̵��������Ľ׶�500ms�ĵ���
			  ������׶�500ms���̵����������׶�2500ms�ĵ���

���ݽṹ:
	 typedef struct
	 {
		 uint8_t					 m_byMaxStep;			 //���׶�
		 USERLED_CYCLE_SECTION		 m_pPeriod[LED_CYCLE_SECTION_CNT_MAX];
	 }LED_CYCLE;

 	 LED_CYCLE�ṹ����
 		N���������׶���(m_byMaxStep)
 		��ÿ���׶�(LED_CYCLE_SECTION)�ĵ���ɫ(m_enumLedColor)�ͳ���ʱ��(m_by100MsCnt)(*100MS)

	 typedef struct
	 {
		 uint8_t					 m_byNowStep;			 //��ǰ�����ĸ��׶�
		 uint8_t					 m_byTimePast100Ms; 	 //��ǰ���ڽ׶��Ѿ�����ʱ��(*100MS)
		 LED_CYCLE				 m_sCycle;
	 }LED_CYCLE_MAG;

	 LED_CYCLE_MAG�ṹ����
 		��ǰN������ǰ���ڽ׶�(m_byNowStep)
 		�͵�ǰ�׶��Ѿ�����ʱ��(m_byTimePast100Ms)


����ʵ��:
	1.ʵ��100ms��ʱ�ص�����:extern void UserLed_CoreLedProcess(USERLED_MAG *pMag)
		�����߼�:
			��⵱ǰ�׶��Ѿ�����ʱ�䣬
				�� == 0����ʾ��ǰ�׶μ�����ʼ����Ҫ�������õƵ�״̬

				����1 <= �ý׶γ���ʱ�䣬��ǰ�׶��Ѿ�����ʱ���1(*100MS)
				���࣬��ǰ�׶��Ѿ�����ʱ�����㣬������һ�׶��ж�
					����ǰ�׶γ��������׶Σ���ǰ�׶θ�λ
					����ǰ�׶�ָ����һ���׶�
		����:���׶γ���ʱ��Ϊ0����
********************************************************************/

/****************************************************************
@FUNCTION���ƿع�����ͣ
@INPUT��LED����ṹ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Led_Suspend(LED_MAG *pMag)
{
    pMag->m_byIsStartLed = FALSE;
}

/****************************************************************
@FUNCTION���ƿع����Ƿ���ͣ
@INPUT��LED����ṹ��
@OUTPUT����
@RETURN: TRUE-��ͣ FALSE-δ��ͣ
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
u32 Led_IsSuspend(LED_MAG *pMag)
{
    return (pMag->m_byIsStartLed == FALSE ? TRUE : FALSE);
}

/****************************************************************
@FUNCTION���ƿع��ָܻ�
@INPUT��LED����ṹ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Led_Recovery(LED_MAG *pMag)
{
    pMag->m_byIsStartLed = TRUE;
}

/****************************************************************
@FUNCTION�����Ƶ�
@INPUT��LED����LEDӲ������
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: Ĭ�ϵ���
****************************************************************/
static void Led_CtrlLedOnOff(LED_COLOR Color, LED_HW_OPERATIONS LedhwOps)
{
    switch(Color)
    {
        case LED_COLOR_BRIGHT:
            if(LedhwOps.led_on != NULL)
            {
                LedhwOps.led_on();
            }

            break;

        default:
            if(LedhwOps.led_off != NULL)
            {
                LedhwOps.led_off();
            }

            break;
    }
}

/****************************************************************
@FUNCTION�����õ�N����
@INPUT��N����,LED����ṹ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ����ʱ����ͣ�ƿع��ܣ�������ɻ�ָ�
****************************************************************/
extern void Led_SetRunMode(LED_MODE BlinkMode, LED_MAG *pMag)
{
    DDL_ASSERT(pMag != NULL);

    if(BlinkMode == pMag->m_dwMode)
    {
        return;
    }

    //��ͣLED��ʱ����
    pMag->m_byIsStartLed = FALSE;

    pMag->m_dwMode = BlinkMode;

    memset(&pMag->m_sCycleMag, 0, sizeof(LED_CYCLE_MAG));

    switch(pMag->m_dwMode)
    {
        case LED_MODE_BLINK_ALWAYS_ON:
            memcpy(&pMag->m_sCycleMag.m_sCycle, &gc_sBlink_AlwaysOn, sizeof(LED_CYCLE));
            break;

        case LED_MODE_BLINK_1:
            memcpy(&pMag->m_sCycleMag.m_sCycle, &gc_sBlink_1, sizeof(LED_CYCLE));
            break;

        case LED_MODE_BLINK_2:
            memcpy(&pMag->m_sCycleMag.m_sCycle, &gc_sBlink_2, sizeof(LED_CYCLE));
            break;

        case LED_MODE_BLINK_3:
            memcpy(&pMag->m_sCycleMag.m_sCycle, &gc_sBlink_3, sizeof(LED_CYCLE));
            break;

        case LED_MODE_BLINK_4:
            memcpy(&pMag->m_sCycleMag.m_sCycle, &gc_sBlink_4, sizeof(LED_CYCLE));
            break;

        case LED_MODE_BLINK_5:
            memcpy(&pMag->m_sCycleMag.m_sCycle, &gc_sBlink_5, sizeof(LED_CYCLE));
            break;

        case LED_MODE_BLINK_6:
            memcpy(&pMag->m_sCycleMag.m_sCycle, &gc_sBlink_6, sizeof(LED_CYCLE));
            break;

        case LED_MODE_BLINK_7:
            memcpy(&pMag->m_sCycleMag.m_sCycle, &gc_sBlink_7, sizeof(LED_CYCLE));
            break;

        default:
            memcpy(&pMag->m_sCycleMag.m_sCycle, &gc_sBlink_AlwaysOff, sizeof(LED_CYCLE));
            break;
    }

    //�ָ�LED��ʱ����
    pMag->m_byIsStartLed = TRUE;
}

/****************************************************************
@FUNCTION�����õ�����
@INPUT��������,LED����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ���Ȱٷֱ�0-100
****************************************************************/
extern void Led_SetBrightness(uint8_t SetValue, LED_MAG *pMag)
{
    DDL_ASSERT(pMag != NULL);

    pMag->m_byBrightness = (SetValue > 100) ? 100 : SetValue;
    pMag->m_byBrightness /= 10;
}

/****************************************************************
@FUNCTION��ʵ�ֵ�ɫ��Ӳ�������߼�
@INPUT��LED����ṹ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: 1Ms����һ��

****************************************************************/
extern void Led_TimedProcess1Ms(LED_MAG *pMag)
{

    if(TRUE == pMag->m_byIsLedOn)
    {

        if(pMag->m_byBrightCnt < pMag->m_byBrightness)
        {
            if(pMag->m_sHwOps.led_on != NULL)
            {
                pMag->m_sHwOps.led_on();
            }
        }
        else if(pMag->m_byBrightCnt > pMag->m_byBrightness)
        {
            if(pMag->m_sHwOps.led_off != NULL)
            {
                pMag->m_sHwOps.led_off();
            }
        }

        pMag->m_byBrightCnt++;

        if(pMag->m_byBrightCnt > 9)
        {
            pMag->m_byBrightCnt = 0;
        }

    }
    else
    {
        if(pMag->m_sHwOps.led_off != NULL)
        {
            pMag->m_sHwOps.led_off();
        }
    }
}

/****************************************************************
@FUNCTION��ʵ�ֵ�ɫ�ƿ����߼�
@INPUT��LED����ṹ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ÿ�̶�ʱ��(Ŀǰ100Ms)����һ��

****************************************************************/
extern void Led_TimedProcess100Ms(LED_MAG *pMag)
{
    LED_CYCLE_MAG *psCycleMag = NULL;
    LED_CYCLE_SECTION *psSection = NULL;

    DDL_ASSERT(pMag != NULL);

    if(TRUE == Led_IsSuspend(pMag))
        return;

    psCycleMag = &pMag->m_sCycleMag;
    psSection = &psCycleMag->m_sCycle.m_pPeriod[psCycleMag->m_byNowStep % LED_CYCLE_SECTION_CNT_MAX];

    if(psCycleMag->m_byTimePast100Ms == 0)
    {
        pMag->m_byIsLedOn = (psSection->m_enumLedColor == LED_COLOR_BRIGHT) ? TRUE : FALSE;
    }//	Led_CtrlLedOnOff(psSection->m_enumLedColor,pMag->m_sHwOps);

    if((psCycleMag->m_byTimePast100Ms + 1) >= psSection->m_by100MsCnt)
    {
        psCycleMag->m_byTimePast100Ms = 0;

        if((psCycleMag->m_byNowStep + 1) >= psCycleMag->m_sCycle.m_byMaxStep)
        {
            psCycleMag->m_byNowStep = 0;
        }
        else
        {
            psCycleMag->m_byNowStep++;
        }
    }
    else
    {
        psCycleMag->m_byTimePast100Ms++;
    }
}

/****************************************************************
@FUNCTION��ʵ�ֵ�ɫ�ƿ����߼�
@INPUT��LED����ṹ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: 1Ms����һ��

****************************************************************/
extern void Led_TimedProcess(LED_MAG *pMag)
{
    Led_TimedProcess1Ms(pMag);

    pMag->m_w1MsCnt++;

    if(pMag->m_w1MsCnt > 99)
    {
        pMag->m_w1MsCnt = 0;
        Led_TimedProcess100Ms(pMag);
    }
}

/****************************************************************
@FUNCTION���ƿع��ܳ�ʼ��
@INPUT��LED����ṹ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ���ݵ�Ӳ��������غ�������ʼ��Ӳ���͹رյƣ���ͣ�ƿع���
****************************************************************/
extern void Led_Init(LED_MAG *pMag)
{
    DDL_ASSERT(pMag != NULL);

    Led_Suspend(pMag);

    if(pMag->m_sHwOps.led_hw_init != NULL)
    {
        pMag->m_sHwOps.led_hw_init();
    }

    if(pMag->m_sHwOps.led_off != NULL)
    {
        pMag->m_sHwOps.led_off();
    }

    Led_SetBrightness(100, pMag);
}

//���Դ��룬���еƣ�PB6
#if 0
void RunLed_GpioInit(void)
{
    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //PB6 �������
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void RunLed_On(void)
{
    GPIO_WriteBit(GPIOB, GPIO_Pins_6, Bit_RESET);
}

void RunLed_Off(void)
{
    GPIO_WriteBit(GPIOB, GPIO_Pins_6, Bit_SET);
}

/****************************************************************
@FUNCTION���ƿع��ܳ�ʼ����
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��
@SPECIAL:
****************************************************************/
extern void RunLed_Init(LED_MAG *pMag)
{
    DDL_ASSERT(pMag != NULL);

    memset(pMag, 0, sizeof(LED_MAG));

    pMag->m_sHwOps.led_hw_init = RunLed_GpioInit;
    pMag->m_sHwOps.led_on = RunLed_On;
    pMag->m_sHwOps.led_off = RunLed_Off;

    Led_Init(pMag);
}
#endif

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
