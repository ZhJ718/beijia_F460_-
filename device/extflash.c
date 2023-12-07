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


// 4Kbytes为一个Sector
//16个扇区为1个Block
//W25Q128
//容量为16M字节,共有128个Block,4096个Sector
//初始化SPI FLASH的IO口
void W25QXX_LowLevelInit(W25QXX_HW_OPERATIONS a_sHwOps, W25QXX_MAG *pMag)
{
    assert_param(pMag != NULL);

    memset(pMag, 0, sizeof(W25QXX_MAG));

    pMag->m_sHwOps.hw_init 	= a_sHwOps.hw_init;
    pMag->m_sHwOps.set_cs 	= a_sHwOps.set_cs;
    pMag->m_sHwOps.reset_cs = a_sHwOps.reset_cs;
    pMag->m_sHwOps.rwbyte 	= a_sHwOps.rwbyte;

    //IO初始化 SPI初始化
    if(pMag->m_sHwOps.hw_init != NULL)
    {
        pMag->m_sHwOps.hw_init();
    }

    //片选拉高，不选中
    if(pMag->m_sHwOps.set_cs != NULL)
    {
        pMag->m_sHwOps.set_cs();
    }

}


void W25QXX_HardWareInit(void)
{
    W25QXX_CS_GpioInit();
    SPI2_Init();		   	//初始化SPI

    #if defined(AT32F4xx)

    SPI2_SetSpeed(SPI_MCLKP_2);//设置为18M时钟,高速模式

    #elif defined(HC32F46x)
    SPI2_SetSpeed(SpiClkDiv2);
    #endif


}

//初始化SPI FLASH
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
    pMag->m_wW25qxxType = W25QXX_ReadID(pMag->m_sHwOps);//读取FLASH ID.
}

//异步Flash写入，不考虑多页写入，写入数据务必保证在同一页
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
                //W25QXX_Read(pMag->pPageBuffer,pMag->PageStartAddr,W25X_PAGE_SIZE);//读出整个扇区的内容
                WriteAddrOffset = pMag->m_dwWriteAddr % W25X_PAGE_SIZE;

                pMag->m_sHwOps.reset_cs();	 //使能器件
                pMag->m_sHwOps.rwbyte(W25X_ReadData);         	//发送读取命令
                pMag->m_sHwOps.rwbyte((u8)((pMag->m_dwPageStartAddr) >> 16));  	//发送24bit地址
                pMag->m_sHwOps.rwbyte((u8)((pMag->m_dwPageStartAddr) >> 8));
                pMag->m_sHwOps.rwbyte((u8)pMag->m_dwPageStartAddr);

                for(i = 0; i < W25X_PAGE_SIZE; i++)
                {
                    data = pMag->m_sHwOps.rwbyte(0XFF);   	//循环读数

                    if((i < WriteAddrOffset) || (i >= WriteAddrEndOffset))
                    {
                        pMag->m_byWriteBuffer[i] = data;	//不覆盖要写入数据
                    }
                }

                pMag->m_sHwOps.set_cs();	   //取消片选
                W25QXX_Write_Enable(pMag->m_sHwOps);                  	//SET WEL
                pMag->m_byWriteStep++;
            }

            break;

        case 1:
            if(!W25QXX_Is_Busy(pMag->m_sHwOps))
            {
                pMag->m_sHwOps.reset_cs();	 //使能器件
                pMag->m_sHwOps.rwbyte(W25X_PageErase);      	//发送扇区擦除指令
                pMag->m_sHwOps.rwbyte((u8)((pMag->m_dwPageStartAddr) >> 16));  	//发送24bit地址
                pMag->m_sHwOps.rwbyte((u8)((pMag->m_dwPageStartAddr) >> 8));
                pMag->m_sHwOps.rwbyte((u8)pMag->m_dwPageStartAddr);
                pMag->m_sHwOps.set_cs();	   //取消片选
                pMag->m_byWriteStep++;
            }

            break;

        case 2:
            if(!W25QXX_Is_Busy(pMag->m_sHwOps))
            {
                W25QXX_Write_Enable(pMag->m_sHwOps);                  	//SET WEL
                pMag->m_sHwOps.reset_cs();	 //使能器件
                pMag->m_sHwOps.rwbyte(W25X_PageProgram);      	//发送写页命令
                pMag->m_sHwOps.rwbyte((u8)((pMag->m_dwPageStartAddr) >> 16)); 	//发送24bit地址
                pMag->m_sHwOps.rwbyte((u8)((pMag->m_dwPageStartAddr) >> 8));
                pMag->m_sHwOps.rwbyte((u8)pMag->m_dwPageStartAddr);

                for(i = 0; i < W25X_PAGE_SIZE; i++)
                    pMag->m_sHwOps.rwbyte(pMag->m_byWriteBuffer[i]);//循环写数

                pMag->m_sHwOps.set_cs();	   //取消片选
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
	每条日志按照外部FLASH页大小对齐，即每条日志占一页，W25X_PAGE_SIZE字节
	日志格式:数据长度(2Bytes)+GPS获取到的UTC实时时间(7Bytes)+数据(长度最大为W25X_PAGE_SIZE-9)
*/

u32 g_dwLogWriteOffset = 0;
u32 g_dwLogReadOffset = 0;

/*
 ************************************************************************************************************************************************************************
 *函数名称: Log_Write
 *功能描述: 写日志
 *输入描述: 存储数据缓冲区、数据长度
 *输出描述: 无
 *返回描述: 无
 *作者日期: XFW 20191014
 *全局声明: g_dwLogWriteOffset-当前存储起始地址，页对齐
 *特殊说明: 演习开始才保存日志,存储设备保留最后RECORD_INJURY_MAX个页，留给伤情管理
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
        //指示数据报文长度
        *(u16 *)Temp = Len;
        TempLen += 2;
        //填充UTC时间
        memcpy(&Temp[TempLen], &g_sGpsMag.m_sNmeaMsg.m_sUtcTime, sizeof(UTC_TIME));
        TempLen += sizeof(g_sGpsMag.m_sNmeaMsg.m_sUtcTime);
        //写入数据
        memcpy(&Temp[TempLen], pBuf, Len);
        TempLen += Len;

        //提交给存储设备
        if((g_dwLogWriteOffset < (g_sFlashMag.m_dwFlashCapacity - (RECORD_INJURY_MAX * W25X_PAGE_SIZE)))
                && (FALSE != FlashStorageSubmit(&g_sFlashMag, Temp, TempLen, g_dwLogWriteOffset)))
        {
            g_dwLogWriteOffset += W25X_PAGE_SIZE;
        }
    }
}


/*
 ************************************************************************************************************************************************************************
 *函数名称: Log_PrintInit
 *功能描述: 日志读取初始化
 *输入描述: 无
 *输出描述: 无
 *返回描述: 无
 *作者日期: XFW 20191014
 *全局声明: g_dwLogReadOffset-存储读取地址
 *特殊说明: g_dwLogReadOffset复位
 ************************************************************************************************************************************************************************
*/
void Log_PrintInit(void)
{
    g_dwLogReadOffset = 0;
}


/*
 ************************************************************************************************************************************************************************
 *函数名称: Log_PrintNext
 *功能描述: 通过电台串口打印下一条日志
 *输入描述: 串口输出函数指针
 *输出描述: 无
 *返回描述: TRUE-成功 FALSE-失败
 *作者日期: XFW 20191014
 *全局声明: g_dwLogReadOffset-存储读取地址
 *特殊说明: 读取成功后g_dwLogReadOffset偏移到下一页,蓝牙模块输出
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
 *函数名称: Log_PrintNext
 *功能描述: 通过蓝牙串口打印下一条日志
 *输入描述: 串口输出函数指针
 *输出描述: 无
 *返回描述: TRUE-成功 FALSE-失败
 *作者日期: XFW 20191014
 *全局声明: g_dwLogReadOffset-存储读取地址
 *特殊说明: 读取成功后g_dwLogReadOffset偏移到下一页,蓝牙模块输出
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
 *函数名称: Log_PrintNext
 *功能描述: 打印下一条日志
 *输入描述: 串口输出函数指针
 *输出描述: 无
 *返回描述: TRUE-成功 FALSE-失败
 *作者日期: XFW 20191014
 *全局声明: g_dwLogReadOffset-存储读取地址
 *特殊说明: 读取成功后g_dwLogReadOffset偏移到下一页,蓝牙模块输出
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

//读取系统外部Flash的Page大小
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
//读取系统外部Flash的尺寸大小(单位:K)
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
@FUNCTION：外部FLASH初始化
@INPUT：硬件接口函数结构体，IFLASH管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 内部FLASH在地址空间的基地址IFLASH_BASE_ADDR
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
@FUNCTION：外部FLASH读
@INPUT：读取起始地址，读取数据缓冲区，读取数据大小
@OUTPUT：读取数据
@RETURN: 实际读取数据大小
@AUTHOR：xfw
@SPECIAL: 若读取数据较多，会有较大延时
****************************************************************/
extern uint32_t ExtFlash_Read(EFLASH_MAG *pMag, uint32_t a_RAddr, uint8_t *a_pBuffer, uint32_t a_NumToRead)
{
    uint32_t i = 0;

    assert_param(pMag != NULL);
    assert_param(a_pBuffer != NULL);

    //判断地址有效性
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
@FUNCTION：外部FLASH页擦除
@INPUT：页内任意地址
@OUTPUT：无
@RETURN: 实际页起始地址
@AUTHOR：xfw
@SPECIAL: 会阻塞系统
****************************************************************/
extern uint32_t ExtFlash_PageErase(EFLASH_MAG *pMag, uint32_t StartAddr)
{
    uint32_t PageAddr;

    //判断地址有效性
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
@FUNCTION：外部FLASH写
@INPUT：写入起始地址，写入数据，写入数据大小
@OUTPUT：无
@RETURN: 实际写入数据大小
@AUTHOR：xfw
@SPECIAL: 会阻塞系统
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
@FUNCTION：获取外部FLASH指定大小区域的校验和
@INPUT：硬件接口函数结构体，起始地址，数据大小
@OUTPUT：无
@RETURN: 校验和
@AUTHOR：xfw
@SPECIAL:会阻塞系统
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
 *函数名称: FlashStorageInit
 *功能描述: 完成闪存储存相关初始化
 *输入描述: 物理设备管理结构体指针/存储应用管理结构体指针
 *输出描述: 无
 *返回描述: TRUE成功/FALSE失败
 *作者日期: XFW
 *全局声明: 无
 *特殊说明: malloc堆空间，未释放
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
 *函数名称: FlashStorageCheckOnTime
 *功能描述: 定时检查存储缓存队列中是否有数据需要保存
 *输入描述: 存储应用管理结构体指针
 *输出描述: 无
 *返回描述: TRUE开始保存数据/FALSE暂时无操作
 *作者日期: XFW
 *全局声明: 无
 *特殊说明:
 ************************************************************************************************************************************************************************
 */
extern u32 FlashStorageCheckOnTime(FLASH_MAG *pFlashMag)
{
    QUEUE_DATA Temp;
    W25QXX_MAG *pW25QXXMag = NULL;

    if(pFlashMag == NULL)
        return FALSE;

    pW25QXXMag = pFlashMag->pW25QXXMag;

    //队列中有数据并且当前FLASH无数据正在保存
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
 *函数名称: FlashStorageSubmit
 *功能描述: 提交需要保存的数据
 *输入描述: 存储应用管理结构体指针、数据缓冲区、数据长度、存储地址
 *输出描述: 无
 *返回描述: TRUE提交成功/FALSE提交失败
 *作者日期: XFW
 *全局声明: 无
 *特殊说明:
 ************************************************************************************************************************************************************************
 */
extern u32 FlashStorageSubmit(FLASH_MAG *pFlashMag, u8 *pData, u32 DataLen, u32 Addr)
{
    QUEUE_DATA Queue;

    if((pFlashMag == NULL) || (pData == NULL)
            || (Addr >= pFlashMag->m_dwFlashCapacity))
        return FALSE;

    //队列数据中前四个字节标识数据存储地址
    *(uint32_t *)(&Queue.DataBuf[0]) = Addr;
    memcpy(&Queue.DataBuf[4], pData, DataLen);
    Queue.DataLength = DataLen + 4;

    return QueuePush(pFlashMag->pQueueHandle, &Queue);
}
#endif


/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
