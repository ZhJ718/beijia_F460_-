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

    u8  m_IsOnLine: 1;		//��ʶ�洢оƬ�Ƿ�ͨѶ����
    u8	m_IsBusy: 1;		//��ʶ���ڴ洢����
    u8	m_IsStart: 1;		//���Կ�ʼ��������
    u8	m_IsRes: 5;		//��ʱ����
    u8 	m_byWriteStep;		//��ʶ��ǰ���е��ڼ���
    u8	m_byWriteBuffer[W25X_PAGE_SIZE];
    //д�����ݻ�����
    //u8	*pPageBuffer;	//д����������ҳ������
    u32 m_dwWriteAddr;		//д�����ݵ�ַ
    u32 m_dwPageStartAddr;	//д������ҳ�����ַ
    u16 m_wNumByteToWrite;	//д�������ֽ���
    u16 m_wW25qxxType;//����W25QXX�ĸ�оƬ

} W25QXX_MAG, *PW25QXX_MAG;

extern W25QXX_MAG g_sW25qxxMag;

//��ʼ��SPI FLASH
void W25QXX_Init(W25QXX_MAG *pMag);

//�첽Flashд�룬�����Ƕ�ҳд�룬д��������ر�֤��ͬһҳ
void W25QXX_Write_AsyncProcess(W25QXX_MAG *pMag);


#if 0
extern u32 g_dwLogWriteOffset;
/*
 ************************************************************************************************************************************************************************
 *��������: Log_Write
 *��������: д��־
 *��������: �洢���ݻ����������ݳ���
 *�������: ��
 *��������: ��
 *��������: XFW 20191014
 *ȫ������: g_dwLogWriteOffset-��ǰ�洢��ʼ��ַ��ҳ����
 *����˵��: ��Ϸ��ʼ�ű�����־
 ************************************************************************************************************************************************************************
*/
void Log_Write(u8 *pBuf, u32 Len);

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
void Log_PrintInit(void);


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
u32 Log_PrintNext_5AA5AA(uint32_t (*fp)(u8 *, u16));



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
u32 Log_PrintNext_5AAA(uint32_t (*fp)(u8 *, u16));

#endif


#define EXT_FLASH_START_ADDR 0xF0000000   //αӳ�䵽ϵͳ��ַ�ռ�


//Ӳ������
typedef struct
{
    //Ӳ���ӿڳ�ʼ������
    void (*hw_init)(void);
    //��ȡ�ڲ�FLASH�ܴ�С
    uint32_t (*get_capacity)(void);
    //��ȡ��Сҳ��С��������������������ִ�п��Բ�������С�ռ䣩
    uint16_t (*get_page_size)(void);
    //������
    //uint32_t (*read)(uint32_t,uint16_t *,uint16_t);
    //д����
    //uint32_t (*write)(uint32_t,uint16_t *,uint16_t);
    //��ʼ��ַ
    uint32_t m_dwStartAddr;
} EFLASH_HW_OPERATIONS;


typedef struct
{

    //�ڲ�FLASH����ַ�����Ե�ַ��
    uint32_t m_dwBaseAddr;
    //��ֹ��ַ
    uint32_t m_dwEndAddr;
    //��������λ:K��
    uint32_t m_dwCapacity;
    //ҳ��С
    uint16_t m_wPageSize;
    //�ڲ�FLASH����
    uint16_t m_wFlashTypeID;

    //ʵ���ڲ�FLASH���Ӳ���������ɶ�д
    //IFLASH_HW_OPERATIONS m_sHwOps;
} EFLASH_MAG;

extern EFLASH_MAG g_sExtFlashMag;

/****************************************************************
@FUNCTION���ⲿFLASH��ʼ��
@INPUT��Ӳ���ӿں����ṹ�壬IFLASH����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �ڲ�FLASH�ڵ�ַ�ռ�Ļ���ַIFLASH_BASE_ADDR
****************************************************************/
extern void ExtFlash_Init(EFLASH_MAG *pMag);




/****************************************************************
@FUNCTION���ⲿFLASH��
@INPUT����ȡ��ʼ��ַ����ȡ���ݻ���������ȡ���ݴ�С
@OUTPUT����ȡ����
@RETURN: ʵ�ʶ�ȡ���ݴ�С
@AUTHOR��xfw
@SPECIAL: ����ȡ���ݽ϶࣬���нϴ���ʱ
****************************************************************/
extern uint32_t ExtFlash_Read(EFLASH_MAG *pMag, uint32_t a_RAddr, uint8_t *a_pBuffer, uint32_t a_NumToRead);


/****************************************************************
@FUNCTION���ⲿFLASHд
@INPUT��д����ʼ��ַ��д�����ݣ�д�����ݴ�С
@OUTPUT����
@RETURN: ʵ��д�����ݴ�С
@AUTHOR��xfw
@SPECIAL: ������ϵͳ
****************************************************************/
extern uint32_t ExtFlash_Write(EFLASH_MAG *pMag, uint32_t a_WAddr, uint8_t *a_pBuffer, uint32_t a_NumToWrite);

extern uint32_t ExtFlash_PageErase(EFLASH_MAG *pMag, uint32_t StartAddr);
/****************************************************************
@FUNCTION����ȡ�ⲿFLASHָ����С�����У���
@INPUT��Ӳ���ӿں����ṹ�壬��ʼ��ַ�����ݴ�С
@OUTPUT����
@RETURN: У���
@AUTHOR��xfw
@SPECIAL:������ϵͳ
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
extern u32 FlashStorageInit(W25QXX_MAG *pW25QXXMag, FLASH_MAG *pFlashMag);


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
extern u32 FlashStorageCheckOnTime(FLASH_MAG *pFlashMag);

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
extern u32 FlashStorageSubmit(FLASH_MAG *pFlashMag, u8 *pData, u32 DataLen, u32 Addr);




#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
