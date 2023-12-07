#ifndef RESCOM_TASK_H
#define RESCOM_TASK_H

#include "config.h"
#include "main_task.h"

#define RES_COM COM5
#define RES_COM_BAUDRATE 115200
#define RES_COM_FRAME_LEN_MAX 2048  //最大支持的报文长度

typedef struct
{

    uint8_t 	m_byRes: 8;


    //实现相关硬件操作，可读写
    COM_HW_OPERATIONS m_sHwOps;

} RESCOM_LOWLEVEL_MAG;


typedef struct
{

    RESCOM_LOWLEVEL_MAG *m_pLLMag;

    uint32_t 	m_bIsInit: 1;   //任务初始化
    uint32_t 	m_bIsRecvNew: 1; //接收到新数据
    uint32_t 	m_bRes: 30;

    COMTASK_MAG m_sTaskMag;

    //uint8_t m_byActualNum;

} RESCOMTASK_MAG;

/****************************************************************
@FUNCTION：保留串口功能初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sResComMag

****************************************************************/
void ResComTask_Init(void);

/****************************************************************
@FUNCTION：保留串口功能反初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sResComMag

****************************************************************/
void ResComTask_DeInit(void);

/****************************************************************
@FUNCTION：保留串口功能任务主进程
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sResComMag

****************************************************************/
void ResComTask_Process(void);

void ResComTask_RecvSendToUpcom(uint8_t *pBuf, uint16_t BufLen, uint8_t DevIdx);

#endif



