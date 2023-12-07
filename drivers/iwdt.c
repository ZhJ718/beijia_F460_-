/**
  ******************************************************************************
  * @file    iwdt.c
  * @author  xfw
  * @brief   This file contains definitions for watch dog
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
#include "iwdt.h"


void IWDG_Init(void)
{

    #if defined(AT32F4xx)
    //1000ms内喂狗
    // 使能 预分频寄存器PR和重装载寄存器RLR可写
    IWDG_KeyRegWrite( IWDG_KeyRegWrite_Enable );
    // 设置预分频器值
    IWDG_SetPrescaler( IWDG_Psc_256 );
    // 设置重装载寄存器值
    IWDG_SetReload( 160 );
    // 把重装载寄存器的值放到计数器中
    IWDG_ReloadCounter();
    // 使能 IWDG
    IWDG_Enable();

    #elif defined(HC32F46x)
    //估计1.597s多的喂狗窗口期
    stc_wdt_init_t stcWdtInit;

    /* configure structure initialization */
    MEM_ZERO_STRUCT(stcWdtInit);

    stcWdtInit.enClkDiv = WdtPclk3Div1024;
    stcWdtInit.enCountCycle = WdtCountCycle65536;
    stcWdtInit.enRefreshRange = WdtRefresh0To100Pct;
    stcWdtInit.enSleepModeCountEn = Disable;
    stcWdtInit.enRequsetType = WdtTriggerResetRequest;

    WDT_Init(&stcWdtInit);
    #endif
}

// 喂狗
void IWDG_Feed(void)
{
    #if defined(AT32F4xx)
    // 把重装载寄存器的值放到计数器中，喂狗，防止IWDG复位
    // 当计数器的值减到0的时候会产生系统复位
    IWDG_ReloadCounter();
    #elif defined(HC32F46x)
    WDT_RefreshCounter();
    #endif
}




/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
