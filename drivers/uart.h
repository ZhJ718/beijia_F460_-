/**
  ******************************************************************************
  * @file    uart.h
  * @author  xfw
  * @brief   This file contains definitions for STM32091C usart and uart
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2019 Beijing HuaRu Tech Co.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of HuaRu nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART_H
#define __UART_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
//#include "stm32f0xx_hal.h"
#include "config.h"
//#include "hc32f46x_usart.h"

#if defined(AT32F4xx)

//AT32F403A  have 8 uart
typedef enum
{
    #ifdef ENABLE_COM1
    COM1,
    #endif

    #ifdef ENABLE_COM2
    COM2,
    #endif

    #ifdef ENABLE_COM3
    COM3,
    #endif

    #ifdef ENABLE_COM4
    COM4,
    #endif

    #ifdef ENABLE_COM5
    COM5,
    #endif

    #ifdef ENABLE_COM6
    COM6,
    #endif

    #ifdef ENABLE_COM7
    COM7,
    #endif

    #ifdef ENABLE_COM8
    COM8,
    #endif

    COM_MAX,  //8个串口
} ENUM_COM;


#define UART_SEND_BUFFER_LEN	1024	//串口发送缓冲长度
#define UART_RECV_BUFFER_LEN	2048	//串口接收缓冲长度
#define UART_PROTOCAL_BUFFER_LEN  (2*UART_RECV_BUFFER_LEN)


extern USART_Type *COM_TYPEDEF[COM_MAX];

typedef enum
{
    MODE_NONE = 0, 	//不发或不收
    MODE_POLL, 		//查询方式
    MODE_INTERRUPT, //中断方式
    MODE_DMA, 		//DMA方式
} UART_TXRX_MODE;

typedef struct _UART_DMA_CONFIG
{
    //若使能了DMA，需配置

    DMA_Type *m_pDMAx;
    DMA_Channel_Type* m_pDMA_CHx;
    uint32_t m_byDMA_FLAG_TC;
    //uint32_t m_dwDMA_BufferSize;
    //uint8_t m_byNVIC_IRQChannel;
    //uint8_t m_byNVIC_IRQChannelPreemptionPriority;
    //uint8_t m_byNVIC_IRQChannelSubPriority;
    //FunctionalState m_enumNVIC_IRQChannelCmd;
} UART_DMA_CONFIG, *PUART_DMA_CONFIG;

typedef struct _UART_CONFIG
{
    //必须初始化
    USART_Type 	*m_pUSARTx;
    uint32_t 	m_dwBaudRate;
    //默认关闭收发
    UART_TXRX_MODE TxMode;
    UART_TXRX_MODE RxMode;
    //若使能了中断，需配置
    IRQn_Type 	m_pUartIRQn;
    u8 	m_byPreemptionPriority;//抢占优先级
    u8 	m_bySubPriority;//子优先级
    UART_DMA_CONFIG m_sTxDMAConfig;
    UART_DMA_CONFIG m_sRxDMAConfig;
} UART_CONFIG, *PUART_CONFIG;

static const UART_CONFIG g_scUartDefConfig[COM_MAX] =
{
    {USART1, 115200, MODE_DMA, MODE_DMA, USART1_IRQn, 3, 5, {DMA1, DMA1_Channel4, DMA1_FLAG_TC4}, {DMA1, DMA1_Channel5, 0}},
    {USART2, 115200, MODE_DMA, MODE_DMA, USART2_IRQn, 3, 4, {DMA1, DMA1_Channel7, DMA1_FLAG_TC7}, {DMA1, DMA1_Channel6, 0}},
    {USART3, 115200, MODE_DMA, MODE_DMA, USART3_IRQn, 3, 3, {DMA1, DMA1_Channel2, DMA1_FLAG_TC2}, {DMA1, DMA1_Channel3, 0}},
    {UART4, 115200, MODE_DMA, MODE_DMA, UART4_IRQn, 3, 2, {DMA2, DMA2_Channel5, DMA2_FLAG_TC5}, {DMA2, DMA2_Channel3, 0}},
    {UART5, 115200, MODE_INTERRUPT, MODE_INTERRUPT, UART5_IRQn, 3, 1, {0, 0, 0}, {0, 0, 0}},
    {USART6, 115200, MODE_INTERRUPT, MODE_INTERRUPT, USART6_IRQn, 3, 6, {0, 0, 0}, {0, 0, 0}},
    {UART7, 115200, MODE_INTERRUPT, MODE_INTERRUPT, UART7_IRQn, 3, 7, {0, 0, 0}, {0, 0, 0}},
    {UART8, 115200, MODE_INTERRUPT, MODE_INTERRUPT, UART8_IRQn, 3, 8, {0, 0, 0}, {0, 0, 0}},

};

typedef struct _UART_MANAGER
{
    //UART_RECVBUFFER m_sRecvCyc;
    //UART_SENDBUFFER m_sSendCyc;
    UART_CONFIG 	m_sConfig;
    uint32_t		m_dwRecvCnt;		//接收计数
    uint32_t		m_dwSendCnt;		//发送计数

    uint8_t			m_bySendBuf[UART_SEND_BUFFER_LEN];
    uint8_t			m_byRecvBuf[UART_RECV_BUFFER_LEN];

    //uint8_t 		m_byProtBuffer[UART_PROTOCAL_BUFFER_LEN];

    uint16_t		m_wRecvLen;
    uint16_t		m_wSendLen;
    uint16_t		m_wSendIndex;
    uint16_t		m_wProtDataLen;

    uint32_t		m_dwSendOverFlowCnt;		//发送缓冲已满计数
    uint32_t		m_dwRecvOverFlowCnt;		//接收缓冲已满计数

    uint8_t 		m_bIsComRecv: 1;			//接收完成标志
    uint8_t 		m_bIsComSend: 1;			//有数据需要发送时，该位为1
    uint8_t 		m_bIsTxDMAHalf: 1;	//DMA发送半完成
    uint8_t 		m_bIsRxDMAHalf: 1;	//DMA接收半完成
    uint8_t			m_bIsRes: 4;
    //uint8_t         m_byIsDebugMode:1;	    //调试模式
} UART_MANAGER, *PUART_MANAGER;

extern void Uart_ComConfiguration(UART_CONFIG Config, UART_MANAGER *pUartMag);

extern void Uart_ComInit(UART_MANAGER *pUartMag);

extern void Uart_ComDeInit(UART_MANAGER *pUartMag);

extern void Uart_ComIRQHandle(UART_MANAGER *pUartMag);
//extern void Uart_ComIRQHandle1(UART_MANAGER *pUartMag);

extern void Uart_DmaIRQHandle(UART_MANAGER *pUartMag) ;


#elif defined(HC32F46x)

//HC32F460  have 4 uart
typedef enum
{
    #ifdef ENABLE_COM1
    COM1,
    #endif

    #ifdef ENABLE_COM2
    COM2,
    #endif

    #ifdef ENABLE_COM3
    COM3,
    #endif

    #ifdef ENABLE_COM4
    COM4,
    #endif

    #ifdef ENABLE_COM5
    COM5,
    #endif

    #ifdef ENABLE_COM6
    COM6,
    #endif

    #ifdef ENABLE_COM7
    COM7,
    #endif

    #ifdef ENABLE_COM8
    COM8,
    #endif

    COM_MAX,  //8个串口
} ENUM_COM;



#define UART_SEND_BUFFER_LEN	1024	//串口发送缓冲长度
#define UART_RECV_BUFFER_LEN	1024	//串口接收缓冲长度
#define UART_PROTOCAL_BUFFER_LEN  (2*UART_RECV_BUFFER_LEN)

extern M4_USART_TypeDef *COM_TYPEDEF[COM_MAX];

typedef enum
{
    MODE_NONE = 0, 	//不发或不收
    MODE_POLL, 		//查询方式
    MODE_INTERRUPT, //中断方式
    MODE_DMA, 		//DMA方式
} UART_TXRX_MODE;

typedef struct _UART_DMA_CONFIG
{
    //若使能了DMA，需配置

    M4_DMA_TypeDef *m_pDMAx;
    en_dma_channel_t m_eDMAChx;
    en_event_src_t  m_eDMATrgSel;

} UART_DMA_CONFIG, *PUART_DMA_CONFIG;

typedef struct _UART_CONFIG
{
    //必须初始化
    M4_USART_TypeDef 	*m_pUSARTx;
    uint32_t 	m_dwBaudRate;

    en_port_t RxPort;
    uint16_t  RxPin;
    en_port_func_t  RxFuncSel;

    en_port_t TxPort;
    uint16_t  TxPin;
    en_port_func_t  TxFuncSel;

    //默认关闭收发
    UART_TXRX_MODE TxMode;
    UART_TXRX_MODE RxMode;
    //若使能了中断，需配置
    //IRQn_Type 	m_wUartIRQn;
    //en_int_src_t   m_wUartIntSrc;

    UART_DMA_CONFIG m_sTxDMAConfig;
    UART_DMA_CONFIG m_sRxDMAConfig;
} UART_CONFIG, *PUART_CONFIG;

static const UART_CONFIG g_scUartDefConfig[COM_MAX] =
{
    {M4_USART1, 115200, PortB, Pin02, Func_Usart1_Rx, PortB, Pin01, Func_Usart1_Tx,  MODE_DMA, MODE_DMA, {M4_DMA1, DmaCh0, EVT_USART1_TI}, {M4_DMA2, DmaCh0, EVT_USART1_RI}},
    {M4_USART2, 115200, PortA, Pin10, Func_Usart2_Rx, PortA, Pin09, Func_Usart2_Tx,  MODE_DMA, MODE_DMA, {M4_DMA1, DmaCh1, EVT_USART2_TI}, {M4_DMA2, DmaCh1, EVT_USART2_RI}},
    {M4_USART3, 115200, PortB, Pin13, Func_Usart3_Rx, PortB, Pin12, Func_Usart3_Tx,  MODE_DMA, MODE_DMA, {M4_DMA1, DmaCh2, EVT_USART3_TI}, {M4_DMA2, DmaCh2, EVT_USART3_RI}},
    {M4_USART4, 115200, PortB, Pin07, Func_Usart4_Rx, PortB, Pin06, Func_Usart4_Tx,  MODE_DMA, MODE_DMA, {M4_DMA1, DmaCh3, EVT_USART4_TI}, {M4_DMA2, DmaCh3, EVT_USART4_RI}},
};

typedef struct _UART_MANAGER
{
    //UART_RECVBUFFER m_sRecvCyc;
    //UART_SENDBUFFER m_sSendCyc;
    UART_CONFIG 	m_sConfig;
    uint32_t		m_dwRecvCnt;		//接收计数
    uint32_t		m_dwSendCnt;		//发送计数

    uint8_t			m_bySendBuf[UART_SEND_BUFFER_LEN];
    uint8_t			m_byRecvBuf[UART_RECV_BUFFER_LEN];

    //uint8_t 		m_byProtBuffer[UART_PROTOCAL_BUFFER_LEN];

    uint16_t		m_wRecvLen;
    uint16_t		m_wSendLen;
    uint16_t    	m_wLastDmaCnt;
    uint16_t		m_wSentLen;
    uint16_t		m_wProtDataLen;

    uint32_t		m_dwSendOverFlowCnt;		//发送缓冲已满计数
    uint32_t		m_dwRecvOverFlowCnt;		//接收缓冲已满计数

    uint8_t 		m_bIsComRecv: 1;			//接收完成标志
    uint8_t 		m_bIsComSend: 1;			//有数据需要发送时，该位为1
    uint8_t 		m_bIsTxDMAHalf: 1;	//DMA发送半完成
    uint8_t 		m_bIsRxDMAHalf: 1;	//DMA接收半完成
    uint8_t			m_bIsRes: 4;
    //uint8_t         m_byIsDebugMode:1;	    //调试模式

    uint32_t        m_dwSendTime; //记录DMA 发送下发送时间
} UART_MANAGER, *PUART_MANAGER;


extern void Uart_ComConfiguration(UART_CONFIG Config, UART_MANAGER *pUartMag);

extern void Uart_ComInit(UART_MANAGER *pUartMag);

void Uart_ComDeInit(UART_MANAGER *pUartMag);


#endif

extern uint32_t Uart_SendBuf(UART_MANAGER *pUartMag, uint8_t *pBuf, uint32_t len);
//如果中断连发，会覆盖原有未发完数据
//extern uint32_t Uart_SendBuf_GPS(UART_MANAGER *pUartMag,uint8_t *pBuf,uint32_t len);

extern void Uart_DebugPrintf(UART_MANAGER *pUartMag, char* fmt, ...) ;

//待测试串口和调试口连通
extern void UartTestToDebug(UART_MANAGER *pTestUartMag, UART_MANAGER *pDebugUartMag);

//串口回环测试
extern void UartLoopTest(UART_MANAGER *pTestUartMag);


typedef struct _UART_SET_MANAGER
{
    UART_MANAGER Com[COM_MAX];
} UART_SET_MANAGER, *PUART_SET_MANAGER;

extern UART_SET_MANAGER g_sUartSetMag;

#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
