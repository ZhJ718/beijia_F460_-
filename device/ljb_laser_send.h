/**
  ******************************************************************************
  * @file    ljb_laser_send.h
  * @author  xfw
  * @brief   This file contains definitions for laser sending
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2021 Beijing HuaRu Tech Co.</center></h2>
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
#ifndef __LJB_LASER_SEND_H
#define __LJB_LASER_SEND_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "config.h"
#include "ljb_laser.h"

#define SELF_CHECK_BYTE 0x55

/*
	LJB新标准:高电平持续时间恒定2-3us，只是低电平持续时间不一样
	内部晶振 22.1184MHz,1us计数器比较值为22.1184
	每个编码分为6个部分，高电平+低电平，而且高电平持续时间一样，这样每个编码只需要记录下低电平持续时间
*/

#define LJB_LASER_TIMER_CLOCK_MHZ	42 //定时器时钟(单位：MHZ)

#define LJB_LASER_SEND_HIGH_DURATION	3	//高电平默认脉宽
#define LJB_LASER_SEND_BASED_FREQ	48000	//激光调制频率
#define LJB_LASER_SEND_LOW_CNT_STEP	6	//编码时序中每个码包含的低电平次数
#define LJB_LASER_SEND_STEP_CNT		12	//编码时序中包含的阶段数

#if 0
#define LJB_LASER_SEND_CMP_CALIBRATION	40	//修正值(由于程序执行耽误的时间)
#define HIGH_CMP	(66-LJB_LASER_SEND_CMP_CALIBRATION)
#define LOW_1T_CMP	(394-LJB_LASER_SEND_CMP_CALIBRATION)
#define LOW_2T_CMP	(854-LJB_LASER_SEND_CMP_CALIBRATION)
#define LOW_3T_CMP	(1314-LJB_LASER_SEND_CMP_CALIBRATION)
#define LOW_4T_CMP	(1774-LJB_LASER_SEND_CMP_CALIBRATION)
#define LOW_5T_CMP	(2234-LJB_LASER_SEND_CMP_CALIBRATION)
#endif

//计数时钟42MHz
#define LJB_LASER_SEND_CMP_CALIBRATION	108	//修正值(由于程序执行耽误的时间)
#define HIGH_CMP	(126 - LJB_LASER_SEND_CMP_CALIBRATION)	//高电平取3us  (3*48 - LJB_LASER_SEND_CMP_CALIBRATION)
#define LOW_1T_CMP	(748-LJB_LASER_SEND_CMP_CALIBRATION+15)	//(1*20.8-3) * LJB_LASER_TIMER_CLOCK_MHZ - LJB_LASER_SEND_CMP_CALIBRATION
#define LOW_2T_CMP	(1621-LJB_LASER_SEND_CMP_CALIBRATION+30)	//(2*20.8-3) * LJB_LASER_TIMER_CLOCK_MHZ - LJB_LASER_SEND_CMP_CALIBRATION
#define LOW_3T_CMP	(2495-LJB_LASER_SEND_CMP_CALIBRATION+45)	//(3*20.8-3) * LJB_LASER_TIMER_CLOCK_MHZ - LJB_LASER_SEND_CMP_CALIBRATION
#define LOW_4T_CMP	(3368-LJB_LASER_SEND_CMP_CALIBRATION+60)	//(4*20.8-3) * LJB_LASER_TIMER_CLOCK_MHZ - LJB_LASER_SEND_CMP_CALIBRATION
#define LOW_5T_CMP	(4242-LJB_LASER_SEND_CMP_CALIBRATION+75)	//(5*20.8-3) * LJB_LASER_TIMER_CLOCK_MHZ - LJB_LASER_SEND_CMP_CALIBRATION



//硬件操作
typedef struct
{
    //硬件初始化操作，包含IO口初始化、串口等
    void (*hw_init)(void);
    //发射口置高，不能为空
    void (*send_gpio_high)(void);
    //发射口置低，不能为空
    void (*send_gpio_low)(void);
    //打开定时，不能为空
    void (*open_timing)(uint16_t);
    //关闭定时，不能为空
    void (*close_timing)(void);
} LJB_LASER_SEND_HW_OPERATIONS;


typedef struct
{
    uint8_t m_IsHigh;	//数据线状态 0-低电平 1-高电平
    uint8_t m_byRes;		//保留
    uint16_t m_wDelayCmpCnt;//定时器计数值
} LJB_LASER_SEND_STEP_MAG;

typedef struct
{
    LJB_LASER_SEND_STEP_MAG m_saStep[LJB_LASER_SEND_STEP_CNT];
} LJB_LASER_SEND_ENCODE_MAG;


typedef struct
{
    LJB_LASER_SEND_ENCODE_MAG *m_psEncode;
    uint8_t m_byNowStep;	//当前发送到哪步
    uint8_t m_byMaxStep;	//最大步数
} LJB_LASER_SEND_SINGLE_ENCODE;

#define LJB_LASER_SEND_REPEAT_CNT 3

typedef struct
{
    LJB_LASER_SEND_HW_OPERATIONS m_sHwOps;


    uint8_t m_IsRun: 1;	//TRUE-正在发送中
    uint8_t m_IsEnableIRT1: 1; ////TRUE-使能IRT1
    uint8_t m_IsEnableIRT2: 1; ////TRUE-使能IRT2
    uint8_t m_IsEnableIRT3: 1; ////TRUE-使能IRT3
    uint8_t m_IsEnableIRT4: 1; ////TRUE-使能IRT4
    uint8_t m_IsRes: 2;

    LJB_LASER_SEND_SINGLE_ENCODE m_sSingle;//编码分步数据格式

    uint8_t m_bySendLen;	//需要发送编码数（1字节->2编码）
    uint8_t m_bySentLen;	//已发送编码数
    uint8_t m_bySendBuf[LJB_LASER_SEND_LEN_MAX];
    uint8_t m_byRepeatCnt;	//重复次数
    uint16_t m_wLaserIntensity;	//激光发射强度0-500  ，单位0.01
} LJB_LASER_SEND_MAG;



static const LJB_LASER_SEND_ENCODE_MAG gs_LaserSendEncode[ENCODE_MAX] =
{
    //ENCODE_HIGH_0000 	2 3 3 1 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
//ENCODE_HIGH_0001	4 1 2 2 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_4T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_0010	4 2 2 1 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_4T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_0011	3 2 3 1 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_0100	2 2 2 3 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_0101	3 1 3 2 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_0110	2 1 2 1 4 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_4T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_0111	2 2 1 2 3 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_1000	2 1 3 3 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_1001 	3 1 2 3 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_1010	3 3 2 1 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_1011	2 2 1 3 2 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_1100 	2 1 3 2 2 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_1101	3 2 1 3 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_1110	2 2 2 1 3 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_HIGH_1111	3 2 2 2 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},

    //ENCODE_LOW_0000	2 3 2 2 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_0001	4 1 1 1 3 1
    {1, 0, HIGH_CMP, 0, 0, LOW_4T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_0010	2 2 1 4 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_4T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_0011	2 1 4 2 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_4T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_0100	2 1 2 2 3 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_0101	4 1 3 1 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_4T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_0110	2 4 2 1 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_4T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_0111	5 2 1 1 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_5T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_1000	2 4 1 2 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_4T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_1001	3 3 1 2 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_1010	3 4 1 1 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_4T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_1011	2 1 1 5 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_5T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_1100	4 3 1 1 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_4T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_1101	2 5 1 1 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_5T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_1110	2 2 4 1 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_4T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_LOW_1111	2 3 1 3 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},

    //ENCODE_HEAD	2 1 5 1 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_5T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
    //ENCODE_TAIL	2 2 3 2 1 1
    {1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_3T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_2T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP, 1, 0, HIGH_CMP, 0, 0, LOW_1T_CMP},
};

extern LJB_LASER_SEND_MAG g_sLaserSendMag;

//激光发射过程，定时器中断调用
void LaserSend_Process(LJB_LASER_SEND_MAG *pMag);

void LaserSend_Start(LJB_LASER_SEND_MAG *pMag, uint8_t *pBuf, uint16_t Len);

void LaserSend_Init(LJB_LASER_SEND_MAG *pMag, LJB_LASER_SEND_HW_OPERATIONS HwOps);

uint8_t IsLaserSendIdle(LJB_LASER_SEND_MAG Mag);

//底层驱动部分
//void LaserSend_Gpio_Toggle(void);

void LaserSend_LowlevelInit(void);

void LaserSend_Gpio_On(void);

void LaserSend_Gpio_Off(void);

void LaserSendTimer_Open(uint16_t Val);

void LaserSendTimer_Close(void);



#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
