#ifndef UPCOM_TASK_H
#define UPCOM_TASK_H

#include "config.h"
#include "main_task.h"

#define UP_COM COM2
#define UP_COM_BAUDRATE 115200
#define UP_COM_FRAME_LEN_MAX 1024  //���֧�ֵı��ĳ���

typedef struct
{

    uint8_t 	m_byRes: 8;


    //ʵ�����Ӳ���������ɶ�д
    COM_HW_OPERATIONS m_sHwOps;

} UPCOM_LOWLEVEL_MAG;


typedef struct
{

    UPCOM_LOWLEVEL_MAG *m_pLLMag;

    uint32_t 	m_bIsInit: 1;   //�����ʼ��
    uint32_t 	m_bIsRecvNew: 1; //���յ�������
    uint32_t 	m_bRes: 30;

    COMTASK_MAG m_sTaskMag;

    //uint8_t m_byActualNum;

} UPCOMTASK_MAG;

/****************************************************************
@FUNCTION�����д��ڹ��ܳ�ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:

****************************************************************/
void UpComTask_Init(void);

/****************************************************************
@FUNCTION�����д��ڹ��ܷ���ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:

****************************************************************/
void UpComTask_DeInit(void);

/****************************************************************
@FUNCTION�����д��ڹ�������������
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:

****************************************************************/
void UpComTask_Process(void);


#endif



