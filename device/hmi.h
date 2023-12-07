/**
  ******************************************************************************
  * @file    hmi.h
  * @author  xfw
  * @brief   This file contains definitions for human machine interface
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
#ifndef __HMI_H
#define __HMI_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
//#include "stm32f0xx_hal.h"
#include "config.h"
#include "uart.h"

#define HMI_FRAME_CMDPARAM_LEN_MAX  50


typedef struct
{
    uint8_t Head;	//帧头
    uint8_t Cmd;	//指令
    uint8_t CmdParaLen; //指令参数长度
    uint8_t CmdPara[HMI_FRAME_CMDPARAM_LEN_MAX];//指令参数
    uint32_t Tail; 	//帧尾
} HMI_FRAME, *PHMI_FRAME;

typedef struct
{
    uint16_t m_wAmmuCnt[HMI_AMMU_MAX];	//弹药数量

    uint16_t m_wSetValue; //缓存装定距离0-9999 m

    uint8_t m_wDamageValue; //毁伤0-100
    uint8_t m_wOilValue; //油料0-100

    uint32_t m_byIsSet: 1; //TRUE-装定

    uint32_t m_byIsAskForAmmo: 1; //TRUE-申请弹药
    uint32_t m_byIsAskForOil: 1; //TRUE-申请油料
    uint32_t m_byIsRes: 29;

    uint32_t m_byIsAmmuSet; //对应位为TRUE-配置相应弹种诸元信息

} TANK_HMI_MAG;


typedef struct
{
    uint8_t m_LocalX[30];
    uint8_t m_LocalX_len;

    uint8_t m_LocalY[30];
    uint8_t m_LocalY_len;

    uint8_t m_YinXinMethod; //0X31-瞬发 0x32-短延  0x33-延期

    uint8_t m_SendMethod; //0x31- 单发   0x32-连发

    uint8_t m_IsSend: 1;

} PJP_HMI_MAG;


typedef struct
{
    u8 m_byRID;
    u16 m_wPID;
    u8 m_strRID[TERMINAL_STRING_RID_LEN + 1];
    u8 m_strPID[TERMINAL_STRING_PID_LEN + 1];
    u8 m_strMac[TERMINAL_STRING_MAC_LEN + 1];
    u8 m_strName[TERMINAL_STRING_NAME_LEN + 1];
} TERMINAL_MAG;

typedef struct
{
    uint32_t m_IsScan: 1;	//1-扫描中  0-停止扫描
    uint32_t m_IsConnect: 1; //1-连接中  0-连接停止
    uint32_t m_IsConnectedTerminal: 1; //1-已连接  0-连接断开
    uint32_t m_IsGetLog: 1;
    uint32_t m_IsRes: 28;

    uint16_t m_wConnectPID;		//连接终端PID
    uint16_t m_wScanTime;

    uint8_t m_byTmCnt;		//扫描在线终端数量
    TERMINAL_MAG m_saTmMag[TERMINAL_MAG_MAX];	//终端信息

    uint8_t m_byStrBuf[TERMINAL_STR_BUF_LEN_MAX];
    uint16_t m_wStrLen;

    //页面2激光射击测试
    uint8_t m_byUni1;	//选择的武器弹药统一编码
    uint8_t m_byRID1;	//选择的RID
    uint16_t m_wPID1;	//选择的PID
    uint8_t m_byKEY1;	//选择的授权码
    uint8_t m_byROrB1;	//选择的红蓝

    uint8_t m_byTLQ;	//0-判死 1-复活 2-连接超级蓝牙 3-模式切换 4-弹药补充

    //页面4蓝牙子设备
    uint8_t m_byIdx;	//更新的蓝牙子设备编号 1-7
    uint8_t m_byType;	//更新的蓝牙子设备类型
    uint8_t m_aMac[6];	//更新的蓝牙子设备MAC

    //页面10排雷
    //uint8_t m_IsScanMine:1;	//1-扫描中  0-停止扫描

    //uint8_t m_IsResMine:7;
    //PARA_MINE m_aMinePara; //扫描到雷的信息

} TM_HMI_MAG;

typedef union
{
    TANK_HMI_MAG TankMag;
    PJP_HMI_MAG PjpMag;
    TM_HMI_MAG TmDetectorMag;
} HMI_DATA_MAG;


typedef struct
{
    UART_MANAGER *m_psUartMag;
    //HMI_FRAME m_sFrameRecv;
    //HMI_FRAME m_sFrameSend;

    uint8_t m_byIsWakeUp: 1; 	//标识当前屏幕是否处于唤醒状态
    uint8_t m_byIsKeyDown: 1; 	//屏幕唤醒或休眠按键被按下
    uint8_t m_byIsNoAction: 1;	//长时间未操作屏幕
    uint8_t m_byIsNeedSend: 1;	//有数据发送

    uint8_t m_byIsUpdatePage: 1; //定时刷新页面
    uint8_t m_byRes: 3;

    uint8_t m_byCurrentPage;    //当前页面
    uint16_t m_wUpdatePageTime; //定时刷新页面计时

    uint16_t m_wKeyDownTime;	//按键按下时间(*10ms)
    uint16_t m_wNoActionTime;	//未操作屏幕时间(*10ms)

    uint8_t m_byTType;	//设备类型 1-坦克
    HMI_DATA_MAG DataUnion;
} HMI_MANAGER, *PHMI_MANAGER;



typedef enum
{
    Button_Off = 0,
    Button_On = 1
} ENUM_BUTTON_STA;



/****************************************************************
@FUNCTION：解析传入的字符串，是否满足大彩组态屏协议
@INPUT：待解析数据pBuf及长度pLen
@OUTPUT：pLen真实帧长度、pIsCutted是否被分包
@RETURN: TRUE-成功(分包或解析成功) FALSE-失败
@AUTHOR：xfw
@SPECIAL:防溢出处理，25-最多不满足协议的数据长度(出现帧头，未出现帧尾)，减小该值，可以更快的从异常中恢复
****************************************************************/
uint32_t HMI_FindFrame(uint8_t *pBuf, uint16_t *pLen, uint32_t *pIsCutted);

/****************************************************************
@FUNCTION：大彩组态屏协议解析
@INPUT：待解析数据pBuf及长度Len
@OUTPUT：报文解析结构体
@RETURN:
@AUTHOR：xfw
@SPECIAL: 需要保证传入的报文是符合协议的，本函数不校验报文的正确性
***************************************************************
void HMI_FrameParse(uint8_t *pBuf,uint16_t Len,HMI_FRAME *pFrame);*/



/****************************************************************
@FUNCTION：更新进度条控件数值
@INPUT：
	Screen_id(2 个字节)：画面编号
	Control_id(2 个字节)：控件编号
	Progressvalue（4 个字节） : 用户写入的新进度条值
@OUTPUT：
	pBuf:缓存组好的包
	pLen:包长度
@RETURN: TRUE-成功(分包或解析成功) FALSE-失败
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint32_t HMI_SetProgress(uint16_t Screen_id, uint16_t Control_id, uint16_t ProgressValue, uint8_t *pBuf, uint16_t *pLen);


/****************************************************************
@FUNCTION：更新文本控件数值
@INPUT：
	Screen_id(2 个字节)：画面编号
	Control_id(2 个字节)：控件编号
	pStr（不定长） : 文本框要显示的字符串，可以为空
@OUTPUT：
	pBuf:缓存组好的包
	pLen:包长度
@RETURN: TRUE-成功(分包或解析成功) FALSE-失败
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint32_t HMI_SetText(uint16_t Screen_id, uint16_t Control_id, uint8_t *pStr, uint8_t *pBuf, uint16_t *pLen);


/****************************************************************
@FUNCTION：获取文本控件数值
@INPUT：
	Screen_id(2 个字节)：画面编号
	Control_id(2 个字节)：控件编号
@OUTPUT：
	pBuf:缓存组好的包
	pLen:包长度
@RETURN: TRUE-成功(分包或解析成功) FALSE-失败
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint32_t HMI_GetText(uint16_t Screen_id, uint16_t Control_id, uint8_t *pBuf, uint16_t *pLen);



/****************************************************************
@FUNCTION：更新按钮状态
@INPUT：
	Screen_id(2 个字节)：画面编号
	Control_id(2 个字节)：控件编号
	Status（1 个字节） : 按钮状态，见ENUM_BUTTON_STA
		0x00：按钮由按下变成弹起
		0x01：按钮由弹起变成按下
@OUTPUT：
	pBuf:缓存组好的包
	pLen:包长度
@RETURN: TRUE-成功(分包或解析成功) FALSE-失败
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint32_t HMI_SetButton(uint16_t Screen_id, uint16_t Control_id, ENUM_BUTTON_STA Status, uint8_t *pBuf, uint16_t *pLen);

typedef enum
{
    IMAGE_GREY = 0,
    IMAGE_GREEN,
    IMAGE_RED,
    IMAGE_YELLOW
} ENUM_FLASHIMAGE_STA;

/****************************************************************
@FUNCTION：更新按钮状态
@INPUT：
	Screen_id(2 个字节)：画面编号
	Control_id(2 个字节)：控件编号
	FlashImgae_ID（1 个字节） : 某一动画帧ID指定从某一帧开始播放 0-灰色 1-绿色 2-红色 3-黄色
@OUTPUT：
	pBuf:缓存组好的包
	pLen:包长度
@RETURN: TRUE-成功(分包或解析成功) FALSE-失败
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint32_t HMI_SetFlashImage(uint16_t Screen_id, uint16_t Control_id, ENUM_FLASHIMAGE_STA FlashImgae_ID, uint8_t *pBuf, uint16_t *pLen);

/****************************************************************
@FUNCTION：数据记录控件-添加常规记录
@INPUT：
	Screen_id(2 个字节)：画面编号
	Control_id(2 个字节)：控件编号
	String（不定长） : 用户添加的字符串内容
@OUTPUT：
	pBuf:缓存组好的包
	pLen:包长度
@RETURN: TRUE-成功  FALSE-失败
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint32_t HMI_DatRcd_Insert(uint16_t Screen_id, uint16_t Control_id, uint8_t *pStr, uint8_t *pBuf, uint16_t *pLen);


/****************************************************************
@FUNCTION：数据记录控件-清除记录数据
@INPUT：
	Screen_id(2 个字节)：画面编号
	Control_id(2 个字节)：控件编号
@OUTPUT：
	pBuf:缓存组好的包
	pLen:包长度
@RETURN: TRUE-成功  FALSE-失败
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint32_t HMI_DatRcd_Reset(uint16_t Screen_id, uint16_t Control_id, uint8_t *pBuf, uint16_t *pLen);

#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
