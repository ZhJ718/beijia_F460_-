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


/*************底层实现************************************************/
//端口初始化
void IR38Com_ComInit(void)
{
    User_Ir_Init();
}

//端口写数据
uint32_t IR38Com_Write(uint8_t *pBuf, uint16_t Len)
{
    #ifdef IR38_COM_LOG_OUTPUT

    LOG("IR_38K(%d)->", Len);
    printf_with_data(pBuf, Len);

    #endif

    return User_Ir_Send(pBuf, Len);
}


//端口读数据
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
            //缓存区不够,不拷贝多余数据
            memcpy((void *)pBuf, (void *)Tmp, *pLen);
        }

        return TRUE;
    }

    return FALSE;
}

/*************底层实现结束************************************************/



IR38COMTASK_MAG g_sIR38ComTask_Mag;

/****************************************************************
@FUNCTION：红外短距功能初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sIR38ComTask_Mag

****************************************************************/
void IR38ComTask_Init(void)
{
    IR38COMTASK_MAG *pMag = &g_sIR38ComTask_Mag;

    memset((void *)pMag, 0, sizeof(IR38COMTASK_MAG));

    pMag->m_sLLMag.m_sHwOps.hw_init = IR38Com_ComInit;
    pMag->m_sLLMag.m_sHwOps.read = IR38Com_Read;
    pMag->m_sLLMag.m_sHwOps.write = IR38Com_Write;

    //初始化
    pMag->m_sLLMag.m_sHwOps.hw_init();

    pMag->m_bIsInit = ComTask_Init(&pMag->m_sTaskMag, IR38_COM_FRAME_LEN_MAX, 100);
}


/****************************************************************
@FUNCTION：红外短距功能反初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
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

    //红外短距回复
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
    //测试模式
    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 1);				//马达震动
    Beep_On(&g_sBeepMag, OPT_MODE_NONBLOCK, 400);				//蜂鸣器响
    Led_SetRunMode(LED_MODE_BLINK_1, &g_sRunledMag);			//灯亮
    //Led_SetRunMode(LED_MODE_BLINK_2, &g_sStaledMag);			//灯亮
    LaserCom_Prot_CtrlMode(&g_sLaserComTask_Mag, LASER_MODE_TEST);		//探头带进入测试模式
    Fume_On(&g_sFumeMag, 3);									//发烟帽点亮v

    Wtn6_Play(WTN6_YX_ZDHX1);
    #endif
}



//端口协议处理
void IR38Com_Analysis(uint8_t *pBuf, uint16_t BufLen, void *pHandle)
{
    IR38COMTASK_MAG *pMag = (IR38COMTASK_MAG *)pHandle;
    uint8_t TmpLen;
    uint16_t IR_Cmd;


    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 1);
    IR_Cmd = ((pBuf[0] << 8) | pBuf[1]);

    switch(IR_Cmd)
    {

        case 0xA267:	//身份信息读取
            break;

        case 0xA269:	//演习参数设置
            break;

        case 0xA271:	//弹药物资信息读取
            break;

        case 0xA274:	//弹药物资补充
            break;

        case 0xA201:	//获取本机MAC地址
            //Respon_Mac();
            break;

        case 0xA2A2:	//保存RID_PID
            break;

        case 0xA2A4:	//配置子设备
            break;

        case 0xA2A5:	//清除子设备
            break;

        case 0xA251:	//人员伤情读取
            break;

        case 0xA252:	//人员救治结果反馈
            break;

        case 0xA2FF:	//测试硬件状态
            IR_ProtDeal_TestMode(pHandle);
            break;

        default:
            break;
    }


}



/****************************************************************
@FUNCTION：红外短距功能任务主进程
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
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

    //定时任务
    if((NowTime >  pComTaskMag->m_dwLastScheduleTime) &&
            (NowTime -  pComTaskMag->m_dwLastScheduleTime >= pComTaskMag->m_dwScheduleTime) )
    {
        pComTaskMag->m_dwLastScheduleTime = NowTime;

        //红外短距不定长数据接收超时处理
        //Ir_Rcv_Timeout();

        if(FALSE == IsIR38SendBusy())
        {
            //检查发送队列
            pFrame = LinkQueue_Header(pComTaskMag->m_pSendQueue);

            if(pFrame != NULL)
            {
                pMag->m_sLLMag.m_sHwOps.write(pFrame->m_byBuf, pFrame->m_wBufLen);
                LinkQueue_Retrieve(pComTaskMag->m_pSendQueue);
                sdram_free(0, (void * )pFrame);
            }
        }

    }

    //串口接收处理
    if(TRUE == pMag->m_sLLMag.m_sHwOps.read(&pComTaskMag->m_pProtBuf[pComTaskMag->m_wProtBufLen], &Len))
    {
        pMag->m_sLLMag.m_sHwOps.hw_init();
        //转发数据到上行口
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



