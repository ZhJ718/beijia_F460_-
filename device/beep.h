/**
  ******************************************************************************
  * @file    beep.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BEEP_H
#define __BEEP_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "config.h"



//BEEP硬件操作
typedef struct
{
    //Beep硬件接口初始化函数
    void (*beep_hw_init)(void);
    //Beep开操作
    void (*beep_on)(void);
    //Beep关操作
    void (*beep_off)(void);
    //Beep翻转操作
    void (*beep_toggle)(void);
} BEEP_HW_OPERATIONS;


typedef struct
{
    //实现Beep相关异步操作逻辑，只读
    uint8_t		m_byIsStartBeep: 1;
    uint8_t		m_byIsPassiveBeep: 1;	//有源还是无源
    uint8_t		m_byRes: 6;

    uint16_t 	m_wSetCnt;	//延时设置值
    uint16_t 	m_wNowCnt;	//已经延时时间

    //实现Beep相关硬件操作，可读写
    BEEP_HW_OPERATIONS m_sHwOps;
} BEEP_MAG;


/****************************************************************
@FUNCTION：蜂鸣器控制功能暂停
@INPUT：BEEP管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Beep_Suspend(BEEP_MAG *pMag);

/****************************************************************
@FUNCTION：蜂鸣器控制功能是否暂停
@INPUT：BEEP管理结构体
@OUTPUT：无
@RETURN: TRUE-暂停 FALSE-未暂停
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
u32 Beep_IsSuspend(BEEP_MAG Mag);

/****************************************************************
@FUNCTION：设置蜂鸣器控制延时时间
@INPUT：BEEP管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Beep_SetDelayTime(uint16_t a_wCnt, BEEP_MAG *pMag);

/****************************************************************
@FUNCTION：增加蜂鸣器控制延时时间
@INPUT：BEEP管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Beep_AddDelayTime(uint16_t a_wCnt, BEEP_MAG *pMag);

/****************************************************************
@FUNCTION：蜂鸣器控制功能恢复
@INPUT：BEEP管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Beep_Recovery(BEEP_MAG *pMag);


/****************************************************************
@FUNCTION：非阻塞模式下实现蜂鸣器控制功能
@INPUT：BEEP管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 每固定时间(目前1Ms)调度一次,为了兼容无源蜂鸣器，1kHz

****************************************************************/
extern void Beep_TimedProcess(BEEP_MAG *pMag) ;


/****************************************************************
@FUNCTION：非阻塞模式下蜂鸣器控制功能初始化
@INPUT：硬件接口函数结构体，是否是无源蜂鸣器(TRUE-无源)，BEEP管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 根据蜂鸣器硬件操作相关函数，初始化硬件和关闭蜂鸣器，暂停蜂鸣器功能
****************************************************************/
extern void Beep_LL_Init(BEEP_HW_OPERATIONS a_sHwOps, u32 a_IsPassiveBeep, BEEP_MAG *pMag);


/****************************************************************
@FUNCTION：蜂鸣器鸣叫
@INPUT：BEEP管理结构体指针，操作模式，震动延迟
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:不支持阻塞模式下无源蜂鸣器鸣叫操作
****************************************************************/
extern void Beep_On(BEEP_MAG *pMag, OPT_MODE OptMode, uint16_t SetTime);

/****************************************************************
@FUNCTION：蜂鸣器关闭
@INPUT：BEEP管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
extern void Beep_Off(BEEP_MAG *pMag, OPT_MODE OptMode);


#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
