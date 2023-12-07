/******************************************************************
*��Ȩ���� (C)2023, ���ջ������Ƽ����޹�˾
*
*�ļ�����:  lasercom.h
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
#ifndef  _LASERCOM_H
#define  _LASERCOM_H
/******************************************************************
*                             ͷ�ļ�                              *
******************************************************************/
#include "config.h"

/******************************************************************
*                           �ṹ�嶨��                          *
******************************************************************/

//Ӳ������
typedef enum
{
    LASER_MODE_NORMAL = 0,
    LASER_MODE_TEST = 1,	//����ģʽ�£�̽ͷ�յ������Ϣ��Ƴ���

} LASER_MODE;

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

} LASER_HW_OPERATIONS;

typedef struct
{

    uint8_t		m_byIsPowerEn: 1;		//TRUE-��Դʹ��
    uint8_t		m_byIsCommunicationSucc: 1;
    uint8_t 	m_byRes: 6;

    //uint32_t	m_dwNoCommunicationTimeout;	//��ͨ�ų�ʱ

    //uint8_t 	m_byPowerOnNowCnt;	//��ǰ������ϵ�ʱ��

    //ʵ�����Ӳ���������ɶ�д
    COM_HW_OPERATIONS m_sHwOps;

} LASERCOM_LOWLEVEL_MAG;


LASERCOM_LOWLEVEL_MAG *LaserCom_GetLLMagHandle(void);

/****************************************************************
@FUNCTION��̽ͷ�ײ��ʼ��
@INPUT��Ӳ���ӿں����ṹ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �󶨵ײ�Ӳ��������������ʼ�������͵�Դʹ������
****************************************************************/
extern void LaserCom_LowLevelInit(COM_HW_OPERATIONS a_sHwOps);


/****************************************************************
@FUNCTION��LASER�ӿ��ϵ�
@INPUT��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �ȴ�һ��ʱ������
****************************************************************/
void LaserCom_PowerOn(void);



/****************************************************************
@FUNCTION��LASER�ӿڵ���
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void LaserCom_PowerOff(void);



/****************************************************************
@FUNCTION��LASER�ӿڵײ㶨ʱ���Ƚ���
@INPUT��
@OUTPUT����
@RETURN: TRUE-����ִ�еײ㶨ʱ����,  FALSE-�޵ײ㶨ʱ����, ��ִ���ϲ�Ӧ�ö�ʱ����
@AUTHOR��xfw
@SPECIAL: ʵ��ģ���ϵ�
****************************************************************/
extern uint32_t LaserCom_LowLevelTimedProcess(void);


#endif


