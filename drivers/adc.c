#include "adc.h"
#include "config.h"


volatile uint16_t ADCConvertedValue[ADC_SAMPLE_CNT][ADC_CHANNEL_CNT];

#if defined(AT32F4xx)



/*基于DMA的ADC多通道采集*/



void ADC1_DMA1_CHANNEL1_Init(void)
{
    DMA_InitType DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1, ENABLE);//使能时钟
    DMA_Reset(DMA1_Channel1);    //将通道一寄存器设为默认值
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (ADC1->RDOR); //该参数用以定义DMA外设基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue;//该参数用以定义DMA内存基地址(转换结果保存的地址)
    DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALSRC;//该参数规定了外设是作为数据传输的目的地还是来源，此处是作为来源
    DMA_InitStructure.DMA_BufferSize = ADC_CHANNEL_CNT * ADC_SAMPLE_CNT; //定义指定DMA通道的DMA缓存的大小,单位为数据单位。这里也就是ADCConvertedValue的大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;//设定外设地址寄存器递增与否,此处设为不变 Disable
    DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;//用来设定内存地址寄存器递增与否,此处设为递增，Enable
    DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_HALFWORD;//数据宽度为16位
    DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_HALFWORD;//数据宽度为16位
    DMA_InitStructure.DMA_Mode = DMA_MODE_CIRCULAR; //工作在循环缓存模式
    DMA_InitStructure.DMA_Priority = DMA_PRIORITY_HIGH;//DMA通道拥有高优先级 分别4个等级 低、中、高、非常高
    DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;//禁止DMA通道的内存到内存传输
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);//根据DMA_InitStruct中指定的参数初始化DMA的通道
    DMA_ChannelEnable(DMA1_Channel1, ENABLE);//启动DMA通道一
}

void ADC1_Init(void)
{
    ADC_InitType ADC_InitStructure;
    GPIO_InitType GPIO_InitStructure;

    /*IO和ADC使能时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA | RCC_APB2PERIPH_ADC1	, ENABLE );	 //使能ADC1通道时钟
    RCC_ADCCLKConfig(RCC_APB2CLK_Div16);    //设置ADC分频因子6 72M/16 = 4.5M , ADC最大时间不能超过14M


    /*电压采样口*/
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_ANALOG;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_ANALOG;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    ADC_Reset(ADC1);  //复位ADC1
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousMode = ENABLE; //连续转换
    ADC_InitStructure.ADC_ExternalTrig = ADC_ExternalTrig_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NumOfChannel = 3;
    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_Vrefint, 1, ADC_SampleTime_239_5); //通道十三转换结果保存到ADCConvertedValue[0~10][0]
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 2, ADC_SampleTime_239_5); //通道一转换结果保存到ADCConvertedValue[0~10][1]
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 3, ADC_SampleTime_239_5); //通道二转换结果保存到ADCConvertedValue[0~10][2]

    ADC_DMACtrl(ADC1, ENABLE);//开启ADC的DMA支持

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
    ADC_SoftwareStartConvCtrl(ADC1, ENABLE);//开始采集
}

/******************************************************************
*函数名称:adc_init
*功能描述:初始化
*输入参数:
*输出参数:
*返回值:
*其它说明:
*修改日期    版本号   修改人    修改内容
*---------------------------------------------------
*                              查飞
*****************************************************************
void  adc_init(void)
{
	ADC_InitType ADC_InitStructure;
	GPIO_InitType GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOC |RCC_APB2PERIPH_GPIOA |RCC_APB2PERIPH_ADC1	, ENABLE );	  //使能ADC1通道时钟


	RCC_ADCCLKConfig(RCC_APB2CLK_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M


	//PA0 作为模拟通道输入引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pins_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_ANALOG;		//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//PA1 作为模拟通道输入引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pins_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_ANALOG;		//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//PC3 作为模拟通道输入引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pins_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_ANALOG;		//模拟输入引脚
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	ADC_Reset(ADC1);  //复位ADC1

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrig_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器


	ADC_Ctrl(ADC1, ENABLE);	//使能指定的ADC1

	ADC_RstCalibration(ADC1);	//使能复位校准

	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束

	ADC_StartCalibration(ADC1);	 //开启AD校准

	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束

     VREFINT_CAL = *(__IO uint16_t *)(0X1FFFF7BA);

   //	ADC_SoftwareStartConvCtrl(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

}				  */

/******************************************************************
*函数名称:Get_Adc_
*功能描述:获得ADC值
*输入参数:通道,ch:通道值 0~3
*输出参数:
*返回值:    AD采样值
*其它说明:
*修改日期    版本号   修改人    修改内容
*---------------------------------------------------
*                              查飞
*****************************************************************
u16 Get_Adc(u8 ch)
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239_5 );	//ADC1,ADC通道,采样时间为239.5周期

	ADC_SoftwareStartConvCtrl(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}*/
/******************************************************************
*函数名称:Get_Adc_Average
*功能描述:读AD平均值
*输入参数:通道，读次数
*输出参数:
*返回值:    AD采样平均值
*其它说明:
*修改日期    版本号   修改人    修改内容
*---------------------------------------------------
*                              查飞
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
*函数名称:get_ref_batv
*功能描述:读内部电池电压
*输入参数:无
*输出参数:
*返回值:    u16电压值
*其它说明:
*修改日期    版本号   修改人    修改内容
*---------------------------------------------------
*                              查飞
*****************************************************************
u16 get_ref_batv(void)
{
	u16 adcx;
	float temp;
	//  AD13 采样测试  内部电池
	adcx=Get_Adc_Average(ADC_Channel_0,5);
	//temp=(3.3*VREFINT_CAL)/adcx;
	temp=(float)adcx*(3.3/4096);
	//printf(" 2.5V基准电压 :%fV\r\n",temp);          //打印浮点格式
	return adcx;
}*/


/******************************************************************
*函数名称:get_int_batv
*功能描述:读内部电池电压
*输入参数:无
*输出参数:
*返回值:    u16电压值
*其它说明:
*修改日期    版本号   修改人    修改内容
*---------------------------------------------------
*                              查飞
*****************************************************************
u16 get_int_batv(void)
{
	u16 adcx;
	float temp;
	//  AD13 采样测试  内部电池
	adcx=Get_Adc_Average(ADC_Channel_13,5);
	temp=(float)adcx*(6.65/4096);
	//printf(" 内部电池电压 :%fV\r\n",temp);          //打印浮点格式
	return adcx;
}*/

/******************************************************************
*函数名称:get_ext_batv
*功能描述:读外部电池电压
*输入参数:无
*输出参数:
*返回值:  u16电压值
*其它说明:
*修改日期    版本号   修改人    修改内容
*---------------------------------------------------
*                              查飞
*****************************************************************
u16 get_ext_batv(void)
{
	u16 adcx;
	float temp;
	//  AD1 采样测试  外部电池
	adcx=Get_Adc_Average(ADC_Channel_1,5);   //3123 对应6.6V (R20=100K,R21=62K)
	temp=(float)adcx*(8.6577/4096);
	//printf("ExtBat=%fV,0x%x\r\n",temp,adcx);
	return adcx;
}*/




/*
 **************************************************************************************
 *函数名称: ADC_GetExtBat
 *功能描述: 获取外部电池电压ADC采样值
 *输入描述: 浮点型电压值、ADC原始值
 *输出描述: 无
 *返回描述: 0成功/1未更新
 *作者日期: XFW
 *全局声明: 无
 *特殊说明: ADC_SAMPLE_CNT-指定ADC采样多少次后再处理  ADC_REFERENCE-ADC参考基准
 **************************************************************************************

extern u32 ADC_GetVrefVoltage(float *pVol,u16 *pADCVal)
{
	ChooseSort_U16(temp,ADC_SAMPLE_CNT);
	adcx=MakeU16Sum(&temp[2],ADC_SAMPLE_CNT-4);//去掉最值
	adcx /= (ADC_SAMPLE_CNT-4);//取平均
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

    //时钟初始化
    CLK_SetPeriClkSource(ClkPeriSrcPclk);
    PWC_Fcg3PeriphClockCmd(PWC_FCG3_PERIPH_ADC1, Enable);

    //ADC初始化
    stcAdcInit.enResolution = AdcResolution_12Bit;
    stcAdcInit.enDataAlign  = AdcDataAlign_Right;
    stcAdcInit.enAutoClear  = AdcClren_Disable;
    stcAdcInit.enScanMode   = AdcMode_SAOnce;

    ADC_Init(M4_ADC1, &stcAdcInit);
    ADC_ConfigAvg(M4_ADC1, AdcAvcnt_4);				//平均次数

    stcPortInit.enPinMode = Pin_Mode_Ana;
    PORT_Init(PortA, Pin06 | Pin07, &stcPortInit);	//vref引脚初始化|bat引脚初始化

    //ch6
    stcChCfg.u32Channel  = ADC1_CH6 | ADC1_CH7 | ADC1_CH_INTERNAL;
    stcChCfg.u8Sequence  = ADC_SEQ_A;
    stcChCfg.pu8SampTime = au8Adc1SaSampTime;
    ADC_AddAdcChannel(M4_ADC1, &stcChCfg);

    ADC_AddAvgChannel(M4_ADC1, ADC1_CH6 | ADC1_CH7 | ADC1_CH_INTERNAL);

}


#endif

