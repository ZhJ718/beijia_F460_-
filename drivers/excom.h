#ifndef __EXCOM_H
#define __EXCOM_H

#include "config.h"
#include "uart.h"

//���ӽ������� COM4

#define EXCHAN_COM  COM4
#define EXCHAN_BAUDRATE 115200


#define EXCOM_BUFFER_LEN_MAX 1024

#define EXCOM_IDX_MAX	7	//���д�����

#define EXUART_SEND_BUFFER_LEN	256	//���ڷ��ͻ��峤��
#define EXUART_RECV_BUFFER_LEN	256	//���ڽ��ջ��峤��
#define EXUART_PROTOCAL_BUFFER_LEN  (2*EXUART_RECV_BUFFER_LEN)


typedef enum
{

    EXCOM1,

    EXCOM2,

    EXCOM3,

    EXCOM4,

    EXCOM5,

    EXCOM6,

    EXCOM7,

    EXCOM8,

    EXCOM_SEQ_MAX,  //8������
} ENUM_COM_EX_SEQ;


typedef enum
{
    UART_PARITY_NONE = 'N',	//��У��
    UART_PARITY_EVEN = 'E',	//żУ��
    UART_PARITY_ODD = 'O',	//��У��
} ENUM_UART_PARITY;

typedef enum
{
    UART_WORDLEN_8 = 8,	//8λ����λ
    UART_WORDLEN_9 = 9,	//9λ����λ
} ENUM_UART_WORDLEN;

typedef enum
{
    UART_STOPBITS_1 = 2,	//1λֹͣλ
    UART_STOPBITS_1_5 = 3,	//1.5λֹͣλ
    UART_STOPBITS_2 = 4,	//2λֹͣλ
} ENUM_UART_STOPBITS;




typedef struct _EXUART_CONFIG
{
    ENUM_COM_EX_SEQ  m_byComSeq;
    uint32_t m_dwBaudrate;
    ENUM_UART_PARITY m_eParity;
    ENUM_UART_WORDLEN m_eWordLength;
    ENUM_UART_STOPBITS m_eStopBits;
} EXUART_CONFIG, *PEXUART_CONFIG;

typedef struct
{
    EXUART_CONFIG 	m_sConfig;
    uint32_t		m_dwRecvCnt;		//���ռ���
    uint32_t		m_dwSendCnt;		//���ͼ���

    uint8_t			m_bySendBuf[EXUART_SEND_BUFFER_LEN];
    uint8_t			m_byRecvBuf[EXUART_RECV_BUFFER_LEN];

    //uint8_t 		m_byProtBuffer[UART_PROTOCAL_BUFFER_LEN];

    uint16_t		m_wRecvLen;
    uint16_t		m_wSendLen;


    uint32_t		m_dwSendOverFlowCnt;		//���ͻ�����������
    uint32_t		m_dwRecvOverFlowCnt;		//���ջ�����������

    uint8_t		m_bIsEnable: 1;		//����ʹ�ܱ�־
    uint8_t 		m_bIsComRecv: 1;			//������ɱ�־
    uint8_t 		m_bIsComSend: 1;			//��������Ҫ����ʱ����λΪ1

    uint8_t		m_bIsSendConfig: 1;	//�������ñ���
    uint8_t		m_bIsSendEnable: 1;	//���ʹ���ʹ�ܱ���
    uint8_t		m_bIsSendDisEnable: 1;	//���ʹ��ڽ��ñ���


    uint8_t			m_bIsRes: 2;
    //uint8_t         m_byIsDebugMode:1;	    //����ģʽ

} EXUART_MANAGER, *PEXUART_MANAGER;


static const EXUART_CONFIG g_scExUartDefConfig[EXCOM_SEQ_MAX] =
{
    {EXCOM1, 115200, UART_PARITY_NONE, UART_WORDLEN_8, UART_STOPBITS_1},
    {EXCOM2, 115200, UART_PARITY_NONE, UART_WORDLEN_8, UART_STOPBITS_1},
    {EXCOM3, 115200, UART_PARITY_NONE, UART_WORDLEN_8, UART_STOPBITS_1},
    {EXCOM4, 115200, UART_PARITY_NONE, UART_WORDLEN_8, UART_STOPBITS_1},
    {EXCOM5, 115200, UART_PARITY_NONE, UART_WORDLEN_8, UART_STOPBITS_1},
    {EXCOM6, 115200, UART_PARITY_NONE, UART_WORDLEN_8, UART_STOPBITS_1},
    {EXCOM7, 115200, UART_PARITY_NONE, UART_WORDLEN_8, UART_STOPBITS_1},
    {EXCOM8, 115200, UART_PARITY_NONE, UART_WORDLEN_8, UART_STOPBITS_1},
};


typedef struct
{
    //Ӳ����ʼ������
    void (*hw_init)(void);
    //������
    uint32_t (*read)(uint8_t *, uint16_t *);
    //д����
    uint32_t (*write)(uint8_t *, uint16_t );

} EXCOM_HW_OPERATIONS;

typedef struct
{
    EXCOM_HW_OPERATIONS m_sHwOps;
    //���ջ�����
    uint8_t	m_aProtBuf[EXCOM_BUFFER_LEN_MAX];
    uint16_t	m_wProtBufLen;

    //��ʱ������
    uint8_t	m_aTmpBuf[EXCOM_BUFFER_LEN_MAX];
    uint16_t	m_wTmpBufLen;

    uint8_t	m_byUsedExUartCnt;	//�Ѿ�ʹ�õ����д���
    EXUART_MANAGER m_saExUartMag[EXCOM_IDX_MAX];

} EXCOM_MANAGE;


//���ӽ������ڳ�ʼ��
void ExChange_HwInit(void);

uint32_t ExChange_HwWrite(uint8_t *pBuf, uint16_t Len);

uint32_t ExChange_HwRead(uint8_t *pBuf, uint16_t *pLen);


void PrintExComMag(EXCOM_MANAGE *pMag);


/****************************************************************
@FUNCTION����ȡ���������ţ����������Ÿ��ݴ���ʵ��ʹ��������ı�
@INPUT��
	@@@ pMag - ������չ���ܹ���ָ��
	@@@ eComSeq -�������к�
@OUTPUT����
@RETURN: RET_TRUE-�ɹ�  RET_FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint8_t ExCom_GetComIdx(EXCOM_MANAGE *pMag, ENUM_COM_EX_SEQ eComSeq);


/****************************************************************
@FUNCTION��������չ����
@INPUT��
	@@@ Cfg - ���ò���
	@@@ pMag - ������չ���ܹ���ָ��
@OUTPUT����
@RETURN: RET_TRUE-�ɹ�  RET_FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
int32_t ExCom_ConfigUart(EXUART_CONFIG Cfg, EXCOM_MANAGE *pMag);


/****************************************************************
@FUNCTION������չ����
@INPUT��
	@@@ eExComSeq - ��չ�������к�
	@@@ pMag - ������չ���ܹ���ָ��
@OUTPUT����
@RETURN: RET_TRUE-�ɹ�  RET_FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
int32_t ExCom_OpenUart(ENUM_COM_EX_SEQ eExComSeq, EXCOM_MANAGE *pMag);

/****************************************************************
@FUNCTION���ر���չ����
@INPUT��
	@@@ eExComSeq - ��չ�������к�
	@@@ pMag - ������չ���ܹ���ָ��
@OUTPUT����
@RETURN: RET_TRUE-�ɹ�  RET_FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
int32_t ExCom_CloseUart(ENUM_COM_EX_SEQ eExComSeq, EXCOM_MANAGE *pMag);


/****************************************************************
@FUNCTION������͸�����ݵ�ָ����չ����
@INPUT��
	@@@ eExCom - ��չ�������к�
	@@@ pMag - ������չ���ܹ���ָ��
@OUTPUT����
@RETURN: RET_TRUE-�ɹ�  RET_FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
int32_t ExCom_SendDataPayloadToSpecifyUart(ENUM_COM_EX_SEQ eExComSeq, uint8_t *pBuf, uint16_t Len, EXCOM_MANAGE *pMag);

/****************************************************************
@FUNCTION����ȡָ����չ���ڵĽ��������غ�
@INPUT��
	@@@ eExCom - ��չ�������к�
	@@@ pMag - ������չ���ܹ���ָ��
	@@@ pBuf - �������ݻ�����
	@@@ pLen - �������ݻ���������
@OUTPUT��
	@@@ pBuf - ��������
	@@@ pLen - ��������ʵ�ʳ���
@RETURN: RET_TRUE-�ɹ�  RET_FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
int32_t ExCom_RecvSpecifyUartDataPayload(ENUM_COM_EX_SEQ eExComSeq, uint8_t *pBuf, uint16_t *pLen, EXCOM_MANAGE *pMag);


/****************************************************************
@FUNCTION��������չ���ʼ��
@INPUT��
	@@@ a_sHwOps - ������չ����Ӳ������ָ��
	@@@ pMag - ������չ���ܹ���ָ��
@OUTPUT����
@RETURN: TRUE-��ͣ FALSE-δ��ͣ
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void ExCom_Init(EXCOM_HW_OPERATIONS a_sHwOps, EXCOM_MANAGE *pMag);


/****************************************************************
@FUNCTION��������չ�嶨ʱ����,��Ҫ��⴮�����õ��Ƿ�ɹ�
@INPUT��
	@@@ pMag - ������չ���ܹ���ָ��
@OUTPUT����
@RETURN: TRUE-��ͣ FALSE-δ��ͣ
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void ExCom_TimedProcess(EXCOM_MANAGE *pMag);


/****************************************************************
@FUNCTION��������չ�崦���������ݣ������󻺴��ڶ�Ӧ���ڻ�����
@INPUT��
	@@@ pMag - ������չ���ܹ���ָ��
@OUTPUT����
@RETURN: TRUE-��ͣ FALSE-δ��ͣ
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint32_t ExCom_Process(EXCOM_MANAGE *pMag);


extern EXCOM_MANAGE g_sExcomMag;

#endif

