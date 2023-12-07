/**
  ******************************************************************************
  * @file    led_rgb.h
  * @author  xfw
  * @brief   This file contains definitions for RGB Led
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
#ifndef __LED_RGB_H
#define __LED_RGB_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "config.h"
#include "led.h"

/**
 * @brief LED Types Definition
 */

//灯色彩
typedef enum
{
    LED_RGB_COLOR_GREEN = 0,//绿  - 电流最小
    LED_RGB_COLOR_RED,// 红
    LED_RGB_COLOR_BLUE,// 蓝
    LED_RGB_COLOR_YELLOW,// 黄
    LED_RGB_COLOR_PURPLE,// 紫
    LED_RGB_COLOR_LIGHT_BLUE,// 浅蓝
    LED_RGB_COLOR_WRITE,// 白
    LED_RGB_COLOR_MAX,//
} LED_RGB_COLOR;

//灯模式，默认单闪
typedef enum
{
    LED_RGB_MODE_BLINK_ALWAYS_OFF,	//常灭
    LED_RGB_MODE_BLINK_TEMPORARY,	//暂时闪
    LED_RGB_MODE_BLINK_1,	//单闪
    LED_RGB_MODE_BLINK_2,	//双闪
    LED_RGB_MODE_BLINK_ALWAYS_ON,	//常亮
    LED_RGB_MODE_BLINK_MAX,
} LED_RGB_MODE;


//LED硬件操作
typedef struct
{
    //LED硬件接口初始化函数
    void (*led_hw_init)(void);
    //红LED亮操作
    void (*led_r_on)(void);
    //绿LED亮操作
    void (*led_g_on)(void);
    //蓝LED亮操作
    void (*led_b_on)(void);
    //LED灭操作
    void (*led_off)(void);
} LED_RGB_HW_OPERATIONS;


typedef struct
{
    //实现LED相关逻辑，只读
    uint8_t		m_byIsStartLed: 1;
    uint8_t		m_byIsLedOn: 1;
    uint8_t		m_byRes: 6;
    uint8_t		m_byBrightness;//亮度 0 - 100

    uint8_t		m_byBrightCnt;
    uint8_t		m_wRunCnt;	//运行次数，仅适用于LED_RGB_MODE_BLINK_TEMPORARY模式
    uint16_t	m_w1MsCnt;	//1ms计数

    LED_CYCLE_MAG	 m_sCycleMag;
    LED_RGB_MODE		m_dwMode;
    LED_RGB_COLOR		m_eColor;
    //实现LED相关硬件操作，可读写
    LED_RGB_HW_OPERATIONS m_sHwOps;


} LED_RGB_MAG;


/****************************************************************
@FUNCTION：灯控功能暂停
@INPUT：LED管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Led_RGB_Suspend(LED_RGB_MAG *pMag);

/****************************************************************
@FUNCTION：灯控功能是否暂停
@INPUT：LED管理结构体指针
@OUTPUT：无
@RETURN: TRUE-暂停 FALSE-未暂停
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
u32 Led_RGB_IsSuspend(LED_RGB_MAG *pMag);

/****************************************************************
@FUNCTION：灯控功能恢复
@INPUT：LED管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Led_RGB_Recovery(LED_RGB_MAG *pMag);

/****************************************************************
@FUNCTION：设置灯闪烁模式
@INPUT：LED管理结构体
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 设置时会暂停灯控功能，设置完成会恢复
****************************************************************/
extern void Led_RGB_SetRunMode(LED_RGB_MODE BlinkMode, uint16_t BlinkCnt, LED_RGB_MAG *pMag);


/****************************************************************
@FUNCTION：设置灯亮度
@INPUT：灯亮度,LED管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 亮度百分比0-100
****************************************************************/
extern void Led_RGB_SetBrightness(uint8_t SetValue, LED_RGB_MAG *pMag);


/****************************************************************
@FUNCTION：设置灯色彩
@INPUT：色彩,LED管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 默认绿色
****************************************************************/
extern void Led_RGB_SetColor(LED_RGB_COLOR eColor, LED_RGB_MAG *pMag);



/****************************************************************
@FUNCTION：实现单色灯控制逻辑
@INPUT：LED管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 每固定时间(目前100Ms)调度一次

****************************************************************/
extern void Led_RGB_TimedProcess(LED_RGB_MAG *pMag) ;

/****************************************************************
@FUNCTION：灯控功能初始化
@INPUT：LED管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 根据灯硬件操作相关函数，初始化硬件和关闭灯，暂停灯控功能
****************************************************************/
extern void Led_RGB_Init(LED_RGB_MAG *pMag);


#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
