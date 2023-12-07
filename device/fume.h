/**
  ******************************************************************************
  * @file    fume.h
  * @author  xfw
  * @brief   This file contains definitions for fume
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
#ifndef __FUME_H
#define __FUME_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "config.h"



//FUMEӲ������
typedef struct
{
    //FUMEӲ���ӿڳ�ʼ������
    void (*hw_init)(void);
    //FUME������
    void (*on)(void);
    //FUME�ز���
    void (*off)(void);
    //FUME��װ������
    uint32_t (*is_installed)(void);
} FUME_HW_OPERATIONS;


typedef struct
{
    //ʵ��Motor����첽�����߼���ֻ��
    uint8_t		m_byIsStart: 1;
    uint8_t		m_byRes: 7;

    uint16_t 	m_wSetCnt;	//��ʱ����ֵ
    uint16_t 	m_wNowCnt;	//�Ѿ���ʱʱ��

    //ʵ��Motor���Ӳ���������ɶ�д
    FUME_HW_OPERATIONS m_sHwOps;
} FUME_MAG;

/****************************************************************
@FUNCTION��ʵ�ַ�����ģʽ�·��̹޿��ƹ���
@INPUT��FUME����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ÿ�̶�ʱ��(Ŀǰ100Ms)����һ��

****************************************************************/
extern void Fume_TimedProcess(FUME_MAG *pMag) ;


/****************************************************************
@FUNCTION������ģʽ�·��̹޿��ƹ��ܳ�ʼ��
@INPUT��Ӳ���ӿڳ�ʼ���������ӿڴ򿪺������ӿڹرպ�����FUME����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ���ݷ��̹�Ӳ��������غ�������ʼ��Ӳ���͹رշ��̹�
****************************************************************/
extern void Fume_LL_Init(FUME_HW_OPERATIONS a_sHwOps, FUME_MAG *pMag);

/****************************************************************
@FUNCTION��������ģʽ�·��̹��Ƿ�װ���
@INPUT��FUME����ṹ��
@OUTPUT����
@RETURN: TRUE-�Ѱ�װ FALSE-δ��װ
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint32_t Fume_IsInstalled(FUME_MAG Mag);


/****************************************************************
@FUNCTION�����̹޷���
@INPUT��FUME����ṹ��ָ�룬���ӳ�
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:������ģʽ�£�ʵ����������ӳ�(0-��ʱ���ȼ����),ָ����ʱ�������ر����
****************************************************************/
extern void Fume_On(FUME_MAG *pMag, uint16_t SetTime);


/****************************************************************
@FUNCTION�����̹޷��̹ر�
@INPUT��FUME����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern void Fume_Off(FUME_MAG *pMag);

#ifdef __cplusplus
}
#endif

#endif /* __STM32091C_EVAL_H */

/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
