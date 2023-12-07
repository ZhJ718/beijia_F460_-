/**
  ******************************************************************************
  * @file    gpio.c
  * @author  xfw
  * @brief   This file contains definitions for STM32103 gpio
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 Beijing HuaRu Tech Co.</center></h2>
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
#include "gpio.h"



//��Դ����GPIO�ڳ�ʼ��
void PwrKey_GpioInit(void)
{
    #if defined(AT32F4xx)

    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PD; 		 //PB9 ��������
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_9;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    #elif defined(HC32F46x)
    stc_port_init_t stcPortInit;
    MEM_ZERO_STRUCT(stcPortInit);

    PORT_DebugPortSetting(0x0C, Disable);	//PB3��PA15�����⹦�����ţ�ȡ����������ͨio
    PORT_SetFunc(PortA, Pin15, Func_Gpio, Disable);

    stcPortInit.enPinMode = Pin_Mode_In; //����
    stcPortInit.enPullUp = Disable;		//�ڲ�����
    PORT_Init(PortA, Pin15, &stcPortInit);
    #endif
}

//��Դ���ذ�������
uint32_t Is_PwrKeyOn(void)
{
    #if defined(AT32F4xx)

    return GPIO_ReadInputDataBit(GPIOB, GPIO_Pins_9);

    #elif defined(HC32F46x)
    return PORT_GetBit(PortA, Pin15) == Set;
    #endif
}


//��Դʹ��GPIO�ڳ�ʼ��
void PwrEnable_GpioInit(void)
{
    #if defined(AT32F4xx)
    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //PB9 ��������
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    #elif defined(HC32F46x)
    stc_port_init_t stcPortInit;

    PORT_SetFunc(PortB, Pin00, Func_Gpio, Disable);	//�������ģʽ��������Χģʽ

    MEM_ZERO_STRUCT(stcPortInit);
    stcPortInit.enPinMode = Pin_Mode_Out;	//���
    stcPortInit.enPinOType = Pin_OType_Cmos;	//����ģʽ
    stcPortInit.enPinDrv = Pin_Drv_H;	//����������
    stcPortInit.enExInt = Disable;		//���ⲿ�ж�
    stcPortInit.enPullUp = Disable;		//����Ҫ�ڲ�������������ʱ�����ã�

    PORT_Init(PortB, Pin00, &stcPortInit);
    #endif
}
//��Դʹ��
void PwrEnable_On(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOA, GPIO_Pins_6, Bit_SET);

    #elif defined(HC32F46x)
    PwrEnable_GpioInit();

    PORT_SetBits(PortB, Pin00);
    #endif
}
//��Դ��ֹ
void PwrEnable_Off(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOA, GPIO_Pins_6, Bit_RESET);

    #elif defined(HC32F46x)
    PwrEnable_GpioInit();

    PORT_ResetBits(PortB, Pin00);
    #endif
}


//���е�GPIO�ڳ�ʼ��
void RunLed_GpioInit(void)
{
    #if defined(AT32F4xx)

    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //PB6 �������
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    #elif defined(HC32F46x)
    stc_port_init_t stcPortInit;

    PORT_SetFunc(PortC, Pin15, Func_Gpio, Disable);	//�������ģʽ����ֹ��Χģʽ

    MEM_ZERO_STRUCT(stcPortInit);
    stcPortInit.enPinMode = Pin_Mode_Out;	//���
    stcPortInit.enPinOType = Pin_OType_Cmos;	//����ģʽ
    stcPortInit.enPinDrv = Pin_Drv_H;	//����������
    stcPortInit.enExInt = Disable;		//���ⲿ�ж�
    stcPortInit.enPullUp = Disable;		//����Ҫ�ڲ�������������ʱ�����ã�

    PORT_Init(PortC, Pin15, &stcPortInit);
    #endif
}
//���е���
void RunLed_On(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOB, GPIO_Pins_6, Bit_RESET);

    #elif defined(HC32F46x)
    PORT_ResetBits(PortC, Pin15);
    #endif
}
//���е���
void RunLed_Off(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOB, GPIO_Pins_6, Bit_SET);

    #elif defined(HC32F46x)
    PORT_SetBits(PortC, Pin15);
    #endif
}

//״̬�Ʒ�ת
void RunLed_Toggle(void)
{
    #if defined(AT32F4xx)

    GPIOB->OPTDT ^= GPIO_Pins_6;

    #elif defined(HC32F46x)
    PORT_Toggle(PortC, Pin15);
    #endif
}

//״̬��GPIO�ڳ�ʼ��
void RLed_GpioInit(void)
{
    #if defined(AT32F4xx)

    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //PB6 �������
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_7;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    #elif defined(HC32F46x)
    stc_port_init_t stcPortInit;

    PORT_SetFunc(PortC, Pin14, Func_Gpio, Disable);	//�������ģʽ����ֹ��Χģʽ

    MEM_ZERO_STRUCT(stcPortInit);
    stcPortInit.enPinMode = Pin_Mode_Out;	//���
    stcPortInit.enPinOType = Pin_OType_Cmos;	//����ģʽ
    stcPortInit.enPinDrv = Pin_Drv_H;	//����������
    stcPortInit.enExInt = Disable;		//���ⲿ�ж�
    stcPortInit.enPullUp = Disable;		//����Ҫ�ڲ�������������ʱ�����ã�

    PORT_Init(PortC, Pin14, &stcPortInit);
    #endif
}
//״̬�Ƶ���
void RLed_On(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOB, GPIO_Pins_7, Bit_RESET);

    #elif defined(HC32F46x)
    PORT_ResetBits(PortC, Pin14);
    #endif
}
//״̬�Ƶ���
void RLed_Off(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOB, GPIO_Pins_7, Bit_SET);

    #elif defined(HC32F46x)
    PORT_SetBits(PortC, Pin14);
    #endif
}
//״̬�Ʒ�ת
void RLed_Toggle(void)
{
    #if defined(AT32F4xx)

    GPIOB->OPTDT ^= GPIO_Pins_7;

    #elif defined(HC32F46x)
    PORT_Toggle(PortC, Pin14);
    #endif
}

//���GPIO�ڳ�ʼ��
void Motor_GpioInit(void)
{
    #if defined(AT32F4xx)

    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //PA5 �������
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_5;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    #elif defined(HC32F46x)
    stc_port_init_t stcPortInit;

    PORT_SetFunc(PortB, Pin10, Func_Gpio, Disable);	//�������ģʽ����ֹ��Χģʽ

    MEM_ZERO_STRUCT(stcPortInit);
    stcPortInit.enPinMode = Pin_Mode_Out;	//���
    stcPortInit.enPinOType = Pin_OType_Cmos;	//����ģʽ
    stcPortInit.enPinDrv = Pin_Drv_H;	//����������
    stcPortInit.enExInt = Disable;		//���ⲿ�ж�
    stcPortInit.enPullUp = Disable;		//����Ҫ�ڲ�������������ʱ�����ã�

    PORT_Init(PortB, Pin10, &stcPortInit);
    #endif
}
//�����
void Motor_GpioOn(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOA, GPIO_Pins_5, Bit_SET);

    #elif defined(HC32F46x)
    PORT_SetBits(PortB, Pin10);
    #endif
}
//���ر�
void Motor_GpioOff(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOA, GPIO_Pins_5, Bit_RESET);

    #elif defined(HC32F46x)
    PORT_ResetBits(PortB, Pin10);
    #endif
}


//������GPIO�ڳ�ʼ��
void Beep_GpioInit(void)
{
    #if defined(AT32F4xx)

    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //PA5 �������
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_5;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    #elif defined(HC32F46x)
    stc_port_init_t stcPortInit;
    PORT_SetFunc(PortB, Pin15, Func_Gpio, Disable);	//�������ģʽ����ֹ��Χģʽ

    MEM_ZERO_STRUCT(stcPortInit);
    /*initiallize LED port*/
    stcPortInit.enPinMode = Pin_Mode_Out;
    stcPortInit.enPinOType = Pin_OType_Cmos;
    stcPortInit.enPinDrv = Pin_Drv_H;
    stcPortInit.enExInt = Disable;
    stcPortInit.enPullUp = Disable;

    PORT_Init(PortB, Pin15, &stcPortInit);
    #endif
}
//��������
void Beep_GpioOn(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOB, GPIO_Pins_5, Bit_SET);

    #elif defined(HC32F46x)
    PORT_SetBits(PortB, Pin15);
    #endif
}
//��������
void Beep_GpioOff(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOB, GPIO_Pins_5, Bit_RESET);

    #elif defined(HC32F46x)
    PORT_ResetBits(PortB, Pin15);
    #endif
}
//��������ת
void Beep_GpioToggle(void)
{
    #if defined(AT32F4xx)

    GPIOB->OPTDT ^= GPIO_Pins_5;

    #elif defined(HC32F46x)
    PORT_Toggle(PortB, Pin15);
    #endif
}




//W25QXXƬѡ���ų�ʼ��
void W25QXX_CS_GpioInit(void)
{

    #if defined(AT32F4xx)

    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);	 //ʹ��PA,PB��PC�˿�ʱ��

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //�������
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_12;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    #elif defined(HC32F46x)

    stc_port_init_t stcPortInit;

    MEM_ZERO_STRUCT(stcPortInit);
    /*initiallize LED port*/
    stcPortInit.enPinMode = Pin_Mode_Out;
    stcPortInit.enPinOType = Pin_OType_Cmos;
    stcPortInit.enPinDrv = Pin_Drv_H;
    stcPortInit.enExInt = Disable;
    stcPortInit.enPullUp = Disable;

    PORT_Init(PortA, Pin00, &stcPortInit);

    #endif

}

//W25QXXƬѡ����
void W25QXX_CS_SET(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOB, GPIO_Pins_12, Bit_SET);

    #elif defined(HC32F46x)

    PORT_SetBits(PortA, Pin00);

    #endif
}

//W25QXXƬѡ����
void W25QXX_CS_RESET(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOB, GPIO_Pins_12, Bit_RESET);

    #elif defined(HC32F46x)

    PORT_ResetBits(PortA, Pin00);

    #endif

}

//Zigbee��λ���ų�ʼ��
void Zigbee_GpioResetInit(void)
{
    #if defined(AT32F4xx)

    GPIO_InitType  GPIO_InitStructure;

    //PA4 ������λ����
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //�������
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO���ٶ�Ϊ50MHz

    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8;	   	 	//�˿�����
    GPIO_Init(GPIOA, &GPIO_InitStructure);	  			//�����趨������ʼ��GPIOA

    #elif defined(HC32F46x)

    stc_port_init_t stcPortInit;

    MEM_ZERO_STRUCT(stcPortInit);
    /*initiallize LED port*/
    stcPortInit.enPinMode = Pin_Mode_Out;
    stcPortInit.enPinOType = Pin_OType_Cmos;
    stcPortInit.enPinDrv = Pin_Drv_H;
    stcPortInit.enExInt = Disable;
    stcPortInit.enPullUp = Disable;

    PORT_Init(PortB, Pin05, &stcPortInit);

    #endif

}

//Zigbee��λ��������
void Zigbee_GpioResetOn(void)
{
    Zigbee_GpioResetInit();

    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOA, GPIO_Pins_8, Bit_RESET);

    #elif defined(HC32F46x)

    PORT_ResetBits(PortB, Pin05);

    #endif
}

//Zigbee��λ��������
void Zigbee_GpioResetOff(void)
{
    Zigbee_GpioResetInit();

    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOA, GPIO_Pins_8, Bit_SET);

    #elif defined(HC32F46x)
    PORT_SetBits(PortB, Pin05);

    #endif
}

//Gps��λ���ų�ʼ��
void Gps_GpioResetInit(void)
{
    #if defined(AT32F4xx)

    GPIO_InitType  GPIO_InitStructure;

    //PA4 ������λ����
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //�������
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO���ٶ�Ϊ50MHz

    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_2;	   	 	//�˿�����
    GPIO_Init(GPIOB, &GPIO_InitStructure);	  			//�����趨������ʼ��GPIOA

    #elif defined(HC32F46x)


    #endif

}

//Gps��λ��������
void Gps_GpioResetOn(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOB, GPIO_Pins_2, Bit_RESET);

    #elif defined(HC32F46x)



    #endif

}

//Gps��λ��������
void Gps_GpioResetOff(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOB, GPIO_Pins_2, Bit_SET);

    #elif defined(HC32F46x)



    #endif

}




//���̹�GPIO�ڳ�ʼ��
void Fume_GpioInit(void)
{

    #if defined(AT32F4xx)

    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 	 //PA5 �������
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_12;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 //PB8 ��������
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //���̹�ʹ��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 	 //PB3 �������
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_3;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    #elif defined(HC32F46x)
    stc_port_init_t stcPortInit;
    PORT_SetFunc(PortB, Pin15, Func_Gpio, Disable);

    MEM_ZERO_STRUCT(stcPortInit);
    stcPortInit.enPinMode = Pin_Mode_In; //����
    stcPortInit.enPullUp = Disable;		//��ֹ�ڲ�����

    PORT_Init(PortB, Pin15, &stcPortInit);

    PORT_SetFunc(PortB, Pin14, Func_Gpio, Disable);

    /*initiallize LED port*/
    stcPortInit.enPinMode = Pin_Mode_Out;
    stcPortInit.enPinOType = Pin_OType_Cmos;
    stcPortInit.enPinDrv = Pin_Drv_H;
    stcPortInit.enExInt = Disable;
    stcPortInit.enPullUp = Disable;

    PORT_Init(PortB, Pin14, &stcPortInit);
    #endif
}

//���̹ް�װ���
uint32_t Fume_GpioIsInstalled(void)
{
    #if defined(AT32F4xx)

    uint8_t Dat = 30;

    GPIO_WriteBit(GPIOB, GPIO_Pins_3, Bit_SET);	//��Դ

    while(Dat--);

    Dat = GPIO_ReadInputDataBit(GPIOB, GPIO_Pins_8);
    GPIO_WriteBit(GPIOB, GPIO_Pins_3, Bit_RESET);	//��Դ

    return Dat == Bit_SET;

    #elif defined(HC32F46x)
    uint8_t Dat = PORT_GetBit(PortB, Pin15);

    return Dat == Set;
    #endif
}


//���̹޵���
void Fume_GpioOn(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOB, GPIO_Pins_3, Bit_SET);	//��Դ
    GPIO_WriteBit(GPIOA, GPIO_Pins_12, Bit_SET);

    #elif defined(HC32F46x)
    PORT_SetBits(PortB, Pin14);
    #endif
}
//���̹޹ر�
void Fume_GpioOff(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOB, GPIO_Pins_3, Bit_RESET);	//��Դ
    GPIO_WriteBit(GPIOA, GPIO_Pins_12, Bit_RESET);

    #elif defined(HC32F46x)
    PORT_ResetBits(PortB, Pin14);
    #endif
}


//����оƬ1 æ���ų�ʼ��
void Wtn6_1_GpioBusyInit(void)
{
    #if defined(AT32F4xx)

    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU; 		 //PC7 ��������
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_7;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    #elif defined(HC32F46x)
    stc_port_init_t stcPortInit;

    PORT_SetFunc(PortA, Pin05, Func_Gpio, Disable);
    MEM_ZERO_STRUCT(stcPortInit);
    stcPortInit.enPinMode = Pin_Mode_In; //����
    stcPortInit.enPullUp = Enable;		//�ڲ�����
    PORT_Init(PortA, Pin05, &stcPortInit);
    #endif


}
//����оƬ2 æ���ų�ʼ��
void Wtn6_2_GpioBusyInit(void)
{
    #if defined(AT32F4xx)
    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU; 		 //PC9 ��������
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_9;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    #elif defined(HC32F46x)

    #endif

}

//����оƬ1 �������ų�ʼ��
void Wtn6_1_GpioDataInit(void)
{
    #if defined(AT32F4xx)

    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //PC6 �������
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    #elif defined(HC32F46x)
    stc_port_init_t stcPortInit;

    MEM_ZERO_STRUCT(stcPortInit);
    /*initiallize LED port*/
    stcPortInit.enPinMode = Pin_Mode_Out;
    stcPortInit.enPinOType = Pin_OType_Cmos;
    stcPortInit.enPinDrv = Pin_Drv_H;
    stcPortInit.enExInt = Disable;
    stcPortInit.enPullUp = Disable;

    PORT_Init(PortA, Pin04, &stcPortInit);
    #endif

}

//����оƬ2 �������ų�ʼ��
void Wtn6_2_GpioDataInit(void)
{
    #if defined(AT32F4xx)
    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //PC8 �������
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    #elif defined(HC32F46x)

    #endif

}

//����оƬ1 ������������
void Wtn6_1_GpioDataSet(void)
{
    #if defined(AT32F4xx)
    GPIO_WriteBit(GPIOC, GPIO_Pins_6, Bit_SET);
    #elif defined(HC32F46x)
    PORT_SetBits(PortA, Pin04);
    #endif

}
//����оƬ1 ������������
void Wtn6_1_GpioDataReset(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOC, GPIO_Pins_6, Bit_RESET);

    #elif defined(HC32F46x)
    PORT_ResetBits(PortA, Pin04);
    #endif

}

//����оƬ2 ������������
void Wtn6_2_GpioDataSet(void)
{
    #if defined(AT32F4xx)
    GPIO_WriteBit(GPIOC, GPIO_Pins_8, Bit_SET);
    #elif defined(HC32F46x)

    #endif

}
//����оƬ2 ������������
void Wtn6_2_GpioDataReset(void)
{
    #if defined(AT32F4xx)
    GPIO_WriteBit(GPIOC, GPIO_Pins_8, Bit_RESET);
    #elif defined(HC32F46x)

    #endif

}
//����оƬ1 æ���
uint32_t Wtn6_1_IsGpioBusySet(void)
{
    #if defined(AT32F4xx)
    uint8_t Dat = GPIO_ReadInputDataBit(GPIOC, GPIO_Pins_7);
    return Dat == Bit_RESET;
    #elif defined(HC32F46x)
    volatile uint8_t Dat = PORT_GetBit(PortA, Pin05);
    return Dat == Reset;
    #endif

}
//����оƬ2 æ���
uint32_t Wtn6_2_IsGpioBusySet(void)
{
    #if defined(AT32F4xx)
    uint8_t Dat = GPIO_ReadInputDataBit(GPIOC, GPIO_Pins_9);
    return Dat == Bit_RESET;
    #elif defined(HC32F46x)
    return FALSE;
    #endif

}

//̽ͷ���ڵ�Դʹ�����ų�ʼ��
void LaserCom_GpioPowerOnInit(void)
{
    #if defined(AT32F4xx)
    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);	 //ʹ��PA,PB��PC�˿�ʱ��

    GPIO_PinsRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//PA15��PB3

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //�������
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_15;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    #elif defined(HC32F46x)
    stc_port_init_t stcPortInit;

    PORT_SetFunc(PortB, Pin14, Func_Gpio, Disable);	//�������ģʽ����ֹ��Χģʽ
    MEM_ZERO_STRUCT(stcPortInit);
    /*initiallize LED port*/
    stcPortInit.enPinMode = Pin_Mode_Out;
    stcPortInit.enPinOType = Pin_OType_Cmos;
    stcPortInit.enPinDrv = Pin_Drv_H;
    stcPortInit.enExInt = Disable;
    stcPortInit.enPullUp = Disable;

    PORT_Init(PortB, Pin14, &stcPortInit);
    #endif

}

//̽ͷ���ڵ�Դʹ��
void LaserCom_GpioPowerOn(void)
{
    #if defined(AT32F4xx)
    GPIO_WriteBit(GPIOA, GPIO_Pins_15, Bit_SET);
    #elif defined(HC32F46x)
    PORT_SetBits(PortB, Pin14);
    #endif


}

//̽ͷ���ڵ���
void LaserCom_GpioPowerOff(void)
{
    #if defined(AT32F4xx)
    GPIO_WriteBit(GPIOA, GPIO_Pins_15, Bit_RESET);
    #elif defined(HC32F46x)
    PORT_ResetBits(PortB, Pin14);
    #endif

}

// ����ͨѶ���ڵ�Դʹ�ܿڳ�ʼ��
void LteCom_GpioPowerOnInit(void)
{
    #if defined(AT32F4xx)

    #elif defined(HC32F46x)
    stc_port_init_t stcPortInit;

    MEM_ZERO_STRUCT(stcPortInit);

    stcPortInit.enPinMode = Pin_Mode_Out;
    stcPortInit.enPinOType = Pin_OType_Cmos;
    stcPortInit.enPinDrv = Pin_Drv_H;
    stcPortInit.enExInt = Disable;
    stcPortInit.enPullUp = Disable;

    PORT_Init(PortA, Pin08, &stcPortInit);
    #endif

}

// ����ͨѶ���ڵ�Դʹ��
void LteCom_GpioPowerOn(void)
{
    PORT_SetBits(PortA, Pin08);
}

// ����ͨѶ���ڵ�Դ����
void LteCom_GpioPowerOff(void)
{
    PORT_ResetBits(PortA, Pin08);
}

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
