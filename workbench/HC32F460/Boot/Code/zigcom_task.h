#ifndef ZIGCOM_TASK_H
#define ZIGCOM_TASK_H

#include "config.h"
#include "main_task.h"
#include "new_zigbee.h"

#if (BOARDID == 1)
    #define ZIG_COM COM1
#else  (BOARDID == 2)
    #define ZIG_COM COM4
#endif

#define ZIG_COM_BAUDRATE 115200
#define ZIG_COM_FRAME_LEN_MAX 256  //最大支持的报文长度,zigbee模组硬件限制了接收长度不超过90，这边是为了数据对齐



typedef struct
{

    NEW_ZIGBEE_LOWLEVEL_MAG *m_pLLMag;

    uint32_t 	m_bIsInit: 1;   //任务初始化
    uint32_t 	m_bIsRecvNew: 1; //接收到新数据

    uint32_t 	m_bRes: 30;

    COMTASK_MAG m_sTaskMag;

    uint8_t     m_aLastCommMacBuf[8];   //最新通讯zigbee MAC地址
    uint8_t     m_byLastCommMode;   //最新接收数据方式，0-点对点，1-广播
    uint8_t     m_byLastCommRSSI;   //最新通讯信号强度


} ZIGCOMTASK_MAG;


extern ZIGCOMTASK_MAG g_sZigComTask_Mag;


/****************************************************************
@FUNCTION：zigbee 串口功能初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sZigComTask_Mag

****************************************************************/
void ZigComTask_Init(void);

/****************************************************************
@FUNCTION：zigbee 串口功能反初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sZigComTask_Mag

****************************************************************/
void ZigComTask_DeInit(void);

/****************************************************************
@FUNCTION：zigbee 串口功能任务主进程
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sZigComTask_Mag

****************************************************************/
void ZigComTask_Process(void);


#endif



