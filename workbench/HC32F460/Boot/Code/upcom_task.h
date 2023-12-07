#ifndef UPCOM_TASK_H
#define UPCOM_TASK_H

#include "config.h"
#include "main_task.h"

#define UP_COM COM2
#define UP_COM_BAUDRATE 115200
#define UP_COM_FRAME_LEN_MAX 1024  //最大支持的报文长度

typedef struct
{

    uint8_t 	m_byRes: 8;


    //实现相关硬件操作，可读写
    COM_HW_OPERATIONS m_sHwOps;

} UPCOM_LOWLEVEL_MAG;


typedef struct
{

    UPCOM_LOWLEVEL_MAG *m_pLLMag;

    uint32_t 	m_bIsInit: 1;   //任务初始化
    uint32_t 	m_bIsRecvNew: 1; //接收到新数据
    uint32_t 	m_bRes: 30;

    COMTASK_MAG m_sTaskMag;

    //uint8_t m_byActualNum;

} UPCOMTASK_MAG;

/****************************************************************
@FUNCTION：上行串口功能初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:

****************************************************************/
void UpComTask_Init(void);

/****************************************************************
@FUNCTION：上行串口功能反初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:

****************************************************************/
void UpComTask_DeInit(void);

/****************************************************************
@FUNCTION：上行串口功能任务主进程
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:

****************************************************************/
void UpComTask_Process(void);


#endif



