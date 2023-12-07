
/******************************************************************************/
/** \file main.c
 **
 ** A detailed description is available at
 ** @link Sample Group Some description @endlink
 **
 **   - 2016-02-16  1.0  XYZ First version for Device Driver Library of Module.
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/


#include "zigcom_task.h"
#include "gpscom_task.h"
#include "rescom_task.h"
#include "lasercom_task.h"
#include "battery_task.h"
#include "ir38com_task.h"

#if 0

void SramTest(void)
{
    uint8_t *p = sdram_malloc(0, 100);

    if(p != NULL)
    {
        LOG("mem %d%\n\r", my_mem_perused(0));
        sdram_free(0, p);

    }
}






#define IFLASH_TEST_DATA_CNT 2100

void IFlashTest(void)
{

    uint8_t *pBuf = sdram_malloc(0, IFLASH_TEST_DATA_CNT);
    uint16_t i;



    //LOG("start IFLASH Test\n\r");
    SetRand();

    for(i = 0; i < IFLASH_TEST_DATA_CNT; i++)
    {
        pBuf[i] = 2;//rand()%0xff;
    }

    #if 1
    IFlash_Write(&g_sIFlashMag, FLASH_UPDATE_ADDR, pBuf, IFLASH_TEST_DATA_CNT);
    #endif

    sdram_free(0, pBuf);
    //LOG("IFLASH Test end\n\r");
}

#endif

uint8_t Buf[] = {0xA2, 0xFF, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/****************************************************************
@FUNCTION：定时任务
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：
@SPECIAL:每1ms定时调用
****************************************************************/
extern void System_TimedTask(void)
{

    if(g_sSysScheTime.IsSysTimeUpdate_1MS == TRUE)
    {
        g_sSysScheTime.IsSysTimeUpdate_1MS = FALSE;
        Led_TimedProcess(&g_sRunledMag);
        Led_TimedProcess(&g_sStaledMag);
        Beep_TimedProcess(&g_sBeepMag);
        Ir_LowLevelTimedProcess();
    }

    if(g_sSysScheTime.IsSysTimeUpdate_10MS == TRUE)
    {
        g_sSysScheTime.IsSysTimeUpdate_10MS = FALSE;

    }

    if(g_sSysScheTime.IsSysTimeUpdate_100MS == TRUE)
    {
        g_sSysScheTime.IsSysTimeUpdate_100MS = FALSE;
        SoftTimerIntervalProcess();
        Motor_TimedProcess(&g_sMotorMag);
        Fume_TimedProcess(&g_sFumeMag);
        Wtn6_TimedProcess();
        AppPowerOnOffTimedProcess(&g_sPowerOffMag);

    }

    if(g_sSysScheTime.IsSysTimeUpdate_1S == TRUE)
    {
        g_sSysScheTime.IsSysTimeUpdate_1S = FALSE;

        AppPowerOffHandle(&g_sPowerOffMag);

        //Beep_On(&g_sBeepMag, OPT_MODE_NONBLOCK, 400);
        //Wtn6_Play(g_sSysScheTime.TimSysTimeCountS);

        //ComTask_RetransmissionData(&g_sIR38ComTask_Mag.m_sTaskMag,Buf,sizeof(Buf));
    }


}

void CoreInit(void)
{
    #if defined(AT32F4xx)
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
    #elif defined(HC32F46x)
    SysClkIni();
    #endif

    sdram_init(0);

    Delay_Init();
    IFlash_Init(&g_sIFlashMag);
    ExtFlash_Init(&g_sExtFlashMag);
    Update_MagInit();
    SoftTimerInit();
}


void BoardInit(void)
{
    //运行灯任务初始化
    RunLed_Init(&g_sRunledMag);
    //状态灯任务初始化
    StaLed_Init(&g_sStaledMag);
    //蜂鸣器任务初始化
    Beep_Init(&g_sBeepMag);
    //马达任务初始化
    Motor_Init(&g_sMotorMag);
    //发烟帽任务初始化
    Fume_Init(&g_sFumeMag);

    Wtn6_1_2_Init();


}

/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ** This sample
 **
 ******************************************************************************/

int32_t main(void)
{
    uint32_t i;

    CoreInit();

    #ifdef DEBUG
    DebugComInit();

    #endif

    #ifdef DEBUG
    Uart_SendBuf(&g_sUartSetMag.Com[DEBUG_COM], "I'm In App...\n\r", 16);
    #endif

    BoardInit();

    AppPowerOnOffInit(&g_sPowerOffMag);

    Wtn6_Play(WTN6_SystemStart);

    BatTask_Init();
    ResComTask_Init();

    ZigComTask_Init();
    LaserComTask_Init();
    IR38ComTask_Init();

    SysScheduleTimerInit();

    //IWDG_Init();
    Led_SetRunMode(LED_MODE_BLINK_1, &g_sRunledMag);

    while(1)
    {
        IWDG_Feed();

        System_TimedTask();

        ResComTask_Process();
        ZigComTask_Process();
        LaserComTask_Process();
        BatTask_Process();
        IR38ComTask_Process();
    }
}



/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


