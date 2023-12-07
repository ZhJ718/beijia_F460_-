/********************************************************************************
* @File name: ir38com_task.c
* @Author: xfw
* @Version: 1.0
* @Date: 2023-4-11
* @Description:
********************************************************************************/

#include "ir38com_task.h"
#include "ir.h"
#include "zigcom_task.h"
#include "rescom_task.h"
#include "lasercom_task.h"
#include "battery_task.h"
/*************底层实现************************************************/
//端口初始化
void IR38Com_ComInit(void)
{
    User_Ir_Init();
}

//端口写数据
uint32_t IR38Com_Write(uint8_t *pBuf, uint16_t Len)
{
    #ifdef IR38_COM_LOG_OUTPUT

    LOG("IR_38K(%d)->", Len);
    printf_with_data(pBuf, Len);

    #endif

    return User_Ir_Send(pBuf, Len);
}


//端口读数据
uint32_t IR38Com_Read(uint8_t *pBuf, uint16_t *pLen)
{
    uint8_t ret, Tmp[Ir_Max_Rcv];
    uint16_t TmpLen = Ir_Max_Rcv;

    ret = User_Ir_Rcv(Tmp, &TmpLen);

    if(ret == 0)
    {
        #ifdef IR38_COM_LOG_OUTPUT

        LOG("IR_38K(%d)<-", TmpLen);
        printf_with_data(Tmp, TmpLen);

        #endif

        if(*pLen >= TmpLen)
        {
            memcpy((void *)pBuf, (void *)Tmp, TmpLen);
            *pLen = TmpLen;
        }
        else
        {
            //缓存区不够,不拷贝多余数据
            memcpy((void *)pBuf, (void *)Tmp, *pLen);
        }

        return TRUE;
    }

    return FALSE;
}

/*************底层实现结束************************************************/



static IR38COMTASK_MAG g_sIR38ComTask_Mag;


/****************************************************************
@FUNCTION：获取红外短距端口管理句柄
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 

****************************************************************/
IR38COMTASK_MAG *IR38ComTask_HandleGet(void)
{
    return &g_sIR38ComTask_Mag;
}

/****************************************************************
@FUNCTION：获取红外短距端口任务管理句柄
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 

****************************************************************/
COMTASK_MAG *IR38ComTask_TaskHandleGet(void)
{
    return &g_sIR38ComTask_Mag.m_sTaskMag;
}


/****************************************************************
@FUNCTION：红外短距功能初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sIR38ComTask_Mag

****************************************************************/
void IR38ComTask_Init(void)
{
    IR38COMTASK_MAG *pMag = IR38ComTask_HandleGet();

    memset((void *)pMag, 0, sizeof(IR38COMTASK_MAG));

    pMag->m_sLLMag.m_sHwOps.hw_init = IR38Com_ComInit;
    pMag->m_sLLMag.m_sHwOps.read = IR38Com_Read;
    pMag->m_sLLMag.m_sHwOps.write = IR38Com_Write;

    //初始化
    pMag->m_sLLMag.m_sHwOps.hw_init();

    pMag->m_bIsInit = ComTask_Init(&pMag->m_sTaskMag, IR38_COM_FRAME_LEN_MAX, 100);
}


/****************************************************************
@FUNCTION：红外短距功能反初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sIR38ComTask_Mag

****************************************************************/
void IR38ComTask_DeInit(void)
{
    IR38COMTASK_MAG *pMag = IR38ComTask_HandleGet();

    ComTask_DeInit(&pMag->m_sTaskMag);

    memset((void *)pMag, 0, sizeof(IR38COMTASK_MAG));
}

#if 0

void IR_ProtDeal_InfoSend(uint8_t *pBuf, uint16_t BufLen, void *pHandle)
{
    IR38COMTASK_MAG *pMag = (IR38COMTASK_MAG *)pHandle;
    FRAME_BUF *pFrame = NULL;
    uint16_t i, Len, SlaverCnt;
    SINGLE_SLAVER_MAG *pSlaverMag;

    pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

    if(pFrame == NULL)
    {
        return;
    }

    pFrame->m_wBufLen = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xA3;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xFF;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = FIXVERSION;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = BOARDID;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = GetHardSta1();
    pFrame->m_byBuf[pFrame->m_wBufLen++] = GetHardSta2();
    pFrame->m_byBuf[pFrame->m_wBufLen++] = GetHardSta3();
    pFrame->m_byBuf[pFrame->m_wBufLen++] = GetHardSta4();
    pFrame->m_byBuf[pFrame->m_wBufLen++] = UserParam_GetActLaserCnt();
    // 电池电量（扩大1000倍）
    pFrame->m_byBuf[pFrame->m_wBufLen++] = BatTask_GetExBatVol() >> 8;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = BatTask_GetExBatVol() & 0xFF;
    // 自身MAC地址
    memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen],UserParam_GetWMac(),8);
    pFrame->m_wBufLen += 8;
    
    // 电台信号强度（0-4）
    pFrame->m_byBuf[pFrame->m_wBufLen++] = GetDTRssi();
    // 配置子设备数量n(若为0，则无后面字段)
    SlaverCnt = UserParam_GetSlaverCnt();
    pFrame->m_byBuf[pFrame->m_wBufLen++] = SlaverCnt;

    for(i = 0; i < MIN(SlaverCnt, SYS_SLAVER_CNT_MAX); i++)
    {
        pSlaverMag = UserParam_GetSlaverMag(i);
        pFrame->m_byBuf[pFrame->m_wBufLen++] = pSlaverMag->m_byTType;
        pFrame->m_byBuf[pFrame->m_wBufLen++] = pSlaverMag->m_byTSubType;
        pFrame->m_byBuf[pFrame->m_wBufLen++] = pSlaverMag->m_byWType;
        memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen], pSlaverMag->m_aWMac, sizeof(pSlaverMag->m_aWMac));
        pFrame->m_wBufLen += sizeof(pSlaverMag->m_aWMac);
        memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen], (uint8_t *)&pSlaverMag->m_sTsChar1, 1);
        pFrame->m_wBufLen += 1;
        pFrame->m_byBuf[pFrame->m_wBufLen++] = pSlaverMag->m_wBatVol >> 8;
        pFrame->m_byBuf[pFrame->m_wBufLen++] = pSlaverMag->m_wBatVol & 0xFF;
    }

    pFrame->m_byBuf[2] = pFrame->m_wBufLen - 3;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = Xor_Calc(&pFrame->m_byBuf[3], pFrame->m_wBufLen - 3);

    LinkQueue_Append(pMag->m_sTaskMag.m_pSendQueue, (void*)pFrame);

}




void IR_ProtDeal_IDSet(uint8_t *pBuf, uint16_t BufLen, void *pHandle)
{
    IR38COMTASK_MAG *pMag = (IR38COMTASK_MAG *)pHandle;
    FRAME_BUF *pFrame = NULL;

    uint8_t Offset, TType, TSubtype, RID, BrChar, Key, NeedSave;
    uint16_t PID;

    //输入参数校验
    if(pBuf == NULL || BufLen < 11 || pHandle == NULL)
    {
        return;
    }

    Offset = 3;
    TType = pBuf[Offset++];
    TSubtype = pBuf[Offset++];
    RID = pBuf[Offset++];
    PID = (uint16_t)pBuf[Offset] << 8 | pBuf[Offset + 1];
    Offset += 2;
    BrChar = pBuf[Offset++];
    Key = pBuf[Offset++];

    NeedSave = FALSE;

    if(TRUE == IsParamValid((uint8_t *)&TType, 1))
    {
        IDParam_SetTType(TType);
        NeedSave = TRUE;
    }

    if(TRUE == IsParamValid((uint8_t *)&TSubtype, 1))
    {
        IDParam_SetTSubType(TSubtype);
        NeedSave = TRUE;
    }

    if(TRUE == IsParamValid((uint8_t *)&RID, 1))
    {
        IDParam_SetRid(RID);
        NeedSave = TRUE;
    }

    if(TRUE == IsParamValid((uint8_t *)&PID, 2))
    {
        IDParam_SetPid(PID);
        NeedSave = TRUE;
    }

    if(TRUE == IsParamValid((uint8_t *)&BrChar, 1))
    {
        UserParam_SetBRChar((ENUM_BRCHAR)BrChar);
        NeedSave = TRUE;
    }

    if(TRUE == IsParamValid((uint8_t *)&Key, 1))
    {
        UserParam_SetKey(Key);
        NeedSave = TRUE;
    }

    if(TRUE == NeedSave)
    {
        IWDG_Feed();
        IDParam_Save();
        IWDG_Feed();
        UserParam_Save();
        IWDG_Feed();
    }

    //组织回包
    pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

    if(pFrame == NULL)
    {
        return;
    }

    //红外短距回复

    pFrame->m_wBufLen = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xA3;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = pBuf[1];
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetTType();
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetTSubType();
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetRid();
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetPid() >> 8;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetPid() & 0xFF;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = UserParam_GetBRChar();
    pFrame->m_byBuf[pFrame->m_wBufLen++] = UserParam_GetKey();

    pFrame->m_byBuf[2] = pFrame->m_wBufLen - 3;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = Xor_Calc(&pFrame->m_byBuf[3], pFrame->m_wBufLen - 3);

    LinkQueue_Append(pMag->m_sTaskMag.m_pSendQueue, (void*)pFrame);
}

void IR_ProtDeal_SlaverClear(uint8_t *pBuf, uint16_t BufLen, void *pHandle)
{
    IR38COMTASK_MAG *pMag = (IR38COMTASK_MAG *)pHandle;
    FRAME_BUF *pFrame = NULL;

    uint8_t Offset, TType, TSubtype, RID;
    uint16_t PID;

    //输入参数校验
    if(pBuf == NULL || BufLen < 9 || pHandle == NULL)
    {
        return;
    }

    Offset = 3;
    TType = pBuf[Offset++];
    TSubtype = pBuf[Offset++];
    RID = pBuf[Offset++];
    PID = (uint16_t)pBuf[Offset] << 8 | pBuf[Offset + 1];
    Offset += 2;

    //是否是发给自己
    if(TRUE != IDParam_IsIDValid(TType, TSubtype, RID, PID))
    {
        //return;
    }

    SysParam_SetDefaultValue(NULL);
    IWDG_Feed();
    SysParam_Save();
    IWDG_Feed();
    //组织回包
    pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

    if(pFrame == NULL)
    {
        return;
    }

    //红外短距回复

    pFrame->m_wBufLen = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xA3;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = pBuf[1];
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetTType();
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetTSubType();
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetRid();
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetPid() >> 8;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetPid() & 0xFF;

    pFrame->m_byBuf[2] = pFrame->m_wBufLen - 3;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = Xor_Calc(&pFrame->m_byBuf[3], pFrame->m_wBufLen - 3);

    LinkQueue_Append(pMag->m_sTaskMag.m_pSendQueue, (void*)pFrame);
}

void IR_ProtDeal_SlaverConfig(uint8_t *pBuf, uint16_t BufLen, void *pHandle)
{
    IR38COMTASK_MAG *pMag = (IR38COMTASK_MAG *)pHandle;
    FRAME_BUF *pFrame = NULL;

    uint8_t Offset, TType, TSubtype, RID, Idx;
    uint16_t PID;
    SINGLE_SLAVER_PARAM SlaverParam = {0};

    //输入参数校验
    if(pBuf == NULL || BufLen < 21 || pHandle == NULL)
    {
        return;
    }

    Offset = 3;
    TType = pBuf[Offset++];
    TSubtype = pBuf[Offset++];
    RID = pBuf[Offset++];
    PID = (uint16_t)pBuf[Offset] << 8 | pBuf[Offset + 1];
    Offset += 2;

    //是否是发给自己
    if(TRUE != IDParam_IsIDValid(TType, TSubtype, RID, PID))
    {
        //return;
    }

    Idx = pBuf[Offset++];

    if(Idx == 0 || Idx > SYS_SLAVER_CNT_MAX)
    {
        return;
    }

    //保存配置参数
    SlaverParam.m_byTType = pBuf[Offset++];
    SlaverParam.m_byTSubType = pBuf[Offset++];
    SlaverParam.m_byWType = pBuf[Offset++];
    memcpy(SlaverParam.m_aWMac, &pBuf[Offset], sizeof(SlaverParam.m_aWMac));
    Offset += sizeof(SlaverParam.m_aWMac);
    SysParam_SetSlaverParam(&SlaverParam, Idx - 1);
    IWDG_Feed();
    SysParam_Save();
    IWDG_Feed();
    //组织回包
    pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

    if(pFrame == NULL)
    {
        return;
    }

    //红外短距回复

    pFrame->m_wBufLen = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xA3;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = pBuf[1];
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetTType();
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetTSubType();
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetRid();
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetPid() >> 8;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetPid() & 0xFF;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = Idx;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = SlaverParam.m_byWType;
    memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen], SlaverParam.m_aWMac, sizeof(SlaverParam.m_aWMac));
    pFrame->m_wBufLen += sizeof(SlaverParam.m_aWMac);

    pFrame->m_byBuf[2] = pFrame->m_wBufLen - 3;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = Xor_Calc(&pFrame->m_byBuf[3], pFrame->m_wBufLen - 3);

    LinkQueue_Append(pMag->m_sTaskMag.m_pSendQueue, (void*)pFrame);
}

#if 0
void IR_ProtDeal_ParamGet(uint8_t *pBuf, uint16_t BufLen, void *pHandle)
{
    IR38COMTASK_MAG *pMag = (IR38COMTASK_MAG *)pHandle;
    FRAME_BUF *pFrame = NULL;

    uint8_t Offset, TType, TSubtype, RID;
    uint16_t PID;
    uint8_t *pSerIp;

    //输入参数校验
    if(pBuf == NULL || BufLen < 9 || pHandle == NULL)
    {
        return;
    }

    Offset = 3;
    TType = pBuf[Offset++];
    TSubtype = pBuf[Offset++];
    RID = pBuf[Offset++];
    PID = (uint16_t)pBuf[Offset] << 8 | pBuf[Offset + 1];
    Offset += 2;

    //是否是发给自己
    if(TRUE != IDParam_IsIDValid(TType, TSubtype, RID, PID))
    {
        return;
    }

    //组织回包
    pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

    if(pFrame == NULL)
    {
        return;
    }

    //红外短距回复

    pFrame->m_wBufLen = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xA3;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = pBuf[1];
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetTType();
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetTSubType();
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetRid();
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetPid() >> 8;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetPid() & 0xFF;

    memcpy((void *)&pFrame->m_byBuf[pFrame->m_wBufLen], (void *)IDParam_GetServerIP(), 4);
    pFrame->m_wBufLen += 4;

    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetServerPort() >> 8;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetServerPort() & 0xFF;

    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetIsUDP();
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetTxPwr();

    memcpy((void *)&pFrame->m_byBuf[pFrame->m_wBufLen], (void *)IDParam_GetWMac(), 8);
    pFrame->m_wBufLen += 8;

    pFrame->m_byBuf[2] = pFrame->m_wBufLen - 3;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = Xor_Calc(&pFrame->m_byBuf[3], pFrame->m_wBufLen - 3);

    LinkQueue_Append(pMag->m_sTaskMag.m_pSendQueue, (void*)pFrame);
}
#endif

void IR_ProtDeal_ParamSet(uint8_t *pBuf, uint16_t BufLen, void *pHandle)
{
    IR38COMTASK_MAG *pMag = (IR38COMTASK_MAG *)pHandle;
    FRAME_BUF *pFrame = NULL;

    uint8_t Offset, TType, TSubtype, RID, IsUDP, TxPwr, NeedSave;
    uint16_t PID, SerPort;
    uint8_t SerIp[4], ZigMac[8];


    //输入参数校验
    if(pBuf == NULL || BufLen < 25 || pHandle == NULL)
    {
        return;
    }

    Offset = 3;
    TType = pBuf[Offset++];
    TSubtype = pBuf[Offset++];
    RID = pBuf[Offset++];
    PID = (uint16_t)pBuf[Offset] << 8 | pBuf[Offset + 1];
    Offset += 2;

    //是否是发给自己
    if(TRUE != IDParam_IsIDValid(TType, TSubtype, RID, PID))
    {
        //return;
    }

    memcpy(SerIp, &pBuf[Offset], 4);
    Offset += 4;
    SerPort = (uint16_t)pBuf[Offset] << 8 | pBuf[Offset + 1];
    Offset += 2;
    IsUDP = pBuf[Offset++];
    TxPwr = pBuf[Offset++];
    memcpy(ZigMac, &pBuf[Offset], 8);
    Offset += 8;

    NeedSave = FALSE;

    if(TRUE == IsParamValid(SerIp, sizeof(SerIp)))
    {
        IDParam_SetServerIP(SerIp, sizeof(SerIp));
        NeedSave = TRUE;
    }

    if(TRUE == IsParamValid((uint8_t *)&SerPort, sizeof(SerPort)))
    {
        IDParam_SetServerPort(SerPort);
        NeedSave = TRUE;
    }

    if(TRUE == IsParamValid((uint8_t *)&IsUDP, 1))
    {
        IDParam_SetIsUDP(IsUDP);
        NeedSave = TRUE;
    }

    if(TRUE == IsParamValid((uint8_t *)&TxPwr, 1))
    {
        IDParam_SetTxPwr(TxPwr);
        NeedSave = TRUE;
    }

    if(TRUE == IsParamValid(ZigMac, sizeof(ZigMac)))
    {
        IDParam_SetWMac(ZigMac, sizeof(ZigMac));
        NeedSave = TRUE;
    }

    if(TRUE == NeedSave)
    {
        IWDG_Feed();
        IDParam_Save();
        IWDG_Feed();
    }

    //组织回包
    pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

    if(pFrame == NULL)
    {
        return;
    }

    //红外短距回复

    pFrame->m_wBufLen = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xA3;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = pBuf[1];
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetTType();
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetTSubType();
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetRid();
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetPid() >> 8;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetPid() & 0xFF;

    memcpy((void *)&pFrame->m_byBuf[pFrame->m_wBufLen], (void *)IDParam_GetServerIP(), 4);
    pFrame->m_wBufLen += 4;

    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetServerPort() >> 8;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetServerPort() & 0xFF;

    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetIsUDP();
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetTxPwr();

    memcpy((void *)&pFrame->m_byBuf[pFrame->m_wBufLen], (void *)UserParam_GetWMac(), 8);
    pFrame->m_wBufLen += 8;

    pFrame->m_byBuf[2] = pFrame->m_wBufLen - 3;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = Xor_Calc(&pFrame->m_byBuf[3], pFrame->m_wBufLen - 3);

    LinkQueue_Append(pMag->m_sTaskMag.m_pSendQueue, (void*)pFrame);
}


//端口协议处理
void IR38Com_Analysis(uint8_t *pBuf, uint16_t BufLen, void *pHandle)
{
    IR38COMTASK_MAG *pMag = (IR38COMTASK_MAG *)pHandle;
    uint8_t TmpLen;
    uint16_t IR_Cmd;


    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 1);
    IR_Cmd = ((pBuf[0] << 8) | pBuf[1]);

    switch(IR_Cmd)
    {

        case 0xA2A0:	// 4.2 无线设备MAC地址获取指令
            //InterConn_ProtDeal_MacGet(pBuf, BufLen, pHandle);
            break;

        case 0xA2A1:	// 4.4 身份信息设置指令（0xA2，0xA1）
            IR_ProtDeal_IDSet(pBuf, BufLen, pHandle);
            break;

        case 0xA2A3:	// 4.8 清空无线子设备指令
            IR_ProtDeal_SlaverClear(pBuf, BufLen, pHandle);
            SlaverMag_ListInit();
            break;

        case 0xA2A4:	// 4.10 配置无线子设备指令（0xA2，0xA4）
            IR_ProtDeal_SlaverConfig(pBuf, BufLen, pHandle);
            SlaverMag_ListInit();
            break;

        case 0xA2AD:	// 4.28 获取工况信息指令（0xA2，0xAD）
            IR_ProtDeal_InfoSend(pBuf, BufLen, pHandle);
            break;

        case 0xA2AE:	// 4.29 实体参数获取指令（0xA2，0xAE） 删除

            break;

        case 0xA2AF:	// 4.31 实体参数设置指令（0xA2，0xAF）
            IR_ProtDeal_ParamSet(pBuf, BufLen, pHandle);
            break;

        case 0xA2FF:	// 板级测试模式（0xA2，0xFF） 自定义
            IR_ProtDeal_InfoSend(pBuf, BufLen, pHandle);
            TesModeHandle();
            break;

        default:
            break;
    }


}

#endif

/****************************************************************
@FUNCTION：红外短距功能任务主进程
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sIR38ComTask_Mag

****************************************************************/
void IR38ComTask_Process(void)
{
    IR38COMTASK_MAG *pMag = IR38ComTask_HandleGet();
    COMTASK_MAG *pComTaskMag = &pMag->m_sTaskMag;
    uint16_t i, Len = pComTaskMag->m_wProtBufMaxLen - pComTaskMag->m_wProtBufLen;
    uint32_t NowTime = GetSysRunMilliSecond();
    FRAME_BUF *pFrame = NULL;

    if(FALSE == pMag->m_bIsInit)
    {
        return;
    }

    //定时任务
    if((NowTime >  pComTaskMag->m_dwLastScheduleTime) &&
            (NowTime -  pComTaskMag->m_dwLastScheduleTime >= pComTaskMag->m_dwScheduleTime) )
    {
        pComTaskMag->m_dwLastScheduleTime = NowTime;

        //红外短距不定长数据接收超时处理
        //Ir_Rcv_Timeout();

        if(FALSE == IsIR38SendBusy())
        {
            //检查发送队列
            pFrame = LinkQueue_Header(pComTaskMag->m_pSendQueue);

            if(pFrame != NULL)
            {
								pMag->m_sLLMag.m_sHwOps.write(pFrame->m_byBuf, pFrame->m_wBufLen);
                LinkQueue_Retrieve(pComTaskMag->m_pSendQueue);
								
            }
        }

    }

    //串口接收处理
    if(TRUE == pMag->m_sLLMag.m_sHwOps.read(&pComTaskMag->m_pProtBuf[pComTaskMag->m_wProtBufLen], &Len))
    {
        //pMag->m_sLLMag.m_sHwOps.hw_init();

        #ifdef SEND_TO_UPCOM
        //转发数据到上行口
        if(Len>0)
        {
            ResComTask_SendToUpcom(&pComTaskMag->m_pProtBuf[pComTaskMag->m_wProtBufLen], Len, 0x08, CTRL_RECV);
        }
        pComTaskMag->m_wProtBufLen = 0;
        #endif

        #if 1
        pComTaskMag->m_wProtBufLen += Len;

        for(i = 0; i < pComTaskMag->m_wProtBufLen;)
        {
            Len = pComTaskMag->m_wProtBufLen - i;

            if(TRUE == Protocal_FindIR38_Frame(&pComTaskMag->m_pProtBuf[i], &Len))
            {
                pComTaskMag->m_dwLastCommTime = NowTime;

								Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 1);
		
                //IR38Com_Analysis(&pComTaskMag->m_pProtBuf[i], Len, (void *)pMag);
                InterConn_Analysis(CHAN_IR38, &pComTaskMag->m_pProtBuf[i], Len);
                
                i = Len + i;

            }
            else
            {
                i++;
            }

            if(i >= pComTaskMag->m_wProtBufLen)
            {
                pComTaskMag->m_wProtBufLen = 0;
            }
        }

        #endif
    }

}



