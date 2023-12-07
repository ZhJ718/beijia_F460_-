#ifndef GPSCOM_TASK_H
#define GPSCOM_TASK_H

#include "config.h"
#include "main_task.h"
#include "gps.h"

#define GPS_COM COM3
#define GPS_BAUDRATE 	9600
#define GPS_COM_FRAME_LEN_MAX 512  //���֧�ֵı��ĳ���


typedef struct
{
    UTC_TIME m_sUtcTime;			//UTCʱ��
    uint32_t m_dwLatitude;				//γ�� ���ݲ��ṩ����Ҫ��ʱ���Ի��㣩
    uint8_t  m_byNorS;					//��γ/��γ,'N':��γ;'S':��γ
    uint32_t m_dwLongitude;			    //���� ���ݲ��ṩ����Ҫ��ʱ���Ի��㣩
    uint8_t  m_byEorW;					//����/����,E:����;W:����
    int32_t  m_dwAltitude;			 	//���θ߶�,�Ŵ���10��,ʵ�ʳ���10.��λ:0.1m  ���ݲ��ṩ��
    uint16_t m_wSpeed;					//��������,�Ŵ���1000��,ʵ�ʳ���10.��λ:0.001����/Сʱ	   ���ݲ��ṩ��
    uint8_t m_byJD_Du;
    uint8_t m_byJD_Fen;
    uint32_t m_dwJD_Fen_XiaoShu;		//����С������λС��
    uint8_t m_byWD_Du;
    uint8_t m_byWD_Fen;
    uint32_t m_dwWD_Fen_XiaoShu;		//����С������λС��
    uint8_t	m_byNumSv;					//���ڶ�λ��������Ŀ��00~24
    uint8_t WDLength;
    uint8_t JDLength;
    uint8_t RTKMode;
} NMEA_MSG;



typedef struct
{

    GPS_LOWLEVEL_MAG *m_pLLMag;

    uint32_t 	m_bIsInit: 1;   //�����ʼ��
    uint32_t 	m_bIsRecvNew: 1; //���յ�������
    uint32_t 	m_bRes: 30;

    COMTASK_MAG m_sTaskMag;

    NMEA_MSG    m_sMsg;

} GPSCOMTASK_MAG;

extern GPSCOMTASK_MAG g_sGpsComTask_Mag;

#endif



