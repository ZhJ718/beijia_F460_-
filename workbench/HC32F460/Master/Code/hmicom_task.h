#ifndef HMICOM_TASK_H
#define HMICOM_TASK_H

#include "config.h"
#include "hmi_lt268b.h"
#include "comtask.h"
#include "main_task.h"

#if BOARDID == 1
    #define
#elif(  BOARDID == 2)
    #define HMI_COM COM1
#endif

#define HMI_COM_BAUDRATE 115200
#define HMI_COM_FRAME_LEN_MAX 512  //���֧�ֵı��ĳ���


#define HMI_INJURYLEN 11

__packed typedef struct
{
    uint8_t m_byRcdCnt;
    STRUCT_TIME1 m_sUtc;
    uint8_t m_byDamageKind;		//����ENUM_DAMAGE_KIND
    uint8_t m_byPart;		//�˲�ENUM_PARTITION
    uint8_t	m_cBlood;		//����
    uint8_t	m_byResidueBlood;	//ʣ��Ѫ��
    uint8_t m_byFRid;		//�����RID
    uint16_t m_wFPid;		//�����PID
    uint8_t  m_byUniID;		//�����������ҩ����
    uint8_t  m_Ttype;		//��������
    uint8_t  m_Tsubtype;		//����������
    uint8_t  m_byChkSum;		//У���
	
} INJURY_MAG;

extern INJURY_MAG g_sInjuryMag;


typedef struct
{

    HMI268B_LOWLEVEL_MAG *m_pLLMag;

    uint32_t 	m_bIsInit: 1;		//�����ʼ��
    uint32_t 	m_bIsOnline: 1;		//���յ�������
    uint32_t 	m_bRes: 30;

    SCOMTASK_MAG m_sTaskMag;
		
		STRUCT_LPOS m_sLastPos;		//��һ�ε�gps��Ϣ
		double m_Jingdu;		//Ҫ��ʾ�ľ���
    double m_Weidu;		//Ҫ��ʾ��γ��
	
		uint8_t  m_byHurtKind;		//����
		uint8_t  m_byLastMode;		//��һ�ε�ģʽ
    uint8_t  m_byLastIsNight;		//��һ�ε���ҹģʽ
    uint16_t m_wLastPID;		//��һ������PID
    uint8_t  m_byLastBelong;		//��һ�ι���������=0����=1����=2����=3
    uint8_t  m_byLastBlood;		//��һ��Ѫ��
		uint8_t  m_HurtKind;
		INJURY_MAG m_sInjury[11];		//������Ϣ
		
		uint8_t  m_byHurtNextFlg;		//����������Ϣ ��һ����
    uint8_t  m_byHurtCountFlg;		//��¼��������Ĵ���
    uint8_t  m_byHurtfinishFlg;		//������Ϣ��ȡ��ɵı�־
    uint8_t  m_byHurtNone;		//���������һ����0xff�ı�־
		
		uint8_t m_byHelmetIsOnline;
		uint8_t m_byFirstPowerOn;
		uint8_t m_byDispalyOnFlg;
		uint8_t m_byFirstDispFlg;
		uint8_t m_byButtonMsgFlg;
		uint8_t m_byShowInfoPolling;
		
    uint32_t m_dwLastCheckTime;
		uint32_t m_dwLastBatteryShowTime;
		uint32_t m_dwLastInfoShowTime;
		uint32_t m_dwLastBaseInfoFlushTime;
		uint32_t m_dwLastDisplayOnTime;
	
} HMICOMTASK_MAG;

extern HMICOMTASK_MAG g_sHmiComTask_Mag;
/****************************************************************
@FUNCTION����ȡHMI�˿����������
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: 

****************************************************************/
HMICOMTASK_MAG *HmiComTask_HandleGet(void);

/****************************************************************
@FUNCTION����ȡHMI�˿����������
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: 

****************************************************************/
SCOMTASK_MAG *HmiComTask_TaskHandleGet(void);


/****************************************************************
@FUNCTION��HMI ���ڹ��ܳ�ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:

****************************************************************/
void HmiComTask_Init(void);


/****************************************************************
@FUNCTION��HMI ���ڹ��ܷ���ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:

****************************************************************/
void HmiComTask_DeInit(void);

/****************************************************************
@FUNCTION��HMI ���ڹ�������������
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:

****************************************************************/
void HmiComTask_Process(void);

/****************************************************************
@FUNCTION��HMI ���ڹ��ܶ�ʱ����
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:

****************************************************************/
void HmiCom_TimedTask(HMICOMTASK_MAG *pMag);

/****************************************************************
@FUNCTION��HMI ���ڴ���Э�鴦��
@INPUT��
    @param pBuf - ��������
    @param BufLen - �������ݳ���
    @param pHandle - ������
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:

****************************************************************/
void HmiCom_Analysis(uint8_t *pBuf, uint16_t BufLen, void *pHandle);


/****************************************************************
@FUNCTION����ȡ����״̬
@INPUT��
    @param
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void HmiCom_Prot_CheckOnline(HMICOMTASK_MAG *pMag, uint16_t Cnt, uint32_t ScheTim);

/****************************************************************
@FUNCTION����ʾ���Ż����ͼƬ
@INPUT��
    @param eIdx - ͼƬ���
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void HmiCom_Prot_PicShow(HMICOMTASK_MAG *pMag, HMI268B_PICIDX eIdx, uint16_t Cnt, uint32_t ScheTim);
void HmiCom_Prot_BarShow(HMICOMTASK_MAG *pMag, HMI268B_BARIDX eIdx, uint8_t Buf,uint16_t Cnt, uint32_t ScheTim);
uint8_t GetHurtPic(ENUM_PARTITION Part);
WpStr GetOwnBulletTypeString(HMICOMTASK_MAG *pMag);

void HMI_Show_Init(void);

void HMI_GetInjury(HMICOMTASK_MAG* pMag, uint8_t Part, uint8_t TType, uint8_t TSubtype);
void Hmi_Version_Send(HMICOMTASK_MAG *pMag);
void HMI_UctTime(HMICOMTASK_MAG *pMag);
#endif



