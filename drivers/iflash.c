/**
  ******************************************************************************
  * @file    iflash.c
  * @author  xfw
  * @brief   This file contains definitions for inter flash
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
#include "iflash.h"
#include "malloc.h"

IFLASH_MAG g_sIFlashMag;


uint8_t GetIFlashCheckSum_U8(uint8_t *a_pDat, uint32_t a_len)
{
    uint8_t Sum = 0;

    while(a_len--)
    {
        Sum += *a_pDat;
        a_pDat++;
    }

    return Sum;
}


uint32_t GetIFlashCheckSum_U32(uint8_t *a_pDat, uint32_t a_len)
{
    uint32_t Sum = 0;

    while(a_len--)
    {
        Sum += *a_pDat;
        a_pDat++;
    }

    return Sum;
}


#if defined(AT32F4xx)


//读取STM32F10X系列单片机内部Flash大小(单位:K)
uint32_t IFLASH_GetFlashSize_STM32F10X(void)
{
    u32 Stm32_Flash_Size;

    Stm32_Flash_Size = *(u16*)(0x1FFFF7E0);//闪存容量寄存器
    //Stm32_Flash_Size *= 1024;

    return Stm32_Flash_Size;
}

//读取STM32F10X系列单片机内部Flash的Page大小
uint16_t IFLASH_GetPageSize_STM32F10X(void)
{
    u16 Stm32_Flash_Size;

    Stm32_Flash_Size = *(u16*)(0x1FFFF7E0);//闪存容量寄存器

    return (Stm32_Flash_Size < 256 ? 1024 : 2048);
}


/****************************************************************
@FUNCTION：内部FLASH初始化
@INPUT：硬件接口函数结构体，IFLASH管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 内部FLASH在地址空间的基地址IFLASH_BASE_ADDR
****************************************************************/
extern void IFlash_Init(IFLASH_MAG *pMag)
{
    assert_param(pMag != NULL);

    memset(pMag, 0, sizeof(IFLASH_MAG));

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
    pMag->m_wPageSize = IFLASH_GetPageSize_STM32F10X();
    pMag->m_dwCapacity = IFLASH_GetFlashSize_STM32F10X();
    pMag->m_dwEndAddr = pMag->m_dwBaseAddr = IFLASH_BASE_ADDR;
    pMag->m_dwEndAddr += 1024 * pMag->m_dwCapacity;

}


/****************************************************************
@FUNCTION：内部FLASH读
@INPUT：硬件接口函数结构体，读取起始地址，读取数据缓冲区，读取数据大小
@OUTPUT：读取数据
@RETURN: 实际读取数据大小
@AUTHOR：xfw
@SPECIAL: 不校验地址是否位于内部FLASH地址空间，因此此函数也可以用于读取任意地址空间数据
****************************************************************/
extern uint32_t IFlash_Read(IFLASH_MAG *pMag, uint32_t a_RAddr, uint8_t *a_pBuffer, uint32_t a_NumToRead)
{
    uint32_t i;

    assert_param(pMag != NULL);
    assert_param(a_pBuffer != NULL);

    for(i = 0; i < a_NumToRead; i++)
    {
        a_pBuffer[i] = *(vu8 *)a_RAddr;
        a_RAddr += 1;
    }

    return a_NumToRead;
}

/****************************************************************
@FUNCTION：内部FLASH页擦除
@INPUT：页起始地址
@OUTPUT：无
@RETURN: FLASH_Status
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
extern uint32_t IFlash_PageErase(IFLASH_MAG *pMag, uint32_t StartAddr)
{
    FLASH_Status ret;

    FLASH_Unlock();
    ret = FLASH_ErasePage(StartAddr - StartAddr % pMag->m_wPageSize);
    FLASH_Lock();

    return ret;
}

/****************************************************************
@FUNCTION：内部FLASH写
@INPUT：硬件接口函数结构体，写入起始地址，写入数据，写入数据大小
@OUTPUT：无
@RETURN: 实际写入数据大小
@AUTHOR：xfw
@SPECIAL: 支持跨页写
****************************************************************/
extern uint32_t IFlash_Write(IFLASH_MAG *pMag, uint32_t a_WAddr, uint8_t *a_pBuffer, uint32_t a_NumToWrite)
{
    uint32_t i, PageStartAddr;
    uint16_t PageSize = pMag->m_wPageSize;
    uint8_t *pPageBuf = NULL;
    uint16_t PageOffset;
    uint16_t PageRemain;
    uint32_t Num = a_NumToWrite;

    assert_param(pMag != NULL);
    assert_param(a_pBuffer != NULL);

    if(Num == 0)
    {
        return 0;
    }

    if((a_WAddr >= pMag->m_dwBaseAddr)
            && ((a_WAddr + Num) < pMag->m_dwEndAddr))
    {

        pPageBuf = (uint8_t *)sdram_malloc(0, PageSize);

        if(pPageBuf == NULL)
        {
            return 0;
        }

        FLASH_Unlock();
        a_WAddr -= pMag->m_dwBaseAddr;

        PageStartAddr = a_WAddr / PageSize;
        PageStartAddr *= PageSize;
        PageOffset = a_WAddr % PageSize;
        PageRemain = PageSize - PageOffset;

        while(1)
        {
            IFlash_Read(pMag, PageStartAddr + pMag->m_dwBaseAddr, pPageBuf, PageSize);

            for(i = 0; i < MIN(PageRemain, Num); i++)
            {
                pPageBuf[PageOffset + i] = a_pBuffer[i];
            }

            Num -= i;
            a_pBuffer += i;

            FLASH_ErasePage(PageStartAddr + pMag->m_dwBaseAddr);

            for(i = 0; i < PageSize; i += 2)
            {
                FLASH_ProgramHalfWord(PageStartAddr + pMag->m_dwBaseAddr + i, *((uint16_t *)&pPageBuf[i]));
            }

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
        FLASH_Lock();//上锁

        return a_NumToWrite;
    }

    return 0;
}


/****************************************************************
@FUNCTION：获取内部FLASH指定大小区域的校验和
@INPUT：硬件接口函数结构体，起始地址，数据大小
@OUTPUT：无
@RETURN: 校验和
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
extern uint32_t IFlash_GetFlashCheckSum(IFLASH_MAG *pMag, uint32_t StartAddr, uint32_t Size)
{
    uint32_t Sum;

    if((StartAddr < pMag->m_dwBaseAddr)
            || ((StartAddr + Size) >= pMag->m_dwEndAddr))
        return 0;

    Sum = 0;

    while(Size)
    {
        Sum += *(vu8*)StartAddr;
        Size -= 1;
        StartAddr += 1;
    }

    return Sum;
}

#elif defined(HC32F46x)


/*  Bit definition for EFM_FRMC register  */
#define EFM_FRMC_SLPMD                            ((uint32_t)0x00000001)
#define EFM_FRMC_FLWT                             ((uint32_t)0x000000F0)
#define EFM_FRMC_FLWT_0                           ((uint32_t)0x00000010)
#define EFM_FRMC_FLWT_1                           ((uint32_t)0x00000020)
#define EFM_FRMC_FLWT_2                           ((uint32_t)0x00000040)
#define EFM_FRMC_FLWT_3                           ((uint32_t)0x00000080)
#define EFM_FRMC_CACHE                            ((uint32_t)0x00010000)
#define EFM_FRMC_CRST                             ((uint32_t)0x01000000)

/*  Bit definition for EFM_FWMC register  */
#define EFM_FWMC_PEMODE                           ((uint32_t)0x00000001)
#define EFM_FWMC_PEMOD                            ((uint32_t)0x00000070)
#define EFM_FWMC_PEMOD_0                          ((uint32_t)0x00000010)
#define EFM_FWMC_PEMOD_1                          ((uint32_t)0x00000020)
#define EFM_FWMC_PEMOD_2                          ((uint32_t)0x00000040)
#define EFM_FWMC_BUSHLDCTL                        ((uint32_t)0x00000100)

/*  Bit definition for EFM_FSR register  */
#define EFM_FSR_PEWERR                            ((uint32_t)0x00000001)
#define EFM_FSR_PEPRTERR                          ((uint32_t)0x00000002)
#define EFM_FSR_PGSZERR                           ((uint32_t)0x00000004)
#define EFM_FSR_PGMISMTCH                         ((uint32_t)0x00000008)
#define EFM_FSR_OPTEND                            ((uint32_t)0x00000010)
#define EFM_FSR_RDCOLERR                          ((uint32_t)0x00000020)
#define EFM_FSR_RDY                               ((uint32_t)0x00000100)

/*  Bit definition for EFM_FSCLR register  */
#define EFM_FSCLR_PEWERRCLR                       ((uint32_t)0x00000001)
#define EFM_FSCLR_PEPRTERRCLR                     ((uint32_t)0x00000002)
#define EFM_FSCLR_PGSZERRCLR                      ((uint32_t)0x00000004)
#define EFM_FSCLR_PGMISMTCHCLR                    ((uint32_t)0x00000008)
#define EFM_FSCLR_OPTENDCLR                       ((uint32_t)0x00000010)
#define EFM_FSCLR_RDCOLERRCLR                     ((uint32_t)0x00000020)

/*  Bit definition for EFM_FITE register  */
#define EFM_FITE_PEERRITE                         ((uint32_t)0x00000001)
#define EFM_FITE_OPTENDITE                        ((uint32_t)0x00000002)
#define EFM_FITE_RDCOLERRITE                      ((uint32_t)0x00000004)

/*  Bit definition for EFM_FSWP register  */
#define EFM_FSWP_FSWP                             ((uint32_t)0x00000001)



#define EFM_TIMEOUT                     0x2000u

#define ERROR_FLAG_CLEAR_MASK           (EFM_FSCLR_PEWERRCLR  | EFM_FSCLR_PEPRTERRCLR  | \
        EFM_FSCLR_PGSZERRCLR | EFM_FSCLR_PGMISMTCHCLR | \
        EFM_FSCLR_OPTENDCLR  | EFM_FSCLR_RDCOLERRCLR)


//读取单片机内部Flash大小(单位:K)
uint32_t IFLASH_GetFlashSize_HC32F460(void)
{
    uint32_t Flash_Size = 512;

    return Flash_Size;
}

//读取单片机内部Flash的Page大小
uint16_t IFLASH_GetPageSize_HC32F460(void)
{
    uint16_t PageSize = 8 * 1024;

    return PageSize;
}

/****************************************************************
@FUNCTION：内部FLASH初始化
@INPUT：硬件接口函数结构体，IFLASH管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 内部FLASH在地址空间的基地址IFLASH_BASE_ADDR
****************************************************************/
extern void IFlash_Init(IFLASH_MAG *pMag)
{
    assert_param(pMag != NULL);

    memset(pMag, 0, sizeof(IFLASH_MAG));

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
    pMag->m_wPageSize = IFLASH_GetPageSize_HC32F460();
    pMag->m_dwCapacity = IFLASH_GetFlashSize_HC32F460();
    pMag->m_dwEndAddr = pMag->m_dwBaseAddr = IFLASH_BASE_ADDR;
    pMag->m_dwEndAddr += 1024 * pMag->m_dwCapacity;

}


/****************************************************************
@FUNCTION：内部FLASH读
@INPUT：硬件接口函数结构体，读取起始地址，读取数据缓冲区，读取数据大小
@OUTPUT：读取数据
@RETURN: 实际读取数据大小
@AUTHOR：xfw
@SPECIAL: 不校验地址是否位于内部FLASH地址空间，因此此函数也可以用于读取任意地址空间数据
****************************************************************/
extern uint32_t IFlash_Read(IFLASH_MAG *pMag, uint32_t a_RAddr, uint8_t *a_pBuffer, uint32_t a_NumToRead)
{
    uint32_t i;

    assert_param(pMag != NULL);
    assert_param(a_pBuffer != NULL);

    for(i = 0; i < a_NumToRead; i++)
    {
        a_pBuffer[i] = *(volatile uint8_t *)a_RAddr;
        a_RAddr += 1;
    }

    return a_NumToRead;
}

/****************************************************************
@FUNCTION：内部FLASH页擦除
@INPUT：页起始地址
@OUTPUT：无
@RETURN: en_result_t
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
extern uint32_t IFlash_PageErase(IFLASH_MAG *pMag, uint32_t StartAddr)
{
    en_result_t ret;

    /* Unlock EFM. */
    EFM_Unlock();
    /* Enable flash. */
    EFM_FlashCmd(Enable);

    /* Wait flash ready. */
    while(Set != EFM_GetFlagStatus(EFM_FLAG_RDY))
    {
        ;
    }

    ret = EFM_SectorErase(StartAddr);

    EFM_Lock();

    return ret;
}

/****************************************************************
@FUNCTION：内部FLASH写
@INPUT：硬件接口函数结构体，写入起始地址，写入数据，写入数据大小
@OUTPUT：无
@RETURN: 实际写入数据大小
@AUTHOR：xfw
@SPECIAL: 支持跨页写
****************************************************************/
extern uint32_t IFlash_Write(IFLASH_MAG *pMag, uint32_t a_WAddr, uint8_t *a_pBuffer, uint32_t a_NumToWrite)
{
	  uint32_t Num = a_NumToWrite;
    uint32_t i, PageStartAddr;
	  uint16_t PageOffset;
    uint16_t PageRemain;
    uint16_t PageSize = pMag->m_wPageSize;
    uint8_t *pPageBuf = NULL;

    assert_param(pMag != NULL);
    assert_param(a_pBuffer != NULL);

    if(Num == 0)
    {
        return 0;
    }

    if((a_WAddr >= pMag->m_dwBaseAddr)
            && ((a_WAddr + Num) < pMag->m_dwEndAddr))
    {

        pPageBuf = (uint8_t *)sdram_malloc(0, PageSize);

        if(pPageBuf == NULL)
        {
            return 0;
        }

        EFM_Unlock();
        /* Enable flash. */
        EFM_FlashCmd(Enable);

        /* Wait flash ready. */
        while(Set != EFM_GetFlagStatus(EFM_FLAG_RDY))
        {
            ;
        }

        a_WAddr -= pMag->m_dwBaseAddr;

        PageStartAddr = a_WAddr / PageSize;
        PageStartAddr *= PageSize;
        PageOffset = a_WAddr % PageSize;
        PageRemain = PageSize - PageOffset;

        while(1)
        {
            IFlash_Read(pMag, PageStartAddr + pMag->m_dwBaseAddr, pPageBuf, PageSize);

            for(i = 0; i < MIN(PageRemain, Num); i++)
            {
                pPageBuf[PageOffset + i] = a_pBuffer[i];
            }

            Num -= i;
            a_pBuffer += i;

            EFM_SectorErase(PageStartAddr + pMag->m_dwBaseAddr);

            for(i = 0; i < PageSize; i += 4)
            {
                EFM_SingleProgram(PageStartAddr + pMag->m_dwBaseAddr + i, *((uint32_t *)&pPageBuf[i]));
            }

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
        EFM_Lock();//上锁

        return a_NumToWrite;
    }

    return 0;
}


/****************************************************************
@FUNCTION：获取内部FLASH指定大小区域的校验和
@INPUT：硬件接口函数结构体，起始地址，数据大小
@OUTPUT：无
@RETURN: 校验和
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
extern uint32_t IFlash_GetFlashCheckSum(IFLASH_MAG *pMag, uint32_t StartAddr, uint32_t Size)
{
    uint32_t Sum;

    if((StartAddr < pMag->m_dwBaseAddr)
            || ((StartAddr + Size) >= pMag->m_dwEndAddr))
        return 0;

    Sum = 0;

    while(Size)
    {
        Sum += *(volatile uint8_t *)StartAddr;
        Size -= 1;
        StartAddr += 1;
    }

    return Sum;
}

#endif




/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
