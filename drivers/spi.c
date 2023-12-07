#include "spi.h"
//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK精英STM32开发板
//SPI驱动 代码
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/9
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////

//以下是SPI模块的初始化代码，配置成主机模式，访问SD Card/W25Q64/NRF24L01
//SPI口初始化
//这里针是对SPI2的初始化



void SPI2_Init(void)
{
    #if defined(AT32F4xx)

    GPIO_InitType GPIO_InitStructure;
    SPI_InitType  SPI_InitStructure;

    RCC_APB2PeriphClockCmd(	RCC_APB2PERIPH_GPIOB, ENABLE );//PORTB时钟使能
    RCC_APB1PeriphClockCmd(	RCC_APB1PERIPH_SPI2,  ENABLE );//SPI2时钟使能

    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_13 | GPIO_Pins_14 | GPIO_Pins_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //PB13/14/15复用推挽输出
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB

    GPIO_SetBits(GPIOB, GPIO_Pins_13 | GPIO_Pins_14 | GPIO_Pins_15); //PB13/14/15上拉

    SPI_InitStructure.SPI_TransMode = SPI_TRANSMODE_FULLDUPLEX;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
    SPI_InitStructure.SPI_Mode = SPI_MODE_MASTER;		//设置SPI工作模式:设置为主SPI
    SPI_InitStructure.SPI_FrameSize = SPI_FRAMESIZE_8BIT;		//设置SPI的数据大小:SPI发送接收8位帧结构
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_HIGH;		//串行同步时钟的空闲状态为高电平
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2EDGE;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI_InitStructure.SPI_NSSSEL = SPI_NSSSEL_SOFT;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI_InitStructure.SPI_MCLKP = SPI_MCLKP_256;		//定义波特率预分频的值:波特率预分频值为256
    SPI_InitStructure.SPI_FirstBit = SPI_FIRSTBIT_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI_InitStructure.SPI_CPOLY = 7;	//CRC值计算的多项式
    SPI_Init(SPI2, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

    SPI_Enable(SPI2, ENABLE); //使能SPI外设

    SPI2_ReadWriteByte(0xff);//启动传输

    #elif defined(HC32F46x)

    stc_spi_init_t stcSpiInit;

    /* configuration structure initialization */
    MEM_ZERO_STRUCT(stcSpiInit);

    /* Configuration peripheral clock */
    PWC_Fcg1PeriphClockCmd(PWC_FCG1_PERIPH_SPI2, Enable);

    /* Configuration SPI pin */
    PORT_SetFunc(PortA, Pin02, Func_Spi2_Sck, Disable);
    PORT_SetFunc(PortA, Pin03, Func_Spi2_Mosi, Disable);
    PORT_SetFunc(PortA, Pin01, Func_Spi2_Miso, Disable);

    /* Configuration SPI structure */
    stcSpiInit.enClkDiv = SpiClkDiv64;
    stcSpiInit.enFrameNumber = SpiFrameNumber1;
    stcSpiInit.enDataLength = SpiDataLengthBit8;
    stcSpiInit.enFirstBitPosition = SpiFirstBitPositionMSB;
    stcSpiInit.enSckPolarity = SpiSckIdleLevelLow;
    stcSpiInit.enSckPhase = SpiSckOddSampleEvenChange;
    stcSpiInit.enReadBufferObject = SpiReadReceiverBuffer;
    stcSpiInit.enWorkMode = SpiWorkMode3Line;
    stcSpiInit.enTransMode = SpiTransFullDuplex;
    stcSpiInit.enCommAutoSuspendEn = Disable;
    stcSpiInit.enModeFaultErrorDetectEn = Disable;
    stcSpiInit.enParitySelfDetectEn = Disable;
    stcSpiInit.enParityEn = Disable;
    stcSpiInit.enParity = SpiParityEven;
    stcSpiInit.enMasterSlaveMode = SpiModeMaster;
    stcSpiInit.stcDelayConfig.enSsSetupDelayOption = SpiSsSetupDelayCustomValue;
    stcSpiInit.stcDelayConfig.enSsSetupDelayTime = SpiSsSetupDelaySck1;
    stcSpiInit.stcDelayConfig.enSsHoldDelayOption = SpiSsHoldDelayCustomValue;
    stcSpiInit.stcDelayConfig.enSsHoldDelayTime = SpiSsHoldDelaySck1;
    stcSpiInit.stcDelayConfig.enSsIntervalTimeOption = SpiSsIntervalCustomValue;
    stcSpiInit.stcDelayConfig.enSsIntervalTime = SpiSsIntervalSck6PlusPck2;

    SPI_Init(M4_SPI2, &stcSpiInit);
    SPI_Cmd(M4_SPI2, Enable);

    SPI2_ReadWriteByte(0xff);//启动传输

    #endif

}
//SPI 速度设置函数
//SpeedSet:
//SPI_MCLKP_2   2分频
//SPI_MCLKP_8   8分频
//SPI_MCLKP_16  16分频
//SPI_MCLKP_256 256分频

void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    #if defined(AT32F4xx)

    assert_param(IS_SPI_MCLKP(SPI_BaudRatePrescaler));
    SPI2->CTRL1 &= 0XFFC7;
    SPI2->CTRL1 |= SPI_BaudRatePrescaler;	//设置SPI2速度
    SPI_Enable(SPI2, ENABLE);

    #elif defined(HC32F46x)

    SPI_SetClockDiv(M4_SPI2, SPI_BaudRatePrescaler);
    SPI_Cmd(M4_SPI2, Enable);

    #endif

}

//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI2_ReadWriteByte(u8 TxData)
{
    #if defined(AT32F4xx)

    u8 retry = 0;

    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
    {
        retry++;

        if(retry > 200)return 0;
    }

    SPI_I2S_TxData(SPI2, TxData); //通过外设SPIx发送一个数据
    retry = 0;

    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
    {
        retry++;

        if(retry > 200)return 0;
    }

    return SPI_I2S_RxData(SPI2); //返回通过SPIx最近接收的数据

    #elif defined(HC32F46x)

    uint8_t u8Byte;
    uint8_t retry = 0;

    /* Wait tx buffer empty */
    while (Reset == SPI_GetFlag(M4_SPI2, SpiFlagSendBufferEmpty))
    {
        retry++;

        if(retry > 200)return 0;
    }

    /* Send data */
    SPI_SendData8(M4_SPI2, TxData);
    retry = 0;

    /* Wait rx buffer full */
    while (Reset == SPI_GetFlag(M4_SPI2, SpiFlagReceiveBufferFull))
    {
        retry++;

        if(retry > 200)return 0;
    }

    /* Receive data */
    u8Byte = SPI_ReceiveData8(M4_SPI2);

    return u8Byte;

    #endif

}































