/******************************************************************
*版权所有 (C)2020, 江苏华如防务科技有限公司
*
*文件名称:  lte_chuangyi.c
*文件标识:
*内容摘要:
*其它说明:
*当前版本:
*作    者:
*完成日期:  2020/4/20
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

#include "lte_chuangyi.h"
/******************************************************************
*                           全局变量声明                          *
******************************************************************/

/****************************************************************
@FUNCTION：创毅4G模块底层初始化
@INPUT：硬件接口函数结构体，4G底层管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 绑定底层硬件操作函数，初始化，拉低电源使能引脚
****************************************************************/
extern void Lte_ChuangYi_LowLevelInit(LTE_CHUANGYI_HW_OPERATIONS a_sHwOps, LTE_CHUANGYI_LOWLEVEL_MAG *pMag)
{
    assert_param(pMag != NULL);

    memset(pMag, 0, sizeof(LTE_CHUANGYI_LOWLEVEL_MAG));

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

/****************************************************************
@FUNCTION：创毅4G模块接口上电
@INPUT：Lte_ChuangYi底层管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Lte_ChuangYi_PowerOn(LTE_CHUANGYI_LOWLEVEL_MAG *pMag)
{
    if(pMag->m_sHwOps.gpio_power_on != NULL)
    {
        pMag->m_sHwOps.gpio_power_on();
        pMag->m_byIsPowerEn = TRUE;
    }
}

/****************************************************************
@FUNCTION：创毅4G模块接口下电
@INPUT：Lte_ChuangYi底层管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Lte_ChuangYi_PowerOff(LTE_CHUANGYI_LOWLEVEL_MAG *pMag)
{
    if(pMag->m_sHwOps.gpio_power_off != NULL)
    {
        pMag->m_sHwOps.gpio_power_off();
        pMag->m_byIsPowerEn = FALSE;
    }
}




