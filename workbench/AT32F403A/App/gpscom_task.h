#ifndef GPSCOM_TASK_H
#define GPSCOM_TASK_H

#include "config.h"
#include "main_task.h"
#include "gps.h"

#define GPS_COM COM3
#define GPS_BAUDRATE 	9600
#define GPS_COM_FRAME_LEN_MAX 512  //最大支持的报文长度


typedef struct
{
    UTC_TIME m_sUtcTime;			//UTC时间
    uint32_t m_dwLatitude;				//纬度 （暂不提供，需要用时可以换算）
    uint8_t  m_byNorS;					//北纬/南纬,'N':北纬;'S':南纬
    uint32_t m_dwLongitude;			    //经度 （暂不提供，需要用时可以换算）
    uint8_t  m_byEorW;					//东经/西经,E:东经;W:西经
    int32_t  m_dwAltitude;			 	//海拔高度,放大了10倍,实际除以10.单位:0.1m  （暂不提供）
    uint16_t m_wSpeed;					//地面速率,放大了1000倍,实际除以10.单位:0.001公里/小时	   （暂不提供）
    uint8_t m_byJD_Du;
    uint8_t m_byJD_Fen;
    uint32_t m_dwJD_Fen_XiaoShu;		//保留小数点五位小数
    uint8_t m_byWD_Du;
    uint8_t m_byWD_Fen;
    uint32_t m_dwWD_Fen_XiaoShu;		//保留小数点五位小数
    uint8_t	m_byNumSv;					//用于定位的卫星数目，00~24
    uint8_t WDLength;
    uint8_t JDLength;
    uint8_t RTKMode;
} NMEA_MSG;



typedef struct
{

    GPS_LOWLEVEL_MAG *m_pLLMag;

    uint32_t 	m_bIsInit: 1;   //任务初始化
    uint32_t 	m_bIsRecvNew: 1; //接收到新数据
    uint32_t 	m_bRes: 30;

    COMTASK_MAG m_sTaskMag;

    NMEA_MSG    m_sMsg;

} GPSCOMTASK_MAG;

extern GPSCOMTASK_MAG g_sGpsComTask_Mag;

#endif



