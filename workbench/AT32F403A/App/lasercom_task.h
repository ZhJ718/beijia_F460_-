#ifndef LASERCOM_TASK_H
#define LASERCOM_TASK_H

#include "config.h"
#include "linkqueue.h"
#include "lasercom.h"
#include "main_task.h"

#define LASER_COM COM4
#define LASER_COM_BAUDRATE 19200
#define LASER_COM_FRAME_LEN_MAX 512  //���֧�ֵı��ĳ���



typedef struct
{

    LASERCOM_LOWLEVEL_MAG *m_pLLMag;

    uint32_t 	m_bIsInit: 1;   //�����ʼ��
    uint32_t 	m_bIsRecvNew: 1; //���յ�������
    uint32_t 	m_bRes: 30;

    COMTASK_MAG m_sTaskMag;

    uint8_t m_byActualNum;

} LASERCOMTASK_MAG;


extern LASERCOMTASK_MAG g_sLaserComTask_Mag;

/****************************************************************
@FUNCTION��������մ��ڹ��ܳ�ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sLaserComTask_Mag

****************************************************************/
void LaserComTask_Init(void);


/****************************************************************
@FUNCTION��������մ��ڹ��ܷ���ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sLaserComTask_Mag

****************************************************************/
void LaserComTask_DeInit(void);


/****************************************************************
@FUNCTION��������մ��ڹ�������������
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sZigComTask_Mag

****************************************************************/
void LaserComTask_Process(void);

/****************************************************************
@FUNCTION����ȡ̽ͷ������̽ͷ��
@INPUT��
    @param  MaxCnt - ���ܵ����̽ͷ��
    @param  IntVal - ����̽ͷ������ʱ����λ10ms����Ϊ0,��ȡĬ��ֵ10
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ���ظ�������ģ���������MaxCnt����IntValֵ
****************************************************************/
void LaserCom_Prot_GetOnlineNum(LASERCOMTASK_MAG *pMag, uint8_t MaxCnt, uint8_t IntVal);

/****************************************************************
@FUNCTION��LASER����ģʽ�л�����
@INPUT��LASER�ײ����ṹ��ָ�룬ģʽ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:��
****************************************************************/
void LaserCom_Prot_CtrlMode(LASERCOMTASK_MAG *pMag, LASER_MODE a_Mod);





#endif



