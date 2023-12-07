/**
  ******************************************************************************
  * @file    fume.h
  * @author  xfw
  * @brief   This file contains definitions for fume
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
#ifndef __FUME_H
#define __FUME_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "config.h"



//FUME硬件操作
typedef struct
{
    //FUME硬件接口初始化函数
    void (*hw_init)(void);
    //FUME开操作
    void (*on)(void);
    //FUME关操作
    void (*off)(void);
    //FUME安装检测操作
    uint32_t (*is_installed)(void);
} FUME_HW_OPERATIONS;


typedef struct
{
    //实现Motor相关异步操作逻辑，只读
    uint8_t		m_byIsStart: 1;
    uint8_t		m_byRes: 7;

    uint16_t 	m_wSetCnt;	//延时设置值
    uint16_t 	m_wNowCnt;	//已经延时时间

    //实现Motor相关硬件操作，可读写
    FUME_HW_OPERATIONS m_sHwOps;
} FUME_MAG;

/****************************************************************
@FUNCTION：实现非阻塞模式下发烟罐控制功能
@INPUT：FUME管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 每固定时间(目前100Ms)调度一次

****************************************************************/
extern void Fume_TimedProcess(FUME_MAG *pMag) ;


/****************************************************************
@FUNCTION：阻塞模式下发烟罐控制功能初始化
@INPUT：硬件接口初始化函数，接口打开函数，接口关闭函数，FUME管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 根据发烟罐硬件操作相关函数，初始化硬件和关闭发烟罐
****************************************************************/
extern void Fume_LL_Init(FUME_HW_OPERATIONS a_sHwOps, FUME_MAG *pMag);

/****************************************************************
@FUNCTION：非阻塞模式下发烟罐是否安装检测
@INPUT：FUME管理结构体
@OUTPUT：无
@RETURN: TRUE-已安装 FALSE-未安装
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint32_t Fume_IsInstalled(FUME_MAG Mag);


/****************************************************************
@FUNCTION：发烟罐发烟
@INPUT：FUME管理结构体指针，震动延迟
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:非阻塞模式下，实际马达震动有延迟(0-定时调度间隔内),指定延时后主动关闭马达
****************************************************************/
extern void Fume_On(FUME_MAG *pMag, uint16_t SetTime);


/****************************************************************
@FUNCTION：发烟罐发烟关闭
@INPUT：FUME管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
extern void Fume_Off(FUME_MAG *pMag);

#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
