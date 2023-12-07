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
    uint16_t m_wPanID;  //PAN ID ���̶�Ϊ0x0101
    uint16_t m_wSelfAddr; //ģ������̵�ַ�����������ã����ø���
    uint8_t m_aSelfExtAddr[8]; //ģ��������չ��ַ
    uint16_t m_wTargetAddr; //Ŀ��̵�ַ��ͨ�Ų�ʹ�ö̵�ַ�����ø���
    uint8_t m_aTargetExtAddr[8]; //���ø���
    uint8_t m_byRadioChannel; //��0B-1A�� ���̶�Ϊ0x0B��2405MHz
    uint8_t m_byBaudRate; //��01-05�� ���̶������ø��� ��Ĭ��05��115200
    uint8_t m_byRes1; //������д��01
    uint8_t m_byTxPwr; //����ǿ�ȣ�д��0F  ����Ӧ���书�ʲο�ENUM_NEW_ZIGBEE_TXPWR
    uint8_t m_byRes2; //������д��01
    uint8_t m_byRes3; //������д��01
} NEW_ZIGBEE_MOD_PARAM;

//Ӳ������
typedef struct
{
    //Ӳ����ʼ������������IO�ڳ�ʼ�������ڵ�
    void (*hw_init)(void);
    //��Դʹ��
    void (*gpio_power_on)(void);
    //��Դ����
    void (*gpio_power_off)(void);
    //Ӳ����λ��������
    void (*gpio_reset)(void);
    //Ӳ����λ��������
    void (*gpio_set)(void);

    //������
    uint32_t (*read)(uint8_t *, uint16_t *);
    //д����
    uint32_t (*write)(uint8_t *, uint16_t );

} NEW_ZIGBEE_HW_OPERATIONS;



typedef struct
{
    //
    uint8_t		m_byIsReset: 1;		//TRUE-��λ��
    uint8_t		m_byIsSoftReset: 1;		//TRUE-ģ��������
    uint8_t		m_byIsCommunicationSucc: 1; // TRUE-ģ������Ӧ
    uint8_t   m_byIsGetParam: 1;  // TRUE-ģ���ȡ����
    uint8_t   m_byIsSetParam: 1;  // TRUE-ģ�����ò���
    uint8_t   m_byRes: 3;

    //uint32_t	m_dwNoCommunicationTimeout;

    uint8_t 	m_byResetSetCnt;	//�趨�ĸ�λʱ��
    uint8_t 	m_byResetNowCnt;	//��ǰ����ɸ�λʱ��
    uint8_t 	m_bySoftResetNowCnt;	//��ǰ����������ȴ�ʱ��
    uint8_t 	m_byGetParamNowCnt;	//�趨�Ļ�ȡ������Сʱ����
    uint8_t 	m_bySetParamNowCnt;	//�趨�����ò�����Сʱ����

    //ʵ���豸���Ӳ���������ɶ�д
    COM_HW_OPERATIONS m_sHwOps;
    //ģ�����
    NEW_ZIGBEE_MOD_PARAM m_sParam;
    //ģ�����ò�������
    NEW_ZIGBEE_MOD_PARAM m_sSetParam;

} NEW_ZIGBEE_LOWLEVEL_MAG;


/****************************************************************
@FUNCTION����һ��zigbeeģ�鸴λ
@INPUT����λ������>0��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��λʱ��=��ʱ������ȼ�� * ��λ����
		�˺�����λ����������Ч������λ����Ϊ0����ģ�鸴λһ����λ�ĵ��ȼ��
****************************************************************/
void NewZigbee_Reset(uint8_t Timer);

/****************************************************************
@FUNCTION����һ��zigbeeģ�鸴λ������ͣ
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �˺���������Ч
****************************************************************/
void NewZigbee_ResetSuspend(void);

/****************************************************************
@FUNCTION����һ��zigbeeģ�鸴λ�����ָ�
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ����ʱ��λ�����Ѿ�����������Ӱ��
****************************************************************/
void NewZigbee_ResetRecovery(void);


/****************************************************************
@FUNCTION����һ��zigbeeģ������
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void NewZigbee_Restart(void);


/****************************************************************
@FUNCTION����һ��zigbeeģ�鿪ʼ��ȡ����
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ֻ�ǿ����жϣ���һ���������ȡ����
****************************************************************/
void NewZigbee_StartGetParam(void);


/****************************************************************
@FUNCTION����һ��zigbeeģ��ֹͣ��ȡ����
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void NewZigbee_StopGetParam(void);


/****************************************************************
@FUNCTION����һ��zigbeeģ�鿪ʼ���ò���
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void NewZigbee_StartSetParam(void);


/****************************************************************
@FUNCTION����һ��zigbeeģ��ֹͣ���ò���
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void NewZigbee_StopSetParam(void);

/****************************************************************
@FUNCTION����һ��zigbeeģ�鿪ʼ��λ
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void NewZigbee_StartSoftReset(void);


/****************************************************************
@FUNCTION����һ��zigbeeģ��ֹͣ��λ
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void NewZigbee_StopSoftReset(void);


/****************************************************************
@FUNCTION����һ��zigbeeģ�鷢��ָ��
@INPUT��Ŀ�ĵص�ַ����������
@OUTPUT����
@RETURN: �����ֽ���
@AUTHOR��xfw
@SPECIAL: ��
****************************************************************/
uint16_t NewZigbee_Send(uint8_t *pDstAddr, uint8_t DstAddrLen, uint8_t *pBuf, uint16_t BufLen);

/****************************************************************
@FUNCTION����һ��zigbeeģ���������ָ��
@INPUT��Ŀ�ĵص�ַ�������غɺͳ��ȣ����Ļ���ռ�
@OUTPUT�����Ļ��棬���ĳ���
@RETURN: ���ĳ���
@AUTHOR��xfw
@SPECIAL: ��
****************************************************************/
uint16_t NewZigbee_MakeSendFrame(uint8_t *pDstAddr, uint8_t DstAddrLen, uint8_t *pDat, uint16_t DatLen, uint8_t *pBuf,uint16_t *pBufLen);


/****************************************************************
@FUNCTION����һ��zigbeeģ�����÷��书��
@INPUT���µ�ַ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void NewZigbee_SetSelfAddr(uint8_t *pAddr, uint8_t AddrLen);

/****************************************************************
@FUNCTION����һ��zigbeeģ��������Ƶ�ŵ�
@INPUT���ŵ�ѡ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:Ĭ��2405MHZ,��ģ�������
****************************************************************/
void NewZigbee_SetRadioChannel(ENUM_NEW_ZIGBEE_RADIO_CHAN a_enumChan);

/****************************************************************
@FUNCTION����һ��zigbeeģ�����÷��书��
@INPUT�����书��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void NewZigbee_SetTxPwr(ENUM_NEW_ZIGBEE_TXPWR a_enumTxPwr);

/****************************************************************
@FUNCTION����һ��zigbeeģ��ײ㶨ʱ���Ƚ���
@INPUT��
@OUTPUT����
@RETURN: TRUE-����ִ�еײ㶨ʱ����,  FALSE-�޵ײ㶨ʱ����, ��ִ���ϲ�Ӧ�ö�ʱ����
@AUTHOR��xfw
@SPECIAL: ʵ��ģ�鸴λ
****************************************************************/
extern uint32_t NewZigbee_LowLevelTimedProcess(void);


/****************************************************************
@FUNCTION����һ��zigbeeģ��ײ����ṹ����
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
NEW_ZIGBEE_LOWLEVEL_MAG *NewZigbee_GetLLMagHandle(void);

/****************************************************************
@FUNCTION����һ��zigbeeģ��ײ�Ӳ����ʼ��
@INPUT��Ӳ���ӿں����ṹ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �󶨵ײ�Ӳ��������������ʼ�����ϵ磬��ס��λ����
		�����ͺ�ΪXY-MZA66A  XY-MZA66C
****************************************************************/
extern void NewZigbee_LowLevelInit(COM_HW_OPERATIONS a_sHwOps);



#ifdef __cplusplus
}
#endif

#endif /* NEW_ZIGBEE_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
