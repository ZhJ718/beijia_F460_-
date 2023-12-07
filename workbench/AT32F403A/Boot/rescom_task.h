#ifndef RESCOM_TASK_H
#define RESCOM_TASK_H

#include "config.h"
#include "main_task.h"

#define RES_COM COM5
#define RES_COM_BAUDRATE 115200
#define RES_COM_FRAME_LEN_MAX 2048  //���֧�ֵı��ĳ���

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


#endif



