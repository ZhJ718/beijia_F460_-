
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

#include "upcom_task.h"
#include "zigcom_task.h"


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

void BootPowerOffTimedProcess(void)
{
    static uint32_t KyOnCnt = 0;

    if(Is_PwrKeyOn())
    {

        if(KyOnCnt++ >= 25)
        {
            //关机时间到，马达震动提示
            Motor_GpioOn();

            do
            {
                Delay_Ms(100);
                IWDG_Feed();
            }
            while(Is_PwrKeyOn());

            //等待按键释放
            PowerOffHandle();
        }
    }
    else
    {
        KyOnCnt = 0;
    }
}



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
    }

    if(g_sSysScheTime.IsSysTimeUpdate_10MS == TRUE)
    {
        g_sSysScheTime.IsSysTimeUpdate_10MS = FALSE;

    }

    if(g_sSysScheTime.IsSysTimeUpdate_100MS == TRUE)
    {
        g_sSysScheTime.IsSysTimeUpdate_100MS = FALSE;
        SoftTimerIntervalProcess();
        RLed_Toggle();
        BootPowerOffTimedProcess();
    }

    if(g_sSysScheTime.IsSysTimeUpdate_1S == TRUE)
    {
        g_sSysScheTime.IsSysTimeUpdate_1S = FALSE;
        //Uart_SendBuf(&g_sUartSetMag.Com[COM2],"Test\n\r",6);
        //LOG("Mem:%d%\n\r",my_mem_perused(0));
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


    #if 0
    UPDATECOM_HW_OPERATIONS DownComHwOps =
    {
        .hw_init = DownCom_ComInit,
        .read = DownCom_ComRead,
        .write = DownCom_ComWrite
    };
    #endif
    PowerOnOffInit();

    RunLed_GpioInit();
    RunLed_Off();
    RLed_GpioInit();
    RLed_Off();

    Beep_GpioInit();
    Beep_GpioOff();

    Motor_GpioInit();
    Motor_GpioOff();

    //UpdateCom_Init(DownComHwOps, &g_sDownComMag);
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

    BoardInit();

    //延时1s，防误触
    Delay_Ms(1000);

    //确定是开机
    //RunLed_On();
    RLed_On();

    // 1.维持住主电源
    PowerOnHandle();

    #ifndef Debug
    Motor_GpioOn();  //  控制马达震动
    #endif

    // 3.延时1000ms
    Delay_Ms(1000);

    // 4.关闭震动提示
    Motor_GpioOff();

    // 5.检查是否有更新包
    if(TRUE == CheckNewData())
    {
        EraseNewData();
    }

    // 6.检查实际开机意图，进入主程序还是进入Boot

    #if 0
    //临时测试
    JumpToApp();

    while(1);

    #endif

    //power on without key
    #if 0

    // 1s内若
    for(i = 0; i < 100; i++)
    {
        Delay_Ms(10);    //延时

        if(UpdateCom_Find55AAFrame(&g_sUpComMag))
        {
            //DebugPrints("App start running\n\r");
            break;
        }

        if(UpdateCom_Find55AAFrame(&g_sDownComMag))
        {
            //DebugPrints("App start running\n\r");
            break;
        }
    }

    if(i >= 100)
    {
        JumpToApp();

        while(1);
    }

    #endif

// poweron by key
    #if 1

    //8s内若按键松开，跳入APP，否则进入Boot
    for(i = 0; i < 600; i++)
    {
        Delay_Ms(10);    //延时

        if(!Is_PwrKeyOn())
        {
            //DebugPrints("App start running\n\r");
            JumpToApp();

            while(1);
        }

    }

    Motor_GpioOn();

    //等待按键释放
    while(Is_PwrKeyOn())
    {
        IWDG_Feed();
        Delay_Ms(100);    //延时
    }

    Motor_GpioOff();
    #endif


BOOT:

    #ifdef DEBUG
    DebugComInit();

    #endif

    #ifdef DEBUG
    Uart_SendBuf(&g_sUartSetMag.Com[DEBUG_COM], "I'm In Boot...\n\r", 16);
    #endif


    //Update_SetDevTType(0);
    //Update_SetDevTSubType(0);
    //Update_SetDevRID(0);
    //Update_SetDevPID(0);

    UpComTask_Init();

    ZigComTask_Init();

    SysScheduleTimerInit();

    IWDG_Init();


    while(1)
    {
        IWDG_Feed();

        System_TimedTask();

        UpComTask_Process();

        ZigComTask_Process();

    }
}



/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


