/**
  ******************************************************************************
  * @file    param.h
  * @author  xfw
  * @brief   This file contains definitions for sys param
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
#ifndef __PARAM_H
#define __PARAM_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "config.h"

/****************************************************************
@FUNCTION：数据是否有效
@INPUT：数据
@OUTPUT：无
@RETURN:  TRUE - 有效/ FALSE - 无效
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint32_t IsParamValid(uint8_t *pDat, uint16_t DatLen);


// 定义终端身份参数,保持四字节对齐
typedef struct
{
    uint8_t m_byTType;          //设备实体类型
    uint8_t m_byTSubType;   //设备实体子类型
    uint8_t m_byRid;	        //设备单位代字  4
    uint16_t m_wPid;            //设备编号

    uint8_t m_aServerIP[4] ;    //入网IP地址（大端，形如 C0 A8 01 01,代表192.168.1.1)
    uint16_t m_wServerPort;   //入网端口号（大端，形如 1F 90，代表8080，注意0xFFFF为不修改）12
    uint8_t m_byIsUDP;          //入网协议（0-TCP，1-UDP）
    uint8_t m_byWType;  //无线类型,见E_WIRELESS_TYPE
    uint8_t m_byWTxPwr;       // 无线模组发射功率(1-15 ，注意0xFF为不修改)
    uint8_t m_aWMac[8];   // 无线模组Mac地址(注意0xFFFFFFFFFFFFFFFF为不修改)
    uint8_t m_byRes1;   //保留字节1，对齐用
    uint8_t m_byRes2;   //保留字节2，对齐用24

    uint32_t	m_dwChk;    //校验
} ID_PARAM;

extern ID_PARAM g_sIDParam;

/****************************************************************
@FUNCTION：终端运行时参数设置默认值
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void IDParam_SetDefaultValue(ID_PARAM *pParam);

/****************************************************************
@FUNCTION：终端身份参数初始化
@INPUT：无
@OUTPUT：g_sIDParam结构体赋值
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 若数据取出失败，则清空数据
****************************************************************/
void IDParam_Init(void);

/****************************************************************
@FUNCTION：终端身份参数保存
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 阻塞较长时间
****************************************************************/
void IDParam_Save(void);


/****************************************************************
@FUNCTION：获取终端身份参数中的实体类型
@INPUT：无
@OUTPUT：无
@RETURN: 实体类型
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint8_t IDParam_GetTType(void);

/****************************************************************
@FUNCTION：设置终端身份参数中的实体类型
@INPUT：实体类型设置值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
void IDParam_SetTType(uint8_t val);


/****************************************************************
@FUNCTION：获取终端身份参数中的实体子类型
@INPUT：无
@OUTPUT：无
@RETURN: 实体子类型
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint8_t IDParam_GetTSubType(void);


/****************************************************************
@FUNCTION：设置终端身份参数中的实体子类型
@INPUT：实体子类型设置值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
void IDParam_SetTSubType(uint8_t val);


/****************************************************************
@FUNCTION：获取终端身份参数中的设备单位代字
@INPUT：无
@OUTPUT：无
@RETURN: 设备单位代字
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint8_t IDParam_GetRid(void);


/****************************************************************
@FUNCTION：设置终端身份参数中的设备单位代字
@INPUT：设备单位代字 设置值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
void IDParam_SetRid(uint8_t val);


/****************************************************************
@FUNCTION：获取终端身份参数中的设备编号
@INPUT：无
@OUTPUT：无
@RETURN: 设备编号
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint16_t IDParam_GetPid(void);


/****************************************************************
@FUNCTION：设置终端身份参数中的设备编号
@INPUT：设备编号 设置值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
void IDParam_SetPid(uint16_t val);


/****************************************************************
@FUNCTION：获取终端身份参数中的入网IP地址
@INPUT：无
@OUTPUT：无
@RETURN: 入网IP地址
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint8_t *IDParam_GetServerIP(void);


/****************************************************************
@FUNCTION：设置终端身份参数中的入网IP地址
@INPUT：入网IP地址 设置值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
void IDParam_SetServerIP(uint8_t * pVal, uint16_t ValLen);


/****************************************************************
@FUNCTION：获取终端身份参数中的入网端口号
@INPUT：无
@OUTPUT：无
@RETURN: 入网端口号
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint16_t IDParam_GetServerPort(void);

/****************************************************************
@FUNCTION：设置终端身份参数中的入网端口号
@INPUT：入网端口号设置值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
void IDParam_SetServerPort(uint16_t val);

/****************************************************************
@FUNCTION：获取终端身份参数中的入网协议选择
@INPUT：无
@OUTPUT：无
@RETURN: 入网协议选择
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint8_t IDParam_GetIsUDP(void);


/****************************************************************
@FUNCTION：设置终端身份参数中的入网协议选择
@INPUT：入网协议选择设置值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
void IDParam_SetIsUDP(uint8_t val);

/****************************************************************
@FUNCTION：获取终端身份参数中的无线模组类型
@INPUT：无
@OUTPUT：无
@RETURN: 无线模组发射功率
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint8_t IDParam_GetWType(void);

/****************************************************************
@FUNCTION：设置终端身份参数中的无线模组类型
@INPUT：无线模组发射功率设置值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
void IDParam_SetWType(uint8_t val);

/****************************************************************
@FUNCTION：获取终端身份参数中的无线模组发射功率
@INPUT：无
@OUTPUT：无
@RETURN: Zigbee模组发射功率
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint8_t IDParam_GetTxPwr(void);

/****************************************************************
@FUNCTION：设置终端身份参数中的无线模组发射功率
@INPUT：Zigbee模组发射功率设置值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 下次重启后才能影响硬件
****************************************************************/
void IDParam_SetTxPwr(uint8_t val);

/****************************************************************
@FUNCTION：获取终端身份参数中的无线模组Mac地址
@INPUT：无
@OUTPUT：无
@RETURN: Zigbee模组Mac地址
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint8_t *IDParam_GetWMac(void);


/****************************************************************
@FUNCTION：设置终端身份参数中的无线模组Mac地址
@INPUT：Zigbee模组Mac地址 设置值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: pVal为空时，填入无效MAC
****************************************************************/
void IDParam_SetWMac(uint8_t * pVal, uint16_t ValLen);


/****************************************************************
@FUNCTION：终端身份参数判断是否为自身身份信息
@INPUT：无
@OUTPUT: 无
@RETURN: TRUE - 有效/ FALSE - 无效
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint32_t IDParam_IsIDValid(uint8_t TType, uint8_t TSubType, uint8_t Rid, uint16_t Pid);


typedef struct
{
    uint8_t m_byTType;          //子设备实体类型
    uint8_t m_byTSubType;   //子设备实体子类型（0xFF-不关心，适用于配置发射机的时候）
    uint8_t m_byWType;	        //子设备无线类型 (0x0 -有线，0x01 - 蓝牙，0x02 - 新标准zigbee，0x03 - lora设备)
    uint8_t m_aWMac[8];            //子设备MAC地址（高位在前）11

		uint8_t m_byWeaponIdx;
		uint8_t m_byIdx;
    uint8_t m_byRes1;   //保留字节1，对齐用
    uint8_t m_byRes2;   //保留字节2，对齐用
    uint8_t m_byRes3;   //保留字节3，对齐用
} SINGLE_SLAVER_PARAM;


#define SYS_SLAVER_CNT_MAX 10

// 定义系统参数,保持四字节对齐
typedef struct
{
    // 终端子设备参数
    SINGLE_SLAVER_PARAM  m_sSingleParam[SYS_SLAVER_CNT_MAX];// 16*10 = 160

    uint32_t	m_dwChk;    //校验
} SYS_PARAM;


extern SYS_PARAM g_sSysParam;

/****************************************************************
@FUNCTION：系统参数设置默认值
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void SysParam_SetDefaultValue(SYS_PARAM *pParam);


/****************************************************************
@FUNCTION：系统参数初始化
@INPUT：无
@OUTPUT：g_sSysParam 结构体赋值
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 若数据取出失败，则清空数据
****************************************************************/
void SysParam_Init(void);


/****************************************************************
@FUNCTION：系统参数参数保存
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 阻塞较长时间
****************************************************************/
void SysParam_Save(void);


/****************************************************************
@FUNCTION：获取系统参数中的子设备参数
@INPUT：无
@OUTPUT：无
@RETURN: Zigbee模组Mac地址
@AUTHOR：xfw
@SPECIAL: g_sSysParam
****************************************************************/
SINGLE_SLAVER_PARAM *SysParam_GetSlaverParam(uint8_t idx);

/****************************************************************
@FUNCTION：设置系统参数中的子设备参数
@INPUT：子设备参数，子设备序号
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sSysParam
****************************************************************/
void SysParam_SetSlaverParam(SINGLE_SLAVER_PARAM * pVal, uint8_t idx);


#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
