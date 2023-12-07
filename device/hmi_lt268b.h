/**
  ******************************************************************************
  * @file    hmi_lt268b.h
  * @author  xfw
  * @brief   This file contains definitions for LT268B human machine interface
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2019 Beijing HuaRu Tech Co.</center></h2>
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
#ifndef __HMI_LT268B_H
#define __HMI_LT268B_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/

#include "config.h"


typedef struct
{

    uint8_t m_byIsCommunicationSucc: 1;
    uint8_t m_byRes: 7;

    //uint32_t	m_dwNoCommunicationTimeout;	//��ͨ�ų�ʱ

    //uint8_t 	m_byPowerOnNowCnt;	//��ǰ������ϵ�ʱ��

    //ʵ�����Ӳ���������ɶ�д
    COM_HW_OPERATIONS m_sHwOps;

} HMI268B_LOWLEVEL_MAG;

HMI268B_LOWLEVEL_MAG *Hmi268b_GetLLMagHandle(void);


/****************************************************************
@FUNCTION��LT268B �ײ��ʼ��
@INPUT��Ӳ���ӿں����ṹ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �󶨵ײ�Ӳ��������������ʼ��
****************************************************************/
extern void Hmi268b_LowLevelInit(COM_HW_OPERATIONS a_sHwOps);


/****************************************************************
@FUNCTION��LT268B  �ײ㶨ʱ���Ƚ���
@INPUT��
@OUTPUT����
@RETURN: TRUE-����ִ�еײ㶨ʱ����,  FALSE-�޵ײ㶨ʱ����, ��ִ���ϲ�Ӧ�ö�ʱ����
@AUTHOR��xfw
@SPECIAL: ʵ��ģ���ϵ�
****************************************************************/
extern uint32_t Hmi268b_LowLevelTimedProcess(void);


#define HMI268B_FRAME_START 0xAA

#define HMI268B_FRAME_END 0xEE111BE4

#define HMIBUFLEN 50

typedef enum
{
	HMI268B_FRAME_CMD_PICSHOW = 0x80,		//ͼƬ�ؼ�ָ��

	HMI_BUTTONCMD_SHOW = 0xA0,		//��ť�ؼ���ʾ
	HMI_BUTTONCMD_HIDE = 0xA1,		//��ť�ؼ�����

	HMI_BARCMD = 0xB0,		//������ָ��
	
	HMI_BACKLIGHTBRIGHTNESS_CTRL = 0xBA,
	
	HMI_DISPLAY_CTRL = 0xBC,
	
	HMI268B_FRAME_CMD_CHKON = 0xBE,		//�������ָ��
	HMI268B_FRAME_CMD_TXT0SHOW = 0xC0,		//��ʾ�ı�-�ֿ�1
	HMI268B_FRAME_CMD_TXT1SHOW = 0xC1,		//��ʾ�ı�-�ֿ�2
	
}HMI268B_CMD;

typedef enum
{
	HMI_TIME_TXT = 0x00,		
	HMI_LINE1WEAPONNUM_TXT = 0x00,		//��һ��������ҩ��
	HMI_LINE2WEAPONNUM_TXT = 0x01,		//�ڶ���������ҩ��
	HMI_LINE3WEAPONNUM_TXT = 0x02,		//������������ҩ��
	
	HMI_LOCATIONX_TXT = 0x03,		//��λx����
	HMI_LOCATIONY_TXT = 0x04,		//��λy����
	
	HMI_BLOOD_TXT = 0x05,
	HMI_UNUPLOADEVTNO_TXT = 0x06,
	
	HMI_BUTTONMSG1_TXT = 0x07,
	HMI_BUTTONMSG2_TXT = 0x08,
	HMI_BUTTONMSG3_TXT = 0x09,
	HMI_BUTTONMSG4_TXT = 0x0A,
	
	HMI_RES1_TXT = 0x0B,
	HMI_RES2_TXT = 0x0C
	
}HMI268B_TXT0;

typedef enum
{
	HMI_POSTURE_TXT = 0X00,
	HMI_PID_TXT = 0X01,		//PID
	
}HMI268B_TXT1;

typedef enum
{
		HMI_LINE1WEAPONNUM_BAR = 0x00,  //��һ��������ҩ����    ��Χ0000-003e      
		HMI_LINE2WEAPONNUM_BAR = 0x01,  //�ڶ���������ҩ����    ��Χ0000-003e   
		HMI_LINE3WEAPONNUM_BAR = 0x02,  //������������ҩ����    ��Χ0000-003e   
		HMI_LINE1WEAPONNUMRED_BAR = 0x03,  //��һ��������ҩ����-��ɫ    ��Χ0000-003e      
		HMI_LINE2WEAPONNUMRED_BAR = 0x04,  //�ڶ���������ҩ����-��ɫ    ��Χ0000-003e   
		HMI_LINE3WEAPONNUMRED_BAR = 0x05,  //������������ҩ����-��ɫ    ��Χ0000-003e   

} HMI268B_BARIDX;

typedef enum
{
	
		HMI_LINE1BULLET_PIC = 0x04,  //��һ�е�ҩͼ
		HMI_LINE2BULLET_PIC = 0x05,  //�ڶ��е�ҩͼ
		HMI_LINE3BULLET_PIC = 0x06,  //�����е�ҩͼ

		HMI_LINE1WEAPONBACK_PIC = 0x27,  //��һ��������ͼ
		HMI_LINE2WEAPONBACK_PIC = 0x20,  //�ڶ���������ͼ
		HMI_LINE3WEAPONBACK_PIC = 0x28,  //������������ͼ

		HMI_LINE1RIFLE_PIC = 0x01,  //��һ��������ǹ
		HMI_LINE2RIFLE_PIC = 0x24,  //�ڶ���������ǹ
		HMI_LINE3RIFLE_PIC = 0x25,  //������������ǹ

		HMI_LINE1SMG_PIC = 0x21,  //��һ���������ǹ
		HMI_LINE2SMG_PIC = 0x02,  //�ڶ����������ǹ
		HMI_LINE3SMG_PIC = 0x26,  //�������������ǹ

		HMI_LINE1PISTOL_PIC = 0x23,  //��һ��������ǹ
		HMI_LINE2PISTOL_PIC = 0x22,  //�ڶ���������ǹ
		HMI_LINE3PISTOL_PIC = 0x03,  //������������ǹ

		HMI_LINE1SAW_PIC = 0x29,  //��һ���������û�ǹ
		HMI_LINE2SAW_PIC = 0x2B,  //�ڶ����������û�ǹ
		HMI_LINE3SAW_PIC = 0x2C,  //�������������û�ǹ

		HMI_LINE1GRENADE_PIC = 0x2D,  //��һ�������񵯷����
		HMI_LINE2GRENADE_PIC = 0x2E,  //�ڶ��������񵯷����
		HMI_LINE3GRENADE_PIC = 0x2F,  //�����������񵯷�����

		HMI_LINE1SR_PIC = 0x30,  //��һ�������ѻ�ǹ
		HMI_LINE2SR_PIC = 0x31,  //�ڶ��������ѻ�ǹ
		HMI_LINE3SR_PIC = 0x32,  //�����������ѻ�ǹ

		HMI_LINE1HMG_PIC = 0x33,  //��һ�������ػ�ǹ
		HMI_LINE2HMG_PIC = 0x34,  //�ڶ��������ػ�ǹ
		HMI_LINE3HMG_PIC = 0x35,  //�����������ػ�ǹ

		HMI_LINE1MORTAR_PIC = 0x36,  //��һ�������Ȼ���
		HMI_LINE2MORTAR_PIC = 0x37,  //�ڶ��������Ȼ���
		HMI_LINE3MORTAR_PIC = 0x38,  //�����������Ȼ���
		
		HMI_WPLINE1BG_PIC = 0x48,  //��һ���������е�ͼ
		HMI_WPLINE2BG_PIC = 0x49,  //�ڶ����������е�ͼ
		HMI_WPLINE3BG_PIC = 0x4A,  //�������������е�ͼ
		
		HMI_NOSIGNAL_PIC = 0X3A,		//���ź�ͼ��
		HMI_SIGNAL_PIC = 0X6B,		//�ź�ͼ��
		HMI_REDBELONG_PIC = 0X3C,		//�����췽
		HMI_BLUEBELONG_PIC = 0X3D,		//��������
		HMI_YELLOWBELONG_PIC = 0X3E,		//�����Ʒ�
		HMI_WHITEBELONG_PIC = 0X3F,		//�����׷�
		HMI_BELONGBACKGROUND_PIC = 0X40,		//������ͼ
		HMI_INJURYBACKGROUND_PIC = 0X41,		//����-��ͼ
		HMI_INJURYSW_PIC = 0X42,		//����-����
		HMI_INJURYWZS_PIC = 0X43,		//����-Σ����
		HMI_INJURYHDS_PIC = 0X44,		//����-�ض���
		HMI_INJURYZDS_PIC = 0X45,		//����-�ж���
		HMI_INJURYQS_PIC = 0X46,		//����-����
		HMI_INJURYZC_PIC = 0X47,		//����-����

    PIC_CLOTHBATTERYLOW = 0x4C,  //���е�ص͵���ͼ��
    PIC_CLOTHBATTERY25 = 0x4D,  //���е��25%ͼ��
    PIC_CLOTHBATTERY50 = 0x4E,  //���е��50%ͼ��
    PIC_CLOTHBATTERY75 = 0x4F,  //���е��75%ͼ��
    PIC_CLOTHBATTERY100 = 0x50,  //���е��100%ͼ��
		
		HMI_HELMETBATTERYLOW_PIC = 0x51,		//ͷ����ص͵���ͼ��
		HMI_HELMETBATTERY25_PIC = 0x52,		//ͷ�����25%ͼ��
		HMI_HELMETBATTERY50_PIC = 0x53,		//ͷ�����50%ͼ��
		HMI_HELMETBATTERY75_PIC = 0x54,		//ͷ�����75%ͼ��
		HMI_HELMETBATTERY100_PIC = 0x55,		//ͷ�����100%ͼ��
		HMI_HELMETBATTERY_BACK = 0x65,		//ͷ����ص�ͼ
		
		HMI_WEAPON1BATTERYLOW_PIC = 0x56,		//����1��ص͵���ͼ��
		HMI_WEAPON1BATTERY25_PIC = 0x57,		//����1���25%ͼ��
		HMI_WEAPON1BATTERY50_PIC = 0x58,		//����1���50%ͼ��
		HMI_WEAPON1BATTERY75_PIC = 0x59,		//����1���75%ͼ��
		HMI_WEAPON1BATTERY100_PIC = 0x5A,		//����1���100%ͼ��

		HMI_WEAPON2BATTERYLOW_PIC = 0x5B,		//����1��ص͵���ͼ��
		HMI_WEAPON2BATTERY25_PIC = 0x5C,		//����1���25%ͼ��
		HMI_WEAPON2BATTERY50_PIC = 0x5D,		//����1���50%ͼ��
		HMI_WEAPON2BATTERY75_PIC = 0x5E,		//����1���75%ͼ��
		HMI_WEAPON2BATTERY100_PIC = 0x5F,		//����1���100%ͼ��

		HMI_WEAPON3BATTERYLOW_PIC = 0x60,		//����1��ص͵���ͼ��
		HMI_WEAPON3BATTERY25_PIC = 0x61,		//����1���25%ͼ��
		HMI_WEAPON3BATTERY50_PIC = 0x62,		//����1���50%ͼ��
		HMI_WEAPON3BATTERY75_PIC = 0x63,		//����1���75%ͼ��
		HMI_WEAPON3BATTERY100_PIC = 0x64,		//����1���100%ͼ��
		
		HMI_BLACKSOLDIER_PIC = 0x07,		//��ɫʿ��ͼ
		HMI_GREENSOLDIER_PIC = 0x39,		//��ɫʿ��ͼ 
	
		HMI_GPS_PIC = 0x10,		//GPSͼ��
    HMI_NOGPS_PIC = 0x3B,		//��GPSͼ��
		
		HMI_TRAIN_PIC = 0x6C,		//ѵ��ͼ��
		HMI_MANO_PIC = 0x6D,		//��ϰͼ��
		
		HMI_SUN_PIC = 0x11,		//̫��
		HMI_MOON_PIC = 0x12,		//����
		
		HMI_HURTKIND_CW = 0x6E,		//���ࡪ����λ
		HMI_HURTKIND_DFS = 0x6F,		//���ࡪ���෢��
		HMI_HURTKIND_DT = 0x70,		//���ࡪ������
		HMI_HURTKIND_GZ = 0x71,		//���ࡪ������
		HMI_HURTKIND_HFS = 0x72,		//���ࡪ���˷���
		HMI_HURTKIND_HXZS = 0x73,		//���ࡪ����ѧ����
		HMI_HURTKIND_NS = 0x74,		//���ࡪ��Ť��
		HMI_HURTKIND_QDS = 0x75,		//���ࡪ��ǹ����
		HMI_HURTKIND_SS = 0x76,		//���ࡪ������
		HMI_HURTKIND_SWZS = 0x77,		//���ࡪ����������
		HMI_HURTKIND_TJ = 0x78,		//���ࡪ���Ѿ�
		HMI_HURTKIND_ZDS = 0x79,		//���ࡪ������
		HMI_HURTKIND_ZJS = 0x7A,		//���ࡪ��ײ����
		HMI_HURTKIND_ZS = 0x7B,		//���ࡪ��ը��
		HMI_HURTKIND_ZTLD = 0x7C,		//���ࡪ��֫�����
		HMI_HURTKIND_DITU = 0x7D,		//���ࡪ����ͼ
		
		HMI_BUTTONMSG_PIC = 0x7E,		//��ť������ʾ����Ϣ��ͼ
		
		HMI_LEFTARMHURT_PIC = 0X13,		//�����
		HMI_RIGHTARMHURT_PIC = 0X14,		//�ұ���
		HMI_LEFTCHESTHURT_PIC = 0X15,		//������
		HMI_RIGHTCHESTHURT_PIC = 0X16,		//������
		HMI_BELLYHURT_PIC = 0X17,		//������
		HMI_LEFTLEGHURT_PIC = 0X18,		//������
		HMI_RIGHTLEGHURT_PIC = 0X19,		//������
		HMI_HEADHURT_PIC = 0X1A,		//ͷ��
		HMI_BACKHURT_PIC = 0X4B,		//����

} HMI268B_PICIDX;

typedef enum
{
		HMI_BUTTON = 0x00,		//��ť
	
}HMI268B_ICONIDX;

typedef char* (*WpStr)[3];


#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
