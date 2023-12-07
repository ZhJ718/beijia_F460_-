/**
  ******************************************************************************
  * @file    ble_skylab.h
  * @author  xfw
  * @brief   This file contains definitions for skylab's ble module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BLE_SKYLAB_H
#define __BLE_SKYLAB_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/

#include "config.h"
#include "uart.h"

typedef enum
{
    BLE_TXPWR_4DB = 4,	//4DB
    BLE_TXPWR_0DB = 0,		//0DB
    BLE_TXPWR_N4DB = -4,		//-4DB
    BLE_TXPWR_N8DB = -8,		//-8DB
    BLE_TXPWR_N12DB = -12,	//-12DB
    BLE_TXPWR_N16DB = -16,	//-16DB
    BLE_TXPWR_N20DB	= -20,	//-20DB
    BLE_TXPWR_N30DB = -30,	//-30DB

} ENUM_BLE_TXPWR;


//硬件操作
typedef struct
{
    //蓝牙硬件初始化操作，包含IO口初始化、串口等
    void (*hw_init)(void);
    //蓝牙电源使能
    void (*gpio_power_on)(void);
    //蓝牙电源进制
    void (*gpio_power_off)(void);
    //蓝牙硬件复位引脚拉低
    void (*gpio_reset)(void);
    //蓝牙硬件复位引脚拉高
    void (*gpio_set)(void);

    //蓝牙读操作
    uint32_t (*read)(uint8_t *, uint16_t *);
    //蓝牙写操作
    uint32_t (*write)(uint8_t *, uint16_t );

} BLE_SKYLAB_HW_OPERATIONS;



typedef struct
{
    //
    uint8_t		m_byIsReset: 1;		//TRUE-复位中
    uint8_t		m_byIsCommunicationSucc: 1;
    uint8_t 	m_byRes: 6;

    uint32_t	m_dwNoCommunicationTImeout;

    uint8_t 	m_byResetSetCnt;	//设定的复位时长
    uint8_t 	m_byResetNowCnt;	//当前已完成复位时长

    //实现Ble相关硬件操作，可读写
    BLE_SKYLAB_HW_OPERATIONS m_sHwOps;

} BLE_SKYLAB_LOWLEVEL_MAG;


//AT指令
#define BLE_ATCMD_GETMAC "AT+MAC\r\n"

#define BLE_ATCMD_DEVNAME "AT+DEV_NAME="

#define BLE_ATCMD_TXPWR "AT+TXPWR="

#define BLE_ATCMD_CONNECT_MAC "AT+CON_MAC=00:00:00:00:00:00\r\n"

#define BLE_ATCMD_DISCONNECT "AT+DISCON\r\n"

#define BLE_ATCMD_SCAN	"AT+SCAN_BLE\r\n"



//AT指令响应特征字符串

#define BLE_ATCMD_RSP_CONNECTED "[DA]Connected="
#define BLE_ATCMD_RSP_DISCONNECTED "[DA]Disconnected="
#define BLE_ATCMD_RSP_RSSI "[DA]RSSI="
#define BLE_ATCMD_RSP_ERROR	"[AT]ER\r\n"

/****************************************************************
@FUNCTION：天工测控蓝牙模块MAC地址转换(字符串形式->数字)
@INPUT：MAC地址字符串形式，长度，转化成数字存放位置和大小
@OUTPUT：转化成数字形式大小
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 形如"12:34:56:78:9A:BC"-> 0x12 0x34 0x56 0x78 0x9A 0xBC
****************************************************************/
void Ble_MacCharToHex(uint8_t *a_strMac, uint16_t a_wMacLen, uint8_t *a_aMacHex, uint16_t *a_wMacHexLen);


/****************************************************************
@FUNCTION：天工测控蓝牙模块MAC地址转换(数字->字符串形式)
@INPUT：MAC地址数字形式，长度，转化成字符串存放位置和大小
@OUTPUT：转化成字符串形式大小
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 形如0x12 0x34 0x56 0x78 0x9A 0xBC -> "12:34:56:78:9A:BC"
****************************************************************/
void Ble_MacHexToChar(uint8_t *a_aMacHex, uint16_t MacHexLen, uint8_t *a_strMac, uint16_t *a_wMacLen);


/****************************************************************
@FUNCTION：解析传入字符串是否满足AT指令帧
@INPUT：字符串及长度
@OUTPUT：AT指令帧实际长度，传入字符串是否被截断
@RETURN: TRUE-解析成功 FALSE-解析失败
@AUTHOR：xfw
@SPECIAL: 绑定底层硬件操作函数，初始化，上电，拉住复位引脚
****************************************************************/
uint8_t Ble_FindATFrame(uint8_t *pBuf, uint16_t *pLen, uint32_t *pIsCutted);

/****************************************************************
@FUNCTION：天工测控蓝牙模块底层硬件初始化
@INPUT：硬件接口函数结构体，BLE管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 绑定底层硬件操作函数，初始化，上电，拉住复位引脚
****************************************************************/
extern void Ble_LowLevelInit(BLE_SKYLAB_HW_OPERATIONS a_sHwOps, BLE_SKYLAB_LOWLEVEL_MAG *pMag);


/****************************************************************
@FUNCTION：天工测控蓝牙模块复位
@INPUT：BLE底层管理结构体指针，复位步长（>0）
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 复位时间=Ble_LowLevelTimedProcess调度间隔 * 复位步长
		此函数复位操作立即生效，若复位步长为0，则模块复位一个单位的调度间隔
****************************************************************/
void Ble_Reset(BLE_SKYLAB_LOWLEVEL_MAG *pMag, uint8_t Timer);


/****************************************************************
@FUNCTION：天工测控蓝牙模块复位操作暂停
@INPUT：BLE底层管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 此函数立即生效
****************************************************************/
void Ble_ResetSuspend(BLE_SKYLAB_LOWLEVEL_MAG *pMag);


/****************************************************************
@FUNCTION：天工测控蓝牙模块复位操作恢复
@INPUT：BLE底层管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 若此时蓝牙复位操作已经结束，则无影响
****************************************************************/
void Ble_ResetRecovery(BLE_SKYLAB_LOWLEVEL_MAG *pMag);


/****************************************************************
@FUNCTION：天工测控蓝牙模块底层定时调度进程
@INPUT：BLE底层管理结构体指针
@OUTPUT：无
@RETURN: TRUE-正在执行底层定时调度， FALSE-无底层定时调度，可执行上层应用定时调度
@AUTHOR：xfw
@SPECIAL: 实现模块复位
****************************************************************/
extern uint32_t Ble_LowLevelTimedProcess(BLE_SKYLAB_LOWLEVEL_MAG *pMag);



/****************************************************************
@FUNCTION：天工测控蓝牙模块获取本身MAC
@INPUT：BLE底层管理结构体指针,保留
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 无
****************************************************************/
void Ble_GetMac(BLE_SKYLAB_LOWLEVEL_MAG *pMag, uint8_t *a_sRes1, uint16_t a_wRes2);



/****************************************************************
@FUNCTION：天工测控蓝牙模块设置设备名称
@INPUT：BLE底层管理结构体指针,设备名称,设备名称长度
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:参数校验
****************************************************************/
void Ble_SetDevName(BLE_SKYLAB_LOWLEVEL_MAG *pMag, uint8_t *a_strName, uint16_t NameLen);


/****************************************************************
@FUNCTION：天工测控蓝牙模块设置发射功率
@INPUT：BLE底层管理结构体指针,发射功率
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Ble_SetTxPwr(BLE_SKYLAB_LOWLEVEL_MAG *pMag, ENUM_BLE_TXPWR a_enumTxPwr);

/****************************************************************
@FUNCTION：天工测控蓝牙模块连接指定MAC地址设备
@INPUT：BLE底层管理结构体指针,MAC地址(Hex形式)，MAC地址所占空间
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: MAC地址所占空间需要大于6字节
****************************************************************/
void Ble_ConnectMac(BLE_SKYLAB_LOWLEVEL_MAG *pMag, uint8_t *a_sPeerMacHex, uint16_t a_wPeerMacHexLen);


/****************************************************************
@FUNCTION：天工测控蓝牙模块扫描周边蓝牙设备
@INPUT：BLE底层管理结构体指针,保留
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 无
****************************************************************/
void Ble_Scan(BLE_SKYLAB_LOWLEVEL_MAG *pMag, uint8_t *a_sRes1, uint16_t a_wRes2);


/****************************************************************
@FUNCTION：天工测控蓝牙模块断开连接
@INPUT：BLE底层管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Ble_DisConnect(BLE_SKYLAB_LOWLEVEL_MAG *pMag, uint8_t *a_sRes1, uint16_t a_wRes2);



#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
