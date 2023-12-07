/**
  ******************************************************************************
  * @file    ble_skylab.h
  * @author  xfw
  * @brief   This file contains definitions for skylab's ble module
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
#ifndef __BLE_SKYLAB_H
#define __BLE_SKYLAB_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/

#include "config.h"
#include "uart.h"

typedef enum
{
    BLE_TXPWR_4DB = 4,	//4DB
    BLE_TXPWR_0DB = 0,		//0DB
    BLE_TXPWR_N4DB = -4,		//-4DB
    BLE_TXPWR_N8DB = -8,		//-8DB
    BLE_TXPWR_N12DB = -12,	//-12DB
    BLE_TXPWR_N16DB = -16,	//-16DB
    BLE_TXPWR_N20DB	= -20,	//-20DB
    BLE_TXPWR_N30DB = -30,	//-30DB

} ENUM_BLE_TXPWR;


//Ӳ������
typedef struct
{
    //����Ӳ����ʼ������������IO�ڳ�ʼ�������ڵ�
    void (*hw_init)(void);
    //������Դʹ��
    void (*gpio_power_on)(void);
    //������Դ����
    void (*gpio_power_off)(void);
    //����Ӳ����λ��������
    void (*gpio_reset)(void);
    //����Ӳ����λ��������
    void (*gpio_set)(void);

    //����������
    uint32_t (*read)(uint8_t *, uint16_t *);
    //����д����
    uint32_t (*write)(uint8_t *, uint16_t );

} BLE_SKYLAB_HW_OPERATIONS;



typedef struct
{
    //
    uint8_t		m_byIsReset: 1;		//TRUE-��λ��
    uint8_t		m_byIsCommunicationSucc: 1;
    uint8_t 	m_byRes: 6;

    uint32_t	m_dwNoCommunicationTImeout;

    uint8_t 	m_byResetSetCnt;	//�趨�ĸ�λʱ��
    uint8_t 	m_byResetNowCnt;	//��ǰ����ɸ�λʱ��

    //ʵ��Ble���Ӳ���������ɶ�д
    BLE_SKYLAB_HW_OPERATIONS m_sHwOps;

} BLE_SKYLAB_LOWLEVEL_MAG;


//ATָ��
#define BLE_ATCMD_GETMAC "AT+MAC\r\n"

#define BLE_ATCMD_DEVNAME "AT+DEV_NAME="

#define BLE_ATCMD_TXPWR "AT+TXPWR="

#define BLE_ATCMD_CONNECT_MAC "AT+CON_MAC=00:00:00:00:00:00\r\n"

#define BLE_ATCMD_DISCONNECT "AT+DISCON\r\n"

#define BLE_ATCMD_SCAN	"AT+SCAN_BLE\r\n"



//ATָ����Ӧ�����ַ���

#define BLE_ATCMD_RSP_CONNECTED "[DA]Connected="
#define BLE_ATCMD_RSP_DISCONNECTED "[DA]Disconnected="
#define BLE_ATCMD_RSP_RSSI "[DA]RSSI="
#define BLE_ATCMD_RSP_ERROR	"[AT]ER\r\n"

/****************************************************************
@FUNCTION���칤�������ģ��MAC��ַת��(�ַ�����ʽ->����)
@INPUT��MAC��ַ�ַ�����ʽ�����ȣ�ת�������ִ��λ�úʹ�С
@OUTPUT��ת����������ʽ��С
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ����"12:34:56:78:9A:BC"-> 0x12 0x34 0x56 0x78 0x9A 0xBC
****************************************************************/
void Ble_MacCharToHex(uint8_t *a_strMac, uint16_t a_wMacLen, uint8_t *a_aMacHex, uint16_t *a_wMacHexLen);


/****************************************************************
@FUNCTION���칤�������ģ��MAC��ַת��(����->�ַ�����ʽ)
@INPUT��MAC��ַ������ʽ�����ȣ�ת�����ַ������λ�úʹ�С
@OUTPUT��ת�����ַ�����ʽ��С
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ����0x12 0x34 0x56 0x78 0x9A 0xBC -> "12:34:56:78:9A:BC"
****************************************************************/
void Ble_MacHexToChar(uint8_t *a_aMacHex, uint16_t MacHexLen, uint8_t *a_strMac, uint16_t *a_wMacLen);


/****************************************************************
@FUNCTION�����������ַ����Ƿ�����ATָ��֡
@INPUT���ַ���������
@OUTPUT��ATָ��֡ʵ�ʳ��ȣ������ַ����Ƿ񱻽ض�
@RETURN: TRUE-�����ɹ� FALSE-����ʧ��
@AUTHOR��xfw
@SPECIAL: �󶨵ײ�Ӳ��������������ʼ�����ϵ磬��ס��λ����
****************************************************************/
uint8_t Ble_FindATFrame(uint8_t *pBuf, uint16_t *pLen, uint32_t *pIsCutted);

/****************************************************************
@FUNCTION���칤�������ģ��ײ�Ӳ����ʼ��
@INPUT��Ӳ���ӿں����ṹ�壬BLE����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �󶨵ײ�Ӳ��������������ʼ�����ϵ磬��ס��λ����
****************************************************************/
extern void Ble_LowLevelInit(BLE_SKYLAB_HW_OPERATIONS a_sHwOps, BLE_SKYLAB_LOWLEVEL_MAG *pMag);


/****************************************************************
@FUNCTION���칤�������ģ�鸴λ
@INPUT��BLE�ײ����ṹ��ָ�룬��λ������>0��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��λʱ��=Ble_LowLevelTimedProcess���ȼ�� * ��λ����
		�˺�����λ����������Ч������λ����Ϊ0����ģ�鸴λһ����λ�ĵ��ȼ��
****************************************************************/
void Ble_Reset(BLE_SKYLAB_LOWLEVEL_MAG *pMag, uint8_t Timer);


/****************************************************************
@FUNCTION���칤�������ģ�鸴λ������ͣ
@INPUT��BLE�ײ����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �˺���������Ч
****************************************************************/
void Ble_ResetSuspend(BLE_SKYLAB_LOWLEVEL_MAG *pMag);


/****************************************************************
@FUNCTION���칤�������ģ�鸴λ�����ָ�
@INPUT��BLE�ײ����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ����ʱ������λ�����Ѿ�����������Ӱ��
****************************************************************/
void Ble_ResetRecovery(BLE_SKYLAB_LOWLEVEL_MAG *pMag);


/****************************************************************
@FUNCTION���칤�������ģ��ײ㶨ʱ���Ƚ���
@INPUT��BLE�ײ����ṹ��ָ��
@OUTPUT����
@RETURN: TRUE-����ִ�еײ㶨ʱ���ȣ� FALSE-�޵ײ㶨ʱ���ȣ���ִ���ϲ�Ӧ�ö�ʱ����
@AUTHOR��xfw
@SPECIAL: ʵ��ģ�鸴λ
****************************************************************/
extern uint32_t Ble_LowLevelTimedProcess(BLE_SKYLAB_LOWLEVEL_MAG *pMag);



/****************************************************************
@FUNCTION���칤�������ģ���ȡ����MAC
@INPUT��BLE�ײ����ṹ��ָ��,����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��
****************************************************************/
void Ble_GetMac(BLE_SKYLAB_LOWLEVEL_MAG *pMag, uint8_t *a_sRes1, uint16_t a_wRes2);



/****************************************************************
@FUNCTION���칤�������ģ�������豸����
@INPUT��BLE�ײ����ṹ��ָ��,�豸����,�豸���Ƴ���
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:����У��
****************************************************************/
void Ble_SetDevName(BLE_SKYLAB_LOWLEVEL_MAG *pMag, uint8_t *a_strName, uint16_t NameLen);


/****************************************************************
@FUNCTION���칤�������ģ�����÷��书��
@INPUT��BLE�ײ����ṹ��ָ��,���书��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Ble_SetTxPwr(BLE_SKYLAB_LOWLEVEL_MAG *pMag, ENUM_BLE_TXPWR a_enumTxPwr);

/****************************************************************
@FUNCTION���칤�������ģ������ָ��MAC��ַ�豸
@INPUT��BLE�ײ����ṹ��ָ��,MAC��ַ(Hex��ʽ)��MAC��ַ��ռ�ռ�
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: MAC��ַ��ռ�ռ���Ҫ����6�ֽ�
****************************************************************/
void Ble_ConnectMac(BLE_SKYLAB_LOWLEVEL_MAG *pMag, uint8_t *a_sPeerMacHex, uint16_t a_wPeerMacHexLen);


/****************************************************************
@FUNCTION���칤�������ģ��ɨ���ܱ������豸
@INPUT��BLE�ײ����ṹ��ָ��,����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��
****************************************************************/
void Ble_Scan(BLE_SKYLAB_LOWLEVEL_MAG *pMag, uint8_t *a_sRes1, uint16_t a_wRes2);


/****************************************************************
@FUNCTION���칤�������ģ��Ͽ�����
@INPUT��BLE�ײ����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Ble_DisConnect(BLE_SKYLAB_LOWLEVEL_MAG *pMag, uint8_t *a_sRes1, uint16_t a_wRes2);



#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
