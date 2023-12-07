/**
  ******************************************************************************
  * @file    new_zigbee.h
  * @author  xfw
  * @brief   This file contains definitions for XY's new zigbee module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2023 Beijing HuaRu Tech Co.</center></h2>
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
#ifndef __NEW_ZIGBEE_H
#define __NEW_ZIGBEE_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "config.h"

typedef enum
{
    ZIGBEE_TXPWR_N25DB = 0,	// -25.2DB
    ZIGBEE_TXPWR_N19DB,		// -19.3DB
    ZIGBEE_TXPWR_N15DB,		// -15.9DB
    ZIGBEE_TXPWR_N13DB,		// -13.3DB
    ZIGBEE_TXPWR_N11DB,	// -11.4DB
    ZIGBEE_TXPWR_N10DB,	// -10DB
    ZIGBEE_TXPWR_N8DB,	// -8.5DB
    ZIGBEE_TXPWR_N7DB,	// -7.5DB
    ZIGBEE_TXPWR_N5DB,	// -5DB
    ZIGBEE_TXPWR_N2DB,	// -2.5DB
    ZIGBEE_TXPWR_0DB,	// 0DB
    ZIGBEE_TXPWR_2DB,// 2.4DB
    ZIGBEE_TXPWR_5DB,	// 5DB
    ZIGBEE_TXPWR_6DB,	// 6.6DB
    ZIGBEE_TXPWR_8DB,	// 8.5DB
    ZIGBEE_TXPWR_10DB,	// 10.5DB
} ENUM_NEW_ZIGBEE_TXPWR;


typedef enum
{
    ZIGBEE_RCHAN_2405MHZ = 0x0B,
    ZIGBEE_RCHAN_2410MHZ,
    ZIGBEE_RCHAN_2415MHZ,
    ZIGBEE_RCHAN_2420MHZ,
    ZIGBEE_RCHAN_2425MHZ,
    ZIGBEE_RCHAN_2430MHZ,
    ZIGBEE_RCHAN_2435MHZ,
    ZIGBEE_RCHAN_2440MHZ,
    ZIGBEE_RCHAN_2445MHZ,
    ZIGBEE_RCHAN_2450MHZ,
    ZIGBEE_RCHAN_2455MHZ,
    ZIGBEE_RCHAN_2460MHZ,
    ZIGBEE_RCHAN_2465MHZ,
    ZIGBEE_RCHAN_2470MHZ,
    ZIGBEE_RCHAN_2475MHZ,
} ENUM_NEW_ZIGBEE_RADIO_CHAN;


typedef struct
{
    uint16_t m_wPanID;  //PAN ID ，固定为0x0101
    uint16_t m_wSelfAddr; //模块自身短地址，可随意设置，不用更改
    uint8_t m_aSelfExtAddr[8]; //模块自身扩展地址
    uint16_t m_wTargetAddr; //目标短地址，通信不使用短地址，不用更改
    uint8_t m_aTargetExtAddr[8]; //不用更改
    uint8_t m_byRadioChannel; //（0B-1A） ，固定为0x0B，2405MHz
    uint8_t m_byBaudRate; //（01-05） ，固定，不用更改 ，默认05：115200
    uint8_t m_byRes1; //保留，写入01
    uint8_t m_byTxPwr; //发射强度，写入0F  ，对应发射功率参考ENUM_NEW_ZIGBEE_TXPWR
    uint8_t m_byRes2; //保留，写入01
    uint8_t m_byRes3; //保留，写入01
} NEW_ZIGBEE_MOD_PARAM;

//硬件操作
typedef struct
{
    //硬件初始化操作，包含IO口初始化、串口等
    void (*hw_init)(void);
    //电源使能
    void (*gpio_power_on)(void);
    //电源进制
    void (*gpio_power_off)(void);
    //硬件复位引脚拉低
    void (*gpio_reset)(void);
    //硬件复位引脚拉高
    void (*gpio_set)(void);

    //读操作
    uint32_t (*read)(uint8_t *, uint16_t *);
    //写操作
    uint32_t (*write)(uint8_t *, uint16_t );

} NEW_ZIGBEE_HW_OPERATIONS;



typedef struct
{
    //
    uint8_t		m_byIsReset: 1;		//TRUE-复位中
    uint8_t		m_byIsSoftReset: 1;		//TRUE-模块重启中
    uint8_t		m_byIsCommunicationSucc: 1; // TRUE-模块有响应
    uint8_t   m_byIsGetParam: 1;  // TRUE-模块获取参数
    uint8_t   m_byIsSetParam: 1;  // TRUE-模块设置参数
    uint8_t   m_byRes: 3;

    //uint32_t	m_dwNoCommunicationTimeout;

    uint8_t 	m_byResetSetCnt;	//设定的复位时长
    uint8_t 	m_byResetNowCnt;	//当前已完成复位时长
    uint8_t 	m_bySoftResetNowCnt;	//当前已完成重启等待时长
    uint8_t 	m_byGetParamNowCnt;	//设定的获取参数最小时间间隔
    uint8_t 	m_bySetParamNowCnt;	//设定的设置参数最小时间间隔

    //实现设备相关硬件操作，可读写
    COM_HW_OPERATIONS m_sHwOps;
    //模块参数
    NEW_ZIGBEE_MOD_PARAM m_sParam;
    //模块设置参数备份
    NEW_ZIGBEE_MOD_PARAM m_sSetParam;

} NEW_ZIGBEE_LOWLEVEL_MAG;


/****************************************************************
@FUNCTION：新一新zigbee模块复位
@INPUT：复位步长（>0）
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 复位时间=定时任务调度间隔 * 复位步长
		此函数复位操作立即生效，若复位步长为0，则模块复位一个单位的调度间隔
****************************************************************/
void NewZigbee_Reset(uint8_t Timer);

/****************************************************************
@FUNCTION：新一新zigbee模块复位操作暂停
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 此函数立即生效
****************************************************************/
void NewZigbee_ResetSuspend(void);

/****************************************************************
@FUNCTION：新一新zigbee模块复位操作恢复
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 若此时复位操作已经结束，则无影响
****************************************************************/
void NewZigbee_ResetRecovery(void);


/****************************************************************
@FUNCTION：新一新zigbee模块重启
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void NewZigbee_Restart(void);


/****************************************************************
@FUNCTION：新一新zigbee模块开始获取参数
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 只是开启判断，不一定立即会获取参数
****************************************************************/
void NewZigbee_StartGetParam(void);


/****************************************************************
@FUNCTION：新一新zigbee模块停止获取参数
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void NewZigbee_StopGetParam(void);


/****************************************************************
@FUNCTION：新一新zigbee模块开始设置参数
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void NewZigbee_StartSetParam(void);


/****************************************************************
@FUNCTION：新一新zigbee模块停止设置参数
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void NewZigbee_StopSetParam(void);

/****************************************************************
@FUNCTION：新一新zigbee模块开始软复位
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void NewZigbee_StartSoftReset(void);


/****************************************************************
@FUNCTION：新一新zigbee模块停止软复位
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void NewZigbee_StopSoftReset(void);


/****************************************************************
@FUNCTION：新一新zigbee模块发送指令
@INPUT：目的地地址，发送数据
@OUTPUT：无
@RETURN: 发送字节数
@AUTHOR：xfw
@SPECIAL: 无
****************************************************************/
uint16_t NewZigbee_Send(uint8_t *pDstAddr, uint8_t DstAddrLen, uint8_t *pBuf, uint16_t BufLen);

/****************************************************************
@FUNCTION：新一新zigbee模块组包发送指令
@INPUT：目的地地址，数据载荷和长度，报文缓存空间
@OUTPUT：报文缓存，报文长度
@RETURN: 报文长度
@AUTHOR：xfw
@SPECIAL: 无
****************************************************************/
uint16_t NewZigbee_MakeSendFrame(uint8_t *pDstAddr, uint8_t DstAddrLen, uint8_t *pDat, uint16_t DatLen, uint8_t *pBuf,uint16_t *pBufLen);


/****************************************************************
@FUNCTION：新一新zigbee模块设置发射功率
@INPUT：新地址
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void NewZigbee_SetSelfAddr(uint8_t *pAddr, uint8_t AddrLen);

/****************************************************************
@FUNCTION：新一新zigbee模块设置射频信道
@INPUT：信道选择
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:默认2405MHZ,别改！！！！
****************************************************************/
void NewZigbee_SetRadioChannel(ENUM_NEW_ZIGBEE_RADIO_CHAN a_enumChan);

/****************************************************************
@FUNCTION：新一新zigbee模块设置发射功率
@INPUT：发射功率
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void NewZigbee_SetTxPwr(ENUM_NEW_ZIGBEE_TXPWR a_enumTxPwr);

/****************************************************************
@FUNCTION：新一新zigbee模块底层定时调度进程
@INPUT：
@OUTPUT：无
@RETURN: TRUE-正在执行底层定时调度,  FALSE-无底层定时调度, 可执行上层应用定时调度
@AUTHOR：xfw
@SPECIAL: 实现模块复位
****************************************************************/
extern uint32_t NewZigbee_LowLevelTimedProcess(void);


/****************************************************************
@FUNCTION：新一新zigbee模块底层管理结构体句柄
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
NEW_ZIGBEE_LOWLEVEL_MAG *NewZigbee_GetLLMagHandle(void);

/****************************************************************
@FUNCTION：新一新zigbee模块底层硬件初始化
@INPUT：硬件接口函数结构体
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 绑定底层硬件操作函数，初始化，上电，拉住复位引脚
		具体型号为XY-MZA66A  XY-MZA66C
****************************************************************/
extern void NewZigbee_LowLevelInit(COM_HW_OPERATIONS a_sHwOps);



#ifdef __cplusplus
}
#endif

#endif /* NEW_ZIGBEE_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
