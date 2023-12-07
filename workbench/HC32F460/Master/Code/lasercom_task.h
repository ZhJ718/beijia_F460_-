#ifndef LASERCOM_TASK_H
#define LASERCOM_TASK_H

#include "config.h"
#include "linkqueue.h"
#include "lasercom.h"
#include "comtask.h"
#include "main_task.h"

#if BOARDID == 1
    #define LASER_COM COM4
#elif (BOARDID == 2)
    #define LASER_COM COM3
#endif

#define LASER_COM_BAUDRATE 19200
#define LASER_COM_FRAME_LEN_MAX 512  //���֧�ֵı��ĳ���

#define LASER_CTRL_LED_ON 50	//ѵ��ģʽ��LED����ʱ��

typedef struct _LASER_LED_MANAGER
{
	u8 m_bIsCtrlOn:1;	//����LED��
	u8 m_bIsCtrlOff:1;	//����LED��
	u8 m_bIsOn:1;		//LED״ָ̬ʾ
	u8 m_bIsToggle:1;
	u8 m_bRes:4;
	
	u16 m_wLedOnTimer;	//LED����ʱ��
}LASER_LED_MANAGER, *PLASER_LED_MANAGER;

typedef struct
{

    LASERCOM_LOWLEVEL_MAG *m_pLLMag;

    uint32_t m_bIsInit: 1;   //�����ʼ��
    uint32_t m_bIsRecvNew: 1; //���յ�������
				
		uint32_t m_bIsOK: 1;		//�Լ�OK
    uint32_t m_bIsAutoNumbered: 1;		//�Լ�OK
    uint32_t m_bIsShoted: 1;		//TRUE-���յ��е���Ϣ
    uint32_t m_bIsShotArround: 1;		//TRUE-���յ�������Ϣ
    uint32_t m_bIsJudge: 1;		//TRUE-���յ���������ָ��
    uint32_t m_bIsAmmuSupply: 1;		//TRUE-���յ�����ָ��
    uint32_t m_bIsAmmuSet: 1;		//TRUE-���յ�������ʼ��ָ��
    uint32_t m_bIsSuperScan: 1;		//TRUE-���յ����ӳ�������ָ��
    uint32_t m_bIsUpdate: 1;		//TRUE-���յ�����ָ��
    uint32_t m_TestMode: 1;		//TRUE-����ģʽ
    uint32_t m_TestOnlineMode: 1;		//TRUE-���Կ���
    uint32_t m_bRes: 19;

    COMTASK_MAG m_sTaskMag;
		LASER_LED_MANAGER m_aLedCtrlMag[100];		//40
		
		RCVHURTINFO_T m_sRcvHurtInfo;   //���յ��Ĵ����Ϣ

		uint8_t m_byActualNum; //̽ͷʵ������
    uint8_t m_byConfigNum; //̽ͷ��������
		
		uint8_t m_byUniID;		//���������ҩͳһ����
    uint8_t m_byLaserID;		//���̽ͷ���
    uint8_t m_byKey;		//�������Ȩ��
		uint8_t m_byDectorShelterID;
		uint8_t m_byLaserTimer;		//���ں���֡���ʱ�����ڴ�һǹ�ж�����������յ����ݣ���Ҫ����ѡ���˺����صĲ�λ
   
		uint8_t m_sPartID[LASER_PART_MAX][LASER_PART_ID_MAX];
    uint8_t m_sPartIDNum[LASER_PART_MAX];
		
		uint8_t m_DectorShelterFlg;
		uint8_t DamaCalcLater;
		uint8_t ButtletTime;
		float HJ73Distance;
    uint16_t HJ73ShotCount;
		
		uint16_t Shell[PARTITION_MAX];
		uint32_t DectorCheatCntTimer;
		uint32_t m_DectorCheatCnt;
		uint32_t DectorCheatJudgeTimer;
    uint32_t m_dwLaserCtrlTimes;
		uint32_t m_dwSupplyBulletTimes;

} LASERCOMTASK_MAG;
extern LASERCOMTASK_MAG g_sLaserComTask_Mag;

typedef struct
{
    //VWrite write;
    uint8_t m_byActualNum; //̽ͷʵ������
    uint8_t m_byConfigNum; //̽ͷ��������
    RCVHURTINFO_T m_sRcvHurtInfo;   //���յ��Ĵ����Ϣ
    uint8_t m_byUniID;	      //���������ҩͳһ����
    uint8_t m_byLaserID;	  //���̽ͷ���
    uint8_t m_byKey;	      //�������Ȩ��
    uint8_t m_byLaserTimer;     //���ں���֡���ʱ�����ڴ�һǹ�ж�����������յ����ݣ���Ҫ����ѡ���˺����صĲ�λ
    uint8_t m_byShotArroundTimer;
    uint8_t m_byJudgeTimer;
    uint8_t m_byAmmuSupplyTimer;
    uint8_t m_byAmmuSetTimer;
    uint8_t m_bySuperScanTimer;
    uint8_t m_byUpdateTimer;
    uint8_t m_aFrameTemp[15];
    uint32_t m_ulLaserCtrlInterval;
    LASER_LED_MANAGER m_aLedCtrlMag[40];
    uint8_t m_sPartID[LASER_PART_MAX][LASER_PART_ID_MAX];	//�������˷���
    uint8_t m_sPartIDNum[LASER_PART_MAX];
    //TESTMODE_T m_TestLightInfo[40];
    uint8_t m_DectorShelterFlg;
    uint8_t m_DectorShelterID;
    uint8_t m_bIsOK;
    u8 DamaCalcLater;
    u8 ButtletTime;
    float HJ73Distance;
    uint16_t HJ73ShotCount;
} VCOM_LASER_MAG;

extern VCOM_LASER_MAG g_sVLaserMag;
/****************************************************************
@FUNCTION����ȡ������ն˿ڹ�����
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: 

****************************************************************/
LASERCOMTASK_MAG *LaserComTask_HandleGet(void);

/****************************************************************
@FUNCTION����ȡ������ն˿����������
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: 

****************************************************************/
COMTASK_MAG *LaserComTask_TaskHandleGet(void);


/****************************************************************
@FUNCTION��������մ��ڹ��ܳ�ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sLaserComTask_Mag

****************************************************************/
void LaserComTask_Init(void);


/****************************************************************
@FUNCTION��������մ��ڹ��ܷ���ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sLaserComTask_Mag

****************************************************************/
void LaserComTask_DeInit(void);


/****************************************************************
@FUNCTION��������մ��ڹ�������������
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sZigComTask_Mag

****************************************************************/
void LaserComTask_Process(void);

/****************************************************************
@FUNCTION��������մ��ڹ��ܻ�ȡ��ǰ����̽ͷ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ����̽ͷ��
@AUTHOR��xfw
@SPECIAL: g_sZigComTask_Mag

***************************************************************
uint8_t LaserComTask_GetActualNum(void);*/


/****************************************************************
@FUNCTION����ȡ̽ͷ������̽ͷ��
@INPUT��
    @param  MaxCnt - ���ܵ����̽ͷ��
    @param  IntVal - ����̽ͷ������ʱ����λ10ms����Ϊ0,��ȡĬ��ֵ10
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ���ظ�������ģ���������MaxCnt����IntValֵ
****************************************************************/
void LaserCom_Prot_GetOnlineNum(LASERCOMTASK_MAG *pMag, uint8_t MaxCnt, uint8_t IntVal);

/****************************************************************
@FUNCTION��LASER����ģʽ�л�����
@INPUT��LASER�ײ����ṹ��ָ�룬ģʽ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:��
****************************************************************/
void LaserCom_Prot_CtrlMode(LASERCOMTASK_MAG *pMag, LASER_MODE a_Mod);


//RGB����׹⣬��˸
void RGB(LASERCOMTASK_MAG *pMag);

//̽ͷ
void Laser_GetShotHandle_Master(LASERCOMTASK_MAG *pMag);
void Laser_GetShotHandle_Slave(LASERCOMTASK_MAG *pMag);

void LaserProtocalDeal(LASERCOMTASK_MAG *pMag, uint8_t *pBuf, uint8_t len, uint8_t IsMaster);

void Laser_JudgeHandle(LASERCOMTASK_MAG *pMag,uint8_t IsMaster,uint8_t Sta);

void Laser_CtrlLedOn(u8 Id, LASERCOMTASK_MAG *pMag);
void Laser_CtrlLedOff(uint8_t Id,LASERCOMTASK_MAG *pMag);
//̽ͷ����ָʾ�������
void Laser_CtrlLedToggle(uint8_t Id, LASERCOMTASK_MAG *pMag);
void Detector_Led_AllOn();
void Detector_Led_AllOff() ;
void Laser_InsertPartitionAndID(u8 byID, u8 byPartition, LASERCOMTASK_MAG *pMag);
void RemapHurtPart(RCVHURTINFO_T *RcvHurtInfo, u8 Where);
#endif



