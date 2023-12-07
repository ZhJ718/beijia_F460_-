#ifndef __ADC_H
#define __ADC_H
#include "config.h"


#if defined(AT32F4xx)

typedef enum
{
    ADC_CHANNEL_INNER_VREF = 0,
    ADC_CHANNEL_2_5V,
    ADC_CHANNEL_EXBAT,
    ADC_CHANNEL_CNT,
} ADC_CHAN;

#elif defined(HC32F46x)

typedef enum
{

    ADC_CHANNEL_2_5V = 0,
    ADC_CHANNEL_EXBAT,
    ADC_CHANNEL_INNER_VREF,
    ADC_CHANNEL_CNT,
} ADC_CHAN;

#endif


//用来存放ADC转换结果，也是DMA的目标地址,3通道，每通道采集11次后面取平均数
#define ADC_SAMPLE_CNT 15	//ADC采样数

extern volatile uint16_t ADCConvertedValue[ADC_SAMPLE_CNT][ADC_CHANNEL_CNT];

//#define ADC_SAMPLE_CNT 21
#define ADC_REFERENCE 8.6577


void AdcConfig(void);



#endif
