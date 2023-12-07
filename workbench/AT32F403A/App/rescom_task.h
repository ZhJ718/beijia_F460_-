#ifndef RESCOM_TASK_H
#define RESCOM_TASK_H

#include "config.h"
#include "main_task.h"

#define RES_COM COM5
#define RES_COM_BAUDRATE 115200
#define RES_COM_FRAME_LEN_MAX 2048  //���֧�ֵı��ĳ���

typedef struct
{

    uint8_t 	m_byRes: 8;


    //ʵ�����Ӳ���������ɶ�д
    COM_HW_OPERATIONS m_sHwOps;

} RESCOM_LOWLEVEL_MAG;


typedef struct
{

    RESCOM_LOWLEVEL_MAG *m_pLLMag;

    uint32_t 	m_bIsInit: 1;   //�����ʼ��
    uint32_t 	m_bIsRecvNew: 1; //���յ�������
    uint32_t 	m_bRes: 30;

    COMTASK_MAG m_sTaskMag;

    //uint8_t m_byActualNum;

} RESCOMTASK_MAG;

/****************************************************************
@FUNCTION���������ڹ��ܳ�ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sResComMag

****************************************************************/
void ResComTask_Init(void);

/****************************************************************
@FUNCTION���������ڹ��ܷ���ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sResComMag

****************************************************************/
void ResComTask_DeInit(void);

/****************************************************************
@FUNCTION���������ڹ�������������
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sResComMag

****************************************************************/
void ResComTask_Process(void);

void ResComTask_RecvSendToUpcom(uint8_t *pBuf, uint16_t BufLen, uint8_t DevIdx);

#endif



