/**
  ******************************************************************************
  * @file    motor.h
  * @author  xfw
  * @brief   This file contains definitions for MOTOR
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
#ifndef __MOTOR_H
#define __MOTOR_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "config.h"

//MOTOR硬件操作
typedef struct
{
    //Motor硬件接口初始化函数
    void (*motor_hw_init)(void);
    //Motor开操作
    void (*motor_on)(void);
    //Motor关操作
    void (*motor_off)(void);
} MOTOR_HW_OPERATIONS;

typedef struct
{
    //实现Motor相关异步操作逻辑，只读
    uint8_t		m_byIsStartMotor: 1;
    uint8_t		m_byRes: 7;
    uint16_t 	m_wSetCnt;	//延时设置值
    uint16_t 	m_wNowCnt;	//已经延时时间
    //实现Motor相关硬件操作，可读写
    MOTOR_HW_OPERATIONS m_sHwOps;
} MOTOR_MAG;


/****************************************************************
@FUNCTION：非阻塞模式下马达控制功能暂停
@INPUT：MOTOR管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:暂停恢复后，马达继续振动规定的剩余时间
****************************************************************/
void Motor_Suspend(MOTOR_MAG *pMag);

/****************************************************************
@FUNCTION：非阻塞模式下马达控制功能是否暂停
@INPUT：MOTOR管理结构体
@OUTPUT：无
@RETURN: TRUE-暂停 FALSE-未暂停
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
u32 Motor_IsSuspend(MOTOR_MAG Mag);

/****************************************************************
@FUNCTION：非阻塞模式下马达控制功能恢复
@INPUT：MOTOR管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Motor_Recovery(MOTOR_MAG *pMag);


/****************************************************************
@FUNCTION：非阻塞模式下马达控制功能逻辑
@INPUT：MOTOR管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 每固定时间(目前100Ms)调度一次

****************************************************************/
extern void Motor_TimedProcess(MOTOR_MAG *pMag) ;


/****************************************************************
@FUNCTION：阻塞模式下马达控制功能初始化
@INPUT：硬件接口初始化函数，接口打开函数，接口关闭函数，MOTOR管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 根据马达硬件操作相关函数，初始化硬件和关闭马达，暂停马达功能
****************************************************************/
extern void Motor_LL_Init(MOTOR_HW_OPERATIONS a_sHwOps, MOTOR_MAG *pMag);

/****************************************************************
@FUNCTION：马达震动
@INPUT：MOTOR管理结构体指针，操作模式，震动延迟
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:非阻塞模式下，实际马达震动有延迟(0-定时调度间隔内)
****************************************************************/
extern void Motor_On(MOTOR_MAG *pMag, OPT_MODE OptMode, uint16_t SetTime);

/****************************************************************
@FUNCTION：马达关闭
@INPUT：MOTOR管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
extern void Motor_Off(MOTOR_MAG *pMag, OPT_MODE OptMode);


#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
