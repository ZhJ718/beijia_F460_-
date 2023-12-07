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

#define NBEE_FRAME_TYPE_ATCMD		0x08	//AT ָ���� (AT Command Frame)			
#define NBEE_FRAME_TYPE_ZBTREQ		0x10	//��������( ZigBee Transmit Request )	
#define NBEE_FRAME_TYPE_EXZBTREQ	0x11	//�߼���������(Explicit ZigBee Transmit Request )	
#define NBEE_FRAME_TYPE_ATCMDRSP	0x88	//AT ָ��Ӧ�� (AT Command Response)
#define NBEE_FRAME_TYPE_ZBTRSP		0x8B	//����������ָʾ(ZigBee Transmit Status)
#define NBEE_FRAME_TYPE_ZBR			0x90	//��������ָʾ (ZigBee Receive Packet AO=0)
#define NBEE_FRAME_TYPE_EXZBR		0x91	//�߼���������ָʾ(ZigBee Explicit RX Packet AO=1)
#define NBEE_FRAME_TYPE_MSTA		0x8A	//ģ��״̬ (Modem Status)

//ģ��״ֵ̬
#define NBEE_STA_HARDWARE_RESET			0x0
#define NBEE_STA_WATCHDOG_RESET 		0x1
#define NBEE_STA_JOINED_NETWORK			0x2
#define NBEE_STA_DISASSOCIATED			0x3
#define NBEE_STA_COORDINATOR_STARTED	0x6


//����
typedef struct
{
    //Ӳ����ʼ������������IO�ڳ�ʼ�������ڵ�
    void (*hw_init)(void);

    //Ӳ����λ��������
    void (*gpio_reset)(void);
    //Ӳ����λ��������
    void (*gpio_set)(void);

    //������
    uint32_t (*read)(uint8_t *, uint16_t *);
    //д����
    uint32_t (*write)(uint8_t *, uint16_t );

} NBEE_HW_OPERATIONS;

typedef struct
{
    //
    uint8_t		m_byIsReset: 1;		//TRUE-��λ��
    uint8_t		m_byIsCommunicationSucc: 1;
    uint8_t 	m_byRes: 6;

    uint32_t	m_dwNoCommunicationTImeout;

    uint8_t 	m_byResetSetCnt;	//�趨�ĸ�λʱ��
    uint8_t 	m_byResetNowCnt;	//��ǰ����ɸ�λʱ��

    //ʵ��Nbee���Ӳ���������ɶ�д
    NBEE_HW_OPERATIONS m_sHwOps;


} NBEE_LOWLEVEL_MAG;


/****************************************************************
@FUNCTION��NBEEϵ��Zigbeeģ��ײ�Ӳ����ʼ��
@INPUT��Ӳ���ӿں����ṹ�壬BLE����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �󶨵ײ�Ӳ��������������ʼ������ס��λ����
****************************************************************/
extern void NBee_LowLevelInit(NBEE_HW_OPERATIONS a_sHwOps, NBEE_LOWLEVEL_MAG *pMag);


/****************************************************************
@FUNCTION��NBEEϵ��Zigbee����ģ�鸴λ
@INPUT��NBEE�ײ����ṹ��ָ�룬��λ������>0��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��λʱ��=NBee_LowLevelTimedProcess���ȼ�� * ��λ����
		�˺�����λ����������Ч������λ����Ϊ0����ģ�鸴λһ����λ�ĵ��ȼ��
****************************************************************/
void NBee_Reset(NBEE_LOWLEVEL_MAG *pMag, uint8_t Timer);



/****************************************************************
@FUNCTION��NBEEϵ��Zigbeeģ�鸴λ������ͣ
@INPUT��NBEE�ײ����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �˺���������Ч
****************************************************************/
void NBee_ResetSuspend(NBEE_LOWLEVEL_MAG *pMag);



/****************************************************************
@FUNCTION��NBEEϵ��Zigbeeģ�鸴λ�����ָ�
@INPUT��NBEE�ײ����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ����ʱ��λ�����Ѿ�����������Ӱ��
****************************************************************/
void NBee_ResetRecovery(NBEE_LOWLEVEL_MAG *pMag);



/****************************************************************
@FUNCTION��NBEEϵ��Zigbeeģ��ײ㶨ʱ���Ƚ���
@INPUT��NBEE�ײ����ṹ��ָ��
@OUTPUT����
@RETURN: TRUE-����ִ�еײ㶨ʱ���ȣ� FALSE-�޵ײ㶨ʱ���ȣ���ִ���ϲ�Ӧ�ö�ʱ����
@AUTHOR��xfw
@SPECIAL: ʵ��ģ�鸴λ
****************************************************************/
extern uint32_t NBee_LowLevelTimedProcess(NBEE_LOWLEVEL_MAG *pMag);



typedef struct _NB_ApiFrame_t_
{
    uint8_t	u8Start;		//��ʼ��
    uint16_t	u16Length;		//����
    //uint8_t 	u8ApiID;		//֡��ʶ
    //uint8_t 	u8ApiDataLen;	//֡��������
    uint8_t	u8ApiData[MAX_PAYLOAD_BYTES];	//֡����
    uint8_t	u8Fcs;			//֡У��
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
    uint8_t	u8ExPanid[8];		//64 λĿ���ַ
    uint8_t	u8Panid[2];		//

    uint8_t	u8BroadcastRadius;	//���㲥�뾶�� 0 ����ʹ�����Ĺ㲥�뾶
    uint8_t	u8Options;		//ͨ��ѡ��,Ĭ��0
    uint8_t	u8DataLen;
    uint8_t	u8Data[0x60]; 	//�غ�

} NB_ApiFrameStruct_ZBTREQ_t;

typedef struct _NB_ApiFrame_ZBR_t_
{
    uint8_t	u8SrcAddr_64bit[8];		//���ݷ��ͽڵ�� 64 λ��ַ
    uint8_t	u8SrcAddr_16bit[2];		//���ݷ��ͽڵ�� 16 λ��ַ

    uint8_t	u8Options;		//����ѡ�� 0x01 - Packet Acknowledged  0x02 - Packet was a broadcast packet
    uint16_t	u16DataLen;
    uint8_t	u8Data[0x60]; 	//�غ�

} NB_ApiFrameStruct_ZBR_t;

typedef struct _NB_ApiFrame_ZBTRSP_t_
{
    uint8_t	u8FrameID;		//
    uint8_t	u8DstAddr[2];		//

    uint8_t	u8RetryCnt;		//�ش�����
    uint8_t	u8DeliveryStatus;		//����״̬ 0-success
    uint8_t	u8DiscoveryStatus;		//����״̬  0x00 - No Discovery overhead

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


//�жϻ����������Ƿ����NBeeЭ��Ҫ��
uint32_t NBee_FindFrame(uint8_t *pBuf, u16 *pLen, uint32_t *pIsCutted);

//����֡,��Ҫ����֡�ṹ�壬���ĺͳ���
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
