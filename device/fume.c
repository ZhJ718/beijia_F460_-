/**
  ******************************************************************************
  * @file    fume.c
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

/* Includes ------------------------------------------------------------------*/
#include "fume.h"

/****************************************************************
@FUNCTION：非阻塞模式下发烟罐是否安装检测
@INPUT：FUME管理结构体
@OUTPUT：无
@RETURN: TRUE-已安装 FALSE-未安装
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint32_t Fume_IsInstalled(FUME_MAG Mag)
{
    if(Mag.m_sHwOps.is_installed != NULL)
    {
        return Mag.m_sHwOps.is_installed();
    }

    return 1;
}


/****************************************************************
@FUNCTION：非阻塞模式下发烟罐控制功能暂停
@INPUT：FUME管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:暂停恢复后，发烟罐继续规定的剩余时间
****************************************************************/
void Fume_Suspend(FUME_MAG *pMag)
{
    pMag->m_byIsStart = FALSE;
}

/****************************************************************
@FUNCTION：非阻塞模式下发烟罐控制功能是否暂停
@INPUT：FUME管理结构体
@OUTPUT：无
@RETURN: TRUE-暂停 FALSE-未暂停
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
u32 Fume_IsSuspend(FUME_MAG Mag)
{
    return (Mag.m_byIsStart == FALSE ? TRUE : FALSE);
}

/****************************************************************
@FUNCTION：非阻塞模式下发烟罐控制设置延时时间
@INPUT：FUME管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Fume_SetDelayTime(uint16_t a_wCnt, FUME_MAG *pMag)
{
    pMag->m_wSetCnt = a_wCnt;
}

/****************************************************************
@FUNCTION：非阻塞模式下发烟罐控制增加延时时间
@INPUT：FUME管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Fume_AddDelayTime(uint16_t a_wCnt, FUME_MAG *pMag)
{
    pMag->m_wSetCnt += a_wCnt;
}

/****************************************************************
@FUNCTION：非阻塞模式下发烟罐控制功能恢复
@INPUT：FUME管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Fume_Recovery(FUME_MAG *pMag)
{
    pMag->m_byIsStart = TRUE;
}

/****************************************************************
@FUNCTION：实现非阻塞模式下发烟罐控制功能
@INPUT：FUME管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 每固定时间(目前100Ms)调度一次

****************************************************************/
extern void Fume_TimedProcess(FUME_MAG *pMag)
{
    assert_param(pMag != NULL);

    if(TRUE == Fume_IsSuspend(*pMag))
        return;

    if(pMag->m_wNowCnt <= pMag->m_wSetCnt)
    {
        pMag->m_sHwOps.on();
        pMag->m_wNowCnt++;
    }
    else
    {
        Fume_Suspend(pMag);
        pMag->m_sHwOps.off();
    }
}


/****************************************************************
@FUNCTION：阻塞模式下发烟罐控制功能初始化
@INPUT：硬件接口初始化函数，接口打开函数，接口关闭函数，FUME管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 根据发烟罐硬件操作相关函数，初始化硬件和关闭发烟罐
****************************************************************/
extern void Fume_LL_Init(FUME_HW_OPERATIONS a_sHwOps, FUME_MAG *pMag)
{
    assert_param(pMag != NULL);

    memset(pMag, 0, sizeof(FUME_MAG));

    pMag->m_sHwOps.hw_init = a_sHwOps.hw_init;
    pMag->m_sHwOps.on = a_sHwOps.on;
    pMag->m_sHwOps.off = a_sHwOps.off;
    pMag->m_sHwOps.is_installed = a_sHwOps.is_installed;

    Fume_Suspend(pMag);

    if(pMag->m_sHwOps.hw_init != NULL)
    {
        pMag->m_sHwOps.hw_init();
    }

    if(pMag->m_sHwOps.off != NULL)
    {
        pMag->m_sHwOps.off();
    }
}


/****************************************************************
@FUNCTION：发烟罐发烟
@INPUT：FUME管理结构体指针，震动延迟
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:非阻塞模式下，实际点烟有延迟(0-定时调度间隔内),指定延时后主动关闭点烟器
****************************************************************/
extern void Fume_On(FUME_MAG *pMag, uint16_t SetTime)
{
    if(Fume_IsInstalled(*pMag))
    {
        if(TRUE == Fume_IsSuspend(*pMag))
        {
            Fume_SetDelayTime(SetTime, pMag);
            pMag->m_wNowCnt = 0;
        }
        else
        {
            Fume_AddDelayTime(SetTime, pMag);
        }

        Fume_Recovery(pMag);
    }
}

/****************************************************************
@FUNCTION：发烟罐发烟关闭
@INPUT：FUME管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
extern void Fume_Off(FUME_MAG *pMag)
{
    pMag->m_sHwOps.off();


    Fume_Suspend(pMag);

}
/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
