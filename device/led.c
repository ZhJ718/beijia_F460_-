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


/******************************状态灯N连闪算法***********************
将N连闪分成5s一循环,再将该循环分成若干阶段。
一连闪:2个阶段，第一阶段500ms的绿灯亮，第二阶段500ms的灯灭
二连闪:4个阶段，第一阶段500ms的绿灯亮，第二阶段500ms的灯灭
			  ，第三阶段500ms的绿灯亮，第四阶段3500ms的灯灭，
三连闪:6个阶段，第一阶段500ms的绿灯亮，第二阶段500ms的灯灭
			  ，第三阶段500ms的绿灯亮，第四阶段500ms的灯灭，
			  ，第五阶段500ms的绿灯亮，第六阶段2500ms的灯灭，

数据结构:
	 typedef struct
	 {
		 uint8_t					 m_byMaxStep;			 //最大阶段
		 USERLED_CYCLE_SECTION		 m_pPeriod[LED_CYCLE_SECTION_CNT_MAX];
	 }LED_CYCLE;

 	 LED_CYCLE结构体存放
 		N连闪包含阶段数(m_byMaxStep)
 		和每个阶段(LED_CYCLE_SECTION)的灯颜色(m_enumLedColor)和持续时间(m_by100MsCnt)(*100MS)

	 typedef struct
	 {
		 uint8_t					 m_byNowStep;			 //当前所在哪个阶段
		 uint8_t					 m_byTimePast100Ms; 	 //当前所在阶段已经持续时间(*100MS)
		 LED_CYCLE				 m_sCycle;
	 }LED_CYCLE_MAG;

	 LED_CYCLE_MAG结构体存放
 		当前N连闪当前所在阶段(m_byNowStep)
 		和当前阶段已经持续时间(m_byTimePast100Ms)


代码实现:
	1.实现100ms定时回调函数:extern void UserLed_CoreLedProcess(USERLED_MAG *pMag)
		程序逻辑:
			检测当前阶段已经持续时间，
				若 == 0，表示当前阶段即将开始，需要重新设置灯的状态

				若加1 <= 该阶段持续时间，则当前阶段已经持续时间加1(*100MS)
				其余，当前阶段已经持续时间清零，进行下一阶段判断
					若当前阶段超过了最大阶段，则当前阶段复位
					否则当前阶段指向下一个阶段
		特殊:若阶段持续时间为0，则
********************************************************************/

/****************************************************************
@FUNCTION：灯控功能暂停
@INPUT：LED管理结构体
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Led_Suspend(LED_MAG *pMag)
{
    pMag->m_byIsStartLed = FALSE;
}

/****************************************************************
@FUNCTION：灯控功能是否暂停
@INPUT：LED管理结构体
@OUTPUT：无
@RETURN: TRUE-暂停 FALSE-未暂停
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
u32 Led_IsSuspend(LED_MAG *pMag)
{
    return (pMag->m_byIsStartLed == FALSE ? TRUE : FALSE);
}

/****************************************************************
@FUNCTION：灯控功能恢复
@INPUT：LED管理结构体
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Led_Recovery(LED_MAG *pMag)
{
    pMag->m_byIsStartLed = TRUE;
}

/****************************************************************
@FUNCTION：控制灯
@INPUT：LED亮灭，LED硬件操作
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 默认灯灭
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
@FUNCTION：设置灯N连闪
@INPUT：N连闪,LED管理结构体
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 设置时会暂停灯控功能，设置完成会恢复
****************************************************************/
extern void Led_SetRunMode(LED_MODE BlinkMode, LED_MAG *pMag)
{
    DDL_ASSERT(pMag != NULL);

    if(BlinkMode == pMag->m_dwMode)
    {
        return;
    }

    //暂停LED定时任务
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

    //恢复LED定时任务
    pMag->m_byIsStartLed = TRUE;
}

/****************************************************************
@FUNCTION：设置灯亮度
@INPUT：灯亮度,LED管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 亮度百分比0-100
****************************************************************/
extern void Led_SetBrightness(uint8_t SetValue, LED_MAG *pMag)
{
    DDL_ASSERT(pMag != NULL);

    pMag->m_byBrightness = (SetValue > 100) ? 100 : SetValue;
    pMag->m_byBrightness /= 10;
}

/****************************************************************
@FUNCTION：实现单色灯硬件控制逻辑
@INPUT：LED管理结构体
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 1Ms调度一次

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
@FUNCTION：实现单色灯控制逻辑
@INPUT：LED管理结构体
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 每固定时间(目前100Ms)调度一次

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
@FUNCTION：实现单色灯控制逻辑
@INPUT：LED管理结构体
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 1Ms调度一次

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
@FUNCTION：灯控功能初始化
@INPUT：LED管理结构体
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 根据灯硬件操作相关函数，初始化硬件和关闭灯，暂停灯控功能
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

//测试代码，运行灯，PB6
#if 0
void RunLed_GpioInit(void)
{
    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //PB6 推挽输出
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO口速度为50MHz
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
@FUNCTION：灯控功能初始化，
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：
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
