#ifndef RESCOM_TASK_H
#define RESCOM_TASK_H

#include "config.h"
#include "main_task.h"

#define RES_COM COM5
#define RES_COM_BAUDRATE 115200
#define RES_COM_FRAME_LEN_MAX 2048  //最大支持的报文长度

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


#endif



