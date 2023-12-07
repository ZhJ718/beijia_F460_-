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
@FUNCTION：解析传入的字符串，是否满足大彩组态屏协议
@INPUT：待解析数据pBuf及长度pLen
@OUTPUT：pLen真实帧长度、pIsCutted是否被分包
@RETURN: TRUE-成功(分包或解析成功) FALSE-失败
@AUTHOR：xfw
@SPECIAL:新增对前条包不完整的情况处理，不考虑报文中出现0xEE
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
                    return FALSE;//出现包不完整的情况，不考虑报文中出现0xEE
                }
            }

            //防止无帧尾，导致当前缓冲区溢出
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
@FUNCTION：更新进度条控件数值
@INPUT：
	Screen_id(2 个字节)：画面编号
	Control_id(2 个字节)：控件编号
	Progressvalue（4 个字节） : 用户写入的新进度条值
@OUTPUT：
	pBuf:缓存组好的包
	pLen:包长度
@RETURN: TRUE-成功(分包或解析成功) FALSE-失败
@AUTHOR：xfw
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
@FUNCTION：更新文本控件数值
@INPUT：
	Screen_id(2 个字节)：画面编号
	Control_id(2 个字节)：控件编号
	pStr（不定长） : 文本框要显示的字符串，可以为空
@OUTPUT：
	pBuf:缓存组好的包
	pLen:包长度
@RETURN: TRUE-成功(分包或解析成功) FALSE-失败
@AUTHOR：xfw
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
@FUNCTION：获取文本控件数值
@INPUT：
	Screen_id(2 个字节)：画面编号
	Control_id(2 个字节)：控件编号
@OUTPUT：
	pBuf:缓存组好的包
	pLen:包长度
@RETURN: TRUE-成功(分包或解析成功) FALSE-失败
@AUTHOR：xfw
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
@FUNCTION：更新按钮状态
@INPUT：
	Screen_id(2 个字节)：画面编号
	Control_id(2 个字节)：控件编号
	Status（1 个字节） : 按钮状态，见ENUM_BUTTON_STA
		0x00：按钮由按下变成弹起
		0x01：按钮由弹起变成按下
@OUTPUT：
	pBuf:缓存组好的包
	pLen:包长度
@RETURN: TRUE-成功(分包或解析成功) FALSE-失败
@AUTHOR：xfw
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
@FUNCTION：更新按钮状态
@INPUT：
	Screen_id(2 个字节)：画面编号
	Control_id(2 个字节)：控件编号
	FlashImgae_ID（1 个字节） : 某一动画帧ID指定从某一帧开始播放 0-灰色 1-绿色 2-红色 3-黄色
@OUTPUT：
	pBuf:缓存组好的包
	pLen:包长度
@RETURN: TRUE-成功(分包或解析成功) FALSE-失败
@AUTHOR：xfw
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
@FUNCTION：大彩组态屏协议解析
@INPUT：待解析数据pBuf及长度Len
@OUTPUT：报文解析结构体
@RETURN:
@AUTHOR：xfw
@SPECIAL: 需要保证传入的报文是符合协议的，本函数不校验报文的正确性
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
@FUNCTION：数据记录控件-添加常规记录
@INPUT：
	Screen_id(2 个字节)：画面编号
	Control_id(2 个字节)：控件编号
	String（不定长） : 用户添加的字符串内容
@OUTPUT：
	pBuf:缓存组好的包
	pLen:包长度
@RETURN: TRUE-成功  FALSE-失败
@AUTHOR：xfw
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
@FUNCTION：数据记录控件-清除记录数据
@INPUT：
	Screen_id(2 个字节)：画面编号
	Control_id(2 个字节)：控件编号
@OUTPUT：
	pBuf:缓存组好的包
	pLen:包长度
@RETURN: TRUE-成功  FALSE-失败
@AUTHOR：xfw
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
