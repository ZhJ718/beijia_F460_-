//调试用
IDParam_GetTType();
IDParam_GetTSubType();
IDParam_GetRid();
IDParam_GetPid() >> 8;
IDParam_GetPid() & 0xff;
g_sUserParam.m_sLPosition;    //位置和高程
g_sUserParam.m_sUtcTime;

STRUCT_TS_Character1 character1;
STRUCT_TS_Character2 character2;

character1.m_bConnectionState = GetAllSlavesConnectStatus();   //各部件连接状态0：正常	1：异常 
character1.m_bFailureState = g_sUserParam.m_sTsChar1.m_bFailureState;         //0：无故障	1：有故障     
character1.m_bUnitmode = g_sUserParam.m_byMode == 2 ? 1 : 0;                    //0：训练模式	1：演习模式
character1.m_bBatteryState = g_sUserParam.m_sTsChar1.m_bBatteryState;         //0：电量正常	1：欠电
character1.m_bIdentity = UserParam_GetBRChar();     
character1.m_bLocked = TRUE;
character1.m_bIsConn = TRUE;

character2.m_bUnitmode = g_sUserParam.m_byMode == 2 ? 1 : 0;
character2.m_bIdentity = UserParam_GetBRChar();
character2.m_bKey = UserParam_GetKey();

//Uart_SendBuf(&g_sUartSetMag.Com[COM2], &pComTaskMag->m_pProtBuf[pComTaskMag->m_wProtBufLen], Len);
//Uart_SendBuf(&g_sUartSetMag.Com[COM2], pFrame->m_byBuf, pFrame->m_wBufLen);        
//Uart_SendBuf(&g_sUartSetMag.Com[COM2], pBuf, BufLen);
	
uint8_t dat55[5];
dat55[0] = 0x69;
dat55[1] = pMag->m_byTType;
dat55[2] = pMag->m_byTSubType;
dat55[3] = TType;
dat55[4] = TSubtype;
Uart_SendBuf(&g_sUartSetMag.Com[COM2], dat55, 5);			

//伤情读取
//A3 51 0C 30 01 11 00 39 00 80 08 01 FF FF FF 6F 
//装弹采集器演习参数下发
//A2 68 29 30 01 03 00 09 FF FF 03 00 FF 17 0B 04 0F 1E 00 06 00 00 12 00 00 03 01 00 27 10 00 27 10 00 00 01 BC 0A 18 08 06 0F 30 00 46 
//红外配对
//A2 A4 11 FE 06 00 00 00 02 14 01 02 A4 C1 38 AB 23 96 F4 41 1B 
//A2 A4 11 30 01 FF FF FF 01 FE 08 02 A4 C1 38 73 7A 97 BF 4C 0B 
//A2 A4 11 FE 06 FF FF FF FF FE 06 02 A4 C1 38 4D 76 3B 16 37 7E 
//A2 A4 11 31 01 FF FF FF 01 31 01 02 A4 C1 38 DF 23 FC EA D8 93
//红外读取参数
//A2 AF 15 FF FF FF FF FF 0A 0A 0A A6 0F A1 FF FF FF FF FF FF FF FF FF FF FD 
//头盔信息
//A3 C0 1E FE 08 11 00 39 00 00 00 00 00 00 00 00 00 00 80 00 00 00 00 00 00 00 00 01 00 C0 0D FE 05 69
//上下车信标
//5A 31 FD 01 20 FF FF F1 70 1C 03 01 03 00 0C 07 91 18 07 12 22 32 34 87 58 00 00 00 0B 06 13 35 0C 00 00 00 00 14 5E A4 C1 38 CA EB 23 FB 52 AE AA 
//爆炸物
//5A 3A FD 01 29 FF FF F1 91 28 41 01 11 06 7B 00 14 06 81 18 53 69 04 32 04 21 30 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 02 03 04 05 BF 00 00 00 30 01 FF 00 01 BF AA

//探头接收
//5A 1A 10 01 03 01 01 31 01 11 00 39 A0 00 00 00 00 00 00 00 00 00 14 14 08 AA 
//5A 1A 10 06 03 01 01 31 01 FF FF FF 00 00 00 00 00 00 00 00 00 00 14 14 0C AA
//5A 07 A0 00 0A 00 AA探头数量	