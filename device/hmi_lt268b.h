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

    //uint32_t	m_dwNoCommunicationTimeout;	//无通信超时

    //uint8_t 	m_byPowerOnNowCnt;	//当前已完成上电时长

    //实现相关硬件操作，可读写
    COM_HW_OPERATIONS m_sHwOps;

} HMI268B_LOWLEVEL_MAG;

HMI268B_LOWLEVEL_MAG *Hmi268b_GetLLMagHandle(void);


/****************************************************************
@FUNCTION：LT268B 底层初始化
@INPUT：硬件接口函数结构体
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 绑定底层硬件操作函数，初始化
****************************************************************/
extern void Hmi268b_LowLevelInit(COM_HW_OPERATIONS a_sHwOps);


/****************************************************************
@FUNCTION：LT268B  底层定时调度进程
@INPUT：
@OUTPUT：无
@RETURN: TRUE-正在执行底层定时调度,  FALSE-无底层定时调度, 可执行上层应用定时调度
@AUTHOR：xfw
@SPECIAL: 实现模块上电
****************************************************************/
extern uint32_t Hmi268b_LowLevelTimedProcess(void);


#define HMI268B_FRAME_START 0xAA

#define HMI268B_FRAME_END 0xEE111BE4

#define HMIBUFLEN 50

typedef enum
{
	HMI268B_FRAME_CMD_PICSHOW = 0x80,		//图片控件指令

	HMI_BUTTONCMD_SHOW = 0xA0,		//按钮控件显示
	HMI_BUTTONCMD_HIDE = 0xA1,		//按钮控件隐藏

	HMI_BARCMD = 0xB0,		//进度条指令
	
	HMI_BACKLIGHTBRIGHTNESS_CTRL = 0xBA,
	
	HMI_DISPLAY_CTRL = 0xBC,
	
	HMI268B_FRAME_CMD_CHKON = 0xBE,		//联机检查指令
	HMI268B_FRAME_CMD_TXT0SHOW = 0xC0,		//显示文本-字库1
	HMI268B_FRAME_CMD_TXT1SHOW = 0xC1,		//显示文本-字库2
	
}HMI268B_CMD;

typedef enum
{
	HMI_TIME_TXT = 0x00,		
	HMI_LINE1WEAPONNUM_TXT = 0x00,		//第一行武器弹药数
	HMI_LINE2WEAPONNUM_TXT = 0x01,		//第二行武器弹药数
	HMI_LINE3WEAPONNUM_TXT = 0x02,		//第三行武器弹药数
	
	HMI_LOCATIONX_TXT = 0x03,		//定位x数据
	HMI_LOCATIONY_TXT = 0x04,		//定位y数据
	
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
		HMI_LINE1WEAPONNUM_BAR = 0x00,  //第一行武器弹药进度    范围0000-003e      
		HMI_LINE2WEAPONNUM_BAR = 0x01,  //第二行武器弹药进度    范围0000-003e   
		HMI_LINE3WEAPONNUM_BAR = 0x02,  //第三行武器弹药进度    范围0000-003e   
		HMI_LINE1WEAPONNUMRED_BAR = 0x03,  //第一行武器弹药进度-红色    范围0000-003e      
		HMI_LINE2WEAPONNUMRED_BAR = 0x04,  //第二行武器弹药进度-红色    范围0000-003e   
		HMI_LINE3WEAPONNUMRED_BAR = 0x05,  //第三行武器弹药进度-红色    范围0000-003e   

} HMI268B_BARIDX;

typedef enum
{
	
		HMI_LINE1BULLET_PIC = 0x04,  //第一行弹药图
		HMI_LINE2BULLET_PIC = 0x05,  //第二行弹药图
		HMI_LINE3BULLET_PIC = 0x06,  //第三行弹药图

		HMI_LINE1WEAPONBACK_PIC = 0x27,  //第一行武器底图
		HMI_LINE2WEAPONBACK_PIC = 0x20,  //第二行武器底图
		HMI_LINE3WEAPONBACK_PIC = 0x28,  //第三行武器底图

		HMI_LINE1RIFLE_PIC = 0x01,  //第一行武器步枪
		HMI_LINE2RIFLE_PIC = 0x24,  //第二行武器步枪
		HMI_LINE3RIFLE_PIC = 0x25,  //第三行武器步枪

		HMI_LINE1SMG_PIC = 0x21,  //第一行武器冲锋枪
		HMI_LINE2SMG_PIC = 0x02,  //第二行武器冲锋枪
		HMI_LINE3SMG_PIC = 0x26,  //第三行武器冲锋枪

		HMI_LINE1PISTOL_PIC = 0x23,  //第一行武器手枪
		HMI_LINE2PISTOL_PIC = 0x22,  //第二行武器手枪
		HMI_LINE3PISTOL_PIC = 0x03,  //第三行武器手枪

		HMI_LINE1SAW_PIC = 0x29,  //第一行武器班用机枪
		HMI_LINE2SAW_PIC = 0x2B,  //第二行武器班用机枪
		HMI_LINE3SAW_PIC = 0x2C,  //第三行武器班用机枪

		HMI_LINE1GRENADE_PIC = 0x2D,  //第一行武器榴弹发射机
		HMI_LINE2GRENADE_PIC = 0x2E,  //第二行武器榴弹发射机
		HMI_LINE3GRENADE_PIC = 0x2F,  //第三行武器榴弹发射器

		HMI_LINE1SR_PIC = 0x30,  //第一行武器狙击枪
		HMI_LINE2SR_PIC = 0x31,  //第二行武器狙击枪
		HMI_LINE3SR_PIC = 0x32,  //第三行武器狙击枪

		HMI_LINE1HMG_PIC = 0x33,  //第一行武器重机枪
		HMI_LINE2HMG_PIC = 0x34,  //第二行武器重机枪
		HMI_LINE3HMG_PIC = 0x35,  //第三行武器重机枪

		HMI_LINE1MORTAR_PIC = 0x36,  //第一行武器迫击炮
		HMI_LINE2MORTAR_PIC = 0x37,  //第二行武器迫击炮
		HMI_LINE3MORTAR_PIC = 0x38,  //第三行武器迫击炮
		
		HMI_WPLINE1BG_PIC = 0x48,  //第一行武器整行底图
		HMI_WPLINE2BG_PIC = 0x49,  //第二行武器整行底图
		HMI_WPLINE3BG_PIC = 0x4A,  //第三行武器整行底图
		
		HMI_NOSIGNAL_PIC = 0X3A,		//无信号图标
		HMI_SIGNAL_PIC = 0X6B,		//信号图标
		HMI_REDBELONG_PIC = 0X3C,		//所属红方
		HMI_BLUEBELONG_PIC = 0X3D,		//所属蓝方
		HMI_YELLOWBELONG_PIC = 0X3E,		//所属黄方
		HMI_WHITEBELONG_PIC = 0X3F,		//所属白方
		HMI_BELONGBACKGROUND_PIC = 0X40,		//属方底图
		HMI_INJURYBACKGROUND_PIC = 0X41,		//伤势-底图
		HMI_INJURYSW_PIC = 0X42,		//伤势-死亡
		HMI_INJURYWZS_PIC = 0X43,		//伤势-危重伤
		HMI_INJURYHDS_PIC = 0X44,		//伤势-重度伤
		HMI_INJURYZDS_PIC = 0X45,		//伤势-中度伤
		HMI_INJURYQS_PIC = 0X46,		//伤势-轻伤
		HMI_INJURYZC_PIC = 0X47,		//伤势-正常

    PIC_CLOTHBATTERYLOW = 0x4C,  //背夹电池低电量图标
    PIC_CLOTHBATTERY25 = 0x4D,  //背夹电池25%图标
    PIC_CLOTHBATTERY50 = 0x4E,  //背夹电池50%图标
    PIC_CLOTHBATTERY75 = 0x4F,  //背夹电池75%图标
    PIC_CLOTHBATTERY100 = 0x50,  //背夹电池100%图标
		
		HMI_HELMETBATTERYLOW_PIC = 0x51,		//头盔电池低电量图标
		HMI_HELMETBATTERY25_PIC = 0x52,		//头盔电池25%图标
		HMI_HELMETBATTERY50_PIC = 0x53,		//头盔电池50%图标
		HMI_HELMETBATTERY75_PIC = 0x54,		//头盔电池75%图标
		HMI_HELMETBATTERY100_PIC = 0x55,		//头盔电池100%图标
		HMI_HELMETBATTERY_BACK = 0x65,		//头盔电池底图
		
		HMI_WEAPON1BATTERYLOW_PIC = 0x56,		//武器1电池低电量图标
		HMI_WEAPON1BATTERY25_PIC = 0x57,		//武器1电池25%图标
		HMI_WEAPON1BATTERY50_PIC = 0x58,		//武器1电池50%图标
		HMI_WEAPON1BATTERY75_PIC = 0x59,		//武器1电池75%图标
		HMI_WEAPON1BATTERY100_PIC = 0x5A,		//武器1电池100%图标

		HMI_WEAPON2BATTERYLOW_PIC = 0x5B,		//武器1电池低电量图标
		HMI_WEAPON2BATTERY25_PIC = 0x5C,		//武器1电池25%图标
		HMI_WEAPON2BATTERY50_PIC = 0x5D,		//武器1电池50%图标
		HMI_WEAPON2BATTERY75_PIC = 0x5E,		//武器1电池75%图标
		HMI_WEAPON2BATTERY100_PIC = 0x5F,		//武器1电池100%图标

		HMI_WEAPON3BATTERYLOW_PIC = 0x60,		//武器1电池低电量图标
		HMI_WEAPON3BATTERY25_PIC = 0x61,		//武器1电池25%图标
		HMI_WEAPON3BATTERY50_PIC = 0x62,		//武器1电池50%图标
		HMI_WEAPON3BATTERY75_PIC = 0x63,		//武器1电池75%图标
		HMI_WEAPON3BATTERY100_PIC = 0x64,		//武器1电池100%图标
		
		HMI_BLACKSOLDIER_PIC = 0x07,		//黑色士兵图
		HMI_GREENSOLDIER_PIC = 0x39,		//绿色士兵图 
	
		HMI_GPS_PIC = 0x10,		//GPS图标
    HMI_NOGPS_PIC = 0x3B,		//无GPS图标
		
		HMI_TRAIN_PIC = 0x6C,		//训练图标
		HMI_MANO_PIC = 0x6D,		//演习图标
		
		HMI_SUN_PIC = 0x11,		//太阳
		HMI_MOON_PIC = 0x12,		//月亮
		
		HMI_HURTKIND_CW = 0x6E,		//伤类——错位
		HMI_HURTKIND_DFS = 0x6F,		//伤类——多发伤
		HMI_HURTKIND_DT = 0x70,		//伤类——导调
		HMI_HURTKIND_GZ = 0x71,		//伤类——骨折
		HMI_HURTKIND_HFS = 0x72,		//伤类——核辐射
		HMI_HURTKIND_HXZS = 0x73,		//伤类——化学致伤
		HMI_HURTKIND_NS = 0x74,		//伤类——扭伤
		HMI_HURTKIND_QDS = 0x75,		//伤类——枪弹伤
		HMI_HURTKIND_SS = 0x76,		//伤类——烧伤
		HMI_HURTKIND_SWZS = 0x77,		//伤类——生物致伤
		HMI_HURTKIND_TJ = 0x78,		//伤类——脱臼
		HMI_HURTKIND_ZDS = 0x79,		//伤类——震荡伤
		HMI_HURTKIND_ZJS = 0x7A,		//伤类——撞击伤
		HMI_HURTKIND_ZS = 0x7B,		//伤类——炸伤
		HMI_HURTKIND_ZTLD = 0x7C,		//伤类——肢体离断
		HMI_HURTKIND_DITU = 0x7D,		//伤类——底图
		
		HMI_BUTTONMSG_PIC = 0x7E,		//按钮按下显示的信息底图
		
		HMI_LEFTARMHURT_PIC = 0X13,		//左臂伤
		HMI_RIGHTARMHURT_PIC = 0X14,		//右臂伤
		HMI_LEFTCHESTHURT_PIC = 0X15,		//左胸伤
		HMI_RIGHTCHESTHURT_PIC = 0X16,		//右胸伤
		HMI_BELLYHURT_PIC = 0X17,		//腹部伤
		HMI_LEFTLEGHURT_PIC = 0X18,		//左腿伤
		HMI_RIGHTLEGHURT_PIC = 0X19,		//右腿伤
		HMI_HEADHURT_PIC = 0X1A,		//头伤
		HMI_BACKHURT_PIC = 0X4B,		//后背伤

} HMI268B_PICIDX;

typedef enum
{
		HMI_BUTTON = 0x00,		//按钮
	
}HMI268B_ICONIDX;

typedef char* (*WpStr)[3];


#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
