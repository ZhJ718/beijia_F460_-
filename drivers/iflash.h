/**
  ******************************************************************************
  * @file    iflash.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IFLASH_H
#define __IFLASH_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/

#include "config.h"

#if defined(AT32F4xx)

#define IFLASH_BASE_ADDR 0x08000000


#elif defined(HC32F46x)

#define IFLASH_BASE_ADDR 0x00000000

#endif


//硬件操作
typedef struct
{
    //硬件接口初始化函数
    void (*hw_init)(void);
    //获取内部FLASH总大小
    uint32_t (*get_capacity)(void);
    //获取最小页大小（或扇区，即擦除操作执行可以擦除的最小空间）
    uint16_t (*get_page_size)(void);
    //读操作
    //uint32_t (*read)(uint32_t,uint16_t *,uint16_t);
    //写操作
    //uint32_t (*write)(uint32_t,uint16_t *,uint16_t);
    //起始地址
    uint32_t m_dwStartAddr;
} IFLASH_HW_OPERATIONS;


typedef struct
{
    //内部FLASH基地址（绝对地址）
    uint32_t m_dwBaseAddr;
    //终止地址
    uint32_t m_dwEndAddr;
    //容量（单位:K）
    uint16_t m_dwCapacity;
    //页大小
    uint16_t m_wPageSize;
    //实现内部FLASH相关硬件操作，可读写
    //IFLASH_HW_OPERATIONS m_sHwOps;
} IFLASH_MAG;

extern IFLASH_MAG g_sIFlashMag;

/*******************8内部空间划分**********************/

//FLASH >= 256KB

//boot 空间(32KB)
#define FLASH_BOOT_SIZE  0x8000
#define FLASH_BOOT_START  (g_sIFlashMag.m_dwBaseAddr)

//参数空间(32KB,至少保证4个最小擦除单元)

#define FLASH_PARAM_SIZE 	0x8000//(4*g_sIFlashMag.m_wPageSize)
#define FLASH_PARAM_START  (FLASH_BOOT_START+FLASH_BOOT_SIZE)
#define FLASH_UPDATE_ADDR  	(FLASH_PARAM_START)
#define FLASH_UPDATE_SIZE  	(g_sIFlashMag.m_wPageSize)
#define FLASH_ID_ADDR		(FLASH_UPDATE_ADDR+FLASH_UPDATE_SIZE)
#define FLASH_ID_SIZE			(g_sIFlashMag.m_wPageSize)
#define FLASH_SYS_ADDR		(FLASH_ID_ADDR+FLASH_ID_SIZE)
#define FLASH_SYS_SIZE		(g_sIFlashMag.m_wPageSize)
#define FLASH_USER_ADDR		(FLASH_SYS_ADDR+FLASH_SYS_SIZE)
#define FLASH_USER_SIZE		(g_sIFlashMag.m_wPageSize)

//APP空间
#define FLASH_APP_SIZE  (g_sIFlashMag.m_wPageSize*512 - FLASH_BOOT_SIZE - FLASH_PARAM_SIZE)
#define FLASH_APP_START  (FLASH_BOOT_START+FLASH_BOOT_SIZE+FLASH_PARAM_SIZE)

//备用空间(总FLASH大小的一半)
#define FLASH_APP_UPDATE_SIZE  (g_sIFlashMag.m_dwCapacity*512)
#define FLASH_APP_UPDATE_START  (FLASH_BOOT_START+g_sIFlashMag.m_dwCapacity*512)



/****************************************************************
@FUNCTION：获取校验和(uint8_t)
@INPUT：数据首地址和大小
@OUTPUT：无
@RETURN: 校验和
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint8_t GetIFlashCheckSum_U8(uint8_t *a_pDat, uint32_t a_len);


/****************************************************************
@FUNCTION：获取校验和(uint32_t)
@INPUT：数据首地址和大小
@OUTPUT：无
@RETURN: 校验和
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint32_t GetIFlashCheckSum_U32(uint8_t *a_pDat, uint32_t a_len);


/****************************************************************
@FUNCTION：内部FLASH初始化
@INPUT：硬件接口函数结构体，IFLASH管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 内部FLASH在地址空间的基地址IFLASH_BASE_ADDR
****************************************************************/
extern void IFlash_Init(IFLASH_MAG *pMag);

/****************************************************************
@FUNCTION：内部FLASH读
@INPUT：硬件接口函数结构体，读取起始地址，读取数据缓冲区，读取数据大小
@OUTPUT：读取数据
@RETURN: 实际读取数据大小
@AUTHOR：xfw
@SPECIAL: 不校验地址是否位于内部FLASH地址空间，因此此函数也可以用于读取任意地址空间数据
****************************************************************/
extern uint32_t IFlash_Read(IFLASH_MAG *pMag, uint32_t a_RAddr, uint8_t *a_pBuffer, uint32_t a_NumToRead);

/****************************************************************
@FUNCTION：内部FLASH页擦除
@INPUT：页起始地址
@OUTPUT：无
@RETURN: FLASH_Status
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
extern uint32_t IFlash_PageErase(IFLASH_MAG *pMag, uint32_t StartAddr);

/****************************************************************
@FUNCTION：内部FLASH写
@INPUT：硬件接口函数结构体，写入起始地址，写入数据，写入数据大小
@OUTPUT：无
@RETURN: 实际写入数据大小
@AUTHOR：xfw
@SPECIAL: 支持跨页写
****************************************************************/
extern uint32_t IFlash_Write(IFLASH_MAG *pMag, uint32_t a_WAddr, uint8_t *a_pBuffer, uint32_t a_NumToWrite);

/****************************************************************
@FUNCTION：获取内部FLASH指定大小区域的校验和
@INPUT：硬件接口函数结构体，起始地址，数据大小
@OUTPUT：无
@RETURN: 校验和
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
extern uint32_t IFlash_GetFlashCheckSum(IFLASH_MAG *pMag, uint32_t StartAddr, uint32_t Size);


#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
