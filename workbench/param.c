/**
  ******************************************************************************
  * @file    param.c
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

/* Includes ------------------------------------------------------------------*/
#include "param.h"
#include "iflash.h"
#include "main_task.h"
/****************************************************************
@FUNCTION：数据是否有效
@INPUT：数据
@OUTPUT：无
@RETURN:  TRUE - 有效/ FALSE - 无效
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint32_t IsParamValid(uint8_t *pDat, uint16_t DatLen)
{
    if(pDat == NULL || DatLen == 0)
    {
        return FALSE;
    }

    while(DatLen--)
    {
        if(*pDat++ != 0xFF)
        {
            return TRUE;
        }
    }

    return FALSE;
}


ID_PARAM g_sIDParam;

/****************************************************************
@FUNCTION：终端运行时参数设置默认值
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void IDParam_SetDefaultValue(ID_PARAM *pParam)
{
    if(pParam == NULL)
    {
        return;
    }

    memset((void *)pParam, 0, sizeof(ID_PARAM));

    pParam->m_byRid = 0xFF;
    pParam->m_byTType = 0x30;    //人员
    pParam->m_byTSubType = 0x01;  //单兵装具
    pParam->m_byRid = 0xFF;
    pParam->m_wPid = 0xFFFF;
    pParam->m_aServerIP[0] = 192;
    pParam->m_aServerIP[1] = 168;
    pParam->m_aServerIP[2] = 6;
    pParam->m_aServerIP[3] = 42;
    pParam->m_wServerPort = 4001;
    pParam->m_byIsUDP = 1;

    pParam->m_byWType = (uint8_t)WTYPE_XYZ66X;
    pParam->m_byWTxPwr = 15;
    pParam->m_aWMac[0] = 0xFF;
    pParam->m_aWMac[1] = 0xFF;
    pParam->m_aWMac[2] = 0xFF;
    pParam->m_aWMac[3] = 0xFF;
    pParam->m_aWMac[4] = 0xFF;
    pParam->m_aWMac[5] = 0xFF;
    pParam->m_aWMac[6] = 0xFF;
    pParam->m_aWMac[7] = 0xFF;
}


/****************************************************************
@FUNCTION：终端身份参数初始化
@INPUT：无
@OUTPUT：g_sIDParam结构体赋值
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 若数据取出失败，则清空数据
****************************************************************/
void IDParam_Init(void)
{
    ID_PARAM ParamTmp;

    IFlash_Read(&g_sIFlashMag, FLASH_ID_ADDR, (uint8_t *)&ParamTmp, sizeof(ID_PARAM));

    if(ParamTmp.m_dwChk != GetIFlashCheckSum_U32((uint8_t *)&ParamTmp, sizeof(ID_PARAM) - 4))
    {
        IDParam_SetDefaultValue(&ParamTmp);
        ParamTmp.m_dwChk = GetIFlashCheckSum_U32((uint8_t *)&ParamTmp, sizeof(ID_PARAM) - 4);
        IFlash_Write(&g_sIFlashMag, FLASH_ID_ADDR, (uint8_t *)&ParamTmp, sizeof(ID_PARAM));

    }

    memcpy((void *)&g_sIDParam, (void *)&ParamTmp, sizeof(ID_PARAM) - 4);
}

/****************************************************************
@FUNCTION：终端身份参数保存
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 阻塞较长时间
****************************************************************/
void IDParam_Save(void)
{
    g_sIDParam.m_dwChk = GetIFlashCheckSum_U32((uint8_t *)&g_sIDParam, sizeof(ID_PARAM) - 4);
    IFlash_Write(&g_sIFlashMag, FLASH_ID_ADDR, (uint8_t *)&g_sIDParam, sizeof(ID_PARAM));
}

/****************************************************************
@FUNCTION：获取终端身份参数中的实体类型
@INPUT：无
@OUTPUT：无
@RETURN: 实体类型
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint8_t IDParam_GetTType(void)
{
    return g_sIDParam.m_byTType;
}

/****************************************************************
@FUNCTION：设置终端身份参数中的实体类型
@INPUT：实体类型设置值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
void IDParam_SetTType(uint8_t val)
{
    g_sIDParam.m_byTType = val;
}

/****************************************************************
@FUNCTION：获取终端身份参数中的实体子类型
@INPUT：无
@OUTPUT：无
@RETURN: 实体子类型
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint8_t IDParam_GetTSubType(void)
{
    return g_sIDParam.m_byTSubType;
}

/****************************************************************
@FUNCTION：设置终端身份参数中的实体子类型
@INPUT：实体子类型设置值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
void IDParam_SetTSubType(uint8_t val)
{
    g_sIDParam.m_byTSubType = val;
}

/****************************************************************
@FUNCTION：获取终端身份参数中的设备单位代字
@INPUT：无
@OUTPUT：无
@RETURN: 设备单位代字
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint8_t IDParam_GetRid(void)
{
    return g_sIDParam.m_byRid;
}

/****************************************************************
@FUNCTION：设置终端身份参数中的设备单位代字
@INPUT：设备单位代字 设置值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
void IDParam_SetRid(uint8_t val)
{
    g_sIDParam.m_byRid = val;
}


/****************************************************************
@FUNCTION：获取终端身份参数中的设备编号
@INPUT：无
@OUTPUT：无
@RETURN: 设备编号
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint16_t IDParam_GetPid(void)
{
    return g_sIDParam.m_wPid;
}

/****************************************************************
@FUNCTION：设置终端身份参数中的设备编号
@INPUT：设备编号 设置值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
void IDParam_SetPid(uint16_t val)
{
    g_sIDParam.m_wPid = val;
}



/****************************************************************
@FUNCTION：获取终端身份参数中的入网IP地址
@INPUT：无
@OUTPUT：无
@RETURN: 入网IP地址
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint8_t *IDParam_GetServerIP(void)
{
    return g_sIDParam.m_aServerIP;
}

/****************************************************************
@FUNCTION：设置终端身份参数中的入网IP地址
@INPUT：入网IP地址 设置值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
void IDParam_SetServerIP(uint8_t * pVal, uint16_t ValLen)
{
    if(pVal != NULL && ValLen >= 4)
    {
        memcpy((void *)g_sIDParam.m_aServerIP, pVal, 4);
    }
}

/****************************************************************
@FUNCTION：获取终端身份参数中的入网端口号
@INPUT：无
@OUTPUT：无
@RETURN: 入网端口号
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint16_t IDParam_GetServerPort(void)
{
    return g_sIDParam.m_wServerPort;
}

/****************************************************************
@FUNCTION：设置终端身份参数中的入网端口号
@INPUT：入网端口号设置值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
void IDParam_SetServerPort(uint16_t val)
{
    g_sIDParam.m_wServerPort = val;
}

/****************************************************************
@FUNCTION：获取终端身份参数中的入网协议选择
@INPUT：无
@OUTPUT：无
@RETURN: 入网协议选择
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint8_t IDParam_GetIsUDP(void)
{
    return g_sIDParam.m_byIsUDP;
}

/****************************************************************
@FUNCTION：设置终端身份参数中的入网协议选择
@INPUT：入网协议选择设置值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
void IDParam_SetIsUDP(uint8_t val)
{
    if(val < 2)
    {
        g_sIDParam.m_byIsUDP = val;
    }
}

/****************************************************************
@FUNCTION：获取终端身份参数中的无线模组类型
@INPUT：无
@OUTPUT：无
@RETURN: 无线模组发射功率
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint8_t IDParam_GetWType(void)
{
    return g_sIDParam.m_byWType;
}

/****************************************************************
@FUNCTION：设置终端身份参数中的无线模组类型
@INPUT：无线模组发射功率设置值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
void IDParam_SetWType(uint8_t val)
{
    g_sIDParam.m_byWType = val;
}

/****************************************************************
@FUNCTION：获取终端身份参数中的无线模组发射功率
@INPUT：无
@OUTPUT：无
@RETURN: Zigbee模组发射功率
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint8_t IDParam_GetTxPwr(void)
{
    return g_sIDParam.m_byWTxPwr;
}

/****************************************************************
@FUNCTION：设置终端身份参数中的无线模组发射功率
@INPUT：Zigbee模组发射功率设置值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 下次重启后才能影响硬件
****************************************************************/
void IDParam_SetTxPwr(uint8_t val)
{
    if((val > 0 && val < 16) || (val == 0xFF))
    {
        g_sIDParam.m_byWTxPwr = val;
    }
}

/****************************************************************
@FUNCTION：获取终端身份参数中的无线模组Mac地址
@INPUT：无
@OUTPUT：无
@RETURN: Zigbee模组Mac地址
@AUTHOR：xfw
@SPECIAL: g_sIDParam
****************************************************************/
uint8_t *IDParam_GetWMac(void)
{
    return g_sIDParam.m_aWMac;
}

/****************************************************************
@FUNCTION：设置终端身份参数中的无线模组Mac地址
@INPUT：Zigbee模组Mac地址 设置值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: pVal为空时，填入无效MAC
****************************************************************/
void IDParam_SetWMac(uint8_t * pVal, uint16_t ValLen)
{
    uint8_t InvalidAddr[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //{0x30, 0x01, 0x06, 0x00, 0x01, 0x00, 0x00, 0x00};

    if(pVal != NULL && ValLen >= 8)
    {
        memcpy((void *)g_sIDParam.m_aWMac, pVal, 8);
    }
    else
    {
        memcpy((void *)g_sIDParam.m_aWMac, InvalidAddr, sizeof(InvalidAddr));
    }
}



/****************************************************************
@FUNCTION：终端身份参数中无线模组检验地址有效性
@INPUT：地址
@OUTPUT：无
@RETURN:  TRUE - 有效/ FALSE - 无效
@AUTHOR：xfw
@SPECIAL:
***************************************************************
uint32_t IDParam_CheckWMacValid(uint8_t *pAddr, uint8_t AddrLen)
{
    uint8_t InvalidAddr[]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

    if(pAddr == NULL || AddrLen != 8)
    {
        return FALSE;
    }

    if(0 == memcmp(pAddr, InvalidAddr, AddrLen))
    {
        return FALSE;
    }

    return TRUE;

}
*/

/****************************************************************
@FUNCTION：终端身份参数判断是否为自身身份信息
@INPUT：无
@OUTPUT: 无
@RETURN: TRUE - 有效/ FALSE - 无效
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint32_t IDParam_IsIDValid(uint8_t TType, uint8_t TSubType, uint8_t Rid, uint16_t Pid)
{
    if(TType != 0xFF && TType != g_sIDParam.m_byTType)
    {
        return FALSE;
    }

    if(TSubType != 0xFF && TType != g_sIDParam.m_byTSubType)
    {
        return FALSE;
    }

    if(Rid != 0xFF && TType != g_sIDParam.m_byRid)
    {
        return FALSE;
    }

    if(Pid != 0xFFFF && TType != g_sIDParam.m_wPid)
    {
        return FALSE;
    }

    return TRUE;
}




SYS_PARAM g_sSysParam;

#ifdef _APP
/****************************************************************
@FUNCTION：系统参数设置默认值
@INPUT：pParam - 指针，若为NULL,指向g_sSysParam
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void SysParam_SetDefaultValue(SYS_PARAM *pParam)
{
    if(pParam == NULL)
    {
        for(uint8_t i = 0; i < SYS_SLAVER_CNT_MAX; i++)
				{
					memset((void *)&g_sUserParam.m_saSingleMag[i], 0xFF, sizeof(SINGLE_SLAVER_MAG));
					memset((void *)&g_sSysParam.m_sSingleParam[i], 0xFF, sizeof(SINGLE_SLAVER_PARAM));
				}
    }
    else
    {
        memset((void *)pParam, 0xFF, sizeof(SYS_PARAM));
    }

}


/****************************************************************
@FUNCTION：系统参数初始化
@INPUT：无
@OUTPUT：g_sSysParam 结构体赋值
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 若数据取出失败，则清空数据
****************************************************************/
void SysParam_Init(void)
{
    SYS_PARAM ParamTmp;

    IFlash_Read(&g_sIFlashMag, FLASH_SYS_ADDR, (uint8_t *)&ParamTmp, sizeof(SYS_PARAM));

    if(ParamTmp.m_dwChk != GetIFlashCheckSum_U32((uint8_t *)&ParamTmp, sizeof(SYS_PARAM) - 4))
    {
        SysParam_SetDefaultValue(&ParamTmp);
        ParamTmp.m_dwChk = GetIFlashCheckSum_U32((uint8_t *)&ParamTmp, sizeof(SYS_PARAM) - 4);
        IFlash_Write(&g_sIFlashMag, FLASH_SYS_ADDR, (uint8_t *)&ParamTmp, sizeof(SYS_PARAM));
    }

    memcpy((void *)&g_sSysParam, (void *)&ParamTmp, sizeof(SYS_PARAM) - 4);
}
#endif
/****************************************************************
@FUNCTION：系统参数参数保存
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 阻塞较长时间
****************************************************************/
void SysParam_Save(void)
{
    g_sSysParam.m_dwChk = GetIFlashCheckSum_U32((uint8_t *)&g_sSysParam, sizeof(SYS_PARAM) - 4);
    IFlash_Write(&g_sIFlashMag, FLASH_SYS_ADDR, (uint8_t *)&g_sSysParam, sizeof(SYS_PARAM));
}

/****************************************************************
@FUNCTION：获取系统参数中的子设备参数
@INPUT：无
@OUTPUT：无
@RETURN: Zigbee模组Mac地址
@AUTHOR：xfw
@SPECIAL: g_sSysParam
****************************************************************/
SINGLE_SLAVER_PARAM *SysParam_GetSlaverParam(uint8_t idx)
{
    if(idx < SYS_SLAVER_CNT_MAX)
    {
        return &g_sSysParam.m_sSingleParam[idx];
    }

    return NULL;
}

/****************************************************************
@FUNCTION：设置系统参数中的子设备参数
@INPUT：子设备参数，子设备序号
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sSysParam
****************************************************************/
void SysParam_SetSlaverParam(SINGLE_SLAVER_PARAM * pVal, uint8_t idx)
{
    if(pVal != NULL && idx < SYS_SLAVER_CNT_MAX)
    {
        memcpy((void *)&g_sSysParam.m_sSingleParam[idx], pVal, sizeof(SINGLE_SLAVER_PARAM));
    }
}


/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
