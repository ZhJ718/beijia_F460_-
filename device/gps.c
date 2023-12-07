/**
  ******************************************************************************
  * @file    gps.c
  * @author  xfw
  * @brief   This file contains definitions for gps module
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

/* Includes ------------------------------------------------------------------*/
#include "gps.h"

/*
 ************************************************************************************************************************************************************************
 *函数名称:	HexToAscii
 *功能描述:	二进制转换为ASCII
 *输入描述:	原始数据，转换缓冲区，原始数据长度，字母是否大写
 *输出描述:	转换数据
 *返回描述:
 *作者日期:
 *全局声明:
 *特殊说明: 转换缓冲区长度必须大于等于原始数据长度的2倍，转换后顺序为高位在前
 ************************************************************************************************************************************************************************
 */
static void HexToAscii(const uint8_t *oBuf, uint8_t *dBuf, const uint32_t oLen, const uint32_t IsCapital)
{
    uint32_t		Count;
    uint8_t		Add;

    if (IsCapital)
        Add		= 0;
    else
        Add		= 0x20;

    for (Count = 0; Count < oLen; Count ++)
    {
        if (((*oBuf >> 4) & 0xF) > 9)
            *dBuf		= Add + 0x41 + ((*oBuf >> 4) & 0xF) - 10;
        else
            *dBuf		= 0x30 + ((*oBuf >> 4) & 0xF);

        dBuf			++;

        if ((*oBuf & 0xF) > 9)
            *dBuf		= Add + 0x41 + (*oBuf & 0xF) - 10;
        else
            *dBuf		= 0x30 + (*oBuf & 0xF);

        oBuf			++;
        dBuf			++;
    }
}

//获取校验结果
uint32_t NMEA_GetCheck(uint8_t *pBuf, uint8_t Len, uint8_t *Chk)
{
    uint8_t i, chk;

    if((pBuf != NULL) && (pBuf[0] == '$'))
    {
        for(i = 0, chk = 0; i < (Len - 1); i++)
        {
            chk ^= pBuf[i + 1];
        }

        if(Chk != NULL)
        {
            *Chk = chk;
        }

        return TRUE;
    }

    return FALSE;
}


uint8_t NMEA_Comma_Pos(uint8_t *buf, uint8_t cx)
{
    uint8_t *p = buf;

    while(cx)
    {
        if(*buf == '*' || *buf < ' ' || *buf > 'z')return 0XFF; //判断无效字符

        if(*buf == ',')cx--;

        buf++;
    }

    return buf - p;
}

uint32_t NMEA_Pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while(n--)result *= m;

    return result;
}

int NMEA_Str2num(uint8_t *buf, uint8_t*dx)
{
    uint8_t *p = buf;
    uint32_t ires = 0, fres = 0;
    uint8_t ilen = 0, flen = 0, i;
    uint8_t mask = 0;
    int res;

    while(1) //得到整数和小数的长度
    {
        if(*p == '-')
        {
            mask |= 0X02;    //是负数
            p++;
        }

        if(*p == ',' || (*p == '*'))break; //遇到结束了

        if(*p == '.')
        {
            mask |= 0X01;    //遇到小数点了
            p++;
        }
        else if(*p > '9' || (*p < '0'))	//有非法字符
        {
            ilen = 0;
            flen = 0;
            break;
        }

        if(mask & 0X01)flen++;
        else ilen++;

        p++;
    }

    if(mask & 0X02)buf++;	//去掉负号

    for(i = 0; i < ilen; i++)	//得到整数部分数据
    {
        ires += NMEA_Pow(10, ilen - 1 - i) * (buf[i] - '0');
    }

    if(flen > 5)flen = 5;	//最多取5位小数

    *dx = flen;	 		//小数点位数

    for(i = 0; i < flen; i++)	//得到小数部分数据
    {
        fres += NMEA_Pow(10, flen - 1 - i) * (buf[ilen + 1 + i] - '0');
    }

    res = ires * NMEA_Pow(10, flen) + fres;

    if(mask & 0X02)res = -res;

    return res;
}


static GPS_LOWLEVEL_MAG GpsLLMag;


GPS_LOWLEVEL_MAG *Gps_GetLLMagHandle(void)
{
    return &GpsLLMag;
}


//模块底层操作供电
void Gps_LL_PowerOn(GPS_LOWLEVEL_MAG *pMag)
{
    assert_param(pMag != NULL);

    if(pMag->m_sHwOps.gpio_power_on != NULL)
    {
        pMag->m_sHwOps.gpio_power_on();
    }
}

//模块底层操作断电
void Gps_LL_PowerOff(GPS_LOWLEVEL_MAG *pMag)
{
    assert_param(pMag != NULL);

    if(pMag->m_sHwOps.gpio_power_off != NULL)
    {
        pMag->m_sHwOps.gpio_power_off();
    }
}

//模块底层操作复位引脚
void Gps_LL_Reset(GPS_LOWLEVEL_MAG *pMag)
{
    assert_param(pMag != NULL);

    if(pMag->m_sHwOps.gpio_reset != NULL)
    {
        pMag->m_sHwOps.gpio_reset();
    }
}

//模块底层置位复位引脚
void Gps_LL_Set(GPS_LOWLEVEL_MAG *pMag)
{
    assert_param(pMag != NULL);

    if(pMag->m_sHwOps.gpio_set != NULL)
    {
        pMag->m_sHwOps.gpio_set();
    }
}

/****************************************************************
@FUNCTION：模块设置NMEA语句输出频次,要求输出或停止输出的 NMEA 语句
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 语句输出频率(ENUM_NMEA_OUTPUT_FREQ)是以定位更新率为基准的
	//	n（0~9）表示每 n 次定位输出一次，0 表示 不输出该语句，0xff则保持原有配置
	//格式：$PCAS03,nGGA,nGLL,nGSA,nGSV,nRMC,nVTG,nZDA,nTXT*CS<CR><LF>
	//示例：$PCAS03,1,1,1,1,1,1,0,1*03
****************************************************************/
void Gps_LL_SetNMEAOutputCtrl(GPS_LOWLEVEL_MAG *pMag)
{
    uint8_t Chk, Buf[50];
    uint16_t i, Len;
    NMEA_OUTPUT_FREQ_PARAM *pParam = NULL;

    assert_param(pMag != NULL);

    pParam = &pMag->m_sParam;

    Len = 0;
    Buf[Len++] = '$';
    Buf[Len++] = 'P';
    Buf[Len++] = 'C';
    Buf[Len++] = 'A';
    Buf[Len++] = 'S';
    Buf[Len++] = '0';
    Buf[Len++] = '3';
    Buf[Len++] = ',';

    if(pParam->m_eNGGA != OUTPUT_UNCHANGE)
    {
        Buf[Len++] = pParam->m_eNGGA + 0x30;
    }

    Buf[Len++] = ',';

    if(pParam->m_eNGLL != OUTPUT_UNCHANGE)
    {
        Buf[Len++] = pParam->m_eNGLL + 0x30;
    }

    Buf[Len++] = ',';

    if(pParam->m_eNGSA != OUTPUT_UNCHANGE)
    {
        Buf[Len++] = pParam->m_eNGSA + 0x30;
    }

    Buf[Len++] = ',';

    if(pParam->m_eNGSV != OUTPUT_UNCHANGE)
    {
        Buf[Len++] = pParam->m_eNGSV + 0x30;
    }

    Buf[Len++] = ',';

    if(pParam->m_eNRMC != OUTPUT_UNCHANGE)
    {
        Buf[Len++] = pParam->m_eNRMC + 0x30;
    }

    Buf[Len++] = ',';

    if(pParam->m_eNVTG != OUTPUT_UNCHANGE)
    {
        Buf[Len++] = pParam->m_eNVTG + 0x30;
    }

    Buf[Len++] = ',';

    if(pParam->m_eNZDA != OUTPUT_UNCHANGE)
    {
        Buf[Len++] = pParam->m_eNZDA + 0x30;
    }

    Buf[Len++] = ',';

    if(pParam->m_eNTXT != OUTPUT_UNCHANGE)
    {
        Buf[Len++] = pParam->m_eNTXT + 0x30;
    }

    //Temp[TempLen++] = '*';

    if(TRUE == NMEA_GetCheck(Buf, Len, &Chk))
    {
        Buf[Len++] = '*';
        HexToAscii(&Chk, Buf + Len, 1, TRUE);
        Len += 2;
    }

    Buf[Len++] = '\r';//0D

    Buf[Len++] = '\n'; //0A

    if(pMag->m_sHwOps.write != NULL)
    {
        pMag->m_sHwOps.write(Buf, Len);
    }
}





/****************************************************************
@FUNCTION：中科微332D GPS初始化
@INPUT：硬件接口函数结构体，GPS管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 绑定底层硬件操作函数，初始化，拉高复位引脚，控制各语句输出频率
****************************************************************/
extern void Gps_LowLevelInit(COM_HW_OPERATIONS a_sHwOps)
{
    GPS_LOWLEVEL_MAG *pMag = Gps_GetLLMagHandle();

    DDL_ASSERT(pMag != NULL);


    pMag->m_sHwOps.hw_init = a_sHwOps.hw_init;
    pMag->m_sHwOps.gpio_reset = a_sHwOps.gpio_reset;
    pMag->m_sHwOps.gpio_set = a_sHwOps.gpio_set;

    pMag->m_sHwOps.read = a_sHwOps.read;
    pMag->m_sHwOps.write = a_sHwOps.write;

    //IO初始化
    if(pMag->m_sHwOps.hw_init != NULL)
    {
        pMag->m_sHwOps.hw_init();
    }

    //复位脚拉低
    Gps_LL_Reset(pMag);

    //默认只输出RMC语句
    pMag->m_sParam.m_eNGGA = OUTPUT_1;
    pMag->m_sParam.m_eNGLL = OUTPUT_NO;
    pMag->m_sParam.m_eNGSA = OUTPUT_NO;
    pMag->m_sParam.m_eNGSV = OUTPUT_NO;
    pMag->m_sParam.m_eNRMC = OUTPUT_1;
    pMag->m_sParam.m_eNVTG = OUTPUT_NO;
    pMag->m_sParam.m_eNZDA = OUTPUT_NO;
    pMag->m_sParam.m_eNTXT = OUTPUT_NO;

    Gps_LL_SetNMEAOutputCtrl(pMag);

}



/****************************************************************
@FUNCTION：模块复位
@INPUT：复位步长（>0）
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 复位时间=定时任务调度间隔 * 复位步长
		此函数复位操作立即生效，若复位步长为0，则模块复位一个单位的调度间隔
****************************************************************/
void Gps_Reset(uint8_t Timer)
{
    GPS_LOWLEVEL_MAG *pMag = Gps_GetLLMagHandle();

    assert_param(pMag != NULL);

    pMag->m_byResetSetCnt = Timer;
    pMag->m_byResetNowCnt = 0;
    pMag->m_byIsReset = TRUE;

    Gps_LL_Reset(pMag);
}


/****************************************************************
@FUNCTION：模块复位操作暂停
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 此函数立即生效
****************************************************************/
void Gps_ResetSuspend(void)
{
    GPS_LOWLEVEL_MAG *pMag = Gps_GetLLMagHandle();

    assert_param(pMag != NULL);

    pMag->m_byIsReset = FALSE;
    Gps_LL_Set(pMag);
}

/****************************************************************
@FUNCTION：模块复位操作恢复
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 若此时复位操作已经结束，则无影响
****************************************************************/
void Gps_ResetRecovery(void)
{
    GPS_LOWLEVEL_MAG *pMag = Gps_GetLLMagHandle();

    assert_param(pMag != NULL);

    if((pMag->m_byIsReset == FALSE) && (pMag->m_byResetSetCnt > pMag->m_byResetNowCnt))
    {
        Gps_Reset(pMag->m_byResetSetCnt - pMag->m_byResetNowCnt);
    }

}

/****************************************************************
@FUNCTION：模块开始设置参数
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 调用后异步执行，不用等回复
****************************************************************/
void Gps_StartSetParam(NMEA_OUTPUT_FREQ_PARAM SetParam)
{
    GPS_LOWLEVEL_MAG *pMag = Gps_GetLLMagHandle();

    assert_param(pMag != NULL);

    if(SetParam.m_eNGGA != OUTPUT_UNCHANGE)
    {
        pMag->m_sParam.m_eNGGA = SetParam.m_eNGGA;
    }

    if(SetParam.m_eNGLL != OUTPUT_UNCHANGE)
    {
        pMag->m_sParam.m_eNGLL = SetParam.m_eNGLL;
    }

    if(SetParam.m_eNGSA != OUTPUT_UNCHANGE)
    {
        pMag->m_sParam.m_eNGSA = SetParam.m_eNGSA;
    }

    if(SetParam.m_eNGSV != OUTPUT_UNCHANGE)
    {
        pMag->m_sParam.m_eNGSV = SetParam.m_eNGSV;
    }

    if(SetParam.m_eNRMC != OUTPUT_UNCHANGE)
    {
        pMag->m_sParam.m_eNRMC = SetParam.m_eNRMC;
    }

    if(SetParam.m_eNVTG != OUTPUT_UNCHANGE)
    {
        pMag->m_sParam.m_eNVTG = SetParam.m_eNVTG;
    }

    if(SetParam.m_eNZDA != OUTPUT_UNCHANGE)
    {
        pMag->m_sParam.m_eNZDA = SetParam.m_eNZDA;
    }

    if(SetParam.m_eNTXT != OUTPUT_UNCHANGE)
    {
        pMag->m_sParam.m_eNTXT = SetParam.m_eNTXT;
    }

    pMag->m_bySetParamNowCnt = 0;
    pMag->m_byIsSetParam = TRUE;

}



/****************************************************************
@FUNCTION：模块底层定时调度进程
@INPUT：
@OUTPUT：无
@RETURN: TRUE-正在执行底层定时调度,  FALSE-无底层定时调度, 可执行上层应用定时调度
@AUTHOR：xfw
@SPECIAL: 实现模块复位
****************************************************************/
extern uint32_t Gps_LowLevelTimedProcess(void)
{
    GPS_LOWLEVEL_MAG *pMag = Gps_GetLLMagHandle();

    assert_param(pMag != NULL);


    if(pMag->m_byIsReset == TRUE)
    {
        if(pMag->m_byResetNowCnt == pMag->m_byResetSetCnt)
        {
            pMag->m_sHwOps.gpio_set();
            pMag->m_byResetNowCnt++;
        }
        else if(pMag->m_byResetNowCnt < (pMag->m_byResetSetCnt + 1))
        {
            pMag->m_byResetNowCnt++;	//这边的+1 给模块一个初始化时间,否则复位一完成就发指令，会不认
        }
        else
        {
            pMag->m_byIsReset = FALSE;
        }

        return TRUE;
    }

    else if(pMag->m_byIsSetParam == TRUE)
    {
        if(pMag->m_bySetParamNowCnt++ % 20 == 10)
        {
            Gps_LL_SetNMEAOutputCtrl(pMag);
            pMag->m_byIsSetParam = FALSE;
        }

        return TRUE;
    }

    return FALSE;
}




/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
