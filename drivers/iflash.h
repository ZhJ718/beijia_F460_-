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
} IFLASH_HW_OPERATIONS;


typedef struct
{
    //�ڲ�FLASH����ַ�����Ե�ַ��
    uint32_t m_dwBaseAddr;
    //��ֹ��ַ
    uint32_t m_dwEndAddr;
    //��������λ:K��
    uint16_t m_dwCapacity;
    //ҳ��С
    uint16_t m_wPageSize;
    //ʵ���ڲ�FLASH���Ӳ���������ɶ�д
    //IFLASH_HW_OPERATIONS m_sHwOps;
} IFLASH_MAG;

extern IFLASH_MAG g_sIFlashMag;

/*******************8�ڲ��ռ仮��**********************/

//FLASH >= 256KB

//boot �ռ�(32KB)
#define FLASH_BOOT_SIZE  0x8000
#define FLASH_BOOT_START  (g_sIFlashMag.m_dwBaseAddr)

//�����ռ�(32KB,���ٱ�֤4����С������Ԫ)

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

//APP�ռ�
#define FLASH_APP_SIZE  (g_sIFlashMag.m_wPageSize*512 - FLASH_BOOT_SIZE - FLASH_PARAM_SIZE)
#define FLASH_APP_START  (FLASH_BOOT_START+FLASH_BOOT_SIZE+FLASH_PARAM_SIZE)

//���ÿռ�(��FLASH��С��һ��)
#define FLASH_APP_UPDATE_SIZE  (g_sIFlashMag.m_dwCapacity*512)
#define FLASH_APP_UPDATE_START  (FLASH_BOOT_START+g_sIFlashMag.m_dwCapacity*512)



/****************************************************************
@FUNCTION����ȡУ���(uint8_t)
@INPUT�������׵�ַ�ʹ�С
@OUTPUT����
@RETURN: У���
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint8_t GetIFlashCheckSum_U8(uint8_t *a_pDat, uint32_t a_len);


/****************************************************************
@FUNCTION����ȡУ���(uint32_t)
@INPUT�������׵�ַ�ʹ�С
@OUTPUT����
@RETURN: У���
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint32_t GetIFlashCheckSum_U32(uint8_t *a_pDat, uint32_t a_len);


/****************************************************************
@FUNCTION���ڲ�FLASH��ʼ��
@INPUT��Ӳ���ӿں����ṹ�壬IFLASH����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �ڲ�FLASH�ڵ�ַ�ռ�Ļ���ַIFLASH_BASE_ADDR
****************************************************************/
extern void IFlash_Init(IFLASH_MAG *pMag);

/****************************************************************
@FUNCTION���ڲ�FLASH��
@INPUT��Ӳ���ӿں����ṹ�壬��ȡ��ʼ��ַ����ȡ���ݻ���������ȡ���ݴ�С
@OUTPUT����ȡ����
@RETURN: ʵ�ʶ�ȡ���ݴ�С
@AUTHOR��xfw
@SPECIAL: ��У���ַ�Ƿ�λ���ڲ�FLASH��ַ�ռ䣬��˴˺���Ҳ�������ڶ�ȡ�����ַ�ռ�����
****************************************************************/
extern uint32_t IFlash_Read(IFLASH_MAG *pMag, uint32_t a_RAddr, uint8_t *a_pBuffer, uint32_t a_NumToRead);

/****************************************************************
@FUNCTION���ڲ�FLASHҳ����
@INPUT��ҳ��ʼ��ַ
@OUTPUT����
@RETURN: FLASH_Status
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern uint32_t IFlash_PageErase(IFLASH_MAG *pMag, uint32_t StartAddr);

/****************************************************************
@FUNCTION���ڲ�FLASHд
@INPUT��Ӳ���ӿں����ṹ�壬д����ʼ��ַ��д�����ݣ�д�����ݴ�С
@OUTPUT����
@RETURN: ʵ��д�����ݴ�С
@AUTHOR��xfw
@SPECIAL: ֧�ֿ�ҳд
****************************************************************/
extern uint32_t IFlash_Write(IFLASH_MAG *pMag, uint32_t a_WAddr, uint8_t *a_pBuffer, uint32_t a_NumToWrite);

/****************************************************************
@FUNCTION����ȡ�ڲ�FLASHָ����С�����У���
@INPUT��Ӳ���ӿں����ṹ�壬��ʼ��ַ�����ݴ�С
@OUTPUT����
@RETURN: У���
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern uint32_t IFlash_GetFlashCheckSum(IFLASH_MAG *pMag, uint32_t StartAddr, uint32_t Size);


#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
