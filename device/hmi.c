/**
  ******************************************************************************
  * @file    hmi.c
  * @author  xfw
  * @brief   This file contains definitions for human machine interface
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2019 Beijing HuaRu Tech Co.</center></h2>
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
#include "hmi.h"


/****************************************************************
@FUNCTION������������ַ������Ƿ���������̬��Э��
@INPUT������������pBuf������pLen
@OUTPUT��pLen��ʵ֡���ȡ�pIsCutted�Ƿ񱻷ְ�
@RETURN: TRUE-�ɹ�(�ְ�������ɹ�) FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:������ǰ������������������������Ǳ����г���0xEE
****************************************************************/
uint32_t HMI_FindFrame(uint8_t *pBuf, uint16_t *pLen, uint32_t *pIsCutted)
{
    uint8_t i;

    if (pIsCutted != NULL)
    {
        *pIsCutted	= FALSE;
    }

    if((pBuf != NULL) && (pLen != NULL))
    {
        if(pBuf[0] == 0xEE)
        {
            if(*pLen < 4)
            {
                if(pIsCutted != NULL)
                {
                    *pIsCutted = TRUE;
                }

                return	TRUE;
            }

            for(i = 1; i < *pLen; i++)
            {
                if((pBuf[i] == 0xFF) && (pBuf[i + 1] == 0xFC) && (pBuf[i + 2] == 0xFF) && (pBuf[i + 3] == 0xFF))
                {
                    *pLen = i + 4;
                    return TRUE;
                }
                else if(pBuf[i] == 0xEE)
                {
                    return FALSE;//���ְ�������������������Ǳ����г���0xEE
                }
            }

            //��ֹ��֡β�����µ�ǰ���������
            //if(*pLen<25){
            //	if(pIsCutted!=NULL){
            //		*pIsCutted = TRUE;
            //	}
            //	return	TRUE;
            //}
        }
    }

    return FALSE;
}

/****************************************************************
@FUNCTION�����½������ؼ���ֵ
@INPUT��
	Screen_id(2 ���ֽ�)��������
	Control_id(2 ���ֽ�)���ؼ����
	Progressvalue��4 ���ֽڣ� : �û�д����½�����ֵ
@OUTPUT��
	pBuf:������õİ�
	pLen:������
@RETURN: TRUE-�ɹ�(�ְ�������ɹ�) FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint32_t HMI_SetProgress(uint16_t Screen_id, uint16_t Control_id, uint16_t ProgressValue, uint8_t *pBuf, uint16_t *pLen)
{
    uint16_t len = 0;
    assert_param((pBuf != NULL) && (pLen != NULL));

    if(*pLen >= 15)
    {
        pBuf[len++] = 0xEE;
        pBuf[len++] = 0xB1;
        pBuf[len++] = 0x10;
        pBuf[len++] = (Screen_id >> 8) & 0xff;
        pBuf[len++] = (Screen_id >> 0) & 0xff;
        pBuf[len++] = (Control_id >> 8) & 0xff;
        pBuf[len++] = (Control_id >> 0) & 0xff;
        pBuf[len++] = (ProgressValue >> 24) & 0xff;
        pBuf[len++] = (ProgressValue >> 16) & 0xff;
        pBuf[len++] = (ProgressValue >> 8) & 0xff;
        pBuf[len++] = (ProgressValue >> 0) & 0xff;
        pBuf[len++] = 0xFF;
        pBuf[len++] = 0xFC;
        pBuf[len++] = 0xFF;
        pBuf[len++] = 0xFF;
        *pLen = len;
        return TRUE;
    }

    return FALSE;
}

/****************************************************************
@FUNCTION�������ı��ؼ���ֵ
@INPUT��
	Screen_id(2 ���ֽ�)��������
	Control_id(2 ���ֽ�)���ؼ����
	pStr���������� : �ı���Ҫ��ʾ���ַ���������Ϊ��
@OUTPUT��
	pBuf:������õİ�
	pLen:������
@RETURN: TRUE-�ɹ�(�ְ�������ɹ�) FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint32_t HMI_SetText(uint16_t Screen_id, uint16_t Control_id, uint8_t *pStr, uint8_t *pBuf, uint16_t *pLen)
{
    uint16_t len = 0;
    uint8_t slen = 0;
    assert_param((pBuf != NULL) && (pLen != NULL));

    if(*pLen >= 11)
    {
        pBuf[len++] = 0xEE;
        pBuf[len++] = 0xB1;
        pBuf[len++] = 0x10;
        pBuf[len++] = (Screen_id >> 8) & 0xff;
        pBuf[len++] = (Screen_id >> 0) & 0xff;
        pBuf[len++] = (Control_id >> 8) & 0xff;
        pBuf[len++] = (Control_id >> 0) & 0xff;

        if(pStr != NULL)
        {
            slen = strlen((char *)pStr);

            if(*pLen >= (11 + slen))
            {
                strcpy((char *)&pBuf[len], (char *)pStr);
                len += slen;
            }
            else
            {
                return FALSE;
            }
        }

        pBuf[len++] = 0xFF;
        pBuf[len++] = 0xFC;
        pBuf[len++] = 0xFF;
        pBuf[len++] = 0xFF;
        *pLen = len;
        return TRUE;
    }

    return FALSE;
}

/****************************************************************
@FUNCTION����ȡ�ı��ؼ���ֵ
@INPUT��
	Screen_id(2 ���ֽ�)��������
	Control_id(2 ���ֽ�)���ؼ����
@OUTPUT��
	pBuf:������õİ�
	pLen:������
@RETURN: TRUE-�ɹ�(�ְ�������ɹ�) FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint32_t HMI_GetText(uint16_t Screen_id, uint16_t Control_id, uint8_t *pBuf, uint16_t *pLen)
{
    uint16_t len = 0;

    assert_param((pBuf != NULL) && (pLen != NULL));

    if(*pLen >= 11)
    {
        pBuf[len++] = 0xEE;
        pBuf[len++] = 0xB1;
        pBuf[len++] = 0x11;
        pBuf[len++] = (Screen_id >> 8) & 0xff;
        pBuf[len++] = (Screen_id >> 0) & 0xff;
        pBuf[len++] = (Control_id >> 8) & 0xff;
        pBuf[len++] = (Control_id >> 0) & 0xff;
        pBuf[len++] = 0xFF;
        pBuf[len++] = 0xFC;
        pBuf[len++] = 0xFF;
        pBuf[len++] = 0xFF;
        *pLen = len;
        return TRUE;
    }

    return FALSE;
}

/****************************************************************
@FUNCTION�����°�ť״̬
@INPUT��
	Screen_id(2 ���ֽ�)��������
	Control_id(2 ���ֽ�)���ؼ����
	Status��1 ���ֽڣ� : ��ť״̬����ENUM_BUTTON_STA
		0x00����ť�ɰ��±�ɵ���
		0x01����ť�ɵ����ɰ���
@OUTPUT��
	pBuf:������õİ�
	pLen:������
@RETURN: TRUE-�ɹ�(�ְ�������ɹ�) FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint32_t HMI_SetButton(uint16_t Screen_id, uint16_t Control_id, ENUM_BUTTON_STA Status, uint8_t *pBuf, uint16_t *pLen)
{
    uint16_t len = 0;
    assert_param((pBuf != NULL) && (pLen != NULL));

    if(*pLen >= 15)
    {
        pBuf[len++] = 0xEE;
        pBuf[len++] = 0xB1;
        pBuf[len++] = 0x10;
        pBuf[len++] = (Screen_id >> 8) & 0xff;
        pBuf[len++] = (Screen_id >> 0) & 0xff;
        pBuf[len++] = (Control_id >> 8) & 0xff;
        pBuf[len++] = (Control_id >> 0) & 0xff;
        pBuf[len++] = (uint8_t)Status;
        pBuf[len++] = 0xFF;
        pBuf[len++] = 0xFC;
        pBuf[len++] = 0xFF;
        pBuf[len++] = 0xFF;
        *pLen = len;
        return TRUE;
    }

    return FALSE;
}

/****************************************************************
@FUNCTION�����°�ť״̬
@INPUT��
	Screen_id(2 ���ֽ�)��������
	Control_id(2 ���ֽ�)���ؼ����
	FlashImgae_ID��1 ���ֽڣ� : ĳһ����֡IDָ����ĳһ֡��ʼ���� 0-��ɫ 1-��ɫ 2-��ɫ 3-��ɫ
@OUTPUT��
	pBuf:������õİ�
	pLen:������
@RETURN: TRUE-�ɹ�(�ְ�������ɹ�) FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint32_t HMI_SetFlashImage(uint16_t Screen_id, uint16_t Control_id, ENUM_FLASHIMAGE_STA FlashImgae_ID, uint8_t *pBuf, uint16_t *pLen)
{
    uint16_t len = 0;
    assert_param((pBuf != NULL) && (pLen != NULL));

    if(*pLen >= 15)
    {
        pBuf[len++] = 0xEE;
        pBuf[len++] = 0xB1;
        pBuf[len++] = 0x23;
        pBuf[len++] = (Screen_id >> 8) & 0xff;
        pBuf[len++] = (Screen_id >> 0) & 0xff;
        pBuf[len++] = (Control_id >> 8) & 0xff;
        pBuf[len++] = (Control_id >> 0) & 0xff;
        pBuf[len++] = (uint8_t)FlashImgae_ID;
        pBuf[len++] = 0xFF;
        pBuf[len++] = 0xFC;
        pBuf[len++] = 0xFF;
        pBuf[len++] = 0xFF;
        *pLen = len;
        return TRUE;
    }

    return FALSE;
}

/****************************************************************
@FUNCTION�������̬��Э�����
@INPUT������������pBuf������Len
@OUTPUT�����Ľ����ṹ��
@RETURN:
@AUTHOR��xfw
@SPECIAL: ��Ҫ��֤����ı����Ƿ���Э��ģ���������У�鱨�ĵ���ȷ��
***************************************************************
void HMI_FrameHandle(uint8_t *pBuf,uint16_t Len,HMI_MANAGER *pMag)

{
	uint8_t CmdParaLen;
	uint8_t Cmd;

	assert_param((pBuf!=NULL) && (pMag!=NULL));

	if(Len<6)
		return;


	Cmd = pBuf[1];
	CmdParaLen = Len-6;


}

*/

/****************************************************************
@FUNCTION�����ݼ�¼�ؼ�-��ӳ����¼
@INPUT��
	Screen_id(2 ���ֽ�)��������
	Control_id(2 ���ֽ�)���ؼ����
	String���������� : �û���ӵ��ַ�������
@OUTPUT��
	pBuf:������õİ�
	pLen:������
@RETURN: TRUE-�ɹ�  FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint32_t HMI_DatRcd_Insert(uint16_t Screen_id, uint16_t Control_id, uint8_t *pStr, uint8_t *pBuf, uint16_t *pLen)
{
    uint16_t len = 0;
    uint8_t slen = 0;
    assert_param((pBuf != NULL) && (pLen != NULL));

    if(*pLen >= 11)
    {
        pBuf[len++] = 0xEE;
        pBuf[len++] = 0xB1;
        pBuf[len++] = 0x52;
        pBuf[len++] = (Screen_id >> 8) & 0xff;
        pBuf[len++] = (Screen_id >> 0) & 0xff;
        pBuf[len++] = (Control_id >> 8) & 0xff;
        pBuf[len++] = (Control_id >> 0) & 0xff;

        if(pStr != NULL)
        {
            slen = strlen((char *)pStr);

            if(*pLen >= (11 + slen))
            {
                strcpy((char *)&pBuf[len], (char *)pStr);
                len += slen;
            }
            else
            {
                return FALSE;
            }
        }

        pBuf[len++] = 0xFF;
        pBuf[len++] = 0xFC;
        pBuf[len++] = 0xFF;
        pBuf[len++] = 0xFF;
        *pLen = len;
        return TRUE;
    }

    return FALSE;
}

/****************************************************************
@FUNCTION�����ݼ�¼�ؼ�-�����¼����
@INPUT��
	Screen_id(2 ���ֽ�)��������
	Control_id(2 ���ֽ�)���ؼ����
@OUTPUT��
	pBuf:������õİ�
	pLen:������
@RETURN: TRUE-�ɹ�  FALSE-ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint32_t HMI_DatRcd_Reset(uint16_t Screen_id, uint16_t Control_id, uint8_t *pBuf, uint16_t *pLen)
{
    uint16_t len = 0;
    uint8_t slen = 0;
    assert_param((pBuf != NULL) && (pLen != NULL));

    if(*pLen >= 11)
    {
        pBuf[len++] = 0xEE;
        pBuf[len++] = 0xB1;
        pBuf[len++] = 0x53;
        pBuf[len++] = (Screen_id >> 8) & 0xff;
        pBuf[len++] = (Screen_id >> 0) & 0xff;
        pBuf[len++] = (Control_id >> 8) & 0xff;
        pBuf[len++] = (Control_id >> 0) & 0xff;

        pBuf[len++] = 0xFF;
        pBuf[len++] = 0xFC;
        pBuf[len++] = 0xFF;
        pBuf[len++] = 0xFF;
        *pLen = len;
        return TRUE;
    }

    return FALSE;
}


/************************ (C) COPYRIGHT Beijing HuaRu Tech Co. *****END OF FILE****/
