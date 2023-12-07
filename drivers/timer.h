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
    uint8_t	IsSysTimeUpdate_250US: 1;		// 250us定时到标记,需要软件清0
    uint8_t	IsSysTimeUpdate_1MS: 1;		// 1ms定时到标记,需要软件清0
    uint8_t	IsSysTimeUpdate_10MS: 1;		//10ms定时到标记,需要软件清0
    uint8_t	IsSysTimeUpdate_100MS: 1;	//100ms定时到标记,需要软件清0
    uint8_t	IsSysTimeUpdate_1S: 1;		// 1s定时到标记,需要软件清0
    uint8_t	IsSysTimeUpdate_10S: 1;		// 10s定时到标记,需要软件清0
    uint8_t	IsSysTimeUpdate_1Min: 1;	// 1min定时到标记,需要软件清0
    uint8_t	IsSysTimeUpdate_1Hour: 1;	// hour定时到标记,需要软件清0

    uint8_t isRes: 8;
    uint16_t TimSysTimeCount250US;
    uint16_t TimSysTimeCountMS;			//当前系统运行时间,单位:ms
    uint32_t TimSysTimeCountS;			//当前系统运行时间,单位:s
    uint32_t TimSysTimeCountTick;			//当前系统核心时钟调度次数
} TIMER_SCHEDULE_CLOCK;


extern TIMER_SCHEDULE_CLOCK g_sSysScheTime;


/****************************************************************
@FUNCTION：系统核心时钟定时器初始化
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：xfw
@SPECIAL:实现250us定时中断
****************************************************************/
void SysScheduleTimerInit(void);

/****************************************************************
@FUNCTION：获取系统自启动以来经历的秒数
@INPUT：无
@OUTPUT：无
@RETURN: 秒数
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
extern uint32_t GetSysRunSecond(void);

/****************************************************************
@FUNCTION：获取系统自启动以来经历的毫秒数
@INPUT：无
@OUTPUT：无
@RETURN: 秒数
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
extern uint32_t GetSysRunMilliSecond(void);

/****************************************************************
@FUNCTION：设置随机数种子
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:根据当前系统时钟设置
****************************************************************/
extern void SetRand(void);

//初始化延迟函数
//当使用OS的时候,此函数会初始化OS的时钟节拍
//SYSTICK的时钟固定为HCLK时钟的1/8
//SYSCLK:系统时钟
void Delay_Init(void);


//延时nus
//nus为要延时的us数.
void Delay_Us(uint32_t nus);


//延时nms
//注意nms的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK单位为Hz,nms单位为ms
//对72M条件下,nms<=1864
void Delay_Ms(uint32_t nms);

#if defined( AT32F4xx)

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
void TIM6_Int_Init(u16 arr, u16 psc);

/****************************************************************
@FUNCTION：TIM6微秒级中断
@INPUT：中断us数
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 中断时间需大于0，若为0，则中断1us
****************************************************************/
void TIM6_Sleep_Us(uint16_t nus);



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
void TIM7_Int_Init(u16 arr, u16 psc);

/****************************************************************
@FUNCTION：TIM7微秒级中断
@INPUT：中断us数
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 中断时间需大于0，若为0，则中断1us
****************************************************************/
void TIM7_Sleep_Us(uint16_t nus);

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
void TIM1_Cap_Init(u16 arr, u16 psc);

/******************************************************************
*函数名称:TIM4_Cap_Init
*功能描述:通用定时器4 输入捕获功能初始化
*输入参数:arr：自动重装值。psc：时钟预分频数
*输出参数:
*返回值:
*其它说明:
*修改日期    版本号   修改人    修改内
******************************************************************/
void TIM4_Cap_Init(u16 arr, u16 psc);

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
void TIM9_Cap_Init(u16 arr, u16 psc);

/******************************************************************
*函数名称:TIM10_Cap_Init
*功能描述:通用定时器10 输入捕获功能初始化
*输入参数:arr：自动重装值。psc：时钟预分频数
*输出参数:
*返回值:
*其它说明:
*修改日期    版本号   修改人    修改内容
*---------------------------------------------------
*                              查飞
******************************************************************/
void TIM10_Cap_Init(u16 arr, u16 psc);

/******************************************************************
*函数名称:TIM11_Cap_Init
*功能描述:通用定时器11 输入捕获功能初始化
*输入参数:arr：自动重装值。psc：时钟预分频数
*输出参数:
*返回值:
*其它说明:
*修改日期    版本号   修改人    修改内容
*---------------------------------------------------
*                              查飞
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
    MODE_NORMAL = 0,//0常规模式,需要自己判断定时器是否超时,此时后面参数无用
    MODE_AUTO,		// 1定时器自动超时处理模式,定时中断时判断超时后调用超时回调函数
} ENUM_SOFT_TIMER_MODE;

typedef struct
{
    uint32_t TimerEnable;
    uint32_t TimerOut;
    uint32_t TimerCnt;
    // 0常规模式,需要自己判断定时器是否超时,此时后面参数无用
    // 1定时器自动超时处理模式,定时中断时判断超时后调用超时回调函数
    ENUM_SOFT_TIMER_MODE TimerMode;
    PTimeOutCallBack pFunc;
    void *pFuncParam;
} SoftTimer;

typedef  enum
{
    TIMER_TEST,                     //系统测试定时器
    TIMER_CAR_SPEED_MONITOR,                     //小车速度监控
    TIMER_NUM						//定时器个数
} ENUM_TIMER;

#define SOFT_Timer_Disable 0//TIMER_DisableInt(TIMER3)
#define SOFT_Timer_Enable 0//TIMER_EnableInt(TIMER3)
#define SOFT_Timer_Interval_MS 100		//定时器最小定时间隔为100ms
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
