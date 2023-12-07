/**
  ******************************************************************************
  * @file    ljb_laser_send.c
  * @author  xfw
  * @brief   This file contains definitions for laser sending
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2021 Beijing HuaRu Tech Co.</center></h2>
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
#include "ljb_laser_send.h"
#include "protocal.h"

#if 1

LJB_LASER_SEND_MAG g_sLaserSendMag;

//激光发射过程，定时器中断调用
void LaserSend_Process(LJB_LASER_SEND_MAG *pMag)
{
    LJB_LASER_SEND_SINGLE_ENCODE *pSingle = &pMag->m_sSingle;
    volatile uint8_t idx;


    if(pSingle->m_byNowStep < pSingle->m_byMaxStep)
    {
        pSingle->m_psEncode->m_saStep[pSingle->m_byNowStep].m_IsHigh ? pMag->m_sHwOps.send_gpio_high() : pMag->m_sHwOps.send_gpio_low();
        pMag->m_sHwOps.open_timing(pSingle->m_psEncode->m_saStep[pSingle->m_byNowStep].m_wDelayCmpCnt);

        if(++pSingle->m_byNowStep >= pSingle->m_byMaxStep)
        {
            pSingle->m_byNowStep = 0;
            #if 1 //连续发送三遍 AAABBBCCC

            if(++pMag->m_byRepeatCnt >= LJB_LASER_SEND_REPEAT_CNT)
            {
                pMag->m_byRepeatCnt = 0;

                if(pMag->m_bySentLen < pMag->m_bySendLen)
                {
                    if(pMag->m_bySentLen % 2 == 0)
                    {
                        idx = pMag->m_bySendBuf[pMag->m_bySentLen / 2] >> 4;
                        pSingle->m_psEncode = (LJB_LASER_SEND_ENCODE_MAG *)&gs_LaserSendEncode[idx];
                    }
                    else
                    {
                        idx = pMag->m_bySendBuf[pMag->m_bySentLen / 2] & 0x0f;
                        idx += 16;
                        pSingle->m_psEncode = (LJB_LASER_SEND_ENCODE_MAG *)&gs_LaserSendEncode[idx];
                    }
                }
                else if(pMag->m_bySentLen == pMag->m_bySendLen)
                {
                    pSingle->m_psEncode = (LJB_LASER_SEND_ENCODE_MAG *)&gs_LaserSendEncode[ENCODE_TAIL];

                }
                else
                {
                    pMag->m_IsRun = FALSE;
                    //TIMER0->CTL &= ~TIMER_CTL_CNTEN_Msk;//关闭定时器
                    pMag->m_sHwOps.close_timing();
                    //NVIC_ClearPendingIRQ(TMR1_IRQn);

                    return;
                }

                pMag->m_bySentLen++;

            }
            else
            {

            }

            #endif

            #if 0  //连续发送三遍 ABCABCABC

            if(pMag->m_bySentLen < pMag->m_bySendLen)
            {
                if(pMag->m_bySentLen % 2 == 0)
                {
                    idx = pMag->m_bySendBuf[pMag->m_bySentLen / 2] >> 4;
                    pSingle->m_psEncode = (LJB_LASER_SEND_ENCODE_MAG *)&gs_LaserEncode[idx];
                }
                else
                {
                    idx = pMag->m_bySendBuf[pMag->m_bySentLen / 2] & 0x0f;
                    idx += 16;
                    pSingle->m_psEncode = (LJB_LASER_SEND_ENCODE_MAG *)&gs_LaserEncode[idx];
                }
            }
            else if(pMag->m_bySentLen == pMag->m_bySendLen)
            {
                pSingle->m_psEncode = (LJB_LASER_SEND_ENCODE_MAG *)&gs_LaserEncode[ENCODE_TAIL];

            }
            else
            {
                //连续发送三次
                if(++pMag->m_byRepeatCnt >= LJB_LASER_SEND_REPEAT_CNT)
                {
                    pMag->m_IsRun = FALSE;
                    TIMER0->CTL &= ~TIMER_CTL_CNTEN_Msk;//关闭定时器
                    NVIC_ClearPendingIRQ(TMR1_IRQn);
                }
                else
                {
                    pMag->m_bySentLen = 0;
                    pMag->m_sSingle.m_psEncode = (LJB_LASER_ENCODE_MAG *)&gs_LaserEncode[ENCODE_HEAD];
                }

                return;
            }

            pMag->m_bySentLen++;
            #endif
            #if 0

            //连续发送帧头LJB_LASER_SEND_REPEAT_CNT次
            if(++pMag->m_byRepeatCnt >= LJB_LASER_SEND_REPEAT_CNT)
            {
                pMag->m_IsRun = FALSE;
                TIMER0->CTL &= ~TIMER_CTL_CNTEN_Msk;//关闭定时器
                NVIC_ClearPendingIRQ(TMR1_IRQn);
            }
            else
            {
                pMag->m_bySentLen = 0;
                pMag->m_sSingle.m_psEncode = (LJB_LASER_SEND_ENCODE_MAG *)&gs_LaserEncode[ENCODE_HEAD];
            }

            #endif


        }
    }

}

void LaserSend_Start(LJB_LASER_SEND_MAG *pMag, uint8_t *pBuf, uint16_t Len)
{
    if((!pMag->m_IsRun) && (Len < LJB_LASER_SEND_LEN_MAX))
    {
        pMag->m_IsRun = TRUE;
        pMag->m_byRepeatCnt = 0;
        pMag->m_bySentLen = 0;
        pMag->m_bySendLen = Len * 2 + 2;
        pMag->m_bySendBuf[Len] = Crc8_Calc(0x07, pBuf, Len);

        while(Len)
        {
            pMag->m_bySendBuf[Len - 1] = pBuf[Len - 1];
            Len -= 1;
        }


        pMag->m_sSingle.m_byNowStep = 0;
        pMag->m_sSingle.m_psEncode = (LJB_LASER_SEND_ENCODE_MAG *)&gs_LaserSendEncode[ENCODE_HEAD];
        pMag->m_sSingle.m_byMaxStep = 12;

        //延时一会进定时器中断开始发送
        pMag->m_sHwOps.open_timing(40);

        //NVIC_SetPendingIRQ(TMR1_IRQn);
    }
}

void LaserSend_Init(LJB_LASER_SEND_MAG *pMag, LJB_LASER_SEND_HW_OPERATIONS a_sHwOps)
{

    memset(pMag, 0, sizeof(LJB_LASER_SEND_MAG));

    pMag->m_sHwOps.hw_init = a_sHwOps.hw_init;
    pMag->m_sHwOps.send_gpio_high = a_sHwOps.send_gpio_high;
    pMag->m_sHwOps.send_gpio_low = a_sHwOps.send_gpio_low;
    pMag->m_sHwOps.open_timing = a_sHwOps.open_timing;
    pMag->m_sHwOps.close_timing = a_sHwOps.close_timing;

    //IO初始化
    if(pMag->m_sHwOps.hw_init != NULL)
    {
        pMag->m_sHwOps.hw_init();
    }

    //IO拉低
    if(pMag->m_sHwOps.send_gpio_low != NULL)
    {
        pMag->m_sHwOps.send_gpio_low();
    }

    if(pMag->m_sHwOps.close_timing != NULL)
    {
        pMag->m_sHwOps.close_timing();
    }

    pMag->m_wLaserIntensity = 100;	//默认新标准要求的高脉冲3us，最大3*500/100 = 15us

    pMag->m_IsEnableIRT1 = TRUE;
    pMag->m_IsEnableIRT2 = TRUE;
    pMag->m_IsEnableIRT3 = TRUE;
    pMag->m_IsEnableIRT4 = TRUE;
}

uint8_t IsLaserSendIdle(LJB_LASER_SEND_MAG Mag)
{
    return Mag.m_IsRun ? 0 : 1;
}


#endif


/*
需要移植部分

*/

#if defined(AT32F4xx)

//TODO

#elif defined(HC32F46x)

void LaserSend_Gpio_Init(void)
{
    stc_port_init_t stcPortInit;
    MEM_ZERO_STRUCT(stcPortInit);
    /*initiallize LED port*/
    stcPortInit.enPinMode = Pin_Mode_Out;

    //	stcPortInit.enExInt = Enable;
    //    stcPortInit.enPullUp = Enable;

    /* LED0 and LED1 Port/Pin initialization */
    PORT_Init(PortA, Pin01, &stcPortInit);
    PORT_ResetBits(PortA, Pin01);
    PORT_Init(PortA, Pin03, &stcPortInit);
    PORT_ResetBits(PortA, Pin03);
    PORT_Init(PortA, Pin12, &stcPortInit);
    PORT_ResetBits(PortA, Pin12);
    PORT_Init(PortB, Pin06, &stcPortInit);
    PORT_ResetBits(PortB, Pin06);
}

void LaserSend_Gpio_On(void)
{
    if(TRUE == g_sLaserSendMag.m_IsEnableIRT1)
    {
        PORT_SetBits(PortB, Pin06);
    }

    if(TRUE == g_sLaserSendMag.m_IsEnableIRT2)
    {
        PORT_SetBits(PortA, Pin01);
    }

    if(TRUE == g_sLaserSendMag.m_IsEnableIRT3)
    {
        PORT_SetBits(PortA, Pin12);
    }

    if(TRUE == g_sLaserSendMag.m_IsEnableIRT4)
    {
        PORT_SetBits(PortA, Pin03);
    }

}
void LaserSend_Gpio_Off(void)
{
    if(TRUE == g_sLaserSendMag.m_IsEnableIRT1)
    {
        PORT_ResetBits(PortB, Pin06);
    }

    if(TRUE == g_sLaserSendMag.m_IsEnableIRT2)
    {
        PORT_ResetBits(PortA, Pin01);
    }

    if(TRUE == g_sLaserSendMag.m_IsEnableIRT3)
    {
        PORT_ResetBits(PortA, Pin12);
    }

    if(TRUE == g_sLaserSendMag.m_IsEnableIRT4)
    {
        PORT_ResetBits(PortA, Pin03);
    }
}

void LaserSend_Gpio_Toggle(void)
{
    if(TRUE == g_sLaserSendMag.m_IsEnableIRT1)
    {
        PORT_Toggle(PortB, Pin06);
    }

    if(TRUE == g_sLaserSendMag.m_IsEnableIRT2)
    {
        PORT_Toggle(PortA, Pin01);
    }

    if(TRUE == g_sLaserSendMag.m_IsEnableIRT3)
    {
        PORT_Toggle(PortA, Pin12);
    }

    if(TRUE == g_sLaserSendMag.m_IsEnableIRT4)
    {
        PORT_Toggle(PortA, Pin03);
    }

}



//绑定中断号
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

void LaserSend_LowlevelInit(void)
{
    LaserSend_Gpio_Init();

    stc_timer4_cnt_init_t stcCntInit;    /* Clear structures */

    MEM_ZERO_STRUCT(stcCntInit);

    /* Enable peripheral clock */
    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIM42, Enable);

    /* Timer4-CNT : Initialize CNT configuration structure */
    stcCntInit.enBufferCmd = Disable;
    stcCntInit.enClk = Timer4CntPclk;
    stcCntInit.enClkDiv = Timer4CntPclkDiv2;    /* Timer4-CNT clock divide */
    stcCntInit.u16Cycle = 0;  /* Timer4-CNT cycle */
    stcCntInit.enCntMode = Timer4CntSawtoothWave;
    stcCntInit.enZeroIntCmd = Disable;    /* Enable zero match interrupt */
    stcCntInit.enPeakIntCmd = Enable;   /* Disable peak match interrupt */
    stcCntInit.enZeroIntMsk = Timer4CntIntMask0;
    stcCntInit.enPeakIntMsk = Timer4CntIntMask0;
    TIMER4_CNT_Init(M4_TMR42, &stcCntInit); /* Initialize Timer4-CNT */

    IrqRegistration(INT_TMR42_GOVF, Int000_IRQn);
    NVIC_SetPriority(Int000_IRQn, 0);
    NVIC_ClearPendingIRQ(Int000_IRQn);
    NVIC_EnableIRQ(Int000_IRQn);

    /* Start Timer4-CNT */
    //TIMER4_CNT_ClearCountVal(M4_TMR42);



    TIMER4_CNT_Stop(M4_TMR42);
}

void LaserSendTimer_Open(uint16_t Val)
{
    M4_TMR42->CCSR_f.STOP = 1;
    M4_TMR42->CCSR_f.CLEAR = 1;
    M4_TMR42->CPSR = Val;
    M4_TMR42->CCSR_f.STOP = 0;
}


void LaserSendTimer_Close(void)
{
    M4_TMR42->CCSR_f.STOP = 1;//关闭定时器
}



//计数完成中断
void IRQ000_Handler(void)
{
    if(TIMER4_CNT_GetIrqFlag(M4_TMR42, Timer4CntPeakMatchInt))
    {
        LaserSend_Process(&g_sLaserSendMag);
        /* Clear Timer4-CNT Peak interrupt flag */
        TIMER4_CNT_ClearIrqFlag(M4_TMR42, Timer4CntPeakMatchInt);
    }

}






void LaserSendTimerTest(void)
{
    #if 0
    SYS_UnlockReg();

    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_SetModuleClock(TMR0_MODULE, CLK_TMR0_SRC_HCLK, CLK_CLKDIV_HCLK(1));//分不了频

    SYS_LockReg();

    TIMER0->CTL = 0;
    TIMER0->EXTCTL = 0;

    TIMER0->CTL = TIMER_ONESHOT_MODE | TIMER_CTL_INTEN_Msk; //单次计数模式，使能中断

    NVIC_SetPriority(TMR0_IRQn, 0);
    NVIC_EnableIRQ(TMR0_IRQn);

    //TIMER_Start(TIMER0);
    TIMER0->CMP = 40;
    TIMER0->CTL |= TIMER_CTL_CNTEN_Msk;



    #endif

    stc_timer4_cnt_init_t stcCntInit;    /* Clear structures */

    MEM_ZERO_STRUCT(stcCntInit);

    /* Enable peripheral clock */
    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIM42, Enable);

    /* Timer4-CNT : Initialize CNT configuration structure */
    stcCntInit.enBufferCmd = Disable;
    stcCntInit.enClk = Timer4CntPclk;
    stcCntInit.enClkDiv = Timer4CntPclkDiv2;    /* Timer4-CNT clock divide */
    stcCntInit.u16Cycle = 0;  /* Timer4-CNT cycle */
    stcCntInit.enCntMode = Timer4CntSawtoothWave;
    stcCntInit.enZeroIntCmd = Disable;    /* Enable zero match interrupt */
    stcCntInit.enPeakIntCmd = Enable;   /* Disable peak match interrupt */
    stcCntInit.enZeroIntMsk = Timer4CntIntMask0;
    stcCntInit.enPeakIntMsk = Timer4CntIntMask0;
    TIMER4_CNT_Init(M4_TMR42, &stcCntInit); /* Initialize Timer4-CNT */

    IrqRegistration(INT_TMR42_GOVF, Int000_IRQn);
    NVIC_SetPriority(Int000_IRQn, 0);
    NVIC_ClearPendingIRQ(Int000_IRQn);
    NVIC_EnableIRQ(Int000_IRQn);

    /* Start Timer4-CNT */
    //TIMER4_CNT_ClearCountVal(M4_TMR42);


    M4_TMR42->CCSR_f.CLEAR = 1;
    M4_TMR42->CPSR = 42000 - 1;

    TIMER4_CNT_Start(M4_TMR42);

    LaserSend_Gpio_Init();

}


#endif



/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
