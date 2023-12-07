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

/*************�ײ�ʵ��************************************************/
//���ڳ�ʼ��
void LaserCom_ComInit(void)
{
    UART_CONFIG Config = {0};
    UART_MANAGER *pUartMag = &g_sUartSetMag.Com[LASER_COM];

    memcpy((void *)&Config, (void *)&g_scUartDefConfig[LASER_COM], sizeof(UART_CONFIG));
    Config.m_dwBaudRate = LASER_COM_BAUDRATE;
    //Config.TxMode = MODE_INTERRUPT;
    //Config.RxMode = MODE_INTERRUPT;
    //��Ӧ��������
    Uart_ComConfiguration(Config, pUartMag);
    Uart_ComInit(pUartMag);

    LaserCom_GpioPowerOnInit();
}

//����д����
uint32_t LaserCom_ComWrite(uint8_t *pBuf, uint16_t Len)
{
    #ifdef LASER_COM_LOG_OUTPUT

    LOG("LASERCOM(%d)->", Len);
    printf_with_data(pBuf, Len);

    #endif

    return Uart_SendBuf(&g_sUartSetMag.Com[LASER_COM], pBuf, Len);
}

//���ڶ�����
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
            //����������,��������������
            memcpy((void *)pBuf, (void *)pUartMag->m_byRecvBuf, *pLen);
        }

        pUartMag->m_wRecvLen = 0;
        pUartMag->m_bIsComRecv = FALSE;

        return TRUE;
    }

    return FALSE;
}

/*************�ײ�ʵ�ֽ���************************************************/


/****************************************************************
@FUNCTION����ȡ̽ͷ������̽ͷ��
@INPUT��
    @param  MaxCnt - ���ܵ����̽ͷ��
    @param  IntVal - ����̽ͷ������ʱ����λ10ms����Ϊ0,��ȡĬ��ֵ10
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ���ظ�������ģ���������MaxCnt����IntValֵ
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
    //���Э�鳤��
    pFrame->m_byBuf[1] = pFrame->m_wBufLen;

    LinkQueue_Append(pMag->m_sTaskMag.m_pSendQueue, (void*)pFrame);

}

/****************************************************************
@FUNCTION��LASER���������Զ��������
@INPUT��LASER�ײ����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:��
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
    //���Э�鳤��
    pFrame->m_byBuf[1] = pFrame->m_wBufLen;

    LinkQueue_Append(pMag->m_sTaskMag.m_pSendQueue, (void*)pFrame);

}

/****************************************************************
@FUNCTION��LASER����ģʽ�л�����
@INPUT��LASER�ײ����ṹ��ָ�룬ģʽ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:��
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
    //���Э�鳤��
    pFrame->m_byBuf[1] = pFrame->m_wBufLen;

    LinkQueue_Append(pMag->m_sTaskMag.m_pSendQueue, (void*)pFrame);

}

//RGB����׹⣬��˸
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
@FUNCTION��LASER���͵ƿ�����
@INPUT��LASER�ײ����ṹ��ָ�룬�Ʊ�ţ���״̬
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:�Ʊ�Ŵ�1��ʼ
****************************************************************/
void Detector_Led_Proc(LASERCOMTASK_MAG *pMag,uint8_t a_Id,uint32_t a_IsOn) /* ��̽ͷ */
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
	//���Э�鳤��
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
	//���Э�鳤��
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
	//���Э�鳤��
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

//������������ࡢ�˲�����������ӳ��
void RemapHurtPartSpecial(RCVHURTINFO_T *RcvHurtInfo)
{
    RcvHurtInfo->m_HType = H_Type_I_Burn;	//����

    switch(RcvHurtInfo->m_HPart)
    {
        case PARTITION_NONE:
            RcvHurtInfo->m_HurtBlood = 100;
            break;

        case PARTITION_HEAD:
            RcvHurtInfo->m_HurtBlood = 60;//Σ����5
            break;

        case PARTITION_LCHEST:
            RcvHurtInfo->m_HurtBlood = 35;//Σ����9
            break;

        case PARTITION_RCHEST:
            RcvHurtInfo->m_HurtBlood = 35;//Σ����9
            break;

        case PARTITION_ABDOMEN:
            RcvHurtInfo->m_HurtBlood = 60;//Σ����5
            break;

        case PARTITION_BACK:
            RcvHurtInfo->m_HurtBlood = 60;//Σ����5
            break;

        case PARTITION_LARM:
            RcvHurtInfo->m_HurtBlood = 35;//Σ����9
            break;

        case PARTITION_RARM:
            RcvHurtInfo->m_HurtBlood = 35;//Σ����9
            break;

        case PARTITION_LLEG:
            RcvHurtInfo->m_HurtBlood = 60;//Σ����5
            break;

        case PARTITION_RLEG:
            RcvHurtInfo->m_HurtBlood = 60;//Σ����5
            break;

        default:
            RcvHurtInfo->m_HurtBlood = 100;
            break;
    }

}

/*
-------------------------------------------------
PARTITION_HEAD = ͷ
1ͷ 				5ǹ���� 	12(��)
14��				5ǹ����		11(��)
1ͷ					5ǹ����	  	10(��)
14��				5ǹ����		9(��)
1ͷ					6����		8���أ�
1ͷ					5ǹ����	  	7���أ�
2­��				4����		6���أ�
2­��				4����		5��Σ��
10ͷ/����/����		9�෢��		4��Σ��
5ͷ/�ز�			9�෢��		3��Σ��
8ͷ/��/����			9�෢��		2��Σ��
24��/��/�᳦/����	9�෢��		1��Σ��
14��				5ǹ����		0������
---------------------------------------------------------
PARTITION_LCHEST = �ز�
PARTITION_RCHEST = �ز�
18�ز� 				5ǹ���� 	12 	��
17�� 				4���� 		11 	��
18�ز� 				5ǹ���� 	10 	��
17�� 				4���� 		9 	��
18�ز� 				5ǹ���� 	8 	��
19��/��/���� 		9�෢�� 	7 	��
20��/��/���� 		9�෢�� 	6 	��
38��/�᳦/��֫ 		9�෢�� 	5 	Σ
31С�� 				5ǹ���� 	4 	Σ
19��/��/���� 		9�෢�� 	3 	Σ
21��/��/Ƣ 			9�෢�� 	2 	Σ
24��/��/�᳦/���� 	9�෢�� 	1 	Σ
19��/��/���� 		9 �෢�� 	0 	��
----------------------------------------------------------
PARTITION_ABDOMEN	= ����
PARTITION_BACK		= ����
32���� 				5ǹ���� 	12 	��
32���� 				5ǹ���� 	11 	��
43���� 				4���� 		10 	��
31С�� 				5ǹ���� 	9 	��
32���� 				5ǹ���� 	8 	��
35��/�᳦/���� 		9�෢�� 	7 	��
39��/��ǻ/��֫ 		9�෢�� 	6 	��
38��/�᳦/��֫ 		9�෢�� 	5 	Σ
32���� 				5ǹ���� 	4 	Σ
33��/�᳦/Ƣ 		9�෢�� 	3 	Σ
23��/��/�᳦ 		9�෢�� 	2 	Σ
24��/��/�᳦/���� 	9�෢�� 	1 	Σ
32���� 				5ǹ���� 	0 	��
----------------------------------------------------------
PARTITION_LARM		= ��֫
PARTITION_RARM		= ��֫
48��/��ָ 			5ǹ���� 	12 	��
44��֫ 				7ײ���� 	11 	��
44��֫ 				5ǹ���� 	10 	��
44��֫ 				5ǹ���� 	9 	��
44��֫ 				5ǹ���� 	8 	��
44��֫ 				3֫����� 	7 	��
48��/��ָ 			4���� 		6 	��
45�Ź� 				4���� 		5 	Σ
34��/�᳦/���� 		9�෢�� 	4 	Σ
25��/��֫ 			9�෢�� 	3 	Σ
23��/��/�᳦ 		9�෢�� 	2 	Σ
27��/�� 			9�෢�� 	1 	Σ
25��/��֫ 			9�෢�� 	0 	��
--------------------------------------------------------------
PARTITION_LLEG		= ����
PARTITION_RLEG		= ����
61���� 				5ǹ���� 	12 	��
61���� 				5ǹ���� 	11 	��
61���� 				5ǹ���� 	10 	��
66��/�� 			4���� 		9 	��
57�β� 				5ǹ���� 	8 	��
69ϥ������ 			3֫����� 	7 	��
43���� 				4���� 		6 	��
38��/�᳦/��֫ 		9�෢�� 	5 	Σ
41��/��ǻ/��֫ 		9�෢�� 	4 	Σ
40��/��֫ 			9�෢�� 	3 	Σ
37��/��ǻ/Ƣ/���� 	9�෢�� 	2 	Σ
24��/��/�᳦/���� 	9�෢�� 	1 	Σ
40��/��֫ 			9�෢�� 	0 	��
*/
//��ӳ�����˲�λ����Ӧ��̨��ʾ
//void RemapHurtPart(ENUM_PARTITION *Part)
void RemapHurtPart(RCVHURTINFO_T *RcvHurtInfo, u8 Where)
{
//	PARTITION_HEAD =1	,	//	ͷ
//	PARTITION_LCHEST	,	//	����
//	PARTITION_RCHEST	,	//	����
//	PARTITION_ABDOMEN	,	//	����
//	PARTITION_BACK	,		//	��
//	PARTITION_LARM	,		//	���
//	PARTITION_RARM	,		//	�ұ�
//	PARTITION_LLEG	,		//	����
//	PARTITION_RLEG	,		//	����
//��ӳ������λ��ͷ��=1���ز�=18������=32����֫=44 ����=56���Ȳ�=61

//	uint8_t RemapPart[11]={0,1,18,18,32,56,44,44,61,61,0};
//	*Part = RemapPart[*Part];

    typedef struct
    {
        uint8_t HurtPart;
        uint8_t HurtKind;
    } RemapInfo;

    uint8_t HurtDegree = 0;

    //����ӳ�����˲�λ
    uint8_t RemapPart[11] = {0, 1, 18, 18, 32, 56, 44, 44, 61, 61, 0};
    RcvHurtInfo->m_HPart = RemapPart[RcvHurtInfo->m_HPart];

    //������Ӧ��λ��Ӧ���Ʊ�(����13���ȼ�)
    //ͷ��
    RemapInfo Head[13]		=	{{14, 5}, {24, 9}, {8, 9}, {5, 9}, {10, 9}, {2, 4}, {2, 4}, {1, 5}, {1, 6}, {14, 5}, {1, 5}, {14, 5}, {1, 5}};
    //�ز�
    RemapInfo Chest[13]		=	{{19, 9}, {24, 9}, {21, 9}, {19, 9}, {31, 5}, {38, 9}, {20, 9}, {19, 9}, {18, 5}, {17, 4}, {18, 5}, {17, 4}, {18, 5}};
    //����
    RemapInfo Abdomen[13]	=	{{32, 5}, {24, 9}, {23, 9}, {33, 9}, {32, 5}, {38, 9}, {39, 9}, {35, 9}, {32, 5}, {31, 5}, {43, 4}, {32, 5}, {32, 5}};
    //����
    RemapInfo Back[13]		=	{{32, 5}, {24, 9}, {23, 9}, {33, 9}, {32, 5}, {38, 9}, {39, 9}, {35, 9}, {32, 5}, {31, 5}, {43, 4}, {32, 5}, {32, 5}};
    //��֫
    RemapInfo Arm[13]		=	{{25, 9}, {27, 9}, {32, 9}, {25, 9}, {34, 9}, {45, 4}, {48, 4}, {44, 3}, {44, 5}, {44, 5}, {44, 5}, {44, 7}, {48, 5}};
    //����
    RemapInfo Leg[13]		=	{{40, 9}, {24, 9}, {37, 9}, {40, 9}, {41, 9}, {38, 9}, {43, 4}, {69, 3}, {57, 5}, {66, 4}, {61, 5}, {61, 5}, {61, 5}};

    //����Ѫ����ȡ����
    HurtDegree =  GetInjuryDegree(100 - RcvHurtInfo->m_HurtBlood);

    //������Ӧ���Ʊ��滻֮ǰ��ֵ
    switch((u8)RcvHurtInfo->m_HPart)
    {
        case 1:		//ͷ
            RcvHurtInfo->m_HType = Head[HurtDegree].HurtKind;
            RcvHurtInfo->m_HPart = Head[HurtDegree].HurtPart;
            break;

        case 18:	//��
            RcvHurtInfo->m_HType = Chest[HurtDegree].HurtKind;
            RcvHurtInfo->m_HPart = Chest[HurtDegree].HurtPart;
            break;

        case 32:	//��
            RcvHurtInfo->m_HType = Abdomen[HurtDegree].HurtKind;
            RcvHurtInfo->m_HPart = Abdomen[HurtDegree].HurtPart;
            break;

        case 44:	//��֫
            RcvHurtInfo->m_HType = Back[HurtDegree].HurtKind;
            RcvHurtInfo->m_HPart = Back[HurtDegree].HurtPart;
            break;

        case 56:	//����
            RcvHurtInfo->m_HType = Arm[HurtDegree].HurtKind;
            RcvHurtInfo->m_HPart = Arm[HurtDegree].HurtPart;
            break;

        case 61:	//�Ȳ�
            RcvHurtInfo->m_HType = Leg[HurtDegree].HurtKind;
            RcvHurtInfo->m_HPart = Leg[HurtDegree].HurtPart;
            break;

        default:
            break;
    }

    //���Ա�ը�ˣ������ܰ�ǹ���˸ĳɱ�ը��
    if(1 == Where && 5 == RcvHurtInfo->m_HType)
    {
        RcvHurtInfo->m_HType = 8;
    }
}

//�·������д���
void Laser_GetShotHandle_Slave(LASERCOMTASK_MAG *pMag)
{
    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 2);

    //���ڴ����ܱ߻����ɱ��:
    if(pMag->m_sRcvHurtInfo.m_RT_Type == 0x37 || pMag->m_sRcvHurtInfo.m_RT_Type == 0x40 || pMag->m_sRcvHurtInfo.m_RT_Type == 0x25
        || pMag->m_sRcvHurtInfo.m_RT_Type == 0x41 || pMag->m_sRcvHurtInfo.m_RT_Type == 0x42)
    {
        CollateralDamage(pMag->m_sRcvHurtInfo);
    }
}

void Laser_GetShotHandle_Master(LASERCOMTASK_MAG *pMag)
{
    //�����������ٴ�������Ϣ
    if(IsDie(g_sUserParam.m_eStatus))
        return;

    //��ȡѪ��
    pMag->m_sRcvHurtInfo.m_HurtBlood = GetDirectWeaponDamage(pMag->m_byUniID, pMag->m_sRcvHurtInfo.m_HPart);

    if(pMag->m_sRcvHurtInfo.m_HurtBlood != 0)
    {
        Wtn6_Play(WTN6_YX_ZDHX2);
    }
		
    pMag->m_sRcvHurtInfo.m_HType = H_Type_I_BulletWounded;

    if(pMag->m_sRcvHurtInfo.m_RT_Type == 0x43)  //�Ƕ����ף����������˸�Ϊ��ը��
    {
        pMag->m_sRcvHurtInfo.m_HType = H_Type_I_Wounded;
    }

    g_sInjuryMag.m_Ttype 	= pMag->m_sRcvHurtInfo.m_RT_Type;
    g_sInjuryMag.m_Tsubtype = pMag->m_sRcvHurtInfo.m_RT_Subtype;

    play_StruckPart(pMag->m_sRcvHurtInfo.m_HPart);
    //��Ļ��ʾ
    HMI_GetInjury(&g_sHmiComTask_Mag, pMag->m_sRcvHurtInfo.m_HPart, pMag->m_sRcvHurtInfo.m_RT_Type, pMag->m_sRcvHurtInfo.m_RT_Subtype);

    //��ӳ����������
    if(pMag->m_sRcvHurtInfo.m_RT_Type == 0x3b)
    {
        RemapHurtPartSpecial(&pMag->m_sRcvHurtInfo);//���������Ϊ����
    }
    else
    {
        RemapHurtPart(&pMag->m_sRcvHurtInfo, 0);
    }
		
    //�˺�����
    UpdateDamage(pMag->m_sRcvHurtInfo);
    //�¼��ϴ�
		if(g_sUserParam.m_byMode == GAME_GOING)
			DamageReportEventUploadHandle(&pMag->m_sRcvHurtInfo);

    //��������ˣ����н��պ�ͨ���������͸�ͷ��
    if((pMag->m_sRcvHurtInfo.m_RT_Type == 0x35 || pMag->m_sRcvHurtInfo.m_RT_Type == 0x37 || pMag->m_sRcvHurtInfo.m_RT_Type == 0x40 
        || pMag->m_sRcvHurtInfo.m_RT_Type == 0x42 ||  pMag->m_sRcvHurtInfo.m_RT_Type == 0x41 ||pMag->m_sRcvHurtInfo.m_RT_Type == 0x25 ))
    {
				CollateralDamage(pMag->m_sRcvHurtInfo);
    }
}

void Laser_GetShotHandle(LASERCOMTASK_MAG *pMag, uint8_t IsMaster)
{
    uint8_t Idx = 0;

    //Laser_CalcPartition(&Idx, (uint8_t*)&pMag->m_sRcvHurtInfo.m_HPart, pMag); //�����������˲�λ

    g_sResComTask_Mag.BackupArea[0].Decter[Idx] = 1;

    //�����յ��е���Ϣֱ�ӽ��л��˴���
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

//̽ͷ����ָʾ�������
void Laser_CtrlLedToggle(uint8_t Id, LASERCOMTASK_MAG *pMag)
{
    uint32_t i;

    assert_param(pMag != NULL);

    //����ģʽ��
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

	//����ģʽ��
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
        //10s���һ��̽ͷ���׼���
				if(pMag->DectorCheatCntTimer++ > 100)
        {
            pMag->DectorCheatCntTimer = 0;

            //ͷ���ͱ��ж����ڵ�
            if(pMag->m_DectorShelterFlg == TRUE && g_sVLaserMag.m_DectorShelterFlg == TRUE)
            {
                pMag->m_DectorCheatCnt ++;	//̽ͷ�ڵ�����¼+1
                pMag->m_DectorShelterFlg = FALSE;
                g_sVLaserMag.m_DectorShelterFlg = FALSE;
            }
            else if(pMag->m_DectorShelterFlg == FALSE || g_sVLaserMag.m_DectorShelterFlg == FALSE)
            {
                pMag->m_DectorShelterFlg = FALSE;
                g_sVLaserMag.m_DectorShelterFlg = FALSE;

                if(pMag->m_DectorCheatCnt > 0)
                {
                    pMag->m_DectorCheatCnt --;	//̽ͷδ�ڵ�����¼-1;
                }
            }
        }

        //1�����ϱ�һ��̽ͷ�����¼�
				if(pMag->DectorCheatJudgeTimer++ > 300)
        {
            pMag->DectorCheatJudgeTimer = 0;

            //1��������8���ڵ���¼
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

//���ⷢ�ʹ���������
void Laser_TimedProcess(LASERCOMTASK_MAG *pMag)
{
		LASER_LED_MANAGER *pLedMag = NULL;
    uint8_t i = 0;

    //�������еƣ����Ƿ���Ҫ������Ϩ�����
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

		
    //֡����,���յ��е���Ϣ
    if(pMag->m_bIsShoted == TRUE)
    {
        //����0.4sδ���յ��µ��е���Ϣ,���ݸ�������������������˷���
        if((pMag->m_byLaserTimer++) >= 4)
        {
						Laser_GetShotHandle(pMag, TRUE);

            pMag->m_bIsShoted = FALSE;
        }
    }
		
    //̽ͷ�ڵ����ײþ�
    DectorCheatHandle(pMag);
		
		//��Ա������̽ͷ��˸
}


LASERCOMTASK_MAG g_sLaserComTask_Mag;
VCOM_LASER_MAG g_sVLaserMag;
/****************************************************************
@FUNCTION����ȡ������ն˿ڹ�����
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: 

****************************************************************/
LASERCOMTASK_MAG *LaserComTask_HandleGet(void)
{
    return &g_sLaserComTask_Mag;
}

/****************************************************************
@FUNCTION����ȡ������ն˿����������
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: 

****************************************************************/
COMTASK_MAG *LaserComTask_TaskHandleGet(void)
{
    return &g_sLaserComTask_Mag.m_sTaskMag;
}

/****************************************************************
@FUNCTION��������մ��ڹ��ܳ�ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
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

    //�ӿ��ϵ�
    LaserCom_PowerOn();

    //��ȡ����̽ͷ��
    LaserCom_Prot_GetOnlineNum(pMag, 0x0A, 0x0A);
}


/****************************************************************
@FUNCTION��������մ��ڹ��ܷ���ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sLaserComTask_Mag

****************************************************************/
void LaserComTask_DeInit(void)
{
    LASERCOMTASK_MAG *pMag = LaserComTask_HandleGet();

    ComTask_DeInit(&pMag->m_sTaskMag);

    memset((void *)pMag, 0, sizeof(LASERCOMTASK_MAG));
}




//������մ���Э�鴦��
void LaserCom_Analysis(uint8_t *pBuf, uint16_t BufLen, void *pHandle)
{
    LASERCOMTASK_MAG *pMag = (LASERCOMTASK_MAG *)pHandle;
    uint8_t Offset;
 
    switch(pBuf[2])
    {
				case 0x10:
						LaserProtocalDeal(pMag, pBuf, BufLen, FALSE);		//֧������̽ͷ��С̽ͷ
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
@FUNCTION��������մ��ڹ�������������
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
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

    //��ʱ����
    if((NowTime >  pComTaskMag->m_dwLastScheduleTime) &&
            (NowTime -  pComTaskMag->m_dwLastScheduleTime >= pComTaskMag->m_dwScheduleTime) )
    {
        pComTaskMag->m_dwLastScheduleTime = NowTime;

        //���ȱ�֤�ײ����
        if(FALSE == LaserCom_LowLevelTimedProcess())
        {
            //��鷢�Ͷ���
            pFrame = LinkQueue_Header(pComTaskMag->m_pSendQueue);

            if(pFrame != NULL)
            {
								pMag->m_pLLMag->m_sHwOps.write(pFrame->m_byBuf, pFrame->m_wBufLen);
                LinkQueue_Retrieve(pComTaskMag->m_pSendQueue);
            }

            /***********Ӧ���߼�***************************/
            // 1. ���50s�˿�û�յ����ݣ����ȡ����̽ͷ��
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

    //���ڽ��մ���
    if(TRUE == pMag->m_pLLMag->m_sHwOps.read(&pComTaskMag->m_pProtBuf[pComTaskMag->m_wProtBufLen], &Len))
    {
        #ifdef SEND_TO_UPCOM
        //ת�����ݵ����п�
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
@FUNCTION��������մ��ڹ��ܻ�ȡ��ǰ����̽ͷ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ����̽ͷ��
@AUTHOR��xfw
@SPECIAL: g_sZigComTask_Mag

****************************************************************/
uint8_t LaserComTask_GetActualNum(void)
{
    LASERCOMTASK_MAG *pMag = &g_sLaserComTask_Mag;

    return pMag->m_byActualNum;
}
#endif

