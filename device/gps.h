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

    uint8_t	m_byIsReset: 1;		//TRUE-��λ��
    uint8_t 	m_byIsCommunicationSucc: 1; //TRUE-GPSͨ������
    uint8_t 	m_byIsSetParam: 1;  // TRUE-ģ�����ò���
    uint8_t 	m_byRes: 5;

    uint32_t	m_dwNoCommunicationTImeout;

    uint8_t 	m_byResetSetCnt;	//�趨�ĸ�λʱ��
    uint8_t 	m_byResetNowCnt;	//��ǰ����ɸ�λʱ��
    uint8_t 	m_bySetParamNowCnt;	//�趨�����ò�����Сʱ����


    //ʵ��GPS���Ӳ���������ɶ�д
    COM_HW_OPERATIONS m_sHwOps;

    //���ò���
    NMEA_OUTPUT_FREQ_PARAM m_sParam;

} GPS_LOWLEVEL_MAG;


/****************************************************************
@FUNCTION���п�΢332D GPS��ʼ��
@INPUT��Ӳ���ӿں����ṹ�壬GPS����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �󶨵ײ�Ӳ��������������ʼ�������߸�λ���ţ����Ƹ�������Ƶ��
****************************************************************/
extern void Gps_LowLevelInit(COM_HW_OPERATIONS a_sHwOps);


/****************************************************************
@FUNCTION��ģ�鸴λ
@INPUT����λ������>0��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��λʱ��=��ʱ������ȼ�� * ��λ����
		�˺�����λ����������Ч������λ����Ϊ0����ģ�鸴λһ����λ�ĵ��ȼ��
****************************************************************/
void Gps_Reset(uint8_t Timer);

/****************************************************************
@FUNCTION��ģ�鸴λ������ͣ
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �˺���������Ч
****************************************************************/
void Gps_ResetSuspend(void);


/****************************************************************
@FUNCTION��ģ�鸴λ�����ָ�
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ����ʱ��λ�����Ѿ�����������Ӱ��
****************************************************************/
void Gps_ResetRecovery(void);


/****************************************************************
@FUNCTION��ģ�鿪ʼ���ò���
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ���ú��첽ִ�У����õȻظ�
****************************************************************/
void Gps_StartSetParam(NMEA_OUTPUT_FREQ_PARAM SetParam);

/****************************************************************
@FUNCTION��ģ��ײ㶨ʱ���Ƚ���
@INPUT��
@OUTPUT����
@RETURN: TRUE-����ִ�еײ㶨ʱ����,  FALSE-�޵ײ㶨ʱ����, ��ִ���ϲ�Ӧ�ö�ʱ����
@AUTHOR��xfw
@SPECIAL: ʵ��ģ�鸴λ
****************************************************************/
extern uint32_t Gps_LowLevelTimedProcess(void);


GPS_LOWLEVEL_MAG *Gps_GetLLMagHandle(void);


#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
