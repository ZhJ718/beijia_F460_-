#ifndef __W25QXX_H
#define __W25QXX_H

#include "config.h"
//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//W25Q64 代码
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/9
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////

//W25X系列/Q系列芯片列表
//W25Q80  ID  0XEF13
//W25Q16  ID  0XEF14
//W25Q32  ID  0XEF15
//W25Q64  ID  0XEF16
//W25Q128 ID  0XEF17
#define W25Q80 	0XEF13
#define W25Q16 	0XEF14
#define W25Q32 	0X8515
#define W25Q64 	0XEF16
#define W25Q128	0XEF17

extern u16 W25QXX_TYPE;					//定义W25QXX芯片型号

////////////////////////////////////////////////////////////////////////////

//指令表
#define W25X_WriteEnable		0x06
#define W25X_WriteDisable		0x04
#define W25X_ReadStatusReg		0x05
#define W25X_WriteStatusReg		0x01
#define W25X_ReadData			0x03
#define W25X_FastReadData		0x0B
#define W25X_FastReadDual		0x3B
#define W25X_PageProgram		0x02
#define W25X_BlockErase			0xD8
#define W25X_SectorErase		0x20
#define W25X_PageErase			0x81
#define W25X_ChipErase			0xC7
#define W25X_PowerDown			0xB9
#define W25X_ReleasePowerDown	0xAB
#define W25X_DeviceID			0xAB
#define W25X_ManufactDeviceID	0x90
#define W25X_JedecDeviceID		0x9F


#define W25X_PAGE_SIZE 256



//硬件操作
typedef struct
{
    //硬件接口初始化函数
    void (*hw_init)(void);

    void (*set_cs)(void);

    void (*reset_cs)(void);

    //读写操作
    u8 (*rwbyte)(u8);

} W25QXX_HW_OPERATIONS;



//读取W25QXX的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
u8 W25QXX_ReadSR(W25QXX_HW_OPERATIONS a_sHwOps);


//写W25QXX状态寄存器
//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
void W25QXX_Write_SR(W25QXX_HW_OPERATIONS a_sHwOps, u8 sr);

//W25QXX写使能
//将WEL置位
void W25QXX_Write_Enable(W25QXX_HW_OPERATIONS a_sHwOps) ;


//W25QXX写禁止
//将WEL清零
void W25QXX_Write_Disable(W25QXX_HW_OPERATIONS a_sHwOps)   ;


//读取芯片ID
//返回值如下:
//0XEF13,表示芯片型号为W25Q80
//0XEF14,表示芯片型号为W25Q16
//0X8515,表示芯片型号为W25Q32
//0XEF16,表示芯片型号为W25Q64
//0XEF17,表示芯片型号为W25Q128
u16 W25QXX_ReadID(W25QXX_HW_OPERATIONS a_sHwOps);


//读取SPI FLASH
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
void W25QXX_Read(W25QXX_HW_OPERATIONS a_sHwOps, u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)   ;


//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
void W25QXX_Write_Page(W25QXX_HW_OPERATIONS a_sHwOps, u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);


//无检验写SPI FLASH
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void W25QXX_Write_NoCheck(W25QXX_HW_OPERATIONS a_sHwOps, u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)   ;

//擦除整个芯片
//等待时间超长...
void W25QXX_Erase_Chip(W25QXX_HW_OPERATIONS a_sHwOps);


//擦除一个扇区
//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个山区的最少时间:150ms
void W25QXX_Erase_Sector(W25QXX_HW_OPERATIONS a_sHwOps, u32 Dst_Addr)   ;


//擦除一个页
//Dst_Addr:页地址 根据实际容量设置
void W25QXX_Erase_Page(W25QXX_HW_OPERATIONS a_sHwOps, u32 Dst_Addr);


//等待空闲
void W25QXX_Wait_Busy(W25QXX_HW_OPERATIONS a_sHwOps)   ;


//等待空闲
u32 W25QXX_Is_Busy(W25QXX_HW_OPERATIONS a_sHwOps)  ;

//进入掉电模式
void W25QXX_PowerDown(W25QXX_HW_OPERATIONS a_sHwOps)   ;

//唤醒
void W25QXX_WAKEUP(W25QXX_HW_OPERATIONS a_sHwOps)   ;



#endif
















