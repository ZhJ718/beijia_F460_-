#ifndef __EXCOM_H
#define __EXCOM_H

#include "config.h"
#include "uart.h"

//主从交互串口 COM4

#define EXCHAN_COM  COM4
#define EXCHAN_BAUDRATE 115200


#define EXCOM_BUFFER_LEN_MAX 1024

#define EXCOM_IDX_MAX	7	//下行串口数

#define EXUART_SEND_BUFFER_LEN	256	//串口发送缓冲长度
#define EXUART_RECV_BUFFER_LEN	256	//串口接收缓冲长度
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

    EXCOM_SEQ_MAX,  //8个串口
} ENUM_COM_EX_SEQ;


typedef enum
{
    UART_PARITY_NONE = 'N',	//无校验
    UART_PARITY_EVEN = 'E',	//偶校验
    UART_PARITY_ODD = 'O',	//奇校验
} ENUM_UART_PARITY;

typedef enum
{
    UART_WORDLEN_8 = 8,	//8位数据位
    UART_WORDLEN_9 = 9,	//9位数据位
} ENUM_UART_WORDLEN;

typedef enum
{
    UART_STOPBITS_1 = 2,	//1位停止位
    UART_STOPBITS_1_5 = 3,	//1.5位停止位
    UART_STOPBITS_2 = 4,	//2位停止位
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
    uint32_t		m_dwRecvCnt;		//接收计数
    uint32_t		m_dwSendCnt;		//发送计数

    uint8_t			m_bySendBuf[EXUART_SEND_BUFFER_LEN];
    uint8_t			m_byRecvBuf[EXUART_RECV_BUFFER_LEN];

    //uint8_t 		m_byProtBuffer[UART_PROTOCAL_BUFFER_LEN];

    uint16_t		m_wRecvLen;
    uint16_t		m_wSendLen;


    uint32_t		m_dwSendOverFlowCnt;		//发送缓冲已满计数
    uint32_t		m_dwRecvOverFlowCnt;		//接收缓冲已满计数

    uint8_t		m_bIsEnable: 1;		//串口使能标志
    uint8_t 		m_bIsComRecv: 1;			//接收完成标志
    uint8_t 		m_bIsComSend: 1;			//有数据需要发送时，该位为1

    uint8_t		m_bIsSendConfig: 1;	//发送配置报文
    uint8_t		m_bIsSendEnable: 1;	//发送串口使能报文
    uint8_t		m_bIsSendDisEnable: 1;	//发送串口禁用报文


    uint8_t			m_bIsRes: 2;
    //uint8_t         m_byIsDebugMode:1;	    //调试模式

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
    //硬件初始化操作
    void (*hw_init)(void);
    //读操作
    uint32_t (*read)(uint8_t *, uint16_t *);
    //写操作
    uint32_t (*write)(uint8_t *, uint16_t );

} EXCOM_HW_OPERATIONS;

typedef struct
{
    EXCOM_HW_OPERATIONS m_sHwOps;
    //接收缓冲区
    uint8_t	m_aProtBuf[EXCOM_BUFFER_LEN_MAX];
    uint16_t	m_wProtBufLen;

    //临时缓冲区
    uint8_t	m_aTmpBuf[EXCOM_BUFFER_LEN_MAX];
    uint16_t	m_wTmpBufLen;

    uint8_t	m_byUsedExUartCnt;	//已经使用的下行串口
    EXUART_MANAGER m_saExUartMag[EXCOM_IDX_MAX];

} EXCOM_MANAGE;


//主从交互串口初始化
void ExChange_HwInit(void);

uint32_t ExChange_HwWrite(uint8_t *pBuf, uint16_t Len);

uint32_t ExChange_HwRead(uint8_t *pBuf, uint16_t *pLen);


void PrintExComMag(EXCOM_MANAGE *pMag);


/****************************************************************
@FUNCTION：获取串口索引号，串口索引号根据串口实际使能情况而改变
@INPUT：
	@@@ pMag - 串口扩展功能管理指针
	@@@ eComSeq -串口序列号
@OUTPUT：无
@RETURN: RET_TRUE-成功  RET_FALSE-失败
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint8_t ExCom_GetComIdx(EXCOM_MANAGE *pMag, ENUM_COM_EX_SEQ eComSeq);


/****************************************************************
@FUNCTION：配置扩展串口
@INPUT：
	@@@ Cfg - 配置参数
	@@@ pMag - 串口扩展功能管理指针
@OUTPUT：无
@RETURN: RET_TRUE-成功  RET_FALSE-失败
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
int32_t ExCom_ConfigUart(EXUART_CONFIG Cfg, EXCOM_MANAGE *pMag);


/****************************************************************
@FUNCTION：打开扩展串口
@INPUT：
	@@@ eExComSeq - 扩展串口序列号
	@@@ pMag - 串口扩展功能管理指针
@OUTPUT：无
@RETURN: RET_TRUE-成功  RET_FALSE-失败
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
int32_t ExCom_OpenUart(ENUM_COM_EX_SEQ eExComSeq, EXCOM_MANAGE *pMag);

/****************************************************************
@FUNCTION：关闭扩展串口
@INPUT：
	@@@ eExComSeq - 扩展串口序列号
	@@@ pMag - 串口扩展功能管理指针
@OUTPUT：无
@RETURN: RET_TRUE-成功  RET_FALSE-失败
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
int32_t ExCom_CloseUart(ENUM_COM_EX_SEQ eExComSeq, EXCOM_MANAGE *pMag);


/****************************************************************
@FUNCTION：发送透传数据到指定扩展串口
@INPUT：
	@@@ eExCom - 扩展串口序列号
	@@@ pMag - 串口扩展功能管理指针
@OUTPUT：无
@RETURN: RET_TRUE-成功  RET_FALSE-失败
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
int32_t ExCom_SendDataPayloadToSpecifyUart(ENUM_COM_EX_SEQ eExComSeq, uint8_t *pBuf, uint16_t Len, EXCOM_MANAGE *pMag);

/****************************************************************
@FUNCTION：获取指定扩展串口的接收数据载荷
@INPUT：
	@@@ eExCom - 扩展串口序列号
	@@@ pMag - 串口扩展功能管理指针
	@@@ pBuf - 接收数据缓冲区
	@@@ pLen - 接收数据缓冲区长度
@OUTPUT：
	@@@ pBuf - 接收数据
	@@@ pLen - 接收数据实际长度
@RETURN: RET_TRUE-成功  RET_FALSE-失败
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
int32_t ExCom_RecvSpecifyUartDataPayload(ENUM_COM_EX_SEQ eExComSeq, uint8_t *pBuf, uint16_t *pLen, EXCOM_MANAGE *pMag);


/****************************************************************
@FUNCTION：串口扩展板初始化
@INPUT：
	@@@ a_sHwOps - 串口扩展功能硬件操作指针
	@@@ pMag - 串口扩展功能管理指针
@OUTPUT：无
@RETURN: TRUE-暂停 FALSE-未暂停
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void ExCom_Init(EXCOM_HW_OPERATIONS a_sHwOps, EXCOM_MANAGE *pMag);


/****************************************************************
@FUNCTION：串口扩展板定时任务,主要检测串口配置等是否成功
@INPUT：
	@@@ pMag - 串口扩展功能管理指针
@OUTPUT：无
@RETURN: TRUE-暂停 FALSE-未暂停
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void ExCom_TimedProcess(EXCOM_MANAGE *pMag);


/****************************************************************
@FUNCTION：串口扩展板处理下行数据，解析后缓存在对应串口缓冲内
@INPUT：
	@@@ pMag - 串口扩展功能管理指针
@OUTPUT：无
@RETURN: TRUE-暂停 FALSE-未暂停
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint32_t ExCom_Process(EXCOM_MANAGE *pMag);


extern EXCOM_MANAGE g_sExcomMag;

#endif

