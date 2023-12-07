/******************************************************************
*��Ȩ���� (C)2023, ���ջ������Ƽ����޹�˾
*
*�ļ�����:  lasercom.c
*�ļ���ʶ:
*����ժҪ:
*����˵��:
*��ǰ�汾:
*��    ��:
*�������:  2023/4/4
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

#include "lasercom.h"
/******************************************************************
*                           ȫ�ֱ�������                          *
******************************************************************/

static LASERCOM_LOWLEVEL_MAG LaserComLLMag;


LASERCOM_LOWLEVEL_MAG *LaserCom_GetLLMagHandle(void)
{
    return &LaserComLLMag;
}


/****************************************************************
@FUNCTION��̽ͷ�ײ��ʼ��
@INPUT��Ӳ���ӿں����ṹ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �󶨵ײ�Ӳ��������������ʼ�������͵�Դʹ������
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


//ģ��ײ��������
void LaserCom_LL_PowerOn(LASERCOM_LOWLEVEL_MAG *pMag)
{
    assert_param(pMag != NULL);

    if(pMag->m_sHwOps.gpio_power_on != NULL)
    {
        pMag->m_sHwOps.gpio_power_on();
    }
}

//ģ��ײ�����ϵ�
void LaserCom_LL_PowerOff(LASERCOM_LOWLEVEL_MAG *pMag)
{
    assert_param(pMag != NULL);

    if(pMag->m_sHwOps.gpio_power_off != NULL)
    {
        pMag->m_sHwOps.gpio_power_off();
    }
}


/****************************************************************
@FUNCTION��LASER�ӿ��ϵ�
@INPUT��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �ȴ�һ��ʱ������
****************************************************************/
void LaserCom_PowerOn(void)
{
    LASERCOM_LOWLEVEL_MAG *pMag = LaserCom_GetLLMagHandle();

    assert_param(pMag != NULL);

    pMag->m_byIsPowerEn = TRUE;
    LaserCom_LL_PowerOn(pMag);
}

/****************************************************************
@FUNCTION��LASER�ӿڵ���
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
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
@FUNCTION��LASER�ӿڵײ㶨ʱ���Ƚ���
@INPUT��
@OUTPUT����
@RETURN: TRUE-����ִ�еײ㶨ʱ����,  FALSE-�޵ײ㶨ʱ����, ��ִ���ϲ�Ӧ�ö�ʱ����
@AUTHOR��xfw
@SPECIAL: ʵ��ģ���ϵ�
****************************************************************/
extern uint32_t LaserCom_LowLevelTimedProcess(void)
{
    LASERCOM_LOWLEVEL_MAG *pMag = LaserCom_GetLLMagHandle();

    assert_param(pMag != NULL);



    return FALSE;
}



