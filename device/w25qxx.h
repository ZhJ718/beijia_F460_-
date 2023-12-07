#ifndef __W25QXX_H
#define __W25QXX_H

#include "config.h"
//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//W25Q64 ����
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/9
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////

//W25Xϵ��/Qϵ��оƬ�б�
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

extern u16 W25QXX_TYPE;					//����W25QXXоƬ�ͺ�

////////////////////////////////////////////////////////////////////////////

//ָ���
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



//Ӳ������
typedef struct
{
    //Ӳ���ӿڳ�ʼ������
    void (*hw_init)(void);

    void (*set_cs)(void);

    void (*reset_cs)(void);

    //��д����
    u8 (*rwbyte)(u8);

} W25QXX_HW_OPERATIONS;



//��ȡW25QXX��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
u8 W25QXX_ReadSR(W25QXX_HW_OPERATIONS a_sHwOps);


//дW25QXX״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
void W25QXX_Write_SR(W25QXX_HW_OPERATIONS a_sHwOps, u8 sr);

//W25QXXдʹ��
//��WEL��λ
void W25QXX_Write_Enable(W25QXX_HW_OPERATIONS a_sHwOps) ;


//W25QXXд��ֹ
//��WEL����
void W25QXX_Write_Disable(W25QXX_HW_OPERATIONS a_sHwOps)   ;


//��ȡоƬID
//����ֵ����:
//0XEF13,��ʾоƬ�ͺ�ΪW25Q80
//0XEF14,��ʾоƬ�ͺ�ΪW25Q16
//0X8515,��ʾоƬ�ͺ�ΪW25Q32
//0XEF16,��ʾоƬ�ͺ�ΪW25Q64
//0XEF17,��ʾоƬ�ͺ�ΪW25Q128
u16 W25QXX_ReadID(W25QXX_HW_OPERATIONS a_sHwOps);


//��ȡSPI FLASH
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void W25QXX_Read(W25QXX_HW_OPERATIONS a_sHwOps, u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)   ;


//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
void W25QXX_Write_Page(W25QXX_HW_OPERATIONS a_sHwOps, u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);


//�޼���дSPI FLASH
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ����
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void W25QXX_Write_NoCheck(W25QXX_HW_OPERATIONS a_sHwOps, u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)   ;

//��������оƬ
//�ȴ�ʱ�䳬��...
void W25QXX_Erase_Chip(W25QXX_HW_OPERATIONS a_sHwOps);


//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ��ɽ��������ʱ��:150ms
void W25QXX_Erase_Sector(W25QXX_HW_OPERATIONS a_sHwOps, u32 Dst_Addr)   ;


//����һ��ҳ
//Dst_Addr:ҳ��ַ ����ʵ����������
void W25QXX_Erase_Page(W25QXX_HW_OPERATIONS a_sHwOps, u32 Dst_Addr);


//�ȴ�����
void W25QXX_Wait_Busy(W25QXX_HW_OPERATIONS a_sHwOps)   ;


//�ȴ�����
u32 W25QXX_Is_Busy(W25QXX_HW_OPERATIONS a_sHwOps)  ;

//�������ģʽ
void W25QXX_PowerDown(W25QXX_HW_OPERATIONS a_sHwOps)   ;

//����
void W25QXX_WAKEUP(W25QXX_HW_OPERATIONS a_sHwOps)   ;



#endif
















