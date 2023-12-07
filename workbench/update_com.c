/******************************************************************
*版权所有 (C)2021, 江苏华如防务科技有限公司
*
*文件名称:  update_com.c
*文件标识:
*内容摘要:
*其它说明:
*当前版本:
*作    者:
*完成日期:  2021/07/02
*
*修改记录1:
*    修改日期:
*    版 本 号:
*    修 改 人:
*    修改内容:
******************************************************************/


/******************************************************************
*                             头文件                              *
******************************************************************/

#include "update_com.h"

#include "upgrade.h"
#include "malloc.h"

/****************************************************************
@FUNCTION：升级口设置最大缓存协议长度
@INPUT：
	UPCOM_MAG	-	通讯口管理指针，不能为空
    Len   - 最大长度
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 需要指明缓冲区最大缓存长度，最大不超过8k

****************************************************************/
void UpdateCom_SetMaxPackLen(UPDATECOM_MAG *pMag, uint32_t Len)
{
    assert_param(pMag != NULL);

    pMag->m_wProtBufMaxLen = MIN(Len, 8 * 1024);
}

/****************************************************************
@FUNCTION：升级口初始化
@INPUT：
	UPCOM_HW_OPERATIONS - 上行通讯口相关底层操作函数
	UPCOM_MAG	-	上行通讯口管理指针，不能为空
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 需要指明缓冲区最大缓存长度

****************************************************************/
void UpdateCom_Init(UPDATECOM_HW_OPERATIONS a_sHwOps, UPDATECOM_MAG *pMag)
{
    assert_param(pMag != NULL);

    pMag->m_sHwOps.hw_init = a_sHwOps.hw_init;
    pMag->m_sHwOps.read = a_sHwOps.read;
    pMag->m_sHwOps.write = a_sHwOps.write;

    if(pMag->m_sHwOps.hw_init != NULL)
    {
        pMag->m_sHwOps.hw_init();
    }

    pMag->m_pProtBuf = (uint8_t *)sdram_malloc(0, pMag->m_wProtBufMaxLen);

    pMag->m_bIsInit = (pMag->m_pProtBuf == NULL ? FALSE : TRUE);


}

/****************************************************************
@FUNCTION：升级口初始化销毁
@INPUT：
	UPCOM_MAG	-	上行通讯口管理指针，不能为空
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 善始善终

****************************************************************/
void UpdateCom_DeInit(UPDATECOM_MAG *pMag)
{
    assert_param(pMag != NULL);


    if(pMag->m_pProtBuf != NULL)
    {
        //释放申请内存
        sdram_free(0, pMag->m_pProtBuf);
    }

    memset((void *)pMag, 0, sizeof(UPDATECOM_MAG));
}

/****************************************************************
@FUNCTION：升级串口接收处理进程
@INPUT：
	UPDATECOM_MAG	-	升级串口管理指针
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 支持升级

****************************************************************/
void UpdateCom_RecvProcess(UPDATECOM_MAG *pMag)
{
    uint16_t Len = pMag->m_wProtBufMaxLen - pMag->m_wProtBufLen;
    uint16_t i, DataLen;
    uint32_t IsCutted = FALSE, IsLong55AA = FALSE;

    if(FALSE == pMag->m_bIsInit)
    {
        return;
    }

    if(TRUE == pMag->m_sHwOps.read(&pMag->m_pProtBuf[pMag->m_wProtBufLen], &Len))
    {
        pMag->m_wProtBufLen += Len;

        pMag->m_dwNoCommTimeout = 0;

        for(i = 0; i < pMag->m_wProtBufLen;)
        {
            Len = pMag->m_wProtBufLen - i;

            if(TRUE == Update_ProtocalParse(&pMag->m_pProtBuf[i], &Len, &IsCutted, &IsLong55AA))
            {
                if(TRUE == IsCutted)
                {
                    if(pMag->m_wProtBufLen == pMag->m_wProtBufMaxLen)
                    {
                        //如果发现未处理的数据填满了缓冲区，且报文是被截断的，则清除该缓冲区数据
                        pMag->m_wProtBufLen = 0;
                        break;
                    }

                    //LOG("parse Cutted,i=%d,Len=%d\n\r",i,Len);
                    memcpy(pMag->m_pProtBuf, &pMag->m_pProtBuf[i], pMag->m_wProtBufLen - i);
                    pMag->m_wProtBufLen -= i;
                    break;
                }
                else
                {
                    Update_ConfigSendFunc(pMag->m_sHwOps.write);

                    //发现为55AA长包
                    if (IsLong55AA)
                    {
                        DataLen	= pMag->m_pProtBuf[i + 3] | (pMag->m_pProtBuf[i + 4] << 8);
                        Update_ProtocalProcess(&pMag->m_pProtBuf[i + 7], DataLen);
                    }
                    else
                    {
                        //LOG("parse succ,i=%d,Len=%d\n\r",i,Len);
                        Update_ProtocalProcess(&pMag->m_pProtBuf[i + 3], pMag->m_pProtBuf[i + 2]);
                    }

                    i = Len + i;
                }
            }
            else
            {
                i++;
            }

            if(i == pMag->m_wProtBufLen)
            {
                pMag->m_wProtBufLen = 0;
            }
        }


    }
}



/****************************************************************
@FUNCTION：升级串口接收处理 - 找到符合协议格式的报文
@INPUT：
	UPDATECOM_MAG	-	升级串口管理指针
@OUTPUT: 无
@RETURN: 0 -  未找到,  1 - 找到
@AUTHOR：xfw
@SPECIAL:

****************************************************************/
uint32_t UpdateCom_Find55AAFrame(UPDATECOM_MAG *pMag)
{
    uint16_t Len = pMag->m_wProtBufMaxLen - pMag->m_wProtBufLen;
    uint16_t i, DataLen;
    uint32_t IsCutted = FALSE, IsLong55AA = FALSE;

    if(NULL == pMag->m_pProtBuf)
    {
        return 0;
    }

    if(TRUE == pMag->m_sHwOps.read(&pMag->m_pProtBuf[pMag->m_wProtBufLen], &Len))
    {
        pMag->m_wProtBufLen += Len;

        pMag->m_dwNoCommTimeout = 0;

        for(i = 0; i < pMag->m_wProtBufLen;)
        {
            Len = pMag->m_wProtBufLen - i;

            if(TRUE == Update_ProtocalParse(&pMag->m_pProtBuf[i], &Len, &IsCutted, &IsLong55AA))
            {
                if(TRUE == IsCutted)
                {
                    //LOG("parse Cutted,i=%d,Len=%d\n\r",i,Len);
                    memcpy(pMag->m_pProtBuf, &pMag->m_pProtBuf[i], pMag->m_wProtBufLen - i);
                    pMag->m_wProtBufLen -= i;
                    break;
                }
                else
                {
                    //发现为55AA长包

                    i = Len + i;

                    return 1;
                }
            }
            else
            {
                i++;
            }

            if(i == pMag->m_wProtBufLen)
            {
                pMag->m_wProtBufLen = 0;
            }
        }
    }

    return 0;
}

