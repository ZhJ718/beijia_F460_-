/******************************************************************
*版权所有 (C)2023, 江苏华如防务科技有限公司
*
*文件名称:  upgrade.c
*文件标识:
*内容摘要:
*其它说明:
*当前版本:
*作    者:  xfw
*完成日期:  2023/2/27
*
*修改记录1:
*    修改日期:
*    版 本 号:
*    修 改 人:
*    修改内容:
******************************************************************/

/******************************************************************
*                             头文件                              *
******************************************************************/

#include "upgrade.h"
#include "iflash.h"
#include "extflash.h"
#include "uart.h"
#include "iwdt.h"
#include "protocal.h"
#include "malloc.h"
#include "iwdt.h"
#include "param.h"


UPDATE_MANAGER g_sUpdateMag;

uint8_t Protocal_CmdProcess_GetDevMac(uint16_t CmdLen, uint8_t *pCmdBuf, uint8_t *pRetBuf, uint16_t *pRetLen)
{

    uint8_t temp[20];
    uint8_t len;
    UPDATE_MANAGER *pUpMag = &g_sUpdateMag;

    len = 0;
    temp[len] = PROTOCAL_CMD_GET_MAC;
    len += 1;

    memcpy(&temp[len], pUpMag->m_aDevMac, sizeof(pUpMag->m_aDevMac));

    len += sizeof(pUpMag->m_aDevMac);

    return Protocal_ProtocolMakeShort55AA(temp, len, pRetBuf, pRetLen);

}

uint8_t Protocal_CmdProcess_GetAppVersion(uint16_t CmdLen, uint8_t *pCmdBuf, uint8_t *pRetBuf, uint16_t *pRetLen)
{

    uint8_t temp[20];
    uint8_t len;

    len = 0;
    temp[len] = PROTOCAL_CMD_GET_APPVER;
    len += 1;
    temp[len] = BOARDID >> 8;
    len += 1;
    temp[len] = BOARDID & 0xFF;
    len += 1;
    #ifdef _APP
    temp[len] = (SUBVERSION&0x7F)|0x80;
    len += 1;
    temp[len] = FIXVERSION;
    len += 1;
    #else
    temp[len] = MAINVERSION;
    len += 1;
    temp[len] = SUBVERSION;
    len += 1;
    #endif
    return Protocal_ProtocolMakeShort55AA(temp, len, pRetBuf, pRetLen);

}

uint8_t Protocal_CmdProcess_GetDevID(uint16_t CmdLen, uint8_t *pCmdBuf, uint8_t *pRetBuf, uint16_t *pRetLen)
{

    uint8_t temp[20];
    uint8_t len;
    UPDATE_MANAGER *pUpMag = &g_sUpdateMag;


    len = 0;
    temp[len] = PROTOCAL_CMD_GET_DEVID;
    len += 1;
    temp[len] = pUpMag->m_byTType;
    len += 1;
    temp[len] = pUpMag->m_byTSubType;
    len += 1;
    temp[len] = pUpMag->m_byRID;
    len += 1;
    temp[len] = pUpMag->m_wPID >> 8;
    len += 1;
    temp[len] = pUpMag->m_wPID & 0xFF;
    len += 1;
    #ifdef _APP
    return Protocal_ProtocolMakeShort55AA(temp, len, pRetBuf, pRetLen);
    #else
    return FALSE;
    #endif
}

uint8_t Protocal_CmdProcess_SetDevMac(uint16_t CmdLen, uint8_t *pCmdBuf, uint8_t *pRetBuf, uint16_t *pRetLen)
{

    uint8_t temp[20];
    uint8_t len;
    //UPDATE_MANAGER *pUpMag = &g_sUpdateMag;

    if(CmdLen < 9)
        return FALSE;

    len = 0;
    temp[len] = PROTOCAL_CMD_SET_MAC;
    len += 1;
    temp[len] = 0x0;
    len += 1;


    if(TRUE == IsParamValid(&pCmdBuf[1], 8))
    {
        IDParam_SetWMac(&pCmdBuf[1], 8);
    }

    return Protocal_ProtocolMakeShort55AA(temp, len, pRetBuf, pRetLen);

}

static uint8_t Protocal_CmdProcess_DOWNLOAD_START(uint16_t CmdLen, uint8_t *pCmdBuf, uint8_t *pRetBuf, uint16_t *pRetLen)
{
    uint8_t temp[20];
    uint8_t len;
    uint32_t FileSize, FileCheckSum;


    if(CmdLen < 9)
        return FALSE;

    memcpy((u8 *)&FileSize, &pCmdBuf[1], 4);
    memcpy((u8 *)&FileCheckSum, &pCmdBuf[5], 4);
    g_sUpdateMag.FileSize = FileSize;
    g_sUpdateMag.FileCheckSum = FileCheckSum;

    StartDownloadProcess(&g_sUpdateMag);

    len = 0;
    temp[len] = PROTOCAL_CMD_DOWNLOAD_START;
    len += 1;
    temp[len] = 0x0;
    len += 1;
    return Protocal_ProtocolMakeShort55AA(temp, len, pRetBuf, pRetLen);
}


static uint8_t SaveLastUpdateFrame(UPDATE_MANAGER *pUpmag)
{
    volatile u32 DataLen;

    assert_param(pUpmag != NULL);

    DataLen = pUpmag->byLastFrameLen - 3;

    if((pUpmag->PageOffset + DataLen) > (sizeof(pUpmag->PageBuffer)))
        return FALSE;

    memcpy(&pUpmag->PageBuffer[pUpmag->PageOffset], &pUpmag->byLastFrame[3], DataLen);
    pUpmag->PageOffset += DataLen;
    pUpmag->FileDownloadSize += DataLen;
    pUpmag->byLastFrameLen = 0;
    return TRUE;
}

static uint16_t GetLastUpdateFrameIndex(UPDATE_MANAGER *pUpmag)
{
    assert_param(pUpmag != NULL);

    return pUpmag->byLastFrame[1] | (pUpmag->byLastFrame[2] << 8);
}

static uint8_t Protocal_CmdProcess_DOWNLOAD_END(uint16_t CmdLen, uint8_t *pCmdBuf, uint8_t *pRetBuf, uint16_t *pRetLen)
{
    uint8_t temp[100];
    uint8_t len;
    uint32_t FileSize, FileCheckSum;


    if(CmdLen < 9)
        return FALSE;

    if(TRUE == g_sUpdateMag.IsDownloadStart)
    {

        SaveLastUpdateFrame(&g_sUpdateMag);

        memcpy((u8 *)&FileSize, &pCmdBuf[1], 4);
        memcpy((u8 *)&FileCheckSum, &pCmdBuf[5], 4);
        g_sUpdateMag.FileSize = FileSize;
        g_sUpdateMag.FileCheckSum = FileCheckSum;
        //DebugPrintf("Download FileSize:0x%08X\n\r",g_sSysMag.UpdateMag.FileDownloadSize);
        g_sUpdateMag.IsDownloadEnd = TRUE;
        //DebugPrintf("Update App(FileSize:0x%08X,FileCheckSum:0x%08X) End\n\r",FileSize,FileCheckSum);

        return FALSE;//返回在StopDownloadProcess
    }
    else
    {
        len = 0;
        temp[len] = pCmdBuf[0];
        len += 1;
        temp[len] = 0x01; // 1-下载失败
        len += 1;
        return Protocal_ProtocolMakeShort55AA(temp, len, pRetBuf, pRetLen);
    }
}

static uint8_t Protocal_CmdProcess_Ret(u8 Cmd, uint8_t *pRetBuf, uint16_t *pRetLen)
{
    uint8_t temp[20] = {0};
    uint8_t len;

    len = 0;
    temp[len] = Cmd;
    len += 1;
    return Protocal_ProtocolMakeShort55AA(temp, len, pRetBuf, pRetLen);
}

static uint8_t Protocal_CmdProcess_DOWNLOAD(uint16_t CmdLen, uint8_t *pCmdBuf, uint8_t *pRetBuf, uint16_t *pRetLen)
{
    uint8_t temp[100];
    uint8_t len;
    uint16_t FrameIndex, LastFrameIndex;
    uint32_t PageSize;
    UPDATE_MANAGER *pUpmag = &g_sUpdateMag;

    if(CmdLen < 3)
        return FALSE;


    FrameIndex = pCmdBuf[1] | (pCmdBuf[2] << 8);


    //若前次报文已缓存
    if(pUpmag->byLastFrameLen > 0)
    {
        LastFrameIndex = GetLastUpdateFrameIndex(pUpmag);

        //且本次报文和前次报文不一致，则缓存上次报文
        if(FrameIndex != LastFrameIndex)
        {
            if(FALSE == SaveLastUpdateFrame(pUpmag))
            {
                return FALSE;
            }
        }
    }

    memcpy(&pUpmag->byLastFrame, pCmdBuf, CmdLen);
    pUpmag->byLastFrameLen = CmdLen;

    len = 0;
    temp[len] = PROTOCAL_CMD_DOWNLOAD;
    len += 1;

    PageSize = (TRUE == pUpmag->IsDownloadInExt ? g_sExtFlashMag.m_wPageSize : g_sIFlashMag.m_wPageSize);

    if(pUpmag->PageOffset < PageSize)
    {
        return Protocal_CmdProcess_Ret(PROTOCAL_CMD_DOWNLOAD, pRetBuf, pRetLen);
    }

    return FALSE;
}

static uint8_t Protocal_CmdProcess_RESET_REPLACE_ADDR(uint8_t CmdLen, uint8_t *pCmdBuf, uint8_t *pRetBuf, uint16_t *pRetLen)
{
    uint8_t temp[100];
    uint8_t len, ret = 0;
    uint32_t ReplaceAddr = 0;


    memcpy((u8 *)&ReplaceAddr, &pCmdBuf[1], 4);


    if(ReplaceAddr >= g_sExtFlashMag.m_dwBaseAddr)
    {
        g_sUpdateMag.IsDownloadInExt = TRUE;
        g_sUpdateMag.DownloadAddr = ReplaceAddr;
        g_sUpdateMag.FlashAddr = ReplaceAddr;
    }
    else if((ReplaceAddr < (FLASH_BOOT_START + FLASH_BOOT_SIZE) )
            || (ReplaceAddr > g_sIFlashMag.m_dwEndAddr))
    {
        ret = 1;
    }
    else
    {
        g_sUpdateMag.ReplaceAddr = ReplaceAddr;
    }

    len = 0;
    temp[len] = pCmdBuf[0];
    len += 1;
    temp[len] = ret;
    len += 1;
    return Protocal_ProtocolMakeShort55AA(temp, len, pRetBuf, pRetLen);

}

static uint8_t Protocal_CmdProcess_RESET_DOWNLOAD_ADDR(uint16_t CmdLen, uint8_t *pCmdBuf, uint8_t *pRetBuf, uint16_t *pRetLen)
{
    uint8_t temp[100];
    uint8_t len, ret = 0;
    uint32_t Addr = 0;


    memcpy((u8 *)&Addr, &pCmdBuf[1], 4);

    //若下载数据地址在外部存储区
    if(Addr >= g_sExtFlashMag.m_dwBaseAddr)
    {
        g_sUpdateMag.IsDownloadInExt = TRUE;
        g_sUpdateMag.DownloadAddr = Addr;
        g_sUpdateMag.FlashAddr = Addr;
    }
    else if((Addr < (FLASH_BOOT_START + FLASH_BOOT_SIZE) )
            || (Addr > g_sIFlashMag.m_dwEndAddr))
    {
        ret = 1;
    }
    else
    {
        g_sUpdateMag.DownloadAddr = Addr;
    }

    len = 0;
    temp[len] = pCmdBuf[0];
    len += 1;
    temp[len] = ret;
    len += 1;
    return Protocal_ProtocolMakeShort55AA(temp, len, pRetBuf, pRetLen);

}


#if 0
/*
	1字节  串口配置

	1字节  串口号

	4字节  波特率 (高位在前)

	1字节  校验位 'N' 'n'    	（无效）

	1字节  数据位 8		（无效）

	1字节  停止位 1		（无效）
*/
uint8_t Protocal_CmdProcess_FrdComConfig(uint8_t *pCmdBuf, uint8_t CmdLen, uint8_t *pRetBuf, uint16_t *pRetLen)
{
    uint8_t ComIdx;
    uint32_t ComBd;
    UART_CONFIG Config = {0};
    UART_MANAGER *pUartMag = NULL;

    //参数不全
    if(CmdLen < 6)
    {
        return FALSE;
    }

    //校验数据有效性
    ComIdx = pCmdBuf[1];
    ComBd =  pCmdBuf[2] << 24 | pCmdBuf[3] << 16 | pCmdBuf[4] << 8 | pCmdBuf[5];

    if(ComIdx > COM1 && ComIdx <= COM_MAX)
    {

        pUartMag = &g_sUartSetMag.Com[ComIdx];

        memcpy((void *)&Config, (void *)&g_scUartDefConfig[ComIdx], sizeof(UART_CONFIG));
        //对应串口配置
        Config.m_dwBaudRate = ComBd;

        Uart_ComConfiguration(Config, pUartMag);

        //原样返回
        return Protocal_ProtocolMakeShort55AA(pCmdBuf, CmdLen, pRetBuf, pRetLen);

    }

    return FALSE;
}

/*
	1字节    命令

	1字节  串口号

	1字节  开关标记    0-禁用 1-使能
*/
uint8_t Protocal_CmdProcess_FrdComEnable(uint8_t *pCmdBuf, uint8_t CmdLen, uint8_t *pRetBuf, uint16_t *pRetLen)
{
    uint8_t ComIdx;
    UART_MANAGER *pUartMag = NULL;

    //参数不全
    if(CmdLen < 3)
    {
        return FALSE;
    }

    //校验数据有效性
    ComIdx = pCmdBuf[1];

    if(ComIdx > COM1 && ComIdx <= COM_MAX)
    {

        pUartMag = &g_sUartSetMag.Com[ComIdx];

        if(0 == pCmdBuf[2])
        {
            Uart_ComDeInit(pUartMag);
        }
        else
        {
            Uart_ComInit(pUartMag);
        }

        //原样返回
        return Protocal_ProtocolMakeShort55AA(pCmdBuf, CmdLen, pRetBuf, pRetLen);

    }

    return FALSE;
}

#endif


uint32_t Protocal_UpgradeProcess(uint8_t *pCmdBuf, uint16_t CmdLen, uint8_t *pRetBuf, uint16_t *pRetLen)
{

    uint8_t Ret = FALSE;

    switch(pCmdBuf[0])
    {
        case PROTOCAL_CMD_GET_MAC:	//获取设备MAC地址
            Ret = Protocal_CmdProcess_GetDevMac(CmdLen, pCmdBuf, pRetBuf, pRetLen);
            break;

        case PROTOCAL_CMD_GET_APPVER:	//获取应用程序版本号
            Ret = Protocal_CmdProcess_GetAppVersion(CmdLen, pCmdBuf, pRetBuf, pRetLen);
            break;

        case PROTOCAL_CMD_GET_DEVID:	//获取设备身份信息（仅限APP）
            Ret = Protocal_CmdProcess_GetDevID(CmdLen, pCmdBuf, pRetBuf, pRetLen);
            break;

        case PROTOCAL_CMD_SET_MAC:	//设置MAC地址
            Ret = Protocal_CmdProcess_SetDevMac(CmdLen, pCmdBuf, pRetBuf, pRetLen);
            break;


        case PROTOCAL_CMD_DOWNLOAD_START:   //开始升级
            Ret = Protocal_CmdProcess_DOWNLOAD_START(CmdLen, pCmdBuf, pRetBuf, pRetLen);
            break;

        case PROTOCAL_CMD_DOWNLOAD_END:     //升级结束
            Ret = Protocal_CmdProcess_DOWNLOAD_END(CmdLen, pCmdBuf, pRetBuf, pRetLen);
            break;

        case PROTOCAL_CMD_DOWNLOAD:     //应用程序下载
            Ret = Protocal_CmdProcess_DOWNLOAD(CmdLen, pCmdBuf, pRetBuf, pRetLen);
            break;

        case PROTOCAL_CMD_RESET_REPLACE_ADDR:   //重置替换地址（默认应用起始地址）
            Ret = Protocal_CmdProcess_RESET_REPLACE_ADDR(CmdLen, pCmdBuf, pRetBuf, pRetLen);
            break;

        case PROTOCAL_CMD_RESET_DOWNLOAD_ADDR:
            Ret = Protocal_CmdProcess_RESET_DOWNLOAD_ADDR(CmdLen, pCmdBuf, pRetBuf, pRetLen);
            break;

        case PROTOCAL_CMD_RESTART:  //重启
            //while(1);
            NVIC_SystemReset();
            break;

        default:
            break;
    }

    return Ret;
}

void FlashWrite(uint8_t IsExt, uint32_t WriteAddr, uint8_t *pWriteBuf, uint32_t Size)
{
    if(IsExt)
    {
        ExtFlash_Write(&g_sExtFlashMag, WriteAddr, pWriteBuf, Size) ;
    }
    else
    {
        IFlash_Write(&g_sIFlashMag, WriteAddr, pWriteBuf, Size) ;
    }
}

uint32_t FlashGetSum(uint8_t IsExt, uint32_t StartAddr, uint32_t Size)
{
    uint32_t Sum;

    if(IsExt)
    {
        Sum = ExtFlash_GetFlashCheckSum(&g_sExtFlashMag, StartAddr, Size) ;
    }
    else
    {
        Sum = IFlash_GetFlashCheckSum(&g_sIFlashMag, StartAddr, Size) ;
    }

    return Sum;
}


/****************************************************************
@FUNCTION：升级进程初始化
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 初始化g_sUpdateMag
****************************************************************/
extern void Update_MagInit(void)
{
    UPDATE_MANAGER *pMag = &g_sUpdateMag;

    memset(pMag, 0, sizeof(UPDATE_MANAGER));
    pMag->IsUpload = FALSE;
    pMag->IsGetFileSize = FALSE;
    pMag->IsGetFileCheckSum = FALSE;
    pMag->IsDownloadStart = FALSE;
    pMag->IsDownloadEnd = FALSE;
    pMag->ReplaceAddr = FLASH_APP_START;
    pMag->DownloadAddr = FLASH_APP_UPDATE_START;
}

extern void StartUploadeProcess(UPDATE_MANAGER *pMag)
{
    pMag->IsUpload = TRUE;
    pMag->FileUploadSize = 0;
}

extern void StartDownloadProcess(UPDATE_MANAGER *pMag)
{
    pMag->IsDownloadStart = TRUE;
    pMag->IsDownloadEnd = FALSE;
    pMag->FileDownloadSize = 0;

    pMag->PageOffset = 0;
    //pMag->ProgramStep = 0;
    pMag->FlashAddr = pMag->DownloadAddr;
    pMag->FileDownloadSize = 0;

    pMag->byLastFrameLen = 0;
}


extern void StopDownloadProcess(UPDATE_MANAGER *pMag)
{
    uint8_t temp[20];
    uint8_t len;
    uint8_t SendBuf[30];
    uint16_t SendLen;
    uint32_t CheckSum;

    CheckSum = FlashGetSum(pMag->IsDownloadInExt, pMag->DownloadAddr, pMag->FileSize);

    pMag->IsDownloadStart = FALSE;
    pMag->IsDownloadEnd = FALSE;
    pMag->ProgramStep = 0;

    len = 0;
    temp[len] = PROTOCAL_CMD_DOWNLOAD_END;
    len += 1;
    temp[len] = ((CheckSum == pMag->FileCheckSum) ? 0x0 : 0x1); //0-下载成功 1-下载失败
    len += 1;
    SendLen = sizeof(SendBuf);

    if(TRUE == Protocal_ProtocolMakeShort55AA(temp, len, SendBuf, &SendLen))
    {
        pMag->write(SendBuf, SendLen);
    }

    IWDG_Feed();

    //若数据直接下载到APP取或者外部存储区，则无需拷贝数据
    if(pMag->DownloadAddr == FLASH_APP_START
            || pMag->DownloadAddr >= g_sExtFlashMag.m_dwBaseAddr)
    {
        return;
    }

    //下载成功
    if(CheckSum == pMag->FileCheckSum)
    {
        //保存文件长度和校验和
        len = 0;
        *(uint32_t *)&temp[len] = pMag->FileSize;
        len += 4;
        *(uint32_t *)&temp[len] = pMag->FileCheckSum;
        len += 4;
        *(uint32_t *)&temp[len] = pMag->ReplaceAddr;
        len += 4;
        *(uint32_t *)&temp[len] = pMag->DownloadAddr;
        len += 4;
        IFlash_Write(&g_sIFlashMag, FLASH_UPDATE_ADDR, (uint8_t *)&temp, len);
    }
    else
    {

    }
}

/****************************************************************
@FUNCTION：升级进程协议解析，甄别是否为升级包
@INPUT：pBuf pLen - 报文
@OUTPUT：pIsCutted - 报文是否完整
@RETURN: TRUE - 成功FALSE - 失败
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint32_t Update_ProtocalParse(const uint8_t *pBuf, uint16_t *pLen, uint32_t *pIsCutted, uint32_t *pIsLong55AA)
{
    return Protocal_Find55AAFrame(pBuf, pLen, pIsCutted, pIsLong55AA, 2052 - 4);
}



extern void DownloadProcess_Sync(UPDATE_MANAGER *pMag)
{
    uint32_t i, PageSize;
    uint8_t SendBuf[30] = {0};
    uint16_t SendLen;

    if(pMag->IsDownloadStart != TRUE)
    {
        return;
    }

    PageSize = (TRUE == pMag->IsDownloadInExt ? g_sExtFlashMag.m_wPageSize : g_sIFlashMag.m_wPageSize);

    //可能单个报文的大小超过最小擦除单元的N倍
    if(pMag->PageOffset >= PageSize)
    {
        i = 0;

        // 1.写入N 页
        while(pMag->PageOffset >= PageSize)
        {
            FlashWrite(pMag->IsDownloadInExt, pMag->FlashAddr, &pMag->PageBuffer[i], PageSize);

            pMag->FlashAddr += PageSize;
            pMag->PageOffset -= PageSize;
            i += PageSize;

        }

        // 2 拷贝剩余数据到缓存最前面
        if(pMag->PageOffset )//% PageSize
        {
            memcpy(pMag->PageBuffer, &pMag->PageBuffer[i], pMag->PageOffset);
        }

        SendLen = sizeof(SendBuf);

        if(TRUE == Protocal_CmdProcess_Ret(PROTOCAL_CMD_DOWNLOAD, SendBuf, &SendLen))
        {
            pMag->write(SendBuf, SendLen);
        }
    }


    // 若升级结束，最后一包,写入存储
    if(pMag->IsDownloadEnd == TRUE)
    {
        FlashWrite(pMag->IsDownloadInExt, pMag->FlashAddr, pMag->PageBuffer, pMag->PageOffset);

        pMag->FlashAddr += PageSize;
        pMag->PageOffset = 0;

        StopDownloadProcess(pMag);
    }
}

/****************************************************************
@FUNCTION：升级进程协议处理
@INPUT：pBSMag-后台结构体指针
@OUTPUT：pCmdBuf  CmdLen - 返回报文
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Update_ProtocalProcess(uint8_t *pCmdBuf, uint16_t CmdLen)
{
    uint8_t SendBuf[50] = {0};
    uint16_t SendLen;


    SendLen = sizeof(SendBuf);

    if(TRUE == Protocal_UpgradeProcess(pCmdBuf, CmdLen, SendBuf, &SendLen))
    {
        g_sUpdateMag.write(SendBuf, SendLen);
    }

    DownloadProcess_Sync(&g_sUpdateMag);
}


/****************************************************************
@FUNCTION：升级进程设置串口发送函数
@INPUT：无
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
extern void Update_ConfigSendFunc(uint32_t (*write)(uint8_t *, uint16_t ))
{
    g_sUpdateMag.write = write;
}


/****************************************************************
@FUNCTION：升级进程是否正在进行中
@INPUT：无
@OUTPUT：无
@RETURN:  TRUE - 正在进行中 FALSE -不在
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
extern uint32_t IsUpdateStart(void)
{
    return g_sUpdateMag.IsDownloadStart;
}

/****************************************************************
@FUNCTION：升级进程设置设备通讯MAC地址
@INPUT：无
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Update_SetDevMac(uint8_t *pDevMac, uint8_t DevMacLen)
{
    UPDATE_MANAGER *pUpMag = &g_sUpdateMag;

    if(pDevMac && DevMacLen <= sizeof(pUpMag->m_aDevMac))
    {
        memcpy(pUpMag->m_aDevMac, pDevMac, DevMacLen);
    }
}


/****************************************************************
@FUNCTION：升级进程设置设备实体类型
@INPUT：无
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Update_SetDevTType(uint8_t DevTType)
{
    UPDATE_MANAGER *pUpMag = &g_sUpdateMag;

    pUpMag->m_byTType = DevTType;
}

/****************************************************************
@FUNCTION：升级进程设置设备实体子类型
@INPUT：无
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Update_SetDevTSubType(uint8_t DevTSubType)
{
    UPDATE_MANAGER *pUpMag = &g_sUpdateMag;

    pUpMag->m_byTSubType = DevTSubType;
}


/****************************************************************
@FUNCTION：升级进程设置设备单位代字
@INPUT：无
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Update_SetDevRID(uint8_t DevRID)
{
    UPDATE_MANAGER *pUpMag = &g_sUpdateMag;

    pUpMag->m_byRID = DevRID;
}

/****************************************************************
@FUNCTION：升级进程设置设备编号
@INPUT：无
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Update_SetDevPID(uint8_t DevPID)
{
    UPDATE_MANAGER *pUpMag = &g_sUpdateMag;

    pUpMag->m_wPID = DevPID;
}


//该函数阻塞时间较长
extern u32 DataReplace(u32 NewFileSize)
{
    uint32_t i, AddrEnd;
    volatile uint32_t dwData;

    if(NewFileSize > 0)
    {
        AddrEnd = g_sUpdateMag.ReplaceAddr + NewFileSize;

        for(i = 0; i < NewFileSize; i += g_sIFlashMag.m_wPageSize)
        {
            IFlash_Write(&g_sIFlashMag, g_sUpdateMag.ReplaceAddr + i, (uint8_t *)(g_sUpdateMag.DownloadAddr + i), g_sIFlashMag.m_wPageSize);
            IWDG_Feed();
        }

        return TRUE;
    }

    return FALSE;
}


extern u32 GetNewDataFileSize(void)
{
    u32 temp;

    IFlash_Read(&g_sIFlashMag, FLASH_UPDATE_ADDR, (u8 *)&temp, 4);

    return temp;
}

extern u32 GetNewDataFileCheck(void)
{
    u32 temp;

    IFlash_Read(&g_sIFlashMag, FLASH_UPDATE_ADDR + 4, (u8 *)&temp, 4);

    return temp;
}

//获取新数据实际放置地址
extern u32 GetNewDataReplaceAddr(void)
{
    u32 temp;

    IFlash_Read(&g_sIFlashMag, FLASH_UPDATE_ADDR + 8, (u8 *)&temp, 4);

    return temp;
}

//获取新数据缓存地址
extern u32 GetNewDataDownloadAddr(void)
{
    u32 temp;

    IFlash_Read(&g_sIFlashMag, FLASH_UPDATE_ADDR + 12, (u8 *)&temp, 4);

    return temp;
}

extern void EraseNewData(void)
{
    IFlash_PageErase(&g_sIFlashMag, FLASH_UPDATE_ADDR);
}

extern uint32_t CheckNewData(void)
{
    uint32_t CheckSum;
    uint32_t FileSize, FileChkSum, ReplaceAddr, DownloadAddr;

    FileSize = GetNewDataFileSize();

    if(FileSize == 0 || FileSize == 0xFFFFFFFFUL
            || (FileSize > g_sIFlashMag.m_dwCapacity * 1024))
    {
        return FALSE;
    }

    DownloadAddr = GetNewDataDownloadAddr();

    if((DownloadAddr < FLASH_APP_START )
            || (DownloadAddr >= g_sIFlashMag.m_dwEndAddr))
    {
        g_sUpdateMag.DownloadAddr = FLASH_APP_UPDATE_START;
    }
    else
    {
        g_sUpdateMag.DownloadAddr = DownloadAddr;
    }

    FileChkSum = GetNewDataFileCheck();

    CheckSum = Protocal_MakeU32Sum((uint8_t *)g_sUpdateMag.DownloadAddr, FileSize);

    if(FileChkSum != CheckSum)
    {
        return FALSE;
    }

    ReplaceAddr = GetNewDataReplaceAddr();

    if((ReplaceAddr == 0xFFFFFFFF)
            || (ReplaceAddr < FLASH_APP_START)
            || (ReplaceAddr >= g_sIFlashMag.m_dwEndAddr))
    {
        g_sUpdateMag.ReplaceAddr = FLASH_APP_START;
    }
    else
    {
        g_sUpdateMag.ReplaceAddr = ReplaceAddr;
    }


    if(TRUE != DataReplace(FileSize))
    {
        return FALSE;
    }

    return TRUE;
}

void JumpToApp(void)
{
    PTRFUNC Jump_To_Application;
    uint32_t JumpAddress;

    //检测栈指针
    if (((*(__IO uint32_t*)FLASH_APP_START) & 0x2FFE0000 ) == 0x20000000)
    {
        /* Jump to user application */
        JumpAddress = *(__IO uint32_t*) (FLASH_APP_START + 4);
        Jump_To_Application = (PTRFUNC) JumpAddress;
        /* Initialize user application's Stack Pointer */
        __set_MSP(*(__IO uint32_t*) FLASH_APP_START);
        IWDG_Feed();
        Jump_To_Application();
    }

}



