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
@FUNCTION：系统核心时钟定时中断服务程序
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：xfw
@SPECIAL:每1ms定时调用
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
    srand(g_sSysScheTime.TimSysTimeCountMS);//设置随机数种子
}

/****************************************************************
@FUNCTION：获取系统自启动以来经历的秒数
@INPUT：无
@OUTPUT：无
@RETURN: 秒数
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
extern uint32_t GetSysRunSecond(void)
{
    return g_sSysScheTime.TimSysTimeCountS;
}

/****************************************************************
@FUNCTION：获取系统自启动以来经历的毫秒数
@INPUT：无
@OUTPUT：无
@RETURN: 秒数
@AUTHOR：xfw
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

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR4, ENABLE); //时钟使能

    //定时器TIM3初始化
    TIM_TimeBaseStructure.TMR_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TMR_DIV = psc; //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TMR_ClockDivision = TMR_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;  //TIM向上计数模式
    TMR_TimeBaseInit(TMR4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位

    TMR_INTConfig(TMR4, TMR_INT_Overflow, ENABLE ); //使能指定的TIM3中断,允许更新中断

    //中断优先级NVIC设置
    NVIC_InitStructure.NVIC_IRQChannel = TMR4_GLOBAL_IRQn;  //TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;  //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器


    TMR_Cmd(TMR4, ENABLE);  //使能TIMx
}

void TMR4_GLOBAL_IRQHandler(void)   //TIM3中断
{

    if (TMR_GetINTStatus(TMR4, TMR_INT_Overflow) != RESET)  //检查TIM3更新中断发生与否
    {

        SysScheduleProcess();
        TMR_ClearITPendingBit(TMR4, TMR_INT_Overflow); //清除TIMx更新中断标志
    }
}

/****************************************************************
@FUNCTION：Systick中断服务程序
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：xfw
@SPECIAL:每1ms定时调用,内部调用SysScheduleProcess()函数
***************************************************************
void SysTick_Handler(void)
{
		SysScheduleProcess();
}*/

/****************************************************************
@FUNCTION：系统核心时钟定时器初始化
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：xfw
@SPECIAL:实现250uss定时中断
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

    TIM4_Int_Init(A - 1, B - 1); // 40Khz的计数频率 计数到10为250us 产生一次中断
}



static u8  fac_us = 0;							//us延时倍乘数
static u16 fac_ms = 0;							//ms延时倍乘数,在ucos下,代表每个节拍的ms数


//初始化延迟函数
//当使用OS的时候,此函数会初始化OS的时钟节拍
//SYSTICK的时钟固定为HCLK时钟的1/8
//SYSCLK:系统时钟
void Delay_Init(void)
{
    #if SYSTEM_SUPPORT_OS  							//如果需要支持OS.
    u32 reload;
    #endif
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
    fac_us = SystemCoreClock / 8000000;				//为系统时钟的1/8
    #if SYSTEM_SUPPORT_OS  							//如果需要支持OS.
    reload = SystemCoreClock / 8000000;				//每秒钟的计数次数 单位为K
    reload *= 1000000 / delay_ostickspersec;		//根据delay_ostickspersec设定溢出时间
    //reload为24位寄存器,最大值:16777216,在72M下,约合1.86s左右
    fac_ms = 1000 / delay_ostickspersec;			//代表OS可以延时的最少单位

    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;   	//开启SYSTICK中断
    SysTick->LOAD = reload; 						//每1/delay_ostickspersec秒中断一次
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   	//开启SYSTICK

    #else
    fac_ms = (u16)fac_us * 1000;					//非OS下,代表每个ms需要的systick时钟数
    #endif
}

//延时nus
//nus为要延时的us数.
void Delay_Us(u32 nus)
{
    u32 temp;
    SysTick->LOAD = nus * fac_us; 					//时间加载
    SysTick->VAL = 0x00;        					//清空计数器
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;	//开始倒数

    do
    {
        temp = SysTick->CTRL;
    }
    while((temp & 0x01) && !(temp & (1 << 16)));		//等待时间到达

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
    SysTick->VAL = 0X00;      					 //清空计数器
}

//延时nms
//注意nms的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK单位为Hz,nms单位为ms
//对72M条件下,nms<=1864
void Delay_Ms(uint32_t nms)
{
    u32 temp;
    SysTick->LOAD = (u32)nms * fac_ms;				//时间加载(SysTick->LOAD为24bit)
    SysTick->VAL = 0x00;							//清空计数器
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;	//开始倒数

    do
    {
        temp = SysTick->CTRL;
    }
    while((temp & 0x01) && !(temp & (1 << 16)));		//等待时间到达

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
    SysTick->VAL = 0X00;       					//清空计数器
}






/******************************************************************
*函数名称:TIM6_Int_Init
*功能描述:定时器6中断初始化
*输入参数:arr：自动重装值。psc：时钟预分频数
*输出参数:
*返回值:
*其它说明: 单点操作模式
*修改日期    版本号   修改人    修改内容
*---------------------------------------------------
*                              查飞
******************************************************************/
void TIM6_Int_Init(u16 arr, u16 psc)
{
    NVIC_InitType NVIC_InitStructure;
    TMR_TimerBaseInitType  TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR6, ENABLE);//TIM6时钟使能

    //定时器TIM7初始化
    TIM_TimeBaseStructure.TMR_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TMR_DIV = psc; //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TMR_ClockDivision = TMR_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;  //TIM向上计数模式
    TMR_TimeBaseInit(TMR6, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
    TMR_ClearFlag(TMR6, TMR_FLAG_Update);
    TMR_INTConfig(TMR6, TMR_INT_Overflow, ENABLE ); //使能指定的TIM6中断,允许更新中断

    /* 禁止ARR预装载缓冲器 */
    TMR_ARPreloadConfig(TMR6, DISABLE);

    TMR_Cmd(TMR6, ENABLE); //开启定时器6

    NVIC_InitStructure.NVIC_IRQChannel = TMR6_GLOBAL_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0 ; //抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

}

/****************************************************************
@FUNCTION：TIM6微秒级中断
@INPUT：中断us数
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 中断时间需大于0，若为0，则中断1us
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
*函数名称:TIM7_Int_Init
*功能描述:定时器7中断初始化
*输入参数:arr：自动重装值。psc：时钟预分频数
*输出参数:
*返回值:
*其它说明:
*修改日期    版本号   修改人    修改内容
*---------------------------------------------------
*                              查飞
******************************************************************/
void TIM7_Int_Init(u16 arr, u16 psc)
{
    NVIC_InitType NVIC_InitStructure;
    TMR_TimerBaseInitType  TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR7, ENABLE);//TIM7时钟使能

    //定时器TIM7初始化
    TIM_TimeBaseStructure.TMR_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TMR_DIV = psc; //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TMR_ClockDivision = TMR_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;  //TIM向上计数模式
    TMR_TimeBaseInit(TMR7, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
    TMR_ClearFlag(TMR7, TMR_FLAG_Update);
    TMR_INTConfig(TMR7, TMR_INT_Overflow, ENABLE ); //使能指定的TIM7中断,允许更新中断

    /* 禁止ARR预装载缓冲器 */
    TMR_ARPreloadConfig(TMR7, DISABLE);

    TMR_Cmd(TMR7, ENABLE); //开启定时器7

    NVIC_InitStructure.NVIC_IRQChannel = TMR7_GLOBAL_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0 ; //抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

}

/****************************************************************
@FUNCTION：TIM7微秒级中断
@INPUT：中断us数
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 中断时间需大于0，若为0，则中断1us
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
*函数名称:TIM1_Cap_Init
*功能描述:高级定时器1输入捕获功能初始化
*输入参数:arr：自动重装值。psc：时钟预分频数
*输出参数:
*返回值:
*其它说明:
*修改日期    版本号   修改人    修改内容
*---------------------------------------------------
*                              查飞
******************************************************************/
void TIM1_Cap_Init(u16 arr, u16 psc)
{
    GPIO_InitType GPIO_InitStructure;
    TMR_TimerBaseInitType  TIM_TimeBaseStructure;
    TMR_ICInitType  TIM1_ICInitStructure;
    NVIC_InitType NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR1, ENABLE);	//ê1?üTIM2ê±?ó
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOE, ENABLE);  //ê1?üGPIOAê±?ó
    TMR_Reset(TMR1);
    TMR_ClearITPendingBit(TMR1, TMR_INT_Overflow );     //清除TIM1更新中断标志

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO, ENABLE);

    //完全重映射 7.4.6 定时器复用功能重映射
    GPIO_PinsRemapConfig(AFIO_MAP4_TMR1_0011, ENABLE);

    GPIO_InitStructure.GPIO_Pins  = GPIO_Pins_9 | GPIO_Pins_11 | GPIO_Pins_13; //PA0 ??3y???°éè??
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU; //PA0 ê?è?
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz; //PA0 ê?è?
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    //3?ê??ˉ?¨ê±?÷2 TMR2
    TIM_TimeBaseStructure.TMR_Period = arr; //éè?¨??êy?÷×??ˉ??×°?μ
    TIM_TimeBaseStructure.TMR_DIV = psc; 	//?¤·??μ?÷
    TIM_TimeBaseStructure.TMR_ClockDivision = TMR_CKD_DIV1; //éè??ê±?ó·???:TDTS = Tck_tim
    TIM_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;  //TIM?òé???êy?￡ê?
    TMR_TimeBaseInit(TMR1, &TIM_TimeBaseStructure); //?ù?YTIM_TimeBaseInitStruct?D???¨μ?2?êy3?ê??ˉTIMxμ?ê±???ùêyμ￥??

    //3?ê??ˉTIM2ê?è?2???2?êy
    TIM1_ICInitStructure.TMR_Channel = TMR_Channel_1; //CC1S=01 	????ê?è??? IC1ó3é?μ?TI1é?
    TIM1_ICInitStructure.TMR_ICPolarity = TMR_ICPolarity_BothEdge;	//é?éy??2???
    TIM1_ICInitStructure.TMR_ICSelection = TMR_ICSelection_DirectTI; //ó3é?μ?TI1é?
    TIM1_ICInitStructure.TMR_ICDIV = TMR_ICDIV_DIV1;	 //????ê?è?·??μ,2?·??μ
    TIM1_ICInitStructure.TMR_ICFilter = 0x00;//IC1F=0000 ????ê?è???2¨?÷ 2???2¨
    TMR_ICInit(TMR1, &TIM1_ICInitStructure);
    TIM1_ICInitStructure.TMR_Channel = TMR_Channel_2;
    TMR_ICInit(TMR1, &TIM1_ICInitStructure);
    TIM1_ICInitStructure.TMR_Channel = TMR_Channel_3;
    TMR_ICInit(TMR1, &TIM1_ICInitStructure);
    //?D??·?×é3?ê??ˉ
    NVIC_InitStructure.NVIC_IRQChannel = TMR1_CC_IRQn;  //TMR2?D??
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //?è??ó??è??2??
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 8;  //′óó??è??0??
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQí¨μà±?ê1?ü
    NVIC_Init(&NVIC_InitStructure);  //?ù?YNVIC_InitStruct?D???¨μ?2?êy3?ê??ˉíaéèNVIC??′??÷

    TMR_INTConfig(TMR1, TMR_INT_CC1 | TMR_INT_CC2 | TMR_INT_CC3, ENABLE);

    TMR_ClearITPendingBit(TMR1, TMR_INT_CC1 | TMR_INT_CC2 | TMR_INT_CC3);

    TMR_Cmd(TMR1, ENABLE ); 	//ê1?ü?¨ê±?÷2
}

/******************************************************************
*函数名称:TIM4_Cap_Init
*功能描述:通用定时器4 输入捕获功能初始化
*输入参数:arr：自动重装值。psc：时钟预分频数
*输出参数:
*返回值:
*其它说明:
*修改日期    版本号   修改人    修改内
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
    TMR_ClearITPendingBit(TMR4, TMR_INT_Overflow );     //清除TIM4更新中断标志

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO, ENABLE);

    //完全重映射 7.4.6 定时器复用功能重映射
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

    //?D??·?×é3?ê??ˉ
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
*函数名称:TIM9_Cap_Init
*功能描述:通用定时器9 输入捕获功能初始化
*输入参数:arr：自动重装值。psc：时钟预分频数
*输出参数:
*返回值:
*其它说明:
*修改日期    版本号   修改人    修改内容
*---------------------------------------------------
*                              查飞
******************************************************************/
void TIM9_Cap_Init(u16 arr, u16 psc)
{
    GPIO_InitType GPIO_InitStructure;
    TMR_TimerBaseInitType  TIM_TimeBaseStructure;
    TMR_ICInitType  TIM_ICInitStructure;
    NVIC_InitType NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR9, ENABLE);	//ê1?üTIM2ê±?ó
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOE, ENABLE);  //ê1?üGPIOAê±?ó
    TMR_Reset(TMR9);
    TMR_ClearITPendingBit(TMR9, TMR_INT_Overflow );     //清除TIM1更新中断标志

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO, ENABLE);

    //完全重映射 7.4.6 定时器复用功能重映射
    GPIO_PinsRemapConfig(AFIO_MAP3_TMR9_0010, ENABLE);

    GPIO_InitStructure.GPIO_Pins  = GPIO_Pins_5 | GPIO_Pins_6; //PA0 ??3y???°éè??
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU; //PA0 ê?è?
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz; //PA0 ê?è?
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    //3?ê??ˉ?¨ê±?÷2 TMR2
    TIM_TimeBaseStructure.TMR_Period = arr; //éè?¨??êy?÷×??ˉ??×°?μ
    TIM_TimeBaseStructure.TMR_DIV = psc; 	//?¤·??μ?÷
    TIM_TimeBaseStructure.TMR_ClockDivision = TMR_CKD_DIV1; //éè??ê±?ó·???:TDTS = Tck_tim
    TIM_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;  //TIM?òé???êy?￡ê?
    TMR_TimeBaseInit(TMR9, &TIM_TimeBaseStructure); //?ù?YTIM_TimeBaseInitStruct?D???¨μ?2?êy3?ê??ˉTIMxμ?ê±???ùêyμ￥??

    //3?ê??ˉTIM2ê?è?2???2?êy
    TIM_ICInitStructure.TMR_Channel = TMR_Channel_1; //CC1S=01 	????ê?è??? IC1ó3é?μ?TI1é?
    TIM_ICInitStructure.TMR_ICPolarity = TMR_ICPolarity_BothEdge;	//é?éy??2???
    TIM_ICInitStructure.TMR_ICSelection = TMR_ICSelection_DirectTI; //ó3é?μ?TI1é?
    TIM_ICInitStructure.TMR_ICDIV = TMR_ICDIV_DIV1;	 //????ê?è?·??μ,2?·??μ
    TIM_ICInitStructure.TMR_ICFilter = 0x00;//IC1F=0000 ????ê?è???2¨?÷ 2???2¨
    TMR_ICInit(TMR9, &TIM_ICInitStructure);
    TIM_ICInitStructure.TMR_Channel = TMR_Channel_2;
    TMR_ICInit(TMR9, &TIM_ICInitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = TMR1_BRK_TMR9_IRQn;  //TMR2?D??
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //?è??ó??è??2??
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;  //′óó??è??0??
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQí¨μà±?ê1?ü
    NVIC_Init(&NVIC_InitStructure);  //?ù?YNVIC_InitStruct?D???¨μ?2?êy3?ê??ˉíaéèNVIC??′??÷

    TMR_INTConfig(TMR9, TMR_INT_CC1 | TMR_INT_CC2, ENABLE);

    TMR_ClearITPendingBit(TMR9, TMR_INT_CC1 | TMR_INT_CC2);

    TMR_Cmd(TMR9, ENABLE ); 	//ê1?ü?¨ê±?÷2
}

/******************************************************************
*函数名称:TIM10_Cap_Init
*功能描述:通用定时器10 输入捕获功能初始化
*输入参数:arr：自动重装值。psc：时钟预分频数
*输出参数:
*返回值:
*其它说明:
*修改日期    版本号   修改人    修改内
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
    TMR_ClearITPendingBit(TMR10, TMR_INT_Overflow );     //清除TIM4更新中断标志

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

    //?D??·?×é3?ê??ˉ
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
*函数名称:TIM10_Cap_Init
*功能描述:通用定时器11 输入捕获功能初始化
*输入参数:arr：自动重装值。psc：时钟预分频数
*输出参数:
*返回值:
*其它说明:
*修改日期    版本号   修改人    修改内
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
    TMR_ClearITPendingBit(TMR11, TMR_INT_Overflow );     //清除TIM4更新中断标志

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

    //?D??·?×é3?ê??ˉ
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

    CLK_SetPllSource(ClkPllSrcXTAL); //外部晶振8M
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

//系统核心调度定时器初始化，产生250us定时间隔
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


//激光编码发射TIM配置
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
