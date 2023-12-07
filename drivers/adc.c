#include "adc.h"
#include "config.h"


volatile uint16_t ADCConvertedValue[ADC_SAMPLE_CNT][ADC_CHANNEL_CNT];

#if defined(AT32F4xx)



/*����DMA��ADC��ͨ���ɼ�*/



void ADC1_DMA1_CHANNEL1_Init(void)
{
    DMA_InitType DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1, ENABLE);//ʹ��ʱ��
    DMA_Reset(DMA1_Channel1);    //��ͨ��һ�Ĵ�����ΪĬ��ֵ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (ADC1->RDOR); //�ò������Զ���DMA�������ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue;//�ò������Զ���DMA�ڴ����ַ(ת���������ĵ�ַ)
    DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALSRC;//�ò����涨����������Ϊ���ݴ����Ŀ�ĵػ�����Դ���˴�����Ϊ��Դ
    DMA_InitStructure.DMA_BufferSize = ADC_CHANNEL_CNT * ADC_SAMPLE_CNT; //����ָ��DMAͨ����DMA����Ĵ�С,��λΪ���ݵ�λ������Ҳ����ADCConvertedValue�Ĵ�С
    DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;//�趨�����ַ�Ĵ����������,�˴���Ϊ���� Disable
    DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;//�����趨�ڴ��ַ�Ĵ����������,�˴���Ϊ������Enable
    DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_HALFWORD;//���ݿ��Ϊ16λ
    DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_HALFWORD;//���ݿ��Ϊ16λ
    DMA_InitStructure.DMA_Mode = DMA_MODE_CIRCULAR; //������ѭ������ģʽ
    DMA_InitStructure.DMA_Priority = DMA_PRIORITY_HIGH;//DMAͨ��ӵ�и����ȼ� �ֱ�4���ȼ� �͡��С��ߡ��ǳ���
    DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;//��ֹDMAͨ�����ڴ浽�ڴ洫��
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);//����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��
    DMA_ChannelEnable(DMA1_Channel1, ENABLE);//����DMAͨ��һ
}

void ADC1_Init(void)
{
    ADC_InitType ADC_InitStructure;
    GPIO_InitType GPIO_InitStructure;

    /*IO��ADCʹ��ʱ��*/
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA | RCC_APB2PERIPH_ADC1	, ENABLE );	 //ʹ��ADC1ͨ��ʱ��
    RCC_ADCCLKConfig(RCC_APB2CLK_Div16);    //����ADC��Ƶ����6 72M/16 = 4.5M , ADC���ʱ�䲻�ܳ���14M


    /*��ѹ������*/
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_ANALOG;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_ANALOG;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    ADC_Reset(ADC1);  //��λADC1
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousMode = ENABLE; //����ת��
    ADC_InitStructure.ADC_ExternalTrig = ADC_ExternalTrig_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NumOfChannel = 3;
    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_Vrefint, 1, ADC_SampleTime_239_5); //ͨ��ʮ��ת��������浽ADCConvertedValue[0~10][0]
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 2, ADC_SampleTime_239_5); //ͨ��һת��������浽ADCConvertedValue[0~10][1]
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 3, ADC_SampleTime_239_5); //ͨ����ת��������浽ADCConvertedValue[0~10][2]

    ADC_DMACtrl(ADC1, ENABLE);//����ADC��DMA֧��

    /* Enables Temperature Sensor and Vrefint Channel */
    ADC_TempSensorVrefintCtrl(ENABLE);

    ADC_Ctrl(ADC1, ENABLE);
    ADC_RstCalibration(ADC1);

    while(ADC_GetResetCalibrationStatus(ADC1));

    ADC_StartCalibration(ADC1);

    while(ADC_GetCalibrationStatus(ADC1));

}

void ADC1_Start(void)
{
    ADC_SoftwareStartConvCtrl(ADC1, ENABLE);//��ʼ�ɼ�
}

/******************************************************************
*��������:adc_init
*��������:��ʼ��
*�������:
*�������:
*����ֵ:
*����˵��:
*�޸�����    �汾��   �޸���    �޸�����
*---------------------------------------------------
*                              ���
*****************************************************************
void  adc_init(void)
{
	ADC_InitType ADC_InitStructure;
	GPIO_InitType GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOC |RCC_APB2PERIPH_GPIOA |RCC_APB2PERIPH_ADC1	, ENABLE );	  //ʹ��ADC1ͨ��ʱ��


	RCC_ADCCLKConfig(RCC_APB2CLK_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M


	//PA0 ��Ϊģ��ͨ����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pins_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_ANALOG;		//ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//PA1 ��Ϊģ��ͨ����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pins_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_ANALOG;		//ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//PC3 ��Ϊģ��ͨ����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pins_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_ANALOG;		//ģ����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	ADC_Reset(ADC1);  //��λADC1

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrig_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���


	ADC_Ctrl(ADC1, ENABLE);	//ʹ��ָ����ADC1

	ADC_RstCalibration(ADC1);	//ʹ�ܸ�λУ׼

	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����

	ADC_StartCalibration(ADC1);	 //����ADУ׼

	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����

     VREFINT_CAL = *(__IO uint16_t *)(0X1FFFF7BA);

   //	ADC_SoftwareStartConvCtrl(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������

}				  */

/******************************************************************
*��������:Get_Adc_
*��������:���ADCֵ
*�������:ͨ��,ch:ͨ��ֵ 0~3
*�������:
*����ֵ:    AD����ֵ
*����˵��:
*�޸�����    �汾��   �޸���    �޸�����
*---------------------------------------------------
*                              ���
*****************************************************************
u16 Get_Adc(u8 ch)
{
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239_5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����

	ADC_SoftwareStartConvCtrl(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������

	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}*/
/******************************************************************
*��������:Get_Adc_Average
*��������:��ADƽ��ֵ
*�������:ͨ����������
*�������:
*����ֵ:    AD����ƽ��ֵ
*����˵��:
*�޸�����    �汾��   �޸���    �޸�����
*---------------------------------------------------
*                              ���
*****************************************************************
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		//delay_ms(1);
	}
	return temp_val/times;
} 	 */


/******************************************************************
*��������:get_ref_batv
*��������:���ڲ���ص�ѹ
*�������:��
*�������:
*����ֵ:    u16��ѹֵ
*����˵��:
*�޸�����    �汾��   �޸���    �޸�����
*---------------------------------------------------
*                              ���
*****************************************************************
u16 get_ref_batv(void)
{
	u16 adcx;
	float temp;
	//  AD13 ��������  �ڲ����
	adcx=Get_Adc_Average(ADC_Channel_0,5);
	//temp=(3.3*VREFINT_CAL)/adcx;
	temp=(float)adcx*(3.3/4096);
	//printf(" 2.5V��׼��ѹ :%fV\r\n",temp);          //��ӡ�����ʽ
	return adcx;
}*/


/******************************************************************
*��������:get_int_batv
*��������:���ڲ���ص�ѹ
*�������:��
*�������:
*����ֵ:    u16��ѹֵ
*����˵��:
*�޸�����    �汾��   �޸���    �޸�����
*---------------------------------------------------
*                              ���
*****************************************************************
u16 get_int_batv(void)
{
	u16 adcx;
	float temp;
	//  AD13 ��������  �ڲ����
	adcx=Get_Adc_Average(ADC_Channel_13,5);
	temp=(float)adcx*(6.65/4096);
	//printf(" �ڲ���ص�ѹ :%fV\r\n",temp);          //��ӡ�����ʽ
	return adcx;
}*/

/******************************************************************
*��������:get_ext_batv
*��������:���ⲿ��ص�ѹ
*�������:��
*�������:
*����ֵ:  u16��ѹֵ
*����˵��:
*�޸�����    �汾��   �޸���    �޸�����
*---------------------------------------------------
*                              ���
*****************************************************************
u16 get_ext_batv(void)
{
	u16 adcx;
	float temp;
	//  AD1 ��������  �ⲿ���
	adcx=Get_Adc_Average(ADC_Channel_1,5);   //3123 ��Ӧ6.6V (R20=100K,R21=62K)
	temp=(float)adcx*(8.6577/4096);
	//printf("ExtBat=%fV,0x%x\r\n",temp,adcx);
	return adcx;
}*/




/*
 **************************************************************************************
 *��������: ADC_GetExtBat
 *��������: ��ȡ�ⲿ��ص�ѹADC����ֵ
 *��������: �����͵�ѹֵ��ADCԭʼֵ
 *�������: ��
 *��������: 0�ɹ�/1δ����
 *��������: XFW
 *ȫ������: ��
 *����˵��: ADC_SAMPLE_CNT-ָ��ADC�������ٴκ��ٴ���  ADC_REFERENCE-ADC�ο���׼
 **************************************************************************************

extern u32 ADC_GetVrefVoltage(float *pVol,u16 *pADCVal)
{
	ChooseSort_U16(temp,ADC_SAMPLE_CNT);
	adcx=MakeU16Sum(&temp[2],ADC_SAMPLE_CNT-4);//ȥ����ֵ
	adcx /= (ADC_SAMPLE_CNT-4);//ȡƽ��
	if(pVol!=NULL){
		*pVol = (float)adcx*(ADC_REFERENCE/4096);
		//printf("ExtBat=%fV,0x%x\r\n",*pVol,adcx);
	}
	if(pADCVal!=NULL){
		*pADCVal = adcx;
	}
	return 0;

} */

void AdcConfig(void)
{
    ADC1_Init();
    ADC1_DMA1_CHANNEL1_Init();

}


#elif defined(HC32F46x)

void AdcConfig(void)
{
    stc_adc_init_t stcAdcInit;
    stc_adc_ch_cfg_t stcChCfg;
    stc_port_init_t stcPortInit;
    uint8_t au8Adc1SaSampTime[1] = {0x05};//{0x30};

    MEM_ZERO_STRUCT(stcAdcInit);
    MEM_ZERO_STRUCT(stcChCfg);
    MEM_ZERO_STRUCT(stcPortInit);

    //ʱ�ӳ�ʼ��
    CLK_SetPeriClkSource(ClkPeriSrcPclk);
    PWC_Fcg3PeriphClockCmd(PWC_FCG3_PERIPH_ADC1, Enable);

    //ADC��ʼ��
    stcAdcInit.enResolution = AdcResolution_12Bit;
    stcAdcInit.enDataAlign  = AdcDataAlign_Right;
    stcAdcInit.enAutoClear  = AdcClren_Disable;
    stcAdcInit.enScanMode   = AdcMode_SAOnce;

    ADC_Init(M4_ADC1, &stcAdcInit);
    ADC_ConfigAvg(M4_ADC1, AdcAvcnt_4);				//ƽ������

    stcPortInit.enPinMode = Pin_Mode_Ana;
    PORT_Init(PortA, Pin06 | Pin07, &stcPortInit);	//vref���ų�ʼ��|bat���ų�ʼ��

    //ch6
    stcChCfg.u32Channel  = ADC1_CH6 | ADC1_CH7 | ADC1_CH_INTERNAL;
    stcChCfg.u8Sequence  = ADC_SEQ_A;
    stcChCfg.pu8SampTime = au8Adc1SaSampTime;
    ADC_AddAdcChannel(M4_ADC1, &stcChCfg);

    ADC_AddAvgChannel(M4_ADC1, ADC1_CH6 | ADC1_CH7 | ADC1_CH_INTERNAL);

}


#endif

