/**
  ******************************************************************************
  * @file    soft_rtc.h
  * @author  xfw
  * @brief   This file contains definitions for soft RTC
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
#ifndef __SOFT_RTC_H
#define __SOFT_RTC_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/

#include "timer.h"


typedef struct
{
    TIMER_MAG m_sTime;	//��ǰʵʱʱ��
    uint32_t m_dwSecondOffset;//����RTCʱϵͳ��1970����������������
    uint32_t m_dwSysCnt;//����RTCʱϵͳ��������
} SOFT_RTC_MAG;


extern SOFT_RTC_MAG g_sRtcMag;


/****************************************************************
@FUNCTION�����ʵʱʱ�ӳ�ʼ��
@INPUT����
@OUTPUT��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:����STM32F103��RTC���׳����⣬���һ��������������ϵ�RTC
		���� g_sRtcMag
****************************************************************/
extern void SoftRTC_Init(void);

/****************************************************************
@FUNCTION�������ʱ��1s��ʱ����
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern void SoftRTC_TimedTask_1S(void);

/****************************************************************
@FUNCTION����ȡ��ǰʱ��
@INPUT��TIMER_MAG
@OUTPUT��TIMER_MAG����
@RETURN: 0,�ɹ�;����:�������.
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern uint32_t SoftRTC_Get(TIMER_MAG *pDate);


/****************************************************************
@FUNCTION�����õ�ǰʱ��
@INPUT��TIMER_MAG
@OUTPUT����
@RETURN: 0,�ɹ�;����:�������.
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern uint32_t SoftRTC_Set(TIMER_MAG *pDate);


#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
