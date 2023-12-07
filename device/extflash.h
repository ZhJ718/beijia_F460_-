/**
  ******************************************************************************
  * @file    extflash.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EXTFLASH_H
#define __EXTFLASH_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "config.h"
#include "w25qxx.h"
#include "queue.h"

typedef struct
{
    W25QXX_HW_OPERATIONS m_sHwOps;

    u8  m_IsOnLine: 1;		//标识存储芯片是否通讯正常
    u8	m_IsBusy: 1;		//标识正在存储数据
    u8	m_IsStart: 1;		//可以开始保存数据
    u8	m_IsRes: 5;		//暂时保留
    u8 	m_byWriteStep;		//标识当前进行到第几步
    u8	m_byWriteBuffer[W25X_PAGE_SIZE];
    //写入数据缓冲区
    //u8	*pPageBuffer;	//写入数据所在页缓冲区
    u32 m_dwWriteAddr;		//写入数据地址
    u32 m_dwPageStartAddr;	//写入数据页对齐地址
    u16 m_wNumByteToWrite;	//写入数据字节数
    u16 m_wW25qxxType;//具体W25QXX哪个芯片

} W25QXX_MAG, *PW25QXX_MAG;

extern W25QXX_MAG g_sW25qxxMag;

//初始化SPI FLASH
void W25QXX_Init(W25QXX_MAG *pMag);

//异步Flash写入，不考虑多页写入，写入数据务必保证在同一页
void W25QXX_Write_AsyncProcess(W25QXX_MAG *pMag);


#if 0
extern u32 g_dwLogWriteOffset;
/*
 ************************************************************************************************************************************************************************
 *函数名称: Log_Write
 *功能描述: 写日志
 *输入描述: 存储数据缓冲区、数据长度
 *输出描述: 无
 *返回描述: 无
 *作者日期: XFW 20191014
 *全局声明: g_dwLogWriteOffset-当前存储起始地址，页对齐
 *特殊说明: 演戏开始才保存日志
 ************************************************************************************************************************************************************************
*/
void Log_Write(u8 *pBuf, u32 Len);

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
void Log_PrintInit(void);


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
u32 Log_PrintNext_5AA5AA(uint32_t (*fp)(u8 *, u16));



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
u32 Log_PrintNext_5AAA(uint32_t (*fp)(u8 *, u16));

#endif


#define EXT_FLASH_START_ADDR 0xF0000000   //伪映射到系统地址空间


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
} EFLASH_HW_OPERATIONS;


typedef struct
{

    //内部FLASH基地址（绝对地址）
    uint32_t m_dwBaseAddr;
    //终止地址
    uint32_t m_dwEndAddr;
    //容量（单位:K）
    uint32_t m_dwCapacity;
    //页大小
    uint16_t m_wPageSize;
    //内部FLASH类型
    uint16_t m_wFlashTypeID;

    //实现内部FLASH相关硬件操作，可读写
    //IFLASH_HW_OPERATIONS m_sHwOps;
} EFLASH_MAG;

extern EFLASH_MAG g_sExtFlashMag;

/****************************************************************
@FUNCTION：外部FLASH初始化
@INPUT：硬件接口函数结构体，IFLASH管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 内部FLASH在地址空间的基地址IFLASH_BASE_ADDR
****************************************************************/
extern void ExtFlash_Init(EFLASH_MAG *pMag);




/****************************************************************
@FUNCTION：外部FLASH读
@INPUT：读取起始地址，读取数据缓冲区，读取数据大小
@OUTPUT：读取数据
@RETURN: 实际读取数据大小
@AUTHOR：xfw
@SPECIAL: 若读取数据较多，会有较大延时
****************************************************************/
extern uint32_t ExtFlash_Read(EFLASH_MAG *pMag, uint32_t a_RAddr, uint8_t *a_pBuffer, uint32_t a_NumToRead);


/****************************************************************
@FUNCTION：外部FLASH写
@INPUT：写入起始地址，写入数据，写入数据大小
@OUTPUT：无
@RETURN: 实际写入数据大小
@AUTHOR：xfw
@SPECIAL: 会阻塞系统
****************************************************************/
extern uint32_t ExtFlash_Write(EFLASH_MAG *pMag, uint32_t a_WAddr, uint8_t *a_pBuffer, uint32_t a_NumToWrite);

extern uint32_t ExtFlash_PageErase(EFLASH_MAG *pMag, uint32_t StartAddr);
/****************************************************************
@FUNCTION：获取外部FLASH指定大小区域的校验和
@INPUT：硬件接口函数结构体，起始地址，数据大小
@OUTPUT：无
@RETURN: 校验和
@AUTHOR：xfw
@SPECIAL:会阻塞系统
****************************************************************/
extern uint32_t ExtFlash_GetFlashCheckSum(EFLASH_MAG *pMag, uint32_t StartAddr, uint32_t Size);



typedef struct
{
    PQUEUE pQueueHandle;
    W25QXX_MAG *pW25QXXMag;
    u32	RecordCnt;
    u32 m_dwFlashCapacity;
} FLASH_MAG;

extern FLASH_MAG g_sFlashMag;

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
extern u32 FlashStorageInit(W25QXX_MAG *pW25QXXMag, FLASH_MAG *pFlashMag);


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
extern u32 FlashStorageCheckOnTime(FLASH_MAG *pFlashMag);

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
extern u32 FlashStorageSubmit(FLASH_MAG *pFlashMag, u8 *pData, u32 DataLen, u32 Addr);




#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
