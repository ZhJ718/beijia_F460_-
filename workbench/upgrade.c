/******************************************************************
*��Ȩ���� (C)2023, ���ջ������Ƽ����޹�˾
*
*�ļ�����:  upgrade.c
*�ļ���ʶ:
*����ժҪ:
*����˵��:
*��ǰ�汾:
*��    ��:  xfw
*�������:  2023/2/27
*
*�޸ļ�¼1:
*    �޸�����:
*    �� �� ��:
*    �� �� ��:
*    �޸�����:
******************************************************************/

/******************************************************************
*                             ͷ�ļ�                              *
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

        return FALSE;//������StopDownloadProcess
    }
    else
    {
        len = 0;
        temp[len] = pCmdBuf[0];
        len += 1;
        temp[len] = 0x01; // 1-����ʧ��
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


    //��ǰ�α����ѻ���
    if(pUpmag->byLastFrameLen > 0)
    {
        LastFrameIndex = GetLastUpdateFrameIndex(pUpmag);

        //�ұ��α��ĺ�ǰ�α��Ĳ�һ�£��򻺴��ϴα���
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

    //���������ݵ�ַ���ⲿ�洢��
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
	1�ֽ�  ��������

	1�ֽ�  ���ں�

	4�ֽ�  ������ (��λ��ǰ)

	1�ֽ�  У��λ 'N' 'n'    	����Ч��

	1�ֽ�  ����λ 8		����Ч��

	1�ֽ�  ֹͣλ 1		����Ч��
*/
uint8_t Protocal_CmdProcess_FrdComConfig(uint8_t *pCmdBuf, uint8_t CmdLen, uint8_t *pRetBuf, uint16_t *pRetLen)
{
    uint8_t ComIdx;
    uint32_t ComBd;
    UART_CONFIG Config = {0};
    UART_MANAGER *pUartMag = NULL;

    //������ȫ
    if(CmdLen < 6)
    {
        return FALSE;
    }

    //У��������Ч��
    ComIdx = pCmdBuf[1];
    ComBd =  pCmdBuf[2] << 24 | pCmdBuf[3] << 16 | pCmdBuf[4] << 8 | pCmdBuf[5];

    if(ComIdx > COM1 && ComIdx <= COM_MAX)
    {

        pUartMag = &g_sUartSetMag.Com[ComIdx];

        memcpy((void *)&Config, (void *)&g_scUartDefConfig[ComIdx], sizeof(UART_CONFIG));
        //��Ӧ��������
        Config.m_dwBaudRate = ComBd;

        Uart_ComConfiguration(Config, pUartMag);

        //ԭ������
        return Protocal_ProtocolMakeShort55AA(pCmdBuf, CmdLen, pRetBuf, pRetLen);

    }

    return FALSE;
}

/*
	1�ֽ�    ����

	1�ֽ�  ���ں�

	1�ֽ�  ���ر��    0-���� 1-ʹ��
*/
uint8_t Protocal_CmdProcess_FrdComEnable(uint8_t *pCmdBuf, uint8_t CmdLen, uint8_t *pRetBuf, uint16_t *pRetLen)
{
    uint8_t ComIdx;
    UART_MANAGER *pUartMag = NULL;

    //������ȫ
    if(CmdLen < 3)
    {
        return FALSE;
    }

    //У��������Ч��
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

        //ԭ������
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
        case PROTOCAL_CMD_GET_MAC:	//��ȡ�豸MAC��ַ
            Ret = Protocal_CmdProcess_GetDevMac(CmdLen, pCmdBuf, pRetBuf, pRetLen);
            break;

        case PROTOCAL_CMD_GET_APPVER:	//��ȡӦ�ó���汾��
            Ret = Protocal_CmdProcess_GetAppVersion(CmdLen, pCmdBuf, pRetBuf, pRetLen);
            break;

        case PROTOCAL_CMD_GET_DEVID:	//��ȡ�豸�����Ϣ������APP��
            Ret = Protocal_CmdProcess_GetDevID(CmdLen, pCmdBuf, pRetBuf, pRetLen);
            break;

        case PROTOCAL_CMD_SET_MAC:	//����MAC��ַ
            Ret = Protocal_CmdProcess_SetDevMac(CmdLen, pCmdBuf, pRetBuf, pRetLen);
            break;


        case PROTOCAL_CMD_DOWNLOAD_START:   //��ʼ����
            Ret = Protocal_CmdProcess_DOWNLOAD_START(CmdLen, pCmdBuf, pRetBuf, pRetLen);
            break;

        case PROTOCAL_CMD_DOWNLOAD_END:     //��������
            Ret = Protocal_CmdProcess_DOWNLOAD_END(CmdLen, pCmdBuf, pRetBuf, pRetLen);
            break;

        case PROTOCAL_CMD_DOWNLOAD:     //Ӧ�ó�������
            Ret = Protocal_CmdProcess_DOWNLOAD(CmdLen, pCmdBuf, pRetBuf, pRetLen);
            break;

        case PROTOCAL_CMD_RESET_REPLACE_ADDR:   //�����滻��ַ��Ĭ��Ӧ����ʼ��ַ��
            Ret = Protocal_CmdProcess_RESET_REPLACE_ADDR(CmdLen, pCmdBuf, pRetBuf, pRetLen);
            break;

        case PROTOCAL_CMD_RESET_DOWNLOAD_ADDR:
            Ret = Protocal_CmdProcess_RESET_DOWNLOAD_ADDR(CmdLen, pCmdBuf, pRetBuf, pRetLen);
            break;

        case PROTOCAL_CMD_RESTART:  //����
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
@FUNCTION���������̳�ʼ��
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��ʼ��g_sUpdateMag
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
    temp[len] = ((CheckSum == pMag->FileCheckSum) ? 0x0 : 0x1); //0-���سɹ� 1-����ʧ��
    len += 1;
    SendLen = sizeof(SendBuf);

    if(TRUE == Protocal_ProtocolMakeShort55AA(temp, len, SendBuf, &SendLen))
    {
        pMag->write(SendBuf, SendLen);
    }

    IWDG_Feed();

    //������ֱ�����ص�APPȡ�����ⲿ�洢���������追������
    if(pMag->DownloadAddr == FLASH_APP_START
            || pMag->DownloadAddr >= g_sExtFlashMag.m_dwBaseAddr)
    {
        return;
    }

    //���سɹ�
    if(CheckSum == pMag->FileCheckSum)
    {
        //�����ļ����Ⱥ�У���
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
@FUNCTION����������Э�����������Ƿ�Ϊ������
@INPUT��pBuf pLen - ����
@OUTPUT��pIsCutted - �����Ƿ�����
@RETURN: TRUE - �ɹ�FALSE - ʧ��
@AUTHOR��xfw
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

    //���ܵ������ĵĴ�С������С������Ԫ��N��
    if(pMag->PageOffset >= PageSize)
    {
        i = 0;

        // 1.д��N ҳ
        while(pMag->PageOffset >= PageSize)
        {
            FlashWrite(pMag->IsDownloadInExt, pMag->FlashAddr, &pMag->PageBuffer[i], PageSize);

            pMag->FlashAddr += PageSize;
            pMag->PageOffset -= PageSize;
            i += PageSize;

        }

        // 2 ����ʣ�����ݵ�������ǰ��
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


    // ���������������һ��,д��洢
    if(pMag->IsDownloadEnd == TRUE)
    {
        FlashWrite(pMag->IsDownloadInExt, pMag->FlashAddr, pMag->PageBuffer, pMag->PageOffset);

        pMag->FlashAddr += PageSize;
        pMag->PageOffset = 0;

        StopDownloadProcess(pMag);
    }
}

/****************************************************************
@FUNCTION����������Э�鴦��
@INPUT��pBSMag-��̨�ṹ��ָ��
@OUTPUT��pCmdBuf  CmdLen - ���ر���
@RETURN: ��
@AUTHOR��xfw
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
@FUNCTION�������������ô��ڷ��ͺ���
@INPUT����
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern void Update_ConfigSendFunc(uint32_t (*write)(uint8_t *, uint16_t ))
{
    g_sUpdateMag.write = write;
}


/****************************************************************
@FUNCTION�����������Ƿ����ڽ�����
@INPUT����
@OUTPUT����
@RETURN:  TRUE - ���ڽ����� FALSE -����
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern uint32_t IsUpdateStart(void)
{
    return g_sUpdateMag.IsDownloadStart;
}

/****************************************************************
@FUNCTION���������������豸ͨѶMAC��ַ
@INPUT����
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
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
@FUNCTION���������������豸ʵ������
@INPUT����
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Update_SetDevTType(uint8_t DevTType)
{
    UPDATE_MANAGER *pUpMag = &g_sUpdateMag;

    pUpMag->m_byTType = DevTType;
}

/****************************************************************
@FUNCTION���������������豸ʵ��������
@INPUT����
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Update_SetDevTSubType(uint8_t DevTSubType)
{
    UPDATE_MANAGER *pUpMag = &g_sUpdateMag;

    pUpMag->m_byTSubType = DevTSubType;
}


/****************************************************************
@FUNCTION���������������豸��λ����
@INPUT����
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Update_SetDevRID(uint8_t DevRID)
{
    UPDATE_MANAGER *pUpMag = &g_sUpdateMag;

    pUpMag->m_byRID = DevRID;
}

/****************************************************************
@FUNCTION���������������豸���
@INPUT����
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Update_SetDevPID(uint8_t DevPID)
{
    UPDATE_MANAGER *pUpMag = &g_sUpdateMag;

    pUpMag->m_wPID = DevPID;
}


//�ú�������ʱ��ϳ�
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

//��ȡ������ʵ�ʷ��õ�ַ
extern u32 GetNewDataReplaceAddr(void)
{
    u32 temp;

    IFlash_Read(&g_sIFlashMag, FLASH_UPDATE_ADDR + 8, (u8 *)&temp, 4);

    return temp;
}

//��ȡ�����ݻ����ַ
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

    //���ջָ��
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



