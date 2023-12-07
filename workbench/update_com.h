/******************************************************************
*��Ȩ���� (C)2020, ���ջ������Ƽ����޹�˾
*
*�ļ�����:  upcom.h
*�ļ���ʶ:
*����ժҪ:
*����˵��:
*��ǰ�汾:
*��    ��:
*�������:  2021/02/03
*
*�޸ļ�¼1:
*    �޸�����:
*    �� �� ��:
*    �� �� ��:
*    �޸�����:
******************************************************************/
#ifndef  _UPDATE_COM_H
#define  _UPDATE_COM_H
/******************************************************************
*                             ͷ�ļ�                              *
******************************************************************/
#include "config.h"

//#define UPDATECOM_PROT_BUFFER_LEN_MAX 512

//Ӳ������
typedef struct
{
    //Ӳ����ʼ������
    void (*hw_init)(void);

    //������
    uint32_t (*read)(uint8_t *, uint16_t *);
    //д����
    uint32_t (*write)(uint8_t *, uint16_t );

    #if 0
    //�ϵ�
    void (*gpio_power_on)(void);
    //�ϵ�
    void (*gpio_power_off)(void);

    //Ӳ����λ��������
    void (*gpio_reset)(void);
    //Ӳ����λ��������
    void (*gpio_set)(void);
    #endif

} UPDATECOM_HW_OPERATIONS;

typedef struct
{

    UPDATECOM_HW_OPERATIONS m_sHwOps;

    uint32_t 	m_bIsInit: 1;
    uint32_t 	m_bRes: 31;

    uint32_t	m_dwNoCommTimeout;	//ͨѶ��ʱ���


    //���ջ�����
    //uint8_t	m_aProtBuf[UPDATECOM_PROT_BUFFER_LEN_MAX];
    uint8_t     *m_pProtBuf;   //����ʵ���������
    uint16_t    m_wProtBufMaxLen; //��󲻳���65535
    uint16_t	m_wProtBufLen;

} UPDATECOM_MAG;

/****************************************************************
@FUNCTION��������������󻺴�Э�鳤��
@INPUT��
	UPCOM_MAG	-	ͨѶ�ڹ���ָ�룬����Ϊ��
    Len   - ��󳤶�
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��Ҫָ����������󻺴泤�ȣ���󲻳���8k

****************************************************************/
void UpdateCom_SetMaxPackLen(UPDATECOM_MAG *pMag, uint32_t Len);


/****************************************************************
@FUNCTION�������ڳ�ʼ��
@INPUT��
	UPCOM_HW_OPERATIONS - ����ͨѶ����صײ��������
	UPCOM_MAG	-	����ͨѶ�ڹ���ָ�룬����Ϊ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:

****************************************************************/
void UpdateCom_Init(UPDATECOM_HW_OPERATIONS a_sHwOps, UPDATECOM_MAG *pMag);

/****************************************************************
@FUNCTION�������ڳ�ʼ������
@INPUT��
	UPCOM_MAG	-	����ͨѶ�ڹ���ָ�룬����Ϊ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��ʼ����

****************************************************************/
void UpdateCom_DeInit(UPDATECOM_MAG *pMag);

/****************************************************************
@FUNCTION���������ڽ��մ������
@INPUT��
	UPDATECOM_MAG	-	�������ڹ���ָ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ֧������

****************************************************************/
void UpdateCom_RecvProcess(UPDATECOM_MAG *pMag);

/****************************************************************
@FUNCTION���������ڽ��մ��� - �ҵ�����Э���ʽ�ı���
@INPUT��
	UPDATECOM_MAG	-	�������ڹ���ָ��
@OUTPUT: ��
@RETURN: 0 -  δ�ҵ�,  1 - �ҵ�
@AUTHOR��xfw
@SPECIAL:

****************************************************************/
uint32_t UpdateCom_Find55AAFrame(UPDATECOM_MAG *pMag);

#endif


