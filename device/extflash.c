/**
  ******************************************************************************
  * @file    extflash.c
  * @author  xfw
  * @brief   This file contains definitions for external flash
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
#include "extflash.h"
#include "malloc.h"
#include "gpio.h"
#include "spi.h"

W25QXX_MAG  g_sW25qxxMag;
FLASH_MAG  g_sFlashMag;
EFLASH_MAG g_sExtFlashMag;


// 4KbytesΪһ��Sector
//16������Ϊ1��Block
//W25Q128
//����Ϊ16M�ֽ�,����128��Block,4096��Sector
//��ʼ��SPI FLASH��IO��
void W25QXX_LowLevelInit(W25QXX_HW_OPERATIONS a_sHwOps, W25QXX_MAG *pMag)
{
    assert_param(pMag != NULL);

    memset(pMag, 0, sizeof(W25QXX_MAG));

    pMag->m_sHwOps.hw_init 	= a_sHwOps.hw_init;
    pMag->m_sHwOps.set_cs 	= a_sHwOps.set_cs;
    pMag->m_sHwOps.reset_cs = a_sHwOps.reset_cs;
    pMag->m_sHwOps.rwbyte 	= a_sHwOps.rwbyte;

    //IO��ʼ�� SPI��ʼ��
    if(pMag->m_sHwOps.hw_init != NULL)
    {
        pMag->m_sHwOps.hw_init();
    }

    //Ƭѡ���ߣ���ѡ��
    if(pMag->m_sHwOps.set_cs != NULL)
    {
        pMag->m_sHwOps.set_cs();
    }

}


void W25QXX_HardWareInit(void)
{
    W25QXX_CS_GpioInit();
    SPI2_Init();		   	//��ʼ��SPI

    #if defined(AT32F4xx)

    SPI2_SetSpeed(SPI_MCLKP_2);//����Ϊ18Mʱ��,����ģʽ

    #elif defined(HC32F46x)
    SPI2_SetSpeed(SpiClkDiv2);
    #endif


}

//��ʼ��SPI FLASH
void W25QXX_Init(W25QXX_MAG *pMag)
{
    W25QXX_HW_OPERATIONS HwOps =
    {
        .hw_init 	= W25QXX_HardWareInit,
        .set_cs 	= W25QXX_CS_SET,
        .reset_cs 	= W25QXX_CS_RESET,
        .rwbyte 	= SPI2_ReadWriteByte,
    };

    W25QXX_LowLevelInit(HwOps, pMag);
    pMag->m_wW25qxxType = W25QXX_ReadID(pMag->m_sHwOps);//��ȡFLASH ID.
}

//�첽Flashд�룬�����Ƕ�ҳд�룬д��������ر�֤��ͬһҳ
void W25QXX_Write_AsyncProcess(W25QXX_MAG *pMag)
{
    u32 i;
    u8 data;
    u32 WriteAddrOffset;
    u32 WriteAddrEndOffset;

    if((pMag == NULL) || (pMag->m_wNumByteToWrite > W25X_PAGE_SIZE))
        return;

    if(pMag->m_IsStart == FALSE)
        return;

    switch(pMag->m_byWriteStep)
    {
        case 0:
            //pMag->pPageBuffer = W25QXX_BUFFER;
            WriteAddrEndOffset = pMag->m_dwWriteAddr % W25X_PAGE_SIZE + pMag->m_wNumByteToWrite;

            if(WriteAddrEndOffset > W25X_PAGE_SIZE)
                return;

            pMag->m_IsOnLine = ((W25Q32 == W25QXX_ReadID(pMag->m_sHwOps)) ? TRUE : FALSE);

            if(TRUE == pMag->m_IsOnLine)
            {
                pMag->m_IsBusy = TRUE;
                pMag->m_dwPageStartAddr = pMag->m_dwWriteAddr - pMag->m_dwWriteAddr % W25X_PAGE_SIZE;
                //W25QXX_Read(pMag->pPageBuffer,pMag->PageStartAddr,W25X_PAGE_SIZE);//������������������
                WriteAddrOffset = pMag->m_dwWriteAddr % W25X_PAGE_SIZE;

                pMag->m_sHwOps.reset_cs();	 //ʹ������
                pMag->m_sHwOps.rwbyte(W25X_ReadData);         	//���Ͷ�ȡ����
                pMag->m_sHwOps.rwbyte((u8)((pMag->m_dwPageStartAddr) >> 16));  	//����24bit��ַ
                pMag->m_sHwOps.rwbyte((u8)((pMag->m_dwPageStartAddr) >> 8));
                pMag->m_sHwOps.rwbyte((u8)pMag->m_dwPageStartAddr);

                for(i = 0; i < W25X_PAGE_SIZE; i++)
                {
                    data = pMag->m_sHwOps.rwbyte(0XFF);   	//ѭ������

                    if((i < WriteAddrOffset) || (i >= WriteAddrEndOffset))
                    {
                        pMag->m_byWriteBuffer[i] = data;	//������Ҫд������
                    }
                }

                pMag->m_sHwOps.set_cs();	   //ȡ��Ƭѡ
                W25QXX_Write_Enable(pMag->m_sHwOps);                  	//SET WEL
                pMag->m_byWriteStep++;
            }

            break;

        case 1:
            if(!W25QXX_Is_Busy(pMag->m_sHwOps))
            {
                pMag->m_sHwOps.reset_cs();	 //ʹ������
                pMag->m_sHwOps.rwbyte(W25X_PageErase);      	//������������ָ��
                pMag->m_sHwOps.rwbyte((u8)((pMag->m_dwPageStartAddr) >> 16));  	//����24bit��ַ
                pMag->m_sHwOps.rwbyte((u8)((pMag->m_dwPageStartAddr) >> 8));
                pMag->m_sHwOps.rwbyte((u8)pMag->m_dwPageStartAddr);
                pMag->m_sHwOps.set_cs();	   //ȡ��Ƭѡ
                pMag->m_byWriteStep++;
            }

            break;

        case 2:
            if(!W25QXX_Is_Busy(pMag->m_sHwOps))
            {
                W25QXX_Write_Enable(pMag->m_sHwOps);                  	//SET WEL
                pMag->m_sHwOps.reset_cs();	 //ʹ������
                pMag->m_sHwOps.rwbyte(W25X_PageProgram);      	//����дҳ����
                pMag->m_sHwOps.rwbyte((u8)((pMag->m_dwPageStartAddr) >> 16)); 	//����24bit��ַ
                pMag->m_sHwOps.rwbyte((u8)((pMag->m_dwPageStartAddr) >> 8));
                pMag->m_sHwOps.rwbyte((u8)pMag->m_dwPageStartAddr);

                for(i = 0; i < W25X_PAGE_SIZE; i++)
                    pMag->m_sHwOps.rwbyte(pMag->m_byWriteBuffer[i]);//ѭ��д��

                pMag->m_sHwOps.set_cs();	   //ȡ��Ƭѡ
                pMag->m_byWriteStep++;
            }

            break;

        case 3:
            if(!W25QXX_Is_Busy(pMag->m_sHwOps))
            {
                pMag->m_byWriteStep = 0;
                pMag->m_IsBusy = FALSE;
                pMag->m_IsStart = FALSE;
                //pMag->m_IsOnLine = FALSE;
            }

            break;

        default:
            pMag->m_byWriteStep = 0;
            break;
    }
}

#if 0

/*
	ÿ����־�����ⲿFLASHҳ��С���룬��ÿ����־ռһҳ��W25X_PAGE_SIZE�ֽ�
	��־��ʽ:���ݳ���(2Bytes)+GPS��ȡ����UTCʵʱʱ��(7Bytes)+����(�������ΪW25X_PAGE_SIZE-9)
*/

u32 g_dwLogWriteOffset = 0;
u32 g_dwLogReadOffset = 0;

/*
 ************************************************************************************************************************************************************************
 *��������: Log_Write
 *��������: д��־
 *��������: �洢���ݻ����������ݳ���
 *�������: ��
 *��������: ��
 *��������: XFW 20191014
 *ȫ������: g_dwLogWriteOffset-��ǰ�洢��ʼ��ַ��ҳ����
 *����˵��: ��ϰ��ʼ�ű�����־,�洢�豸�������RECORD_INJURY_MAX��ҳ�������������
 ************************************************************************************************************************************************************************
*/
void Log_Write(u8 *pBuf, u32 Len)
{
    u8 Temp[W25X_PAGE_SIZE] = {0};
    u16 TempLen = 0;

    assert_param(pBuf != NULL);

    if((g_sUserPara.m_byMode == GAME_GOING)
            && (Len <= (W25X_PAGE_SIZE - TempLen - 2)))
    {
        //ָʾ���ݱ��ĳ���
        *(u16 *)Temp = Len;
        TempLen += 2;
        //���UTCʱ��
        memcpy(&Temp[TempLen], &g_sGpsMag.m_sNmeaMsg.m_sUtcTime, sizeof(UTC_TIME));
        TempLen += sizeof(g_sGpsMag.m_sNmeaMsg.m_sUtcTime);
        //д������
        memcpy(&Temp[TempLen], pBuf, Len);
        TempLen += Len;

        //�ύ���洢�豸
        if((g_dwLogWriteOffset < (g_sFlashMag.m_dwFlashCapacity - (RECORD_INJURY_MAX * W25X_PAGE_SIZE)))
                && (FALSE != FlashStorageSubmit(&g_sFlashMag, Temp, TempLen, g_dwLogWriteOffset)))
        {
            g_dwLogWriteOffset += W25X_PAGE_SIZE;
        }
    }
}


/*
 ************************************************************************************************************************************************************************
 *��������: Log_PrintInit
 *��������: ��־��ȡ��ʼ��
 *��������: ��
 *�������: ��
 *��������: ��
 *��������: XFW 20191014
 *ȫ������: g_dwLogReadOffset-�洢��ȡ��ַ
 *����˵��: g_dwLogReadOffset��λ
 ************************************************************************************************************************************************************************
*/
void Log_PrintInit(void)
{
    g_dwLogReadOffset = 0;
}


/*
 ************************************************************************************************************************************************************************
 *��������: Log_PrintNext
 *��������: ͨ����̨���ڴ�ӡ��һ����־
 *��������: �����������ָ��
 *�������: ��
 *��������: TRUE-�ɹ� FALSE-ʧ��
 *��������: XFW 20191014
 *ȫ������: g_dwLogReadOffset-�洢��ȡ��ַ
 *����˵��: ��ȡ�ɹ���g_dwLogReadOffsetƫ�Ƶ���һҳ,����ģ�����
 ************************************************************************************************************************************************************************
*/
u32 Log_PrintNext_5AA5AA(uint32_t (*fp)(u8 *, u16))
{
    u8 Temp[W25X_PAGE_SIZE] = {0};
    u16 TempLen;

    W25QXX_Read(g_sFlashMag.pW25QXXMag->m_sHwOps, &Temp[4 - 2], g_dwLogReadOffset, W25X_PAGE_SIZE - 5);

    if(g_dwLogReadOffset < (4 * 1024 * 1024))
    {
        TempLen = *(u16 *)&Temp[4 - 2];
        g_dwLogReadOffset += W25X_PAGE_SIZE;
        Temp[0] = 0x5A;
        Temp[1] = 0xA5;
        Temp[2] = TempLen + 5 + sizeof(UTC_TIME);
        Temp[3] = Cmd_GetLog;
        Temp[4 + TempLen + sizeof(UTC_TIME)] = 0xAA;

        if(UART_SEND_BUFFER_LEN >= (TempLen + sizeof(UTC_TIME) + 5))
        {
            (*fp)(Temp, TempLen + sizeof(UTC_TIME) + 5);
            return TRUE;
        }
    }

    return FALSE;
}

/*
 ************************************************************************************************************************************************************************
 *��������: Log_PrintNext
 *��������: ͨ���������ڴ�ӡ��һ����־
 *��������: �����������ָ��
 *�������: ��
 *��������: TRUE-�ɹ� FALSE-ʧ��
 *��������: XFW 20191014
 *ȫ������: g_dwLogReadOffset-�洢��ȡ��ַ
 *����˵��: ��ȡ�ɹ���g_dwLogReadOffsetƫ�Ƶ���һҳ,����ģ�����
 ************************************************************************************************************************************************************************
*/
u32 Log_PrintNext_5AAA(uint32_t (*fp)(u8 *, u16))
{
    u8 Temp[W25X_PAGE_SIZE] = {0};
    u16 TempLen;

    W25QXX_Read(g_sFlashMag.pW25QXXMag->m_sHwOps, &Temp[5 - 2], g_dwLogReadOffset, W25X_PAGE_SIZE - 6);

    if(g_dwLogReadOffset < (4 * 1024 * 1024))
    {
        TempLen = Temp[6 - 2] << 8 | Temp[5 - 2];
        g_dwLogReadOffset += W25X_PAGE_SIZE;
        Temp[0] = 0x5A;
        Temp[1] = 6 + TempLen + sizeof(UTC_TIME);
        Temp[2] = 0xA7;
        Temp[3] = 0;
        Temp[4] = TempLen + sizeof(UTC_TIME);
        Temp[5 + TempLen + sizeof(UTC_TIME)] = 0xAA;

        if(UART_SEND_BUFFER_LEN >= (TempLen + sizeof(UTC_TIME) + 6))
        {
            (*fp)(Temp, TempLen + sizeof(UTC_TIME) + 6);
            return TRUE;
        }
    }

    return FALSE;
}


/*
 ************************************************************************************************************************************************************************
 *��������: Log_PrintNext
 *��������: ��ӡ��һ����־
 *��������: �����������ָ��
 *�������: ��
 *��������: TRUE-�ɹ� FALSE-ʧ��
 *��������: XFW 20191014
 *ȫ������: g_dwLogReadOffset-�洢��ȡ��ַ
 *����˵��: ��ȡ�ɹ���g_dwLogReadOffsetƫ�Ƶ���һҳ,����ģ�����
 ************************************************************************************************************************************************************************
*/
u32 Log_PrintNext(uint32_t (*fp)(u8 *, u16))
{
    u8 Temp[W25X_PAGE_SIZE] = {0};
    u16 TempLen;

    W25QXX_Read(g_sFlashMag.pW25QXXMag->m_sHwOps, Temp, g_dwLogReadOffset, W25X_PAGE_SIZE);

    if(g_dwLogReadOffset < (4 * 1024 * 1024))
    {
        TempLen = *(u16 *)Temp;
        g_dwLogReadOffset += W25X_PAGE_SIZE;

        if(UART_SEND_BUFFER_LEN >= (TempLen + sizeof(UTC_TIME)))
        {
            (*fp)(&Temp[2], TempLen + sizeof(UTC_TIME));
            return TRUE;
        }
    }

    return FALSE;
}


#endif

//��ȡϵͳ�ⲿFlash��Page��С
uint16_t EXTFLASH_GetPageSize(uint16_t TypeID)
{
    uint16_t PageSize;

    switch(TypeID)
    {
        case W25Q16:
        case W25Q32:
        case W25Q64:
        case W25Q128:
            PageSize = W25X_PAGE_SIZE;
            break;

        default:
            PageSize = 0;
            break;
    }

    return PageSize;
}
//��ȡϵͳ�ⲿFlash�ĳߴ��С(��λ:K)
uint32_t EXTFLASH_GetFlashSize(uint16_t TypeID)
{
    uint32_t FlashSize = 0;

    switch(TypeID)
    {
        case W25Q16:
            FlashSize = 2 * 1024;
            break;

        case W25Q32:
            FlashSize = 4 * 1024;
            break;

        case W25Q64:
            FlashSize = 8 * 1024;
            break;

        case W25Q128:
            FlashSize = 16 * 1024;
            break;

        default:
            FlashSize = 0;
            break;

    }

    return FlashSize;
}

/****************************************************************
@FUNCTION���ⲿFLASH��ʼ��
@INPUT��Ӳ���ӿں����ṹ�壬IFLASH����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �ڲ�FLASH�ڵ�ַ�ռ�Ļ���ַIFLASH_BASE_ADDR
****************************************************************/
extern void ExtFlash_Init(EFLASH_MAG *pMag)
{
    assert_param(pMag != NULL);

    W25QXX_Init(&g_sW25qxxMag);


    memset(pMag, 0, sizeof(EFLASH_MAG));

    #if 0
    pMag->m_sHwOps.hw_init = NULL;
    pMag->m_sHwOps.get_capacity = IFLASH_GetFlashSize_STM32F10X;
    pMag->m_sHwOps.get_page_size = IFLASH_GetPageSize_STM32F10X;
    pMag->m_sHwOps.m_dwStartAddr = IFLASH_BASE_ADDR;

    if(pMag->m_sHwOps.hw_init != NULL)
    {
        pMag->m_sHwOps.hw_init();
    }

    #endif
    pMag->m_wFlashTypeID = g_sW25qxxMag.m_wW25qxxType;
    pMag->m_wPageSize = EXTFLASH_GetPageSize(pMag->m_wFlashTypeID);
    pMag->m_dwCapacity = EXTFLASH_GetFlashSize(pMag->m_wFlashTypeID);
    pMag->m_dwEndAddr = pMag->m_dwBaseAddr = EXT_FLASH_START_ADDR;
    pMag->m_dwEndAddr += 1024 * pMag->m_dwCapacity;

}


/****************************************************************
@FUNCTION���ⲿFLASH��
@INPUT����ȡ��ʼ��ַ����ȡ���ݻ���������ȡ���ݴ�С
@OUTPUT����ȡ����
@RETURN: ʵ�ʶ�ȡ���ݴ�С
@AUTHOR��xfw
@SPECIAL: ����ȡ���ݽ϶࣬���нϴ���ʱ
****************************************************************/
extern uint32_t ExtFlash_Read(EFLASH_MAG *pMag, uint32_t a_RAddr, uint8_t *a_pBuffer, uint32_t a_NumToRead)
{
    uint32_t i = 0;

    assert_param(pMag != NULL);
    assert_param(a_pBuffer != NULL);

    //�жϵ�ַ��Ч��
    if(a_RAddr < pMag->m_dwBaseAddr
            || a_RAddr > pMag->m_dwEndAddr)
    {
        return 0;
    }
		
    if(pMag->m_wPageSize == 0)
    {
        return 0;
    }

    while(a_NumToRead / pMag->m_wPageSize)
    {
        W25QXX_Read(g_sW25qxxMag.m_sHwOps, a_pBuffer, a_RAddr - pMag->m_dwBaseAddr, pMag->m_wPageSize);

        a_NumToRead -= pMag->m_wPageSize;
        a_RAddr += pMag->m_wPageSize;
        a_pBuffer += pMag->m_wPageSize;
        i += pMag->m_wPageSize;
    }

    if(a_NumToRead > 0)
    {
        W25QXX_Read(g_sW25qxxMag.m_sHwOps, a_pBuffer, a_RAddr - pMag->m_dwBaseAddr, a_NumToRead);
        i += a_NumToRead;
    }

    return i;
}

/****************************************************************
@FUNCTION���ⲿFLASHҳ����
@INPUT��ҳ�������ַ
@OUTPUT����
@RETURN: ʵ��ҳ��ʼ��ַ
@AUTHOR��xfw
@SPECIAL: ������ϵͳ
****************************************************************/
extern uint32_t ExtFlash_PageErase(EFLASH_MAG *pMag, uint32_t StartAddr)
{
    uint32_t PageAddr;

    //�жϵ�ַ��Ч��
    if(StartAddr < pMag->m_dwBaseAddr
            || StartAddr > pMag->m_dwEndAddr)
    {
        return 0;
    }

    StartAddr -= pMag->m_dwBaseAddr;
    PageAddr = StartAddr - StartAddr % pMag->m_wPageSize;

    W25QXX_Erase_Page(g_sW25qxxMag.m_sHwOps, PageAddr);

    return PageAddr;
}

/****************************************************************
@FUNCTION���ⲿFLASHд
@INPUT��д����ʼ��ַ��д�����ݣ�д�����ݴ�С
@OUTPUT����
@RETURN: ʵ��д�����ݴ�С
@AUTHOR��xfw
@SPECIAL: ������ϵͳ
****************************************************************/
extern uint32_t ExtFlash_Write(EFLASH_MAG *pMag, uint32_t a_WAddr, uint8_t *a_pBuffer, uint32_t a_NumToWrite)
{
    uint32_t i, PageStartAddr;
    uint16_t PageSize = pMag->m_wPageSize;
    uint8_t *pPageBuf = NULL;
    uint16_t PageOffset;
    uint16_t PageRemain;
    uint32_t Num = a_NumToWrite;

    assert_param(pMag != NULL);
    assert_param(a_pBuffer != NULL);

    if(Num == 0 || PageSize == 0)
    {
        return 0;
    }

    if((a_WAddr >= pMag->m_dwBaseAddr)
            && ((a_WAddr + Num) < pMag->m_dwEndAddr))
		//if(((a_WAddr + Num) < pMag->m_dwEndAddr))
    {

        pPageBuf = (uint8_t *)sdram_malloc(0, PageSize);

        if(pPageBuf == NULL)
        {
            return 0;
        }

        a_WAddr -= pMag->m_dwBaseAddr;

        PageStartAddr = a_WAddr / PageSize;
        PageStartAddr *= PageSize;
        PageOffset = a_WAddr % PageSize;
        PageRemain = PageSize - PageOffset;

        while(1)
        {
            ExtFlash_Read(pMag, PageStartAddr + pMag->m_dwBaseAddr, pPageBuf, PageSize);

            for(i = 0; i < MIN(PageRemain, Num); i++)
            {
                pPageBuf[PageOffset + i] = a_pBuffer[i];
            }

            Num -= i;
            a_pBuffer += i;

            //ExtFlash_PageErase(PageStartAddr + pMag->m_dwBaseAddr);

            W25QXX_Erase_Page(g_sW25qxxMag.m_sHwOps, PageStartAddr);

            W25QXX_Write_Page(g_sW25qxxMag.m_sHwOps, pPageBuf, PageStartAddr, PageSize);

            PageStartAddr += PageSize;

            if(Num == 0)
            {
                break;
            }
            else
            {
                PageOffset = 0;
                PageRemain = PageSize - PageOffset;
            }
        }


        sdram_free(0, pPageBuf);


        return a_NumToWrite;
    }

    return 0;
}


/****************************************************************
@FUNCTION����ȡ�ⲿFLASHָ����С�����У���
@INPUT��Ӳ���ӿں����ṹ�壬��ʼ��ַ�����ݴ�С
@OUTPUT����
@RETURN: У���
@AUTHOR��xfw
@SPECIAL:������ϵͳ
****************************************************************/
extern uint32_t ExtFlash_GetFlashCheckSum(EFLASH_MAG *pMag, uint32_t StartAddr, uint32_t Size)
{
    uint32_t i, Sum;
    uint8_t *pBuf = NULL;


    if((StartAddr < pMag->m_dwBaseAddr)
            || ((StartAddr + Size) >= pMag->m_dwEndAddr))
    {
        return 0;
    }

    if(pMag->m_wPageSize == 0)
    {
        return 0;
    }

    pBuf = (uint8_t *)sdram_malloc(0, Size);

    if(pBuf == NULL)
    {
        return 0;
    }

    Sum = 0;

    if(Size == ExtFlash_Read(pMag, StartAddr, pBuf, Size))
    {
        for(i = 0; i < Size; i++)
        {
            Sum += pBuf[i];
        }

    }

    sdram_free(0, pBuf);

    return Sum;
}

#if 1

/*
 ************************************************************************************************************************************************************************
 *��������: FlashStorageInit
 *��������: ������洢����س�ʼ��
 *��������: �����豸����ṹ��ָ��/�洢Ӧ�ù���ṹ��ָ��
 *�������: ��
 *��������: TRUE�ɹ�/FALSEʧ��
 *��������: XFW
 *ȫ������: ��
 *����˵��: malloc�ѿռ䣬δ�ͷ�
 ************************************************************************************************************************************************************************
 */
extern u32 FlashStorageInit(W25QXX_MAG *pW25QXXMag, FLASH_MAG *pFlashMag)
{
    if(pW25QXXMag == NULL || pFlashMag == NULL)
        return FALSE;

    //memset(pW25QXXMag,0,sizeof(W25QXX_MAG));
    memset(pFlashMag, 0, sizeof(FLASH_MAG));

    pW25QXXMag->m_IsOnLine 	= FALSE;
    pW25QXXMag->m_IsBusy 	= FALSE;
    pW25QXXMag->m_IsStart 	= FALSE;
    pFlashMag->pW25QXXMag 	= pW25QXXMag;
    pFlashMag->RecordCnt 	= 0;

    switch(pW25QXXMag->m_wW25qxxType)
    {
        case W25Q32:
            pFlashMag->m_dwFlashCapacity = 4 * 1024 * 1024;
            break;

        default:
            pFlashMag->m_dwFlashCapacity = 0;
            break;
    }

    return QueueListInit(&pFlashMag->pQueueHandle);
}


/*
 ************************************************************************************************************************************************************************
 *��������: FlashStorageCheckOnTime
 *��������: ��ʱ���洢����������Ƿ���������Ҫ����
 *��������: �洢Ӧ�ù���ṹ��ָ��
 *�������: ��
 *��������: TRUE��ʼ��������/FALSE��ʱ�޲���
 *��������: XFW
 *ȫ������: ��
 *����˵��:
 ************************************************************************************************************************************************************************
 */
extern u32 FlashStorageCheckOnTime(FLASH_MAG *pFlashMag)
{
    QUEUE_DATA Temp;
    W25QXX_MAG *pW25QXXMag = NULL;

    if(pFlashMag == NULL)
        return FALSE;

    pW25QXXMag = pFlashMag->pW25QXXMag;

    //�����������ݲ��ҵ�ǰFLASH���������ڱ���
    if((TRUE == QueueGetHead(pFlashMag->pQueueHandle)) && (pW25QXXMag->m_IsStart == FALSE))
    {
        QueuePop(pFlashMag->pQueueHandle, &Temp);

        if((Temp.DataLength != 0) && (Temp.DataLength < W25X_PAGE_SIZE))
        {
            pW25QXXMag->m_dwWriteAddr = *(uint32_t *)(&Temp.DataBuf[0]);
            pW25QXXMag->m_wNumByteToWrite = Temp.DataLength - 4;
            memcpy(pW25QXXMag->m_byWriteBuffer, &Temp.DataBuf[4], pW25QXXMag->m_wNumByteToWrite);
            pW25QXXMag->m_IsStart = TRUE;
            return TRUE;
        }
    }

    return FALSE;
}


/*
 ************************************************************************************************************************************************************************
 *��������: FlashStorageSubmit
 *��������: �ύ��Ҫ���������
 *��������: �洢Ӧ�ù���ṹ��ָ�롢���ݻ����������ݳ��ȡ��洢��ַ
 *�������: ��
 *��������: TRUE�ύ�ɹ�/FALSE�ύʧ��
 *��������: XFW
 *ȫ������: ��
 *����˵��:
 ************************************************************************************************************************************************************************
 */
extern u32 FlashStorageSubmit(FLASH_MAG *pFlashMag, u8 *pData, u32 DataLen, u32 Addr)
{
    QUEUE_DATA Queue;

    if((pFlashMag == NULL) || (pData == NULL)
            || (Addr >= pFlashMag->m_dwFlashCapacity))
        return FALSE;

    //����������ǰ�ĸ��ֽڱ�ʶ���ݴ洢��ַ
    *(uint32_t *)(&Queue.DataBuf[0]) = Addr;
    memcpy(&Queue.DataBuf[4], pData, DataLen);
    Queue.DataLength = DataLen + 4;

    return QueuePush(pFlashMag->pQueueHandle, &Queue);
}
#endif


/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
