/**
  ******************************************************************************
  * @file    nbee.c
  * @author  xfw
  * @brief   This file contains definitions for newbit zigbee
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
#include "nbee.h"


/****************************************************************
@FUNCTION：NBEE系列Zigbee模块底层硬件初始化
@INPUT：硬件接口函数结构体，BLE管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 绑定底层硬件操作函数，初始化，拉住复位引脚
****************************************************************/
extern void NBee_LowLevelInit(NBEE_HW_OPERATIONS a_sHwOps, NBEE_LOWLEVEL_MAG *pMag)
{
    assert_param(pMag != NULL);

    memset(pMag, 0, sizeof(NBEE_LOWLEVEL_MAG));

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

    //复位
    if(pMag->m_sHwOps.gpio_reset != NULL)
    {
        pMag->m_sHwOps.gpio_reset();
    }
}


/****************************************************************
@FUNCTION：NBEE系列Zigbee蓝牙模块复位
@INPUT：NBEE底层管理结构体指针，复位步长（>0）
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 复位时间=NBee_LowLevelTimedProcess调度间隔 * 复位步长
		此函数复位操作立即生效，若复位步长为0，则模块复位一个单位的调度间隔
****************************************************************/
void NBee_Reset(NBEE_LOWLEVEL_MAG *pMag, uint8_t Timer)
{
    pMag->m_sHwOps.gpio_reset();
    pMag->m_byResetSetCnt = Timer;
    pMag->m_byResetNowCnt = 0;
    pMag->m_byIsReset = TRUE;
}

/****************************************************************
@FUNCTION：NBEE系列Zigbee模块复位操作暂停
@INPUT：NBEE底层管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 此函数立即生效
****************************************************************/
void NBee_ResetSuspend(NBEE_LOWLEVEL_MAG *pMag)
{
    pMag->m_sHwOps.gpio_set();
    pMag->m_byIsReset = FALSE;
}

/****************************************************************
@FUNCTION：NBEE系列Zigbee模块复位操作恢复
@INPUT：NBEE底层管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 若此时蓝牙复位操作已经结束，则无影响
****************************************************************/
void NBee_ResetRecovery(NBEE_LOWLEVEL_MAG *pMag)
{
    if((pMag->m_byIsReset == FALSE) && (pMag->m_byResetSetCnt > pMag->m_byResetNowCnt))
    {
        NBee_Reset(pMag, pMag->m_byResetSetCnt - pMag->m_byResetNowCnt);
    }

}

/****************************************************************
@FUNCTION：NBEE系列Zigbee模块底层定时调度进程
@INPUT：NBEE底层管理结构体指针
@OUTPUT：无
@RETURN: TRUE-正在执行底层定时调度， FALSE-无底层定时调度，可执行上层应用定时调度
@AUTHOR：xfw
@SPECIAL: 实现模块复位
****************************************************************/
extern uint32_t NBee_LowLevelTimedProcess(NBEE_LOWLEVEL_MAG *pMag)
{
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

    return FALSE;
}

























/****************************************************************
@FUNCTION：Zigbee模块组帧
@INPUT：帧结构体pFrame，缓冲区pBuf和缓冲区大小pLen
@OUTPUT：输出实际帧长度pLen
@RETURN: NBEE_RET_SUCC-成功 NBEE_RET_PARAM_ERROR-传入参数错误
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
u32 NBee_Frame_Make(NB_ApiFrameStruct_t *pFrame, u8 *pBuf, u16 *pLen)
{
    u32 len;
    u8 chk;

    if((pFrame == NULL) || (pBuf == NULL) || (pLen == NULL))
        return NBEE_RET_PARAM_ERROR;

    //传入数据缓冲区较小
    if(*pLen < (pFrame->u16Length + 4))
        return NBEE_RET_PARAM_ERROR;

    //计算累加和
    for(len = 0, chk = 0; len < pFrame->u16Length; len++)
    {
        chk += pFrame->u8ApiData[len];
    }

    len = 0;
    pBuf[len++] = NBEE_FRAME_HEADER;
    pBuf[len++] = pFrame->u16Length >> 8;
    pBuf[len++] = pFrame->u16Length % 0x100;
    memcpy(pBuf + len, pFrame->u8ApiData, pFrame->u16Length);
    len += pFrame->u16Length;
    pBuf[len++] = 0xff - chk;

    *pLen = len;


    return NBEE_RET_SUCC;
}

//判断缓冲区内容是否符合NBee协议要求
u32 NBee_FindFrame(u8 *pBuf, u16 *pLen, u32 *pIsCutted)
{
    u32 i, len;
    u8 chk;

    assert_param((pBuf == NULL) || (pLen == NULL));

    if(pBuf[0] == NBEE_FRAME_HEADER)
    {
        //数据长度太短
        if(*pLen < 6)
        {
            if(pIsCutted != NULL)
            {
                *pIsCutted = TRUE;
            }

            return	TRUE;
        }

        len = (pBuf[1] << 8) | pBuf[2];

        if(len > 0x60)	//
            return FALSE;

        if(*pLen < (len + 4))
        {
            if(pIsCutted != NULL)
            {
                *pIsCutted = TRUE;
            }

            return	TRUE;
        }

        //计算累加和
        for(i = 0, chk = 0; i < len; i++)
        {
            chk += pBuf[3 + i];
        }

        if(chk == (0xff - pBuf[3 + len]))
        {
            *pLen = 4 + len;

            if(pIsCutted != NULL)
            {
                *pIsCutted = FALSE;
            }

            return TRUE;
        }
    }

    if (pIsCutted != NULL)
    {
        *pIsCutted	= FALSE;
    }

    return FALSE;
}

//解析帧,需要传入帧结构体，报文和长度
u32 NBee_Frame_Parse(NB_ApiFrameStruct_t *pFrame, u8 *pBuf, u16 Len)
{

    if((pFrame == NULL) || (pBuf == NULL))
        return NBEE_RET_PARAM_ERROR;

    pFrame->u16Length = (pBuf[1] << 8) | pBuf[2];

    memcpy(pFrame->u8ApiData, pBuf + 3, pFrame->u16Length);


    return NBEE_RET_SUCC;
}

//
u32 NBee_Frame_Make_AllType(NB_ApiFrameStruct_ApiData_t *pApiData, u8 *pBuf, u16 *pLen)
{
    NB_ApiFrameStruct_t ApiFrame;

    if((pApiData == NULL) || (pBuf == NULL) || (pLen == NULL))
        return NBEE_RET_PARAM_ERROR;

    memset((void *)&ApiFrame, 0, sizeof(NB_ApiFrameStruct_t));

    switch(pApiData->u8FrameType)
    {
        case NBEE_FRAME_TYPE_ATCMD:
            ApiFrame.u8ApiData[0] = pApiData->u8FrameType;
            ApiFrame.u8ApiData[1] = pApiData->Union.sATCMD.u8FrameID;
            ApiFrame.u8ApiData[2] = pApiData->Union.sATCMD.u8CMD1;
            ApiFrame.u8ApiData[3] = pApiData->Union.sATCMD.u8CMD2;
            memcpy(&ApiFrame.u8ApiData[4], pApiData->Union.sATCMD.u8Para, pApiData->Union.sATCMD.u8ParaLen);
            ApiFrame.u16Length = pApiData->Union.sATCMD.u8ParaLen + 4;
            break;

        case NBEE_FRAME_TYPE_ZBTREQ:
            ApiFrame.u8ApiData[0] = pApiData->u8FrameType;
            ApiFrame.u8ApiData[1] = pApiData->Union.sZBTREQ.u8FrameID;
            memcpy(&ApiFrame.u8ApiData[2], pApiData->Union.sZBTREQ.u8ExPanid, 8);
            memcpy(&ApiFrame.u8ApiData[10], pApiData->Union.sZBTREQ.u8Panid, 2);
            ApiFrame.u8ApiData[12] = pApiData->Union.sZBTREQ.u8BroadcastRadius;
            ApiFrame.u8ApiData[13] = pApiData->Union.sZBTREQ.u8Options;
            memcpy(&ApiFrame.u8ApiData[14], pApiData->Union.sZBTREQ.u8Data, pApiData->Union.sZBTREQ.u8DataLen);
            ApiFrame.u16Length = pApiData->Union.sZBTREQ.u8DataLen + 14;
            break;

        default:
            return NBEE_RET_FAILURE;
            break;
    }


    return NBee_Frame_Make(&ApiFrame, pBuf, pLen);
}

u32 NBee_Frame_Make_ATCMD(NB_ATCMD_Enum_t AtCmd, u8 *pFrameID, u8 *pParam, u8 ParamLen, u8 *pBuf, u16 *pLen)
{
    NB_ApiFrameStruct_ApiData_t ApiData;
    NB_ApiFrameStruct_ATCMD_t *p = &ApiData.Union.sATCMD;

    if((pBuf == NULL) || (pLen == NULL))
        return NBEE_RET_PARAM_ERROR;

    if(*pFrameID == 0)
    {
        *pFrameID = 1;
    }

    memset((void *)&ApiData, 0, sizeof(NB_ApiFrameStruct_ApiData_t));

    ApiData.u8FrameType = NBEE_FRAME_TYPE_ATCMD;
    p->u8FrameID = *pFrameID;
    *pFrameID += 1;
    p->u8ParaLen = ParamLen;
    memcpy(p->u8Para, pParam, p->u8ParaLen);

    switch(AtCmd)
    {

        case ATCMD_DH:
            if(ParamLen > 4)
                return NBEE_RET_PARAM_ERROR;

            p->u8CMD1 = 'D';
            p->u8CMD2 = 'H';
            break;

        case ATCMD_DL:
            if(ParamLen > 4)
                return NBEE_RET_PARAM_ERROR;

            p->u8CMD1 = 'D';
            p->u8CMD2 = 'L';
            break;

        case ATCMD_MY:
            p->u8ParaLen = 0;//只读
            p->u8CMD1 = 'M';
            p->u8CMD2 = 'Y';
            break;

        case ATCMD_MP:
            p->u8ParaLen = 0;//只读
            p->u8CMD1 = 'M';
            p->u8CMD2 = 'P';
            break;

        case ATCMD_NC:
            p->u8ParaLen = 0;//只读
            p->u8CMD1 = 'N';
            p->u8CMD2 = 'C';
            break;

        case ATCMD_SH:
            p->u8ParaLen = 0;//只读
            p->u8CMD1 = 'S';
            p->u8CMD2 = 'H';
            break;

        case ATCMD_SL:
            p->u8ParaLen = 0;//只读
            p->u8CMD1 = 'S';
            p->u8CMD2 = 'L';
            break;

        case ATCMD_CH:
            p->u8ParaLen = 0;//只读
            p->u8CMD1 = 'C';
            p->u8CMD2 = 'H';
            break;

        /*
        ATCMD_NI,
        ATCMD_SE,
        ATCMD_DE,
        ATCMD_CI,
        ATCMD_NP,*/
        case ATCMD_CE:
            if((ParamLen > 1) || ((pParam != NULL) && (pParam[0] > 1)))
                return NBEE_RET_PARAM_ERROR;

            p->u8CMD1 = 'C';
            p->u8CMD2 = 'E';
            break;

        case ATCMD_DA:
            p->u8ParaLen = 0;//只读
            p->u8CMD1 = 'D';
            p->u8CMD2 = 'A';
            break;

        case ATCMD_ID:
            if((ParamLen > 8))
                return NBEE_RET_PARAM_ERROR;

            p->u8CMD1 = 'I';
            p->u8CMD2 = 'D';
            break;

        case ATCMD_OP:
            p->u8ParaLen = 0;//只读
            p->u8CMD1 = 'O';
            p->u8CMD2 = 'P';
            break;

        case ATCMD_NH:
            if((ParamLen > 1))
                return NBEE_RET_PARAM_ERROR;

            p->u8CMD1 = 'N';
            p->u8CMD2 = 'H';
            break;

        case ATCMD_BH:
            if((ParamLen > 1))
                return NBEE_RET_PARAM_ERROR;

            p->u8CMD1 = 'B';
            p->u8CMD2 = 'H';
            break;

        case ATCMD_OI:
            p->u8ParaLen = 0;//只读
            p->u8CMD1 = 'O';
            p->u8CMD2 = 'I';
            break;

        case ATCMD_SC:
            if((ParamLen > 2))
                return NBEE_RET_PARAM_ERROR;

            p->u8CMD1 = 'S';
            p->u8CMD2 = 'C';
            break;

        /*ATCMD_SD,
        ATCMD_ZS,*/
        case ATCMD_NJ:
            if((ParamLen > 1))
                return NBEE_RET_PARAM_ERROR;

            p->u8CMD1 = 'N';
            p->u8CMD2 = 'J';
            break;

        case ATCMD_JV:
            if((ParamLen > 1) ||  ((pParam != NULL) && (pParam[0] > 1)))
                return NBEE_RET_PARAM_ERROR;

            p->u8CMD1 = 'J';
            p->u8CMD2 = 'V';
            break;

        /*
        ATCMD_DJ,*/
        case ATCMD_II:
            if((ParamLen > 2))
                return NBEE_RET_PARAM_ERROR;

            p->u8CMD1 = 'I';
            p->u8CMD2 = 'I';
            break;

        case ATCMD_PL:
            if((ParamLen > 1))
                return NBEE_RET_PARAM_ERROR;

            p->u8CMD1 = 'P';
            p->u8CMD2 = 'L';
            break;

        /*
        ATCMD_EE,
        ATCMD_EO,
        ATCMD_NK,
        ATCMD_KY,

        ATCMD_PL,*/
        case ATCMD_DB:
            p->u8ParaLen = 0;//只读
            p->u8CMD1 = 'D';
            p->u8CMD2 = 'B';
            break;

        /*
        ATCMD_PP,

        ATCMD_AP,
        ATCMD_AO,
        ATCMD_BD,
        ATCMD_NB,
        ATCMD_SB,
        ATCMD_RO,
        ATCMD_D7,
        ATCMD_D6,
        ATCMD_AS,
        */
        case ATCMD_VR:
            p->u8ParaLen = 0;//只读
            p->u8CMD1 = 'V';
            p->u8CMD2 = 'R';
            break;

        case ATCMD_HV:
            p->u8ParaLen = 0;//只读
            p->u8CMD1 = 'H';
            p->u8CMD2 = 'V';
            break;

        case ATCMD_AI:
            p->u8ParaLen = 0;//只读
            p->u8CMD1 = 'A';
            p->u8CMD2 = 'I';
            break;

        /*
        ATCMD_CT,
        ATCMD_CN,
        ATCMD_GT,
        ATCMD_CC,*/
        case ATCMD_AC:
            p->u8ParaLen = 0;//只读
            p->u8CMD1 = 'A';
            p->u8CMD2 = 'C';
            break;

        case ATCMD_WR:
            p->u8ParaLen = 0;//只读
            p->u8CMD1 = 'W';
            p->u8CMD2 = 'R';
            break;

        case ATCMD_RE:
            p->u8ParaLen = 0;//只读
            p->u8CMD1 = 'R';
            p->u8CMD2 = 'E';
            break;

        case ATCMD_FR:
            p->u8ParaLen = 0;//只读
            p->u8CMD1 = 'F';
            p->u8CMD2 = 'R';
            break;

        case ATCMD_CB:
            if((ParamLen > 1) ||  ((pParam != NULL) && (pParam[0] != 2)))
                return NBEE_RET_PARAM_ERROR;

            p->u8CMD1 = 'C';
            p->u8CMD2 = 'B';
            break;

        /*
        ATCMD_NR,
        ,
        ATCMD_IS,*/

        default:
            return NBEE_RET_FAILURE;
            break;
    }


    return NBee_Frame_Make_AllType(&ApiData, pBuf, pLen);
}

u32 NBee_Frame_Make_Broadcast(u8 FrameID, u8 *pData, u8 DataLen, u8 *pBuf, u16 *pLen)
{
    NB_ApiFrameStruct_ApiData_t ApiData;
    NB_ApiFrameStruct_ZBTREQ_t *p = &ApiData.Union.sZBTREQ;

    if((pBuf == NULL) || (pLen == NULL))
        return NBEE_RET_PARAM_ERROR;

    if(FrameID == 0)
        return NBEE_RET_PARAM_ERROR;

    memset((void *)&ApiData, 0, sizeof(NB_ApiFrameStruct_ApiData_t));

    ApiData.u8FrameType = NBEE_FRAME_TYPE_ZBTREQ;
    p->u8FrameID = FrameID;
    p->u8ExPanid[0] = 0x0;
    p->u8ExPanid[1] = 0x0;
    p->u8ExPanid[2] = 0x0;
    p->u8ExPanid[3] = 0x0;
    p->u8ExPanid[4] = 0x0;
    p->u8ExPanid[5] = 0x0;
    p->u8ExPanid[6] = 0xff;
    p->u8ExPanid[7] = 0xff;
    p->u8Panid[0] = 0xff;
    p->u8Panid[1] = 0xfe;
    p->u8BroadcastRadius = 0x0;
    p->u8Options = 0;
    p->u8DataLen = DataLen;
    memcpy(p->u8Data, pData, p->u8DataLen);

    return NBee_Frame_Make_AllType(&ApiData, pBuf, pLen);
}


u32 NBee_Frame_GetModemStatusFromApiData(NB_ApiFrameStruct_t Frame, NB_ApiFrameStruct_MSTA_t *pModemSta)
{
    if(pModemSta == NULL)
        return NBEE_RET_PARAM_ERROR;

    if(Frame.u16Length != 0x02)
        return NBEE_RET_FAILURE;

    pModemSta->u8Status = Frame.u8ApiData[1];

    return NBEE_RET_SUCC;
}

u32 NBee_Frame_GetAtCmdResponseFromApiData(NB_ApiFrameStruct_t Frame, NB_ApiFrameStruct_ATCMDRSP_t *pRsp)
{
    u8 strBuf[3] = {0};

    if(pRsp == NULL)
        return NBEE_RET_PARAM_ERROR;

    if(Frame.u16Length < 5)
        return NBEE_RET_FAILURE;

    pRsp->u8FrameID = Frame.u8ApiData[1];
    pRsp->u8CMD1 = Frame.u8ApiData[2];
    pRsp->u8CMD2 = Frame.u8ApiData[3];
    pRsp->enumCmdStatus = (NB_CmdStatus_Enum_t)Frame.u8ApiData[4];
    pRsp->u8ParaLen = Frame.u16Length - 5;
    memcpy(pRsp->u8Para, &Frame.u8ApiData[5], pRsp->u8ParaLen);

    strBuf[0] = pRsp->u8CMD1;
    strBuf[1] = pRsp->u8CMD2;


    if(0 == strncmp((const char *)strBuf, "DH", 2))
        pRsp->AtCmd = ATCMD_DH;
    else if(0 == strncmp((const char *)strBuf, "DL", 2))
        pRsp->AtCmd = ATCMD_DL;
    else if(0 == strncmp((const char *)strBuf, "MY", 2))
        pRsp->AtCmd = ATCMD_MY;
    else if(0 == strncmp((const char *)strBuf, "MP", 2))
        pRsp->AtCmd = ATCMD_MP;
    else if(0 == strncmp((const char *)strBuf, "NC", 2))
        pRsp->AtCmd = ATCMD_NC;
    else if(0 == strncmp((const char *)strBuf, "SH", 2))
        pRsp->AtCmd = ATCMD_SH;
    else if(0 == strncmp((const char *)strBuf, "SL", 2))
        pRsp->AtCmd = ATCMD_SL;
    else if(0 == strncmp((const char *)strBuf, "NI", 2))
        pRsp->AtCmd = ATCMD_NI;
    else if(0 == strncmp((const char *)strBuf, "SE", 2))
        pRsp->AtCmd = ATCMD_SE;
    else if(0 == strncmp((const char *)strBuf, "DE", 2))
        pRsp->AtCmd = ATCMD_DE;
    else if(0 == strncmp((const char *)strBuf, "CI", 2))
        pRsp->AtCmd = ATCMD_CI;
    else if(0 == strncmp((const char *)strBuf, "NP", 2))
        pRsp->AtCmd = ATCMD_NP;
    else if(0 == strncmp((const char *)strBuf, "CH", 2))
        pRsp->AtCmd = ATCMD_CH;
    else if(0 == strncmp((const char *)strBuf, "CE", 2))
        pRsp->AtCmd = ATCMD_CE;
    else if(0 == strncmp((const char *)strBuf, "DA", 2))
        pRsp->AtCmd = ATCMD_DA;
    else if(0 == strncmp((const char *)strBuf, "ID", 2))
        pRsp->AtCmd = ATCMD_ID;
    else if(0 == strncmp((const char *)strBuf, "OP", 2))
        pRsp->AtCmd = ATCMD_OP;
    else if(0 == strncmp((const char *)strBuf, "NH", 2))
        pRsp->AtCmd = ATCMD_NH;
    else if(0 == strncmp((const char *)strBuf, "BH", 2))
        pRsp->AtCmd = ATCMD_BH;
    else if(0 == strncmp((const char *)strBuf, "OI", 2))
        pRsp->AtCmd = ATCMD_OI;
    else if(0 == strncmp((const char *)strBuf, "SC", 2))
        pRsp->AtCmd = ATCMD_SC;
    else if(0 == strncmp((const char *)strBuf, "SD", 2))
        pRsp->AtCmd = ATCMD_SD;
    else if(0 == strncmp((const char *)strBuf, "ZS", 2))
        pRsp->AtCmd = ATCMD_ZS;
    else if(0 == strncmp((const char *)strBuf, "NJ", 2))
        pRsp->AtCmd = ATCMD_NJ;
    else if(0 == strncmp((const char *)strBuf, "JV", 2))
        pRsp->AtCmd = ATCMD_JV;
    else if(0 == strncmp((const char *)strBuf, "DJ", 2))
        pRsp->AtCmd = ATCMD_DJ;
    else if(0 == strncmp((const char *)strBuf, "II", 2))
        pRsp->AtCmd = ATCMD_II;
    else if(0 == strncmp((const char *)strBuf, "EE", 2))
        pRsp->AtCmd = ATCMD_EE;
    else if(0 == strncmp((const char *)strBuf, "EO", 2))
        pRsp->AtCmd = ATCMD_EO;
    else if(0 == strncmp((const char *)strBuf, "NK", 2))
        pRsp->AtCmd = ATCMD_NK;
    else if(0 == strncmp((const char *)strBuf, "KY", 2))
        pRsp->AtCmd = ATCMD_KY;
    else if(0 == strncmp((const char *)strBuf, "PL", 2))
        pRsp->AtCmd = ATCMD_PL;
    else if(0 == strncmp((const char *)strBuf, "DB", 2))
        pRsp->AtCmd = ATCMD_DB;
    else if(0 == strncmp((const char *)strBuf, "PP", 2))
        pRsp->AtCmd = ATCMD_PP;
    else if(0 == strncmp((const char *)strBuf, "AO", 2))
        pRsp->AtCmd = ATCMD_AO;
    else if(0 == strncmp((const char *)strBuf, "BD", 2))
        pRsp->AtCmd = ATCMD_BD;
    else if(0 == strncmp((const char *)strBuf, "NB", 2))
        pRsp->AtCmd = ATCMD_NB;
    else if(0 == strncmp((const char *)strBuf, "SB", 2))
        pRsp->AtCmd = ATCMD_SB;
    else if(0 == strncmp((const char *)strBuf, "RO", 2))
        pRsp->AtCmd = ATCMD_RO;
    else if(0 == strncmp((const char *)strBuf, "D7", 2))
        pRsp->AtCmd = ATCMD_D7;
    else if(0 == strncmp((const char *)strBuf, "D6", 2))
        pRsp->AtCmd = ATCMD_D6;
    else if(0 == strncmp((const char *)strBuf, "AS", 2))
        pRsp->AtCmd = ATCMD_AS;
    else if(0 == strncmp((const char *)strBuf, "HV", 2))
        pRsp->AtCmd = ATCMD_HV;
    else if(0 == strncmp((const char *)strBuf, "AI", 2))
        pRsp->AtCmd = ATCMD_AI;
    else if(0 == strncmp((const char *)strBuf, "CT", 2))
        pRsp->AtCmd = ATCMD_CT;
    else if(0 == strncmp((const char *)strBuf, "CN", 2))
        pRsp->AtCmd = ATCMD_CN;
    else if(0 == strncmp((const char *)strBuf, "GT", 2))
        pRsp->AtCmd = ATCMD_GT;
    else if(0 == strncmp((const char *)strBuf, "CC", 2))
        pRsp->AtCmd = ATCMD_CC;
    else if(0 == strncmp((const char *)strBuf, "AC", 2))
        pRsp->AtCmd = ATCMD_AC;
    else if(0 == strncmp((const char *)strBuf, "WR", 2))
        pRsp->AtCmd = ATCMD_WR;
    else if(0 == strncmp((const char *)strBuf, "RE", 2))
        pRsp->AtCmd = ATCMD_RE;
    else if(0 == strncmp((const char *)strBuf, "FR", 2))
        pRsp->AtCmd = ATCMD_FR;
    else if(0 == strncmp((const char *)strBuf, "NR", 2))
        pRsp->AtCmd = ATCMD_NR;
    else if(0 == strncmp((const char *)strBuf, "CB", 2))
        pRsp->AtCmd = ATCMD_CB;
    else if(0 == strncmp((const char *)strBuf, "IS", 2))
        pRsp->AtCmd = ATCMD_IS;
    else
        pRsp->AtCmd = ATCMD_UNKNOWN;


    return NBEE_RET_SUCC;
}


u32 NBee_Frame_GetRecvPacketFromApiData(NB_ApiFrameStruct_t Frame, NB_ApiFrameStruct_ZBR_t *p)
{
    if(p == NULL)
        return NBEE_RET_PARAM_ERROR;

    if(Frame.u16Length < 12)
        return NBEE_RET_FAILURE;

    memcpy(p->u8SrcAddr_64bit, &Frame.u8ApiData[1], 8);
    memcpy(p->u8SrcAddr_16bit, &Frame.u8ApiData[9], 2);
    p->u8Options = Frame.u8ApiData[11];
    p->u16DataLen = Frame.u16Length - 12;

    if(p->u16DataLen != 0)
    {
        memcpy(p->u8Data, &Frame.u8ApiData[12], p->u16DataLen);
    }

    return NBEE_RET_SUCC;
}

u32 NBee_Frame_GetTransmitStatusFromApiData(NB_ApiFrameStruct_t Frame, NB_ApiFrameStruct_ZBTRSP_t *p)
{
    if(p == NULL)
        return NBEE_RET_PARAM_ERROR;

    if(Frame.u16Length != 0x7)
        return NBEE_RET_FAILURE;

    p->u8FrameID = Frame.u8ApiData[1];
    memcpy(p->u8DstAddr, &Frame.u8ApiData[2], 2);
    p->u8RetryCnt = Frame.u8ApiData[4];
    p->u8DeliveryStatus = Frame.u8ApiData[5];
    p->u8DiscoveryStatus = Frame.u8ApiData[6];

    return NBEE_RET_SUCC;
}







/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
