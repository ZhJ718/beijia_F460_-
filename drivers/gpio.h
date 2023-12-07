/**
  ******************************************************************************
  * @file    gpio.h
  * @author  xfw
  * @brief   This file contains definitions for device gpio
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


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_H
#define __GPIO_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "config.h"



//电源开关GPIO口初始化
void PwrKey_GpioInit(void);

//电源开关按键按下
uint32_t Is_PwrKeyOn(void);

//电源使能GPIO口初始化
void PwrEnable_GpioInit(void);

//电源使能
void PwrEnable_On(void);

//电源禁止
void PwrEnable_Off(void);

//运行灯GPIO口初始化
void RunLed_GpioInit(void);
//运行灯亮
void RunLed_On(void);

//运行灯灭
void RunLed_Off(void);

//运行灯翻转
void RunLed_Toggle(void);

//红灯GPIO口初始化
void RLed_GpioInit(void);

//红灯亮
void RLed_On(void);

//红灯灭
void RLed_Off(void);

//红灯翻转
void RLed_Toggle(void);

//告警灯GPIO口初始化
void AlarmLed_GpioInit(void);

//告警灯亮
void AlarmLed_On(void);

//告警灯灭
void AlarmLed_Off(void);

//故障灯GPIO口初始化
void FailureLed_GpioInit(void);

//故障灯亮
void FailureLed_On(void);

//故障灯灭
void FailureLed_Off(void);

//马达GPIO口初始化
void Motor_GpioInit(void);

//马达震动
void Motor_GpioOn(void);

//马达关闭
void Motor_GpioOff(void);

//发烟罐GPIO口初始化
void Fume_GpioInit(void);

//发烟罐安装检测
uint32_t Fume_GpioIsInstalled(void);

//发烟罐点烟
void Fume_GpioOn(void);

//发烟罐关闭
void Fume_GpioOff(void);


//蜂鸣器GPIO口初始化
void Beep_GpioInit(void);

//蜂鸣器开
void Beep_GpioOn(void);

//蜂鸣器关
void Beep_GpioOff(void);

//蜂鸣器翻转
void Beep_GpioToggle(void);


//W25QXX片选引脚初始化
void W25QXX_CS_GpioInit(void);

//W25QXX片选拉高
void W25QXX_CS_SET(void);

//W25QXX片选拉低
void W25QXX_CS_RESET(void);

//Zigbee复位引脚初始化
void Zigbee_GpioResetInit(void);

//Zigbee复位引脚拉低
void Zigbee_GpioResetOn(void);

//Zigbee复位引脚拉高
void Zigbee_GpioResetOff(void);

//Gps复位引脚初始化
void Gps_GpioResetInit(void);

//Gps复位引脚拉低
void Gps_GpioResetOn(void);

//Gps复位引脚拉高
void Gps_GpioResetOff(void);



//语音芯片1 忙引脚初始化
void Wtn6_1_GpioBusyInit(void);
//语音芯片1 数据引脚初始化
void Wtn6_1_GpioDataInit(void);
//语音芯片1 数据引脚拉高
void Wtn6_1_GpioDataSet(void);
//语音芯片1 数据引脚拉低
void Wtn6_1_GpioDataReset(void);
//语音芯片1 忙检测
uint32_t Wtn6_1_IsGpioBusySet(void);

//语音芯片2 忙引脚初始化
void Wtn6_2_GpioBusyInit(void);
//语音芯片2 数据引脚初始化
void Wtn6_2_GpioDataInit(void);
//语音芯片2 数据引脚拉高
void Wtn6_2_GpioDataSet(void);
//语音芯片2 数据引脚拉低
void Wtn6_2_GpioDataReset(void);
//语音芯片2 忙检测
uint32_t Wtn6_2_IsGpioBusySet(void);


//探头串口电源使能引脚初始化
void LaserCom_GpioPowerOnInit(void);

//探头串口电源使能
void LaserCom_GpioPowerOn(void);

//探头串口掉电
void LaserCom_GpioPowerOff(void);

// 上行通讯串口电源使能口初始化
void LteCom_GpioPowerOnInit(void);

// 上行通讯串口电源使能
void LteCom_GpioPowerOn(void);

// 上行通讯串口电源掉电
void LteCom_GpioPowerOff(void);


#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
