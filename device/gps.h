/**
  ******************************************************************************
  * @file    gps.h
  * @author  xfw
  * @brief   This file contains definitions for gps module
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
#ifndef __GPS_H
#define __GPS_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "config.h"

typedef enum
{
    OUTPUT_NO = 0,
    OUTPUT_1,
    OUTPUT_2,
    OUTPUT_3,
    OUTPUT_4,
    OUTPUT_5,
    OUTPUT_6,
    OUTPUT_7,
    OUTPUT_8,
    OUTPUT_9,
    OUTPUT_UNCHANGE = 0xff,
} ENUM_NMEA_OUTPUT_FREQ;

typedef struct
{
    ENUM_NMEA_OUTPUT_FREQ m_eNGGA;
    ENUM_NMEA_OUTPUT_FREQ m_eNGLL;
    ENUM_NMEA_OUTPUT_FREQ m_eNGSA;
    ENUM_NMEA_OUTPUT_FREQ m_eNGSV;
    ENUM_NMEA_OUTPUT_FREQ m_eNRMC;
    ENUM_NMEA_OUTPUT_FREQ m_eNVTG;
    ENUM_NMEA_OUTPUT_FREQ m_eNZDA;
    ENUM_NMEA_OUTPUT_FREQ m_eNTXT;
} NMEA_OUTPUT_FREQ_PARAM;



typedef struct
{

    uint8_t	m_byIsReset: 1;		//TRUE-复位中
    uint8_t 	m_byIsCommunicationSucc: 1; //TRUE-GPS通信正常
    uint8_t 	m_byIsSetParam: 1;  // TRUE-模块设置参数
    uint8_t 	m_byRes: 5;

    uint32_t	m_dwNoCommunicationTImeout;

    uint8_t 	m_byResetSetCnt;	//设定的复位时长
    uint8_t 	m_byResetNowCnt;	//当前已完成复位时长
    uint8_t 	m_bySetParamNowCnt;	//设定的设置参数最小时间间隔


    //实现GPS相关硬件操作，可读写
    COM_HW_OPERATIONS m_sHwOps;

    //设置参数
    NMEA_OUTPUT_FREQ_PARAM m_sParam;

} GPS_LOWLEVEL_MAG;


/****************************************************************
@FUNCTION：中科微332D GPS初始化
@INPUT：硬件接口函数结构体，GPS管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 绑定底层硬件操作函数，初始化，拉高复位引脚，控制各语句输出频率
****************************************************************/
extern void Gps_LowLevelInit(COM_HW_OPERATIONS a_sHwOps);


/****************************************************************
@FUNCTION：模块复位
@INPUT：复位步长（>0）
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 复位时间=定时任务调度间隔 * 复位步长
		此函数复位操作立即生效，若复位步长为0，则模块复位一个单位的调度间隔
****************************************************************/
void Gps_Reset(uint8_t Timer);

/****************************************************************
@FUNCTION：模块复位操作暂停
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 此函数立即生效
****************************************************************/
void Gps_ResetSuspend(void);


/****************************************************************
@FUNCTION：模块复位操作恢复
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 若此时复位操作已经结束，则无影响
****************************************************************/
void Gps_ResetRecovery(void);


/****************************************************************
@FUNCTION：模块开始设置参数
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 调用后异步执行，不用等回复
****************************************************************/
void Gps_StartSetParam(NMEA_OUTPUT_FREQ_PARAM SetParam);

/****************************************************************
@FUNCTION：模块底层定时调度进程
@INPUT：
@OUTPUT：无
@RETURN: TRUE-正在执行底层定时调度,  FALSE-无底层定时调度, 可执行上层应用定时调度
@AUTHOR：xfw
@SPECIAL: 实现模块复位
****************************************************************/
extern uint32_t Gps_LowLevelTimedProcess(void);


GPS_LOWLEVEL_MAG *Gps_GetLLMagHandle(void);


#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
