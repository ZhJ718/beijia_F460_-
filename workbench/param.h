/**
  ******************************************************************************
  * @file    param.h
  * @author  xfw
  * @brief   This file contains definitions for sys param
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
#ifndef __PARAM_H
#define __PARAM_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "config.h"

/****************************************************************
@FUNCTION�������Ƿ���Ч
@INPUT������
@OUTPUT����
@RETURN:  TRUE - ��Ч/ FALSE - ��Ч
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint32_t IsParamValid(uint8_t *pDat, uint16_t DatLen);


// �����ն���ݲ���,�������ֽڶ���
typedef struct
{
    uint8_t m_byTType;          //�豸ʵ������
    uint8_t m_byTSubType;   //�豸ʵ��������
    uint8_t m_byRid;	        //�豸��λ����  4
    uint16_t m_wPid;            //�豸���

    uint8_t m_aServerIP[4] ;    //����IP��ַ����ˣ����� C0 A8 01 01,����192.168.1.1)
    uint16_t m_wServerPort;   //�����˿ںţ���ˣ����� 1F 90������8080��ע��0xFFFFΪ���޸ģ�12
    uint8_t m_byIsUDP;          //����Э�飨0-TCP��1-UDP��
    uint8_t m_byWType;  //��������,��E_WIRELESS_TYPE
    uint8_t m_byWTxPwr;       // ����ģ�鷢�书��(1-15 ��ע��0xFFΪ���޸�)
    uint8_t m_aWMac[8];   // ����ģ��Mac��ַ(ע��0xFFFFFFFFFFFFFFFFΪ���޸�)
    uint8_t m_byRes1;   //�����ֽ�1��������
    uint8_t m_byRes2;   //�����ֽ�2��������24

    uint32_t	m_dwChk;    //У��
} ID_PARAM;

extern ID_PARAM g_sIDParam;

/****************************************************************
@FUNCTION���ն�����ʱ��������Ĭ��ֵ
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void IDParam_SetDefaultValue(ID_PARAM *pParam);

/****************************************************************
@FUNCTION���ն���ݲ�����ʼ��
@INPUT����
@OUTPUT��g_sIDParam�ṹ�帳ֵ
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ������ȡ��ʧ�ܣ����������
****************************************************************/
void IDParam_Init(void);

/****************************************************************
@FUNCTION���ն���ݲ�������
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �����ϳ�ʱ��
****************************************************************/
void IDParam_Save(void);


/****************************************************************
@FUNCTION����ȡ�ն���ݲ����е�ʵ������
@INPUT����
@OUTPUT����
@RETURN: ʵ������
@AUTHOR��xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint8_t IDParam_GetTType(void);

/****************************************************************
@FUNCTION�������ն���ݲ����е�ʵ������
@INPUT��ʵ����������ֵ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sIDParam
****************************************************************/
void IDParam_SetTType(uint8_t val);


/****************************************************************
@FUNCTION����ȡ�ն���ݲ����е�ʵ��������
@INPUT����
@OUTPUT����
@RETURN: ʵ��������
@AUTHOR��xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint8_t IDParam_GetTSubType(void);


/****************************************************************
@FUNCTION�������ն���ݲ����е�ʵ��������
@INPUT��ʵ������������ֵ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sIDParam
****************************************************************/
void IDParam_SetTSubType(uint8_t val);


/****************************************************************
@FUNCTION����ȡ�ն���ݲ����е��豸��λ����
@INPUT����
@OUTPUT����
@RETURN: �豸��λ����
@AUTHOR��xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint8_t IDParam_GetRid(void);


/****************************************************************
@FUNCTION�������ն���ݲ����е��豸��λ����
@INPUT���豸��λ���� ����ֵ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sIDParam
****************************************************************/
void IDParam_SetRid(uint8_t val);


/****************************************************************
@FUNCTION����ȡ�ն���ݲ����е��豸���
@INPUT����
@OUTPUT����
@RETURN: �豸���
@AUTHOR��xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint16_t IDParam_GetPid(void);


/****************************************************************
@FUNCTION�������ն���ݲ����е��豸���
@INPUT���豸��� ����ֵ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sIDParam
****************************************************************/
void IDParam_SetPid(uint16_t val);


/****************************************************************
@FUNCTION����ȡ�ն���ݲ����е�����IP��ַ
@INPUT����
@OUTPUT����
@RETURN: ����IP��ַ
@AUTHOR��xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint8_t *IDParam_GetServerIP(void);


/****************************************************************
@FUNCTION�������ն���ݲ����е�����IP��ַ
@INPUT������IP��ַ ����ֵ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sIDParam
****************************************************************/
void IDParam_SetServerIP(uint8_t * pVal, uint16_t ValLen);


/****************************************************************
@FUNCTION����ȡ�ն���ݲ����е������˿ں�
@INPUT����
@OUTPUT����
@RETURN: �����˿ں�
@AUTHOR��xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint16_t IDParam_GetServerPort(void);

/****************************************************************
@FUNCTION�������ն���ݲ����е������˿ں�
@INPUT�������˿ں�����ֵ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sIDParam
****************************************************************/
void IDParam_SetServerPort(uint16_t val);

/****************************************************************
@FUNCTION����ȡ�ն���ݲ����е�����Э��ѡ��
@INPUT����
@OUTPUT����
@RETURN: ����Э��ѡ��
@AUTHOR��xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint8_t IDParam_GetIsUDP(void);


/****************************************************************
@FUNCTION�������ն���ݲ����е�����Э��ѡ��
@INPUT������Э��ѡ������ֵ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sIDParam
****************************************************************/
void IDParam_SetIsUDP(uint8_t val);

/****************************************************************
@FUNCTION����ȡ�ն���ݲ����е�����ģ������
@INPUT����
@OUTPUT����
@RETURN: ����ģ�鷢�书��
@AUTHOR��xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint8_t IDParam_GetWType(void);

/****************************************************************
@FUNCTION�������ն���ݲ����е�����ģ������
@INPUT������ģ�鷢�书������ֵ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sIDParam
****************************************************************/
void IDParam_SetWType(uint8_t val);

/****************************************************************
@FUNCTION����ȡ�ն���ݲ����е�����ģ�鷢�书��
@INPUT����
@OUTPUT����
@RETURN: Zigbeeģ�鷢�书��
@AUTHOR��xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint8_t IDParam_GetTxPwr(void);

/****************************************************************
@FUNCTION�������ն���ݲ����е�����ģ�鷢�书��
@INPUT��Zigbeeģ�鷢�书������ֵ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �´����������Ӱ��Ӳ��
****************************************************************/
void IDParam_SetTxPwr(uint8_t val);

/****************************************************************
@FUNCTION����ȡ�ն���ݲ����е�����ģ��Mac��ַ
@INPUT����
@OUTPUT����
@RETURN: Zigbeeģ��Mac��ַ
@AUTHOR��xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint8_t *IDParam_GetWMac(void);


/****************************************************************
@FUNCTION�������ն���ݲ����е�����ģ��Mac��ַ
@INPUT��Zigbeeģ��Mac��ַ ����ֵ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: pValΪ��ʱ��������ЧMAC
****************************************************************/
void IDParam_SetWMac(uint8_t * pVal, uint16_t ValLen);


/****************************************************************
@FUNCTION���ն���ݲ����ж��Ƿ�Ϊ���������Ϣ
@INPUT����
@OUTPUT: ��
@RETURN: TRUE - ��Ч/ FALSE - ��Ч
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint32_t IDParam_IsIDValid(uint8_t TType, uint8_t TSubType, uint8_t Rid, uint16_t Pid);


typedef struct
{
    uint8_t m_byTType;          //���豸ʵ������
    uint8_t m_byTSubType;   //���豸ʵ�������ͣ�0xFF-�����ģ����������÷������ʱ��
    uint8_t m_byWType;	        //���豸�������� (0x0 -���ߣ�0x01 - ������0x02 - �±�׼zigbee��0x03 - lora�豸)
    uint8_t m_aWMac[8];            //���豸MAC��ַ����λ��ǰ��11

		uint8_t m_byWeaponIdx;
		uint8_t m_byIdx;
    uint8_t m_byRes1;   //�����ֽ�1��������
    uint8_t m_byRes2;   //�����ֽ�2��������
    uint8_t m_byRes3;   //�����ֽ�3��������
} SINGLE_SLAVER_PARAM;


#define SYS_SLAVER_CNT_MAX 10

// ����ϵͳ����,�������ֽڶ���
typedef struct
{
    // �ն����豸����
    SINGLE_SLAVER_PARAM  m_sSingleParam[SYS_SLAVER_CNT_MAX];// 16*10 = 160

    uint32_t	m_dwChk;    //У��
} SYS_PARAM;


extern SYS_PARAM g_sSysParam;

/****************************************************************
@FUNCTION��ϵͳ��������Ĭ��ֵ
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void SysParam_SetDefaultValue(SYS_PARAM *pParam);


/****************************************************************
@FUNCTION��ϵͳ������ʼ��
@INPUT����
@OUTPUT��g_sSysParam �ṹ�帳ֵ
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ������ȡ��ʧ�ܣ����������
****************************************************************/
void SysParam_Init(void);


/****************************************************************
@FUNCTION��ϵͳ������������
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �����ϳ�ʱ��
****************************************************************/
void SysParam_Save(void);


/****************************************************************
@FUNCTION����ȡϵͳ�����е����豸����
@INPUT����
@OUTPUT����
@RETURN: Zigbeeģ��Mac��ַ
@AUTHOR��xfw
@SPECIAL: g_sSysParam
****************************************************************/
SINGLE_SLAVER_PARAM *SysParam_GetSlaverParam(uint8_t idx);

/****************************************************************
@FUNCTION������ϵͳ�����е����豸����
@INPUT�����豸���������豸���
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sSysParam
****************************************************************/
void SysParam_SetSlaverParam(SINGLE_SLAVER_PARAM * pVal, uint8_t idx);


#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
