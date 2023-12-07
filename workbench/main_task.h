#ifndef MAIN_TASK_H
#define MAIN_TASK_H

#include "config.h"
#include "malloc.h"
#include "math.h"
#include "timer.h"
#include "uart.h"
#include "iwdt.h"
#include "gpio.h"
#include "iflash.h"
#include "extflash.h"
#include "upgrade.h"
#include "protocal.h"
#include "update_com.h"
#include "param.h"
#include "comtask.h"
#include "mpu6050.h"

typedef enum
{
    CHAN_IR38 = 0,
    CHAN_ZIGBEE    
}ENUM_CONN_CHAN;




#ifdef DEBUG

    //ͷ������
    #if BOARDID == 0x01
        #define DEBUG_COM COM5
        //���װ���
    #elif BOARDID == 0x02
        #define DEBUG_COM COM2
    #endif

    #define LOG(fmt,...) Uart_DebugPrintf(&g_sUartSetMag.Com[DEBUG_COM],fmt, ##__VA_ARGS__)

    //���Կ�
    void DebugComInit(void);

    void printf_with_data(uint8_t *pBuf, uint16_t Len);

#else
    #define LOG(fmt,...)
#endif




//���ػ���ʼ��
void PowerOnOffInit(void);

void PowerOnHandle(void);

void PowerOffHandle(void);





#ifdef _APP

#define CheckRID(rid) (((rid)==(g_sIDParam.m_byRid)) ||((rid)==0xff))
#define CheckPID(pid) (((pid)==(g_sIDParam.m_wPid)) ||((pid)==0xffff))

#define LASER_PART_MAX PARTITION_MAX		//���˲�λ����ʱ���֧��10����λͬʱ����
#define LASER_PART_ID_MAX 10		//���˲�λ����ʱ���֧��10����λID
#define PI             3.1415926
#define EARTH_RADIUS   6378.137        //������ư뾶

#define MOVE_OFFSET_MAX 100		//�����ƶ���Χ

#if defined(HC32F46x)
    void InitNVICSELRegister0_127(void);
#endif

#define POWEROFFSTARTCOUNT 10  //������������3s����ʼ�ػ�
#define POWEROFFCOUNT 20  //������������5s����ʽ�ػ�

//	����ػ��¼�����ṹ��
__packed typedef struct
{
    uint16_t m_wTimer;	 //�ۼƹػ�����ʱ��
    uint8_t m_bPoweroff_Force: 1; // 1��ʾǿ�ƹػ�
    uint8_t m_bIsStartOff: 1;
    uint8_t m_bIsOff: 1;	// 1��ʾ�ػ���־
    uint8_t m_bPoweroffPre: 1;
		uint8_t m_bIsRes: 4;
} POWER_OFF_MAG;

extern POWER_OFF_MAG g_sPowerOffMag;

//���ػ���ʼ������ɿ����߼����ú����ᵼ��ϵͳ����1s
void AppPowerOnOffInit(POWER_OFF_MAG *pMag);


//���ػ���ʱ������ɹػ���� 100ms��ʱ����
void AppPowerOnOffTimedProcess(POWER_OFF_MAG *pMag);


void AppPowerOffHandle(POWER_OFF_MAG *pMag);


#include "led.h"

/****************************************************************
@FUNCTION�����еƳ�ʼ��
@INPUT��LED����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ���е�Ӳ����������ǰ����
			@@ RunLed_GpioInit
			@@ RunLed_On
			@@ RunLed_Off
****************************************************************/
extern void RunLed_Init(LED_MAG *pMag);

extern LED_MAG g_sRunledMag;
/****************************************************************
@FUNCTION��״ָ̬ʾ�Ƴ�ʼ��
@INPUT��LED����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ���е�Ӳ����������ǰ����
			@@ RLed_GpioInit
			@@ RLed_On
			@@ RLed_Off
��˸ָʾ��

****************************************************************/
extern void StaLed_Init(LED_MAG *pMag);

extern LED_MAG g_sStaledMag;


#include "beep.h"

extern BEEP_MAG g_sBeepMag;

/****************************************************************
@FUNCTION��������ģʽ����Դ���������ƹ��ܳ�ʼ��
@INPUT��BEEP����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern void Beep_Init(BEEP_MAG *pMag);


#include "motor.h"

extern MOTOR_MAG    g_sMotorMag;

/****************************************************************
@FUNCTION������ģʽ�������ƹ��ܳ�ʼ��
@INPUT��MOTOR����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern void Motor_Init(MOTOR_MAG *pMag);

#include "fume.h"

extern FUME_MAG    g_sFumeMag;

/****************************************************************
@FUNCTION������ģʽ�·��̹޿��ƹ��ܳ�ʼ��
@INPUT��FUME����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern void Fume_Init(FUME_MAG *pMag);


#include "wtn6.h"

typedef struct
{
    uint8_t m_byIsOnline: 1; //���߱��
    uint8_t m_byIsLock: 1; //�������
    uint8_t m_byIsSetTxPwr: 1; //�޸ķ��书��
		uint8_t m_byIsPowerOnFlg:1;
		uint8_t m_byIsSleepFlg:1;
    uint8_t m_byIsRes: 3; //����

    uint8_t m_byTType;		//���豸ʵ������
    uint8_t m_byTSubType;		//���豸ʵ�������ͣ�0xFF-�����ģ����������÷������ʱ��
    uint8_t m_byWType;		//���豸�������� (0x0 -���ߣ�0x01 - ������0x02 - �±�׼zigbee��0x03 - lora�豸)
    uint8_t m_aWMac[8];		//���豸MAC��ַ����λ��ǰ��12
    uint8_t m_byRid;
    uint16_t m_wPid;
    uint8_t m_byTxPwr;

		uint8_t m_byWeaponIdx;
		uint8_t m_byToukuiIdx;
    uint8_t m_byIdx;
	
		uint8_t m_byIsAmmuInit;
		
		uint8_t m_byShootingMode;
		uint8_t m_byShootingFlag;
	
    STRUCT_TS_Character1 m_sTsChar1;
    uint16_t m_wBatVol;		//��ص�ѹ����λ��ǰ������1000���� 16
		uint16_t AmmuLastRemainNum;
		uint16_t m_wPlayInterval;
    uint32_t m_dwLastCommTimeMs;    //�ϴ�ͨѶʱ��
    uint32_t m_dwLastGetStaTimeMs;  //�ϴη��ͻ�ȡ״̬ʱ��
} SINGLE_SLAVER_MAG;


/****************************************************************
@FUNCTION�����豸״̬��ʼ�б��ʼ��
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void SlaverMag_ListInit(void);

/****************************************************************
@FUNCTION�������豸״̬�б��ȡָ��MAC��ַ�����豸����ṹ��ָ��
@INPUT��MAC��ַ
@OUTPUT����
@RETURN: ���豸����ṹ��ָ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
SINGLE_SLAVER_MAG* SlaverMag_GetHandleByMac(uint8_t *pMacHex,uint8_t MacHexLen);

/****************************************************************
@FUNCTION�������豸״̬�б��ȡָ��ʵ�����͵����豸����ṹ��ָ��
@INPUT��ʵ������
@OUTPUT����
@RETURN: ���豸����ṹ��ָ��
@AUTHOR��xfw
@SPECIAL: �ҵ��ĵ�һ��,ע�������(0xFE) �ն�
****************************************************************/
SINGLE_SLAVER_MAG* SlaverMag_GetHandleByTType(uint8_t TType);

__packed typedef struct{
    uint8_t m_byWeapon_Type;		//����ʵ������
    uint8_t m_byWeapon_SubType;		//����ʵ��������
    uint8_t m_byCfgBulletKind;		//���õ�ҩ������

    uint8_t m_byState;		//����״̬

    uint8_t LockAck;		//������Ӧ
    uint8_t UnLockFlag;		//��������
    uint8_t LockBlood0;		//Ѫ��Ϊ0����
    BULLETINFO_T m_sBulletInfo[BULLETKIND_MAX];          //������ҩ��Ϣ(���6��)
}SOLDIERAMMU_T;

//ʱ��ṹ��
typedef struct
{
    //������������
    volatile uint16_t year;	//1970-2099
    volatile uint8_t month;	//1-12
    volatile uint8_t date;	//1-31
    volatile uint8_t hour; //0-23
    volatile uint8_t min;	//0-59
    volatile uint8_t sec;	//0-59
    volatile uint8_t week;
} TIMER_MAG;

typedef struct
{
    TIMER_MAG m_sTimer;		//����ʱ��
    uint8_t m_byDamageKind;		//����ENUM_DAMAGE_KIND
    uint8_t m_byPart;		//�˲�ENUM_PARTITION
    uint8_t m_byFRid;		//�����RID
    uint16_t m_wFPid;		//�����PID
    uint8_t m_byUniID;		//�����������ҩ����

		float m_fJD;
		float m_fWD;
} DEATH_MAG;

typedef struct
{
    uint32_t m_dwLastExplosionMs0xF186;
    uint32_t m_dwLastExplosionMs0xF191;
    uint16_t m_wRcvZigbeeBeaconTimeOutCnt;
	
    uint16_t m_wVehiclePID;
		uint16_t m_wTargetVehiclePID;
	
		uint8_t m_byTargetVehicleMac[8];
		uint8_t m_byVehicleTType;
		uint8_t m_byTargetVehicleTType;
	
		uint8_t m_byVehicleTSubtype;
		uint8_t m_byTargetVehicleTSubtype;

		uint8_t m_byVehicleRID;
		uint8_t m_byTargetVehicleRID;
	
	  uint8_t m_byIsGetOn;

    uint8_t m_byRecBeacon;
	
} STRUCT_BroadcastBeacon;

// �����ն��û�����,�������ֽڶ���
typedef struct
{
    uint16_t m_wSingleCnt;		//ϵͳ���豸��
    uint16_t m_wSlaverStaGetInt;		//���豸״̬��ȡʱ����
    SINGLE_SLAVER_MAG m_saSingleMag[SYS_SLAVER_CNT_MAX];		//���豸����
    
    uint8_t m_aWMac[8];		// ����ģ��ʵʱMac��ַ
    uint8_t m_byWTxPwr;		//����ģ��ʵʱ���书��
		
		ENUM_UNITE_STATE m_eStatus;		// 1-���� 2-�������� 3-��̨���� 4-��̨���� 5-�������� 6-���и��� 7-�������� 8-ȱ������
    
    uint8_t m_byBlood;		//��ԪѪ�� 0~100,100��ʾ״̬������0��ʾ������������ʾ�˵ĳ̶�
    
		uint8_t	m_byMode;		//��ϰ/ѵ��ģʽ,TRAIN--ѵ��ģʽ GAME_PREP--��ϰԤ��ģʽ    GAME_GOING   GAME_OVER
		uint8_t m_byJudge;
		uint8_t m_byDayOrNightJudge;
		//uint8_t m_IsNight:1;
	
		uint8_t m_byKey;		//��Ȩ��
	
		uint8_t Thickness;		//ʵ���������
		uint8_t LayMines_Or_MineClearance;		//��������
		uint8_t EP_Num[3];		//�������
		
    uint8_t m_byActLaserCnt;		//ʵ������̽ͷ��
		
		uint8_t m_IsIllegalMove: 1;	   //�Ƿ��ϴ��Ƿ��ƶ�
    uint8_t m_IsIllegalPowerOff: 1;
    uint8_t m_IsAid: 1; 	           //�Ƿ񱻾���
    uint8_t m_IsNight: 1; 	       //�Ƿ���ҹ��ģʽ
    uint8_t m_IsTerminalOnline: 1;   //�Ƿ�����ն�����
    uint8_t m_IsManoClothDis: 1;	   //��ϰģʽ�±�������
    uint8_t m_IsTestMode: 1;	//����ģʽ
    uint8_t m_IsBulletInit: 1;

		uint8_t m_byAidEndFlag1;		//�Ȼ�������־
		uint8_t m_byAidEndFlag2;		//�Ȼ�������־
		
    ENUM_CI_INDIVIDUAL m_eCIdegree;		//�ۺ�����
    ENUM_BRCHAR m_eBRChar;		//��ݱ�ʶ
    STRUCT_TS_Character1 m_sTsChar1;		//ʵ��״̬��1
    STRUCT_TS_Character2 m_sTsChar2;		//ʵ��״̬��2
    
    STRUCT_TIME1 m_sUtcTime;		//��ǰUTCʱ��
		StructUTC m_sManoStart;	//��ϰ��ʼʱ��
    StructUTC m_sManoEnd;	//��ϰ����ʱ��
		Struct_Time2 m_sDayStartTime;		//�л����������ʼʱ��
    Struct_Time2 m_sNightStartTime;		//�л�����ҹ����ʼʱ��
		
    STRUCT_LPOS m_sLPosition;		//��ǰλ��
    uint8_t m_byPosType;		//��λ״̬
    uint8_t m_bySatNum;		//��������
    uint8_t m_bFireInstallFlg;		//��Ʒ��װ��־
    uint8_t m_byActLaserCnt1;		//�������ʵ������̽ͷ��(��Ե�����Ϊͷ��)
    
		uint8_t m_byWeaponNum;		//����Я������
		uint8_t m_byBackstageCfgWeaponNum;		//��̨���õ���������
    uint8_t m_bySelfWeaponNum;		//Ĭ����������
    SOLDIERAMMU_T m_saBackstageCfgAmmuInfo[WEAPONKIND_MAX];		//��̨����������Ϣ
    SOLDIERAMMU_T m_saWeaponSelfAmmuInfo[WEAPONKIND_MAX];		//Ĭ��������Ϣ
		SOLDIERAMMU_T m_saAmmuInfo[WEAPONKIND_MAX];		//������Ϣ

		DEATH_MAG m_sLastDeathMag;
		
		uint8_t	m_byUploadInterval;		//�ϴ�ʱ��
		uint8_t m_byDynamometerDisplayStatusDataUploadCycle;		//ʵ���ը����ʾ��״̬�����ϴ�����
    uint8_t m_byEquipmentAndIdentityDataUploadCycle;		//����Ա(װ��)��������������ϴ�����
    
		STRUCT_BroadcastBeacon m_sBroadcastInfo;
		
		uint8_t m_byEnableChangeBat;

    uint32_t	m_dwChk;    //У��
} USER_PARAM;


extern USER_PARAM g_sUserParam;

__packed typedef struct
{
    uint8_t m_byGS_or_ZY;       //���˹���ߴ���Ԫ�ı�־��0��˹��1��Ԫ
    uint8_t m_byG_Num;          //װҩ��
    uint8_t m_byYaoTemperature;	//ҩ�¶�
    uint8_t m_byTspanSelect;    //���鵯ҩ��������ʱʱ��ѡ��
    uint8_t m_byT_SpanX;        //���鵯ҩ��������ʱʱ��
    uint8_t m_byPCVelocity2;    //�ٶ�ƫ��
    uint8_t m_byNum1;           //���䵯ҩ����
    //���ֻ����
    uint8_t m_byRmode;          //���ģʽ
    uint8_t m_byT_GUASS_X[3];   //��˹����x
    uint8_t m_byT_GUASS_Y[4];   //��˹����y
    uint8_t m_byT_GUASS_H[2];   //��˹����߳�
    //�����ֻ����
    uint8_t m_Angle1J[2];       //�������
    uint8_t m_Angle1X[2];       //��������
    uint8_t m_Pressure[2];      //��ص�����ѹ
    uint8_t m_Angle2[2];        //��ص������
    uint8_t m_ZDDMVelocity1;    //��ص������
    uint8_t m_ZDTemperature;    //�������
		uint8_t m_byPJPLoactionFlag;
}STRUCT_Various_Elements;

extern STRUCT_Various_Elements g_sVarious_Elements;

__packed typedef struct
{
    u8 m_byA_Type;				//��ҩ����
    u8 m_byA_Subtype;			//��ҩ������
    u8 m_aChangeAmmuNum[3];	    //�仯�ĵ���

} STRUCT_AMMUSUPPLY_t;

__packed typedef struct
{
    u8 m_byM_Type;				//��������
    u8 m_byM_Subtype;			//����������
    u8 m_aRemainSupplyNum[3];	//����ʣ��
} Struct_Supply;				//������Ϣ

__packed typedef struct
{
    uint8_t		m_aData_Frame[2];		//֡ͷ���̶�Ϊ0xC1,0xF2��
    uint8_t 	m_byD_Len;				//����֡����
    uint8_t 	m_byT_Type;				//ʵ������
    uint8_t 	m_byT_Subtype;			//ʵ��������
    uint8_t 	m_byRID;				//��λ����
    uint8_t 	m_aPID[2];					//�豸���(���ģʽ)
    uint8_t 	m_byMasterT_Type;		//����ϵͳʵ������
    uint8_t 	m_byMasterT_Subtype;	//����ϵͳʵ��������
    uint8_t 	m_byMasterRID;			//����ϵͳ��λ����
    uint16_t 	m_wMasterPID;			//����ϵͳ�豸���(���ģʽ)

    uint8_t m_byKey;							//��Կ
    uint8_t m_byBR_Attribute;				//�������� ENUM_BRCHAR
	
		STRUCT_TIME1 m_sStartTime	;		//��ϰ��ʼʱ��
    STRUCT_TIME1 m_sEndTime	;		//��ϰ��ʼʱ��
    STRUCT_TIME2 m_sDayStartTime;		//�л����������ʼʱ��
    STRUCT_TIME2 m_sNightStartTime;		//�л�����ҹ����ʼʱ��
	
    uint8_t m_byBulletMaxAmmuNum[3];			//ǹ�����Я������
    uint8_t m_byShellMaxAmmuNum[3];			//�ڵ�/��������Я������
    uint8_t m_byAmmuNum;						//Я��������������m,m=0 ʱ�����������ݣ�
    //uint8_t m_bySupplyNum;					//Я��������������n��n=0 ʱ�����������������ݣ�
    SOLDIERAMMU_T m_sAmmu[WEAPONKIND_MAX];	        //������Ϣ
    //Struct_Supply m_sSupply[6];		//������Ϣ
		
    uint8_t EP_Num[3];						//�������
    uint8_t Thickness;					    //ʵ�������������Ч����װ�׺�Ȼ��������ȣ�;  0-19 	ȡֵ*1mm 	��Χ��0-19mm ;  20-255 	��ȡֵ-19��*10mm 	��Χ��20mm-2360mm

    uint8_t 	m_byXor;		//����֡����
} Struct_CtrlManoParam_IndividualSoldierDownload;			//��ϰ�������ã�������

__packed typedef struct
{
    uint8_t m_aData_Frame[2];		//֡ͷ���̶�Ϊ0xC1,0xF3��
    uint8_t m_byD_Len;				//����֡����
    uint8_t m_byT_Type;				//ʵ������
    uint8_t m_byT_Subtype;			//ʵ��������
    uint8_t m_byRID;				//��λ����
    uint8_t m_aPID[2];				//�豸���(���ģʽ)
    uint8_t m_byMasterT_Type;		//����ϵͳʵ������
    uint8_t m_byMasterT_Subtype;	//����ϵͳʵ��������
    uint8_t m_byMasterRID;			//����ϵͳ��λ����
    uint8_t m_aMasterPID[2];			//����ϵͳ�豸���(���ģʽ)

    uint8_t m_bySelfStatusDataUploadCycle;					//ʵ������״̬�����ϴ����� 	��
    uint8_t m_byDynamometerDisplayStatusDataUploadCycle;		//ʵ���ը����ʾ��״̬�����ϴ�����  ��
    uint8_t m_byEquipmentAndIdentityDataUploadCycle;			//����Ա��װ������������������ϴ�����  ��

    uint8_t 	m_byXor;					//����֡����
} Struct_CtrlIntervalDownload;				//�ϴ���������

__packed typedef struct
{
    uint8_t		m_aData_Frame[2];		//֡ͷ���̶�Ϊ0xC1,0xF4��
    uint8_t 	m_byD_Len;				//����֡����
    uint8_t 	m_byT_Type;				//ʵ������
    uint8_t 	m_byT_Subtype;			//ʵ��������
    uint8_t 	m_byRID;				//��λ����
    uint8_t 	m_aPID[2];				//�豸���(���ģʽ)
    uint8_t 	m_byMasterT_Type;		//����ϵͳʵ������
    uint8_t 	m_byMasterT_Subtype;	//����ϵͳʵ��������
    uint8_t 	m_byMasterRID;			//����ϵͳ��λ����
    uint8_t 	m_aMasterPID[2];		//����ϵͳ�豸���(���ģʽ)
    u8 m_byChangeAmmuNum;				//�����仯��ҩ��������m��m=0 ʱ����ҩ�����������ݣ�
    u8 m_byChangeSuppleNum;				//�����仯������������n��n=0 ʱ�����������������ݣ�

    STRUCT_AMMUSUPPLY_t m_sChangeAmmu[6];		//��ҩ��Ϣ
    Struct_Supply m_sChangeSupply[6];	//������Ϣ

    uint8_t 	m_byXor;				//����֡����
} Struct_CtrlSupplementDownload;			//��ҩ�����ʣ��ַ�/����

typedef struct
{
    uint8_t H_Part;
    uint8_t H_Type;
    uint8_t H_Degree;
	
}INJURYIFNO_T;

__packed typedef struct{
	uint8_t		m_aData_Frame[2];		//֡ͷ���̶�Ϊ0xC1,0xF5��
	uint8_t 	m_byD_Len;				//����֡���� 
	uint8_t 	m_byT_Type;				//ʵ������
	uint8_t 	m_byT_Subtype;			//ʵ�������� 
	uint8_t 	m_byRID;				//��λ���� 
	uint8_t 	m_aPID[2];					//�豸���(���ģʽ)
	uint8_t 	m_byMasterT_Type;		//����ϵͳʵ������
	uint8_t 	m_byMasterT_Subtype;	//����ϵͳʵ�������� 
	uint8_t 	m_byMasterRID;			//����ϵͳ��λ���� 
	uint8_t 	m_aMasterPID[2];			//����ϵͳ�豸���(���ģʽ)	
	
	uint8_t 	m_byAC_Character;		//�þ������� 	ENUM_ACCHAR		
  STRUCT_TIME1 m_judgeTime;		//�þ�ʱ��    
	uint8_t 	m_byXor;				//����֡����
	
}Struct_JudgeDownload;					//���زþ�

__packed typedef struct
{
    uint8_t m_aData_Frame[2];		//֡ͷ���̶�Ϊ0xC1,0xF6��
    uint8_t m_byD_Len;		//����֡����
    uint8_t m_byT_Type;		//�����ʵ������
    uint8_t m_byT_Subtype;		//�����ʵ��������
    uint8_t m_byRID;		//�����ʵ�嵥λ����
    uint8_t m_aPID[2];		//�����ʵ���豸���
    uint8_t m_byH_Part;		//�����ʵ�����в�λ
    uint8_t m_byH_Type;		//�����ʵ������
    uint8_t m_byH_Idegree;		//�����ʵ����������
    STRUCT_TIME1 m_sAttackTime;		//�����ָ��ʱ��
    uint8_t m_byFT_Type;		//������ʵ������
    uint8_t m_byFT_Subtype;		//������ʵ��������
    uint8_t m_byF_RID;		//������ʵ�嵥λ����
    uint8_t m_aF_PID[2];		//������ʵ���豸���
    uint8_t m_aFA_Type;		//��������ҩ����
    uint8_t m_aFA_Subtype;		//��������ҩ������
    STRUCT_LPOS m_sL_Position ;		//��γ�� �߶� ��λ״̬

    uint8_t 	m_byXor;		//����֡����
} Struct_DamageReportDownload;		

__packed typedef struct
{
    uint8_t m_aData_Frame[2];		//֡ͷ���̶�Ϊ0xC1,0xF7��
    uint8_t m_byD_Len;		//����֡����
    uint8_t m_byT_Type;		//ʵ������
    uint8_t m_byT_Subtype;		//ʵ��������
    uint8_t m_byRID;		//��λ����
    uint8_t m_aPID[2];		//�豸���(���ģʽ)

    uint8_t m_byFT_Type;		//���������������ʵ������
    uint8_t m_byFT_Subtype;		//���������������ʵ��������
    uint8_t m_byF_RID;		//���������������ʵ�嵥λ����
    uint8_t m_aF_PID[2];		//���������������ʵ���豸���
    uint8_t m_byNCBT_Type;		//�˻�����ʶ�� 	ENUM_NCBTTYPE
    uint8_t m_byStrength ;		//մȾ����вǿ��  	0-100
    uint8_t m_byELR_Character ;		//����/�뿪�˻���մȾ�����ʶ��  	ENUM_ELRCHAR

    uint8_t m_byXor;		//����֡����

} Struct_NCBTDownload;		//����/�뿪�˻���մȾ��

__packed typedef struct
{
    uint8_t	m_aData_Frame[2];		//֡ͷ���̶�Ϊ0xC1,0xF6��
    uint8_t m_byD_Len;		//����֡����
    u8 m_byT_Type;		//ʵ������
    u8 m_byT_Subtype;		//ʵ��������
    u8 m_byRID;		//ʵ�嵥λ����
    u8 m_aPID[2];		//ʵ���豸���
    u8 m_byFT_Type;		//����ʵ������
    u8 m_byFT_Subtype;		//����ʵ��������
    u8 m_byF_RID;		//����ʵ�嵥λ����
    u8 m_aF_PID[2];		//����ʵ���豸���
    STRUCT_LPOS m_sL_Position ;		//����ʵ�����λ��
    u8 ELR_Character;		//����/�뿪ʵ�帽�������ʶ��
    uint8_t m_byXor;		//����֡����
} Struct_HelicopterVoiceCtr;		//����ֱ����ָ��

__packed typedef struct
{
    uint8_t		m_aData_Frame[2];			//֡ͷ���̶�Ϊ0xC1,0xFD��
    uint8_t 	m_byD_Len;					//����֡����
    uint8_t 	m_byT_Type;					//ʵ������
    uint8_t 	m_byT_Subtype;				//ʵ��������
    uint8_t 	m_byRID;					//��λ����
    uint8_t 	m_aPID[2];					//�豸���(���ģʽ)

    uint8_t 	m_byES_Num[2];					//�¼�����֡���к�

    uint8_t 	m_byXor;				    //����֡����

} Struct_EventResponceDownload;				//�¼�����Ӧ��ָ��
/******************************************************************
*                           �¼�����                         *
******************************************************************/
typedef enum
{
    BNEventType_RSP_CtrlManoParam_NotIndividualSoldier = 0x01, //��ϰ�������óɹ��¼����ǵ�����
    BNEventType_RSP_CtrlManoParam_IndividualSoldier = 0x02, //��ϰ�������óɹ��¼���������
    BNEventType_RSP_CtrlInterval = 0x03, //�ϴ��������óɹ��¼�
    BNEventType_RSP_CtrlSupplement = 0x04, //��ҩ�����ʣ��ַ�/�����¼�
    BNEventType_DamageReport = 0x05, //���������¼�
    BNEventType_Judge = 0x06, //���زþ��¼�
    BNEventType_NCBT = 0x07, //����/�뿪�˻���մȾ���¼�
    BNEventType_NCB_Protection = 0x08, //�˻��������¼�
    BNEventType_NCB_Injury = 0x09, //�˻��������¼�
    BNEventType_PE_Threat = 0x0A, //�����в�¼�
    BNEventType_PE_Protection = 0x0B, //����/�յ������¼�
    BNEventType_DigitIndirectLanuch = 0x0C, //���ֻ����ڷ����¼�
    BNEventType_NonDigitIndirectLanuch = 0x0D, //�����ֻ����ڷ����¼�(�Ȼ���)
    BNEventType_Radar = 0x0E, //��λ���У���״����/У���¼�
    BNEventType_HeavyWpnLaunch = 0x0F, //ֱ�������������¼�
    BNEventType_IndLayMine = 0x10, //���������¼�
    BNEventType_IndCleanMine = 0x11, //���������¼�
    BNEventType_RocketLayMine = 0x12, //��������¼�
    BNEventType_CarCleanMine = 0x13, //ɨ�׳�ɨ���¼�
    BNEventType_RocketCleanMine = 0x14, //���ɨ���¼�
    BNBNEventType_InjuryWorse = 0x15, //��Ա���ƶ��¼�
    BNEventType_IndRescued = 0x16, //�������Ȼ��¼�
    BNEventType_ArrangeHinder = 0x17, //ս���ϰ������¼�
    BNEventType_RSP_CtrlFirePoint = 0x18, //�����������¼�
    BNEventType_StatusRecovery = 0x19, //״̬�Զ��ָ��¼�
    BNEventType_RSP_CtrlEngineerRecon = 0x1A, //��������¼�
    BNEventType_EquipRepair = 0x1B, //װ�������¼�
    BNEventType_ChangeToMano = 0x1C, //��ϰģʽת���¼�
    BNEventType_IndWeaponOnline = 0x1D, //�����������¼�
    BNEventType_TerminalOnOff = 0x1E, //���ػ��¼�
    BNEventType_AgaistRuleStart = 0x1F, //Υ�濪ʼ�¼�
    BNEventType_AgaistRuleStop = 0x20, //Υ������¼�
    //�Զ�����չ
    BNEventType_Alarm = 0x81, //�澯�¼�
} EnumBNEventType;

typedef enum{
	//������̨
	BNCmd_Terminal_WorkConditionInfoUpload = 0x90,		//������Ϣ�ϴ�
	BNCmd_Terminal_AccessRequest =	0x91,		//�ն���������
	BNCmd_Terminal_StatusUpload  =	0x92,		//״̬�ϴ�
	BNCmd_Terminal_EventUpload   =	0x93,		//�¼��ϴ�
	BNCmd_Terminal_StatusUpload_Bind = 0x94,		//����Ա��װ������״̬�ϴ�ָ��
	BNCmd_Terminal_EventUpload_Bind  = 0x95,		//����Ա��װ�������¼��ϴ�ָ��
	
	//��̨����
	//Cmd_BS_CtrlFSJ = 0xef,		//���������
	BNCmd_CtrlHelicopter = 0xE3,
	BNCmd_CtrlTerminalParam = 0xE4,		//�ն˲�������ָ��
	BNCmd_CtrlSetTerminalType = 0xE5,		//�����ն�����
	BNCmd_CtrlHeartBeat = 0xE6,		//����	
	BNCmd_CtrlSupplementary_Mano_Param = 0xE8,		//������ϰ��������ָ��
	BNCmd_CtrlAccessAuthorization	=	0xE9,		//������Ȩָ��
	BNCmd_CtrlManoParam_NotIndividualSoldier	=	0xF1,		//��ϰ��������ָ��ǵ�����
	BNCmd_CtrlManoParam_IndividualSoldier     =	0xF2,		//��ϰ��������ָ�������
	BNCmd_CtrlInterval    =	0xF3,		//�ϴ���������ָ��
	BNCmd_CtrlSupplement  =	0xF4,		//��ҩ/���ʲ���ָ��
	BNCmd_CtrlJudge				=	0xF5,		//����ϵͳ�þ�ָ��
	BNCmd_CtrlIndirectAim =	0xF6,		//����ģ����ָ��
	BNCmd_CtrlNCBT				=	0xF7,		//����/�뿪�˻���մȾ��ͨ��ָ��
	BNCmd_CtrlBurstPointDisplay =	0xF8,		//ը����ʾ����������ָ��
	BNCmd_CtrlPointFall			=	0xF9,		//���/У����Ϣ�·�ָ���λ����״���ڱ���죩
	BNCmd_CtrlFirePoint			=	0xFA,		//�������������ָ��
	BNCmd_CtrlEngineerRecon	=	0xFB,		//�����Ϣ�·�ָ�������죩
	BNCmd_CtrlMinefield			=	0xFC,		//��������ָ��
	BNCmd_BSResponce			  =	0xFD,		//����Ӧ��ָ��
	 
	// add by xfw 20191026	
	BNCmd_GetRidPid				=	0x80,		//��ȡRID PID
	BNCmd_SetRidPid				=	0x81,		//����RID PID
	BNCmd_GetBleSlaverMac =	0x82,		//��ȡ���豸
	BNCmd_SetBleSlaverMac =	0x83,		//���ô��豸
	BNCmd_SaveBleSlaverMac = 0x84,		//������豸
	BNCmd_GetLog = 0x85,		//��ȡ��־
	BNCmd_GetVersion = 0x86,		//��ȡ�汾��
	BNCmd_GetBackstageMag =	0x87,		//��ȡ��̨����ṹ��
	BNCmd_ZigbeeJoinedNewNet =	0x88,	//Zigbeeģ���뿪��ǰ���磬�����µ�����
	BNCmd_4GServerConfig =	0x89,		//4Gģ������
	BNCmd_GetHardwareStatus =	0x8A,		//��ȡ�弶Ӳ��״̬
	BNCmd_CtrlTType =	0x8B,		//�ն���������
	BNCmd_CtrlPowerOff =	0x8C,		//�ػ�ָ��	
}EnumNewBackStage_CMD;

typedef struct  
{										    
		STRUCT_TIME1 m_sUtcTime;		//UTCʱ��
	
		uint32_t m_dwLatitude;		//γ�� ���ݲ��ṩ����Ҫ��ʱ���Ի��㣩
		uint8_t  m_byNorS;		//��γ/��γ,'N':��γ;'S':��γ				  
		uint32_t m_dwLongitude;		//���� ���ݲ��ṩ����Ҫ��ʱ���Ի��㣩
		uint8_t  m_byEorW;		//����/����,E:����;W:����
		int32_t  m_dwAltitude;		//���θ߶�,�Ŵ���10��,ʵ�ʳ���10.��λ:0.1m  ���ݲ��ṩ��	 
		uint16_t m_wSpeed;		//��������,�Ŵ���1000��,ʵ�ʳ���10.��λ:0.001����/Сʱ	   ���ݲ��ṩ��
		uint8_t  m_byPositionType;		//��ǰ��λ����
		uint8_t  m_byJD_Du;
		uint8_t  m_byJD_Fen;
		uint32_t m_dwJD_Fen_XiaoShu;		//����С������λС��
		uint8_t  m_byWD_Du;
		uint8_t  m_byWD_Fen;
		uint32_t m_dwWD_Fen_XiaoShu;		//����С������λС��
		uint8_t	 m_byNumSv;		//���ڶ�λ��������Ŀ��00~24
		uint8_t  RTKMode;
}NMEA_MAG;

#define USER_PARAM_MEMBER_GET(name) UserParam_Get##name()
#define FUNC_USER_PARAM_MEMBER_GETBY(name)  uint8_t UserParam_Get##name(void) {return g_sUserParam.m_b##name;}


/****************************************************************
@FUNCTION���ն�����ʱ��������Ĭ��ֵ
@INPUT����
@OUTPUT��g_sUserParam�ṹ�帳ֵ
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ������ȡ��ʧ�ܣ�����������
****************************************************************/
void UserParam_SetDefaultValue(USER_PARAM *pParam);

/****************************************************************
@FUNCTION���ն�����ʱ������ʼ��
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ������ȡ��ʧ�ܣ�����������
****************************************************************/
void UserParam_Init(void);

/****************************************************************
@FUNCTION���ն�����ʱ��������
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �����ϳ�ʱ��
****************************************************************/
void UserParam_Save(void);


/****************************************************************
@FUNCTION����ȡ�û������е����豸��
@INPUT����
@OUTPUT����
@RETURN:  ���豸��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint16_t UserParam_GetSlaverCnt(void);

/****************************************************************
@FUNCTION����ȡ�û������е����豸��
@INPUT����
@OUTPUT����
@RETURN:  ���豸��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void UserParam_SetSlaverCnt(uint8_t Cnt);

/****************************************************************
@FUNCTION����ȡ�û������е����豸״̬��ȡ���
@INPUT����
@OUTPUT����
@RETURN:  ���豸��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint16_t UserParam_GetSlaverStatusInterval(void);

/****************************************************************
@FUNCTION�������û������е����豸״̬��ȡ���
@INPUT�����
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void UserParam_SetSlaverStatusInterval(uint16_t Val);

/****************************************************************
@FUNCTION����ȡ�û������еĹ���ṹ��
@INPUT����
@OUTPUT����
@RETURN: �������
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
SINGLE_SLAVER_MAG *UserParam_GetSlaverMag(uint8_t idx);


/****************************************************************
@FUNCTION�������û������е����豸����ṹ��
@INPUT�����豸����ṹ�壬���豸���
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void UserParam_SetSlaverMag(SINGLE_SLAVER_MAG * pVal, uint8_t idx);


/****************************************************************
@FUNCTION����ȡ��ǰѪ��
@INPUT����
@OUTPUT����
@RETURN: Ѫ��
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint8_t UserParam_GetBlood(void) ;

/****************************************************************
@FUNCTION�����õ�ǰѪ��
@INPUT����ֵ
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetBlood(uint8_t Val) ;

/****************************************************************
@FUNCTION����ȡ��ǰ��Կ
@INPUT����
@OUTPUT����
@RETURN: ��Կ
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint8_t UserParam_GetKey(void) ;

/****************************************************************
@FUNCTION�����õ�ǰ��Կ
@INPUT����ֵ
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetKey(uint8_t Val) ;

/****************************************************************
@FUNCTION����ȡ��ǰ����̽ͷ��
@INPUT����
@OUTPUT����
@RETURN: ����̽ͷ��
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint8_t UserParam_GetActLaserCnt(void);

/****************************************************************
@FUNCTION�����õ�ǰ����̽ͷ��
@INPUT����ֵ
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetActLaserCnt(uint8_t Val);

/****************************************************************
@FUNCTION����ȡ��ǰ��ݱ�ʶ
@INPUT����
@OUTPUT����
@RETURN: ��ݱ�ʶ
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
ENUM_BRCHAR UserParam_GetBRChar(void) ;

/****************************************************************
@FUNCTION�����õ�ǰ��ݱ�ʶ
@INPUT����ֵ
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetBRChar(ENUM_BRCHAR eVal) ;

/****************************************************************
@FUNCTION����ȡ�ۺ�����
@INPUT����
@OUTPUT����
@RETURN: �ۺ�����
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
ENUM_CI_INDIVIDUAL UserParam_GetCIdegree(void);

/****************************************************************
@FUNCTION�������ۺ�����
@INPUT����ֵ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetCIdegree(ENUM_CI_INDIVIDUAL eCI);

/****************************************************************
@FUNCTION����ȡʵ��״̬��1
@INPUT����
@OUTPUT����
@RETURN: ʵ��״̬��1
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
STRUCT_TS_Character1 UserParam_GetTsChar1(void);

/****************************************************************
@FUNCTION������ʵ��״̬��1
@INPUT����ֵ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetTsChar1(STRUCT_TS_Character1 Val);

/****************************************************************
@FUNCTION����ȡʵ��״̬��2
@INPUT����
@OUTPUT����
@RETURN: ʵ��״̬��2
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
STRUCT_TS_Character2 UserParam_GetTsChar2(void);

/****************************************************************
@FUNCTION������ʵ��״̬��2
@INPUT����ֵ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetTsChar2(STRUCT_TS_Character2 Val);

/****************************************************************
@FUNCTION����ȡ����ģ��Mac��ַ
@INPUT����
@OUTPUT����
@RETURN: Zigbeeģ��Mac��ַ
@AUTHOR��xfw
@SPECIAL: 
****************************************************************/
uint8_t *UserParam_GetWMac(void);

/****************************************************************
@FUNCTION����������ģ��Mac��ַ
@INPUT�� ����ֵ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��Ӱ��ʵ��MAC
****************************************************************/
void UserParam_SetWMac(uint8_t * pVal, uint16_t ValLen);

/****************************************************************
@FUNCTION����ȡ��ǰ����ģ�鷢�书��
@INPUT����
@OUTPUT����
@RETURN: ��Կ
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint8_t UserParam_GetWTxPwr(void);

/****************************************************************
@FUNCTION�����õ�ǰ����ģ�鷢�书��
@INPUT����ֵ
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL: ֻ��ֵ���ݣ����漰Ӳ������
****************************************************************/
void UserParam_SetWTxPwr(uint8_t Val);

/****************************************************************
@FUNCTION����ȡ��γ������
@INPUT����
@OUTPUT����
@RETURN: ��γ������
@AUTHOR��xfw
@SPECIAL: 
****************************************************************/
STRUCT_LPOS *UserParam_GetLPos(void);

/****************************************************************
@FUNCTION�����þ�γ������
@INPUT�� ����ֵ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: 
****************************************************************/
void UserParam_SetLPos(STRUCT_LPOS * pVal);

/****************************************************************
@FUNCTION����ȡUTCʱ��
@INPUT����
@OUTPUT����
@RETURN: UTCʱ��
@AUTHOR��xfw
@SPECIAL: 
****************************************************************/
STRUCT_TIME1 *UserParam_GetTime1(void);

/****************************************************************
@FUNCTION������UTCʱ��
@INPUT�� ����ֵ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: 
****************************************************************/
void UserParam_SetTime1(STRUCT_TIME1 * pVal);

/****************************************************************
@FUNCTION����ȡ��λ״̬
@INPUT����
@OUTPUT����
@RETURN: ��Կ
@AUTHOR��xfw
@SPECIAL: 
****************************************************************/
uint8_t UserParam_GetPosType(void);

/****************************************************************
@FUNCTION�����ö�λ״̬
@INPUT����ֵ
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL: 
****************************************************************/
void UserParam_SetPosType(uint8_t Val);

/****************************************************************
@FUNCTION����ȡ��������
@INPUT����
@OUTPUT����
@RETURN: ��Կ
@AUTHOR��xfw
@SPECIAL: 
****************************************************************/
uint8_t UserParam_GetSatNum(void);

/****************************************************************
@FUNCTION��������������
@INPUT����ֵ
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetSatNum(uint8_t Val);

/****************************************************************
@FUNCTION����ȡ��Ʒ��װ��־
@INPUT����
@OUTPUT����
@RETURN: ��Կ
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint8_t UserParam_GetFireInstallFlg(void);

/****************************************************************
@FUNCTION�����û�Ʒ��װ��־
@INPUT����ֵ
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL: 
****************************************************************/
void UserParam_SetFireInstallFlg(uint8_t Val);

/****************************************************************
@FUNCTION����ȡ�������ʵ������̽ͷ��(��Ե�����Ϊͷ��)
@INPUT����
@OUTPUT����
@RETURN: ��Կ
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint8_t UserParam_GetActLaserCnt1(void);

/****************************************************************
@FUNCTION�����÷������ʵ������̽ͷ��(��Ե�����Ϊͷ��)
@INPUT����ֵ
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetActLaserCnt1(uint8_t Val);





/****************************************************************
@FUNCTION������ģʽ����
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��
****************************************************************/
void TesModeHandle(void);

/****************************************************************
@FUNCTION����ȡ�弶�Զ���Ӳ��״̬1
@INPUT����
@OUTPUT����
@RETURN:  �Զ���Ӳ��״̬1
@AUTHOR��xfw
@SPECIAL: ��
****************************************************************/
uint8_t GetHardSta1(void);

/****************************************************************
@FUNCTION����ȡ�弶�Զ���Ӳ��״̬2
@INPUT����
@OUTPUT����
@RETURN:  �Զ���Ӳ��״̬2
@AUTHOR��xfw
@SPECIAL: ��
****************************************************************/
uint8_t GetHardSta2(void);

/****************************************************************
@FUNCTION����ȡ�弶�Զ���Ӳ��״̬3
@INPUT����
@OUTPUT����
@RETURN:  �Զ���Ӳ��״̬3
@AUTHOR��xfw
@SPECIAL: ��
****************************************************************/
uint8_t GetHardSta3(void);

/****************************************************************
@FUNCTION����ȡ�弶�Զ���Ӳ��״̬4
@INPUT����
@OUTPUT����
@RETURN:  �Զ���Ӳ��״̬4
@AUTHOR��xfw
@SPECIAL: ��
****************************************************************/
uint8_t GetHardSta4(void);

/****************************************************************
@FUNCTION����ȡ��̨�ź�ǿ��
@INPUT����
@OUTPUT����
@RETURN:  �ź�ǿ�ȣ�0-4��
@AUTHOR��xfw
@SPECIAL: ��
****************************************************************/
uint8_t GetDTRssi(void);


typedef struct
{
    ENUM_CONN_CHAN m_eChan;   //ͨ��ѡ��
    uint8_t *m_pDstAddr;  //��ͨ��ѡ��ZIGBEE��������Ŀ��MAC��ַ
    uint8_t m_byDstAddrLen; //Ŀ���ַ���ȣ���Ϊ2���ҵ�ַΪ0xFFFF����Ϊ�㲥
    uint8_t *m_pDatBuf;   //�����غ�
    uint16_t m_wDatBufLen; //�����غɳ���
    //FRAME_BUF *m_pFrame; //��������֡
    uint32_t  m_dwRepeatCnt;    //����ط�����
    uint32_t  m_dwScheTime;    //�ط����
}S_CONN_CHAN_PARAM;


typedef enum
{
    //������̨
    Cmd_AccessRequest	=	0x91,	    //�ն���������
    Cmd_StatusUpload	=	0x92,	    //״̬�ϴ�
    Cmd_EventUpload	=	0x93,	        //�¼��ϴ�
    Cmd_Response	=	0x94,		    //ͨ��Ӧ��
    //��̨����
    Cmd_CtrlFSJ		=	0xef,		    //���������
    Cmd_AccessAuthorization	=	0xf1,	//������Ȩָ��
    Cmd_CtrlManoeuvre	=	0xf2,		//��ϰ����ָ��
    Cmd_CtrlInterval	=	0xf3,		//״̬��Ϣ�ϴ����ʱ������ָ��
    Cmd_CtrlIndirectAim	=	0xf4,		//���ش��ָ��
    Cmd_CtrlStartEnd	=	0xf5,		//5.��ʼ����ָ��
    Cmd_CtrlSupplement	=	0xf6,		//6.��ҩ���ʲ���ָ��
    Cmd_CtrlStartEndTime =	0xf7,		//7.��ϰʱ�������ã��޸ģ�ָ��
    Cmd_CtrlBloodVolume	=	0xf8,		//Ѫ������ָ��
    Cmd_CtrlSync		=	0xf9,		//����ͬ��ָ��
    Cmd_BSResponce		=	0xfa,		//����Ӧ��ָ��
    Cmd_CtrlTType		=	0xfc,		//�ն���������
    Cmd_CtrlDayAndNight	=	0xfd,		//��ҹģʽ�л�ָ��
    Cmd_CtrlPowerOff	=	0xfe,		//�ػ�ָ��
    Cmd_CtrlPointFall	=	0xed,		//������㷴��
    // add by xfw 20191026
    Cmd_GetRidPid		=	0x80,		//��ȡRID PID
    Cmd_SetRidPid		=	0x81,		//����RID PID

    Cmd_GetBleSlaverMac	=	0x82,		//��ȡ���豸
    Cmd_SetBleSlaverMac	=	0x83,		//���ô��豸
    Cmd_SaveBleSlaverMac =	0x84,		//������豸

    Cmd_GetLog			=	0x85,		//��ȡ��־
    Cmd_GetVersion		=	0x86,		//��ȡ�汾��
    Cmd_GetBackstageMag	=	0x87,		//��ȡ��̨����ṹ��
    Cmd_ZigbeeJoinedNewNet =	0x88,	//Zigbeeģ���뿪��ǰ���磬�����µ�����
    Cmd_4GServerConfig  =	0x89,		//4Gģ������
    Cmd_GetHardwareStatus =	0x8A,		//��ȡ�弶Ӳ��״̬
    Cmd_ProgramVersion  = 0x8B,         //��ȡ����汾��
    Cmd_TransmitProgram = 0x8C,         //Э���������
    Cmd_HardWareTest    = 0x66,			//����Ӳ��״̬
} EnumBackStage_CMD;

/****************************************************************
@FUNCTION���ڲ�������ͨͨ����������
@INPUT��pChanParam - ��ز���
@OUTPUT��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ֡IDȡֵ����:IR38ͨ��-������<<24; ZIGͨ��-(������ << 24) | Ŀ���ַǰ���ֽ�
****************************************************************/
uint32_t InterChanSend(S_CONN_CHAN_PARAM *pChanParam);

/****************************************************************
@FUNCTION�����豸Ӳ������
@INPUT��pDstAddr - ���豸��ַ��DstAddrLen-���豸��ַ��pCtrlDat - Э�����ݣ�CtrlDatLen - ���ݳ���
@OUTPUT��
@RETURN: TRUE - �ɹ�/ FALSE - ʧ��
@AUTHOR��xfw
@SPECIAL: 
****************************************************************/
uint32_t  InterConn_SlaverHardCtrl(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pCtrlDat,uint16_t CtrlDatLen);


/****************************************************************
@FUNCTION�����豸����
@INPUT��pDstAddr - ���豸��ַ��DstAddrLen-���豸��ַ��pHandle - �������
@OUTPUT��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ע���֡���Ǳ�ɾ��������һֱ�ᷢ��
****************************************************************/
//uint32_t InterConn_SlaverConnect(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen);
uint32_t  InterConn_SlaverConnect(ENUM_CONN_CHAN eChan,SINGLE_SLAVER_MAG *pSingleMag);

//�ڲ�������ͨЭ�����
void InterConn_Analysis(ENUM_CONN_CHAN eChan, uint8_t *pBuf, uint16_t BufLen);

//Ӳ������Э�鴦��55AAЭ��0�����룩
void Prot_HardCtrl_Analysis(uint8_t *pBuf, uint16_t BufLen, void *pHandle);

void InterConn_HardCtrl_Analysis(ENUM_CONN_CHAN eChan, uint8_t *pBuf, uint16_t BufLen);

/****************************************************************
@FUNCTION�������豸״̬�б��ȡָ��MAC��ַ�����豸����ṹ��ָ��
@INPUT��MAC��ַ
@OUTPUT����
@RETURN: ���豸����ṹ��ָ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
SINGLE_SLAVER_MAG* SlaverMag_GetHandleByMac(uint8_t *pMacHex,uint8_t MacHexLen);


/****************************************************************
@FUNCTION�������豸״̬�б��ȡָ��ʵ�����͵����豸����ṹ��ָ��
@INPUT��ʵ������+ ʵ�������Ǹ�
@OUTPUT����
@RETURN: ���豸����ṹ��ָ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
SINGLE_SLAVER_MAG* SlaverMag_GetHandleByType(uint8_t TType,uint8_t TSubType);

//ͷ��Ӳ�����ƻ�������
void HardCtrl_HelmetBase(uint8_t *pBuf, uint16_t BufLen);

//����ͷ�����̹޷���
void CtrlHelmetFume(DEVCTRL eCtrl, uint16_t Tim100Ms);

//����ͷ��̽ͷ��
void CtrlHelmetLaserOnOff(uint8_t LedIdx, uint8_t IsOn);
void CtrlHelmetLaserOn();
//����
void CtrlHelmetVoice(uint8_t VoiceIdx);

void HardCtrl_Slaver_PowerOff(void);

//Zigbee����
void SlaverMag_TimedProcess(void);
uint8_t GetAllSlavesConnectStatus(void);
//Zigbee���ʹ���
void InterConn_Protocol_Process(void);

void HardCtrl_Slaver_Day(void);
void HardCtrl_Slaver_Night(void);
void InterConn_ProtDeal_Slaver_AmmuSupply_Init(void);
void InterConn_ProtDeal_Slaver_AmmuSupply(SINGLE_SLAVER_MAG *pSlaverMag);
void InterConn_ProtDeal_Slaver_AmmuSupplyNum(SOLDIERAMMU_T *WeaponInfo, uint8_t b, uint32_t AmmuNum);
void InterConn_ProtDeal_Slaver_Weapon_State(uint8_t mode);
void InterConn_ProtDeal_SelfStatue(void);
void ZigComTask_BroadCast(uint8_t *pBuf, uint16_t BufLen);
//��������
void CollateralDamage(RCVHURTINFO_T HurtInfo);
uint8_t UserParam_GetWeaponNum(void);
void Mano_Start_Judge(void);
uint8_t Convert_HEXASCII(uint8_t *p1, uint8_t *p2, uint8_t *length);

double get_distance(double lat1/*γ��1*/, double lng1/*����1*/, double lat2/*γ��2*/, double lng2/*����2*/);
POSE_E PoseAnalyse(MPU6050_MAG *pMag);

uint32_t IsMaster(uint8_t TType);
uint32_t BatteryReplacementTask(float ExtBatVol, u8 *pEnableChangeBat);
void UpdateDamage(RCVHURTINFO_T Info);

void MedicalAidCheck(void);
void Train_Mode_Revive(void);
void HardStatusDetection(void);
void NCBT_DamageCheck(void);

void EnableChangeBatHandle(void );

void IllegalEventHandle(void);

void MasterPowerOffCheatStartUploadHandle(void);
void MasterPowerOffCheatEndUploadHandle(void);

void IllegalMoveStartUploadHandle(void);
void IllegalMoveEndUploadHandle(void);

void HelmetCheatStartUploadHandle(void);
void HelmetCheatEndUploadHandle(void);

void FumeCheatStartUploadHandle(void);
void FumeCheatEndUploadHandle(void);

void DetectorLossEvtStartUpload(void);
void DetectorLossEvtStopUpload(void);

void IllegalPowerOffHandle(void);
void FumeInstallHandle(void);
void IllegalMoveMonitor(void);
void DetectorLossHandle(void);
void IllegalHelmetDisconHandle(void);

void GetBulletUniID(u8 A_Type, u8 A_Subtype, u8* pUid);
uint8_t GetDirectWeaponDamage(uint16_t UniID, uint8_t Part);
void DamageReportEventUploadHandle(RCVHURTINFO_T *HurtInfo);

float GetDistance(STRUCT_LPOS Position);

//����
void IRAmmuGetHandle(Struct_CtrlSupplementDownload *Supplement);
uint32_t InterConn_ProtDeal_IR_SetOrRescue_ACK(void);

//��̨
void DeathRecord(void);
void UnUpLoadEvtNumCalcu(void);
void EventStorageToFlash(FRAME_BUF *pFrame, uint32_t Len, uint8_t Num);
void EventNoRspRestore(void);
uint32_t Backstage_ProtocalParse(const uint8_t *pBuf, uint16_t *pLen, uint32_t *pIsCutted);
uint8_t BackStageNewAnalysis(uint8_t *pBuf,uint8_t len);
uint32_t BackStage_Frame_Parse(uint8_t *pBuf, uint16_t *pLen, uint32_t *pIsCutted);
void BackStageFactoryDebug(uint8_t *pRecvBuf, uint16_t RecvLen);
void BSN_Judge_DeathHandle(RCVHURTINFO_T Info);
void BSN_Judge_ReviveHandle(RCVHURTINFO_T Info);
void BSN_Judge_RestartHandle(RCVHURTINFO_T Info);
void BSN_Judge_InvalidHandle(void);
void DayModeHandle(uint8_t IsEnForce);
uint8_t GetWorseLevelBlood(uint8_t Blood);
uint8_t GetReliefBlood(uint8_t Blood);
void NightModeHandle(uint8_t IsEnForce);
void BSN_Judge_SlightHandle(RCVHURTINFO_T Info);
void BSN_Judge_ModerateHandle(RCVHURTINFO_T Info);
void BSN_Judge_SevereHandle(RCVHURTINFO_T Info);
void BSN_Judge_CriticalHandle(RCVHURTINFO_T Info);
void BSN_Judge_WorseHandle(RCVHURTINFO_T Info);
void BSN_Judge_ReliefHandle(RCVHURTINFO_T Info);
void BSN_Judge_ToRedHandle(void);
void BSN_Judge_ToBlueHandle(void);
void DectorCheatUploadHandle(void);
void BSN_Judge_NCBHandle(ENUM_NCBTTYPE NBCtype, RCVHURTINFO_T Info);
void RSP_CtrlSupplementEventUploadHandle(Struct_CtrlSupplementDownload *Supplement);
void ModeChange_Reset(void);
uint8_t SynchronizeState(uint8_t ModeFlg, uint8_t IsBatteryLow);
uint8_t DrillStateRecovery(void);
void DrillStateParamHandle(void);
void BackupDrillState(void);
void DayOrNight_Judge(void);

void SetLPosition(NMEA_MAG *pMag);
uint32_t GPS_FindNMEA0183String(uint8_t *pBuf, uint16_t *pLen, uint32_t *pIsCutted);
void NMEA_GNRMC_Analysis(NMEA_MAG *pMag, uint8_t *buf, uint16_t len);

#endif

#endif



