/**
  ******************************************************************************
  * @file    beep.c
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

/* Includes ------------------------------------------------------------------*/
#include "beep.h"



/****************************************************************
@FUNCTION：蜂鸣器控制功能暂停
@INPUT：BEEP管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Beep_Suspend(BEEP_MAG *pMag)
{
    pMag->m_byIsStartBeep = FALSE;

    //if(pMag->m_byIsPassiveBeep != TRUE)
    {
        pMag->m_sHwOps.beep_off();
    }

}

/****************************************************************
@FUNCTION：蜂鸣器控制功能是否暂停
@INPUT：BEEP管理结构体
@OUTPUT：无
@RETURN: TRUE-暂停 FALSE-未暂停
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
u32 Beep_IsSuspend(BEEP_MAG Mag)
{
    return (Mag.m_byIsStartBeep == FALSE ? TRUE : FALSE);
}

/****************************************************************
@FUNCTION：设置蜂鸣器控制延时时间
@INPUT：BEEP管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Beep_SetDelayTime(uint16_t a_wCnt, BEEP_MAG *pMag)
{
    pMag->m_wSetCnt = a_wCnt;
}

/****************************************************************
@FUNCTION：增加蜂鸣器控制延时时间
@INPUT：BEEP管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Beep_AddDelayTime(uint16_t a_wCnt, BEEP_MAG *pMag)
{
    pMag->m_wSetCnt += a_wCnt;
}

/****************************************************************
@FUNCTION：蜂鸣器控制功能恢复
@INPUT：BEEP管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Beep_Recovery(BEEP_MAG *pMag)
{
    pMag->m_byIsStartBeep = TRUE;

    //if(pMag->m_byIsPassiveBeep != TRUE)
    {
        pMag->m_sHwOps.beep_on();
    }
}

/****************************************************************
@FUNCTION：非阻塞模式下实现蜂鸣器控制功能
@INPUT：BEEP管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 每固定时间(目前1Ms)调度一次,为了兼容无源蜂鸣器，1kHz

****************************************************************/
extern void Beep_TimedProcess(BEEP_MAG *pMag)
{
    assert_param(pMag != NULL);

    if(TRUE == Beep_IsSuspend(*pMag))
        return;


    if(pMag->m_wNowCnt <= pMag->m_wSetCnt)
    {
        if(TRUE == pMag->m_byIsPassiveBeep)
        {
            pMag->m_sHwOps.beep_toggle();
        }
        else
        {
            pMag->m_sHwOps.beep_on();
        }

        pMag->m_wNowCnt++;
    }
    else
    {
        Beep_Suspend(pMag);
        pMag->m_sHwOps.beep_off();
    }
}


/****************************************************************
@FUNCTION：非阻塞模式下蜂鸣器控制功能初始化
@INPUT：硬件接口函数结构体，是否是无源蜂鸣器(TRUE-无源)，BEEP管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 根据蜂鸣器硬件操作相关函数，初始化硬件和关闭蜂鸣器，暂停蜂鸣器功能
****************************************************************/
extern void Beep_LL_Init(BEEP_HW_OPERATIONS a_sHwOps, u32 a_IsPassiveBeep, BEEP_MAG *pMag)
{
    assert_param(pMag != NULL);

    memset(pMag, 0, sizeof(BEEP_MAG));

    pMag->m_sHwOps.beep_hw_init = a_sHwOps.beep_hw_init;
    pMag->m_sHwOps.beep_on = a_sHwOps.beep_on;
    pMag->m_sHwOps.beep_off = a_sHwOps.beep_off;
    pMag->m_sHwOps.beep_toggle = a_sHwOps.beep_toggle;

    pMag->m_byIsPassiveBeep = (a_IsPassiveBeep ? TRUE : FALSE);

    Beep_Suspend(pMag);

    if(pMag->m_sHwOps.beep_hw_init != NULL)
    {
        pMag->m_sHwOps.beep_hw_init();
    }

    if(pMag->m_sHwOps.beep_off != NULL)
    {
        pMag->m_sHwOps.beep_off();
    }
}


/****************************************************************
@FUNCTION：蜂鸣器鸣叫
@INPUT：BEEP管理结构体指针，操作模式，震动延迟
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:不支持阻塞模式下无源蜂鸣器鸣叫操作
****************************************************************/
extern void Beep_On(BEEP_MAG *pMag, OPT_MODE OptMode, uint16_t SetTime)
{
    if(OptMode == OPT_MODE_BLOCK && pMag->m_byIsPassiveBeep != TRUE)
    {
        pMag->m_sHwOps.beep_on();
    }
    else if(OptMode == OPT_MODE_NONBLOCK)
    {
        if(TRUE == Beep_IsSuspend(*pMag))
        {
            Beep_SetDelayTime(SetTime, pMag);
            pMag->m_wNowCnt = 0;
        }
        else
        {
            Beep_AddDelayTime(SetTime, pMag);
        }

        Beep_Recovery(pMag);
    }
}

/****************************************************************
@FUNCTION：蜂鸣器关闭
@INPUT：BEEP管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
extern void Beep_Off(BEEP_MAG *pMag, OPT_MODE OptMode)
{
    pMag->m_sHwOps.beep_off();

    if(OptMode != OPT_MODE_BLOCK)
    {
        Beep_Suspend(pMag);
    }

}

//测试代码，蜂鸣器，PB5
#if 0
//蜂鸣器GPIO口初始化
void Beep_GpioInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //PA5 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
//蜂鸣器开
void Beep_GpioOn(void)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
}
//蜂鸣器关
void Beep_GpioOff(void)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);
}
//蜂鸣器翻转
void Beep_GpioToggle(void)
{
    GPIOB->ODR ^= GPIO_Pin_5;
}


#endif

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
