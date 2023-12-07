/**
  ******************************************************************************
  * @file    nbee.h
  * @author  xfw
  * @brief   This file contains definitions for newbit zigbee
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
#ifndef __NBEE_H
#define __NBEE_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/

#include "config.h"
#include "uart.h"
#include "gpio.h"

#define MAX_PAYLOAD_BYTES		0x54

#define NBEE_RET_SUCC 			0
#define NBEE_RET_PARAM_ERROR 	1
#define NBEE_RET_FAILURE 		2

#define NBEE_FRAME_HEADER		0x7E

#define NBEE_FRAME_TYPE_ATCMD		0x08	//AT 指令侦 (AT Command Frame)			
#define NBEE_FRAME_TYPE_ZBTREQ		0x10	//传输请求( ZigBee Transmit Request )	
#define NBEE_FRAME_TYPE_EXZBTREQ	0x11	//高级传输请求(Explicit ZigBee Transmit Request )	
#define NBEE_FRAME_TYPE_ATCMDRSP	0x88	//AT 指令应答 (AT Command Response)
#define NBEE_FRAME_TYPE_ZBTRSP		0x8B	//传输请求结果指示(ZigBee Transmit Status)
#define NBEE_FRAME_TYPE_ZBR			0x90	//接收数据指示 (ZigBee Receive Packet AO=0)
#define NBEE_FRAME_TYPE_EXZBR		0x91	//高级接收数据指示(ZigBee Explicit RX Packet AO=1)
#define NBEE_FRAME_TYPE_MSTA		0x8A	//模块状态 (Modem Status)

//模块状态值
#define NBEE_STA_HARDWARE_RESET			0x0
#define NBEE_STA_WATCHDOG_RESET 		0x1
#define NBEE_STA_JOINED_NETWORK			0x2
#define NBEE_STA_DISASSOCIATED			0x3
#define NBEE_STA_COORDINATOR_STARTED	0x6


//操作
typedef struct
{
    //硬件初始化操作，包含IO口初始化、串口等
    void (*hw_init)(void);

    //硬件复位引脚拉低
    void (*gpio_reset)(void);
    //硬件复位引脚拉高
    void (*gpio_set)(void);

    //读操作
    uint32_t (*read)(uint8_t *, uint16_t *);
    //写操作
    uint32_t (*write)(uint8_t *, uint16_t );

} NBEE_HW_OPERATIONS;

typedef struct
{
    //
    uint8_t		m_byIsReset: 1;		//TRUE-复位中
    uint8_t		m_byIsCommunicationSucc: 1;
    uint8_t 	m_byRes: 6;

    uint32_t	m_dwNoCommunicationTImeout;

    uint8_t 	m_byResetSetCnt;	//设定的复位时长
    uint8_t 	m_byResetNowCnt;	//当前已完成复位时长

    //实现Nbee相关硬件操作，可读写
    NBEE_HW_OPERATIONS m_sHwOps;


} NBEE_LOWLEVEL_MAG;


/****************************************************************
@FUNCTION：NBEE系列Zigbee模块底层硬件初始化
@INPUT：硬件接口函数结构体，BLE管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 绑定底层硬件操作函数，初始化，拉住复位引脚
****************************************************************/
extern void NBee_LowLevelInit(NBEE_HW_OPERATIONS a_sHwOps, NBEE_LOWLEVEL_MAG *pMag);


/****************************************************************
@FUNCTION：NBEE系列Zigbee蓝牙模块复位
@INPUT：NBEE底层管理结构体指针，复位步长（>0）
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 复位时间=NBee_LowLevelTimedProcess调度间隔 * 复位步长
		此函数复位操作立即生效，若复位步长为0，则模块复位一个单位的调度间隔
****************************************************************/
void NBee_Reset(NBEE_LOWLEVEL_MAG *pMag, uint8_t Timer);



/****************************************************************
@FUNCTION：NBEE系列Zigbee模块复位操作暂停
@INPUT：NBEE底层管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 此函数立即生效
****************************************************************/
void NBee_ResetSuspend(NBEE_LOWLEVEL_MAG *pMag);



/****************************************************************
@FUNCTION：NBEE系列Zigbee模块复位操作恢复
@INPUT：NBEE底层管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 若此时复位操作已经结束，则无影响
****************************************************************/
void NBee_ResetRecovery(NBEE_LOWLEVEL_MAG *pMag);



/****************************************************************
@FUNCTION：NBEE系列Zigbee模块底层定时调度进程
@INPUT：NBEE底层管理结构体指针
@OUTPUT：无
@RETURN: TRUE-正在执行底层定时调度， FALSE-无底层定时调度，可执行上层应用定时调度
@AUTHOR：xfw
@SPECIAL: 实现模块复位
****************************************************************/
extern uint32_t NBee_LowLevelTimedProcess(NBEE_LOWLEVEL_MAG *pMag);



typedef struct _NB_ApiFrame_t_
{
    uint8_t	u8Start;		//起始符
    uint16_t	u16Length;		//长度
    //uint8_t 	u8ApiID;		//帧标识
    //uint8_t 	u8ApiDataLen;	//帧参数长度
    uint8_t	u8ApiData[MAX_PAYLOAD_BYTES];	//帧参数
    uint8_t	u8Fcs;			//帧校验
} NB_ApiFrameStruct_t;

typedef enum
{
    ATCMD_DH = 0,
    ATCMD_DL,
    ATCMD_MY,
    ATCMD_MP,
    ATCMD_NC,
    ATCMD_SH,
    ATCMD_SL,
    ATCMD_NI,
    ATCMD_SE,
    ATCMD_DE,
    ATCMD_CI,
    ATCMD_NP,
    ATCMD_CH,
    ATCMD_CE,
    ATCMD_DA,
    ATCMD_ID,
    ATCMD_OP,
    ATCMD_NH,
    ATCMD_BH,
    ATCMD_OI,
    ATCMD_SC,
    ATCMD_SD,
    ATCMD_ZS,
    ATCMD_NJ,
    ATCMD_JV,
    ATCMD_DJ,
    ATCMD_II,

    ATCMD_EE,
    ATCMD_EO,
    ATCMD_NK,
    ATCMD_KY,

    ATCMD_PL,
    ATCMD_DB,
    ATCMD_PP,

    ATCMD_AP,
    ATCMD_AO,
    ATCMD_BD,
    ATCMD_NB,
    ATCMD_SB,
    ATCMD_RO,
    ATCMD_D7,
    ATCMD_D6,
    ATCMD_AS,

    ATCMD_VR,
    ATCMD_HV,
    ATCMD_AI,

    ATCMD_CT,
    ATCMD_CN,
    ATCMD_GT,
    ATCMD_CC,

    ATCMD_AC,
    ATCMD_WR,
    ATCMD_RE,
    ATCMD_FR,
    ATCMD_NR,
    ATCMD_CB,
    ATCMD_IS,
    ATCMD_UNKNOWN,
} NB_ATCMD_Enum_t;

typedef enum
{
    CE_COORDINATOR = 1,
    CE_ROUTER = 0,
} NB_Cmd_CE_Enum_t;

typedef struct _NB_ApiFrame_ATCMD_t_
{
    uint8_t	u8FrameID;		//
    uint8_t	u8CMD1;		//
    uint8_t	u8CMD2;		//
    uint8_t	u8ParaLen;
    uint8_t	u8Para[29];
} NB_ApiFrameStruct_ATCMD_t;

typedef enum
{
    CMD_STA_OK = 0,
    CMD_STA_ERROR,
    CMD_STA_INVALID_COMMAND,
    CMD_STA_INVALID_PARAM,
    CMD_STA_TX_FAILURE,
} NB_CmdStatus_Enum_t;

typedef struct _NB_ApiFrame_ATCMDRSP_t_
{
    uint8_t	u8FrameID;		//
    uint8_t	u8CMD1;		//
    uint8_t	u8CMD2;		//
    NB_CmdStatus_Enum_t	enumCmdStatus;		//
    uint8_t	u8ParaLen;
    uint8_t	u8Para[29];
    NB_ATCMD_Enum_t AtCmd;
} NB_ApiFrameStruct_ATCMDRSP_t;

typedef struct _NB_ApiFrame_MSTA_t_
{
    uint8_t	u8Status;
} NB_ApiFrameStruct_MSTA_t;

typedef struct _NB_ApiFrame_ZBTREQ_t_
{
    uint8_t	u8FrameID;		//
    uint8_t	u8ExPanid[8];		//64 位目标地址
    uint8_t	u8Panid[2];		//

    uint8_t	u8BroadcastRadius;	//最大广播半径， 0 代表使用最大的广播半径
    uint8_t	u8Options;		//通信选项,默认0
    uint8_t	u8DataLen;
    uint8_t	u8Data[0x60]; 	//载荷

} NB_ApiFrameStruct_ZBTREQ_t;

typedef struct _NB_ApiFrame_ZBR_t_
{
    uint8_t	u8SrcAddr_64bit[8];		//数据发送节点的 64 位地址
    uint8_t	u8SrcAddr_16bit[2];		//数据发送节点的 16 位地址

    uint8_t	u8Options;		//接收选项 0x01 - Packet Acknowledged  0x02 - Packet was a broadcast packet
    uint16_t	u16DataLen;
    uint8_t	u8Data[0x60]; 	//载荷

} NB_ApiFrameStruct_ZBR_t;

typedef struct _NB_ApiFrame_ZBTRSP_t_
{
    uint8_t	u8FrameID;		//
    uint8_t	u8DstAddr[2];		//

    uint8_t	u8RetryCnt;		//重传次数
    uint8_t	u8DeliveryStatus;		//交付状态 0-success
    uint8_t	u8DiscoveryStatus;		//发现状态  0x00 - No Discovery overhead

} NB_ApiFrameStruct_ZBTRSP_t;

typedef struct _NB_ApiFrame_ApiData_t_
{
    uint8_t	u8FrameType;
    union
    {
        NB_ApiFrameStruct_ATCMD_t sATCMD;
        NB_ApiFrameStruct_ATCMDRSP_t sATCMDRSP;
        NB_ApiFrameStruct_MSTA_t sMSTA;
        NB_ApiFrameStruct_ZBTREQ_t sZBTREQ;
        NB_ApiFrameStruct_ZBR_t sZBR;
        NB_ApiFrameStruct_ZBTRSP_t sZBTRSP;
    } Union;
} NB_ApiFrameStruct_ApiData_t;


//判断缓冲区内容是否符合NBee协议要求
uint32_t NBee_FindFrame(uint8_t *pBuf, u16 *pLen, uint32_t *pIsCutted);

//解析帧,需要传入帧结构体，报文和长度
uint32_t NBee_Frame_Parse(NB_ApiFrameStruct_t *pFrame, uint8_t *pBuf, uint16_t Len);

uint32_t NBee_Frame_Make_ATCMD(NB_ATCMD_Enum_t AtCmd, uint8_t *pFrameID, uint8_t *pParam, uint8_t ParamLen, uint8_t *pBuf, uint16_t *pLen);


uint32_t NBee_Frame_Make_AllType(NB_ApiFrameStruct_ApiData_t *pApiData, uint8_t *pBuf, uint16_t *pLen);

uint32_t NBee_Frame_Make_Broadcast(uint8_t FrameID, uint8_t *pData, uint8_t DataLen, uint8_t *pBuf, uint16_t *pLen);


uint32_t NBee_Frame_GetModemStatusFromApiData(NB_ApiFrameStruct_t Frame, NB_ApiFrameStruct_MSTA_t *pModemSta);

uint32_t NBee_Frame_GetAtCmdResponseFromApiData(NB_ApiFrameStruct_t Frame, NB_ApiFrameStruct_ATCMDRSP_t *pRsp);

uint32_t NBee_Frame_GetRecvPacketFromApiData(NB_ApiFrameStruct_t Frame, NB_ApiFrameStruct_ZBR_t *p);

uint32_t NBee_Frame_GetTransmitStatusFromApiData(NB_ApiFrameStruct_t Frame, NB_ApiFrameStruct_ZBTRSP_t *p);


#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
