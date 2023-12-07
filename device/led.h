/**
  ******************************************************************************
  * @file    led.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LED_H
#define __LED_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "config.h"


/**
 * @brief LED Types Definition
 */

//状态指示灯，默认灭
typedef enum
{
    LED_COLOR_DARK = 0,
    LED_COLOR_BRIGHT,
} LED_COLOR;

//灯模式，默认单闪
typedef enum
{
    LED_MODE_BLINK_7 = 0,// 7连闪
    LED_MODE_BLINK_6,	// 6连闪
    LED_MODE_BLINK_5,	// 5连闪
    LED_MODE_BLINK_4,	// 4连闪
    LED_MODE_BLINK_3,	// 3连闪
    LED_MODE_BLINK_2,	// 2连闪
    LED_MODE_BLINK_1,	//单闪
    LED_MODE_BLINK_ALWAYS_ON,	//常亮
    LED_MODE_BLINK_ALWAYS_OFF,	//常灭
    LED_MODE_BLINK_MAX,
} LED_MODE;

#define LED_CYCLE_SECTION_CNT_MAX	(2*LED_MODE_BLINK_MAX)

typedef struct
{
    uint8_t						m_by100MsCnt;	//维持时间(*100MS)
    LED_COLOR 	m_enumLedColor;	//灯颜色
} LED_CYCLE_SECTION;

typedef struct
{
    uint8_t 				m_byMaxStep;			//最大阶段
    LED_CYCLE_SECTION 		m_pPeriod[LED_CYCLE_SECTION_CNT_MAX];
} LED_CYCLE;

typedef struct
{
    uint8_t 				m_byNowStep;			//当前所在哪个阶段
    uint8_t 				m_byTimePast100Ms;		//当前所在阶段已经持续时间(*100MS)
    LED_CYCLE 				m_sCycle;
} LED_CYCLE_MAG;


//状态灯控制
//正常运行模式
//单闪每阶段( 7s)状态定义
static const LED_CYCLE gc_sBlink_1 =
{
    2,
    {
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
    }
};

// 2连闪每阶段(4s)状态定义
static const LED_CYCLE gc_sBlink_2 =
{
    4,
    {
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {25, LED_COLOR_DARK},
    }
};

// 3 连闪每阶段(5s)状态定义
static const LED_CYCLE gc_sBlink_3 =
{
    6,
    {
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {25, LED_COLOR_DARK},
    }
};


// 4连闪每阶段(6s)状态定义
static const LED_CYCLE gc_sBlink_4 =
{
    8,
    {
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {25, LED_COLOR_DARK},
    }
};

// 5连闪每阶段(7s)状态定义
static const LED_CYCLE gc_sBlink_5 =
{
    10,
    {
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {25, LED_COLOR_DARK},
    }
};

// 6连闪每阶段(8s)状态定义
static const LED_CYCLE gc_sBlink_6 =
{
    12,
    {
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {25, LED_COLOR_DARK},
    }
};

// 7连闪每阶段(9s)状态定义
static const LED_CYCLE gc_sBlink_7 =
{
    14,
    {
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_BRIGHT},
        {25, LED_COLOR_DARK},
    }
};



//常亮模式每阶段(5s)状态定义
static const LED_CYCLE gc_sBlink_AlwaysOn =
{
    2,
    {
        {5, LED_COLOR_BRIGHT},
        {5, LED_COLOR_BRIGHT},

    }
};

//常灭模式每阶段(5s)状态定义
static const LED_CYCLE gc_sBlink_AlwaysOff =
{
    2,
    {
        {5, LED_COLOR_DARK},
        {5, LED_COLOR_DARK},

    }
};


//LED硬件操作
typedef struct
{
    //LED硬件接口初始化函数
    void (*led_hw_init)(void);
    //LED亮操作
    void (*led_on)(void);
    //LED灭操作
    void (*led_off)(void);
} LED_HW_OPERATIONS;


typedef struct
{
    //实现LED相关逻辑，只读
    uint8_t		m_byIsStartLed: 1;
    uint8_t		m_byIsLedOn: 1;
    uint8_t		m_byRes: 6;
    uint8_t		m_byBrightness;//亮度 0 - 100

    uint8_t		m_byBrightCnt;
    uint16_t	m_w1MsCnt;	//1ms计数

    LED_CYCLE_MAG	 m_sCycleMag;
    LED_MODE		m_dwMode;
    //实现LED相关硬件操作，可读写
    LED_HW_OPERATIONS m_sHwOps;


} LED_MAG;


/****************************************************************
@FUNCTION：灯控功能暂停
@INPUT：LED管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Led_Suspend(LED_MAG *pMag);

/****************************************************************
@FUNCTION：灯控功能是否暂停
@INPUT：LED管理结构体指针
@OUTPUT：无
@RETURN: TRUE-暂停 FALSE-未暂停
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
u32 Led_IsSuspend(LED_MAG *pMag);

/****************************************************************
@FUNCTION：灯控功能恢复
@INPUT：LED管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Led_Recovery(LED_MAG *pMag);

/****************************************************************
@FUNCTION：设置灯N连闪
@INPUT：N连闪,LED管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 设置时会暂停灯控功能，设置完成会恢复
****************************************************************/
extern void Led_SetRunMode(LED_MODE BlinkMode, LED_MAG *pMag);

/****************************************************************
@FUNCTION：设置灯亮度
@INPUT：灯亮度,LED管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 亮度百分比0-100
****************************************************************/
extern void Led_SetBrightness(uint8_t SetValue, LED_MAG *pMag);


/****************************************************************
@FUNCTION：实现单色灯控制逻辑
@INPUT：LED管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 每固定时间(目前100Ms)调度一次

****************************************************************/
extern void Led_TimedProcess(LED_MAG *pMag) ;

/****************************************************************
@FUNCTION：灯控功能初始化
@INPUT：LED管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 根据灯硬件操作相关函数，初始化硬件和关闭灯，暂停灯控功能
****************************************************************/
extern void Led_Init(LED_MAG *pMag);


#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
