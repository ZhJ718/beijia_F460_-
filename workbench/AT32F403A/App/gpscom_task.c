/********************************************************************************
* @File name: gpscom_laser.c
* @Author: xfw
* @Version: 1.0
* @Date: 2023-3-29
* @Description:
********************************************************************************/

#include "gpscom_task.h"

/*************底层实现************************************************/


//初始化
void GpsCom_ComInit(void)
{
    UART_CONFIG Config = {0};
    UART_MANAGER *pUartMag = &g_sUartSetMag.Com[GPS_COM];

    memcpy((void *)&Config, (void *)&g_scUartDefConfig[GPS_COM], sizeof(UART_CONFIG));
    Config.m_dwBaudRate = GPS_BAUDRATE;
    //对应串口配置
    Uart_ComConfiguration(Config, pUartMag);
    Uart_ComInit(pUartMag);

    Gps_GpioResetInit();
}

//写数据
uint32_t GpsCom_ComWrite(uint8_t *pBuf, uint16_t Len)
{
    #ifdef GPS_COM_LOG_OUTPUT

    LOG("GPSCOM(%d)->", Len);
    printf_with_data(pBuf, Len);

    #endif

    return Uart_SendBuf(&g_sUartSetMag.Com[GPS_COM], pBuf, Len);
}

//读数据
uint32_t GpsCom_ComRead(uint8_t *pBuf, uint16_t *pLen)
{
    UART_MANAGER *pUartMag = &g_sUartSetMag.Com[GPS_COM];

    /**/
    if(TRUE == pUartMag->m_bIsComRecv)
    {
        if(pUartMag->m_wRecvLen == 0)
        {
            //pUartMag->m_bIsComRecv = FALSE;
            return FALSE;
        }

        #ifdef GPS_COM_LOG_OUTPUT

        LOG("GPSCOM(%d)<-", pUartMag->m_wRecvLen);
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



uint32_t Gps_GetFen100000(uint8_t byJD_Du, uint8_t byJD_Fen, uint32_t dwJD_Fen_XiaoShu)
{

    uint32_t Off = 0;

    Off = byJD_Du * 60;
    Off += byJD_Fen;
    Off  *= 100000;
    Off += dwJD_Fen_XiaoShu;

    return Off;

}



/******************************************************************
*函数名称:NMEA_GNRMC_Analysis
*功能描述:分析GNRMC信息
*输入参数:gpsx:nmea信息结构体，buf:接收到的GPS数据缓冲区首地址
*输出参数:
*返回值:
*其它说明:
*修改日期    版本号   修改人    修改内容
*---------------------------------------------------
*                              查飞

GPRMC

推荐最小数据量的GPS信息（RecommendedMinimumSpecificGPS/TRANSITData）

样例：

$GPRMC,161229.487,A,3723.2475,N,12158.3416,W,0.13,309.62,120598,,,A*10

格式：

$GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>*<13>

解析：

$--RMC消息ID，$GPRMC，RMC协议头。

<1>定位点的UTC时间，161229.487，格式：hhmmss.sss

<2>定位状态，A，A=定位，V=导航

<3>纬度，3723.2475，格式：ddmm.mmmm

<4>纬度方向，N，N=北纬；S=南纬

<5>经度，12158.3416，格式：dddmm.mmmm

<6>经度方向，W，W=西经；E=东经

<7>对地航速，0.13，Knots，范围：000.0--999.9，1Knot（节）=1852米/时

<8>对地航向，309.62，度，范围：000.0--359.9，以真北为参考基准。

<9>定位点的UTC日期，120598，格式：ddmmyy（日月年）

<10>磁偏角，（000.0--180.0度，前面的0也将被传输）

<11>磁偏角方向，E（东）或W（西）

<12>模式指示，A（仅NMEA-01833.0版本以上输出，A=自主定位，D=差分，E=估算，N=无效数据）

<13>校验和，*10

回车换行将消息结束。

******************************************************************/
void NMEA_GNRMC_Analysis(NMEA_MSG *pMsg, uint8_t *buf, uint16_t len)
{
    uint32_t temp;
    uint8_t StrTemp[15][14] = {0};
    uint16_t i, j, StrIdx = 0;
    uint8_t StrLen[15];

    if(0 == strncmp((char *)buf, "$GNRMC", 6))
    {
        //按照","分割字符串，字符串长度存入StrLen[]中
        for(i = 0, j = 0; i < len; i++)
        {
            if(buf[i] == ',')
            {
                StrLen[StrIdx++] = j;
                j = 0;
            }
            else
            {
                StrTemp[StrIdx][j++] = buf[i];
            }
        }

        StrLen[StrIdx++] = j;

        //for(i=0;i<StrIdx;i++)
        //	printf("StrTemp[%d]=%s,Len=%d\n\r",i,StrTemp[i],StrLen[i]);
        /*
        	StrTemp[0]=$GNRMC,Len=6
        	StrTemp[1]=103809.000,Len=10
        	StrTemp[2]=A,Len=1
        	StrTemp[3]=3158.6270,Len=9
        	StrTemp[4]=N,Len=1
        	StrTemp[5]=11845.7012,Len=10
        	StrTemp[6]=E,Len=1
        	StrTemp[7]=0.00,Len=4
        	StrTemp[8]=0.00,Len=4
        	StrTemp[9]=150519,Len=6
        	StrTemp[10]=,Len=0
        	StrTemp[11]=,Len=0
        	StrTemp[12]=A*7B
        	,Len=6
        */

        StrIdx = 1;

        if(StrLen[StrIdx] >= 10)
        {
            sscanf((char *)StrTemp[StrIdx], "%d", &temp);
            pMsg->m_sUtcTime.hour = temp / 10000 + 8;
            pMsg->m_sUtcTime.min = temp % 10000 / 100;
            pMsg->m_sUtcTime.sec = temp % 100;
        }

        StrIdx = 3;

        if(StrLen[StrIdx] >= 9)
        {
            pMsg->m_byWD_Du = (StrTemp[StrIdx][0] - '0') * 10 + (StrTemp[StrIdx][1] - '0');
            pMsg->m_byWD_Fen = (StrTemp[StrIdx][2] - '0') * 10 + (StrTemp[StrIdx][3] - '0');
            pMsg->m_dwWD_Fen_XiaoShu = (StrTemp[StrIdx][5] - '0') * 1000;
            pMsg->m_dwWD_Fen_XiaoShu += (StrTemp[StrIdx][6] - '0') * 100;
            pMsg->m_dwWD_Fen_XiaoShu += (StrTemp[StrIdx][7] - '0') * 10;
            pMsg->m_dwWD_Fen_XiaoShu += (StrTemp[StrIdx][8] - '0');

            pMsg->m_dwWD_Fen_XiaoShu *= 10;

            if(StrLen[StrIdx] >= 10)
            {
                pMsg->m_dwWD_Fen_XiaoShu += (StrTemp[StrIdx][9] - '0');
            }
        }

        StrIdx = 4;

        if(StrLen[StrIdx] >= 1)
        {
            pMsg->m_byNorS = StrTemp[StrIdx][0];
        }

        StrIdx = 5;

        if(StrLen[StrIdx] >= 10)
        {

            pMsg->m_byJD_Du = (StrTemp[StrIdx][0] - '0') * 100;
            pMsg->m_byJD_Du += (StrTemp[StrIdx][1] - '0') * 10;
            pMsg->m_byJD_Du += (StrTemp[StrIdx][2] - '0');

            pMsg->m_byJD_Fen = (StrTemp[StrIdx][3] - '0') * 10 + (StrTemp[StrIdx][4] - '0');

            pMsg->m_dwJD_Fen_XiaoShu = (StrTemp[StrIdx][6] - '0') * 1000;
            pMsg->m_dwJD_Fen_XiaoShu += (StrTemp[StrIdx][7] - '0') * 100;
            pMsg->m_dwJD_Fen_XiaoShu += (StrTemp[StrIdx][8] - '0') * 10;
            pMsg->m_dwJD_Fen_XiaoShu += (StrTemp[StrIdx][9] - '0');

            pMsg->m_dwJD_Fen_XiaoShu *= 10;

            if(StrLen[StrIdx] >= 11)
            {
                pMsg->m_dwJD_Fen_XiaoShu += (StrTemp[StrIdx][10] - '0');
            }
        }

        StrIdx = 6;

        if(StrLen[StrIdx] >= 1)
        {
            pMsg->m_byEorW = StrTemp[StrIdx][0];
        }

        StrIdx = 12;

        if(StrLen[StrIdx] > 0)
        {
            pMsg->RTKMode = StrTemp[StrIdx][0] == 'R' ? TRUE : FALSE;	//进入RTK模式
        }

        StrIdx = 9;

        if(StrLen[StrIdx] >= 6)
        {
            sscanf((char *)StrTemp[StrIdx], "%d", &temp);

            pMsg->m_sUtcTime.date = temp / 10000;
            pMsg->m_sUtcTime.month = temp % 10000 / 100;
            pMsg->m_sUtcTime.year = temp % 100;

        }

//		LOG("%04d/%02d/%02d %02d:%02d:%02d\n\r",\
//		2000+pMsg->m_sUtcTime.year,pMsg->m_sUtcTime.month,pMsg->m_sUtcTime.date,\
//		pMsg->m_sUtcTime.hour,pMsg->m_sUtcTime.min,pMsg->m_sUtcTime.sec);
//
//		LOG("WD:%d %d.%d %C,JD:%d %d.%d %C\n\r",\
//		pMsg->m_byWD_Du,pMsg->m_byWD_Fen,pMsg->m_dwWD_Fen_XiaoShu,pMsg->m_byNorS,\
//		pMsg->m_byJD_Du,pMsg->m_byJD_Fen,pMsg->m_dwJD_Fen_XiaoShu,pMsg->m_byEorW);
    }
}

/******************************************************************
*函数名称:NMEA_GNGGA_Analysis
*功能描述:分析GNRMC信息
*输入参数:gpsx:nmea信息结构体，buf:接收到的GPS数据缓冲区首地址
*输出参数:
*返回值:
*其它说明:
*修改日期    版本号   修改人    修改内容
*
$GNGGA,072808.000,3204.20386,N,11853.69611,E,1,08,1.4,42.7,M,0.0,M,,*46
*
******************************************************************/
void NMEA_GNGGA_Analysis(NMEA_MSG *pMsg, uint8_t *buf, uint16_t len)
{
    //uint32_t temp;
    uint8_t StrTemp[15][12] = {0};
    uint8_t i, j, StrIdx = 0;
    uint8_t StrLen[15];

    if(0 == strncmp((char *)buf, "$GNGGA", 6))
    {
        //按照","分割字符串，字符串长度存入StrLen[]中
        for(i = 0, j = 0; i < len; i++)
        {
            if(buf[i] == ',')
            {
                StrLen[StrIdx++] = j;
                j = 0;
            }
            else
            {
                StrTemp[StrIdx][j++] = buf[i];
            }
        }

        StrLen[StrIdx++] = j;

        /*for(i=0;i<StrIdx;i++)
        	LOG("StrTemp[%d]=%s,Len=%d\n\r",i,StrTemp[i],StrLen[i]);

        	StrTemp[0]=$GNGGA,Len=6
        	StrTemp[1]=082214.000,Len=10
        	StrTemp[2]=3204.20061,Len=10
        	StrTemp[3]=N,Len=1
        	StrTemp[4]=11853.69447,Len=11
        	StrTemp[5]=E,Len=1
        	StrTemp[6]=1,Len=1
        	StrTemp[7]=14,Len=2
        	StrTemp[8]=0.8,Len=3
        	StrTemp[9]=52.1,Len=4
        	StrTemp[10]=M,Len=1
        	StrTemp[11]=0.0,Len=3
        	StrTemp[12]=M,Len=3
        	StrTemp[13]=
        	StrTemp[14]=*42,Len=3
        */

        StrIdx = 7;

        if(StrLen[StrIdx] == 0)
        {
            pMsg->m_byNumSv = 0;
        }
        else if(StrLen[StrIdx] == 1)
        {
            pMsg->m_byNumSv = StrTemp[StrIdx][0] - '0';
        }
        else if(StrLen[StrIdx] == 2)
        {
            pMsg->m_byNumSv = (StrTemp[StrIdx][0] - '0') * 10;
            pMsg->m_byNumSv += (StrTemp[StrIdx][1] - '0');
        }

    }
}

GPSCOMTASK_MAG g_sGpsComTask_Mag;

/****************************************************************
@FUNCTION：GPS串口功能初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sGpsComTask_Mag

****************************************************************/
void GpsComTask_Init(void)
{
    GPSCOMTASK_MAG *pMag = &g_sGpsComTask_Mag;
    NMEA_OUTPUT_FREQ_PARAM SetParam;

    memset((void *)pMag, 0, sizeof(GPSCOMTASK_MAG));

    COM_HW_OPERATIONS HwOps =
    {
        .hw_init = GpsCom_ComInit,
        .read = GpsCom_ComRead,
        .write = GpsCom_ComWrite,
        .gpio_set = Gps_GpioResetOff,
        .gpio_reset = Gps_GpioResetOn,
    };

    Gps_LowLevelInit(HwOps);

    pMag->m_pLLMag = Gps_GetLLMagHandle();

    pMag->m_bIsInit = ComTask_Init(&pMag->m_sTaskMag, GPS_COM_FRAME_LEN_MAX, 100);

    //复位模块
    Gps_Reset(5);

    //设置模块参数,默认输出GGA、RMC数据
    SetParam.m_eNGGA = OUTPUT_1;
    SetParam.m_eNGLL = OUTPUT_NO;
    SetParam.m_eNGSA = OUTPUT_NO;
    SetParam.m_eNGSV = OUTPUT_NO;
    SetParam.m_eNRMC = OUTPUT_1;
    SetParam.m_eNVTG = OUTPUT_NO;
    SetParam.m_eNZDA = OUTPUT_NO;
    SetParam.m_eNTXT = OUTPUT_NO;
    Gps_StartSetParam(SetParam);


}


/****************************************************************
@FUNCTION：GPS串口功能反初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sGpsComTask_Mag

****************************************************************/
void GpsComTask_DeInit(void)
{
    GPSCOMTASK_MAG *pMag = &g_sGpsComTask_Mag;

    ComTask_DeInit(&pMag->m_sTaskMag);

    memset(pMag, 0, sizeof(GPSCOMTASK_MAG));
}


/****************************************************************
@FUNCTION：Gps串口功能任务主进程
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sGpsComTask_Mag

****************************************************************/
void GpsComTask_Process(void)
{
    GPSCOMTASK_MAG *pMag = &g_sGpsComTask_Mag;
    COMTASK_MAG *pComTaskMag = &pMag->m_sTaskMag;
    uint16_t i, Len = pComTaskMag->m_wProtBufMaxLen - pComTaskMag->m_wProtBufLen;
    uint32_t IsCutted = FALSE;//, IsLong55AA = FALSE;
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

        //优先保证底层操作
        if(FALSE == Gps_LowLevelTimedProcess())
        {
            //检查发送队列
            pFrame = LinkQueue_Header(pComTaskMag->m_pSendQueue);

            if(pFrame != NULL)
            {
                pMag->m_pLLMag->m_sHwOps.write(pFrame->m_byBuf, pFrame->m_wBufLen);
                LinkQueue_Retrieve(pComTaskMag->m_pSendQueue);
                sdram_free(0, (void * )pFrame);
            }

        }

    }

    //串口接收处理
    if(TRUE == pMag->m_pLLMag->m_sHwOps.read(&pComTaskMag->m_pProtBuf[pComTaskMag->m_wProtBufLen], &Len))
    {
        //转发数据到上行口
        ResComTask_RecvSendToUpcom(&pComTaskMag->m_pProtBuf[pComTaskMag->m_wProtBufLen], Len, 0x0B);

        pComTaskMag->m_wProtBufLen += Len;

        for(i = 0; i < pComTaskMag->m_wProtBufLen;)
        {
            Len = pComTaskMag->m_wProtBufLen - i;

            if(TRUE == Protocal_FindNMEA0183String(&pComTaskMag->m_pProtBuf[i], &Len, &IsCutted))
            {
                pComTaskMag->m_dwLastCommTime = NowTime;

                if(TRUE == IsCutted)
                {
                    memcpy(pComTaskMag->m_pProtBuf, &pComTaskMag->m_pProtBuf[i], pComTaskMag->m_wProtBufLen - i);
                    pComTaskMag->m_wProtBufLen -= i ;
                    break;
                }
                else
                {

                    NMEA_GNRMC_Analysis(&pMag->m_sMsg, &pComTaskMag->m_pProtBuf[i], Len);
                    NMEA_GNGGA_Analysis(&pMag->m_sMsg, &pComTaskMag->m_pProtBuf[i], Len);

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


