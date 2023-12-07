/**
  ******************************************************************************
  * @file    hmi.h
  * @author  xfw
  * @brief   This file contains definitions for human machine interface
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2019 Beijing HuaRu Tech Co.</center></h2>
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
#ifndef __HMI_H
#define __HMI_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
//#include "stm32f0xx_hal.h"
#include "config.h"
#include "uart.h"

#define HMI_FRAME_CMDPARAM_LEN_MAX  50


typedef struct
{
    uint8_t Head;	//֡ͷ
    uint8_t Cmd;	//ָ��
    uint8_t CmdParaLen; //ָ���������
    uint8_t CmdPara[HMI_FRAME_CMDPARAM_LEN_MAX];//ָ�����
    uint32_t Tail; 	//֡β
} HMI_FRAME, *PHMI_FRAME;

typedef struct
{
    uint16_t m_wAmmuCnt[HMI_AMMU_MAX];	//��ҩ����

    uint16_t m_wSetValue; //����װ������0-9999 m

    uint8_t m_wDamageValue; //����0-100
    uint8_t m_wOilValue; //����0-100

    uint32_t m_byIsSet: 1; //TRUE-װ��

    uint32_t m_byIsAskForAmmo: 1; //TRUE-���뵯ҩ
    uint32_t m_byIsAskForOil: 1; //TRUE-��������
    uint32_t m_byIsRes: 29;

    uint32_t m_byIsAmmuSet; //��ӦλΪTRUE-������Ӧ������Ԫ��Ϣ

} TANK_HMI_MAG;


typedef struct
{
    uint8_t m_LocalX[30];
    uint8_t m_LocalX_len;

    uint8_t m_LocalY[30];
    uint8_t m_LocalY_len;

    uint8_t m_YinXinMethod; //0X31-˲�� 0x32-����  0x33-����

    uint8_t m_SendMethod; //0x31- ����   0x32-����

    uint8_t m_IsSend: 1;

} PJP_HMI_MAG;


typedef struct
{
    u8 m_byRID;
    u16 m_wPID;
    u8 m_strRID[TERMINAL_STRING_RID_LEN + 1];
    u8 m_strPID[TERMINAL_STRING_PID_LEN + 1];
    u8 m_strMac[TERMINAL_STRING_MAC_LEN + 1];
    u8 m_strName[TERMINAL_STRING_NAME_LEN + 1];
} TERMINAL_MAG;

typedef struct
{
    uint32_t m_IsScan: 1;	//1-ɨ����  0-ֹͣɨ��
    uint32_t m_IsConnect: 1; //1-������  0-����ֹͣ
    uint32_t m_IsConnectedTerminal: 1; //1-������  0-���ӶϿ�
    uint32_t m_IsGetLog: 1;
    uint32_t m_IsRes: 28;

    uint16_t m_wConnectPID;		//�����ն�PID
    uint16_t m_wScanTime;

    uint8_t m_byTmCnt;		//ɨ�������ն�����
    TERMINAL_MAG m_saTmMag[TERMINAL_MAG_MAX];	//�ն���Ϣ

    uint8_t m_byStrBuf[TERMINAL_STR_BUF_LEN_MAX];
    uint16_t m_wStrLen;

    //ҳ��2�����������
    uint8_t m_byUni1;	//ѡ���������ҩͳһ����
    uint8_t m_byRID1;	//ѡ���RID
    uint16_t m_wPID1;	//ѡ���PID
    uint8_t m_byKEY1;	//ѡ�����Ȩ��
    uint8_t m_byROrB1;	//ѡ��ĺ���

    uint8_t m_byTLQ;	//0-���� 1-���� 2-���ӳ������� 3-ģʽ�л� 4-��ҩ����

    //ҳ��4�������豸
    uint8_t m_byIdx;	//���µ��������豸��� 1-7
    uint8_t m_byType;	//���µ��������豸����
    uint8_t m_aMac[6];	//���µ��������豸MAC

    //ҳ��10����
    //uint8_t m_IsScanMine:1;	//1-ɨ����  0-ֹͣɨ��

    //uint8_t m_IsResMine:7;
    //PARA_MINE m_aMinePara; //ɨ�赽�׵���Ϣ

} TM_HMI_MAG;

typedef union
{
    TANK_HMI_MAG TankMag;
    PJP_HMI_MAG PjpMag;
    TM_HMI_MAG TmDetectorMag;
} HMI_DATA_MAG;


typedef struct
{
    UART_MANAGER *m_psUartMag;
    //HMI_FRAME m_sFrameRecv;
    //HMI_FRAME m_sFrameSend;

    uint8_t m_byIsWakeUp: 1; 	//��ʶ��ǰ��Ļ�Ƿ��ڻ���״̬
    uint8_t m_byIsKeyDown: 1; 	//��Ļ���ѻ����߰���������
    uint8_t m_byIsNoAction: 1;	//��ʱ��δ������Ļ
    uint8_t m_byIsNeedSend: 1;	//�����ݷ���

    uint8_t m_byIsUpdatePage: 1; //��ʱˢ��ҳ��
    uint8_t m_byRes: 3;

    uint8_t m_byCurrentPage;    //��ǰҳ��
    uint16_t m_wUpdatePageTime; //��ʱˢ��ҳ���ʱ

    uint16_t m_wKeyDownTime;	//��������ʱ��(*10ms)
    uint16_t m_wNoActionTime;	//δ������Ļʱ��(*10ms)

    uint8_t m_byTType;	//�豸���� 1-̹��
    HMI_DATA_MAG DataUnion;
} HMI_MANAGER, *PHMI_MANAGER;



typedef enum
{
    Button_Off = 0,
    Button_On = 1
} ENUM_BUTTON_STA;



/****************************************************************
@FUNCTION������������ַ������Ƿ���������̬��Э��
@INPUT������������pBuf������pLen
@OUTPUT��pLen��ʵ֡���ȡ�pIsCutted�Ƿ񱻷ְ�
@RETURN: TRUE-�ɹ�(�ְ�������ɹ�) FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:���������25-��಻����Э������ݳ���(����֡ͷ��δ����֡β)����С��ֵ�����Ը���Ĵ��쳣�лָ�
****************************************************************/
uint32_t HMI_FindFrame(uint8_t *pBuf, uint16_t *pLen, uint32_t *pIsCutted);

/****************************************************************
@FUNCTION�������̬��Э�����
@INPUT������������pBuf������Len
@OUTPUT�����Ľ����ṹ��
@RETURN:
@AUTHOR��xfw
@SPECIAL: ��Ҫ��֤����ı����Ƿ���Э��ģ���������У�鱨�ĵ���ȷ��
***************************************************************
void HMI_FrameParse(uint8_t *pBuf,uint16_t Len,HMI_FRAME *pFrame);*/



/****************************************************************
@FUNCTION�����½������ؼ���ֵ
@INPUT��
	Screen_id(2 ���ֽ�)��������
	Control_id(2 ���ֽ�)���ؼ����
	Progressvalue��4 ���ֽڣ� : �û�д����½�����ֵ
@OUTPUT��
	pBuf:������õİ�
	pLen:������
@RETURN: TRUE-�ɹ�(�ְ�������ɹ�) FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint32_t HMI_SetProgress(uint16_t Screen_id, uint16_t Control_id, uint16_t ProgressValue, uint8_t *pBuf, uint16_t *pLen);


/****************************************************************
@FUNCTION�������ı��ؼ���ֵ
@INPUT��
	Screen_id(2 ���ֽ�)��������
	Control_id(2 ���ֽ�)���ؼ����
	pStr���������� : �ı���Ҫ��ʾ���ַ���������Ϊ��
@OUTPUT��
	pBuf:������õİ�
	pLen:������
@RETURN: TRUE-�ɹ�(�ְ�������ɹ�) FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint32_t HMI_SetText(uint16_t Screen_id, uint16_t Control_id, uint8_t *pStr, uint8_t *pBuf, uint16_t *pLen);


/****************************************************************
@FUNCTION����ȡ�ı��ؼ���ֵ
@INPUT��
	Screen_id(2 ���ֽ�)��������
	Control_id(2 ���ֽ�)���ؼ����
@OUTPUT��
	pBuf:������õİ�
	pLen:������
@RETURN: TRUE-�ɹ�(�ְ�������ɹ�) FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint32_t HMI_GetText(uint16_t Screen_id, uint16_t Control_id, uint8_t *pBuf, uint16_t *pLen);



/****************************************************************
@FUNCTION�����°�ť״̬
@INPUT��
	Screen_id(2 ���ֽ�)��������
	Control_id(2 ���ֽ�)���ؼ����
	Status��1 ���ֽڣ� : ��ť״̬����ENUM_BUTTON_STA
		0x00����ť�ɰ��±�ɵ���
		0x01����ť�ɵ����ɰ���
@OUTPUT��
	pBuf:������õİ�
	pLen:������
@RETURN: TRUE-�ɹ�(�ְ�������ɹ�) FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint32_t HMI_SetButton(uint16_t Screen_id, uint16_t Control_id, ENUM_BUTTON_STA Status, uint8_t *pBuf, uint16_t *pLen);

typedef enum
{
    IMAGE_GREY = 0,
    IMAGE_GREEN,
    IMAGE_RED,
    IMAGE_YELLOW
} ENUM_FLASHIMAGE_STA;

/****************************************************************
@FUNCTION�����°�ť״̬
@INPUT��
	Screen_id(2 ���ֽ�)��������
	Control_id(2 ���ֽ�)���ؼ����
	FlashImgae_ID��1 ���ֽڣ� : ĳһ����֡IDָ����ĳһ֡��ʼ���� 0-��ɫ 1-��ɫ 2-��ɫ 3-��ɫ
@OUTPUT��
	pBuf:������õİ�
	pLen:������
@RETURN: TRUE-�ɹ�(�ְ�������ɹ�) FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint32_t HMI_SetFlashImage(uint16_t Screen_id, uint16_t Control_id, ENUM_FLASHIMAGE_STA FlashImgae_ID, uint8_t *pBuf, uint16_t *pLen);

/****************************************************************
@FUNCTION�����ݼ�¼�ؼ�-��ӳ����¼
@INPUT��
	Screen_id(2 ���ֽ�)��������
	Control_id(2 ���ֽ�)���ؼ����
	String���������� : �û���ӵ��ַ�������
@OUTPUT��
	pBuf:������õİ�
	pLen:������
@RETURN: TRUE-�ɹ�  FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint32_t HMI_DatRcd_Insert(uint16_t Screen_id, uint16_t Control_id, uint8_t *pStr, uint8_t *pBuf, uint16_t *pLen);


/****************************************************************
@FUNCTION�����ݼ�¼�ؼ�-�����¼����
@INPUT��
	Screen_id(2 ���ֽ�)��������
	Control_id(2 ���ֽ�)���ؼ����
@OUTPUT��
	pBuf:������õİ�
	pLen:������
@RETURN: TRUE-�ɹ�  FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint32_t HMI_DatRcd_Reset(uint16_t Screen_id, uint16_t Control_id, uint8_t *pBuf, uint16_t *pLen);

#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
