#include "spi.h"
//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK��ӢSTM32������
//SPI���� ����
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/9
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////

//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ������SD Card/W25Q64/NRF24L01
//SPI�ڳ�ʼ��
//�������Ƕ�SPI2�ĳ�ʼ��



void SPI2_Init(void)
{
    #if defined(AT32F4xx)

    GPIO_InitType GPIO_InitStructure;
    SPI_InitType  SPI_InitStructure;

    RCC_APB2PeriphClockCmd(	RCC_APB2PERIPH_GPIOB, ENABLE );//PORTBʱ��ʹ��
    RCC_APB1PeriphClockCmd(	RCC_APB1PERIPH_SPI2,  ENABLE );//SPI2ʱ��ʹ��

    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_13 | GPIO_Pins_14 | GPIO_Pins_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //PB13/14/15�����������
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB

    GPIO_SetBits(GPIOB, GPIO_Pins_13 | GPIO_Pins_14 | GPIO_Pins_15); //PB13/14/15����

    SPI_InitStructure.SPI_TransMode = SPI_TRANSMODE_FULLDUPLEX;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
    SPI_InitStructure.SPI_Mode = SPI_MODE_MASTER;		//����SPI����ģʽ:����Ϊ��SPI
    SPI_InitStructure.SPI_FrameSize = SPI_FRAMESIZE_8BIT;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_HIGH;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2EDGE;	//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    SPI_InitStructure.SPI_NSSSEL = SPI_NSSSEL_SOFT;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
    SPI_InitStructure.SPI_MCLKP = SPI_MCLKP_256;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
    SPI_InitStructure.SPI_FirstBit = SPI_FIRSTBIT_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
    SPI_InitStructure.SPI_CPOLY = 7;	//CRCֵ����Ķ���ʽ
    SPI_Init(SPI2, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���

    SPI_Enable(SPI2, ENABLE); //ʹ��SPI����

    SPI2_ReadWriteByte(0xff);//��������

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

    SPI2_ReadWriteByte(0xff);//��������

    #endif

}
//SPI �ٶ����ú���
//SpeedSet:
//SPI_MCLKP_2   2��Ƶ
//SPI_MCLKP_8   8��Ƶ
//SPI_MCLKP_16  16��Ƶ
//SPI_MCLKP_256 256��Ƶ

void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    #if defined(AT32F4xx)

    assert_param(IS_SPI_MCLKP(SPI_BaudRatePrescaler));
    SPI2->CTRL1 &= 0XFFC7;
    SPI2->CTRL1 |= SPI_BaudRatePrescaler;	//����SPI2�ٶ�
    SPI_Enable(SPI2, ENABLE);

    #elif defined(HC32F46x)

    SPI_SetClockDiv(M4_SPI2, SPI_BaudRatePrescaler);
    SPI_Cmd(M4_SPI2, Enable);

    #endif

}

//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI2_ReadWriteByte(u8 TxData)
{
    #if defined(AT32F4xx)

    u8 retry = 0;

    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
    {
        retry++;

        if(retry > 200)return 0;
    }

    SPI_I2S_TxData(SPI2, TxData); //ͨ������SPIx����һ������
    retry = 0;

    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
    {
        retry++;

        if(retry > 200)return 0;
    }

    return SPI_I2S_RxData(SPI2); //����ͨ��SPIx������յ�����

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































