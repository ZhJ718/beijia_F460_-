/******************************************************************
*��Ȩ���� (C)2018, ��������Ƽ��Ͼ��ֹ�˾
*
*�ļ�����:  wtn6.h
*�ļ���ʶ:
*����ժҪ:
*����˵��:
*��ǰ�汾:
*��    ��:
*�������:  2018/7/11
*
*�޸ļ�¼1:
*    �޸�����:
*    �� �� ��:
*    �� �� ��:
*    �޸�����:
******************************************************************/
#ifndef  _WTN6_H
#define  _WTN6_H
/******************************************************************
*                             ͷ�ļ�                              *
******************************************************************/

#include "config.h"


/******************************************************************
*                           �궨��                                *
******************************************************************/

#define ENABLE_WTN6_1   //����Ĭ��
//#define ENABLE_WTN6_2 //ͷ��Ĭ��

/******************************************************************
*                           �ṹ�嶨��                          *
******************************************************************/

typedef enum
{
    WTN6_Reserve = 0,		//����
    WTN6_SystemStart,		//ϵͳ����
    WTN6_SystemStop,		//ϵͳ�ػ�
    WTN6_ClothConnected,		//����������
    WTN6_WeaponConnected,		//����������
    WTN6_SQConnected,		//��ǹ������
    WTN6_BQConnected,		//��ǹ������
    WTN6_JQConnected,		//��ǹ������
    WTN6_JJQConnected,		//�ѻ�ǹ������
    WTN6_XDQConnected,		//����ǹ������
    WTN6_LDFSQConnected = 10,		//�񵯷�����������
    WTN6_DBHJTConnected,		//�������Ͳ������
    WTN6_ClothDisconnected,		//�������ӶϿ�
    WTN6_WeaponDisconnected,		//�������ӶϿ�
    WTN6_SQDisconnected,		//��ǹ���ӶϿ�
    WTN6_BQDisconnected,		//��ǹ���ӶϿ�
    WTN6_JQDisconnected,		//��ǹ���ӶϿ�
    WTN6_JJQDisconnected,		//�ѻ�ǹ���ӶϿ�
    WTN6_XDQDisconnected,		//����ǹ���ӶϿ�
    WTN6_LDFSQDisconnected,		//�񵯷��������ӶϿ�
    WTN6_DBHJTDisconnected = 20,		//�������Ͳ���ӶϿ�

    WTN6_Shot_Head,		//ͷ���е�
    WTN6_Shot_LeftArm,		//����е�
    WTN6_Shot_RightArm,		//�ұ��е�
    WTN6_Shot_LeftLeg,		//�����е�
    WTN6_Shot_RightLeg,		//�����е�
    WTN6_Shot_LeftChest,		//�����е�
    WTN6_Shot_RightChest,		//�����е�
    WTN6_Shot_Back,		//���е�
    WTN6_Shot_Abdomen,		//�����е�

    WTN6_GrenadeExplosion = 30,		//���ױ�ը
    WTN6_ShellHit,		//�����ڵ�����
    WTN6_AirAttack,		//���л���Ϯ��

    WTN6_OutRange,		//������������
    WTN6_WeaponUnlock,		//��������

    WTN6_ExhaustAmmunition,		//�����þ�
    WTN6_ReplenishAmmunition,		//�����ɹ�
    WTN6_Revive,		//���и���
    WTN6_JudgeDie,		//��������
    WTN6_Injury,		//������
    WTN6_Killed = 40,		//������

    WTN6_YX_Shoot1,		//��ǹ���1
    WTN6_YX_Shoot2,		//��ǹ���2
    WTN6_YX_MineExplosion,		//���ױ�ը

    WTN6_YX_JQ,		//��ǹ
    WTN6_YX_JJ1,		//�ѻ�1
    WTN6_YX_JJ2,		//�ѻ�2
    WTN6_YX_PDFG,		//�ڵ��ɹ�
    WTN6_YX_PDBZ,		//�ڵ��ɹ���ر�ը
    WTN6_YX_SL,		//���ױ�ը
    WTN6_YX_ZDBZ = 50,		//ը����ը
    WTN6_YX_ZDBZ2,		//ը����ը2
    WTN6_YX_ZDBZ3,		//ը����ը3
    WTN6_YX_ZJQ,		//�ػ�ǹ
    WTN6_YX_ZDHX1,		//�ӵ���Х1
    WTN6_YX_ZDHX2,		//�ӵ���Х2

		WTN6_LowBattery_Helmet,			//ͷ���������㡣
		WTN6_Helmet_Already_Connect,	//ͷ�������ӡ�
		WTN6_Helmet_Disconnect,			//ͷ�����ӶϿ���
		WTN6_CHANGE_CLIP,				//�������С�	
		WTN6_CLIP_CHANGE_SUCCESS = 60,	//���и����ɹ���TODO
		
    //WTN6_LowBattery_Master,		//ͷ����������
    //WTN6_ReplaceBat_10Min_Master,		//ͷ��ʮ�����ڸ������
    //WTN6_ReplaceBat_5Min_Master,		//ͷ��������ڸ������
    //WTN6_ReplaceBat_3Min_Master,		//ͷ���������ڸ������
    //WTN6_LowPowerStop_Master,		//ͷ�����������Զ��ػ�

    WTN6_Connect_SuperBle,		//���ӳ��������豸

    WTN6_YX_MineExplosion_Ex,		//���ױ�ը
    WTN6_Mine_Clearance,		//���׳ɹ�
    WTN6_Aiding,		//���ھȻ���
    WTN6_Aid_Finish,		//���ھȻ�����

    WTN6_Slight_Injury,			//������Ҫ�Ȼ�
    WTN6_Moderate_Injury,		//�ж�����Ҫ�Ȼ�
    WTN6_Severe_Injury,		//�ض�����Ҫ�Ȼ�
    WTN6_Injury_Worse,		//�������
    WTN6_Number_1 = 70,		//����1
    WTN6_Number_2,		//����2
    WTN6_Number_3,		//����3
    WTN6_Death_Cheat,		//��������
    //WTN6_Death_Timeout_Start,		//��ʱ��������
		WTN6_HELMET_DISCONNECT,			//ͷ�����ӶϿ�
    WTN6_Death_Illegal_Stop,		//�Ƿ��ػ�����
    WTN6_GPSTrouble,		//��λģ�����

    WTN6_Cloth_IRTrouble,		//����̽ͷ����
    WTN6_Master_IRTrouble,		//ͷ��̽ͷ����
    WTN6_ClothTrouble,		//�������ع���
    WTN6_MasterTrouble = 80,		//ͷ�����ع���
    //WTN6_WeaponTrouble,		//��������
		WNT6_MINE_REMOVE_FAIL,          //����ʧ�ܡ�
    WTN6_Mode_Train,		//ѵ��ģʽ
    WTN6_Mode_Mano,		//��ϰģʽ

    WTN6_LowBattery_IR,		//�������������
		
		WTN6_MINE_SET_SUCCESS,		//���׳ɹ�.
		WTN6_MINE_SET_FAIL,		//����ʧ��.
		WTN6_SAVE_SUCCESS,		//�Ȼ��ɹ�.
		WTN6_SAVE_FALI,		//�Ȼ�ʧ��.
		
		WTN6_LowBattery_Cloth,		//���׵�������.
    WTN6_ReplaceBat_10Min_Cloth,		//����ʮ�����ڸ������
    WTN6_ReplaceBat_5Min_Cloth,		//����������ڸ������
    WTN6_ReplaceBat_3Min_Cloth,		//�����������ڸ������
    WTN6_LowPowerStop_Cloth,		//���׵��������Զ��ػ�
		
		WTN6_START_SAVE = 90,		//��ʼ�Ȼ�.
		WTN6_STOP_SAVE,		//�����Ȼ�.
		WTN6_DAY_MODEL,		//����ģʽ.
		WTN6_NIGHT_MODE,		//ҹ��ģʽ.
		WTN6_CHEAT_EVENT,		//�����¼�.
		WTN6_HITCH,		//���ϡ�
		WTN6_MANO_PARAM_CFG_SUCCESS,		//��ϰ�������óɹ���
		WTN6_TERMINAL_ONLINE,		//�ն�������
		WTN6_TERMINAL_OFFLINE,		//�ն����ߡ�
		WTN6_LOCATION_SUCCESS,		//��λ�ɹ���
		WTN6_EXPLOSIVE_DEVICE_NOT_INSTALL=100,		//��Ʒδ��װ��
		WTN6_ILLEGAL_MOVE,		//�Ƿ��ƶ���
		WTN6_ILLEGAL_TURNOFF,		//�Ƿ��ػ���
		WTN6_TEST_MODEL,		//����ģʽ��
		WTN6_PAIR_SUCCESS,		//��Գɹ���
		WTN6_REMOVE_PAIR,		//�����ԡ�
		WTN6_ATUO_POWEROFF,		//�Զ��ػ���
		WTN6_PLS_CHANGE_BATTERY,		//�������ء�
		WTN6_DEVICE_CONNECT,		//�豸�����ӡ�
		WTN6_DEVICE_DISCONNECT,		//�豸���ӶϿ���
		WTN6_NULL = 110,		//�����ա���
		WTN6_HELMET_NOT_POWERON,		//ͷ��δ������
		WTN6_BLE_DEVICE_HITCH,		//����ģ����ϡ�
	
    //WTN6_ReplaceBat_10Min_IR,		//�����ʮ�����ڸ������
    //WTN6_ReplaceBat_5Min_IR,		//�����������ڸ������
    //WTN6_ReplaceBat_3Min_IR,		//������������ڸ������
    //WTN6_LowPowerStop_IR,		//��������������Զ��ػ�

} ENUM_WTN6_CONTENT;


/******************************************************************
*                           ȫ�ֱ�������                          *
******************************************************************/


//������Ҫ���������Ļ��λ�����
#define VOICE_CNT_MAX	100

typedef struct
{
    u8 m_aBuf[VOICE_CNT_MAX];
    u8 m_byPushPos;
    u8 m_byPopPos;
} VOICE_RING;

typedef struct
{
    VOICE_RING m_sLVoiceRing;
    VOICE_RING m_sRVoiceRing;
} VOICE_MAG;


extern VOICE_MAG g_sVoiceMag;





/******************************************************************
*                         ȫ�ֺ�������                            *
******************************************************************/

//����оƬӲ������
typedef struct
{
    //Ӳ���ӿڳ�ʼ������
    void (*hw_init)(void);
    //���������߲���
    void (*data_high)(void);
    //���������Ͳ���
    void (*data_low)(void);
    //æ���
    uint32_t (*is_busy)(void);
} WTN6_HW_OPERATIONS;


//����оƬ�߼�����
typedef struct
{
    //us���ߺ���
    void (*sleep_us)(uint16_t);
    //����оƬʱ��������첽
    void (*step_ops)(void *);

} WTN6_SOFT_OPERATIONS;

typedef struct
{
    volatile u8 m_IsHigh: 1;	//������״̬ 0-�͵�ƽ 1-�ߵ�ƽ
    volatile u8 m_IsRes: 7;

    volatile u8 m_byRes;		//����
    volatile u16 m_wDelayUs;//��ʱʱ��
} WTN6_STEP_MAG;

#define WTN6_MAG_STEP_MAX 20 //���ֲ�

typedef struct
{
    WTN6_HW_OPERATIONS m_sHwOps;
    WTN6_SOFT_OPERATIONS m_sSoftOps;

    volatile u8 m_byNowStep; 	//��ǰ���е��Ĳ�
    volatile u8 m_byMaxStep;		//�ܹ����ٲ�
    volatile u8 m_byRes2;
    volatile u8 m_byRes3;	//�����ֽڣ�������

    WTN6_STEP_MAG m_sSingle[WTN6_MAG_STEP_MAX];
} WTN6_MAG;

#if defined(ENABLE_WTN6_1)
    extern WTN6_MAG g_sWTN6RightMag;
#endif

#if defined(ENABLE_WTN6_2)
    extern WTN6_MAG g_sWTN6LeftMag;
#endif

/****************************************************************
@FUNCTION�����������������ģ���ʼ��
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��ʼ���ṹ��g_sWTN6LeftMag��g_sWTN6RightMag�����Ӳ�������������첽�����߼�����
****************************************************************/
void Wtn6_1_2_Init(void);

/****************************************************************
@FUNCTION������оƬ��ʱ���񣬸��ݲ��Ŷ��������ݣ���������
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ǹ�����ܻ��ϵ�ǰ���ţ����������ȴ��ϴ�����������ϲŻᲥ��
****************************************************************/
void  Wtn6_TimedProcess(void);


/****************************************************************
@FUNCTION����������������
@INPUT���������
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �˺������ú��������ܲ�����������
****************************************************************/
void Wtn6_RPlay(u8 Voice);

/****************************************************************
@FUNCTION����������������
@INPUT���������
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �˺������ú��������ܲ�����������
****************************************************************/
void Wtn6_LPlay(u8 Voice);



/****************************************************************
@FUNCTION�����ݴ�����������ֵ��������������벥�Ŷ���
@INPUT���������
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �˺������ú��������ܲ�����������
****************************************************************/
void Wtn6_Play(u8 Voice);



/******************************************************************
*��������:play_online
*��������:����װ����������
*�������:
*�������:
*����ֵ:
*����˵��:
*�޸�����    �汾��   �޸���    �޸�����
*---------------------------------------------------
*                              ���
******************************************************************/
void play_online(u8 equipemtid);


/******************************************************************
*��������:play_offline
*��������:����װ����������
*�������:
*�������:
*����ֵ:
*����˵��:
*�޸�����    �汾��   �޸���    �޸�����
*---------------------------------------------------
*                              ���
******************************************************************/
void play_offline(u8 equipemtid);


/******************************************************************
*��������:play_Status
*��������:���������������ǰ״̬
*�������:��ǰ״̬ �μ� ENUM_UNITE_STATE //1-���� 2-�������� 3-��̨���� 4-��̨���� 5-�������� 6-���и��� 7-�������� 8-ȱ������
*�������:
*����ֵ:
*����˵��:
*�޸�����    �汾��   �޸���    �޸�����
*---------------------------------------------------
*                              ���
******************************************************************/
void play_Status(u8 Sta);


/******************************************************************
*��������:play_StruckPart
*��������:�����е���λ
*�������:
*�������:
*����ֵ:
*����˵��:
*�޸�����    �汾��   �޸���    �޸�����
*---------------------------------------------------
*                              ���
******************************************************************/
void play_StruckPart(ENUM_PARTITION part);



//���������
void play_Shot(u8 weaponid, u8 Sta, u8 Num);



extern void Wtn6_StartLSound(uint8_t Dat);


extern void Wtn6_StartRSound(uint8_t Dat);

extern void Wtn6_Sound(uint8_t Dat);

//ҹ������
extern void Wtn6_NightSound(void);

//��������
extern void Wtn6_DaySound(void);

#endif

