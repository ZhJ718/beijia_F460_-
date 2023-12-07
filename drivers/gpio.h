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



//��Դ����GPIO�ڳ�ʼ��
void PwrKey_GpioInit(void);

//��Դ���ذ�������
uint32_t Is_PwrKeyOn(void);

//��Դʹ��GPIO�ڳ�ʼ��
void PwrEnable_GpioInit(void);

//��Դʹ��
void PwrEnable_On(void);

//��Դ��ֹ
void PwrEnable_Off(void);

//���е�GPIO�ڳ�ʼ��
void RunLed_GpioInit(void);
//���е���
void RunLed_On(void);

//���е���
void RunLed_Off(void);

//���еƷ�ת
void RunLed_Toggle(void);

//���GPIO�ڳ�ʼ��
void RLed_GpioInit(void);

//�����
void RLed_On(void);

//�����
void RLed_Off(void);

//��Ʒ�ת
void RLed_Toggle(void);

//�澯��GPIO�ڳ�ʼ��
void AlarmLed_GpioInit(void);

//�澯����
void AlarmLed_On(void);

//�澯����
void AlarmLed_Off(void);

//���ϵ�GPIO�ڳ�ʼ��
void FailureLed_GpioInit(void);

//���ϵ���
void FailureLed_On(void);

//���ϵ���
void FailureLed_Off(void);

//���GPIO�ڳ�ʼ��
void Motor_GpioInit(void);

//�����
void Motor_GpioOn(void);

//���ر�
void Motor_GpioOff(void);

//���̹�GPIO�ڳ�ʼ��
void Fume_GpioInit(void);

//���̹ް�װ���
uint32_t Fume_GpioIsInstalled(void);

//���̹޵���
void Fume_GpioOn(void);

//���̹޹ر�
void Fume_GpioOff(void);


//������GPIO�ڳ�ʼ��
void Beep_GpioInit(void);

//��������
void Beep_GpioOn(void);

//��������
void Beep_GpioOff(void);

//��������ת
void Beep_GpioToggle(void);


//W25QXXƬѡ���ų�ʼ��
void W25QXX_CS_GpioInit(void);

//W25QXXƬѡ����
void W25QXX_CS_SET(void);

//W25QXXƬѡ����
void W25QXX_CS_RESET(void);

//Zigbee��λ���ų�ʼ��
void Zigbee_GpioResetInit(void);

//Zigbee��λ��������
void Zigbee_GpioResetOn(void);

//Zigbee��λ��������
void Zigbee_GpioResetOff(void);

//Gps��λ���ų�ʼ��
void Gps_GpioResetInit(void);

//Gps��λ��������
void Gps_GpioResetOn(void);

//Gps��λ��������
void Gps_GpioResetOff(void);



//����оƬ1 æ���ų�ʼ��
void Wtn6_1_GpioBusyInit(void);
//����оƬ1 �������ų�ʼ��
void Wtn6_1_GpioDataInit(void);
//����оƬ1 ������������
void Wtn6_1_GpioDataSet(void);
//����оƬ1 ������������
void Wtn6_1_GpioDataReset(void);
//����оƬ1 æ���
uint32_t Wtn6_1_IsGpioBusySet(void);

//����оƬ2 æ���ų�ʼ��
void Wtn6_2_GpioBusyInit(void);
//����оƬ2 �������ų�ʼ��
void Wtn6_2_GpioDataInit(void);
//����оƬ2 ������������
void Wtn6_2_GpioDataSet(void);
//����оƬ2 ������������
void Wtn6_2_GpioDataReset(void);
//����оƬ2 æ���
uint32_t Wtn6_2_IsGpioBusySet(void);


//̽ͷ���ڵ�Դʹ�����ų�ʼ��
void LaserCom_GpioPowerOnInit(void);

//̽ͷ���ڵ�Դʹ��
void LaserCom_GpioPowerOn(void);

//̽ͷ���ڵ���
void LaserCom_GpioPowerOff(void);

// ����ͨѶ���ڵ�Դʹ�ܿڳ�ʼ��
void LteCom_GpioPowerOnInit(void);

// ����ͨѶ���ڵ�Դʹ��
void LteCom_GpioPowerOn(void);

// ����ͨѶ���ڵ�Դ����
void LteCom_GpioPowerOff(void);


#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
