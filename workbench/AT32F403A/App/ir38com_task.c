/********************************************************************************
* @File name: ir38com_task.c
* @Author: xfw
* @Version: 1.0
* @Date: 2023-4-11
* @Description:
********************************************************************************/

#include "ir38com_task.h"
#include "ir.h"
#include "zigcom_task.h"
#include "gpscom_task.h"
#include "battery_task.h"
#include "lasercom_task.h"
#include "rescom_task.h"


/*************�ײ�ʵ��************************************************/
//�˿ڳ�ʼ��
void IR38Com_ComInit(void)
{
    User_Ir_Init();
}

//�˿�д����
uint32_t IR38Com_Write(uint8_t *pBuf, uint16_t Len)
{
    #ifdef IR38_COM_LOG_OUTPUT

    LOG("IR_38K(%d)->", Len);
    printf_with_data(pBuf, Len);

    #endif

    return User_Ir_Send(pBuf, Len);
}


//�˿ڶ�����
uint32_t IR38Com_Read(uint8_t *pBuf, uint16_t *pLen)
{
    uint8_t ret, Tmp[Ir_Max_Rcv];
    uint16_t TmpLen = Ir_Max_Rcv;

    ret = User_Ir_Rcv(Tmp, &TmpLen);

    if(ret == 0)
    {
        #ifdef IR38_COM_LOG_OUTPUT

        LOG("IR_38K(%d)<-", TmpLen);
        printf_with_data(Tmp, TmpLen);

        #endif

        if(*pLen >= TmpLen)
        {
            memcpy((void *)pBuf, (void *)Tmp, TmpLen);
            *pLen = TmpLen;
        }
        else
        {
            //����������,��������������
            memcpy((void *)pBuf, (void *)Tmp, *pLen);
        }

        return TRUE;
    }

    return FALSE;
}

/*************�ײ�ʵ�ֽ���************************************************/



IR38COMTASK_MAG g_sIR38ComTask_Mag;

/****************************************************************
@FUNCTION������̾๦�ܳ�ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sIR38ComTask_Mag

****************************************************************/
void IR38ComTask_Init(void)
{
    IR38COMTASK_MAG *pMag = &g_sIR38ComTask_Mag;

    memset((void *)pMag, 0, sizeof(IR38COMTASK_MAG));

    pMag->m_sLLMag.m_sHwOps.hw_init = IR38Com_ComInit;
    pMag->m_sLLMag.m_sHwOps.read = IR38Com_Read;
    pMag->m_sLLMag.m_sHwOps.write = IR38Com_Write;

    //��ʼ��
    pMag->m_sLLMag.m_sHwOps.hw_init();

    pMag->m_bIsInit = ComTask_Init(&pMag->m_sTaskMag, IR38_COM_FRAME_LEN_MAX, 100);
}


/****************************************************************
@FUNCTION������̾๦�ܷ���ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sIR38ComTask_Mag

****************************************************************/
void IR38ComTask_DeInit(void)
{
    IR38COMTASK_MAG *pMag = &g_sIR38ComTask_Mag;

    ComTask_DeInit(&pMag->m_sTaskMag);

    memset((void *)pMag, 0, sizeof(IR38COMTASK_MAG));
}

#if 0
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

#endif

void IR_ProtDeal_TestMode(void *pHandle)
{
    IR38COMTASK_MAG *pMag = (IR38COMTASK_MAG *)pHandle;
    FRAME_BUF *pFrame = NULL;
    uint8_t HardSta1 = 0;
    uint8_t HardSta2 = 0;
    uint8_t HardSta3 = 0;
    uint8_t HardSta4 = 0;

    pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

    if(pFrame == NULL)
    {
        return;
    }

    //����̾�ظ�
    HardSta1 |= (ComTask_IsComOK(&g_sGpsComTask_Mag) == FALSE ? 0x1 << IDX_HARD_GPS : 0);
    HardSta1 |= (ComTask_IsComOK(&g_sLaserComTask_Mag) == FALSE ? 0x1 << IDX_HARD_LASER : 0);
    HardSta1 |= (ComTask_IsComOK(&g_sZigComTask_Mag) == FALSE ? 0x1 << IDX_HARD_ZIGBEE : 0);
    HardSta1 |=  0x1 << IDX_HARD_BLE;

    pFrame->m_wBufLen = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xA3;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xFF;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = FIXVERSION;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = BOARDID;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = HardSta1;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = HardSta2;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = HardSta3;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = HardSta4;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = g_sLaserComTask_Mag.m_byActualNum;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = BatTask_GetExBatRes();
    pFrame->m_byBuf[pFrame->m_wBufLen++] = g_sGpsComTask_Mag.m_sMsg.m_byNumSv;

    memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen], g_sZigComTask_Mag.m_pLLMag->m_sParam.m_aSelfExtAddr, 8);
    pFrame->m_wBufLen += 8;

    pFrame->m_byBuf[2] = pFrame->m_wBufLen - 3;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = Xor_Calc(&pFrame->m_byBuf[3], pFrame->m_wBufLen - 3);

    LinkQueue_Append(pMag->m_sTaskMag.m_pSendQueue, (void*)pFrame);


    #if 1
    //����ģʽ
    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 1);				//�����
    Beep_On(&g_sBeepMag, OPT_MODE_NONBLOCK, 400);				//��������
    Led_SetRunMode(LED_MODE_BLINK_1, &g_sRunledMag);			//����
    //Led_SetRunMode(LED_MODE_BLINK_2, &g_sStaledMag);			//����
    LaserCom_Prot_CtrlMode(&g_sLaserComTask_Mag, LASER_MODE_TEST);		//̽ͷ���������ģʽ
    Fume_On(&g_sFumeMag, 3);									//����ñ����v

    Wtn6_Play(WTN6_YX_ZDHX1);
    #endif
}



//�˿�Э�鴦��
void IR38Com_Analysis(uint8_t *pBuf, uint16_t BufLen, void *pHandle)
{
    IR38COMTASK_MAG *pMag = (IR38COMTASK_MAG *)pHandle;
    uint8_t TmpLen;
    uint16_t IR_Cmd;


    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 1);
    IR_Cmd = ((pBuf[0] << 8) | pBuf[1]);

    switch(IR_Cmd)
    {

        case 0xA267:	//�����Ϣ��ȡ
            break;

        case 0xA269:	//��ϰ��������
            break;

        case 0xA271:	//��ҩ������Ϣ��ȡ
            break;

        case 0xA274:	//��ҩ���ʲ���
            break;

        case 0xA201:	//��ȡ����MAC��ַ
            //Respon_Mac();
            break;

        case 0xA2A2:	//����RID_PID
            break;

        case 0xA2A4:	//�������豸
            break;

        case 0xA2A5:	//������豸
            break;

        case 0xA251:	//��Ա�����ȡ
            break;

        case 0xA252:	//��Ա���ν������
            break;

        case 0xA2FF:	//����Ӳ��״̬
            IR_ProtDeal_TestMode(pHandle);
            break;

        default:
            break;
    }


}



/****************************************************************
@FUNCTION������̾๦������������
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sIR38ComTask_Mag

****************************************************************/
void IR38ComTask_Process(void)
{
    IR38COMTASK_MAG *pMag = &g_sIR38ComTask_Mag;
    COMTASK_MAG *pComTaskMag = &pMag->m_sTaskMag;
    uint16_t i, Len = pComTaskMag->m_wProtBufMaxLen - pComTaskMag->m_wProtBufLen;
    uint32_t NowTime = GetSysRunMilliSecond();
    FRAME_BUF *pFrame = NULL;

    if(FALSE == pMag->m_bIsInit)
    {
        return;
    }

    //��ʱ����
    if((NowTime >  pComTaskMag->m_dwLastScheduleTime) &&
            (NowTime -  pComTaskMag->m_dwLastScheduleTime >= pComTaskMag->m_dwScheduleTime) )
    {
        pComTaskMag->m_dwLastScheduleTime = NowTime;

        //����̾಻�������ݽ��ճ�ʱ����
        //Ir_Rcv_Timeout();

        if(FALSE == IsIR38SendBusy())
        {
            //��鷢�Ͷ���
            pFrame = LinkQueue_Header(pComTaskMag->m_pSendQueue);

            if(pFrame != NULL)
            {
                pMag->m_sLLMag.m_sHwOps.write(pFrame->m_byBuf, pFrame->m_wBufLen);
                LinkQueue_Retrieve(pComTaskMag->m_pSendQueue);
                sdram_free(0, (void * )pFrame);
            }
        }

    }

    //���ڽ��մ���
    if(TRUE == pMag->m_sLLMag.m_sHwOps.read(&pComTaskMag->m_pProtBuf[pComTaskMag->m_wProtBufLen], &Len))
    {
        pMag->m_sLLMag.m_sHwOps.hw_init();
        //ת�����ݵ����п�
        ResComTask_RecvSendToUpcom(&pComTaskMag->m_pProtBuf[pComTaskMag->m_wProtBufLen], Len, 0x08);

        pComTaskMag->m_wProtBufLen += Len;

        for(i = 0; i < pComTaskMag->m_wProtBufLen;)
        {
            Len = pComTaskMag->m_wProtBufLen - i;

            if(TRUE == Protocal_FindIR38_Frame(&pComTaskMag->m_pProtBuf[i], &Len))
            {
                pComTaskMag->m_dwLastCommTime = NowTime;


                IR38Com_Analysis(&pComTaskMag->m_pProtBuf[i], Len, (void *)pMag);

                i = Len + i;

            }
            else
            {
                i++;
            }

            if(i >= pComTaskMag->m_wProtBufLen)
            {
                pComTaskMag->m_wProtBufLen = 0;
            }
        }


    }

}



