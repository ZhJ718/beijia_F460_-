#ifndef LASERCOM_TASK_H
#define LASERCOM_TASK_H

#include "config.h"
#include "linkqueue.h"
#include "lasercom.h"
#include "main_task.h"

#define LASER_COM COM4
#define LASER_COM_BAUDRATE 19200
#define LASER_COM_FRAME_LEN_MAX 512  //最大支持的报文长度



typedef struct
{

    LASERCOM_LOWLEVEL_MAG *m_pLLMag;

    uint32_t 	m_bIsInit: 1;   //任务初始化
    uint32_t 	m_bIsRecvNew: 1; //接收到新数据
    uint32_t 	m_bRes: 30;

    COMTASK_MAG m_sTaskMag;

    uint8_t m_byActualNum;

} LASERCOMTASK_MAG;


extern LASERCOMTASK_MAG g_sLaserComTask_Mag;

/****************************************************************
@FUNCTION：激光接收串口功能初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sLaserComTask_Mag

****************************************************************/
void LaserComTask_Init(void);


/****************************************************************
@FUNCTION：激光接收串口功能反初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sLaserComTask_Mag

****************************************************************/
void LaserComTask_DeInit(void);


/****************************************************************
@FUNCTION：激光接收串口功能任务主进程
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sZigComTask_Mag

****************************************************************/
void LaserComTask_Process(void);

/****************************************************************
@FUNCTION：获取探头带在线探头数
@INPUT：
    @param  MaxCnt - 可能的最大探头数
    @param  IntVal - 单个探头基础超时，单位10ms。若为0,则取默认值10
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 若回复多个报文，可以增大MaxCnt或者IntVal值
****************************************************************/
void LaserCom_Prot_GetOnlineNum(LASERCOMTASK_MAG *pMag, uint8_t MaxCnt, uint8_t IntVal);

/****************************************************************
@FUNCTION：LASER发送模式切换命令
@INPUT：LASER底层管理结构体指针，模式
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:无
****************************************************************/
void LaserCom_Prot_CtrlMode(LASERCOMTASK_MAG *pMag, LASER_MODE a_Mod);





#endif



