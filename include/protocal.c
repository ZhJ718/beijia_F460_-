/******************************************************************
*版权所有 (C)2021, 江苏华如防务科技有限公司
*
*文件名称:  protocal.c
*文件标识:
*内容摘要:
*其它说明:
*当前版本:
*作    者: 
*完成日期:  2021/4/23
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

#include "protocal.h"

/****************************************************************
@FUNCTION：计算CRC8
@INPUT：a_byBase-生成多项式 a_pBuf-数据指针 a_BufLen-数据长度
@OUTPUT：无
@RETURN: CRC8结果
@AUTHOR：xfw
@SPECIAL:生成多项式
CRC-8       x8+x5+x4+1              0x31（0x131）
CRC-8       x8+x2+x1+1              0x07（0x107）
CRC-8       x8+x6+x4+x3+x2+x1       0x5E（0x15E）

****************************************************************/
uint8_t Crc8_Calc(uint8_t a_byBase,uint8_t *a_pBuf,uint8_t a_byBufLen)
{
	uint8_t i,crc;

	crc = 0;

	while(a_byBufLen--)
	{
		crc ^= *a_pBuf++;
		
		for(i=0;i<8;i++)
		{
			if(crc&0x80)
			{
				crc = (crc<<1)^a_byBase;
			}
			else
			{
				crc <<= 1;
			}
		}
	}
	
	return crc;
}




/*
************************************************************************************************************************************************************************     
*函数名称: Crc16_Calc
*功能描述: 计算Modbus的CRC校验
*输入描述: 待计算校验报文、计算长度
*输出描述: 无
*返回描述: 双字节校验值
*作者日期: 
*全局声明: 无
*特殊说明: 适用于下行Modbus校验，Modbus协议传输时，返回值低位先发，高位后发
************************************************************************************************************************************************************************       
*/
extern uint16_t Crc16_Calc(const uint8_t *pBuf, uint32_t Len)
{
	uint32_t	i, j;
	uint16_t	wCrc = 0xFFFF;
	
	for (i = 0; i < Len; i ++)
	{
		wCrc ^= (uint16_t)pBuf[i];
		
		for (j = 0; j < 8; j ++)
		{
			if (wCrc & 1)
			{
				wCrc >>= 1; 
				wCrc ^= 0xA001; 
			}
			else
			{
				wCrc >>= 1; 
			}
		}
	}
	
	return wCrc;
}

/****************************************************************
@FUNCTION：计算异或和
@INPUT：a_pBuf-数据指针 a_BufLen-数据长度
@OUTPUT：无
@RETURN: 结果
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
extern uint8_t Xor_Calc(uint8_t *a_pBuf, uint16_t a_BufLen)
{
    uint8_t Xor_Ret = 0;
    uint16_t i;

    for(i = 0; i < a_BufLen; i++)
    {
        Xor_Ret ^= a_pBuf[i];
    }

    return Xor_Ret;
}

//计算crc校验函数
unsigned int Rx_CRC_CCITT(unsigned char *puchMsg, unsigned int usDataLen)
{
    unsigned char i = 0;
    unsigned short wCRCin = 0x0000;
    unsigned short wCPoly = 0x1021;
    unsigned char wChar = 0;

    while (usDataLen--)
    {
        wChar = *(puchMsg++);
        wCRCin ^= (wChar << 8);

        for(i = 0; i < 8; i++)
        {
            if (wCRCin & 0x8000)
                wCRCin = (wCRCin << 1) ^ wCPoly;
            else
                wCRCin = wCRCin << 1;
        }
    }

    return (wCRCin);
}

/*
************************************************************************************************************************************************************************     
*函数名称: Protocal_MakeU8Sum
*功能描述: 计算校验和，U8
*输入描述: 待计算校验报文、计算长度
*输出描述: 无
*返回描述: 单字节校验和
*作者日期: XFW 
*全局声明: 无
*特殊说明: 
************************************************************************************************************************************************************************       
*/
extern uint8_t Protocal_MakeU8Sum(const uint8_t *byBuf,const uint32_t dwCnt)
{
	uint8_t bySum=0;
	uint32_t Cnt=dwCnt;
    
	while(Cnt--){
		bySum += *byBuf;
		byBuf++;
	}
        
    return bySum;
}



extern uint32_t Protocal_GetU16Sum(const uint16_t *wBuf,const uint32_t dwCnt)
{
	uint32_t Sum=0;
	uint32_t Cnt=dwCnt;
    
	while(Cnt--){
		Sum += *wBuf;
		wBuf++;
	}
        
    return Sum;
}

extern uint32_t Protocal_MakeU32Sum(const uint8_t *byBuf,const uint32_t dwCnt)
{
	uint32_t dwSum=0;
	uint32_t Cnt=dwCnt;
    
	while(Cnt--){
		dwSum += *byBuf;
		byBuf++;
	}
        
    return dwSum;
}




uint8_t GetProtAddr(void)
{
	return 0x01;
}

//寻找下行指令报文
uint8_t Protocol_FindC1XORFrame(const uint8_t *pBuf, uint16_t *pLen, uint32_t *pIsCutted, uint16_t MtuSize)
{

    uint8_t len;
    uint8_t i = 0, Xor = 0;

    if((pBuf != NULL) && (pLen != NULL))
    {
        if(pBuf[0] == 0xC1)
        {
            if(*pLen < 4)
            {
                if(pIsCutted != NULL)
                {
                    *pIsCutted = TRUE;
                }

                return	TRUE;
            }

            len = pBuf[2];//帧长

            for(i = 3; i < len + 3; i++)
            {
                Xor ^= pBuf[i];
            }

            if(len < 255)
            {
                if(len > *pLen)
                {
                    if(pIsCutted != NULL)
                    {
                        *pIsCutted = TRUE;
                    }

                    return	TRUE;
                }
                else if(pBuf[len + 3] == Xor)
                {
                    *pLen = len + 4;

                    if(pIsCutted != NULL)
                    {
                        *pIsCutted = FALSE;
                    }

                    return TRUE;
                }
            }
        }
    }

    if (pIsCutted != NULL)
    {
        *pIsCutted	= FALSE;
    }

    return FALSE;
}

/*
************************************************************************************************************************************************************************     
*函数名称: Protocal_Find55AAFrame
*功能描述: 解析55AA协议报文
*输入描述: 
	@@@ pBuf-报文
	@@@ pLen - 报文长度
	@@@ MtuSize - 缓冲临界值，55AA协议长包数据段最长达0xFFFF-9
*输出描述: 
	@@@ pIsCutted - 是否不完整
	@@@ pIsLong55AA - 是否是长包
*返回描述: TRUE - 传入报文满足协议 ， FALSE - 传入报文不满足协议
*作者日期: XFW 
*全局声明: 无
*特殊说明: 
************************************************************************************************************************************************************************       
*/
extern uint32_t Protocal_Find55AAFrame(const uint8_t *pBuf, uint16_t *pLen, uint32_t *pIsCutted, uint32_t *pIsLong55AA, uint16_t MtuSize)
{
	uint16_t		DataLen;

	if ((pBuf!=NULL) && (pLen!=NULL))
	{
		if (pBuf[0]==0x55)		
		{
			if (*pLen<3)
			{
				if (pIsCutted!=NULL)
				{
					*pIsCutted	= TRUE;
				}

				return	TRUE;
			}
			
			if (pBuf[2]==0)		
			{
				if (*pLen<7)
				{
					if (pIsCutted!=NULL)
					{
						*pIsCutted	= TRUE;
					}

					return	TRUE;
				}
				else if (((pBuf[3]+pBuf[5])==0xFF) && ((pBuf[4]+pBuf[6])==0xFF))
				{
					DataLen		= (pBuf[4]<<8)|pBuf[3];
					if (DataLen>MtuSize)
					{
						return	FALSE;
					}

					if (*pLen<(uint16_t)(DataLen+9))
					{
						if (pIsCutted!=NULL)
						{
							*pIsCutted	= TRUE;
						}

						return	TRUE;
					}
					else
					{
						if ((Protocal_MakeU8Sum(&pBuf[7], DataLen)==pBuf[7+DataLen]) && (pBuf[8+DataLen]==0xAA))
						{
							*pLen		= DataLen+9;
							if (pIsCutted!=NULL)
							{
								*pIsCutted		= FALSE;
							}

							if (pIsLong55AA!=NULL)
							{
								*pIsLong55AA	= TRUE;
							}
							
							return	TRUE;
						}
					
					}
				}
			}
			else
			{
				DataLen		= pBuf[2];
				if (*pLen<(uint16_t)(DataLen+5))	
				{
					if (pIsCutted!=NULL)
					{
						*pIsCutted	= TRUE;
					}

					return	TRUE;
				}
				else
				{
					if ((Protocal_MakeU8Sum(&pBuf[3], DataLen)==pBuf[3+DataLen]) && (pBuf[4+DataLen]==0xAA))
					{
						*pLen		= DataLen+5;
						if (pIsCutted!=NULL)
						{
							*pIsCutted		= FALSE;
						}

						if (pIsLong55AA!=NULL)
						{
							*pIsLong55AA	= FALSE;
						}

						return	TRUE;
					}
					
				}
			}
		}
	}

	if (pIsCutted!=NULL)
	{
		*pIsCutted	= FALSE;
	}

	return	FALSE;
}

/*
************************************************************************************************************************************************************************     
*函数名称: Protocal_ProtocolMakeLong55AA
*功能描述: 55AA长报文组包
*输入描述: 
	@@@ pbyData - 数据载荷
	@@@ wDataLen - 数据长度
*输出描述: 
	@@@ pbyBuf - 报文
	@@@ pwDataLen - 报文长度
*返回描述: TRUE - 成功 ， FALSE - 失败
*作者日期: XFW 
*全局声明: 无
*特殊说明: 
************************************************************************************************************************************************************************       
*/
extern uint32_t Protocal_ProtocolMakeLong55AA(const uint8_t *pbyData, uint16_t wDataLen,
					uint8_t *pbyBuf, uint16_t *pwDataLen)
{
	//uint8_t		TempBuf[UESRUART_DOWMCOM_RECV_BUFFER_LEN+9];
	//uint16_t	DataLen;

	if(pbyData==NULL || pbyBuf==NULL || pwDataLen==NULL)
		return FALSE;
	if(wDataLen+9>*pwDataLen)
		return FALSE;

	memcpy(&pbyBuf[7], pbyData, wDataLen); 
	pbyBuf[0] = 0x55;
	pbyBuf[1] = GetProtAddr();
	pbyBuf[2] = 0;
	pbyBuf[3] = LOBYTE(wDataLen);
	pbyBuf[4] = HIBYTE(wDataLen);
	pbyBuf[5] = ~(LOBYTE(wDataLen));
	pbyBuf[6] = ~(HIBYTE(wDataLen));
	pbyBuf[wDataLen+7] = Protocal_MakeU8Sum(&pbyBuf[7],wDataLen);
	pbyBuf[wDataLen+8] = 0xAA;
	*pwDataLen = wDataLen+9;
	return TRUE;
}

/*
************************************************************************************************************************************************************************     
*函数名称: Protocal_ProtocolMakeShort55AA
*功能描述: 55AA短报文组包
*输入描述: 
	@@@ pbyData - 数据载荷
	@@@ wDataLen - 数据长度
*输出描述: 
	@@@ pbyBuf - 报文
	@@@ pwDataLen - 报文长度
*返回描述: TRUE - 成功 ， FALSE - 失败
*作者日期: XFW 
*全局声明: 无
*特殊说明: 
************************************************************************************************************************************************************************       
*/
extern uint32_t Protocal_ProtocolMakeShort55AA(const uint8_t *pbyData, uint16_t wDataLen,
					uint8_t *pbySendBuf, uint16_t *pwSendLen)
{
	//uint8_t		TempBuf[UESRUART_DOWMCOM_RECV_BUFFER_LEN+9];
	//uint16_t	DataLen;

	if(wDataLen==0)
		return FALSE;
	if(wDataLen+6>*pwSendLen)
		return FALSE;

	memcpy(&pbySendBuf[3], pbyData, wDataLen);
	pbySendBuf[0] = 0x55;
	pbySendBuf[1] = GetProtAddr();
	pbySendBuf[2] = wDataLen;
	
	pbySendBuf[wDataLen+3] = Protocal_MakeU8Sum(&pbySendBuf[3],wDataLen);
	pbySendBuf[wDataLen+4] = 0xAA;
	*pwSendLen = wDataLen+5;
	return TRUE;
}

/*
************************************************************************************************************************************************************************     
*函数名称: Protocal_Find5AAAFrame
*功能描述: 解析报文是否满足5AAA协议
*输入描述: 
	@@@ pBuf - 数据载荷
	@@@ pLen - 数据长度
*输出描述: 
	@@@ pIsCutted - 符合协议，但包不完整
*返回描述: TRUE - 成功 ， FALSE - 失败
*作者日期: XFW 
*全局声明: 无
*特殊说明: 
************************************************************************************************************************************************************************       
*/
uint8_t Protocal_Find5AAAFrame(uint8_t *pBuf,uint16_t *pLen,uint32_t *pIsCutted)
{

	uint16_t len; 

	if((pBuf!=NULL) && (pLen!=NULL)){
		if(pBuf[0]==0x5A){
			if(*pLen<4){
				if(pIsCutted!=NULL){
					*pIsCutted = TRUE;
				}
				return	TRUE;
			}
			len = pBuf[1];//帧长
			//if(len<20){
				if(len>*pLen){
					if(pIsCutted!=NULL){
						*pIsCutted = TRUE;
					}
					return	TRUE;
				}else if(pBuf[len-1]==0xAA){
					*pLen = len;
					if(pIsCutted!=NULL){
						*pIsCutted = FALSE;
					}
					return TRUE;
				}
			//}
			#if 0
			else{
				if(pBuf[len-1]==0xAA){
					*pLen = len;
					if(pIsCutted!=NULL){
						*pIsCutted = FALSE;
					}
					return TRUE;
				}
			}
			#endif
		}
	}
	if (pIsCutted!=NULL){
		*pIsCutted	= FALSE;
	}
	return FALSE;
}

/*
************************************************************************************************************************************************************************     
*函数名称: Protocal_FindNMEA0183String
*功能描述: 解析报文是否满足NMEA协议
*输入描述: 
	@@@ pBuf - 数据载荷
	@@@ pLen - 数据长度
*输出描述: 
	@@@ pIsCutted - 符合协议，但包不完整
*返回描述: TRUE - 成功 ， FALSE - 失败
*作者日期: XFW 
*全局声明: 无
*特殊说明: 
************************************************************************************************************************************************************************       
*/
uint32_t Protocal_FindNMEA0183String(uint8_t *pBuf, uint16_t *pLen, uint32_t *pIsCutted)
{
    //uint8_t *p1=NULL;
    uint8_t *p2 = NULL;
    uint8_t i, chk;

    if((pBuf != NULL) && (pLen != NULL))
    {
        if(pBuf[0] == '$')
        {
            if(*pLen < 4)
            {
                if(pIsCutted != NULL)
                {
                    *pIsCutted = TRUE;
                }

                return	TRUE;
            }

            p2 = (uint8_t*)strstr((const char *)pBuf, "*");

            if(p2 == NULL)
            {
                if(pIsCutted != NULL)
                {
                    *pIsCutted = TRUE;
                }

                return	TRUE;
            }

            for(i = 0, chk = 0; i < (p2 - pBuf - 1); i++)
            {
                chk ^= pBuf[i + 1];
            }

            if(chk == strtoul((const char *)p2 + 1, NULL, 16))
            {
                *pLen = p2 - pBuf + 3;

                if(pIsCutted != NULL)
                {
                    *pIsCutted = FALSE;
                }

                return TRUE;
            }


        }
    }

    if (pIsCutted != NULL)
    {
        *pIsCutted	= FALSE;
    }

    /*
    p1=(uint8_t*)strstr((const char *)buf,(const char *)str);
    if(p1!=NULL){
    	*ppStart = p1;


    	if(p2==NULL){
    		return 1;
    	}
    	*ppEnd = p2;
    	for(i=0,chk=0;i<(p2-p1-1);i++){
    		chk ^= p1[i+1];
    	}
    	if(chk == strtoul((const char *)p2+1,NULL,16)){
    		return 0;
    	}
    }*/
    return FALSE;
}

/****************************************************************
@FUNCTION：寻找红外短距的报文
@INPUT：
    @param pBuf-数据指针 
    @param pLen-数据长度
@OUTPUT：符合协议报文长度
@RETURN: TRUE - 成功 ， FALSE - 失败
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint8_t Protocal_FindIR38_Frame(uint8_t *pBuf, uint16_t *pLen)
{
    uint8_t Xor_Ret = 0;
    uint16_t Len;

    if(pBuf == NULL || *pLen < 5)
    {
        return FALSE;
    }

    if(pBuf[0] < 0xA2 || pBuf[0] > 0xA3 )
    {
        return FALSE;
    }

    Len = pBuf[2];

    if( ( Len == 0 ) || ( Len >= 0xff )
            || ( *pLen < Len + 4 ) )
    {
        return FALSE;
    }

    Xor_Ret = Xor_Calc(&pBuf[3], Len);

    if(Xor_Ret == pBuf[Len + 3])
    {
        *pLen = Len + 4;
        
        return TRUE;
    }

    return FALSE;
}

/****************************************************************
@FUNCTION：寻找符合互联互通协议的报文
@INPUT：
    @param pBuf-数据指针 
    @param pLen-数据长度
@OUTPUT：符合协议报文长度
@RETURN: TRUE - 成功 ， FALSE - 失败
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint8_t Protocal_FindInterConn_Frame(uint8_t *pBuf, uint16_t *pLen)
{
    uint8_t Xor_Ret = 0;
    uint16_t Len;

    if(pBuf == NULL || *pLen < 5)
    {
        return FALSE;
    }

    if(pBuf[0] < 0xA0)
    {
        return FALSE;
    }

    Len = pBuf[2];

    if( ( Len == 0 ) || ( Len >= 0xff )
            || ( *pLen < Len + 4 ) )
    {
        return FALSE;
    }

    Xor_Ret = Xor_Calc(&pBuf[3], Len);

    if(Xor_Ret == pBuf[Len + 3])
    {
        *pLen = Len + 4;
        
        return TRUE;
    }

    return FALSE;
}

/****************************************************************
@FUNCTION：解析传入的字符串，是否满足乐升TFT串口屏通信协议
@INPUT：待解析数据pBuf及长度pLen
@OUTPUT：pLen真实帧长度、pIsCutted是否被分包
@RETURN: TRUE-成功(分包或解析成功) FALSE-失败
@AUTHOR：xfw
@SPECIAL:  8D、BF指令不适用
****************************************************************/
uint32_t Protocal_FindHmi268bFrame(uint8_t *pBuf, uint16_t *pLen, uint32_t *pIsCutted)
{
    uint8_t i,off,minL;

    if (pIsCutted != NULL)
    {
        *pIsCutted	= FALSE;
    }

    if((pBuf != NULL) && (pLen != NULL))
    {
        if(pBuf[0] == 0xAA)
        {
            if(*pLen < 10)
            {
                if(pIsCutted != NULL)
                {
                    *pIsCutted = TRUE;
                }

                return	TRUE;
            }
            
            switch(pBuf[1])
            {
                case 0x8D:
                    off = 6+7;
                    minL = 10+7;
                    break;
                case 0xBF:
                    off = 6+46;
                    minL = 10+46;
                    break;    
                default:
                    off = 6;
                    minL = 10;
                    break;
            }

            if(*pLen >= minL)
            {   
                if(pBuf[off] == 0xE4 && pBuf[off+1] == 0x1B && pBuf[off+2] == 0x11 && pBuf[off+3] == 0xEE)
                {
                    if(pIsCutted != NULL)
                    {
                        *pIsCutted = FALSE;
                        *pLen = 0x0A;
                    }

                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            } 
        }
    }

    return FALSE;
}


/*
************************************************************************************************************************************************************************     
*函数名称: Protocal_ProtocolMakeHmi268b
*功能描述: 乐升LT268B协议组包
*输入描述: 
	@@@ pFrame - 协议帧
	@@@ pbyData - 数据载荷
	@@@ wDataLen - 数据长度
*输出描述: 
	@@@ pbyBuf - 报文
	@@@ pwDataLen - 报文长度
*返回描述: TRUE - 成功 ， FALSE - 失败
*作者日期: XFW 
*全局声明: 无
*特殊说明: 
************************************************************************************************************************************************************************       
*/
extern uint32_t Protocal_ProtocolMakeHmi268b(const HMI_268B_FRAME *pFrame, uint8_t *pbySendBuf, uint16_t *pwSendLen)
{
    uint16_t off,chkSum;
    
    if(pFrame == NULL || pbySendBuf == NULL || pwSendLen == NULL)
    {
         return FALSE;
    }

    if(pFrame->m_wParamLen + 9 > *pwSendLen)
    {
         return FALSE;
    }

    off = 0;
    pbySendBuf[off++] = 0xAA;
    pbySendBuf[off++] = pFrame->m_byCmd;
    pbySendBuf[off++] = pFrame->m_byWidget;
    memcpy(&pbySendBuf[off], pFrame->m_aParam, pFrame->m_wParamLen); 
    off += pFrame->m_wParamLen;
    chkSum = Rx_CRC_CCITT(&pbySendBuf[1],pFrame->m_wParamLen+2);
    pbySendBuf[off++] = (chkSum >> 8) & 0xFF;
    pbySendBuf[off++] = chkSum & 0xFF;
    pbySendBuf[off++] = 0xE4;
    pbySendBuf[off++] = 0x1B;
    pbySendBuf[off++] = 0x11;
    pbySendBuf[off++] = 0xEE;

    *pwSendLen = off;    
    
    return TRUE;
}


