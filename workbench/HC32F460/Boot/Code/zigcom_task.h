#ifndef ZIGCOM_TASK_H
#define ZIGCOM_TASK_H

#include "config.h"
#include "main_task.h"
#include "new_zigbee.h"

#if (BOARDID == 1)
    #define ZIG_COM COM1
#else  (BOARDID == 2)
    #define ZIG_COM COM4
#endif

#define ZIG_COM_BAUDRATE 115200
#define ZIG_COM_FRAME_LEN_MAX 256  //���֧�ֵı��ĳ���,zigbeeģ��Ӳ�������˽��ճ��Ȳ�����90�������Ϊ�����ݶ���



typedef struct
{

    NEW_ZIGBEE_LOWLEVEL_MAG *m_pLLMag;

    uint32_t 	m_bIsInit: 1;   //�����ʼ��
    uint32_t 	m_bIsRecvNew: 1; //���յ�������

    uint32_t 	m_bRes: 30;

    COMTASK_MAG m_sTaskMag;

    uint8_t     m_aLastCommMacBuf[8];   //����ͨѶzigbee MAC��ַ
    uint8_t     m_byLastCommMode;   //���½������ݷ�ʽ��0-��Ե㣬1-�㲥
    uint8_t     m_byLastCommRSSI;   //����ͨѶ�ź�ǿ��


} ZIGCOMTASK_MAG;


extern ZIGCOMTASK_MAG g_sZigComTask_Mag;


/****************************************************************
@FUNCTION��zigbee ���ڹ��ܳ�ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sZigComTask_Mag

****************************************************************/
void ZigComTask_Init(void);

/****************************************************************
@FUNCTION��zigbee ���ڹ��ܷ���ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sZigComTask_Mag

****************************************************************/
void ZigComTask_DeInit(void);

/****************************************************************
@FUNCTION��zigbee ���ڹ�������������
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sZigComTask_Mag

****************************************************************/
void ZigComTask_Process(void);


#endif



