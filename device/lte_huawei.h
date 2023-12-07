/******************************************************************
*��Ȩ���� (C)2020, ���ջ������Ƽ����޹�˾
*
*�ļ�����:  lte_huawei.h
*�ļ���ʶ:
*����ժҪ:
*����˵��:
*��ǰ�汾:
*��    ��:
*�������:  2020/2/24
*
*�޸ļ�¼1:
*    �޸�����:
*    �� �� ��:
*    �� �� ��:
*    �޸�����:
******************************************************************/
#ifndef  _LTE_HUAWEI_H
#define  _LTE_HUAWEI_H
/******************************************************************
*                             ͷ�ļ�                              *
******************************************************************/
#include "config.h"

/******************************************************************
*                           �ṹ�嶨��                          *
******************************************************************/

//Ӳ������
typedef struct
{
    //Ӳ����ʼ������������IO�ڳ�ʼ�������ڵ�
    void (*hw_init)(void);
    //Ӳ����λ��������
    void (*gpio_power_on)(void);
    //Ӳ����λ��������
    void (*gpio_power_off)(void);
    //������
    uint32_t (*read)(uint8_t *, uint16_t *);
    //д����
    uint32_t (*write)(uint8_t *, uint16_t );

} LTE_HUAWEI_HW_OPERATIONS;

typedef struct
{

    uint8_t		m_byIsPowerEn: 1;		//TRUE-��Դʹ��
    uint8_t 	m_byRes: 7;


    //ʵ�����Ӳ���������ɶ�д
    LTE_HUAWEI_HW_OPERATIONS m_sHwOps;

} LTE_HUAWEI_LOWLEVEL_MAG;


/****************************************************************
@FUNCTION����Ϊ4Gģ��ײ��ʼ��
@INPUT��Ӳ���ӿں����ṹ�壬4G�ײ����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �󶨵ײ�Ӳ��������������ʼ�������͵�Դʹ������
****************************************************************/
extern void Lte_HuaWei_LowLevelInit(LTE_HUAWEI_HW_OPERATIONS a_sHwOps, LTE_HUAWEI_LOWLEVEL_MAG *pMag);


/****************************************************************
@FUNCTION����Ϊ4Gģ��ӿ��ϵ�
@INPUT��Lte_HuaWei�ײ����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Lte_HuaWei_PowerOn(LTE_HUAWEI_LOWLEVEL_MAG *pMag);

/****************************************************************
@FUNCTION����Ϊ4Gģ��ӿ��µ�
@INPUT��LTE�ײ����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Lte_HuaWei_PowerOff(LTE_HUAWEI_LOWLEVEL_MAG *pMag);


#endif


