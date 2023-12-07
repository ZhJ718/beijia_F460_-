/**
  ******************************************************************************
  * @file    uart.c
  * @author  xfw
  * @brief   This file contains definitions for MCU usart and uart
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2023 Beijing HuaRu Tech Co.</center></h2>
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

#define CPP_GLOBALS

/* Includes ------------------------------------------------------------------*/
#include "uart.h"
#include "malloc.h"

#if defined(AT32F4xx)

UART_SET_MANAGER g_sUartSetMag;

USART_Type *COM_TYPEDEF[COM_MAX] =
{

    #ifdef ENABLE_COM1
    USART1,
    #endif

    #ifdef ENABLE_COM2
    USART2,
    #endif

    #ifdef ENABLE_COM3
    USART3,
    #endif

    #ifdef ENABLE_COM4
    UART4,
    #endif

    #ifdef ENABLE_COM5
    UART5,
    #endif

    #ifdef ENABLE_COM6
    USART6,
    #endif

    #ifdef ENABLE_COM7
    UART7,
    #endif

    #ifdef ENABLE_COM8
    UART8,
    #endif
};

/*
void UartClockInit(USART_Type *USARTx)
{
	if(USARTx==USART3){
		//RCC_AHBENR_GPIODEN   RCC_APB1EN_USART3EN  PD8/PD9
		PeripheralClockInit(GPIOD,AHB);
		PeripheralClockInit(USART3,APB1);
	}else if(USARTx==USART1){
		//RCC_APB2EN_USART1EN  PA19/PA10
		PeripheralClockInit(GPIOA,AHB);
		PeripheralClockInit(USART1,APB2);
	}else if(USARTx==USART2){
		//RCC_APB1EN_USART2EN  PD5/PD6
		PeripheralClockInit(GPIOD,AHB);
		PeripheralClockInit(USART2,APB1);
	}else if(USARTx==USART4){
		//RCC_APB1ENR_USART4EN  PC10/PC11
		PeripheralClockInit(GPIOC,AHB);
		PeripheralClockInit(USART4,APB1);
	}else if(USARTx==USART5){
		//RCC_APB1ENR_USART5EN  PC12/PD2
		PeripheralClockInit(GPIOC,AHB);
		PeripheralClockInit(GPIOD,AHB);
		PeripheralClockInit(USART5,APB1);
	}else if(USARTx==USART6){
		//RCC_APB2ENR_USART6EN  PA4/PA5
		PeripheralClockInit(GPIOA,AHB);
		PeripheralClockInit(USART6,APB2);
	}else if(USARTx==USART7){
		//RCC_APB2ENR_USART7EN  PC6/PC7
		PeripheralClockInit(GPIOC,AHB);
		PeripheralClockInit(USART7,APB2);
	}else if(USARTx==USART8){
		//RCC_APB2ENR_USART8EN  PD13/PD14
		PeripheralClockInit(GPIOD,AHB);
		PeripheralClockInit(USART8,APB2);
	}
}

*/

static void Uart_GpioConfig(USART_Type *USARTx)
{
    GPIO_InitType  GPIO_InitStructure;

    /* Configure the pin */
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;

    if(USARTx == USART1)
    {
        //enable gpio clock
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);
        //RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO, ENABLE);

        //USART1_GRMP[3:0]=0001或USART1_REMAP=1
        //GPIO_PinsRemapConfig(AFIO_MAP6_USART1_0001, ENABLE);

        /* USART1 使用IO端口配置 PA9/PA10 */
        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_9;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
        GPIO_Init(GPIOA, &GPIO_InitStructure);   //初始化GPIOA
    }
    else if(USARTx == USART2)
    {
        //enable gpio clock
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);
        /* USART2 使用IO端口配置 PA2/PA3 */
        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_2;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_3;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
        GPIO_Init(GPIOA, &GPIO_InitStructure);   //初始化GPIOA
    }
    else if(USARTx == USART3)
    {
        //enable gpio clock
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);
        /* USART3 使用IO端口配置 PB10/PB11 */
        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10; //PB10
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
        GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化PB10
        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_11;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
        GPIO_Init(GPIOB, &GPIO_InitStructure);  //初始化PB11
    }
    else if(USARTx == UART4)
    {
        //enable gpio clock
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOC, ENABLE);
        /* UART4 使用IO端口配置 PC10/PC11 */
        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
        GPIO_Init(GPIOC, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_11;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
        GPIO_Init(GPIOC, &GPIO_InitStructure);   //初始化GPIOA
    }
    else if(USARTx == UART5)
    {
        //enable gpio clock
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOC, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOD, ENABLE);
        /* UART5 使用IO端口配置 PC12/PD2 */
        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_12; //PC12
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
        GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化PC12
        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_2;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
        GPIO_Init(GPIOD, &GPIO_InitStructure);  //初始化PD2
    }
    else if(USARTx == USART6)
    {
        //enable gpio clock
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOC, ENABLE);
        /* USART6 使用IO端口配置 PC6/PC7 */
        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
        GPIO_Init(GPIOC, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_7;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
        GPIO_Init(GPIOC, &GPIO_InitStructure);   //初始化GPIOA
    }
    else if(USARTx == UART7)
    {
        //enable gpio clock
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOE, ENABLE);
        /* USART6 使用IO端口配置 PE8/PE7 */
        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
        GPIO_Init(GPIOE, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_7;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
        GPIO_Init(GPIOE, &GPIO_InitStructure);   //初始化GPIOA
    }
    else if(USARTx == UART8)
    {
        //enable gpio clock
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOE, ENABLE);
        /* USART6 使用IO端口配置 PE1/PE0 */
        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_1;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
        GPIO_Init(GPIOE, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pins = GPIO_Pins_0;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
        GPIO_Init(GPIOE, &GPIO_InitStructure);   //初始化GPIOA
    }

}




static void Uart_EnableClock(USART_Type *USARTx)
{
    if(USARTx == USART1)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_USART1, ENABLE);//串口1时钟使能
    }
    else if(USARTx == USART2)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_USART2, ENABLE);//串口2时钟使能
    }
    else if(USARTx == USART3)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_USART3, ENABLE); //串口3时钟使能
    }
    else if(USARTx == UART4)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_UART4, ENABLE);//串口4时钟使能
    }
    else if(USARTx == UART5)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_UART5, ENABLE); //串口5时钟使能
    }
    else if(USARTx == USART6)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_USART6, ENABLE); //串口6时钟使能
    }
    else if(USARTx == UART7)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_UART7, ENABLE); //串口7时钟使能
    }
    else if(USARTx == UART8)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_UART8, ENABLE); //串口8时钟使能
    }

}

static void Uart_NvicConfig(UART_CONFIG *pConfig)
{
    NVIC_InitType NVIC_InitStructure;

    //enable usart nvic
    if(pConfig->TxMode == MODE_INTERRUPT
            || pConfig->TxMode == MODE_DMA
            || pConfig->RxMode == MODE_INTERRUPT
            || pConfig->RxMode == MODE_DMA)
    {
        NVIC_InitStructure.NVIC_IRQChannel = pConfig->m_pUartIRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = pConfig->m_byPreemptionPriority; //抢占优先级2
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = pConfig->m_bySubPriority;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
    }

    //enable dma nvic
    //if(pConfig->TxMode == MODE_DMA){
    //	NVIC_SetPriority(pConfig->m_sTxDMAConfig.m_pDMAIRQn,pConfig->m_sTxDMAConfig.m_dwDMAPriority);
    //	NVIC_EnableIRQ(pConfig->m_sTxDMAConfig.m_pDMAIRQn);
    //}
}


//LL_DMA_EnableIT_TC

//串口DMA发送使能，使用发送完成中断
static void Uart_EnableDmaTx(UART_MANAGER *pUartMag)
{
    UART_CONFIG *pConfig = &pUartMag->m_sConfig;
    UART_DMA_CONFIG *pDMAConfig = &pConfig->m_sTxDMAConfig;
    //串口收DMA配置
    DMA_InitType DMA_InitStruct;
    //NVIC_InitType NVIC_InitStructure;     /* Configure one bit for preemption priority */

    //enable DMA clock
    if(pDMAConfig->m_pDMAx == DMA1)
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1, ENABLE);  //使能DMA时钟
    }
    else
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA2, ENABLE);  //使能DMA时钟
    }

    DMA_Reset(pDMAConfig->m_pDMA_CHx);   //

    DMA_InitStruct.DMA_PeripheralBaseAddr  = (uint32_t)(&pConfig->m_pUSARTx->DT);
    DMA_InitStruct.DMA_MemoryBaseAddr  = (uint32_t)(pUartMag->m_bySendBuf);
    DMA_InitStruct.DMA_Direction              = DMA_DIR_PERIPHERALDST;
    DMA_InitStruct.DMA_BufferSize = UART_SEND_BUFFER_LEN;  //DMA通道的DMA缓存的大小
    DMA_InitStruct.DMA_Mode                   = DMA_MODE_NORMAL;
    DMA_InitStruct.DMA_PeripheralInc  = DMA_PERIPHERALINC_DISABLE;
    DMA_InitStruct.DMA_MemoryInc  = DMA_MEMORYINC_ENABLE;
    DMA_InitStruct.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_BYTE;
    DMA_InitStruct.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_BYTE;
    DMA_InitStruct.DMA_Priority = DMA_PRIORITY_MEDIUM; //DMA通道 x拥有中优先级
    DMA_InitStruct.DMA_MTOM = DMA_MEMTOMEM_DISABLE;  //DMA通道x没有设置为内存到内存传输

    DMA_Init(pDMAConfig->m_pDMA_CHx, &DMA_InitStruct);
    DMA_ChannelEnable(pDMAConfig->m_pDMA_CHx, DISABLE);
    DMA_INTConfig(pDMAConfig->m_pDMA_CHx, DMA_INT_TC, ENABLE);
    DMA_ClearITPendingBit(DMA_INT_TC);

    //配置中断
    /*
    NVIC_InitStructure.NVIC_IRQChannel = pDMAConfig->m_byNVIC_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = pDMAConfig->m_byNVIC_IRQChannelPreemptionPriority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = pDMAConfig->m_byNVIC_IRQChannelSubPriority;
    NVIC_InitStructure.NVIC_IRQChannelCmd = pDMAConfig->m_enumNVIC_IRQChannelCmd;
    NVIC_Init(&NVIC_InitStructure);*/
}

//串口DMA接收使能
static void Uart_EnableDmaRx(UART_MANAGER *pUartMag)
{
    UART_CONFIG *pConfig = &pUartMag->m_sConfig;
    UART_DMA_CONFIG *pDMAConfig = &pConfig->m_sRxDMAConfig;
    //串口收DMA配置
    DMA_InitType DMA_InitStruct = {0};

    if(pDMAConfig->m_pDMAx == DMA1)
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1, ENABLE);  //使能DMA时钟
    }
    else
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA2, ENABLE);  //使能DMA时钟
    }

    DMA_Reset(pDMAConfig->m_pDMA_CHx);   //

    DMA_InitStruct.DMA_PeripheralBaseAddr  = (uint32_t)(&pConfig->m_pUSARTx->DT);
    DMA_InitStruct.DMA_MemoryBaseAddr  = (uint32_t)(pUartMag->m_byRecvBuf);
    DMA_InitStruct.DMA_Direction              = DMA_DIR_PERIPHERALSRC;
    DMA_InitStruct.DMA_BufferSize = UART_RECV_BUFFER_LEN;  //DMA通道的DMA缓存的大小
    DMA_InitStruct.DMA_Mode                   = DMA_MODE_NORMAL;
    DMA_InitStruct.DMA_PeripheralInc  = DMA_PERIPHERALINC_DISABLE;
    DMA_InitStruct.DMA_MemoryInc  = DMA_MEMORYINC_ENABLE;
    DMA_InitStruct.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_BYTE;
    DMA_InitStruct.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_BYTE;
    DMA_InitStruct.DMA_Priority = DMA_PRIORITY_VERYHIGH; //DMA通道 x拥有中优先级
    DMA_InitStruct.DMA_MTOM = DMA_MEMTOMEM_DISABLE;  //DMA通道x没有设置为内存到内存传输

    DMA_Init(pDMAConfig->m_pDMA_CHx, &DMA_InitStruct);
    DMA_ChannelEnable(pDMAConfig->m_pDMA_CHx, ENABLE); //使能接收通道
}

extern void Uart_ComConfiguration(UART_CONFIG Config, UART_MANAGER *pUartMag)
{
    memcpy((uint8_t *)&pUartMag->m_sConfig, (uint8_t *)&Config, sizeof(UART_CONFIG));
}


void Uart_ComInit(UART_MANAGER *pUartMag)
{
    UART_CONFIG *pConfig = &pUartMag->m_sConfig;
    USART_InitType USART_InitStructure;
    //LL_USART_ClockInitTypeDef USART_ClockInitStruct;

    Uart_ComDeInit(pUartMag);

    Uart_EnableClock(pConfig->m_pUSARTx);
    USART_Reset(pConfig->m_pUSARTx);
    USART_InitStructure.USART_WordLength           = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_BaudRate = pUartMag->m_sConfig.m_dwBaudRate;

    if((pConfig->RxMode != MODE_NONE) && (pConfig->TxMode != MODE_NONE))
    {
        USART_InitStructure.USART_Mode   = USART_Mode_Rx | USART_Mode_Tx;
    }
    else if(pConfig->RxMode != MODE_NONE)
    {
        USART_InitStructure.USART_Mode   = USART_Mode_Rx;
    }
    else if(pConfig->TxMode != MODE_NONE)
    {
        USART_InitStructure.USART_Mode   = USART_Mode_Tx;
    }
    else
    {
        USART_InitStructure.USART_Mode	= 0;
    }

    USART_Init(pConfig->m_pUSARTx, &USART_InitStructure);

    //LL_USART_ClockStructInit(&USART_ClockInitStruct);
    //LL_USART_ClockInit(USART3, &USART_ClockInitStruct);

    //enable interrupt or DMA
    if(pConfig->RxMode == MODE_INTERRUPT)
    {
        USART_INTConfig(pConfig->m_pUSARTx, USART_INT_RDNE, ENABLE);
        USART_INTConfig(pConfig->m_pUSARTx, USART_INT_IDLEF, ENABLE);
    }
    else if(pConfig->RxMode == MODE_DMA)
    {
        Uart_EnableDmaRx(pUartMag);
        USART_DMACmd(pConfig->m_pUSARTx, USART_DMAReq_Rx, ENABLE); //使能串口的DMA接收
        USART_INTConfig(pConfig->m_pUSARTx, USART_INT_IDLEF, ENABLE);
    }

    if(pConfig->TxMode == MODE_INTERRUPT)
    {

    }
    else if(pConfig->TxMode == MODE_DMA)
    {
        Uart_EnableDmaTx(pUartMag);
        USART_DMACmd(pConfig->m_pUSARTx, USART_DMAReq_Tx, ENABLE); //使能串口的DMA发送
    }

    //start usart3
    USART_Cmd(pConfig->m_pUSARTx, ENABLE);

    //clear all flag
    //REG_WRITE(pConfig->m_pUSARTx->ICR,0xffffffff);
    USART_ClearFlag(pConfig->m_pUSARTx, USART_FLAG_TRAC);

    Uart_GpioConfig(pConfig->m_pUSARTx);//防止上电使能TX，发送0的问题

    //enable usart nvic
    Uart_NvicConfig(pConfig);
}

void Uart_ComDeInit(UART_MANAGER *pUartMag)
{
    USART_Reset(pUartMag->m_sConfig.m_pUSARTx);
}

void Uart_ComIRQHandle(UART_MANAGER *pUartMag)
{
    volatile uint32_t dat;
    UART_DMA_CONFIG *pDmaConfig = NULL;
    USART_Type *pUsartType = pUartMag->m_sConfig.m_pUSARTx;

    if (USART_GetITStatus(pUsartType, USART_INT_ORERR) != RESET)
    {
        USART_ClearITPendingBit(pUsartType, USART_INT_ORERR);
    }

    if(USART_GetITStatus(pUsartType, USART_INT_RDNE) != RESET)
    {
        dat = (u8)USART_ReceiveData(pUsartType);
        USART_ClearITPendingBit(pUsartType, USART_INT_RDNE);

        pUartMag->m_byRecvBuf[pUartMag->m_wRecvLen] = dat;
        pUartMag->m_wRecvLen += 1;

        if(pUartMag->m_wRecvLen >= (UART_RECV_BUFFER_LEN / 2))
        {
            pUartMag->m_bIsComRecv = TRUE;
        }

        //pUartMag->m_dwRecvCnt++;
    }

    if(USART_GetITStatus(pUsartType, USART_INT_IDLEF) != RESET)
    {
        dat = REG_READ(pUsartType->STS);
        dat = REG_READ(pUsartType->DT);

        USART_ClearITPendingBit(pUsartType, USART_INT_IDLEF);


        if(pUartMag->m_sConfig.RxMode == MODE_DMA)
        {
            pDmaConfig = &pUartMag->m_sConfig.m_sRxDMAConfig;
            DMA_ChannelEnable(pDmaConfig->m_pDMA_CHx, DISABLE);   //关闭DMA
            pUartMag->m_wRecvLen = sizeof(pUartMag->m_byRecvBuf) - DMA_GetCurrDataCounter(pDmaConfig->m_pDMA_CHx);
            DMA_SetCurrDataCounter(pDmaConfig->m_pDMA_CHx, sizeof(pUartMag->m_byRecvBuf));
            DMA_ChannelEnable(pDmaConfig->m_pDMA_CHx, ENABLE); //开启DMA
            pUartMag->m_bIsComRecv = TRUE;
        }

        if(pUartMag->m_sConfig.RxMode == MODE_INTERRUPT)
        {
            pUartMag->m_bIsComRecv = TRUE;
        }

        pUartMag->m_bIsComSend = FALSE;
    }

    if(USART_GetITStatus(pUsartType, USART_INT_TDE) != RESET)
    {
        if(pUartMag->m_wSendIndex >= pUartMag->m_wSendLen)
        {
            /* Disable the UART Transmit Data Register Empty Interrupt */
            USART_INTConfig(pUartMag->m_sConfig.m_pUSARTx, USART_INT_TDE, DISABLE);
            pUartMag->m_bIsComSend = FALSE;
            /* Enable the UART Transmit Complete Interrupt */
            USART_INTConfig(pUsartType, USART_INT_TRAC, ENABLE);

        }
        else
        {
            pUartMag->m_bIsComSend = TRUE;
            USART_SendData(pUsartType, pUartMag->m_bySendBuf[pUartMag->m_wSendIndex]);
            pUartMag->m_wSendIndex += 1;
        }

    }

    if(USART_GetITStatus(pUsartType, USART_INT_TRAC) != RESET)
    {
        pUartMag->m_bIsComSend = FALSE;
        pUartMag->m_wSendLen = 0;
        USART_ClearITPendingBit(pUsartType, USART_INT_TRAC);
        /* Disable the UART Transmit Complete Interrupt */
        USART_INTConfig(pUsartType, USART_INT_TRAC, DISABLE);
    }

}

//串口轮询方式发送数据，无需拷贝数据到发送缓冲
uint32_t Uart_SendBuf_Poll(UART_MANAGER *pUartMag, uint8_t *pBuf, uint32_t len)
{
    pUartMag->m_wSendIndex = 0;

    while(pUartMag->m_wSendIndex < len)
    {

        USART_SendData(pUartMag->m_sConfig.m_pUSARTx, pBuf[pUartMag->m_wSendIndex]);

        while(!(USART_GetFlagStatus(pUartMag->m_sConfig.m_pUSARTx, USART_FLAG_TDE) == SET));

        pUartMag->m_wSendIndex += 1;
    }

    return 0;
}

void ttcpy(uint8_t* D_buffer, uint8_t* S_buffer, uint16_t D_len, uint16_t S_len)
{
    uint16_t i;

    for (i = 0; i < S_len; i++)
    {
        D_buffer[D_len + i] = S_buffer[i];
    }
}

//串口中断方式发送数据，考虑连续发送，注意该函数退出后，串口数据未发送完成
uint32_t Uart_SendBuf_Interrupt(UART_MANAGER *pUartMag, uint8_t *pBuf, uint32_t len)
{
    #if 0

    if(len == 0)
        return 0;

    if((len > UART_SEND_BUFFER_LEN) || (pUartMag->m_bIsComSend == TRUE))
    {
        pUartMag->m_dwSendOverFlowCnt++;
        return 0;
    }

    memcpy((void *)pUartMag->m_bySendBuf, (void *)pBuf, len);
    pUartMag->m_wSendLen = len;
    pUartMag->m_bIsComSend = TRUE;

    pUartMag->m_wSendIndex = 0;

    USART_INTConfig(pUartMag->m_sConfig.m_pUSARTx, USART_INT_TDE, ENABLE);
    return 0;
    #endif

    if(len == 0)
        return 0;

    #if 0

    if(len > (UART_SEND_BUFFER_LEN - pUartMag->m_wSendLen))
    {
        pUartMag->m_dwSendOverFlowCnt++;
        return 0;
    }

    #endif

    if(pUartMag->m_bIsComSend == FALSE )
    {
        memcpy((void *)pUartMag->m_bySendBuf, (void *)pBuf, len);
        pUartMag->m_wSendLen = len;
        pUartMag->m_wSendIndex = 0;
    }
    else if(pUartMag->m_bIsComSend == TRUE)
    {
        ttcpy(pUartMag->m_bySendBuf, pBuf, pUartMag->m_wSendLen, len);
        pUartMag->m_wSendLen += len;
    }

    USART_INTConfig(pUartMag->m_sConfig.m_pUSARTx, USART_INT_TDE, ENABLE);
    return 0;
}

void dmacpy(uint8_t* D_buffer, uint8_t* S_buffer, uint16_t D_len, uint16_t S_len, uint16_t RemainLen)
{
    uint16_t i;

    for (i = 0; i < RemainLen; i++)
    {
        D_buffer[i] = D_buffer[i + D_len - RemainLen];
    }

    for (i = 0; i < S_len; i++)
    {
        D_buffer[RemainLen + i] = S_buffer[i];
    }

}


//串口DMA方式发送数据，暂不考虑连续发送，注意该函数退出后，串口数据未发送完成
uint32_t Uart_SendBuf_DMA(UART_MANAGER *pUartMag, uint8_t *pBuf, uint32_t len)
{
    UART_DMA_CONFIG *pDMAConfig = &pUartMag->m_sConfig.m_sTxDMAConfig;
    uint16_t RemainLen = 0;

    #if 0

    if(len == 0)
        return 0;

    //if((len>UART_SEND_BUFFER_LEN)||(pUartMag->m_bIsComSend == TRUE)){
    //	pUartMag->m_dwSendOverFlowCnt++;
    //	return 0;
    //}

    memcpy((void *)pUartMag->m_bySendBuf, (void *)pBuf, len);
    pUartMag->m_wSendLen = len;
    //pUartMag->m_bIsComSend = TRUE;

    //pUartMag->m_wSendIndex = 0;
    DMA_ClearFlag(pDMAConfig->m_byDMA_FLAG_TC); //清DMA标记位
    DMA_ChannelEnable(pDMAConfig->m_pDMA_CHx, DISABLE );
    DMA_SetCurrDataCounter(pDMAConfig->m_pDMA_CHx, pUartMag->m_wSendLen);
    DMA_ChannelEnable(pDMAConfig->m_pDMA_CHx, ENABLE);

    while( !DMA_GetFlagStatus(pDMAConfig->m_byDMA_FLAG_TC));	//等待DMA发送完成

    return 0;
    #endif

    DMA_ChannelEnable(pDMAConfig->m_pDMA_CHx, DISABLE );

    if(pUartMag->m_bIsComSend == 1)
    {
        RemainLen = DMA_GetCurrDataCounter(pDMAConfig->m_pDMA_CHx);
        dmacpy(pUartMag->m_bySendBuf, pBuf, pUartMag->m_wSendLen, len, RemainLen);
        pUartMag->m_wSendLen = len + RemainLen;
    }
    else
    {
        pUartMag->m_bIsComSend = 1;
        memcpy((void *)pUartMag->m_bySendBuf, (void *)pBuf, len);
        pUartMag->m_wSendLen = len;
        DMA_ClearFlag(pDMAConfig->m_byDMA_FLAG_TC);
    }

    DMA_SetCurrDataCounter(pDMAConfig->m_pDMA_CHx, pUartMag->m_wSendLen);
    DMA_ChannelEnable(pDMAConfig->m_pDMA_CHx, ENABLE);
    //while( !DMA_GetFlagStatus(pDMAConfig->m_byDMA_FLAG_TC)); //等待 DMA 发送完成
    return 0;
}

void Uart_DmaIRQHandle(UART_MANAGER *pUartMag)
{
    UART_DMA_CONFIG *pDmaConfig = NULL;

    pDmaConfig = &pUartMag->m_sConfig.m_sTxDMAConfig ;
    pUartMag->m_bIsComSend = 0;
    DMA_ClearFlag(pDmaConfig->m_byDMA_FLAG_TC );
    DMA_ClearITPendingBit(pDmaConfig->m_byDMA_FLAG_TC);
}

#ifdef ENABLE_COM1
void USART1_IRQHandler(void)
{
    Uart_ComIRQHandle(&g_sUartSetMag.Com[COM1]);
}
#endif

#ifdef ENABLE_COM2
void USART2_IRQHandler(void)
{
    Uart_ComIRQHandle(&g_sUartSetMag.Com[COM2]);
}
#endif

#ifdef ENABLE_COM3
void USART3_IRQHandler(void)
{
    Uart_ComIRQHandle(&g_sUartSetMag.Com[COM3]);
}
#endif

#ifdef ENABLE_COM4
void UART4_IRQHandler(void)
{
    Uart_ComIRQHandle(&g_sUartSetMag.Com[COM4]);
}
#endif

#ifdef ENABLE_COM5
void UART5_IRQHandler(void)
{
    Uart_ComIRQHandle(&g_sUartSetMag.Com[COM5]);
}
#endif

#ifdef ENABLE_COM6
void USART6_IRQHandler(void)
{
    Uart_ComIRQHandle(&g_sUartSetMag.Com[COM6]);
}
#endif

#ifdef ENABLE_COM7
void UART7_IRQHandler(void)
{
    Uart_ComIRQHandle(&g_sUartSetMag.Com[COM7]);
}
#endif

#ifdef ENABLE_COM8
void UART8_IRQHandler(void)
{
    Uart_ComIRQHandle(&g_sUartSetMag.Com[COM8]);
}
#endif

#ifdef ENABLE_COM1
//Dma 中断函数
void DMA1_Channel4_IRQHandler(void)
{
    Uart_DmaIRQHandle(&g_sUartSetMag.Com[COM1]);//usart1 TX
}
#endif

#ifdef ENABLE_COM2
void DMA1_Channel7_IRQHandler(void)
{
    Uart_DmaIRQHandle(&g_sUartSetMag.Com[COM2]);//usart2 TX
}
#endif

#ifdef ENABLE_COM3
void DMA1_Channel2_IRQHandler(void)
{
    Uart_DmaIRQHandle(&g_sUartSetMag.Com[COM3]);//usart3 TX
}
#endif

#ifdef ENABLE_COM4
void DMA2_Channel4_5_IRQHandler(void)
{
    Uart_DmaIRQHandle(&g_sUartSetMag.Com[COM4]);//uart4 TX
}
#endif


#elif defined(HC32F46x)

UART_SET_MANAGER g_sUartSetMag;

M4_USART_TypeDef *COM_TYPEDEF[COM_MAX] =
{

    #ifdef ENABLE_COM1
    M4_USART1,
    #endif

    #ifdef ENABLE_COM2
    M4_USART2,
    #endif

    #ifdef ENABLE_COM3
    M4_USART3,
    #endif

    #ifdef ENABLE_COM4
    M4_USART4,
    #endif
};

uint32_t GetComIndex(M4_USART_TypeDef *ComType, uint8_t *pIdx)
{
    uint32_t ret = TRUE;

    switch((uint32_t)ComType)
    {
        case (uint32_t)M4_USART1:
            if(NULL != pIdx)
            {
                *pIdx = 0;
            }

            break;

        case (uint32_t)M4_USART2:
            if(NULL != pIdx)
            {
                *pIdx = 1;
            }

            break;

        case (uint32_t)M4_USART3:
            if(NULL != pIdx)
            {
                *pIdx = 2;
            }

            break;

        case (uint32_t)M4_USART4:
            if(NULL != pIdx)
            {
                *pIdx = 3;
            }

            break;

        default:
            ret = FALSE;
            break;
    }

    return TRUE;
}

static void Uart_GpioConfig(UART_CONFIG *pUartConfig)
{
    /* Initialize USART IO
    stc_port_init_t stcPortInit;

    MEM_ZERO_STRUCT(stcPortInit);
    stcPortInit.enPinMode = Pin_Mode_In;	//输入
    stcPortInit.enPullUp = Enable;			//需要内部上拉（做输入时起作用）
    PORT_Init(pUartConfig->RxPort,pUartConfig->RxPin, &stcPortInit);
    */
    /* Configure the pin */
    PORT_SetFunc(pUartConfig->TxPort, pUartConfig->TxPin, pUartConfig->TxFuncSel, Disable);
    PORT_SetFunc(pUartConfig->RxPort, pUartConfig->RxPin, pUartConfig->RxFuncSel, Disable);
}


static void Uart_EnableClock(M4_USART_TypeDef *USARTx)
{
    if(USARTx == M4_USART1)
    {
        PWC_Fcg1PeriphClockCmd(PWC_FCG1_PERIPH_USART1, Enable);
    }
    else if(USARTx == M4_USART2)
    {
        PWC_Fcg1PeriphClockCmd(PWC_FCG1_PERIPH_USART2, Enable);
    }
    else if(USARTx == M4_USART3)
    {
        PWC_Fcg1PeriphClockCmd(PWC_FCG1_PERIPH_USART3, Enable);
    }
    else if(USARTx == M4_USART4)
    {
        PWC_Fcg1PeriphClockCmd(PWC_FCG1_PERIPH_USART4, Enable);
    }
}

//绑定中断号
static void IrqRegistration(en_int_src_t enIntSrc, IRQn_Type enIRQn)
{
    stc_intc_sel_field_t *stcIntSel = NULL;

    /* IRQ032~127 whether out of range */
    if ((enIRQn >= 32) && ((((enIntSrc / 32) * 6 + 32) > enIRQn) || (((enIntSrc / 32) * 6 + 37) < enIRQn)))
    {
        return;
    }
    else
    {
        stcIntSel = (stc_intc_sel_field_t *)((uint32_t)(&M4_INTC->SEL0)  +  (4u * enIRQn));
        //if (0x1FFu == stcIntSel->INTSEL)
        {
            stcIntSel->INTSEL = enIntSrc;
        }
    }
}
static void Uart_NvicConfig(en_int_src_t enIntSrc, IRQn_Type enIRQn, int8_t IrqPrity)
{
    IrqRegistration(enIntSrc, enIRQn);
    NVIC_SetPriority(enIRQn, IrqPrity);
    NVIC_ClearPendingIRQ(enIRQn);
    NVIC_EnableIRQ(enIRQn);
}


extern void Uart_ComConfiguration(UART_CONFIG Config, UART_MANAGER *pUartMag)
{
    memcpy((uint8_t *)&pUartMag->m_sConfig, (uint8_t *)&Config, sizeof(UART_CONFIG));
}

static void Uart_RxTimeoutConfig(UART_CONFIG Config)
{
    stc_clk_freq_t stcClkTmp;
    stc_tim0_base_init_t stcTimerCfg;
    stc_tim0_trigger_init_t StcTimer0TrigInit;
    M4_TMR0_TypeDef *pTmrDef = NULL;
    en_tim0_channel_t eTim0Ch;

    MEM_ZERO_STRUCT(stcClkTmp);
    MEM_ZERO_STRUCT(stcTimerCfg);
    MEM_ZERO_STRUCT(StcTimer0TrigInit);

    switch((uint32_t)Config.m_pUSARTx)
    {
        case (uint32_t)M4_USART1:
            PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIM01, Enable);
            pTmrDef = M4_TMR01;
            eTim0Ch = Tim0_ChannelA;
            break;

        case (uint32_t)M4_USART2:
            PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIM01, Enable);
            pTmrDef = M4_TMR01;
            eTim0Ch = Tim0_ChannelB;
            break;

        case (uint32_t)M4_USART3:
            PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIM02, Enable);
            pTmrDef = M4_TMR02;
            eTim0Ch = Tim0_ChannelA;
            break;

        case (uint32_t)M4_USART4:
            PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIM02, Enable);
            pTmrDef = M4_TMR02;
            eTim0Ch = Tim0_ChannelB;
            break;

        default:
            return;
            break;

    }

    /* Clear CNTAR register  */
    TIMER0_WriteCntReg(pTmrDef, eTim0Ch, 0u);

    /* Config register*/
    stcTimerCfg.Tim0_CounterMode = Tim0_Async;
    stcTimerCfg.Tim0_AsyncClockSource = Tim0_LRC;
    stcTimerCfg.Tim0_ClockDivision = Tim0_ClkDiv0;//Tim0_ClkDiv4; //32k
    stcTimerCfg.Tim0_CmpValue = 36;
    TIMER0_BaseInit(pTmrDef, eTim0Ch, &stcTimerCfg);

    /* Clear compare flag */
    TIMER0_ClearFlag(pTmrDef, eTim0Ch);

    /* Config timer0 hardware trigger */
    StcTimer0TrigInit.Tim0_InTrigEnable = false;
    StcTimer0TrigInit.Tim0_InTrigClear = true;
    StcTimer0TrigInit.Tim0_InTrigStart = true;
    StcTimer0TrigInit.Tim0_InTrigStop = false;
    TIMER0_HardTriggerInit(pTmrDef, eTim0Ch, &StcTimer0TrigInit);

}

void Uart_IrqHandle_TI(UART_MANAGER *pUartMg)
{
    volatile UART_MANAGER *pUartMag = pUartMg;

    if(Set == USART_GetStatus(pUartMag->m_sConfig.m_pUSARTx, UsartTxEmpty))
    {

        if(pUartMag->m_wSentLen >= pUartMag->m_wSendLen)
        {
            /* Disable the UART Transmit Data Register Empty Interrupt */
            USART_FuncCmd(pUartMag->m_sConfig.m_pUSARTx, UsartTxEmptyInt, Disable);
            pUartMag->m_bIsComSend = FALSE;
            /* Enable the UART Transmit Complete Interrupt */
            USART_FuncCmd(pUartMag->m_sConfig.m_pUSARTx, UsartTxCmpltInt, Enable);
        }
        else
        {
            pUartMag->m_bIsComSend = TRUE;
            USART_SendData(pUartMag->m_sConfig.m_pUSARTx, pUartMag->m_bySendBuf[pUartMag->m_wSentLen]);
            pUartMag->m_wSentLen += 1;
        }

    }
}

void Uart_IrqHandle_TCI(UART_MANAGER *pUartMg)
{
    volatile UART_MANAGER *pUartMag = pUartMg;

    if(Set == USART_GetStatus(pUartMag->m_sConfig.m_pUSARTx, UsartTxComplete))
    {
        pUartMag->m_bIsComSend = FALSE;
        pUartMag->m_wSendLen = 0;
        /* Disable the UART Transmit Complete Interrupt */
        USART_FuncCmd(pUartMag->m_sConfig.m_pUSARTx, UsartTxCmpltInt, Disable);
        USART_FuncCmd(pUartMag->m_sConfig.m_pUSARTx, UsartTx, Disable);
    }
}

void Uart_IrqHandle_RI(UART_MANAGER *pUartMg)
{
    volatile UART_MANAGER *pUartMag = pUartMg;
    uint8_t dat;

    //if(Set == USART_GetStatus(pUartMag->m_sConfig.m_pUSARTx, UsartRxInt))
    {
        dat = (u8)USART_RecData(pUartMag->m_sConfig.m_pUSARTx);

        pUartMag->m_byRecvBuf[pUartMag->m_wRecvLen] = dat;
        pUartMag->m_wRecvLen += 1;

        if(pUartMag->m_wRecvLen >= (UART_RECV_BUFFER_LEN / 2))
        {
            pUartMag->m_bIsComRecv = TRUE;
        }
    }
}

void Uart_IrqHandle_RTO(UART_MANAGER *pUartMg)
{
    volatile UART_MANAGER *pUartMag = pUartMg;
    volatile UART_CONFIG *pConfig = &pUartMag->m_sConfig;
    u8 DmaCh = pConfig->m_sRxDMAConfig.m_eDMAChx;
    uint8_t dat;
    stc_dma_mondtctl_field_t *pMondTCtl = &pConfig->m_sRxDMAConfig.m_pDMAx->MONDTCTL0_f;

    //if(Set == USART_GetStatus(pUartMag->m_sConfig.m_pUSARTx, UsartTimeOutInt))
    {

        USART_ClearStatus(pUartMag->m_sConfig.m_pUSARTx, UsartRxTimeOut);

        if(pUartMag->m_sConfig.m_pUSARTx == M4_USART1)
        {
            TIMER0_Cmd(M4_TMR01, Tim0_ChannelA, Disable);
        }
        else if(pUartMag->m_sConfig.m_pUSARTx == M4_USART2)
        {
            TIMER0_Cmd(M4_TMR01, Tim0_ChannelB, Disable);
        }
        else if(pUartMag->m_sConfig.m_pUSARTx == M4_USART3)
        {
            TIMER0_Cmd(M4_TMR02, Tim0_ChannelA, Disable);
        }
        else if(pUartMag->m_sConfig.m_pUSARTx == M4_USART4)
        {
            TIMER0_Cmd(M4_TMR02, Tim0_ChannelB, Disable);
        }

        if(pUartMag->m_sConfig.RxMode == MODE_DMA)
        {

            DMA_ChannelCmd(pConfig->m_sRxDMAConfig.m_pDMAx, pConfig->m_sRxDMAConfig.m_eDMAChx, Disable);

            pMondTCtl = (stc_dma_mondtctl_field_t *)((uint8_t *)pMondTCtl + 0x40 * pConfig->m_sRxDMAConfig.m_eDMAChx);

            pUartMag->m_wRecvLen = UART_RECV_BUFFER_LEN - pMondTCtl->CNT;
            DMA_SetDesAddress(pConfig->m_sRxDMAConfig.m_pDMAx, pConfig->m_sRxDMAConfig.m_eDMAChx, (uint32_t)pUartMag->m_byRecvBuf);
            DMA_SetTransferCnt(pConfig->m_sRxDMAConfig.m_pDMAx, pConfig->m_sRxDMAConfig.m_eDMAChx, UART_RECV_BUFFER_LEN);
            DMA_ChannelCmd(pConfig->m_sRxDMAConfig.m_pDMAx, pConfig->m_sRxDMAConfig.m_eDMAChx, Enable);

            pUartMag->m_bIsComRecv = TRUE;
        }

        if(pUartMag->m_sConfig.RxMode == MODE_INTERRUPT)
        {
            pUartMag->m_bIsComRecv = TRUE;
        }

    }
}

void Uart_IrqHandle_EI(UART_MANAGER *pUartMg)
{
    volatile UART_MANAGER *pUartMag = pUartMg;

    if (Set == USART_GetStatus(pUartMag->m_sConfig.m_pUSARTx, UsartFrameErr))
    {
        USART_ClearStatus(pUartMag->m_sConfig.m_pUSARTx, UsartFrameErr);
    }

    if (Set == USART_GetStatus(pUartMag->m_sConfig.m_pUSARTx, UsartParityErr))
    {
        USART_ClearStatus(pUartMag->m_sConfig.m_pUSARTx, UsartParityErr);
    }

    if (Set == USART_GetStatus(pUartMag->m_sConfig.m_pUSARTx, UsartOverrunErr))
    {
        USART_ClearStatus(pUartMag->m_sConfig.m_pUSARTx, UsartOverrunErr);
    }
}

static void Uart_IrqHandle_DMATC(UART_MANAGER *pUartMag)
{
    UART_DMA_CONFIG *pDMAConfig = &pUartMag->m_sConfig.m_sTxDMAConfig;

    pUartMag->m_wSentLen += pUartMag->m_wLastDmaCnt;

    //缓冲中有新数据
    if(pUartMag->m_wSentLen < pUartMag->m_wSendLen)
    {


        pUartMag->m_wLastDmaCnt = pUartMag->m_wSendLen - pUartMag->m_wSentLen;

        //此时串口数据还未发送结束。
        //USART_FuncCmd(pUartMag->m_sConfig.m_pUSARTx, UsartTx, Disable);


        DMA_SetTransferCnt(pDMAConfig->m_pDMAx, pDMAConfig->m_eDMAChx, pUartMag->m_wLastDmaCnt);
        DMA_SetSrcAddress(pDMAConfig->m_pDMAx, pDMAConfig->m_eDMAChx, (uint32_t)(&pUartMag->m_bySendBuf[pUartMag->m_wSentLen]));

        /* Enable the specified DMA channel. */
        DMA_ChannelCmd(pDMAConfig->m_pDMAx, pDMAConfig->m_eDMAChx, Enable);
        /* Clear DMA flag. */
        DMA_ClearIrqFlag(pDMAConfig->m_pDMAx, pDMAConfig->m_eDMAChx, TrnCpltIrq);

        USART_FuncCmd(pUartMag->m_sConfig.m_pUSARTx, UsartTxAndTxEmptyInt, Enable);
    }
    else
    {
        pUartMag->m_bIsComSend = FALSE;

        DMA_ClearIrqFlag(pDMAConfig->m_pDMAx, pDMAConfig->m_eDMAChx, BlkTrnCpltIrq);
        DMA_ClearIrqFlag(pDMAConfig->m_pDMAx, pDMAConfig->m_eDMAChx, TrnCpltIrq);
    }


}



void Uart_EnableIntError(UART_MANAGER *pUartMag)
{
    uint8_t Idx;
    en_int_src_t enIntSrc;
    IRQn_Type enIRQn;

    if(TRUE != GetComIndex(pUartMag->m_sConfig.m_pUSARTx, &Idx))
    {
        return;
    }

    enIntSrc = (en_int_src_t)(INT_USART1_EI + Idx * 5);
    enIRQn = (IRQn_Type)(Int006_IRQn + Idx * 6);

    IrqRegistration(enIntSrc, enIRQn);

    NVIC_SetPriority(enIRQn, 6 + Idx);
    NVIC_ClearPendingIRQ(enIRQn);
    NVIC_EnableIRQ(enIRQn);
}

void Uart_EnableIntTx(UART_MANAGER *pUartMag)
{
    uint8_t Idx;
    en_int_src_t enIntSrc;
    IRQn_Type enIRQn;

    if(TRUE != GetComIndex(pUartMag->m_sConfig.m_pUSARTx, &Idx))
    {
        return;
    }

    enIntSrc = (en_int_src_t)(INT_USART1_TI + Idx * 5);
    enIRQn = (IRQn_Type)(Int009_IRQn + Idx * 6);

    IrqRegistration(enIntSrc, enIRQn);

    NVIC_SetPriority(enIRQn, 9 + Idx);
    NVIC_ClearPendingIRQ(enIRQn);
    NVIC_EnableIRQ(enIRQn);

    enIntSrc = (en_int_src_t)(INT_USART1_TCI + Idx * 5);
    enIRQn = (IRQn_Type)(Int010_IRQn + Idx * 6);

    IrqRegistration(enIntSrc, enIRQn);

    NVIC_SetPriority(enIRQn, 10 + Idx);
    NVIC_ClearPendingIRQ(enIRQn);
    NVIC_EnableIRQ(enIRQn);
}


void Uart_EnableIntRx(UART_MANAGER *pUartMag)
{
    uint8_t Idx;
    en_int_src_t enIntSrc;
    IRQn_Type enIRQn;

    if(TRUE != GetComIndex(pUartMag->m_sConfig.m_pUSARTx, &Idx))
    {
        return;
    }

    enIntSrc = (en_int_src_t)(INT_USART1_RI + Idx * 5);
    enIRQn = (IRQn_Type)(Int007_IRQn + Idx * 6);

    IrqRegistration(enIntSrc, enIRQn);

    NVIC_SetPriority(enIRQn, 7 + Idx);
    NVIC_ClearPendingIRQ(enIRQn);
    NVIC_EnableIRQ(enIRQn);

    enIntSrc = (en_int_src_t)(INT_USART1_RTO + Idx * 5);
    enIRQn = (IRQn_Type)(Int008_IRQn + Idx * 6);

    IrqRegistration(enIntSrc, enIRQn);

    NVIC_SetPriority(enIRQn, 8 + Idx);
    NVIC_ClearPendingIRQ(enIRQn);
    NVIC_EnableIRQ(enIRQn);

    USART_FuncCmd(pUartMag->m_sConfig.m_pUSARTx, UsartRxInt, Enable);
    USART_FuncCmd(pUartMag->m_sConfig.m_pUSARTx, UsartTimeOut, Enable);
    USART_FuncCmd(pUartMag->m_sConfig.m_pUSARTx, UsartTimeOutInt, Enable);
}


//串口DMA发送使能，使用发送完成中断
static void Uart_EnableDmaTx(UART_MANAGER *pUartMag)
{
    #if  1
    uint8_t Idx;
    en_int_src_t enIntSrc;
    IRQn_Type enIRQn;

    stc_dma_config_t stcDmaInit;
    UART_DMA_CONFIG *pConfig = &pUartMag->m_sConfig.m_sTxDMAConfig;
    MEM_ZERO_STRUCT(stcDmaInit);

    if(TRUE != GetComIndex(pUartMag->m_sConfig.m_pUSARTx, &Idx))
    {
        return;
    }

    /* Enable peripheral clock */
    if(M4_DMA1 == pConfig->m_pDMAx)
    {
        PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_DMA1, Enable);
    }
    else
    {
        PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_DMA2, Enable);
    }

    /* Enable DMA. */
    DMA_Cmd(pConfig->m_pDMAx, Enable);

    /* Initialize DMA. */
    MEM_ZERO_STRUCT(stcDmaInit);
    stcDmaInit.u16BlockSize = 1u; /* 1 block */
    stcDmaInit.u16TransferCnt = UART_PROTOCAL_BUFFER_LEN;//
    stcDmaInit.u32SrcAddr = (uint32_t)(pUartMag->m_bySendBuf); /* Set source address. */
    stcDmaInit.u32DesAddr = (uint32_t)(&pUartMag->m_sConfig.m_pUSARTx->DR);     /* Set destination address. */
    stcDmaInit.stcDmaChCfg.enSrcInc = AddressIncrease;  /* Set source address mode. */
    stcDmaInit.stcDmaChCfg.enDesInc = AddressFix;  /* Set destination address mode. */
    stcDmaInit.stcDmaChCfg.enIntEn = Enable;       /* Enable interrupt. */
    stcDmaInit.stcDmaChCfg.enTrnWidth = Dma8Bit;   /* Set data width 8bit. */
    DMA_InitChannel(pConfig->m_pDMAx, pConfig->m_eDMAChx, &stcDmaInit);
    /* Enable the specified DMA channel. */
    DMA_ChannelCmd(pConfig->m_pDMAx, pConfig->m_eDMAChx, Enable);
    /* Clear DMA flag. */
    DMA_ClearIrqFlag(pConfig->m_pDMAx, pConfig->m_eDMAChx, TrnCpltIrq);
    /* Enable peripheral circuit trigger function. */
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_PTDIS, Enable);
    /* Set DMA trigger source. */
    DMA_SetTriggerSrc(pConfig->m_pDMAx, pConfig->m_eDMAChx, pConfig->m_eDMATrgSel);

    enIntSrc = (en_int_src_t)(INT_DMA1_TC0 + Idx);
    enIRQn = (IRQn_Type)(Int011_IRQn + Idx * 6);

    IrqRegistration(enIntSrc, enIRQn);

    NVIC_SetPriority(enIRQn, 11 + Idx);
    NVIC_ClearPendingIRQ(enIRQn);
    NVIC_EnableIRQ(enIRQn);
    #endif

}

static void Uart_EnableDmaRx(UART_MANAGER *pUartMag)
{
    uint8_t Idx;
    en_int_src_t enIntSrc;
    IRQn_Type enIRQn;
    UART_CONFIG *pConfig = &pUartMag->m_sConfig;
    UART_DMA_CONFIG *pDMAConfig = &pConfig->m_sRxDMAConfig;
    stc_dma_config_t stcDmaInit;
    u8 DmaCh =   pConfig->m_sRxDMAConfig.m_eDMAChx;

    /* Enable peripheral clock */
    if(M4_DMA1 == pDMAConfig->m_pDMAx)
    {
        PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_DMA1, Enable);
    }
    else
    {
        PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_DMA2, Enable);
    }

    /* Enable DMA. */
    DMA_Cmd(pDMAConfig->m_pDMAx, Enable);
    /* Initialize DMA. */
    MEM_ZERO_STRUCT(stcDmaInit);
    stcDmaInit.u16BlockSize = 1u; /* 1 block */

    /*config the repeat mode*/
    //stcDmaInit.u16DesRptSize = UART_RECV_BUFFER_LEN;
    //stcDmaInit.stcDmaChCfg.enDesRptEn = Enable;
    stcDmaInit.u16TransferCnt = UART_RECV_BUFFER_LEN;//
    stcDmaInit.u32SrcAddr = ((uint32_t)(&pConfig->m_pUSARTx->DR) + 2ul); /* Set source address. */
    stcDmaInit.u32DesAddr = (uint32_t)(pUartMag->m_byRecvBuf);     /* Set destination address. */
    stcDmaInit.stcDmaChCfg.enSrcInc = AddressFix;  /* Set source address mode. */
    stcDmaInit.stcDmaChCfg.enDesInc = AddressIncrease;  /* Set destination address mode. */
    stcDmaInit.stcDmaChCfg.enIntEn = Enable;       /* Enable interrupt. */
    stcDmaInit.stcDmaChCfg.enTrnWidth = Dma8Bit;   /* Set data width 8bit. */
    DMA_InitChannel(pDMAConfig->m_pDMAx, pDMAConfig->m_eDMAChx, &stcDmaInit);

    /* Enable the specified DMA channel. */
    DMA_ChannelCmd(pDMAConfig->m_pDMAx, pDMAConfig->m_eDMAChx, Enable);

    /* Clear DMA flag. */
    DMA_ClearIrqFlag(pDMAConfig->m_pDMAx, pDMAConfig->m_eDMAChx, BlkTrnCpltIrq);
    DMA_ClearIrqFlag(pDMAConfig->m_pDMAx, pDMAConfig->m_eDMAChx, TrnCpltIrq);
    /* Enable peripheral circuit trigger function. */
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_PTDIS, Enable); //Disable);//
    /* Set DMA trigger source. */
    DMA_SetTriggerSrc(pDMAConfig->m_pDMAx, pDMAConfig->m_eDMAChx, pDMAConfig->m_eDMATrgSel);


    DMA_EnableIrq(pDMAConfig->m_pDMAx, pDMAConfig->m_eDMAChx, BlkTrnCpltIrq); //TrnCpltIrq);//

    if(TRUE != GetComIndex(pUartMag->m_sConfig.m_pUSARTx, &Idx))
    {
        return;
    }

    enIntSrc = (en_int_src_t)(INT_USART1_RTO + Idx * 5);
    enIRQn = (IRQn_Type)(Int008_IRQn + Idx * 6);

    IrqRegistration(enIntSrc, enIRQn);

    NVIC_SetPriority(enIRQn, 8 + Idx);
    NVIC_ClearPendingIRQ(enIRQn);
    NVIC_EnableIRQ(enIRQn);

    USART_FuncCmd(pUartMag->m_sConfig.m_pUSARTx, UsartRxInt, Enable);
    USART_FuncCmd(pUartMag->m_sConfig.m_pUSARTx, UsartTimeOut, Enable);
    USART_FuncCmd(pUartMag->m_sConfig.m_pUSARTx, UsartTimeOutInt, Enable);

}



void Uart_ComInit(UART_MANAGER *pUartMag)
{
    UART_CONFIG *pConfig = &pUartMag->m_sConfig;
    stc_usart_uart_init_t stcInitCfg;

    Uart_EnableClock(pConfig->m_pUSARTx);

    Uart_GpioConfig(pConfig);

    USART_DeInit(pConfig->m_pUSARTx);

    #if 1
    stcInitCfg.enClkMode = UsartIntClkCkOutput;
    stcInitCfg.enClkDiv = UsartClkDiv_64;
    stcInitCfg.enDataLength = UsartDataBits8;
    stcInitCfg.enDirection = UsartDataLsbFirst;
    stcInitCfg.enStopBit = UsartOneStopBit;
    stcInitCfg.enParity = UsartParityNone;
    stcInitCfg.enSampleMode = UsartSamleBit8;
    stcInitCfg.enDetectMode = UsartStartBitFallEdge;
    stcInitCfg.enHwFlow = UsartRtsEnable;
    #endif

    while(Ok != USART_UART_Init(pConfig->m_pUSARTx, &stcInitCfg)) {};

    while(Ok != USART_SetBaudrate(pConfig->m_pUSARTx, pConfig->m_dwBaudRate)) {};

    if(pConfig->TxMode == MODE_INTERRUPT)
    {
        Uart_EnableIntTx(pUartMag);
    }
    else if(pConfig->TxMode == MODE_DMA)
    {
        Uart_EnableDmaTx(pUartMag);
    }

    if(pConfig->RxMode == MODE_INTERRUPT)
    {
        Uart_RxTimeoutConfig(*pConfig);
        Uart_EnableIntRx(pUartMag);
    }
    else if(pConfig->RxMode == MODE_DMA)
    {

        Uart_RxTimeoutConfig(*pConfig);
        Uart_EnableDmaRx(pUartMag);
    }

    USART_ClearStatus(pConfig->m_pUSARTx, UsartParityErr);
    USART_ClearStatus(pConfig->m_pUSARTx, UsartFrameErr);
    USART_ClearStatus(pConfig->m_pUSARTx, UsartOverrunErr);
    USART_ClearStatus(pConfig->m_pUSARTx, UsartRxTimeOut);

    if(pConfig->RxMode != MODE_NONE)
    {
        USART_FuncCmd(pConfig->m_pUSARTx, UsartRx, Enable);
    }

    if(pConfig->TxMode != MODE_NONE)
    {
        //USART_FuncCmd(pConfig->m_pUSARTx, UsartTx, Enable);
    }

    if(pConfig->TxMode != MODE_POLL || pConfig->RxMode != MODE_POLL)
    {
        Uart_EnableIntError(pUartMag);
    }

}


void Uart_ComDeInit(UART_MANAGER *pUartMag)
{
    USART_DeInit(pUartMag->m_sConfig.m_pUSARTx);
}


//串口轮询方式发送数据，无需拷贝数据到发送缓冲
uint32_t Uart_SendBuf_Poll(UART_MANAGER *pUartMag, uint8_t *pBuf, uint32_t len)
{
    pUartMag->m_wSentLen = 0;

    while(pUartMag->m_wSentLen < len)
    {

        //USART_SendData(pUartMag->m_sConfig.m_pUSARTx, pBuf[pUartMag->m_wSendIndex]);
        pUartMag->m_sConfig.m_pUSARTx->DR_f.TDR = (uint32_t)pBuf[pUartMag->m_wSentLen];

        while (0 == pUartMag->m_sConfig.m_pUSARTx->SR_f.TC)
        {
            ;
        }

        pUartMag->m_wSentLen += 1;
    }

    return 0;
}

void ttcpy(uint8_t* D_buffer, uint8_t* S_buffer, uint16_t D_len, uint16_t S_len)
{
    uint16_t i;

    for (i = 0; i < S_len; i++)
    {
        D_buffer[D_len + i] = S_buffer[i];
    }
}

//串口中断方式发送数据，考虑连续发送，注意该函数退出后，串口数据未发送完成
uint32_t Uart_SendBuf_Interrupt(UART_MANAGER *pUartMag, uint8_t *pBuf, uint32_t len)
{
    if(len == 0)
        return 0;

    if(pUartMag->m_bIsComSend == FALSE )
    {
        memcpy((void *)pUartMag->m_bySendBuf, (void *)pBuf, len);
        pUartMag->m_wSendLen = len;
        pUartMag->m_wSentLen = 0;
    }
    else if(pUartMag->m_bIsComSend == TRUE)
    {
        ttcpy(pUartMag->m_bySendBuf, pBuf, pUartMag->m_wSendLen, len);
        pUartMag->m_wSendLen += len;
    }

    USART_FuncCmd(pUartMag->m_sConfig.m_pUSARTx, UsartTxAndTxEmptyInt, Enable);

    return 0;

}

void dmacpy(uint8_t* D_buffer, uint8_t* S_buffer, uint16_t D_len, uint16_t S_len, uint16_t RemainLen)
{
    uint16_t i;

    for (i = 0; i < RemainLen; i++)
    {
        D_buffer[i] = D_buffer[i + D_len - RemainLen];
    }

    for (i = 0; i < S_len; i++)
    {
        D_buffer[RemainLen + i] = S_buffer[i];
    }

}

#if 0
//串口DMA方式发送数据，暂不考虑连续发送，注意该函数退出后，串口数据未发送完成
uint32_t Uart_SendBuf_DMA(UART_MANAGER *pUartMag, uint8_t *pBuf, uint32_t len)
{
    UART_DMA_CONFIG *pDMACfg = &pUartMag->m_sConfig.m_sTxDMAConfig;
    uint16_t SentCount;



    if(len == 0)
        return 0;

    #if 1
    //USART_FuncCmd(pUartCfg->m_pUSARTx, UsartTx, Disable);
    //DMA_ChannelCmd(pUartCfg->m_sTxDMAConfig.m_pDMAx, pUartCfg->m_sTxDMAConfig.m_eDMAChx, Disable);

    if(pUartMag->m_bIsComSend == TRUE)
    {
        memcpy(&pUartMag->m_bySendBuf[pUartMag->m_wRemainLen], pBuf, MIN(len, sizeof(pUartMag->m_bySendBuf) - pUartMag->m_wRemainLen));
        pUartMag->m_wSendLen = pUartMag->m_wRemainLen + MIN(len, sizeof(pUartMag->m_bySendBuf) - pUartMag->m_wRemainLen);
        pUartMag->m_wRemainLen = pUartMag->m_wSendLen;
    }
    else
    {
        pUartMag->m_bIsComSend = TRUE;
        memcpy(pUartMag->m_bySendBuf, pBuf, MIN(len, sizeof(pUartMag->m_bySendBuf)));
        pUartMag->m_wSendLen = pUartMag->m_wRemainLen = MIN(len, sizeof(pUartMag->m_bySendBuf));
    }

    #endif

    USART_FuncCmd(pUartMag->m_sConfig.m_pUSARTx, UsartTx, Disable);


//  len = 0;
    DMA_SetTransferCnt(pDMACfg->m_pDMAx, pDMACfg->m_eDMAChx, pUartMag->m_wSendLen);
    DMA_SetSrcAddress(pDMACfg->m_pDMAx, pDMACfg->m_eDMAChx, (uint32_t)pUartMag->m_bySendBuf);

    /* Enable the specified DMA channel. */
    DMA_ChannelCmd(pDMACfg->m_pDMAx, pDMACfg->m_eDMAChx, Enable);
    /* Clear DMA flag. */
    DMA_ClearIrqFlag(pDMACfg->m_pDMAx, pDMACfg->m_eDMAChx, TrnCpltIrq);

    USART_FuncCmd(pUartMag->m_sConfig.m_pUSARTx, UsartTxAndTxEmptyInt, Enable);

    return 0;
}
#endif

//串口DMA方式发送数据，支持串口连续发送和未发送完成又来新数据
uint32_t Uart_SendBuf_DMA(UART_MANAGER *pUartMag, uint8_t *pBuf, uint32_t len)
{
    UART_DMA_CONFIG *pDMACfg = &pUartMag->m_sConfig.m_sTxDMAConfig;
    uint16_t SentCount;



    if(len == 0)
        return 0;

    #if 1
    //USART_FuncCmd(pUartCfg->m_pUSARTx, UsartTx, Disable);
    //DMA_ChannelCmd(pUartCfg->m_sTxDMAConfig.m_pDMAx, pUartCfg->m_sTxDMAConfig.m_eDMAChx, Disable);
    
    //若100ms内还没将数据发送出去，则认为串口已经卡死，会丢失一包数据
    if(pUartMag->m_bIsComSend == TRUE && (GetSysRunMilliSecond()-pUartMag->m_dwSendTime>100))
    {
        pUartMag->m_bIsComSend = FALSE;
        Uart_ComInit(pUartMag);
    }
    
    if(pUartMag->m_bIsComSend == TRUE)
    {
        memcpy(&pUartMag->m_bySendBuf[pUartMag->m_wSendLen], pBuf, MIN(len, sizeof(pUartMag->m_bySendBuf) - pUartMag->m_wSendLen));
        pUartMag->m_wSendLen = pUartMag->m_wSendLen + MIN(len, sizeof(pUartMag->m_bySendBuf) - pUartMag->m_wSendLen);
    }
    else
    {
        pUartMag->m_bIsComSend = TRUE;
        memcpy(pUartMag->m_bySendBuf, pBuf, MIN(len, sizeof(pUartMag->m_bySendBuf)));
        pUartMag->m_wLastDmaCnt  = pUartMag->m_wSendLen = MIN(len, sizeof(pUartMag->m_bySendBuf));
        pUartMag->m_wSentLen = 0;

        USART_FuncCmd(pUartMag->m_sConfig.m_pUSARTx, UsartTx, Disable);

        //  len = 0;
        DMA_SetTransferCnt(pDMACfg->m_pDMAx, pDMACfg->m_eDMAChx, pUartMag->m_wLastDmaCnt);
        DMA_SetSrcAddress(pDMACfg->m_pDMAx, pDMACfg->m_eDMAChx, (uint32_t)(&pUartMag->m_bySendBuf[pUartMag->m_wSentLen]));

        /* Enable the specified DMA channel. */
        DMA_ChannelCmd(pDMACfg->m_pDMAx, pDMACfg->m_eDMAChx, Enable);
        /* Clear DMA flag. */
        DMA_ClearIrqFlag(pDMACfg->m_pDMAx, pDMACfg->m_eDMAChx, TrnCpltIrq);

        USART_FuncCmd(pUartMag->m_sConfig.m_pUSARTx, UsartTxAndTxEmptyInt, Enable);

    }
    
    pUartMag->m_dwSendTime = GetSysRunMilliSecond();
    
    #endif



    return 0;
}


#ifdef ENABLE_COM1
//EI Int
void IRQ006_Handler(void)
{
    Uart_IrqHandle_EI(&g_sUartSetMag.Com[COM1]);
}

//RX Int
void IRQ007_Handler(void)
{
    Uart_IrqHandle_RI(&g_sUartSetMag.Com[COM1]);
}

//RTO Int
void IRQ008_Handler(void)
{
    Uart_IrqHandle_RTO(&g_sUartSetMag.Com[COM1]);
}

//Tx Int
void IRQ009_Handler(void)
{
    Uart_IrqHandle_TI(&g_sUartSetMag.Com[COM1]);
}


//TX Cmplt Int
void IRQ010_Handler(void)
{
    Uart_IrqHandle_TCI(&g_sUartSetMag.Com[COM1]);
}


//DMA TC Int
void IRQ011_Handler(void)
{
    Uart_IrqHandle_DMATC(&g_sUartSetMag.Com[COM1]);
}

#endif

#ifdef ENABLE_COM2
//EI Int
void IRQ012_Handler(void)
{
    Uart_IrqHandle_EI(&g_sUartSetMag.Com[COM2]);
}

//RX Int
void IRQ013_Handler(void)
{
    Uart_IrqHandle_RI(&g_sUartSetMag.Com[COM2]);
}

//RTO Int
void IRQ014_Handler(void)
{
    Uart_IrqHandle_RTO(&g_sUartSetMag.Com[COM2]);
}


//Tx Int
void IRQ015_Handler(void)
{
    Uart_IrqHandle_TI(&g_sUartSetMag.Com[COM2]);
}


//TX Cmplt Int
void IRQ016_Handler(void)
{
    Uart_IrqHandle_TCI(&g_sUartSetMag.Com[COM2]);
}


//DMA TC Int
void IRQ017_Handler(void)
{
    Uart_IrqHandle_DMATC(&g_sUartSetMag.Com[COM2]);
}

#endif

#ifdef ENABLE_COM3

//EI Int
void IRQ018_Handler(void)
{
    Uart_IrqHandle_EI(&g_sUartSetMag.Com[COM3]);
}

//RX Int
void IRQ019_Handler(void)
{
    Uart_IrqHandle_RI(&g_sUartSetMag.Com[COM3]);
}

//RTO Int
void IRQ020_Handler(void)
{
    Uart_IrqHandle_RTO(&g_sUartSetMag.Com[COM3]);
}


//Tx Int
void IRQ021_Handler(void)
{
    Uart_IrqHandle_TI(&g_sUartSetMag.Com[COM3]);
}


//TX Cmplt Int
void IRQ022_Handler(void)
{
    Uart_IrqHandle_TCI(&g_sUartSetMag.Com[COM3]);
}


//DMA TC Int
void IRQ023_Handler(void)
{
    Uart_IrqHandle_DMATC(&g_sUartSetMag.Com[COM3]);
}
#endif

#ifdef ENABLE_COM4

//EI Int
void IRQ024_Handler(void)
{
    Uart_IrqHandle_EI(&g_sUartSetMag.Com[COM4]);
}

//RX Int
void IRQ025_Handler(void)
{
    Uart_IrqHandle_RI(&g_sUartSetMag.Com[COM4]);
}

//RTO Int
void IRQ026_Handler(void)
{
    Uart_IrqHandle_RTO(&g_sUartSetMag.Com[COM4]);
}


//Tx Int
void IRQ027_Handler(void)
{
    Uart_IrqHandle_TI(&g_sUartSetMag.Com[COM4]);
}


//TX Cmplt Int
void IRQ028_Handler(void)
{
    Uart_IrqHandle_TCI(&g_sUartSetMag.Com[COM4]);
}


//DMA TC Int
void IRQ029_Handler(void)
{
    Uart_IrqHandle_DMATC(&g_sUartSetMag.Com[COM4]);
}

#endif

#endif


uint32_t Uart_SendBuf(UART_MANAGER *pUartMag, uint8_t *pBuf, uint32_t len)
{
    if(pUartMag->m_sConfig.TxMode == MODE_POLL)
    {
        Uart_SendBuf_Poll(pUartMag, pBuf, len);
    }
    else if(pUartMag->m_sConfig.TxMode == MODE_INTERRUPT)
    {
        Uart_SendBuf_Interrupt(pUartMag, pBuf, len);
    }
    else if(pUartMag->m_sConfig.TxMode == MODE_DMA)
    {
        Uart_SendBuf_DMA(pUartMag, pBuf, len);
    }
    else
    {
        pUartMag->m_wSendLen = 0;
        pUartMag->m_bIsComSend = FALSE;
    }

    return 0;
}

#if 0
void Uart_DebugPrintf(UART_MANAGER *pUartMag, char* fmt, ...)
{
    va_list ap;
    uint8_t buf[200] = "\0";

    va_start(ap, fmt);
    vsprintf((char *)buf, fmt, ap);
    va_end(ap);
    Uart_SendBuf(pUartMag, buf, strlen((char *)buf));
}

#endif

#if 1
void Uart_DebugPrintf(UART_MANAGER *pUartMag, char* fmt, ...)
{
    va_list ap;
    uint8_t *pbuf = sdram_malloc(0, UART_SEND_BUFFER_LEN);

    if(pbuf != NULL)
    {
        va_start(ap, fmt);
        vsprintf((char *)pbuf, fmt, ap);
        va_end(ap);
        Uart_SendBuf(pUartMag, pbuf, strlen((char *)pbuf));
        sdram_free(0, pbuf);
    }
}
#endif

//待测试串口和调试口连通
extern void UartTestToDebug(UART_MANAGER *pTestUartMag, UART_MANAGER *pDebugUartMag)
{
    if(TRUE == pTestUartMag->m_bIsComRecv)
    {
        Uart_SendBuf(pDebugUartMag, pTestUartMag->m_byRecvBuf, pTestUartMag->m_wRecvLen);
        pTestUartMag->m_wRecvLen = 0;
        pTestUartMag->m_bIsComRecv = FALSE;
    }

    if(TRUE == pDebugUartMag->m_bIsComRecv)
    {
        Uart_SendBuf(pTestUartMag, pDebugUartMag->m_byRecvBuf, pDebugUartMag->m_wRecvLen);
        pDebugUartMag->m_wRecvLen = 0;
        pDebugUartMag->m_bIsComRecv = FALSE;
    }
}

//串口回环测试
extern void UartLoopTest(UART_MANAGER *pTestUartMag)
{
    if(TRUE == pTestUartMag->m_bIsComRecv)
    {
        Uart_SendBuf(pTestUartMag, pTestUartMag->m_byRecvBuf, pTestUartMag->m_wRecvLen);
        pTestUartMag->m_wRecvLen = 0;
        pTestUartMag->m_bIsComRecv = FALSE;
    }
}


/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
