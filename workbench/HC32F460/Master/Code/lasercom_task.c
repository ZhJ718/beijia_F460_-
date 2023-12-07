/********************************************************************************
* @File name: lasercom_task.c
* @Author: xfw
* @Version: 1.0
* @Date: 2023-4-4
* @Description:
********************************************************************************/
#include "hmicom_task.h"
#include "lasercom_task.h"
#include "rescom_task.h"

/*************底层实现************************************************/
//串口初始化
void LaserCom_ComInit(void)
{
    UART_CONFIG Config = {0};
    UART_MANAGER *pUartMag = &g_sUartSetMag.Com[LASER_COM];

    memcpy((void *)&Config, (void *)&g_scUartDefConfig[LASER_COM], sizeof(UART_CONFIG));
    Config.m_dwBaudRate = LASER_COM_BAUDRATE;
    //Config.TxMode = MODE_INTERRUPT;
    //Config.RxMode = MODE_INTERRUPT;
    //对应串口配置
    Uart_ComConfiguration(Config, pUartMag);
    Uart_ComInit(pUartMag);

    LaserCom_GpioPowerOnInit();
}

//串口写数据
uint32_t LaserCom_ComWrite(uint8_t *pBuf, uint16_t Len)
{
    #ifdef LASER_COM_LOG_OUTPUT

    LOG("LASERCOM(%d)->", Len);
    printf_with_data(pBuf, Len);

    #endif

    return Uart_SendBuf(&g_sUartSetMag.Com[LASER_COM], pBuf, Len);
}

//串口读数据
uint32_t LaserCom_ComRead(uint8_t *pBuf, uint16_t *pLen)
{
    UART_MANAGER *pUartMag = &g_sUartSetMag.Com[LASER_COM];

    /**/
    if(TRUE == pUartMag->m_bIsComRecv)
    {
        if(pUartMag->m_wRecvLen == 0)
        {
            pUartMag->m_bIsComRecv = FALSE;
            return FALSE;
        }

        #ifdef LASER_COM_LOG_OUTPUT

        LOG("LASERCOM(%d)<-", pUartMag->m_wRecvLen);
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

/*************底层实现结束************************************************/


/****************************************************************
@FUNCTION：获取探头带在线探头数
@INPUT：
    @param  MaxCnt - 可能的最大探头数
    @param  IntVal - 单个探头基础超时，单位10ms。若为0,则取默认值10
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 若回复多个报文，可以增大MaxCnt或者IntVal值
****************************************************************/
void LaserCom_Prot_GetOnlineNum(LASERCOMTASK_MAG *pMag, uint8_t MaxCnt, uint8_t IntVal)
{
    FRAME_BUF *pFrame = NULL;

    assert_param(pMag != NULL);

    pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

    if(pFrame == NULL)
    {
        return;
    }

    pFrame->m_wBufLen = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x5A;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xA0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = MaxCnt;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = (IntVal == 0 ? 0x0A : IntVal);
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xAA;
    //填充协议长度
    pFrame->m_byBuf[1] = pFrame->m_wBufLen;

    LinkQueue_Append(pMag->m_sTaskMag.m_pSendQueue, (void*)pFrame);

}

/****************************************************************
@FUNCTION：LASER发送重新自动编号命令
@INPUT：LASER底层管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:无
****************************************************************/
void Laser_CtrlAutoNumbered(LASERCOMTASK_MAG *pMag)
{
    FRAME_BUF *pFrame = NULL;

    assert_param(pMag != NULL);

    pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

    if(pFrame == NULL)
    {
        return;
    }

    pFrame->m_wBufLen = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x5A;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xA0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x0A;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xAA;
    //填充协议长度
    pFrame->m_byBuf[1] = pFrame->m_wBufLen;

    LinkQueue_Append(pMag->m_sTaskMag.m_pSendQueue, (void*)pFrame);

}

/****************************************************************
@FUNCTION：LASER发送模式切换命令
@INPUT：LASER底层管理结构体指针，模式
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:无
****************************************************************/
void LaserCom_Prot_CtrlMode(LASERCOMTASK_MAG *pMag, LASER_MODE a_Mod)
{
    FRAME_BUF *pFrame = NULL;

    assert_param(pMag != NULL);

    pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

    if(pFrame == NULL)
    {
        return;
    }

    pFrame->m_wBufLen = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x5A;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xB1;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = (uint8_t)a_Mod;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xAA;
    //填充协议长度
    pFrame->m_byBuf[1] = pFrame->m_wBufLen;

    LinkQueue_Append(pMag->m_sTaskMag.m_pSendQueue, (void*)pFrame);

}

//RGB进入白光，闪烁
void RGB(LASERCOMTASK_MAG *pMag)
{
    FRAME_BUF *pFrame = NULL;

    assert_param(pMag != NULL);

    pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

    if(pFrame == NULL)
    {
        return;
    }

    pFrame->m_wBufLen = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x5A;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x08;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x32;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x06;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x32;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x02;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x00;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xAA;

    LinkQueue_Append(pMag->m_sTaskMag.m_pSendQueue, (void*)pFrame);
}

/****************************************************************
@FUNCTION：LASER发送灯控命令
@INPUT：LASER底层管理结构体指针，灯编号，灯状态
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:灯编号从1开始
****************************************************************/
void Detector_Led_Proc(LASERCOMTASK_MAG *pMag,uint8_t a_Id,uint32_t a_IsOn) /* 五探头 */
{
	FRAME_BUF *pFrame = NULL;

	assert_param(pMag != NULL);

	pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

	if(pFrame == NULL)
	{
			return;
	}

	pFrame->m_wBufLen = 0;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x5A;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 6;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x30;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = (a_IsOn?1:0);
	pFrame->m_byBuf[pFrame->m_wBufLen++] = a_Id + 1;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xAA;
	//填充协议长度
	pFrame->m_byBuf[1] = pFrame->m_wBufLen;

	LinkQueue_Append(pMag->m_sTaskMag.m_pSendQueue, (void*)pFrame);
}

void Detector_Led_AllOff() 
{
	LASERCOMTASK_MAG *pMag = LaserComTask_HandleGet();
	FRAME_BUF *pFrame = NULL;

	assert_param(pMag != NULL);

	pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

	if(pFrame == NULL)
	{
			return;
	}

	pFrame->m_wBufLen = 0;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x5A;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 6;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x30;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xFF;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xAA;
	//填充协议长度
	pFrame->m_byBuf[1] = pFrame->m_wBufLen;

	LinkQueue_Append(pMag->m_sTaskMag.m_pSendQueue, (void*)pFrame);
}

void Detector_Led_AllOn() 
{
	LASERCOMTASK_MAG *pMag = LaserComTask_HandleGet();
	FRAME_BUF *pFrame = NULL;

	assert_param(pMag != NULL);

	pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

	if(pFrame == NULL)
	{
			return;
	}

	pFrame->m_wBufLen = 0;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x5A;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 6;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x30;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 1;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xFF;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xAA;
	//填充协议长度
	pFrame->m_byBuf[1] = pFrame->m_wBufLen;

	LinkQueue_Append(pMag->m_sTaskMag.m_pSendQueue, (void*)pFrame);
}

void Laser_CalcPartition(uint8_t *pID, uint8_t *pPartition, LASERCOMTASK_MAG *pMag)
{
	uint8_t i,byPartIndex;
	uint8_t NumMax;
	
	assert_param(pMag!=NULL);

	for(i=0,NumMax=0;i<LASER_PART_MAX;i++)
	{
		if(pMag->m_sPartIDNum[i]>=NumMax)
		{
			byPartIndex = i;
			NumMax = pMag->m_sPartIDNum[i];
		}
	}
	
	SetRand();
	
	if(pID!=NULL)
	{
		*pID = pMag->m_sPartID[byPartIndex][rand()%NumMax];
	}
	if(pPartition!=NULL)
	{
		*pPartition = 4;
	}

	memset(pMag->m_sPartID,0,sizeof(pMag->m_sPartID));
	memset(pMag->m_sPartIDNum,0,sizeof(pMag->m_sPartIDNum));
}

//对喷火发射器伤类、伤部、伤势重新映射
void RemapHurtPartSpecial(RCVHURTINFO_T *RcvHurtInfo)
{
    RcvHurtInfo->m_HType = H_Type_I_Burn;	//烧伤

    switch(RcvHurtInfo->m_HPart)
    {
        case PARTITION_NONE:
            RcvHurtInfo->m_HurtBlood = 100;
            break;

        case PARTITION_HEAD:
            RcvHurtInfo->m_HurtBlood = 60;//危重伤5
            break;

        case PARTITION_LCHEST:
            RcvHurtInfo->m_HurtBlood = 35;//危重伤9
            break;

        case PARTITION_RCHEST:
            RcvHurtInfo->m_HurtBlood = 35;//危重伤9
            break;

        case PARTITION_ABDOMEN:
            RcvHurtInfo->m_HurtBlood = 60;//危重伤5
            break;

        case PARTITION_BACK:
            RcvHurtInfo->m_HurtBlood = 60;//危重伤5
            break;

        case PARTITION_LARM:
            RcvHurtInfo->m_HurtBlood = 35;//危重伤9
            break;

        case PARTITION_RARM:
            RcvHurtInfo->m_HurtBlood = 35;//危重伤9
            break;

        case PARTITION_LLEG:
            RcvHurtInfo->m_HurtBlood = 60;//危重伤5
            break;

        case PARTITION_RLEG:
            RcvHurtInfo->m_HurtBlood = 60;//危重伤5
            break;

        default:
            RcvHurtInfo->m_HurtBlood = 100;
            break;
    }

}

/*
-------------------------------------------------
PARTITION_HEAD = 头
1头 				5枪弹伤 	12(轻)
14眼				5枪弹伤		11(中)
1头					5枪弹伤	  	10(中)
14眼				5枪弹伤		9(重)
1头					6震荡伤		8（重）
1头					5枪弹伤	  	7（重）
2颅骨				4骨折		6（重）
2颅骨				4骨折		5（危）
10头/腹部/膀胱		9多发伤		4（危）
5头/胸部			9多发伤		3（危）
8头/腹/肝脏			9多发伤		2（危）
24胸/腹/结肠/膀胱	9多发伤		1（危）
14眼				5枪弹伤		0（死）
---------------------------------------------------------
PARTITION_LCHEST = 胸部
PARTITION_RCHEST = 胸部
18胸部 				5枪弹伤 	12 	轻
17肩 				4骨折 		11 	中
18胸部 				5枪弹伤 	10 	中
17肩 				4骨折 		9 	重
18胸部 				5枪弹伤 	8 	重
19胸/腹/肾脏 		9多发伤 	7 	重
20胸/腹/膀胱 		9多发伤 	6 	重
38腹/结肠/下肢 		9多发伤 	5 	危
31小肠 				5枪弹伤 	4 	危
19胸/腹/肾脏 		9多发伤 	3 	危
21胸/腹/脾 			9多发伤 	2 	危
24胸/腹/结肠/膀胱 	9多发伤 	1 	危
19胸/腹/肾脏 		9 多发伤 	0 	死
----------------------------------------------------------
PARTITION_ABDOMEN	= 腹部
PARTITION_BACK		= 背部
32腹部 				5枪弹伤 	12 	轻
32腹部 				5枪弹伤 	11 	中
43脊柱 				4骨折 		10 	中
31小肠 				5枪弹伤 	9 	重
32腹部 				5枪弹伤 	8 	重
35腹/结肠/膀胱 		9多发伤 	7 	重
39腹/盆腔/下肢 		9多发伤 	6 	重
38腹/结肠/下肢 		9多发伤 	5 	危
32腹部 				5枪弹伤 	4 	危
33腹/结肠/脾 		9多发伤 	3 	危
23胸/腹/结肠 		9多发伤 	2 	危
24胸/腹/结肠/膀胱 	9多发伤 	1 	危
32腹部 				5枪弹伤 	0 	死
----------------------------------------------------------
PARTITION_LARM		= 上肢
PARTITION_RARM		= 上肢
48手/手指 			5枪弹伤 	12 	轻
44上肢 				7撞击伤 	11 	中
44上肢 				5枪弹伤 	10 	中
44上肢 				5枪弹伤 	9 	重
44上肢 				5枪弹伤 	8 	重
44上肢 				3肢体离断 	7 	重
48手/手指 			4骨折 		6 	重
45肱骨 				4骨折 		5 	危
34腹/结肠/肝脏 		9多发伤 	4 	危
25胸/上肢 			9多发伤 	3 	危
23胸/腹/结肠 		9多发伤 	2 	危
27胸/腹 			9多发伤 	1 	危
25胸/上肢 			9多发伤 	0 	死
--------------------------------------------------------------
PARTITION_LLEG		= 大腿
PARTITION_RLEG		= 大腿
61大腿 				5枪弹伤 	12 	轻
61大腿 				5枪弹伤 	11 	中
61大腿 				5枪弹伤 	10 	中
66足/踝 			4骨折 		9 	重
57臀部 				5枪弹伤 	8 	重
69膝盖以下 			3肢体离断 	7 	重
43脊柱 				4骨折 		6 	重
38腹/结肠/下肢 		9多发伤 	5 	危
41腹/盆腔/四肢 		9多发伤 	4 	危
40腹/下肢 			9多发伤 	3 	危
37腹/盆腔/脾/膀胱 	9多发伤 	2 	危
24胸/腹/结肠/膀胱 	9多发伤 	1 	危
40腹/下肢 			9多发伤 	0 	死
*/
//重映射受伤部位，适应后台显示
//void RemapHurtPart(ENUM_PARTITION *Part)
void RemapHurtPart(RCVHURTINFO_T *RcvHurtInfo, u8 Where)
{
//	PARTITION_HEAD =1	,	//	头
//	PARTITION_LCHEST	,	//	左胸
//	PARTITION_RCHEST	,	//	右胸
//	PARTITION_ABDOMEN	,	//	腹部
//	PARTITION_BACK	,		//	背
//	PARTITION_LARM	,		//	左臂
//	PARTITION_RARM	,		//	右臂
//	PARTITION_LLEG	,		//	左腿
//	PARTITION_RLEG	,		//	右腿
//重映射打击部位：头部=1，胸部=18，腹部=32，上肢=44 背部=56，腿部=61

//	uint8_t RemapPart[11]={0,1,18,18,32,56,44,44,61,61,0};
//	*Part = RemapPart[*Part];

    typedef struct
    {
        uint8_t HurtPart;
        uint8_t HurtKind;
    } RemapInfo;

    uint8_t HurtDegree = 0;

    //重新映射受伤部位
    uint8_t RemapPart[11] = {0, 1, 18, 18, 32, 56, 44, 44, 61, 61, 0};
    RcvHurtInfo->m_HPart = RemapPart[RcvHurtInfo->m_HPart];

    //建立相应部位对应伤势表(伤势13个等级)
    //头部
    RemapInfo Head[13]		=	{{14, 5}, {24, 9}, {8, 9}, {5, 9}, {10, 9}, {2, 4}, {2, 4}, {1, 5}, {1, 6}, {14, 5}, {1, 5}, {14, 5}, {1, 5}};
    //胸部
    RemapInfo Chest[13]		=	{{19, 9}, {24, 9}, {21, 9}, {19, 9}, {31, 5}, {38, 9}, {20, 9}, {19, 9}, {18, 5}, {17, 4}, {18, 5}, {17, 4}, {18, 5}};
    //腹部
    RemapInfo Abdomen[13]	=	{{32, 5}, {24, 9}, {23, 9}, {33, 9}, {32, 5}, {38, 9}, {39, 9}, {35, 9}, {32, 5}, {31, 5}, {43, 4}, {32, 5}, {32, 5}};
    //背部
    RemapInfo Back[13]		=	{{32, 5}, {24, 9}, {23, 9}, {33, 9}, {32, 5}, {38, 9}, {39, 9}, {35, 9}, {32, 5}, {31, 5}, {43, 4}, {32, 5}, {32, 5}};
    //上肢
    RemapInfo Arm[13]		=	{{25, 9}, {27, 9}, {32, 9}, {25, 9}, {34, 9}, {45, 4}, {48, 4}, {44, 3}, {44, 5}, {44, 5}, {44, 5}, {44, 7}, {48, 5}};
    //大腿
    RemapInfo Leg[13]		=	{{40, 9}, {24, 9}, {37, 9}, {40, 9}, {41, 9}, {38, 9}, {43, 4}, {69, 3}, {57, 5}, {66, 4}, {61, 5}, {61, 5}, {61, 5}};

    //根据血量获取伤势
    HurtDegree =  GetInjuryDegree(100 - RcvHurtInfo->m_HurtBlood);

    //根据相应伤势表替换之前的值
    switch((u8)RcvHurtInfo->m_HPart)
    {
        case 1:		//头
            RcvHurtInfo->m_HType = Head[HurtDegree].HurtKind;
            RcvHurtInfo->m_HPart = Head[HurtDegree].HurtPart;
            break;

        case 18:	//胸
            RcvHurtInfo->m_HType = Chest[HurtDegree].HurtKind;
            RcvHurtInfo->m_HPart = Chest[HurtDegree].HurtPart;
            break;

        case 32:	//腹
            RcvHurtInfo->m_HType = Abdomen[HurtDegree].HurtKind;
            RcvHurtInfo->m_HPart = Abdomen[HurtDegree].HurtPart;
            break;

        case 44:	//上肢
            RcvHurtInfo->m_HType = Back[HurtDegree].HurtKind;
            RcvHurtInfo->m_HPart = Back[HurtDegree].HurtPart;
            break;

        case 56:	//背部
            RcvHurtInfo->m_HType = Arm[HurtDegree].HurtKind;
            RcvHurtInfo->m_HPart = Arm[HurtDegree].HurtPart;
            break;

        case 61:	//腿部
            RcvHurtInfo->m_HType = Leg[HurtDegree].HurtKind;
            RcvHurtInfo->m_HPart = Leg[HurtDegree].HurtPart;
            break;

        default:
            break;
    }

    //来自爆炸伤，尽可能把枪弹伤改成爆炸伤
    if(1 == Where && 5 == RcvHurtInfo->m_HType)
    {
        RcvHurtInfo->m_HType = 8;
    }
}

//衣服被击中处理
void Laser_GetShotHandle_Slave(LASERCOMTASK_MAG *pMag)
{
    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 2);

    //榴弹炮处理，周边会造成杀伤:
    if(pMag->m_sRcvHurtInfo.m_RT_Type == 0x37 || pMag->m_sRcvHurtInfo.m_RT_Type == 0x40 || pMag->m_sRcvHurtInfo.m_RT_Type == 0x25
        || pMag->m_sRcvHurtInfo.m_RT_Type == 0x41 || pMag->m_sRcvHurtInfo.m_RT_Type == 0x42)
    {
        CollateralDamage(pMag->m_sRcvHurtInfo);
    }
}

void Laser_GetShotHandle_Master(LASERCOMTASK_MAG *pMag)
{
    //已阵亡，不再处理打击信息
    if(IsDie(g_sUserParam.m_eStatus))
        return;

    //获取血量
    pMag->m_sRcvHurtInfo.m_HurtBlood = GetDirectWeaponDamage(pMag->m_byUniID, pMag->m_sRcvHurtInfo.m_HPart);

    if(pMag->m_sRcvHurtInfo.m_HurtBlood != 0)
    {
        Wtn6_Play(WTN6_YX_ZDHX2);
    }
		
    pMag->m_sRcvHurtInfo.m_HType = H_Type_I_BulletWounded;

    if(pMag->m_sRcvHurtInfo.m_RT_Type == 0x43)  //是定向雷，将激光打击伤改为爆炸伤
    {
        pMag->m_sRcvHurtInfo.m_HType = H_Type_I_Wounded;
    }

    g_sInjuryMag.m_Ttype 	= pMag->m_sRcvHurtInfo.m_RT_Type;
    g_sInjuryMag.m_Tsubtype = pMag->m_sRcvHurtInfo.m_RT_Subtype;

    play_StruckPart(pMag->m_sRcvHurtInfo.m_HPart);
    //屏幕显示
    HMI_GetInjury(&g_sHmiComTask_Mag, pMag->m_sRcvHurtInfo.m_HPart, pMag->m_sRcvHurtInfo.m_RT_Type, pMag->m_sRcvHurtInfo.m_RT_Subtype);

    //重映射受伤区域
    if(pMag->m_sRcvHurtInfo.m_RT_Type == 0x3b)
    {
        RemapHurtPartSpecial(&pMag->m_sRcvHurtInfo);//是喷火器，为烧伤
    }
    else
    {
        RemapHurtPart(&pMag->m_sRcvHurtInfo, 0);
    }
		
    //伤害处理
    UpdateDamage(pMag->m_sRcvHurtInfo);
    //事件上传
		if(g_sUserParam.m_byMode == GAME_GOING)
			DamageReportEventUploadHandle(&pMag->m_sRcvHurtInfo);

    //如果是榴弹伤，背夹接收后通过蓝牙发送给头盔
    if((pMag->m_sRcvHurtInfo.m_RT_Type == 0x35 || pMag->m_sRcvHurtInfo.m_RT_Type == 0x37 || pMag->m_sRcvHurtInfo.m_RT_Type == 0x40 
        || pMag->m_sRcvHurtInfo.m_RT_Type == 0x42 ||  pMag->m_sRcvHurtInfo.m_RT_Type == 0x41 ||pMag->m_sRcvHurtInfo.m_RT_Type == 0x25 ))
    {
				CollateralDamage(pMag->m_sRcvHurtInfo);
    }
}

void Laser_GetShotHandle(LASERCOMTASK_MAG *pMag, uint8_t IsMaster)
{
    uint8_t Idx = 0;

    //Laser_CalcPartition(&Idx, (uint8_t*)&pMag->m_sRcvHurtInfo.m_HPart, pMag); //计算最终受伤部位

    g_sResComTask_Mag.BackupArea[0].Decter[Idx] = 1;

    //主控收到中弹信息直接进行毁伤处理
    Laser_GetShotHandle_Master(pMag);
}

void Laser_CtrlLedOff(uint8_t Id,LASERCOMTASK_MAG *pMag)
{
	assert_param(pMag != NULL);

	if(Id < UserParam_GetActLaserCnt())
	{
		pMag->m_aLedCtrlMag[Id].m_bIsCtrlOff = TRUE;
		pMag->m_aLedCtrlMag[Id].m_bIsCtrlOn = FALSE;
		pMag->m_aLedCtrlMag[Id].m_bIsToggle = FALSE;
		pMag->m_aLedCtrlMag[Id].m_wLedOnTimer = 0;
	}	
}

//探头闪灯指示，最后常亮
void Laser_CtrlLedToggle(uint8_t Id, LASERCOMTASK_MAG *pMag)
{
    uint32_t i;

    assert_param(pMag != NULL);

    //白天模式下
    if(g_sUserParam.m_IsNight == FALSE)
    {
				if(Id < UserParam_GetActLaserCnt())
				{
						pMag->m_aLedCtrlMag[Id].m_bIsToggle = TRUE;
						pMag->m_aLedCtrlMag[Id].m_bIsCtrlOn = FALSE;
						pMag->m_aLedCtrlMag[Id].m_wLedOnTimer = 0;
				}
    }
}

void Laser_CtrlLedOn(u8 Id, LASERCOMTASK_MAG *pMag)
{
	assert_param(pMag!=NULL);

	//白天模式下
	if(Id < UserParam_GetActLaserCnt() && g_sUserParam.m_IsNight == FALSE)
	{
		pMag->m_aLedCtrlMag[Id].m_bIsCtrlOn = TRUE;
		pMag->m_aLedCtrlMag[Id].m_bIsToggle = FALSE;
		pMag->m_aLedCtrlMag[Id].m_wLedOnTimer = 0;
	}
}

void DectorCheatHandle(LASERCOMTASK_MAG *pMag)
{
    if(TRUE != g_sResComTask_Mag.DrillParam.DisableCheat)
    {
        //10s检测一次探头作弊计数
				if(pMag->DectorCheatCntTimer++ > 100)
        {
            pMag->DectorCheatCntTimer = 0;

            //头盔和背夹都有遮挡
            if(pMag->m_DectorShelterFlg == TRUE && g_sVLaserMag.m_DectorShelterFlg == TRUE)
            {
                pMag->m_DectorCheatCnt ++;	//探头遮挡，记录+1
                pMag->m_DectorShelterFlg = FALSE;
                g_sVLaserMag.m_DectorShelterFlg = FALSE;
            }
            else if(pMag->m_DectorShelterFlg == FALSE || g_sVLaserMag.m_DectorShelterFlg == FALSE)
            {
                pMag->m_DectorShelterFlg = FALSE;
                g_sVLaserMag.m_DectorShelterFlg = FALSE;

                if(pMag->m_DectorCheatCnt > 0)
                {
                    pMag->m_DectorCheatCnt --;	//探头未遮挡，记录-1;
                }
            }
        }

        //1分钟上报一次探头作弊事件
				if(pMag->DectorCheatJudgeTimer++ > 300)
        {
            pMag->DectorCheatJudgeTimer = 0;

            //1分钟内有8次遮挡记录
            if(POSE_WO == g_sMpu6050Mag.Pose || POSE_PUFU == g_sMpu6050Mag.Pose)
            {
                if(pMag->m_DectorCheatCnt >= 3)
                {
                    if(g_sResComTask_Mag.m_IsAccess == TRUE)
                    {
                        DectorCheatUploadHandle();
                    }
                }
            }
            else
            {
                if(pMag->m_DectorCheatCnt >= 2)
                {
                    if(g_sResComTask_Mag.m_IsAccess == TRUE)
                    {
                        DectorCheatUploadHandle();
                    }
                }
            }
            pMag->m_DectorCheatCnt = 0;
        }
    }
}

//激光发送处理，慢任务
void Laser_TimedProcess(LASERCOMTASK_MAG *pMag)
{
		LASER_LED_MANAGER *pLedMag = NULL;
    uint8_t i = 0;

    //遍历所有灯，看是否需要点亮或熄灭操作
    for(i = 0; i < UserParam_GetActLaserCnt(); i++)
    {
        pLedMag = &pMag->m_aLedCtrlMag[i];

        if(pLedMag->m_bIsToggle == TRUE)
        {
            if(pLedMag->m_wLedOnTimer >= LASER_CTRL_LED_ON)
            {
                pLedMag->m_bIsToggle = FALSE;
                Laser_CtrlLedOn(i, pMag);
            }
            else
            {
								Detector_Led_Proc(pMag, i, pLedMag->m_wLedOnTimer % 10 == 0 ? TRUE : FALSE);
						}

            pLedMag->m_wLedOnTimer++;
        }
        else if(pLedMag->m_bIsCtrlOn == TRUE)
        {
            Detector_Led_Proc(pMag, i, TRUE);
            pLedMag->m_bIsCtrlOn = FALSE;
            pLedMag->m_wLedOnTimer = 0;
            pLedMag->m_bIsOn = TRUE;
        }
        else if(pLedMag->m_bIsCtrlOff == TRUE)
        {
						if(pLedMag->m_wLedOnTimer++ >= LASER_CTRL_LED_ON)
            {
                Detector_Led_Proc(pMag, i, FALSE);
	
                pLedMag->m_bIsCtrlOff = FALSE;
                pLedMag->m_bIsOn = FALSE;
            }
        }

    }

		
    //帧间间隔,接收到中弹信息
    if(pMag->m_bIsShoted == TRUE)
    {
        //超过0.4s未接收到新的中弹信息,根据各分区受损情况计算受伤分区
        if((pMag->m_byLaserTimer++) >= 4)
        {
						Laser_GetShotHandle(pMag, TRUE);

            pMag->m_bIsShoted = FALSE;
        }
    }
		
    //探头遮挡作弊裁决
    DectorCheatHandle(pMag);
		
		//人员死亡后，探头闪烁
}


LASERCOMTASK_MAG g_sLaserComTask_Mag;
VCOM_LASER_MAG g_sVLaserMag;
/****************************************************************
@FUNCTION：获取激光接收端口管理句柄
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 

****************************************************************/
LASERCOMTASK_MAG *LaserComTask_HandleGet(void)
{
    return &g_sLaserComTask_Mag;
}

/****************************************************************
@FUNCTION：获取激光接收端口任务管理句柄
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 

****************************************************************/
COMTASK_MAG *LaserComTask_TaskHandleGet(void)
{
    return &g_sLaserComTask_Mag.m_sTaskMag;
}

/****************************************************************
@FUNCTION：激光接收串口功能初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sLaserComTask_Mag

****************************************************************/
void LaserComTask_Init(void)
{
    LASERCOMTASK_MAG *pMag = LaserComTask_HandleGet();

    memset((void *)pMag, 0, sizeof(LASERCOMTASK_MAG));

    COM_HW_OPERATIONS HwOps =
    {
        .hw_init = LaserCom_ComInit,
        .read = LaserCom_ComRead,
        .write = LaserCom_ComWrite,
        .gpio_power_on  = LaserCom_GpioPowerOn,
        .gpio_power_off = LaserCom_GpioPowerOff,
    };

    LaserCom_LowLevelInit(HwOps);

    pMag->m_pLLMag = LaserCom_GetLLMagHandle();

    pMag->m_bIsInit = ComTask_Init(&pMag->m_sTaskMag, LASER_COM_FRAME_LEN_MAX, 100);

    //接口上电
    LaserCom_PowerOn();

    //获取在线探头数
    LaserCom_Prot_GetOnlineNum(pMag, 0x0A, 0x0A);
}


/****************************************************************
@FUNCTION：激光接收串口功能反初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sLaserComTask_Mag

****************************************************************/
void LaserComTask_DeInit(void)
{
    LASERCOMTASK_MAG *pMag = LaserComTask_HandleGet();

    ComTask_DeInit(&pMag->m_sTaskMag);

    memset((void *)pMag, 0, sizeof(LASERCOMTASK_MAG));
}




//激光接收串口协议处理
void LaserCom_Analysis(uint8_t *pBuf, uint16_t BufLen, void *pHandle)
{
    LASERCOMTASK_MAG *pMag = (LASERCOMTASK_MAG *)pHandle;
    uint8_t Offset;
 
    switch(pBuf[2])
    {
				case 0x10:
						LaserProtocalDeal(pMag, pBuf, BufLen, FALSE);		//支持五面探头和小探头
						break;

        case 0xA0:
            Offset = 4;
            //pMag->m_byActualNum = pBuf[Offset];
            UserParam_SetActLaserCnt(pBuf[Offset]);
						if(UserParam_GetActLaserCnt() < 0x0A)
						{
								Wtn6_Play(WTN6_Cloth_IRTrouble);
						}
						g_sUserParam.m_sTsChar1.m_bFailureState = FALSE;
				
						if(g_sResComTask_Mag.Vios.DetectorLoss == FALSE)
						{
								g_sResComTask_Mag.DetectorLossStartTime = 0;
						}
						else if(g_sResComTask_Mag.Vios.DetectorLoss == TRUE)
						{
								g_sResComTask_Mag.DetectorLossEndTime++;
						}
            break;

				case 0xAF:
						pMag->m_byDectorShelterID = pBuf[3];
						pMag->m_DectorShelterFlg = TRUE;
						break;

        default:
            break;
    }


}



/****************************************************************
@FUNCTION：激光接收串口功能任务主进程
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sZigComTask_Mag

****************************************************************/
void LaserComTask_Process(void)
{
    LASERCOMTASK_MAG *pMag = LaserComTask_HandleGet();
    COMTASK_MAG *pComTaskMag = &pMag->m_sTaskMag;
    uint16_t i, Len = pComTaskMag->m_wProtBufMaxLen - pComTaskMag->m_wProtBufLen;
    uint32_t IsCutted = FALSE;//, IsLong55AA = FALSE;
    uint32_t NowTime = GetSysRunMilliSecond();
    FRAME_BUF *pFrame = NULL;

    //定时任务
    if((NowTime >  pComTaskMag->m_dwLastScheduleTime) &&
            (NowTime -  pComTaskMag->m_dwLastScheduleTime >= pComTaskMag->m_dwScheduleTime) )
    {
        pComTaskMag->m_dwLastScheduleTime = NowTime;

        //优先保证底层操作
        if(FALSE == LaserCom_LowLevelTimedProcess())
        {
            //检查发送队列
            pFrame = LinkQueue_Header(pComTaskMag->m_pSendQueue);

            if(pFrame != NULL)
            {
								pMag->m_pLLMag->m_sHwOps.write(pFrame->m_byBuf, pFrame->m_wBufLen);
                LinkQueue_Retrieve(pComTaskMag->m_pSendQueue);
            }

            /***********应用逻辑***************************/
            // 1. 如果50s端口没收到数据，则获取在线探头数
            if((NowTime - pComTaskMag->m_dwLastCommTime) >= 60000)
            {
                LaserCom_Prot_GetOnlineNum(pMag, 0x0A, 0x0A);
								Laser_CtrlAutoNumbered(pMag);
                g_sUserParam.m_sTsChar1.m_bFailureState = TRUE;
								pComTaskMag->m_dwLastCommTime = NowTime;
            }
						Laser_TimedProcess(pMag);
        }
    }

    //串口接收处理
    if(TRUE == pMag->m_pLLMag->m_sHwOps.read(&pComTaskMag->m_pProtBuf[pComTaskMag->m_wProtBufLen], &Len))
    {
        #ifdef SEND_TO_UPCOM
        //转发数据到上行口
        ResComTask_SendToUpcom(&pComTaskMag->m_pProtBuf[pComTaskMag->m_wProtBufLen], Len, 0x06, CTRL_RECV);
        pComTaskMag->m_wProtBufLen = 0;
        #endif

        #if 1
        pComTaskMag->m_wProtBufLen += Len;
        pComTaskMag->m_dwLastCommTime = NowTime;

        for(i = 0; i < pComTaskMag->m_wProtBufLen;)
        {
            Len = pComTaskMag->m_wProtBufLen - i;

            if(TRUE == Protocal_Find5AAAFrame(&pComTaskMag->m_pProtBuf[i], &Len, &IsCutted))
            {
                if(TRUE == IsCutted)
                {
                    memcpy(pComTaskMag->m_pProtBuf, &pComTaskMag->m_pProtBuf[i], pComTaskMag->m_wProtBufLen - i);
                    pComTaskMag->m_wProtBufLen -= i ;
                    break;
                }
                else
                {
                    LaserCom_Analysis(&pComTaskMag->m_pProtBuf[i], Len, (void *)pMag);

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

        #endif

    }

}

#if 0
/****************************************************************
@FUNCTION：激光接收串口功能获取当前在线探头数
@INPUT： 无
@OUTPUT: 无
@RETURN: 在线探头数
@AUTHOR：xfw
@SPECIAL: g_sZigComTask_Mag

****************************************************************/
uint8_t LaserComTask_GetActualNum(void)
{
    LASERCOMTASK_MAG *pMag = &g_sLaserComTask_Mag;

    return pMag->m_byActualNum;
}
#endif

