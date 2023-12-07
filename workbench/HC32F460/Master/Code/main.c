
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
#include "rescom_task.h"
#include "lasercom_task.h"
#include "hmicom_task.h"
#include "battery_task.h"
#include "ir38com_task.h"
#include "mpu6050.h"



void CoreInit(void)
{
    #if defined(AT32F4xx)
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    #elif defined(HC32F46x)
    //��ʼ���ж�����ѡ��Ĵ�����0-127��
    //һ��Ҫ�ӣ���ֹboot��app�ж�ӳ���һ�µ���Ī����λ
    InitNVICSELRegister0_127();
    SysClkIni();
    #endif

    sdram_init(0);

    Delay_Init();
    IFlash_Init(&g_sIFlashMag);
    ExtFlash_Init(&g_sExtFlashMag);

    IDParam_Init();
    SysParam_Init();

    Update_MagInit();
    Update_SetDevTType(IDParam_GetTType());
    Update_SetDevTSubType(IDParam_GetTSubType());
    Update_SetDevRID(IDParam_GetRid());
    Update_SetDevPID(IDParam_GetPid());

    SoftTimerInit();
}


void BoardInit(void)
{
    PowerOnOffInit();
    
    //���е������ʼ��
    RunLed_Init(&g_sRunledMag);
    //״̬�������ʼ��
    StaLed_Init(&g_sStaledMag);
    //�����������ʼ��
    Beep_Init(&g_sBeepMag);
    //��������ʼ��
    Motor_Init(&g_sMotorMag);


    Wtn6_1_2_Init();
    
    #ifdef OPEN_MPU6050
    MPU_Init();
    Mpu6050_Init(m_byg_sMpu6050Mag());
    #endif

}

uint8_t BufTmp[] = {0x10, 0x012, 0x013};



/****************************************************************
@FUNCTION����ʱ����
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��
@SPECIAL:ÿ1ms��ʱ����
****************************************************************/
extern void System_TimedTask(void)
{

    static uint8_t i;

    if(g_sSysScheTime.IsSysTimeUpdate_250US == TRUE)
    {
        Beep_TimedProcess(&g_sBeepMag);
        g_sSysScheTime.IsSysTimeUpdate_250US = FALSE;
    }

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
        Wtn6_TimedProcess();
        AppPowerOnOffTimedProcess(&g_sPowerOffMag);
        SlaverMag_TimedProcess();
    }

    if(g_sSysScheTime.IsSysTimeUpdate_1S == TRUE)
    {
        g_sSysScheTime.IsSysTimeUpdate_1S = FALSE;
				
        AppPowerOffHandle(&g_sPowerOffMag);
			
				#ifdef OPEN_MPU6050
					Mpu6050_TimedProcess(m_byg_sMpu6050Mag());
        #endif	
				
				IllegalEventHandle();
			
				EnableChangeBatHandle();
			
				Mano_Start_Judge();
				DayOrNight_Judge();
				Train_Mode_Revive();
			
				UnUpLoadEvtNumCalcu();
				EventNoRspRestore();
				
				DrillStateParamHandle();

				#ifdef SEND_TO_UPCOM     
						if(GetSysRunSecond() % 5 == 0)
						{
								HardCtrl_LocationNoti_Process();
								HardCtrl_HelmetNoti_Process();
						}
				#endif   
    }
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

    CoreInit();

    BoardInit();

    AppPowerOnOffInit(&g_sPowerOffMag);

    SysScheduleTimerInit();

		ResComTask_Init();

    UserParam_Init();

    BatTask_Init();
    ZigComTask_Init();
    LaserComTask_Init();
    HmiComTask_Init();
    IR38ComTask_Init();

    #ifdef DEBUG
    DebugComInit();

    #endif

    Led_SetRunMode(LED_MODE_BLINK_1, &g_sRunledMag);
    #ifdef SEND_TO_UPCOM
    HardCtrl_PowerOn_Process();
    #endif
		
    SlaverMag_ListInit();
    
    RGB(LaserComTask_HandleGet());

		IWDG_Init();
			
    while(1)
    {
        IWDG_Feed();

        System_TimedTask();

        ZigComTask_Process();
        LaserComTask_Process();
        IR38ComTask_Process();
        HmiComTask_Process();
        ResComTask_Process();
				
        BatTask_Process();
    }
}



/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
