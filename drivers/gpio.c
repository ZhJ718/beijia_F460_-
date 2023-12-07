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



//电源开关GPIO口初始化
void PwrKey_GpioInit(void)
{
    #if defined(AT32F4xx)

    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PD; 		 //PB9 下拉输入
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO口速度为50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_9;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    #elif defined(HC32F46x)
    stc_port_init_t stcPortInit;
    MEM_ZERO_STRUCT(stcPortInit);

    PORT_DebugPortSetting(0x0C, Disable);	//PB3和PA15，特殊功能引脚，取消，当做普通io
    PORT_SetFunc(PortA, Pin15, Func_Gpio, Disable);

    stcPortInit.enPinMode = Pin_Mode_In; //输入
    stcPortInit.enPullUp = Disable;		//内部上拉
    PORT_Init(PortA, Pin15, &stcPortInit);
    #endif
}

//电源开关按键按下
uint32_t Is_PwrKeyOn(void)
{
    #if defined(AT32F4xx)

    return GPIO_ReadInputDataBit(GPIOB, GPIO_Pins_9);

    #elif defined(HC32F46x)
    return PORT_GetBit(PortA, Pin15) == Set;
    #endif
}


//电源使能GPIO口初始化
void PwrEnable_GpioInit(void)
{
    #if defined(AT32F4xx)
    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //PB9 上拉输入
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO口速度为50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    #elif defined(HC32F46x)
    stc_port_init_t stcPortInit;

    PORT_SetFunc(PortB, Pin00, Func_Gpio, Disable);	//设置输出模式，进制周围模式

    MEM_ZERO_STRUCT(stcPortInit);
    stcPortInit.enPinMode = Pin_Mode_Out;	//输出
    stcPortInit.enPinOType = Pin_OType_Cmos;	//推挽模式
    stcPortInit.enPinDrv = Pin_Drv_H;	//高驱动能力
    stcPortInit.enExInt = Disable;		//无外部中断
    stcPortInit.enPullUp = Disable;		//不需要内部上拉（做输入时起作用）

    PORT_Init(PortB, Pin00, &stcPortInit);
    #endif
}
//电源使能
void PwrEnable_On(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOA, GPIO_Pins_6, Bit_SET);

    #elif defined(HC32F46x)
    PwrEnable_GpioInit();

    PORT_SetBits(PortB, Pin00);
    #endif
}
//电源禁止
void PwrEnable_Off(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOA, GPIO_Pins_6, Bit_RESET);

    #elif defined(HC32F46x)
    PwrEnable_GpioInit();

    PORT_ResetBits(PortB, Pin00);
    #endif
}


//运行灯GPIO口初始化
void RunLed_GpioInit(void)
{
    #if defined(AT32F4xx)

    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //PB6 推挽输出
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO口速度为50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_6;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    #elif defined(HC32F46x)
    stc_port_init_t stcPortInit;

    PORT_SetFunc(PortC, Pin15, Func_Gpio, Disable);	//设置输出模式，禁止周围模式

    MEM_ZERO_STRUCT(stcPortInit);
    stcPortInit.enPinMode = Pin_Mode_Out;	//输出
    stcPortInit.enPinOType = Pin_OType_Cmos;	//推挽模式
    stcPortInit.enPinDrv = Pin_Drv_H;	//高驱动能力
    stcPortInit.enExInt = Disable;		//无外部中断
    stcPortInit.enPullUp = Disable;		//不需要内部上拉（做输入时起作用）

    PORT_Init(PortC, Pin15, &stcPortInit);
    #endif
}
//运行灯亮
void RunLed_On(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOB, GPIO_Pins_6, Bit_RESET);

    #elif defined(HC32F46x)
    PORT_ResetBits(PortC, Pin15);
    #endif
}
//运行灯灭
void RunLed_Off(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOB, GPIO_Pins_6, Bit_SET);

    #elif defined(HC32F46x)
    PORT_SetBits(PortC, Pin15);
    #endif
}

//状态灯翻转
void RunLed_Toggle(void)
{
    #if defined(AT32F4xx)

    GPIOB->OPTDT ^= GPIO_Pins_6;

    #elif defined(HC32F46x)
    PORT_Toggle(PortC, Pin15);
    #endif
}

//状态灯GPIO口初始化
void RLed_GpioInit(void)
{
    #if defined(AT32F4xx)

    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //PB6 推挽输出
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO口速度为50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_7;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    #elif defined(HC32F46x)
    stc_port_init_t stcPortInit;

    PORT_SetFunc(PortC, Pin14, Func_Gpio, Disable);	//设置输出模式，禁止周围模式

    MEM_ZERO_STRUCT(stcPortInit);
    stcPortInit.enPinMode = Pin_Mode_Out;	//输出
    stcPortInit.enPinOType = Pin_OType_Cmos;	//推挽模式
    stcPortInit.enPinDrv = Pin_Drv_H;	//高驱动能力
    stcPortInit.enExInt = Disable;		//无外部中断
    stcPortInit.enPullUp = Disable;		//不需要内部上拉（做输入时起作用）

    PORT_Init(PortC, Pin14, &stcPortInit);
    #endif
}
//状态灯灯亮
void RLed_On(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOB, GPIO_Pins_7, Bit_RESET);

    #elif defined(HC32F46x)
    PORT_ResetBits(PortC, Pin14);
    #endif
}
//状态灯灯灭
void RLed_Off(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOB, GPIO_Pins_7, Bit_SET);

    #elif defined(HC32F46x)
    PORT_SetBits(PortC, Pin14);
    #endif
}
//状态灯翻转
void RLed_Toggle(void)
{
    #if defined(AT32F4xx)

    GPIOB->OPTDT ^= GPIO_Pins_7;

    #elif defined(HC32F46x)
    PORT_Toggle(PortC, Pin14);
    #endif
}

//马达GPIO口初始化
void Motor_GpioInit(void)
{
    #if defined(AT32F4xx)

    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //PA5 推挽输出
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO口速度为50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_5;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    #elif defined(HC32F46x)
    stc_port_init_t stcPortInit;

    PORT_SetFunc(PortB, Pin10, Func_Gpio, Disable);	//设置输出模式，禁止周围模式

    MEM_ZERO_STRUCT(stcPortInit);
    stcPortInit.enPinMode = Pin_Mode_Out;	//输出
    stcPortInit.enPinOType = Pin_OType_Cmos;	//推挽模式
    stcPortInit.enPinDrv = Pin_Drv_H;	//高驱动能力
    stcPortInit.enExInt = Disable;		//无外部中断
    stcPortInit.enPullUp = Disable;		//不需要内部上拉（做输入时起作用）

    PORT_Init(PortB, Pin10, &stcPortInit);
    #endif
}
//马达震动
void Motor_GpioOn(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOA, GPIO_Pins_5, Bit_SET);

    #elif defined(HC32F46x)
    PORT_SetBits(PortB, Pin10);
    #endif
}
//马达关闭
void Motor_GpioOff(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOA, GPIO_Pins_5, Bit_RESET);

    #elif defined(HC32F46x)
    PORT_ResetBits(PortB, Pin10);
    #endif
}


//蜂鸣器GPIO口初始化
void Beep_GpioInit(void)
{
    #if defined(AT32F4xx)

    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //PA5 推挽输出
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO口速度为50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_5;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    #elif defined(HC32F46x)
    stc_port_init_t stcPortInit;
    PORT_SetFunc(PortB, Pin15, Func_Gpio, Disable);	//设置输出模式，禁止周围模式

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
//蜂鸣器开
void Beep_GpioOn(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOB, GPIO_Pins_5, Bit_SET);

    #elif defined(HC32F46x)
    PORT_SetBits(PortB, Pin15);
    #endif
}
//蜂鸣器关
void Beep_GpioOff(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOB, GPIO_Pins_5, Bit_RESET);

    #elif defined(HC32F46x)
    PORT_ResetBits(PortB, Pin15);
    #endif
}
//蜂鸣器翻转
void Beep_GpioToggle(void)
{
    #if defined(AT32F4xx)

    GPIOB->OPTDT ^= GPIO_Pins_5;

    #elif defined(HC32F46x)
    PORT_Toggle(PortB, Pin15);
    #endif
}




//W25QXX片选引脚初始化
void W25QXX_CS_GpioInit(void)
{

    #if defined(AT32F4xx)

    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);	 //使能PA,PB，PC端口时钟

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO口速度为50MHz
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

//W25QXX片选拉高
void W25QXX_CS_SET(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOB, GPIO_Pins_12, Bit_SET);

    #elif defined(HC32F46x)

    PORT_SetBits(PortA, Pin00);

    #endif
}

//W25QXX片选拉低
void W25QXX_CS_RESET(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOB, GPIO_Pins_12, Bit_RESET);

    #elif defined(HC32F46x)

    PORT_ResetBits(PortA, Pin00);

    #endif

}

//Zigbee复位引脚初始化
void Zigbee_GpioResetInit(void)
{
    #if defined(AT32F4xx)

    GPIO_InitType  GPIO_InitStructure;

    //PA4 蓝牙复位引脚
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO口速度为50MHz

    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8;	   	 	//端口配置
    GPIO_Init(GPIOA, &GPIO_InitStructure);	  			//根据设定参数初始化GPIOA

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

//Zigbee复位引脚拉低
void Zigbee_GpioResetOn(void)
{
    Zigbee_GpioResetInit();

    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOA, GPIO_Pins_8, Bit_RESET);

    #elif defined(HC32F46x)

    PORT_ResetBits(PortB, Pin05);

    #endif
}

//Zigbee复位引脚拉高
void Zigbee_GpioResetOff(void)
{
    Zigbee_GpioResetInit();

    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOA, GPIO_Pins_8, Bit_SET);

    #elif defined(HC32F46x)
    PORT_SetBits(PortB, Pin05);

    #endif
}

//Gps复位引脚初始化
void Gps_GpioResetInit(void)
{
    #if defined(AT32F4xx)

    GPIO_InitType  GPIO_InitStructure;

    //PA4 蓝牙复位引脚
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO口速度为50MHz

    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_2;	   	 	//端口配置
    GPIO_Init(GPIOB, &GPIO_InitStructure);	  			//根据设定参数初始化GPIOA

    #elif defined(HC32F46x)


    #endif

}

//Gps复位引脚拉低
void Gps_GpioResetOn(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOB, GPIO_Pins_2, Bit_RESET);

    #elif defined(HC32F46x)



    #endif

}

//Gps复位引脚拉高
void Gps_GpioResetOff(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOB, GPIO_Pins_2, Bit_SET);

    #elif defined(HC32F46x)



    #endif

}




//发烟罐GPIO口初始化
void Fume_GpioInit(void)
{

    #if defined(AT32F4xx)

    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 	 //PA5 推挽输出
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO口速度为50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_12;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 //PB8 浮空输入
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO口速度为50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //发烟罐使能
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 	 //PB3 推挽输出
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO口速度为50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_3;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    #elif defined(HC32F46x)
    stc_port_init_t stcPortInit;
    PORT_SetFunc(PortB, Pin15, Func_Gpio, Disable);

    MEM_ZERO_STRUCT(stcPortInit);
    stcPortInit.enPinMode = Pin_Mode_In; //输入
    stcPortInit.enPullUp = Disable;		//禁止内部上拉

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

//发烟罐安装检测
uint32_t Fume_GpioIsInstalled(void)
{
    #if defined(AT32F4xx)

    uint8_t Dat = 30;

    GPIO_WriteBit(GPIOB, GPIO_Pins_3, Bit_SET);	//电源

    while(Dat--);

    Dat = GPIO_ReadInputDataBit(GPIOB, GPIO_Pins_8);
    GPIO_WriteBit(GPIOB, GPIO_Pins_3, Bit_RESET);	//电源

    return Dat == Bit_SET;

    #elif defined(HC32F46x)
    uint8_t Dat = PORT_GetBit(PortB, Pin15);

    return Dat == Set;
    #endif
}


//发烟罐点烟
void Fume_GpioOn(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOB, GPIO_Pins_3, Bit_SET);	//电源
    GPIO_WriteBit(GPIOA, GPIO_Pins_12, Bit_SET);

    #elif defined(HC32F46x)
    PORT_SetBits(PortB, Pin14);
    #endif
}
//发烟罐关闭
void Fume_GpioOff(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOB, GPIO_Pins_3, Bit_RESET);	//电源
    GPIO_WriteBit(GPIOA, GPIO_Pins_12, Bit_RESET);

    #elif defined(HC32F46x)
    PORT_ResetBits(PortB, Pin14);
    #endif
}


//语音芯片1 忙引脚初始化
void Wtn6_1_GpioBusyInit(void)
{
    #if defined(AT32F4xx)

    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU; 		 //PC7 上拉输入
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO口速度为50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_7;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    #elif defined(HC32F46x)
    stc_port_init_t stcPortInit;

    PORT_SetFunc(PortA, Pin05, Func_Gpio, Disable);
    MEM_ZERO_STRUCT(stcPortInit);
    stcPortInit.enPinMode = Pin_Mode_In; //输入
    stcPortInit.enPullUp = Enable;		//内部上拉
    PORT_Init(PortA, Pin05, &stcPortInit);
    #endif


}
//语音芯片2 忙引脚初始化
void Wtn6_2_GpioBusyInit(void)
{
    #if defined(AT32F4xx)
    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU; 		 //PC9 上拉输入
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO口速度为50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_9;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    #elif defined(HC32F46x)

    #endif

}

//语音芯片1 数据引脚初始化
void Wtn6_1_GpioDataInit(void)
{
    #if defined(AT32F4xx)

    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //PC6 推挽输出
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO口速度为50MHz
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

//语音芯片2 数据引脚初始化
void Wtn6_2_GpioDataInit(void)
{
    #if defined(AT32F4xx)
    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //PC8 推挽输出
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO口速度为50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_8;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    #elif defined(HC32F46x)

    #endif

}

//语音芯片1 数据引脚拉高
void Wtn6_1_GpioDataSet(void)
{
    #if defined(AT32F4xx)
    GPIO_WriteBit(GPIOC, GPIO_Pins_6, Bit_SET);
    #elif defined(HC32F46x)
    PORT_SetBits(PortA, Pin04);
    #endif

}
//语音芯片1 数据引脚拉低
void Wtn6_1_GpioDataReset(void)
{
    #if defined(AT32F4xx)

    GPIO_WriteBit(GPIOC, GPIO_Pins_6, Bit_RESET);

    #elif defined(HC32F46x)
    PORT_ResetBits(PortA, Pin04);
    #endif

}

//语音芯片2 数据引脚拉高
void Wtn6_2_GpioDataSet(void)
{
    #if defined(AT32F4xx)
    GPIO_WriteBit(GPIOC, GPIO_Pins_8, Bit_SET);
    #elif defined(HC32F46x)

    #endif

}
//语音芯片2 数据引脚拉低
void Wtn6_2_GpioDataReset(void)
{
    #if defined(AT32F4xx)
    GPIO_WriteBit(GPIOC, GPIO_Pins_8, Bit_RESET);
    #elif defined(HC32F46x)

    #endif

}
//语音芯片1 忙检测
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
//语音芯片2 忙检测
uint32_t Wtn6_2_IsGpioBusySet(void)
{
    #if defined(AT32F4xx)
    uint8_t Dat = GPIO_ReadInputDataBit(GPIOC, GPIO_Pins_9);
    return Dat == Bit_RESET;
    #elif defined(HC32F46x)
    return FALSE;
    #endif

}

//探头串口电源使能引脚初始化
void LaserCom_GpioPowerOnInit(void)
{
    #if defined(AT32F4xx)
    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);	 //使能PA,PB，PC端口时钟

    GPIO_PinsRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//PA15和PB3

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO口速度为50MHz
    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_15;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    #elif defined(HC32F46x)
    stc_port_init_t stcPortInit;

    PORT_SetFunc(PortB, Pin14, Func_Gpio, Disable);	//设置输出模式，禁止周围模式
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

//探头串口电源使能
void LaserCom_GpioPowerOn(void)
{
    #if defined(AT32F4xx)
    GPIO_WriteBit(GPIOA, GPIO_Pins_15, Bit_SET);
    #elif defined(HC32F46x)
    PORT_SetBits(PortB, Pin14);
    #endif


}

//探头串口掉电
void LaserCom_GpioPowerOff(void)
{
    #if defined(AT32F4xx)
    GPIO_WriteBit(GPIOA, GPIO_Pins_15, Bit_RESET);
    #elif defined(HC32F46x)
    PORT_ResetBits(PortB, Pin14);
    #endif

}

// 上行通讯串口电源使能口初始化
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

// 上行通讯串口电源使能
void LteCom_GpioPowerOn(void)
{
    PORT_SetBits(PortA, Pin08);
}

// 上行通讯串口电源掉电
void LteCom_GpioPowerOff(void)
{
    PORT_ResetBits(PortA, Pin08);
}

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
