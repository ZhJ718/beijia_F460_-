/**
  ******************************************************************************
  * @file    motor.c
  * @author  xfw
  * @brief   This file contains definitions for motor
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
#include "motor.h"



/****************************************************************
@FUNCTION：非阻塞模式下马达控制功能暂停
@INPUT：MOTOR管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:暂停恢复后，马达继续振动规定的剩余时间
****************************************************************/
void Motor_Suspend(MOTOR_MAG *pMag)
{
    pMag->m_byIsStartMotor = FALSE;
}

/****************************************************************
@FUNCTION：非阻塞模式下马达控制功能是否暂停
@INPUT：MOTOR管理结构体
@OUTPUT：无
@RETURN: TRUE-暂停 FALSE-未暂停
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
u32 Motor_IsSuspend(MOTOR_MAG Mag)
{
    return (Mag.m_byIsStartMotor == FALSE ? TRUE : FALSE);
}

/****************************************************************
@FUNCTION：非阻塞模式下马达控制设置延时时间
@INPUT：MOTOR管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Motor_SetDelayTime(uint16_t a_wCnt, MOTOR_MAG *pMag)
{
    pMag->m_wSetCnt = a_wCnt;
}

/****************************************************************
@FUNCTION：非阻塞模式下马达控制增加延时时间
@INPUT：MOTOR管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Motor_AddDelayTime(uint16_t a_wCnt, MOTOR_MAG *pMag)
{
    pMag->m_wSetCnt += a_wCnt;
}

/****************************************************************
@FUNCTION：非阻塞模式下马达控制功能恢复
@INPUT：MOTOR管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Motor_Recovery(MOTOR_MAG *pMag)
{
    pMag->m_byIsStartMotor = TRUE;
}

/****************************************************************
@FUNCTION：实现非阻塞模式下马达控制功能
@INPUT：MOTOR管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 每固定时间(目前100Ms)调度一次

****************************************************************/
extern void Motor_TimedProcess(MOTOR_MAG *pMag)
{
    DDL_ASSERT(pMag != NULL);

    if(TRUE == Motor_IsSuspend(*pMag))
        return;

    if(pMag->m_wNowCnt <= pMag->m_wSetCnt)
    {
        pMag->m_sHwOps.motor_on();
        pMag->m_wNowCnt++;
    }
    else
    {
        Motor_Suspend(pMag);
        pMag->m_sHwOps.motor_off();
    }
}

/****************************************************************
@FUNCTION：马达管理结构体初始化
@INPUT：MOTOR管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 马达硬件操作需提前定义
			@@ Motor_GpioInit
			@@ Motor_GpioOn
			@@ Motor_GpioOff
****************************************************************/
void Motor_StructInit(PTRFUNC a_pGpioInitFunc, PTRFUNC a_pGpioOnFunc, PTRFUNC a_pGpioOffFunc, MOTOR_MAG *pMag)
{
    DDL_ASSERT(pMag != NULL);

    memset(pMag, 0, sizeof(MOTOR_MAG));

    pMag->m_sHwOps.motor_hw_init = a_pGpioInitFunc;
    pMag->m_sHwOps.motor_on = a_pGpioOnFunc;
    pMag->m_sHwOps.motor_off = a_pGpioOffFunc;

}


/****************************************************************
@FUNCTION：阻塞模式下马达控制功能初始化
@INPUT：硬件接口初始化函数，接口打开函数，接口关闭函数，MOTOR管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 根据马达硬件操作相关函数，初始化硬件和关闭马达，暂停马达功能
****************************************************************/
extern void Motor_LL_Init(MOTOR_HW_OPERATIONS a_sHwOps, MOTOR_MAG *pMag)
{
    DDL_ASSERT(pMag != NULL);

    memset(pMag, 0, sizeof(MOTOR_MAG));

    pMag->m_sHwOps.motor_hw_init = a_sHwOps.motor_hw_init;
    pMag->m_sHwOps.motor_on = a_sHwOps.motor_on;
    pMag->m_sHwOps.motor_off = a_sHwOps.motor_off;

    Motor_Suspend(pMag);

    if(pMag->m_sHwOps.motor_hw_init != NULL)
    {
        pMag->m_sHwOps.motor_hw_init();
    }

    if(pMag->m_sHwOps.motor_off != NULL)
    {
        pMag->m_sHwOps.motor_off();
    }
}


/****************************************************************
@FUNCTION：马达震动
@INPUT：MOTOR管理结构体指针，操作模式，震动延迟
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:非阻塞模式下，实际马达震动有延迟(0-定时调度间隔内),指定延时后主动关闭马达
****************************************************************/
extern void Motor_On(MOTOR_MAG *pMag, OPT_MODE OptMode, uint16_t SetTime)
{
    if(OptMode == OPT_MODE_BLOCK)
    {
        pMag->m_sHwOps.motor_on();
    }
    else
    {
        if(TRUE == Motor_IsSuspend(*pMag))
        {
            Motor_SetDelayTime(SetTime, pMag);
            pMag->m_wNowCnt = 0;
        }
        else
        {
            Motor_AddDelayTime(SetTime, pMag);
        }

        Motor_Recovery(pMag);
    }
}

/****************************************************************
@FUNCTION：马达关闭
@INPUT：MOTOR管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
extern void Motor_Off(MOTOR_MAG *pMag, OPT_MODE OptMode)
{
    pMag->m_sHwOps.motor_off();

    if(OptMode != OPT_MODE_BLOCK)
    {
        Motor_Suspend(pMag);
    }

}


/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
