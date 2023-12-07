/********************************************************************************
* @File name: rescom_laser.c
* @Author: xfw
* @Version: 1.0
* @Date: 2023-3-8
* @Description:
********************************************************************************/

#include "rescom_task.h"
#include "protocal.h"

#include "zigcom_task.h"
#include "lasercom_task.h"
#include "ir38com_task.h"
#include "hmicom_task.h"

//调试口初始化
void ResCom_ComInit(void)
{
    UART_CONFIG Config = {0};
    UART_MANAGER *pUartMag = &g_sUartSetMag.Com[RES_COM];

    memcpy((void *)&Config, (void *)&g_scUartDefConfig[RES_COM], sizeof(UART_CONFIG));
    Config.m_dwBaudRate = RES_COM_BAUDRATE;
    //对应串口配置
    Uart_ComConfiguration(Config, pUartMag);
    Uart_ComInit(pUartMag);

    LteCom_GpioPowerOnInit();
}

//调试口写数据
uint32_t ResCom_ComWrite(uint8_t *pBuf, uint16_t Len)
{
    #ifdef RES_COM_LOG_OUTPUT

    LOG("RESCOM(%d)->", Len);
    printf_with_data(pBuf, Len);

    #endif

    return Uart_SendBuf(&g_sUartSetMag.Com[RES_COM], pBuf, Len);
}

//调试口读数据
uint32_t ResCom_ComRead(uint8_t *pBuf, uint16_t *pLen)
{
    UART_MANAGER *pUartMag = &g_sUartSetMag.Com[RES_COM];

    /**/
    if(TRUE == pUartMag->m_bIsComRecv)
    {
        if(pUartMag->m_wRecvLen == 0)
        {
            pUartMag->m_bIsComRecv = FALSE;
            return FALSE;
        }

        #ifdef RES_COM_LOG_OUTPUT

        LOG("RESCOM(%d)<-", pUartMag->m_wRecvLen);
        printf_with_data(pUartMag->m_byRecvBuf, pUartMag->m_wRecvLen);

        #endif

        if(*pLen >= pUartMag->m_wRecvLen)
        {
            memcpy((void *)pBuf, (void *)pUartMag->m_byRecvBuf, pUartMag->m_wRecvLen);
            *pLen = pUartMag->m_wRecvLen;
        }
        else
        {
            //缓存区不够,不拷贝多余数据
            memcpy((void *)pBuf, (void *)pUartMag->m_byRecvBuf, *pLen);
        }

        pUartMag->m_wRecvLen = 0;
        pUartMag->m_bIsComRecv = FALSE;

        return TRUE;
    }

    return FALSE;
}
//static RESCOMTASK_MAG g_sZigComTask_Mag;
RESCOMTASK_MAG g_sResComTask_Mag;

static RESCOM_LOWLEVEL_MAG ResComLLMag;


RESCOM_LOWLEVEL_MAG *ResCom_GetLLMagHandle(void)
{
    return &ResComLLMag;
}

/****************************************************************
@FUNCTION：获取保留端口任务管理句柄
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 

****************************************************************/
RESCOMTASK_MAG *ResComTask_HandleGet(void)
{
    return &g_sResComTask_Mag;
}

SCOMTASK_MAG *ResComTask_TaskHandleGet(void)
{
    return &g_sResComTask_Mag.m_sTaskMag;
}

/***************************************************************
@FUNCTION：4G参数初始化
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：
@SPECIAL:
***************************************************************/
/*
extern void FourGParamIni(FOURGMAG *pMag)
{
    uint8_t Sum = 0, i = 0;
    uint8_t CheckParam;
    char *pIP = NULL;
    char *pPORT = NULL;
    memset((char *)pMag, 0, sizeof(FOURGMAG)); //初始化4G全局变量

    pMag->m_4GParamFlg = TRUE;
    pMag->m_4GcloseLinkFlg = TRUE;

		//将参数里的0xFF清除
		memset((char *)pMag->m_socketID, 0, sizeof(pMag->m_socketID));
		memset((char *)pMag->m_farIp, 0, sizeof(pMag->m_farIp));
		memset((char *)pMag->m_farPort, 0, sizeof(pMag->m_farPort));
		memset((char *)pMag->m_imsi, 0, sizeof(pMag->m_imsi));
		memset((char *)pMag->m_myPort, 0, sizeof(pMag->m_myPort));

		pIP = strchr((const char*)g_sUserParam.m_strServerIP, ':');
		pPORT = strchr((const char*)g_sUserParam.m_strServerIP, NULL);
		memcpy((void*)pMag->m_farIp, g_sUserParam.m_strServerIP, (pIP - (char*)g_sUserPara.m_strServerIP));
		pIP++;
		memcpy((void *)pMag->m_farPort, pIP, (pPORT - pIP));

		#ifdef USE_FIX_IP   //测试用
			memcpy((void *)pMag->m_farIp, "192.168.6.46", strlen("192.168.6.46"));
			memcpy((void *)pMag->m_farPort, "4001", strlen("4001"));
		#endif
		
		memcpy((void *)pMag->m_myPort, "6501", strlen("6501"));
		memcpy((void *)pMag->m_imsi, "466,92,0100001111", strlen("466,92,0100001111"));
		memcpy((void *)pMag->m_socketID, "1", strlen("1"));

		pMag->m_4GTCP_or_UDP = '1';    //'1'为udp模式，‘0’为tcp模式

		pMag->m_4GParamFlg = TRUE;
}
*/
/****************************************************************
@FUNCTION：保留串口功能初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sResComMag

****************************************************************/
void ResComTask_Init(void)
{
    RESCOMTASK_MAG *pMag = ResComTask_HandleGet();

    memset((void *)pMag, 0, sizeof(RESCOMTASK_MAG));

    pMag->m_pLLMag = ResCom_GetLLMagHandle();

    pMag->m_pLLMag->m_sHwOps.hw_init = ResCom_ComInit;
    pMag->m_pLLMag->m_sHwOps.read = ResCom_ComRead;
    pMag->m_pLLMag->m_sHwOps.write = ResCom_ComWrite;
    pMag->m_pLLMag->m_sHwOps.gpio_power_on = LteCom_GpioPowerOn;
    pMag->m_pLLMag->m_sHwOps.gpio_power_off = LteCom_GpioPowerOff;

    pMag->m_pLLMag->m_sHwOps.hw_init();
    pMag->m_pLLMag->m_sHwOps.gpio_power_on();

    //任何分配内存失败，串口任务初始化失败
		pMag->m_bIsInit = SComTask_Init(&pMag->m_sTaskMag, 2048, 1000);
		
		SComTask_SetListNodeLen(&pMag->m_sTaskMag, 40);
		
		pMag->m_dwRptInterval = 5000;		//上传事件间隔5s
		memset(&pMag->m_sCmdTime, 0, sizeof(pMag->m_sCmdTime));

		pMag->m_dwCtrlEvtNum = 0;
		
		pMag->DrillParam.DisableSuicide = FALSE;
    pMag->DrillParam.DisableKillTeammate = FALSE;
    pMag->DrillParam.DisableCheat = TRUE;
    pMag->DrillParam.DisableInstallFume = TRUE;		//合成营默认装火工品
    pMag->DrillParam.EnableDiffRid = TRUE;
    pMag->DrillParam.DisableAssistLaser = TRUE;
    pMag->DrillParam.LocationShowType = FALSE;
    pMag->DrillParam.DisableDrillStartVoice = TRUE;
		
		pMag->m_IsAccess = FALSE;

		g_sVarious_Elements.m_byPJPLoactionFlag = FALSE;
		
		pMag->Vios.FumeUnInstall = FALSE;
		pMag->Vios.HelmetLoss = FALSE;
		pMag->Vios.DetectorLoss = FALSE;
		pMag->Vios.IllegalMove = FALSE;
}


/****************************************************************
@FUNCTION：保留串口功能反初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sResComMag

****************************************************************/
void ResComTask_DeInit(void)
{
    RESCOMTASK_MAG *pMag = ResComTask_HandleGet();

    SComTask_DeInit(&pMag->m_sTaskMag);

    memset((void *)pMag, 0, sizeof(RESCOMTASK_MAG));
}

/****************************************************************
@FUNCTION：BackStage接口底层定时调度进程
@INPUT：
@OUTPUT：无
@RETURN: TRUE-正在执行底层定时调度,  FALSE-无底层定时调度, 可执行上层应用定时调度
@AUTHOR：zeg
@SPECIAL: 实现模块上电
****************************************************************/
extern uint32_t ResCom_LowLevelTimedProcess(void)
{
    RESCOMTASK_MAG *pMag = ResComTask_HandleGet();
		pMag->m_pLLMag = g_sResComTask_Mag.m_pLLMag; 
    assert_param(pMag != NULL);

    return FALSE;
}

/****************************************************************
@FUNCTION：新后台发送入网请求指令
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：LW
@SPECIAL:
****************************************************************/
void AccessRequest_Send(RESCOMTASK_MAG *pMag)
{
  StructBNTerminal_AccessRequest RequestInfo;
	FRAME_BUF *pFrame = NULL;
	uint8_t  num = 0,cut = 0;
	
	pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

	if(pFrame == NULL)
	{
			return;
	}
	
  //参数赋值
	RequestInfo.m_byXor = 0;
	RequestInfo.m_byHead = 0xA8;
	RequestInfo.m_byCmd = BNCmd_Terminal_AccessRequest;
  RequestInfo.m_byT_Type = IDParam_GetTType();
  RequestInfo.m_byT_Subtype = IDParam_GetTSubType();
  RequestInfo.m_byRID = IDParam_GetRid();
  RequestInfo.m_wPID = IDParam_GetPid();   
    
	//组包
	pFrame->m_wBufLen = 0;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = RequestInfo.m_byHead;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = RequestInfo.m_byCmd;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = RequestInfo.m_byT_Type;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = RequestInfo.m_byT_Subtype;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = RequestInfo.m_byRID;

	pFrame->m_byBuf[pFrame->m_wBufLen++] = RequestInfo.m_wPID >> 8;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = RequestInfo.m_wPID;

  pFrame->m_byBuf[2] = pFrame->m_wBufLen - 3;   

	for(cut = 3; cut < pFrame->m_wBufLen; cut++)
	{
			RequestInfo.m_byXor ^= pFrame->m_byBuf[cut];
	}
	pFrame->m_byBuf[pFrame->m_wBufLen++] = RequestInfo.m_byXor;

	pFrame->m_dwCnt = 1;
	pFrame->m_dwID = 0xA891;
	pFrame->m_dwScheTime = 100;
	pFrame->m_bySendNum = 0;
	SComTask_SendListAppend(pMag->m_sTaskMag.m_pSendList, (void*)pFrame);
}

//根据血量获取伤势c_degree;
uint8_t GetInjuryDegree(uint8_t Blood)
{
    if(Blood == 0)
        return CI_SOLDIER_D0;
    else if(1 <= Blood && Blood < 8)
        return CI_SOLDIER_C1;
    else if(8 <= Blood && Blood < 16)
        return CI_SOLDIER_C2;
    else if(16 <= Blood && Blood < 24)
        return CI_SOLDIER_C3;
    else if(24 <= Blood && Blood < 32)
        return CI_SOLDIER_C4;
    else if(32 <= Blood && Blood <= 40)
        return CI_SOLDIER_C5;
    else if(41 <= Blood && Blood < 48)
        return CI_SOLDIER_S6;
    else if(48 <= Blood && Blood < 56)
        return CI_SOLDIER_S7;
    else if(56 <= Blood && Blood < 64)
        return CI_SOLDIER_S8;
    else if(64 <= Blood && Blood <= 73)
        return CI_SOLDIER_S9;
    else if(74 <= Blood && Blood <= 82)
        return CI_SOLDIER_ME10;
    else if(83 <= Blood && Blood <= 90)
        return CI_SOLDIER_ME11;
    else if(91 <= Blood && Blood <= 99)
        return CI_SOLDIER_L12;
    else if(Blood == 100)
        return CI_SOLDIER_N13;
}

//根据伤势获取扣血量
uint8_t GetInjuryBloodByHurtDegree(ENUM_CI_INDIVIDUAL InjuryDegree)
{
    if(InjuryDegree == CI_SOLDIER_D0)
        return 100;
    else if(InjuryDegree == CI_SOLDIER_C1)
        return 99;
    else if(InjuryDegree == CI_SOLDIER_C2)
        return 92;
    else if(InjuryDegree == CI_SOLDIER_C3)
        return 84;
    else if(InjuryDegree == CI_SOLDIER_C4)
        return 76;
    else if(InjuryDegree == CI_SOLDIER_C5)
        return 68;
    else if(InjuryDegree == CI_SOLDIER_S6)
        return 59;
    else if(InjuryDegree == CI_SOLDIER_S7)
        return 52;
    else if(InjuryDegree == CI_SOLDIER_S8)
        return 44;
    else if(InjuryDegree == CI_SOLDIER_S9)
        return 36;
    else if(InjuryDegree == CI_SOLDIER_ME10)
        return 26;
    else if(InjuryDegree == CI_SOLDIER_ME11)
        return 17;
    else if(InjuryDegree == CI_SOLDIER_L12)
        return 9;
    else if(InjuryDegree == CI_SOLDIER_N13)
        return 0;
}


//单兵
void SoliderStatusUpload_Send(RESCOMTASK_MAG *pMag)
{
	SINGLE_SLAVER_MAG *pSlaverMag;
  Struct_UniversalStatusUpload StatusInfo;
	FRAME_BUF *pFrame = NULL;
	
  uint32_t Rnum = 0, Tnum = 0, Snum = 0;  
	uint8_t i, w, b, num = 0, cut = 0, PJPBondFlag = 0, WeaponNum = 0;

	pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

	if(pFrame == NULL)
	{
			return;
	}

	for(i = 0; i < UserParam_GetSlaverCnt(); i++)
	{
			pSlaverMag = UserParam_GetSlaverMag(i);

			if(pSlaverMag->m_byTType == 0x14)
			{
				PJPBondFlag = 0x5A;
				break;
			}
	}
	
	if(PJPBondFlag != 0x5A)
	{
			//参数赋值
			StatusInfo.m_byXor = 0;
			StatusInfo.m_aData_Frame[0] = 0xA8;
			StatusInfo.m_aData_Frame[1] = BNCmd_Terminal_StatusUpload;
			StatusInfo.m_byD_Type    = 0x03;
			StatusInfo.m_byT_Type    = IDParam_GetTType();
			StatusInfo.m_byT_Subtype = IDParam_GetTSubType();
			StatusInfo.m_byRID   = IDParam_GetRid();
			StatusInfo.m_aPID[0] = IDParam_GetPid() >>8 ;
			StatusInfo.m_aPID[1] = IDParam_GetPid() &0xff;
			StatusInfo.m_byKey   = UserParam_GetKey();
			StatusInfo.m_sL_Position = g_sUserParam.m_sLPosition;    //位置和高程
			StatusInfo.m_sTime = g_sUserParam.m_sUtcTime;
			
			StatusInfo.m_byC_Idegree = GetInjuryDegree(g_sUserParam.m_byBlood);		//综合伤势
			
			StatusInfo.m_sTS_Character1.m_bLocked = g_sUserParam.m_sTsChar1.m_bLocked;
			StatusInfo.m_sTS_Character1.m_bConnectionState = GetAllSlavesConnectStatus();		//各部件连接状态0：正常	1：异常 
			StatusInfo.m_sTS_Character1.m_bFailureState = g_sUserParam.m_sTsChar1.m_bFailureState;		//0：无故障	1：有故障     
			StatusInfo.m_sTS_Character1.m_bUnitmode     = g_sUserParam.m_sTsChar1.m_bUnitmode;		//0：训练模式	1：演习模式
			StatusInfo.m_sTS_Character1.m_bBatteryState = g_sUserParam.m_sTsChar1.m_bBatteryState;		//0：电量正常	1：欠电
			StatusInfo.m_sTS_Character1.m_bIdentity     = g_sUserParam.m_sTsChar1.m_bIdentity;		//身份 ENUM_BRCHAR
			StatusInfo.m_sTS_Character1.m_bIsNight      = g_sUserParam.m_sTsChar1.m_bIsNight;
			
			StatusInfo.m_byBP_Character = g_sMpu6050Mag.Pose;    //姿态【站1、卧2、左蹲3、右蹲4、坐5、跑6、走7、躺8、（跳9）】

			//组包
			pFrame->m_wBufLen = 0;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_aData_Frame[0];
			pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_aData_Frame[1];
			pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;		//长度组包最后给
			pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_byD_Type;		//状态
			pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_byT_Type;		//实体
			pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_byT_Subtype;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_byRID;		//单位代字
			pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_aPID[0];		//设备编号
			pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_aPID[1];		
			pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_byKey;		//密钥
			
			//地理位置
			memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen], &StatusInfo.m_sL_Position, sizeof(STRUCT_LPOS));
			pFrame->m_wBufLen += sizeof(STRUCT_LPOS);
			//时间
			memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen],&StatusInfo.m_sTime,sizeof(STRUCT_TIME1));
			pFrame->m_wBufLen += sizeof(STRUCT_TIME1);
			//综合伤势
			pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_byC_Idegree;
			//实体状态字
			memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen],&StatusInfo.m_sTS_Character1,sizeof(STRUCT_TS_Character1));
			pFrame->m_wBufLen += sizeof(STRUCT_TS_Character1);
			pFrame->m_byBuf[pFrame->m_wBufLen++] = g_sMpu6050Mag.Pose;
			
			//if(g_sUserParam.m_byMode == TRAIN)
			WeaponNum = UserParam_GetWeaponNum();
			//else
			//	WeaponNum = g_sUserParam.m_byBackstageCfgWeaponNum;
			
			pFrame->m_byBuf[pFrame->m_wBufLen++] = WeaponNum;
			//pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;		//物资
			
			for(w = 0; w < WeaponNum; w++)
			{
					pFrame->m_byBuf[pFrame->m_wBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_byWeapon_Type;
					pFrame->m_byBuf[pFrame->m_wBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_byWeapon_SubType;
					pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetRid();
					pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetPid() >> 8;
					pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetPid();
					pFrame->m_byBuf[pFrame->m_wBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_byState;
					pFrame->m_byBuf[pFrame->m_wBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind;
					if(g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind != 0)//判断是否有配置弹药
					{
						for(b = 0; b < g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind; b++)
						{
							
							Rnum = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[0] << 16 | 
										 g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[1] << 8 |
										 g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[2];
							Tnum = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum[0] << 16 | 
										 g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum[1] << 8 |
										 g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum[2];
							Snum = Tnum - Rnum;
							
							pFrame->m_byBuf[pFrame->m_wBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_Type;
							pFrame->m_byBuf[pFrame->m_wBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_SubType;
							pFrame->m_byBuf[pFrame->m_wBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[0];
							pFrame->m_byBuf[pFrame->m_wBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[1];
							pFrame->m_byBuf[pFrame->m_wBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[2];
							pFrame->m_byBuf[pFrame->m_wBufLen++] = Snum >> 16;
							pFrame->m_byBuf[pFrame->m_wBufLen++] = Snum >> 8;
							pFrame->m_byBuf[pFrame->m_wBufLen++] = Snum;
						}
					}
			}
			/*	
			//暂时不配物资
			StatusInfo.m_bySupplyNum = 0;		//物资信息
						
			if(StatusInfo.m_bySupplyNum != 0)		//判断是否有配置物资
			{
				for(w=0;w<StatusInfo.m_byAmmuNum;w++)
				{
					pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_sSupply[w].m_byM_Type;
					pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_sSupply[w].m_byM_Subtype;
					pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_sSupply[w].m_aRemainSupplyNum[0];
					pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_sSupply[w].m_aRemainSupplyNum[1];
					pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_sSupply[w].m_aRemainSupplyNum[2];
				}				
			}
			*/ 
			pFrame->m_byBuf[2] = pFrame->m_wBufLen - 3; //报文内容长度，包括两个帧头、长度、异或和。
			
			//校验
			for(cut = 3; cut < pFrame->m_wBufLen; cut++)
			{
					StatusInfo.m_byXor ^= pFrame->m_byBuf[cut];
			}
			pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_byXor;

			pFrame->m_dwCnt = 1;
			pFrame->m_dwID = 0xA892;
			pFrame->m_dwScheTime = 100;
			pFrame->m_bySendNum = 0;
			SComTask_SendListAppend(pMag->m_sTaskMag.m_pSendList, (void*)pFrame);
	}
	else
	{
			//参数赋值
			StatusInfo.m_byXor = 0;
			StatusInfo.m_aData_Frame[0] = 0xA8;
			StatusInfo.m_aData_Frame[1] = BNCmd_Terminal_StatusUpload_Bind;
			StatusInfo.m_byD_Type    = 0x01;
			StatusInfo.m_byT_Type    = IDParam_GetTType();
			StatusInfo.m_byT_Subtype = IDParam_GetTSubType();
			StatusInfo.m_byRID   = IDParam_GetRid();
			StatusInfo.m_aPID[0] = IDParam_GetPid() >>8 ;
			StatusInfo.m_aPID[1] = IDParam_GetPid() &0xff;
			StatusInfo.m_byKey   = UserParam_GetKey();
			StatusInfo.m_sL_Position = g_sUserParam.m_sLPosition;    //位置和高程
			StatusInfo.m_sTime = g_sUserParam.m_sUtcTime;
			
			StatusInfo.m_byC_Idegree = GetInjuryDegree(g_sUserParam.m_byBlood);		//综合伤势
			
			StatusInfo.m_sTS_Character1.m_bLocked = g_sUserParam.m_sTsChar1.m_bLocked;
			StatusInfo.m_sTS_Character1.m_bConnectionState = GetAllSlavesConnectStatus();		//各部件连接状态0：正常	1：异常 
			StatusInfo.m_sTS_Character1.m_bFailureState = g_sUserParam.m_sTsChar1.m_bFailureState;		//0：无故障	1：有故障     
			StatusInfo.m_sTS_Character1.m_bUnitmode     = g_sUserParam.m_sTsChar1.m_bUnitmode;		//0：训练模式	1：演习模式
			StatusInfo.m_sTS_Character1.m_bBatteryState = g_sUserParam.m_sTsChar1.m_bBatteryState;		//0：电量正常	1：欠电
			StatusInfo.m_sTS_Character1.m_bIdentity     = g_sUserParam.m_sTsChar1.m_bIdentity;		//身份 ENUM_BRCHAR
			StatusInfo.m_sTS_Character1.m_bIsNight      = g_sUserParam.m_sTsChar1.m_bIsNight;
			
			StatusInfo.m_byBP_Character = g_sMpu6050Mag.Pose;    //姿态【站1、卧2、左蹲3、右蹲4、坐5、跑6、走7、躺8、（跳9）】

			//组包
			pFrame->m_wBufLen = 0;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_aData_Frame[0];
			pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_aData_Frame[1];
			pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;		//长度组包最后给
			pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_byT_Type;		//实体
			pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_byT_Subtype;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_byRID;		//单位代字
			pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_aPID[0];		//设备编号
			pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_aPID[1];		
			pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_byD_Type;		//状态
			pFrame->m_byBuf[pFrame->m_wBufLen++] = pSlaverMag->m_byTType;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = pSlaverMag->m_byTSubType;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = pSlaverMag->m_byRid;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = pSlaverMag->m_wPid >> 8;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = pSlaverMag->m_wPid & 0xff;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_byKey;		//密钥
			
			//地理位置
			memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen], &StatusInfo.m_sL_Position, sizeof(STRUCT_LPOS));
			pFrame->m_wBufLen += sizeof(STRUCT_LPOS);
			//时间
			memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen],&StatusInfo.m_sTime,sizeof(STRUCT_TIME1));
			pFrame->m_wBufLen += sizeof(STRUCT_TIME1);
			//综合伤势
			pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_byC_Idegree;
			//实体状态字
			memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen],&StatusInfo.m_sTS_Character1,sizeof(STRUCT_TS_Character1));
			pFrame->m_wBufLen += sizeof(STRUCT_TS_Character1);
			
			w = pSlaverMag->m_byWeaponIdx;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = 1;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
			
			pFrame->m_byBuf[pFrame->m_wBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_byBullet_Type;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_byBullet_SubType;
			Rnum = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_aBulletRemainNum[0] << 16 | 
						 g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_aBulletRemainNum[1] << 8 |
						 g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_aBulletRemainNum[2];
			Tnum = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_aBulletTotalNum[0] << 16 | 
						 g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_aBulletTotalNum[1] << 8 |
						 g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_aBulletTotalNum[2];
			Snum = Tnum - Rnum;
			
			pFrame->m_byBuf[pFrame->m_wBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_aBulletRemainNum[0];
			pFrame->m_byBuf[pFrame->m_wBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_aBulletRemainNum[1];
			pFrame->m_byBuf[pFrame->m_wBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_aBulletRemainNum[2];
			pFrame->m_byBuf[pFrame->m_wBufLen++] = Snum >> 16;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = Snum >> 8;
			pFrame->m_byBuf[pFrame->m_wBufLen++] = Snum;

			pFrame->m_byBuf[2] = pFrame->m_wBufLen - 3; //报文内容长度，包括两个帧头、长度、异或和。
			
			//校验
			for(cut = 3; cut < pFrame->m_wBufLen; cut++)
			{
					StatusInfo.m_byXor ^= pFrame->m_byBuf[cut];
			}
			pFrame->m_byBuf[pFrame->m_wBufLen++] = StatusInfo.m_byXor;

			pFrame->m_dwCnt = 1;
			pFrame->m_dwID = 0xA894;
			pFrame->m_dwScheTime = 100;
			pFrame->m_bySendNum = 0;
			SComTask_SendListAppend(pMag->m_sTaskMag.m_pSendList, (void*)pFrame);
	}
}

/****************************************************************
@FUNCTION：保留串口功能任务主进程
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sResComMag

****************************************************************/
void ResComTask_Process(void)
{
    RESCOMTASK_MAG *pMag = ResComTask_HandleGet();
    SCOMTASK_MAG *pComTaskMag = &pMag->m_sTaskMag;
    uint16_t i, Len = pComTaskMag->m_wProtBufMaxLen - pComTaskMag->m_wProtBufLen;
    uint32_t DataLen, IsCutted = FALSE, IsLong55AA = FALSE;
    uint32_t NowTime = GetSysRunMilliSecond();
		static uint8_t Flg2 = FALSE;
    FRAME_BUF *pFrame = NULL;

    if(FALSE == pMag->m_bIsInit)
    {
        return;
    }

    //定时任务
    if((NowTime >  pComTaskMag->m_dwLastScheduleTime) &&
            (NowTime -  pComTaskMag->m_dwLastScheduleTime >= pComTaskMag->m_dwScheduleTime) )
    {
        pComTaskMag->m_dwLastScheduleTime = NowTime;

        //优先保证底层操作
        if(FALSE == ResCom_LowLevelTimedProcess())
        {
            //检查发送队列
						pFrame = (FRAME_BUF *)SComTask_TimedProcess(pComTaskMag);

						if(pFrame != NULL)
						{
							pComTaskMag->m_dwLastCommTime = 0;
							pMag->m_pLLMag->m_sHwOps.write(pFrame->m_byBuf, pFrame->m_wBufLen);
						}
        }

        /***********应用逻辑***************************/
				if((NowTime - pMag->m_dwTimer_Connect_Request) >= 5000)
				{					
					pMag->m_dwTimer_Connect_Request = NowTime;
					if(pMag->m_IsAccess == FALSE)		
						AccessRequest_Send(pMag);				
				}
				if((NowTime - pMag->m_dwTimer_Upload_SelfState) >= pMag->m_dwRptInterval)
				{
					pMag->m_dwTimer_Upload_SelfState = NowTime;
					if(pMag->m_IsAccess == TRUE)		
						SoliderStatusUpload_Send(pMag);
				}
				
				if(g_sUserParam.m_byAidEndFlag1 == TRUE && g_sUserParam.m_byAidEndFlag2 == TRUE)
				{
						g_sUserParam.m_byAidEndFlag1 = FALSE;
						g_sUserParam.m_byAidEndFlag2 = FALSE;
					
						BSN_RescureEvtHandle(&g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued);  //事件上传
				}
				
				if((NowTime - pMag->m_dwHardStatusCheckTime) >= 60000)
				{
					pMag->m_dwHardStatusCheckTime = NowTime;
					HardStatusDetection();
					MedicalAidCheck();
					NCBT_DamageCheck();
				}
				
				if((NowTime - pMag->m_dwTimer_Connect_GPS) >= 60000 && g_sUserParam.m_sLPosition.m_byLposState == TRUE)
				{
					 g_sUserParam.m_sLPosition.m_byLposState = FALSE;
					 Wtn6_Play(WTN6_GPSTrouble);
					 pMag->m_dwTimer_Connect_GPS = NowTime;
				}
				
				if(((NowTime - pMag->m_dwTimer_Connect_Backstage) >= (5 * 60000)))
				{
					if(g_sResComTask_Mag.m_IsAccess == FALSE)
					{
						if(Flg2 == FALSE)
						{
								Wtn6_Play(WTN6_HITCH);
								Wtn6_Play(WTN6_Number_2);
								Wtn6_Play(WTN6_TERMINAL_OFFLINE);
								Flg2 = TRUE;
						}
					}
					pMag->m_dwTimer_Connect_Backstage = NowTime;
				}
				/*
				if(((NowTime - pMag->m_dwTimer_Connect_Backstage) >= 5 * 60000))
				{
					if(g_sResComTask_Mag.m_IsAccess == TRUE)
					{
						//pMag->m_pLLMag->m_sHwOps.gpio_power_off();
						if((NowTime - pMag->m_dwTimer_Connect_Backstage)  >= (5 * 60000 + 500))
						{
							//pMag->m_pLLMag->m_sHwOps.gpio_power_on();
							pMag->m_IsAccess = FALSE;
							pMag->m_dwTimer_Connect_Backstage = NowTime;
						}
					}
				}
				*/
    }

    //串口接收处理
    if(TRUE == pMag->m_pLLMag->m_sHwOps.read(&pComTaskMag->m_pProtBuf[pComTaskMag->m_wProtBufLen], &Len))
    {
        //转发数据到上行口
        //ResComTask_RecvSendToUpcom(&pComTaskMag->m_pProtBuf[pComTaskMag->m_wProtBufLen],Len,0x06);
				pComTaskMag->m_dwLastCommTime = NowTime;
        pComTaskMag->m_wProtBufLen += Len;

        for(i = 0; i < pComTaskMag->m_wProtBufLen;)
        {
            Len = pComTaskMag->m_wProtBufLen - i;

						if(TRUE == Backstage_ProtocalParse(&pComTaskMag->m_pProtBuf[i], &Len, &IsCutted))
						{
								if(TRUE == IsCutted)
                {
                    memcpy(pComTaskMag->m_pProtBuf, &pComTaskMag->m_pProtBuf[i], pComTaskMag->m_wProtBufLen - i);
										pComTaskMag->m_wProtBufLen -= i ;
										break;		//如果发现协议不完整，则不管，继续向后解析
                }
                else
                {
										pMag->m_dwTimer_Connect_Backstage = NowTime;        
										BackStageNewAnalysis(&pComTaskMag->m_pProtBuf[i], Len);
                    i = Len + i;
                }
						}
						else if(TRUE == GPS_FindNMEA0183String(&pComTaskMag->m_pProtBuf[i],&Len,&IsCutted))
						{
								if(TRUE == IsCutted)
								{
									memcpy(pComTaskMag->m_pProtBuf, &pComTaskMag->m_pProtBuf[i], pComTaskMag->m_wProtBufLen - i);
									pComTaskMag->m_wProtBufLen -= i ;
									break;
								}
								else
								{
										pMag->m_dwTimer_Connect_GPS = NowTime;
									
										NMEA_GNRMC_Analysis(&pMag->m_sNmeaMag, &pComTaskMag->m_pProtBuf[i], Len);

										i = Len + i;
								}
						}
						else if(TRUE == BackStage_Frame_Parse(&pComTaskMag->m_pProtBuf[i], &Len, &IsCutted))
						{
								if(TRUE == IsCutted)
								{
									memcpy(pComTaskMag->m_pProtBuf, &pComTaskMag->m_pProtBuf[i], pComTaskMag->m_wProtBufLen - i);
									pComTaskMag->m_wProtBufLen -= i ;
									break;
								}
								else
								{
									BackStageFactoryDebug(&pComTaskMag->m_pProtBuf[i], Len);
									i = Len + i;
								}
						}
            else if(TRUE == Update_ProtocalParse(&pComTaskMag->m_pProtBuf[i], &Len, &IsCutted, &IsLong55AA))
            {
                if(TRUE == IsCutted)
                {
                    memcpy(pComTaskMag->m_pProtBuf, &pComTaskMag->m_pProtBuf[i], pComTaskMag->m_wProtBufLen - i);
										pComTaskMag->m_wProtBufLen -= i ;
										break;
								}
                else
                {

                    Update_ConfigSendFunc(pMag->m_pLLMag->m_sHwOps.write);

                    //发现为55AA长包
                    if (IsLong55AA)
                    {
                        DataLen	= pComTaskMag->m_pProtBuf[i + 3] | (pComTaskMag->m_pProtBuf[i + 4] << 8);

                        //过滤掉上行指令
                        if(pComTaskMag->m_pProtBuf[i + 7] == 0x0)
                        {
                            Prot_HardCtrl_Analysis(&pComTaskMag->m_pProtBuf[i + 7], DataLen, (void *)pMag);
                        }
                        else
                        {
                            Update_ProtocalProcess(&pComTaskMag->m_pProtBuf[i + 7], DataLen);
                        }

                    }
                    else
                    {
                        //过滤掉上行指令
                        if(pComTaskMag->m_pProtBuf[i + 3] == 0x0)
                        {
                            Prot_HardCtrl_Analysis(&pComTaskMag->m_pProtBuf[i + 3], pComTaskMag->m_pProtBuf[i + 2], (void *)pMag);
                        }
                        else
                        {
                            //LOG("parse succ,i=%d,Len=%d\n\r",i,Len);
                            Update_ProtocalProcess(&pComTaskMag->m_pProtBuf[i + 3], pComTaskMag->m_pProtBuf[i + 2]);

                        }
                    }

                    i = Len + i;
                }
            }
            else
            {
                i++;
            }

            if(i == pComTaskMag->m_wProtBufLen)
            {
                pComTaskMag->m_wProtBufLen = 0;
            }
        }


    }

}


void ResComTask_SendToUpcom(uint8_t *pBuf, uint16_t BufLen, uint8_t DevIdx, DEVCTRL eDevCtl)
{
    //uint8_t temp[256];
    //uint16_t len;
    FRAME_BUF *pFrame = NULL;

    //assert_param(pMag != NULL);

    pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

    if(pFrame == NULL)
    {
        return;
    }

    if(BufLen < 250)
    {
        pFrame->m_wBufLen = 0;
        pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x55;
        pFrame->m_byBuf[pFrame->m_wBufLen++] = GetProtAddr();
        pFrame->m_byBuf[pFrame->m_wBufLen++] = 3 + BufLen;
        pFrame->m_byBuf[pFrame->m_wBufLen++] = PROTOCAL_CMD_BOARD_CTRL;
        pFrame->m_byBuf[pFrame->m_wBufLen++] = DevIdx;//Zigbee
        pFrame->m_byBuf[pFrame->m_wBufLen++] = (uint8_t)eDevCtl;//R
        memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen], pBuf, BufLen);
        pFrame->m_wBufLen += BufLen;
        pFrame->m_byBuf[pFrame->m_wBufLen++] = Protocal_MakeU8Sum(&pFrame->m_byBuf[3], 3 + BufLen);
        pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xAA;
				
				pFrame->m_dwCnt = 1;
				pFrame->m_dwID = 0x5500;
				pFrame->m_dwScheTime = 100;
				pFrame->m_bySendNum = 0;
				SComTask_SendListAppend(g_sResComTask_Mag.m_sTaskMag.m_pSendList, (void*)pFrame);
        //LinkQueue_Append(g_sResComTask_Mag.m_sTaskMag.m_pSendQueue, (void*)pFrame);
				//g_sZigComTask_Mag.m_pLLMag->m_sHwOps.write(pFrame->m_byBuf,pFrame->m_wBufLen);
    }
}
