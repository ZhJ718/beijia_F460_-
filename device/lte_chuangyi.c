/******************************************************************
*��Ȩ���� (C)2020, ���ջ������Ƽ����޹�˾
*
*�ļ�����:  lte_chuangyi.c
*�ļ���ʶ:
*����ժҪ:
*����˵��:
*��ǰ�汾:
*��    ��:
*�������:  2020/4/20
*
*�޸ļ�¼1:
*    �޸�����:
*    �� �� ��:
*    �� �� ��:
*    �޸�����:
******************************************************************/

/******************************************************************
*                             ͷ�ļ�                              *
******************************************************************/

#include "lte_chuangyi.h"
/******************************************************************
*                           ȫ�ֱ�������                          *
******************************************************************/

/****************************************************************
@FUNCTION������4Gģ��ײ��ʼ��
@INPUT��Ӳ���ӿں����ṹ�壬4G�ײ����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �󶨵ײ�Ӳ��������������ʼ�������͵�Դʹ������
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

    //IO��ʼ��
    if(pMag->m_sHwOps.hw_init != NULL)
    {
        pMag->m_sHwOps.hw_init();
    }

    //��Դʹ�ܽ�����
    if(pMag->m_sHwOps.gpio_power_off != NULL)
    {
        pMag->m_sHwOps.gpio_power_off();
    }

}

/****************************************************************
@FUNCTION������4Gģ��ӿ��ϵ�
@INPUT��Lte_ChuangYi�ײ����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
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
@FUNCTION������4Gģ��ӿ��µ�
@INPUT��Lte_ChuangYi�ײ����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
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




