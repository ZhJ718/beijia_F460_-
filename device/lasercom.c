/******************************************************************
*版权所有 (C)2023, 江苏华如防务科技有限公司
*
*文件名称:  lasercom.c
*文件标识:
*内容摘要:
*其它说明:
*当前版本:
*作    者:
*完成日期:  2023/4/4
*
*修改记录1:
*    修改日期:
*    版 本 号:
*    修 改 人:
*    修改内容:
******************************************************************/

/******************************************************************
*                             头文件                              *
******************************************************************/

#include "lasercom.h"
/******************************************************************
*                           全局变量声明                          *
******************************************************************/

static LASERCOM_LOWLEVEL_MAG LaserComLLMag;


LASERCOM_LOWLEVEL_MAG *LaserCom_GetLLMagHandle(void)
{
    return &LaserComLLMag;
}


/****************************************************************
@FUNCTION：探头底层初始化
@INPUT：硬件接口函数结构体
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 绑定底层硬件操作函数，初始化，拉低电源使能引脚
****************************************************************/
extern void LaserCom_LowLevelInit(COM_HW_OPERATIONS a_sHwOps)
{
    LASERCOM_LOWLEVEL_MAG *pMag = LaserCom_GetLLMagHandle();

    assert_param(pMag != NULL);

    memset(pMag, 0, sizeof(LASERCOM_LOWLEVEL_MAG));

    pMag->m_sHwOps.hw_init = a_sHwOps.hw_init;
    pMag->m_sHwOps.gpio_power_on = a_sHwOps.gpio_power_on;
    pMag->m_sHwOps.gpio_power_off = a_sHwOps.gpio_power_off;

    pMag->m_sHwOps.read = a_sHwOps.read;
    pMag->m_sHwOps.write = a_sHwOps.write;

    //IO初始化
    if(pMag->m_sHwOps.hw_init != NULL)
    {
        pMag->m_sHwOps.hw_init();
    }

    //电源使能脚拉低
    if(pMag->m_sHwOps.gpio_power_off != NULL)
    {
        pMag->m_sHwOps.gpio_power_off();
    }

}


//模块底层操作供电
void LaserCom_LL_PowerOn(LASERCOM_LOWLEVEL_MAG *pMag)
{
    assert_param(pMag != NULL);

    if(pMag->m_sHwOps.gpio_power_on != NULL)
    {
        pMag->m_sHwOps.gpio_power_on();
    }
}

//模块底层操作断电
void LaserCom_LL_PowerOff(LASERCOM_LOWLEVEL_MAG *pMag)
{
    assert_param(pMag != NULL);

    if(pMag->m_sHwOps.gpio_power_off != NULL)
    {
        pMag->m_sHwOps.gpio_power_off();
    }
}


/****************************************************************
@FUNCTION：LASER接口上电
@INPUT：
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 等待一定时间后控制
****************************************************************/
void LaserCom_PowerOn(void)
{
    LASERCOM_LOWLEVEL_MAG *pMag = LaserCom_GetLLMagHandle();

    assert_param(pMag != NULL);

    pMag->m_byIsPowerEn = TRUE;
    LaserCom_LL_PowerOn(pMag);
}

/****************************************************************
@FUNCTION：LASER接口掉电
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void LaserCom_PowerOff(void)
{
    LASERCOM_LOWLEVEL_MAG *pMag = LaserCom_GetLLMagHandle();

    assert_param(pMag != NULL);

    pMag->m_byIsPowerEn = FALSE;
    LaserCom_LL_PowerOff(pMag);
}


/****************************************************************
@FUNCTION：LASER接口底层定时调度进程
@INPUT：
@OUTPUT：无
@RETURN: TRUE-正在执行底层定时调度,  FALSE-无底层定时调度, 可执行上层应用定时调度
@AUTHOR：xfw
@SPECIAL: 实现模块上电
****************************************************************/
extern uint32_t LaserCom_LowLevelTimedProcess(void)
{
    LASERCOM_LOWLEVEL_MAG *pMag = LaserCom_GetLLMagHandle();

    assert_param(pMag != NULL);



    return FALSE;
}



