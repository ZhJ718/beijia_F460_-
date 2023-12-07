#ifndef IR38COM_TASK_H
#define IR38COM_TASK_H

#include "config.h"
#include "comtask.h"
#include "main_task.h"
#include "ir.h"

#define IR38_COM_BAUDRATE 1200
#define IR38_COM_FRAME_LEN_MAX 512  //���֧�ֵı��ĳ���

typedef struct
{
    uint8_t 	m_byRes: 8;

    //ʵ�����Ӳ���������ɶ�д
    COM_HW_OPERATIONS m_sHwOps;

} IR38COM_LOWLEVEL_MAG;

typedef struct
{

    IR38COM_LOWLEVEL_MAG m_sLLMag;

    uint32_t 	m_bIsInit: 1;   //�����ʼ��
    uint32_t 	m_bIsRecvNew: 1; //���յ�������
    uint32_t 	m_bRes: 30;

    COMTASK_MAG m_sTaskMag;

} IR38COMTASK_MAG;


/****************************************************************
@FUNCTION����ȡ����̾�˿ڹ�����
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: 

****************************************************************/
IR38COMTASK_MAG *IR38ComTask_HandleGet(void);

/****************************************************************
@FUNCTION����ȡ����̾�˿����������
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: 

****************************************************************/
COMTASK_MAG *IR38ComTask_TaskHandleGet(void);

/****************************************************************
@FUNCTION������̾๦�ܳ�ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sIR38ComTask_Mag

****************************************************************/
void IR38ComTask_Init(void);

/****************************************************************
@FUNCTION������̾๦�ܷ���ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sIR38ComTask_Mag

****************************************************************/
void IR38ComTask_DeInit(void);


/****************************************************************
@FUNCTION������̾๦������������
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sIR38ComTask_Mag

****************************************************************/
void IR38ComTask_Process(void);



#endif



