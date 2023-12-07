#ifndef IR38COM_TASK_H
#define IR38COM_TASK_H

#include "config.h"
#include "comtask.h"
#include "main_task.h"
#include "ir.h"

#define IR38_COM_BAUDRATE 1200
#define IR38_COM_FRAME_LEN_MAX 512  //最大支持的报文长度

typedef struct
{
    uint8_t 	m_byRes: 8;

    //实现相关硬件操作，可读写
    COM_HW_OPERATIONS m_sHwOps;

} IR38COM_LOWLEVEL_MAG;

typedef struct
{

    IR38COM_LOWLEVEL_MAG m_sLLMag;

    uint32_t 	m_bIsInit: 1;   //任务初始化
    uint32_t 	m_bIsRecvNew: 1; //接收到新数据
    uint32_t 	m_bRes: 30;

    COMTASK_MAG m_sTaskMag;

} IR38COMTASK_MAG;


/****************************************************************
@FUNCTION：获取红外短距端口管理句柄
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 

****************************************************************/
IR38COMTASK_MAG *IR38ComTask_HandleGet(void);

/****************************************************************
@FUNCTION：获取红外短距端口任务管理句柄
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 

****************************************************************/
COMTASK_MAG *IR38ComTask_TaskHandleGet(void);

/****************************************************************
@FUNCTION：红外短距功能初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sIR38ComTask_Mag

****************************************************************/
void IR38ComTask_Init(void);

/****************************************************************
@FUNCTION：红外短距功能反初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sIR38ComTask_Mag

****************************************************************/
void IR38ComTask_DeInit(void);


/****************************************************************
@FUNCTION：红外短距功能任务主进程
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sIR38ComTask_Mag

****************************************************************/
void IR38ComTask_Process(void);



#endif



