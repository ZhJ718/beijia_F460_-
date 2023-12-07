#ifndef RESCOM_TASK_H
#define RESCOM_TASK_H

#include "config.h"
#include "comtask.h"
#include "main_task.h"


#if (BOARDID == 1)
    #define RES_COM COM5
#elif  (BOARDID == 2)
    #define RES_COM COM2
#endif


#define RES_COM_BAUDRATE 115200
#define RES_COM_FRAME_LEN_MAX 2048  //���֧�ֵı��ĳ���

typedef struct
{

    uint8_t 	m_byRes: 8;


    //ʵ�����Ӳ���������ɶ�д
    COM_HW_OPERATIONS m_sHwOps;

} RESCOM_LOWLEVEL_MAG;


#define HEXBUF_DST_LENMAX 512

typedef struct
{
		uint8_t m_4GParamFlg;		//��ȡ��̨�����ı�־
    uint8_t m_4GNetFlg;
    uint8_t m_4GLoginNetFlg;		//4G�����ı�־
    uint8_t m_4GestablishSocketFlg;		//4G�����������socket�ı�־
    uint8_t m_4GcloseListenFlg;		//4G�رռ����ı�־
    uint8_t m_4GcloseLinkFlg;		//4G�ر����ӵı�־
    uint8_t m_4GlogoutNetFlg;		//4G�����ı�־
    uint8_t m_4GwriteIMSIFlg;		//4GдIMSI�ŵı�־
    uint8_t m_4GsaveParamFlg;		//4G��������ı�־
    uint8_t m_4GrestartFlg;		//����4Gģ��;
    uint8_t m_4GDsmsFlg;		//ģ���⵽��վ��־ 0x4
    uint8_t m_4GAskDRXFlg;		//��ѯģ��ģʽ���ã�ȡ��˯��Ӧ��Ϊ0��Ĭ����2
    uint8_t m_4GWriteDRXFlg;		//д��ģ��˯��ģʽ;

    uint8_t m_4GStepFlg;		//4G˳��ִ��ʱ�ı�־
    uint8_t m_4GNetCloseFlg;		//��̨�ڷ��͵Ĺ���ָ����

    uint8_t m_time;		//ָ����ʱ��
    uint8_t m_4GneedSaveAndRst[2];		//�Ƿ���Ҫ����������ı�־ ������־��[DRX��ͬ=1][ISMI��ͬ=1]
    uint8_t m_4GAskImsiFlg;		//��ѯģ���IMSI����
    uint8_t m_StationATFlg;		//��̨�ڷ���AT����ı�־
    uint8_t m_AtOpenTime;		//���Ʒ���atopen����ʱ��ı�־
    uint8_t m_4GReadyFlg;		//4Gģ���ϵ������ɹ�

    uint8_t m_LedGFlg;		//�̵ƿ��Ʊ�־
    uint8_t m_LedRFlg;		//��ƿ��Ʊ�־
    uint8_t m_LedGTimeFlg;		//�̵�ʱ���־
    uint8_t m_LedRTimeFlg;		//���ʱ���־
    uint8_t m_LedGStartBlinkFlg;		//�̵ƿ�ʼ��˸ʹ��
    uint8_t m_LedGRoundCountFlg;		//�̵���˸��������
    uint8_t m_LedRCloseNetFlg;		//�����ر�����ʱ���ú����
		
    uint8_t m_socketID[2];		//socketID 1-5
    uint8_t m_4GTCP_or_UDP;		//4G��������ѡ��ı�־��Ĭ��1Ϊudp
    uint8_t m_farIp[20];		//Զ�˷�������IP��ַ
    uint8_t m_farPort[8];		//Զ�˷������Ķ˿ں�
    uint8_t m_myPort[8];		//�����˿ں�
    uint8_t m_imsi[20];		//������imsi
    uint8_t m_myIP[20];		//������IP��ַ

    uint8_t debugrst;		//����������

    uint8_t m_bySendFailCnt;		//��������ʧ�ܴ���
    uint16_t m_wDataCovertLen;		//���ݱ�ת���ĳ���;

    uint8_t hexbuf_dst[HEXBUF_DST_LENMAX];
    UART_MANAGER *pUartMag;

    uint8_t m_bIsConfiguration: 1;		//�Ƿ�������״̬�����ǣ���ʱ���0D 0A����
    uint8_t m_bIsEnterATMode: 1;		//�Ƿ��ѽ���ATָ��ģʽ
    uint8_t m_bIsSetServerIP: 1;		//�Ƿ������÷�����IP
    uint8_t m_bIsSetMode: 1;		//�Ƿ�������ͨ�ŷ�ʽ
    uint8_t m_bIsSave: 1;		//�Ƿ��ѱ���

    uint8_t m_bIsCFUN0: 1;		//AT+CFUN=0
    uint8_t m_bIsSSWEN1: 1;		//AT^SSWEN=1
    uint8_t m_bIsSSIM: 1;		//AT^SSIM=3,460516840000004,0,00000000000000000000000000000000,00000000000000000000000000000000

    uint8_t m_bIsSSWEN0: 1;		//AT^SSWEN=0
    uint8_t m_bIsCFUN1: 1;	//AT+CFUN=1
    uint8_t m_byIsCommunicationSucc: 1;
    uint8_t m_bRes: 5;

    void *m_pPrivateData;

    uint8_t m_byEnterATModeTimeout;	//����ATģʽ��ʱ
    uint32_t m_dwNoCommunicateTimeOut; //�����ݳ�ʱ����������

    uint32_t ModeRcvTime;
    uint16_t m_RssiLevel;
} FOURGMAG;

__packed typedef struct
{
    uint8_t m_byWeapon_Type;		//����ʵ������
    uint8_t m_byWeapon_SubType;		//����ʵ��������
    uint8_t m_byCfgBulletKind;		//���õ�ҩ������
    uint8_t m_byState;		//����״̬
	
    BULLETINFO_T m_sBulletInfo[WEAPONKIND_MAX];		//������ҩ��Ϣ(���6��)
} WeaponInfoBk;

__packed typedef struct
{
    uint8_t DisableSuicide: 1;		//1=��������ɱ 0=������ɱ
    uint8_t DisableKillTeammate: 1;		//1=������ɱ�˶��� 0=����ɱ�˶���
    uint8_t DisableCheat: 1;		//1=�رշ�ֹ���� 0=�򿪷�����
    uint8_t DisableInstallFume: 1;		//1=����װ��Ʒ 0=װ��Ʒ
    uint8_t EnableDiffRid: 1;		//1=����ͬRID��ս 0=������ͬrid��ս
    uint8_t DisableAssistLaser: 1;		//1=��ϰ�¹رո������� 0=��ϰ�²��رո�������
    uint8_t LocationShowType: 1;		//1=��γ������ 0=��˹����
    uint8_t DisableDrillStartVoice: 1;		//1=��ֹ��ϰѵ��ģʽ��Ч 0=������ϰѵ��ģʽ��Ч

    ENUM_CI_INDIVIDUAL CfgFSHHurtDegree;
    uint8_t DisableDrillStartVoiceTime;
} DRILL_PARAM;

__packed typedef struct
{
    uint8_t DetectorLoss: 1;
    uint8_t FumeUnInstall: 1;
    uint8_t IllegalMove: 1;
    uint8_t HelmetLoss: 1;
    uint8_t res: 4;
} VIOLATIONS;

typedef struct{

	uint32_t m_ulAccessAuthorization;		//�ϴ�������Ȩָ��ʱ��(ms)
	uint32_t m_ulCtrlManoeuvre;		//�ϴ���ϰ����ָ��ʱ��
	uint32_t m_ulCtrlInterval;		//�ϴ�״̬ʱ���ϴ��������ָ��ʱ��
	uint32_t m_ulCtrlIndirectAim;			//�ϴε��ش��ָ��ʱ��
	uint32_t m_ulCtrlStartEnd;		//�ϴο�ʼ����ָ��ʱ��
	uint32_t m_ulCtrlSupplement;		//�ϴε�ҩ���ʲ���ָ��ʱ��
	uint32_t m_ulCtrlStartEndTime;		//�ϴ���ϰʱ��������ָ��ʱ��
	uint32_t m_ulCtrlBloodVolume;		//�ϴ�Ѫ������ָ��ʱ��
	uint32_t m_ulCtrlSync;		//�ϴ�����ͬ��ָ��ʱ��
	uint32_t m_ulCtrlTType;		//�ϴ��ն���������ָ��ʱ��
	uint32_t m_ulCtrlDayAndNight;		//�ϴ���ҹģʽ�л�ָ��
	uint32_t m_ulCtrlPowerOff;		//�ϴιػ�ָ��ʱ��
	uint32_t m_ulCtrlFSJ;		//�ϴη��������ָ��ʱ��
	uint32_t m_ulCtrlInOutNBCArea;		//���롢�뿪�˻�������ָ��ʱ��
	uint32_t m_ulCtrlSimulateAim;		//�ϴη�����ָ��ʱ��
	uint32_t m_ulCtrlHelicopterVoice;		//�ϴο��Ʒɻ���Чָ��ʱ��
	
	StructUTC m_BSNulCtrlManoeuvre;		//�ϴ���ϰ����ָ��ʱ��
	StructUTC m_sBSNulCtrlInterval;		//���
	StructUTC m_sBSNulCtrlSupplement;		//����
	StructUTC m_sBSNJudge;		//��̨�þ�ָ��
}BackStage_CmdTimeMag_t;

__packed typedef struct{
	uint8_t m_byA_Type;		//��ҩ����			
	uint8_t m_byA_Subtype;		//��ҩ������				
	uint8_t m_aRemainAmmuNum[3];		//��ҩʣ����						
	uint8_t m_aUsedAmmuNum[3];		//��ҩʣ����	
}Struct_AmmoMsg;		//��ҩ��Ϣ

__packed typedef struct
{
    u8 m_byW_Type;		//��������
    u8 m_byW_Subtype;		//����������
    u8 m_byW_RID;		//������λ����
    u8 m_aW_PID[2];		//�����豸���
    u8 m_byL_Character;		//����������� ENUM_LCHAR
    u8 m_byAmmuNum;		//Я�е�ҩ��������m��m=0 ʱ����ҩ�����������ݣ�
    BULLETINFO_T m_aAmmu[3];		//��ҩ��Ϣ
} Struct_Weapon;		//������Ϣ

__packed typedef struct{
	uint8_t m_aData_Frame[2];		//����֡ͷ��Ϊ 0xA8��0x92
	uint8_t m_byD_Len;		//����֡����
	uint8_t m_byD_Type;		//״̬������ţ�Ϊ 0x01;   
	uint8_t m_byT_Type;		//ʵ������
	uint8_t m_byT_Subtype;		//ʵ��������
	uint8_t m_byRID ;		//��λ����
	uint8_t m_aPID[2];		//�豸���
	uint8_t m_byKey;		//��Կ
	STRUCT_LPOS m_sL_Position;		//��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sTime;		//ʱ��
	uint8_t m_byC_Idegree;		//�ۺ�����	ENUM_CI_ARMOR  ENUM_CI_SOLDIER  ENUM_CI_OTHER
	STRUCT_TS_Character1 m_sTS_Character1;		//ʵ��״̬�� 1
	uint8_t m_byBP_Character;
	uint8_t m_byAmmuNum;		//Я�е�ҩ��������m��m=0 ʱ����ҩ�����������ݣ� 
	uint8_t m_bySupplyNum;		//Я��������������n��n=0 ʱ�����������������ݣ�
	Struct_AmmoMsg m_sAmmu[6];		//��ҩ��Ϣ
	Struct_Supply m_sSupply[6];		//������Ϣ
	
	uint8_t m_byXor;		//����
}Struct_UniversalStatusUpload;		//ͨ��״̬�ϴ�

__packed typedef struct
{
    uint8_t HeadFlg[2];
    uint8_t Blood;
    uint8_t IsNight;
    uint8_t InjuryCnt;
    uint8_t InjuryKind;
    uint8_t Decter[10 + 1];
    uint8_t m_byInjuryPart[11];		//������Ϣ
    uint8_t m_byWeaponNum;		//����Я������
    WeaponInfoBk WeaponInfo[WEAPONKIND_MAX];		//������Ϣ
    DRILL_PARAM DrillParam;
    uint16_t	Shell[PARTITION_MAX];
    uint8_t EP_Num[3];		//�������
    STRUCT_TIME1 m_sManoStart;		//��ϰ��ʼʱ��
    STRUCT_TIME1 m_sManoEnd;		//��ϰ��ʼʱ��
    Struct_Time2 m_sDayTime;
    Struct_Time2 m_sNightTime;
    uint16_t NoRspEvtNum;		//δ�ϴ��¼���
    uint16_t ES_Num;		//��ǰ�¼����
    uint8_t EvtRspFlg[30];		//�ϴ��¼���Ӧ��Ǻϳ�Ӫ�޸��¼�Ϊ240��
		uint8_t EvtRspFlgBak[30];
    uint8_t IsDrill;
    uint8_t Belong;
    uint8_t IsEnablePowerOff;
    uint8_t Key;
    uint8_t T_Subtype;
		uint32_t RptInterval;	//�ϱ����
    VIOLATIONS Vios;
    uint8_t XOR;
		
} PARAMATER_BACKUP;

__packed typedef struct{
	u8 m_byT_Type;		//ʵ������			
	u8 m_byT_Subtype;		//ʵ��������		
	u8 m_byRID;		//ʵ�嵥λ���� 
	u8 m_aPID[2];		//ʵ���豸���	
	STRUCT_TIME1 m_sRecCmdTime;		//����ָ��ʱ��
	STRUCT_TIME1 m_sStartTime;		//��ϰ��ʼʱ��
	u8 m_byST_Type;		//���÷�ʵ������			
	u8 m_byST_Subtype;		//���÷�ʵ��������		
	u8 m_byS_RID;		//���÷�ʵ�嵥λ���� 
	u8 m_aS_PID[2];		//���÷�ʵ���豸���	
	Struct_Time2 m_sDayStartTime;		//�л����������ʼʱ��
	Struct_Time2 m_sNightStartTime;		//�л�����ҹ����ʼʱ��
	u8 m_byKey;		//��Կ
	u8 m_byBR_Attribute;		//�������� ENUM_BRCHAR
	u8 m_aBulletMaxAmmuNum[3];		//ǹ�����Я������
	u8 m_aShellMaxAmmuNum[3];		//�ڵ����Я������
	u8 m_byAmmuNum;		//Я�е�ҩ��������m��m=0 ʱ����ҩ�����������ݣ� 
	u8 m_bySupplyNum;		//Я��������������n��n=0 ʱ�����������������ݣ�
	Struct_AmmoMsg m_sAmmu[6];		//��ҩ��Ϣ
	Struct_Supply m_sSupply[6];		//������Ϣ
	
	u8 m_byEP_Num[3];		//�������
	u8 m_byThickness;		//ʵ�������������Ч����װ�׺�Ȼ��������ȣ�;  0-19 	ȡֵ*1mm 	��Χ��0-19mm ;  20-255 	��ȡֵ-19��*10mm 	��Χ��20mm-2360mm 
}StructEvent_RSP_CtrlManoParam_NotIndividualSoldier;		//��ϰ�������óɹ��¼����ǵ�����(E_Type = EventType_RSP_CtrlManoParam_NotIndividualSoldier) 

__packed typedef struct{
	u8 m_byT_Type;						//������ʵ������			
	u8 m_byT_Subtype;					//������ʵ��������		
	u8 m_byRID;							//������ʵ�嵥λ���� 
	u8 m_aPID[2];						//������ʵ���豸���	
	STRUCT_TIME1 m_sRecCmdTime;		//����ָ��ʱ��
	STRUCT_TIME1 m_sStartTime;			//��ϰ��ʼʱ��
	STRUCT_TIME1 m_sEndTime;			//��ϰ����ʱ��
	u8 m_byST_Type;						//ʵʩ���÷�ʵ������			
	u8 m_byST_Subtype;					//ʵʩ���÷�ʵ��������		
	u8 m_byS_RID;						//ʵʩ���÷�ʵ�嵥λ���� 
	u8 m_aS_PID[2];					//ʵʩ���÷�ʵ���豸���	
	Struct_Time2 m_sDayStartTime;		//�л����������ʼʱ��
	Struct_Time2 m_sNightStartTime;	//�л�����ҹ����ʼʱ��
	u8 m_byKey;							//��Կ
	u8 m_byBR_Attribute;				//�������� ENUM_BRCHAR
	u8 m_aBulletMaxAmmuNum[3];			//ǹ�����Я������
	u8 m_aShellMaxAmmuNum[3];			//�ڵ����Я������
	u8 m_byWeaponNum;					//��������
	Struct_Weapon m_sWeapon[3];		//������Ϣ
	//Struct_Ammu m_sAmmu[6];			//��ҩ��Ϣ
	//Struct_Supply m_sSupply[6];		//������Ϣ
	
	u8 m_byEP_Num[3];						//�������
	u8 m_byThickness;					//ʵ�������������Ч����װ�׺�Ȼ��������ȣ�;  0-19 	ȡֵ*1mm 	��Χ��0-19mm ;  20-255 	��ȡֵ-19��*10mm 	��Χ��20mm-2360mm 
}StructEvent_RSP_CtrlManoParam_IndividualSoldier;				//��ϰ�������óɹ��¼���������(E_Type = EventType_RSP_CtrlManoParam_IndividualSoldier) 

__packed typedef struct{
	u8 m_byT_Type;						//������ʵ������			
	u8 m_byT_Subtype;					//������ʵ��������		
	u8 m_byRID;							//������ʵ�嵥λ���� 
	u8 m_aPID[2];						//������ʵ���豸���	
	STRUCT_TIME1 m_sRecCmdTime;		//����ָ��ʱ��
	u8 m_byST_Type;						//ʵʩ���÷�ʵ������			
	u8 m_byST_Subtype;					//ʵʩ���÷�ʵ��������		
	u8 m_byS_RID;						//ʵʩ���÷�ʵ�嵥λ���� 
	u8 m_aS_PID[2];					//ʵʩ���÷�ʵ���豸���	
	
	u8 m_bySelfStatusDataUploadCycle;	//ʵ������״̬�����ϴ����� 	��
	u8 m_byDynamometerDisplayStatusDataUploadCycle;		//ʵ���ը����ʾ��״̬�����ϴ�����  ��
	u8 m_byEquipmentAndIdentityDataUploadCycle;			//����Ա��װ������������������ϴ�����  ��
}StructEvent_RSP_CtrlInterval;				//�ϴ��������óɹ��¼� (E_Type = EventType_RSP_CtrlInterval) 

__packed typedef struct{
	u8 m_byT_Type;						//������ʵ������			
	u8 m_byT_Subtype;					//������ʵ��������		
	u8 m_byRID;							//������ʵ�嵥λ���� 
	u8 m_aPID[2];						//������ʵ���豸���	
	u8 m_byC_Idegree;					//�ۺ����� ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;		//��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sRecCmdTime;		//����ָ��ʱ��
	u8 m_byST_Type;						//����ҩ/���ʱ仯��ʵ������			
	u8 m_byST_Subtype;					//����ҩ/���ʱ仯��ʵ��������		
	u8 m_byS_RID;						//����ҩ/���ʱ仯��ʵ�嵥λ���� 
	u8 m_aS_PID[2];					//����ҩ/���ʱ仯��ʵ���豸���		
	u8 m_byIO_Idegree;					//��ҩ (����)������ʶ��	ENUM_IOCHAR
	u8 m_byChangeAmmuNum;				//�����仯��ҩ��������m��m=0 ʱ����ҩ�����������ݣ�
	u8 m_byChangeSuppleNum;				//�����仯������������n��n=0 ʱ�����������������ݣ�
	
	STRUCT_AMMUSUPPLY_t m_sChangeAmmu[6];		//��ҩ��Ϣ
	Struct_Supply m_sChangeSupply[6];	//������Ϣ
}StructEvent_RSP_CtrlSupplement;				//��ҩ�����ʣ��ַ�/�����¼�(E_Type = EventType_RSP_CtrlSupplement) 

__packed typedef struct{
	u8 m_byT_Type;							 //�����ʵ������			
	u8 m_byT_Subtype;						 //�����ʵ��������		
	u8 m_byRID;								 //�����ʵ�嵥λ���� 
	u8 m_aPID[2];							 //�����ʵ���豸���	
	u8 m_byC_Idegree;						 //�ۺ����� ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sAttackTime;			 //�����ָ��ʱ��
	u8 m_byFT_Type;							 //������ʵ������			
	u8 m_byFT_Subtype;						 //������ʵ��������		
	u8 m_byF_RID;							 //������ʵ�嵥λ���� 
	u8 m_aF_PID[2];						 //������ʵ���豸���		
	
	u8 m_byFA_Type;							 //��������ҩ����
	u8 m_byFA_Subtype;						 //��������ҩ������ 
	u8 m_byH_Part;							 //�����ʵ�����в�λ
	u8 m_byH_Type;							 //�����ʵ������ 
	u8 m_byH_Idegree;						 //�����ʵ����������
	
	STRUCT_LPOS m_sFL_Position ;			 //����������λ�� 

}StructEvent_DamageReport;				 //���������¼�(E_Type = EventType_DamageReport) 



__packed typedef struct{
	u8 m_byT_Type;							 //ʵ������			
	u8 m_byT_Subtype;						 //ʵ��������		
	u8 m_byRID;								 //��λ���� 
	u8 m_aPID[2];							 //ʵ���豸���	
	u8 m_byC_Idegree;						 //�ۺ����� ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sRecCmdTime;			 //����ָ��ʱ��
	u8 m_byFT_Type;							 //ʵʩ�þ���ʵ������			
	u8 m_byFT_Subtype;						 //ʵʩ�þ���ʵ��������		
	u8 m_byF_RID;							 //ʵʩ�þ���ʵ�嵥λ���� 
	u8 m_aF_PID[2];						 //ʵʩ�þ���ʵ���豸���		
	
	u8 m_byAC_Character;					 //�þ������� 	ENUM_ACCHAR					

}StructEvent_BNJudge;				     //���زþ��¼�(E_Type = EventType_Judge) 

__packed typedef struct{
	u8 m_byT_Type;							 //ʵ������			
	u8 m_byT_Subtype;						 //ʵ��������		
	u8 m_byRID;								 //��λ���� 
	u8 m_aPID[2];							 //ʵ���豸���	
	u8 m_byC_Idegree;						 //�ۺ����� ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sNCBTTime;				 //����/�뿪մȾ��ʱ��
	u8 m_byFT_Type;							 //���������������ʵ������			
	u8 m_byFT_Subtype;						 //���������������ʵ��������		
	u8 m_byF_RID;							 //���������������ʵ�嵥λ���� 
	u8 m_aF_PID[2];						 //���������������ʵ���豸���		
	u8 m_byNCBT_Type;						 //�˻�����ʶ�� 	ENUM_NCBTTYPE
	u8 m_byStrength ;						 //մȾ����вǿ��  	0-100 
	u8 m_byELR_Character ;					 //����/�뿪�˻���մȾ�����ʶ��  	ENUM_ELRCHAR

}StructEvent_NCBT;				         //����/�뿪�˻���մȾ���¼�(E_Type = EventType_NCBT) 

__packed typedef struct{
	u8 m_byT_Type;							 //ʵ������			
	u8 m_byT_Subtype;						 //ʵ��������		
	u8 m_byRID;								 //��λ���� 
	u8 m_aPID[2];							 //ʵ���豸���	
	u8 m_byC_Idegree;						 //�ۺ����� ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sNCBPTime;				 //ʵʩ�˻�������ʱ��
	u8 m_byNCBP_Type ;						 //����/�뿪�˻���մȾ�����ʶ��  	ENUM_NCBPTYPE

}StructEvent_NCB_Protection;			 //�˻��������¼�(E_Type = EventType_NCB_Protection) 

__packed typedef struct{
	u8 m_byT_Type;							 //������ʵ������			
	u8 m_byT_Subtype;						 //������ʵ��������		
	u8 m_byRID;								 //�����˵�λ���� 
	u8 m_aPID[2];							 //������ʵ���豸���	
	u8 m_byC_Idegree;						 //�ۺ����� ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //�����˵���λ�� ��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sNCBPTime;				 //ʵ�屻���˵�ʱ�� 
	u8 m_byFT_Type;							 //���������������ʵ������			
	u8 m_byFT_Subtype;						 //���������������ʵ��������		
	u8 m_byF_RID;							 //���������������ʵ�嵥λ���� 
	u8 m_aF_PID[2];						 //���������������ʵ���豸���		
	u8 m_byNCBT_Type;						 //�˻�����ʶ�� 	ENUM_NCBTTYPE
	
	u8 m_byH_Part;							 //���в�λ
	u8 m_byH_Type;							 //���� 
	u8 m_byH_Idegree;						 //��������

}StructEvent_NCB_Injury;				 //�˻��������¼�(EType = EventType_NCB_Injury) 

__packed typedef struct{
	u8 m_byT_Type;							 //ʵ������			
	u8 m_byT_Subtype;						 //ʵ��������		
	u8 m_byRID;								 //��λ���� 
	u8 m_aPID[2];							 //ʵ���豸���	
	u8 m_byC_Idegree;						 //�ۺ����� ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sPETTime;				 //�ܵ������вʱ��
	u8 m_byFT_Type;							 //��в��ʵ������			
	u8 m_byFT_Subtype;						 //��в��ʵ��������		
	u8 m_byF_RID;							 //��в��ʵ�嵥λ���� 
	u8 m_aF_PID[2];						 //��в��ʵ���豸���		
	STRUCT_LPOS m_sFL_Position ;			 //��в����γ�� �߶� ��λ״̬
	u8 m_byLT_Type;							 //�����в���� 	ENUM_LTTYPE
	u8 m_byDirection ;						 //�����в����Դ���� ENUM_DIR

}StructEvent_PE_Threat;				     //�����в�¼�(E_Type = EventType_PE_Threat) 


__packed typedef struct{
	u8 m_byT_Type;							 //ʵ������			
	u8 m_byT_Subtype;						 //ʵ��������		
	u8 m_byRID;								 //��λ���� 
	u8 m_aPID[2];							 //ʵ���豸���	
	u8 m_byC_Idegree;						 //�ۺ����� ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sTime;					 //ʱ��
	u8 m_bySIP_Type;						 //�������¼������ͣ�ʹ���յ�������з���ʱ�������ֶ�����Ϊ 0   ENUM_SIPTYPE
	u8 m_byFA_Type;							 //����ʹ�õ�ҩ����
	u8 m_byFA_Subtype;						 //����ʹ�õ�ҩ������
	u8 m_byFAmmuNum;						 //����ʹ�õ�ҩ����
}StructEvent_PE_Protection;			 	 //����/�յ������¼�(E_Type = EventType_PE_Protection) 

__packed typedef struct{
	u8 m_byT_Type;							 //�����ڵ���ʵ������			
	u8 m_byT_Subtype;						 //�����ڵ���ʵ��������		
	u8 m_byRID;								 //�����ڵ��ĵ�λ���� 
	u8 m_aPID[2];							 //�����ڵ���ʵ���豸���	
	u8 m_byC_Idegree;						 //�����ڵ����ۺ����� ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //�����ڵ��� ��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sTime;					 //�����ڵ���ʱ��
	
	u8 m_byA_Type;							 //���䵯ҩ����
	u8 m_byA_Subtype;						 //���䵯ҩ������
	u8 m_byG_Num;							 //װҩ��  ENUM_GNUM
	signed char m_byAmmunitionTemperature;	 //ҩ�� -80-+80
	u8 m_byTSpan_Select; 					 //���鵯ҩ��������ʱʱ��ѡ��  ENUM_FUSE_TYPE
	u8 m_T_SpanX;                            //���鵯ҩ��������ʱʱ��
	u8 m_byVelocity2; 						 //����ƫ��  0-255 ��λ��0.1 ��/��
	u8 m_byA_AmmuNum;						 //���䵯ҩ����
	u8 m_byR_Mode ;							 //���ģʽ 
	STRUCT_GPOS m_sG_Position ;				 //Ŀ����ظ�˹���� 
	
}StructEvent_DigitIndirectLanuch;		 //���ֻ����ڷ����¼�(E_Type = EventType_DigitIndirectLanuch) 


__packed typedef struct{
	u8 m_byT_Type;							 //�����ڵ���ʵ������			
	u8 m_byT_Subtype;						 //�����ڵ���ʵ��������		
	u8 m_byRID;								 //�����ڵ��ĵ�λ���� 
	u8 m_aPID[2];							 //�����ڵ���ʵ���豸���	
	u8 m_byC_Idegree;						 //�����ڵ����ۺ����� ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //�����ڵ��� ��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sTime;					 //�����ڵ���ʱ��
	
	u8 m_byA_Type;							 //���䵯ҩ����
	u8 m_byA_Subtype;						 //���䵯ҩ������
	u8 m_byG_Num;							 //װҩ��  ENUM_GNUM
	signed char m_byAmmunitionTemperature;	 //ҩ�� -80-+80
	u8 m_byIndirectTSpan_Select; 			 //���鵯ҩ��������ʱʱ��ѡ��  ENUM_FUSE_TYPE
	u8 m_byIndirectT_Span; 					 //���鵯ҩ��������ʱʱ��     0-9 	ȡֵ*0.1 �� 	��Χ��0.1 ��-0.9 �� ; 10-69 ��ȡֵ-9��*1 �� 	��Χ��1 ��-60 �� 
	u8 m_byVelocity2; 						 //����ƫ��  0-255 ��λ��0.1 ��/��
	
	u8 m_byA_AmmuNum;						 //���䵯ҩ����
	u8 m_aArtilleryShootAngle[2];			 //������� 0-6000 
	u8 m_aArtilleryShootdirection[2];		 //�������� 0-6000 
	u8 m_aGroundAirPressure[2];			 //��ص�����ѹ
	u8 m_aGroundWindDirection[2];			 //��ص������ -360?360
	u8 m_aGroundWindVelocity1;				//��ص������
	signed char m_byGroundTemperature;	     //������� -80-+80
	
}StructEvent_NonDigitIndirectLanuch;	 //�����ֻ����ڷ����¼�(E_Type = EventType_NonDigitIndirectLanuch) 


__packed typedef struct{
	u8 m_byT_Type;							 //��λ���У���״�ʵ������			
	u8 m_byT_Subtype;						 //��λ���У���״�ʵ��������		
	u8 m_byRID;								 //��λ���У���״ﵥλ���� 
	u8 m_aPID[2];							 //��λ���У���״�ʵ���豸���	
	u8 m_byC_Idegree;						 //�ۺ����� ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //��ʼ��ֹͣ���/У�����ʱ�ĵ���λ�� ��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sTime;					 //��ʼ��ֹͣ���/У�������ʱ��
	u8 m_bySAM_Character;					 //���/У��ģʽ��ʶ��  ENUM_SAMCHAR
	u8 m_bySS_Character;					 //���/У�俪�ز���  ENUM_SSCHAR
	u8 m_aRadarAzimuth[2];					 //�״﷽λ��  0-6000 
	u8 m_aRadarPitchAngle[2];				 //�״︩����  0-6000 
	
}StructEvent_Radar;	 //��λ���У���״����/У���¼�(E_Type = EventType_Radar) 

__packed typedef struct{
	u8 m_byT_Type;							 //ʵ������			
	u8 m_byT_Subtype;						 //ʵ��������		
	u8 m_byRID;								 //��λ���� 
	u8 m_aPID[2];							 //ʵ���豸���	
	u8 m_byC_Idegree;						 //�ۺ����� ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //���䵯ҩʱ�ĵ���λ�� ��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sTime;					 //���䵯ҩ��ʱ��
	u8 m_byA_Type;							 //���䵯ҩ����
	u8 m_byA_Subtype;						 //���䵯ҩ������
	
}StructEvent_HeavyWpnLaunch;			 //ֱ�������������¼�(E_Type = EventType_HeavyWpnLaunch) 


__packed typedef struct{
	u8 m_byT_Type;							 //ʵ������			
	u8 m_byT_Subtype;						 //ʵ��������		
	u8 m_byRID;								 //��λ���� 
	u8 m_aPID[2];							 //ʵ���豸���	
	u8 m_byC_Idegree;						 //�ۺ����� ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //����ʱ�ĵ���λ�� ��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sTime;					 //���׵�ʱ��
	u8 m_byMT_Type;							 //����ʵ������			
	u8 m_byMT_Subtype;						 //����ʵ��������		
	u8 m_byM_RID;							 //���׵�λ���� 
	u8 m_aM_PID[2];						 //����ʵ���豸���	
}StructEvent_IndLayMine;				 //���������¼�(E_Type = EventType_IndLayMine) 

__packed typedef struct{
	u8 m_byT_Type;							 //ʵ������			
	u8 m_byT_Subtype;						 //ʵ��������		
	u8 m_byRID;								 //��λ���� 
	u8 m_aPID[2];							 //ʵ���豸���	
	u8 m_byC_Idegree;						 //�ۺ����� ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //����ʱ�ĵ���λ�� ��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sTime;					 //���׵�ʱ��
	u8 m_byMT_Type;							 //����ʵ������			
	u8 m_byMT_Subtype;						 //����ʵ��������		
	u8 m_byM_RID;							 //���׵�λ���� 
	u8 m_aM_PID[2];						 //����ʵ���豸���	
}StructEvent_IndCleanMine;				 //���������¼�(E_Type = EventType_IndCleanMine) 


__packed typedef struct{
	u8 m_byT_Type;							 //������׳�ʵ������			
	u8 m_byT_Subtype;						 //������׳�ʵ��������		
	u8 m_byRID;								 //������׳���λ���� 
	u8 m_aPID[2];							 //������׳�ʵ���豸���	
	u8 m_byC_Idegree;						 //������׳��ۺ����� ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //���������׵�ʱ�ĵ���λ�� ��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sTime;					 //���������׵���ʱ��
	u8 m_byA_Type;							 //���䵯ҩ����		
	u8 m_byA_Subtype;						 //���䵯ҩ������
	u8 m_aRocketLayMineDirection[2];		 //���������׵������� 0-6000 
	u8 m_aRocketLayMineAngle[2];			 //���������׵������ 0-6000 
	u8 m_byRocketLayMineAmmuNum;			 //���η��������׵����� 
	u8 m_byOpenPositionT_Pick;				 //����������ʱʱ��ѡ��  ENUM_FUSE_TYPE
	u8 m_byOpenPositionT_Span; 				 //����������ʱʱ��     0-9 	ȡֵ*0.1 �� 	��Χ��0.1 ��-0.9 �� ; 10-69 ��ȡֵ-9��*1 �� 	��Χ��1 ��-60 �� 
	u8 m_byMineDetonationT_Span; 			 //�����Ի�ʱ��      0-9 	ȡֵ*0.1 �� 	��Χ��0.1 ��-0.9 �� ; 10-69 ��ȡֵ-9��*1 �� 	��Χ��1 ��-60 �� 
}StructEvent_RocketLayMine;				 //��������¼�(E_Type = EventType_RocketLayMine) 


__packed typedef struct{
	u8 m_byT_Type;							 //ɨ�׳�ʵ������			
	u8 m_byT_Subtype;						 //ɨ�׳�ʵ��������		
	u8 m_byRID;								 //ɨ�׳���λ���� 
	u8 m_aPID[2];							 //ɨ�׳�ʵ���豸���	
	u8 m_byC_Idegree;						 //ɨ�׳��ۺ�����  ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //ʵʩɨ�ײ����ĵ���λ�� ��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sTime;					 //ʵʩɨ�ײ�����ʱ��
	u8 m_byMCM_Character;					 //ɨ�׷�ʽ��ʶ��  ENUM_MCMCHAR
	u8 m_bySS_Character;					 //ɨ�׿��ز���  ENUM_SSCHAR
}StructEvent_CarCleanMine;				 //ɨ�׳�ɨ���¼�(E_Type = EventType_CarCleanMine) 

__packed typedef struct{
	u8 m_byT_Type;							 //���ϳ�ʵ������			
	u8 m_byT_Subtype;						 //���ϳ�ʵ��������		
	u8 m_byRID;								 //���ϳ���λ���� 
	u8 m_aPID[2];							 //���ϳ�ʵ���豸���	
	u8 m_byC_Idegree;						 //���ϳ��ۺ�����  ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //���ϳ��ĵ���λ�� ��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sTime;					 //����ʱ�� 
	u8 m_byA_Type;							 //���䵯ҩ����		
	u8 m_byA_Subtype;						 //���䵯ҩ������
	u8 m_aRocketCleanMineDirection[2];		 //���������Ƶ������� 0-6000 
	u8 m_aRocketCleanMineAngle[2];			 //���������Ƶ������ 0-6000 
	u8 m_byRocketCleanMineAmmuNum;			 //���η��������Ƶ����� 
}StructEvent_RocketCleanMine;			 //���ɨ���¼�(E_Type = EventType_RocketCleanMine) 


__packed typedef struct{
	u8 m_byT_Type;							 //ʵ������			
	u8 m_byT_Subtype;						 //ʵ��������		
	u8 m_byRID;								 //��λ���� 
	u8 m_aPID[2];							 //ʵ���豸���	
	u8 m_byC_Idegree;						 //�񻯺��ۺ�����  ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS L_m_sPosition ;			 //���ƶ񻯵ĵ���λ�� ��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sTime;					 //���ƶ�ʱ�� 
}StructEvent_InjuryWorse;				 //��Ա���ƶ��¼�(E_Type = EventType_InjuryWorse) 

__packed typedef struct{
	u8 m_byT_Type;							 //��Աʵ������			
	u8 m_byT_Subtype;						 //��Աʵ��������		
	u8 m_byRID;								 //��Ա��λ���� 
	u8 m_aPID[2];							 //��Աʵ���豸���	
	u8 m_byC_Idegree;						 //���Ȼ����ۺ����� ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //���Ȼ�ʱ����λ�� ��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sRescueStartTime;		 //�Ȼ���ʼ��ʱ�� 
	STRUCT_TIME1 m_sRescueEndTime;			 //�Ȼ�������ʱ�� 
	u8 m_byRT_Type;							 //ʩ����Աʵ������			
	u8 m_byRT_Subtype;						 //ʩ����Աʵ��������		
	u8 m_byR_RID;							 //ʩ����Ա��λ���� 
	u8 m_aR_PID[2];						 //ʩ����Աʵ���豸���	
	u8 m_byCM_Characte;						 //���η�ʽ��ʶ��	ENUM_CMCHAR
	u8 m_byCE_Characte;						 //��Ա���ν����ʶ��	ENUM_CECHAR
	
}StructEvent_IndRescued;				 //�����Ȼ��¼�(E_Type = EventType_IndRescued) 


__packed typedef struct{
	u8 m_byT_Type;							 //ս���ϰ�ʵ������			
	u8 m_byT_Subtype;						 //ս���ϰ�ʵ��������		
	u8 m_byRID;								 //ս���ϰ���λ���� 
	u8 m_aPID[2];							 //ս���ϰ�ʵ���豸���	
	u8 m_bySlaveNum;						 //ս���ϰ��ӻ�������n��n=0 ʱ���ӻ�����λ���������ݣ� 
	STRUCT_LPOS m_sL_Position ;			 //ս���ϰ���������λ�� ��γ�� �߶� ��λ״̬
	STRUCT_LPOS m_sSL_Position[3] ;		 //ս���ϰ��ӻ�����λ�� ��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sArrangeHinderTime;		 //ս���ϰ��������ʱ��  
}StructEvent_ArrangeHinder;				 //ս���ϰ������¼�(E_Type = EventType_ArrangeHinder) 


__packed typedef struct{
	u8 m_byT_Type;							 //������ʵ������			
	u8 m_byT_Subtype;						 //������ʵ��������		
	u8 m_byRID;								 //�����㵥λ���� 
	u8 m_aPID[2];							 //������ʵ���豸���	
	u8 m_byC_Idegree;						 //�������ۺ����� ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //���������λ�� ��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sCtrlFirePointTime;		 //�������������ʱ��
	STRUCT_TIME1 m_sFireStartTime;	 		 //�����㿪ʼɨ���ʱ�䣨����ֵȫΪ 0 ʱ��������ͨ���ֶ����䣬���䵯ҩ��� 0.2 �룬����ʱ�� 2 �룩
	STRUCT_TIME1 m_sFireStopTime;	 		 //�����㿪ʼֹͣ��ʱ�䣨����ֵȫΪ 0 ʱ��������ͨ���ֶ����䣬���䵯ҩ��� 0.2 �룬����ʱ�� 2 �룩
	u8 m_byFireInterval;					 //�����㷢�䵯ҩʱ����   10 ����
	u8 m_byTurnFireInterval;				 //����������������ʱ��   0-9 	ȡֵ*0.1 �� 	��Χ��0.1 ��-0.9 �� ;	10-69 	��ȡֵ-9��*1 �� 	��Χ��1 ��-60 �� 
	u8 m_byA_Type;							 //���䵯ҩ����		
	u8 m_byA_Subtype;						 //���䵯ҩ������
	
}StructEvent_RSP_CtrlFirePoint;				 //�����������¼�(E_Type = EventType_RSP_CtrlFirePoint) 

__packed typedef struct{
	u8 m_byT_Type;							 //ʵ������			
	u8 m_byT_Subtype;						 //ʵ��������		
	u8 m_byRID;								 //��λ���� 
	u8 m_aPID[2];							 //ʵ���豸���	
	STRUCT_LPOS m_sL_Position ;			 //����λ�� ��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sTime;					 //ʱ�� 
}StructEvent_StatusRecovery;			 //״̬�Զ��ָ��¼�(E_Type = EventType_StatusRecovery) 

__packed typedef struct{
	u8 m_byT_Type;							 //������쳵ʵ������			
	u8 m_byT_Subtype;						 //������쳵ʵ��������		
	u8 m_byRID;								 //������쳵��λ���� 
	u8 m_aPID[2];							 //������쳵ʵ���豸���	
	u8 m_byC_Idegree;						 //������쳵�ۺ����� ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //������쳵����λ�� ��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sTime;					 //ʱ��
	u8 m_byMT_Subtype;						 //�׳�����
	STRUCT_LPOS m_sML_Position[3] ;		 //�׳�����λ��
	
}StructEvent_RSP_CtrlEngineerRecon;		 //��������¼�(E_Type = EventType_RSP_CtrlEngineerRecon) 

__packed typedef struct{
	u8 m_byT_Type;							 //����ʵ������			
	u8 m_byT_Subtype;						 //����ʵ��������		
	u8 m_byRID;								 //������λ���� 
	u8 m_aPID[2];							 //����ʵ���豸���	
	u8 m_byC_Idegree;						 //�����ۺ����� ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //ʵʩ���޵���λ�� ��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sRepairStartTime;		 //���޿�ʼ��ʱ�� 
	STRUCT_TIME1 m_sRepairEndTime;			 //���޽�����ʱ�� 
	u8 m_byET_Type;							 //���޳�ʵ������			
	u8 m_byET_Subtype;						 //���޳�ʵ��������		
	u8 m_byE_RID;							 //���޳���λ���� 
	u8 m_aE_PID[2];						 //���޳�ʵ���豸���	
	
}StructEvent_EquipRepair;				 //װ�������¼�(E_Type = EventType_EquipRepair) 

__packed typedef struct{
	u8 m_byT_Type;							 //ʵ������			
	u8 m_byT_Subtype;						 //ʵ��������		
	u8 m_byRID;								 //��λ���� 
	u8 m_aPID[2];							 //ʵ���豸���	
//	u8 m_byC_Idegree;						 //�ۺ����� ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //ģʽת��ʱ�ĵ���λ��  ��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sChangeToManoTime;		 //ģʽת����ʱ��
	
	u8 m_byCT_Type;							 //ʵʩģʽת��ʵ�������			
	u8 m_byCT_Subtype;						 //ʵʩģʽת��ʵ���������	
	u8 m_byC_RID;							 //ʵʩģʽת��ʵ��ĵ�λ����  
	u8 m_aC_PID[2];						     //ʵʩģʽת��ʵ����豸���
  STRUCT_TS_Character1 m_sTS_Character1;  //ʵ��״̬�� 1
	
}StructEvent_ChangeToMano;				 //��ϰģʽת���¼�(E_Type = EventType_ChangeToMano) 

__packed typedef struct{
	u8 m_byT_Type;							 //��Աʵ������			
	u8 m_byT_Subtype;						 //��Աʵ��������		
	u8 m_byRID;								 //��Ա��λ���� 
	u8 m_aPID[2];							 //��Աʵ���豸���	
	STRUCT_LPOS m_sL_Position ;			 //��Ա����λ��  ��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sWeaponOnlineTime;		 //������ʱ��
	u8 m_byCT_Type;							 //������ʵ������		
	u8 m_byCT_Subtype;						 //������ʵ��������
	u8 m_byC_RID;							 //��������λ����  
	u8 m_aC_PID[2];						 //�������豸���
	
}StructEvent_IndWeaponOnline;			 //�����������¼�(E_Type = EventType_IndWeaponOnline) 


__packed typedef struct{
	u8 m_byT_Type;							 //ʵ������			
	u8 m_byT_Subtype;						 //ʵ��������		
	u8 m_byRID;								 //��λ���� 
	u8 m_aPID[2];							 //ʵ���豸���	
	u8 m_byC_Idegree;						 //�ۺ����� ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //����λ��  ��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sOnOffTime;		 		 //����/�ػ�ʱ��
	u8 m_byPO_Character ;					 //����/�ػ�������ʶ��		 ENUM_POCHAR
	
}StructEvent_TerminalOnOff;				 //���ػ��¼�(E_Type = EventType_TerminalOnOff) 

__packed typedef struct{
	u8 m_byT_Type;							 //ʵ������			
	u8 m_byT_Subtype;						 //ʵ��������		
	u8 m_byRID;								 //��λ���� 
	u8 m_aPID[2];							 //ʵ���豸���	
	u8 m_byC_Idegree;						 //�ۺ����� ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //����λ��  ��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sTime;		 		 	 //ʱ��
	u8 m_byIE_Type ;					 	 //Υ���¼�����		 ENUM_IE_TYPE
	
}StructEvent_AgaistRuleStart;			 //Υ�濪ʼ�¼�(E_Type = EventType_AgaistRuleStart) 

__packed typedef struct{
	u8 m_byT_Type;							 //ʵ������			
	u8 m_byT_Subtype;						 //ʵ��������		
	u8 m_byRID;								 //��λ���� 
	u8 m_aPID[2];							 //ʵ���豸���	
	u8 m_byC_Idegree;						 //�ۺ����� ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //����λ��  ��γ�� �߶� ��λ״̬
	STRUCT_TIME1 m_sStartTime;	 			 //��ʼʱ��
	STRUCT_TIME1 m_sStopTime;	 			 //����ʱ��
	u8 m_byIE_Type ;					 	 //Υ���¼�����		 ENUM_IE_TYPE	
}StructEvent_AgaistRuleStop;			 //Υ������¼�(E_Type = EventType_AgaistRuleStop) 

__packed typedef struct
{
    u8 m_aData_Frame[2];			//����֡ͷ��Ϊ 0xA8��0x93
    u8 m_byD_Len;					//����֡����

    __packed union
    {
        StructEvent_RSP_CtrlManoParam_NotIndividualSoldier m_sRSP_CtrlManoParam_NotIndividualSoldier;	//��ϰ�������óɹ��¼����ǵ����� EventType_RSP_CtrlManoParam_NotIndividualSoldier
        StructEvent_RSP_CtrlManoParam_IndividualSoldier m_sRSP_CtrlManoParam_IndividualSoldier;		//��ϰ�������óɹ��¼���������	EventType_RSP_CtrlManoParam_IndividualSoldier
        StructEvent_RSP_CtrlInterval m_sRSP_CtrlInterval;												//�ϴ��������óɹ��¼�	EventType_RSP_CtrlInterval
        StructEvent_RSP_CtrlSupplement m_sRSP_CtrlSupplement;											//��ҩ�����ʣ��ַ�/�����¼�  EventType_RSP_CtrlSupplement
        StructEvent_DamageReport m_sDamageReport;														//���������¼�  EventType_DamageReport
        StructEvent_BNJudge m_sJudge;																	//���زþ��¼�  EventType_Judge
        StructEvent_NCBT m_sNCBT;																		//����/�뿪�˻���մȾ���¼�  EventType_NCBT
        StructEvent_NCB_Protection m_sNCB_Protection;													//�˻��������¼�  EventType_NCB_Protection
        StructEvent_NCB_Injury m_sNCB_Injury;															//�˻��������¼�  EventType_NCB_Injury
        StructEvent_PE_Threat m_sPE_Threat;															//�����в�¼�  EventType_PE_Threat
        StructEvent_PE_Protection m_sPE_Protection;													//����/�յ������¼�  EventType_PE_Protection
        StructEvent_DigitIndirectLanuch m_sDigitIndirectLanuch;										//���ֻ����ڷ����¼�  EventType_DigitIndirectLanuch
        StructEvent_NonDigitIndirectLanuch m_sNonDigitIndirectLanuch;									//�����ֻ����ڷ����¼�  EventType_NonDigitIndirectLanuch
        StructEvent_Radar m_sRadar;																	//��λ���У���״����/У���¼�  EventType_Radar
        StructEvent_HeavyWpnLaunch m_sHeavyWpnLaunch;													//ֱ�������������¼�  EventType_HeavyWpnLaunch
        StructEvent_IndLayMine m_sIndLayMine;															//���������¼�  EventType_IndLayMine
        StructEvent_IndCleanMine m_sIndCleanMine;														//���������¼�  EventType_IndCleanMine
        StructEvent_RocketLayMine m_sRocketLayMine;												    //��������¼�  EventType_RocketLayMine
        StructEvent_CarCleanMine m_sCarCleanMine;													    //ɨ�׳�ɨ���¼�  EventType_CarCleanMine
        StructEvent_RocketCleanMine m_sRocketCleanMine;												//���ɨ���¼�  EventType_RocketCleanMine
        StructEvent_InjuryWorse m_sInjuryWorse;														//��Ա���ƶ��¼�  EventType_InjuryWorse
        StructEvent_IndRescued m_sIndRescued;												    		//�����Ȼ��¼�  StructEvent_IndRescued
        StructEvent_ArrangeHinder m_sArrangeHinder;													//ս���ϰ������¼�  EventType_ArrangeHinder
        StructEvent_RSP_CtrlFirePoint m_sRSP_CtrlFirePoint;											//�����������¼�  EventType_RSP_CtrlFirePoint
        StructEvent_StatusRecovery m_sStatusRecovery;													//״̬�Զ��ָ��¼�  EventType_StatusRecovery
        StructEvent_RSP_CtrlEngineerRecon m_sRSP_CtrlEngineerRecon;									//��������¼�  EventType_RSP_CtrlEngineerRecon
        StructEvent_EquipRepair m_sEquipRepair;														//װ�������¼�   EventType_EquipRepair
        StructEvent_ChangeToMano m_sChangeToMano;														//��ϰģʽת���¼�   EventType_ChangeToMano
        StructEvent_IndWeaponOnline m_sIndWeaponOnline;												//�����������¼�   EventType_IndWeaponOnline
        StructEvent_TerminalOnOff m_sTerminalOnOff;													//���ػ��¼�   EventType_TerminalOnOff
        StructEvent_AgaistRuleStart m_sAgaistRuleStart;												//Υ�濪ʼ�¼�   EventType_AgaistRuleStart
        StructEvent_AgaistRuleStop m_sAgaistRuleStop;													//Υ������¼�   EventType_AgaistRuleStop

        u8 m_aBuffer[120];
    } Event_Pack;				//�¼����ݣ����� n = D_Len��
    u8 m_aES_Num[2];				//�¼�����֡���к�
    u8 m_byXor  ;					//����
} Struct_EventUpload;			//�¼��ϴ�

typedef struct
{
    u8 HelicopterVoiceFlg: 1;
    u8 AeroplaneVoiceFlg: 1;
    u8 IndirectGunVoiceFlg: 1;
    u8 res: 5;

    u8 HelicopterVoiceCnt;
    u8 AeroplaneVoiceCnt;
    u8 IndirectGunVoiceCnt;
} SOUND_SIMU_CTRL;	//ģ����Ч

typedef struct
{

    RESCOM_LOWLEVEL_MAG *m_pLLMag;

    uint32_t 	m_bIsInit: 1;   //�����ʼ��
    uint32_t 	m_bIsRecvNew: 1; //���յ�������
    uint32_t 	m_bRes1: 30;

    SCOMTASK_MAG m_sTaskMag;

    uint8_t m_IsAccess: 1;		//�������
		uint8_t m_ManoParamFlg:1;
    uint8_t m_byRes1: 6;		//����
		uint8_t m_byNuclearFlg;     //����в��־
    uint8_t m_byBiologyFlg;     //������в��־
    uint8_t m_byChemistryFlg;   //��ѧ��в��־
    uint8_t NBC_Strength;
	
		DRILL_PARAM DrillParam;		//��ϰ���ò���

		uint8_t EvtListResPonseNum[240];		//�¼������յ��ظ�
		uint8_t EvtRspFlg[30];		//�ϴ��¼���Ӧ��Ǹ�Ϊ30��
		uint8_t EvtRspFlgBak[30];
		uint16_t UnUpLoadEvtCnt;

		Struct_EventUpload m_sEvtUploadMag;
						
		SOUND_SIMU_CTRL SoundSimulation;
		
		uint32_t m_dwCtrlEvtNum;		//�����¼����
		
		uint32_t m_dwTimer_Connect_Request;
		uint32_t m_dwTimer_Upload_SelfState;
		uint32_t m_dwHardStatusCheckTime;
		uint32_t m_dwTimer_Connect_Backstage;
		uint32_t m_dwTimer_Connect_GPS;
		uint32_t m_dwRptInterval;
					
		VIOLATIONS Vios;
		uint32_t DetectorLossStartTime;
    uint32_t DetectorLossEndTime;
    STRUCT_TIME1 DetectorLossTimeStamp;
    uint32_t FumeInstallStartTime;
    uint32_t FumeInstallEndTime;
    STRUCT_TIME1 FumeInstallTimeStamp;
    uint32_t IllegalMoveStartTime;
    uint32_t IllegalMoveEndTime;
    STRUCT_TIME1 IllegalMoveTimeStamp;
    uint32_t HelmetLossStartTime;
    uint32_t HelmetLossEndTime;
    STRUCT_TIME1 HelmetLossTimeStamp;
		
		uint8_t HelmetLossFlg:1;
		uint8_t StartLoadFlag:1;
	  uint8_t FourLoadFlag:1;
		uint8_t m_byRes2: 5;		//����
		
		NMEA_MAG m_sNmeaMag;
		FOURGMAG m_sFourGMag;
		
		PARAMATER_BACKUP BackupArea[2];
		BackStage_CmdTimeMag_t m_sCmdTime;
} RESCOMTASK_MAG;

extern RESCOMTASK_MAG g_sResComTask_Mag;

typedef struct
{
	uint8_t		m_byHead;		//֡ͷ���̶�Ϊ0xA8��
	uint8_t		m_byCmd;		//�����루�̶�Ϊ Cmd_Terminal_AccessRequest��
	uint8_t 	m_byD_Len;		//����֡���� 
	uint8_t 	m_byT_Type;		//ʵ������
	uint8_t 	m_byT_Subtype;	//ʵ�������� 
	uint8_t 	m_byRID;		//��λ���� 
	uint16_t 	m_wPID;			//�豸���(���ģʽ)
	uint8_t 	m_byXor;		//����֡����
}StructBNTerminal_AccessRequest;

uint8_t GetInjuryDegree(uint8_t Blood);
uint8_t GetInjuryBloodByHurtDegree(ENUM_CI_INDIVIDUAL InjuryDegree);
/****************************************************************
@FUNCTION����ȡ�����˿����������
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: 

****************************************************************/
RESCOMTASK_MAG *ResComTask_HandleGet(void);
SCOMTASK_MAG *ResComTask_TaskHandleGet(void);

/****************************************************************
@FUNCTION���������ڹ��ܳ�ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sResComMag

****************************************************************/
void ResComTask_Init(void);

/****************************************************************
@FUNCTION���������ڹ��ܷ���ʼ��
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sResComMag

****************************************************************/
void ResComTask_DeInit(void);

/****************************************************************
@FUNCTION���������ڹ�������������
@INPUT�� ��
@OUTPUT: ��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sResComMag

****************************************************************/
void ResComTask_Process(void);

void RecordRcvBackStageProtocolTime(StructUTC * Time);
uint8_t BNSoldierManoParamSetHandle(Struct_CtrlManoParam_IndividualSoldierDownload ManoParam);
void CtrlManoParamEventUploadHandle(Struct_CtrlManoParam_IndividualSoldierDownload *ManoParam);
//���������¼��ϴ�
void PersonnelLayMineEventUpload_Send(Struct_EventUpload *pMag);
//����
void PersonnelClearMineEventUpload_Send(Struct_EventUpload *pMag);
void ResComTask_SendToUpcom(uint8_t *pBuf, uint16_t BufLen, uint8_t DevIdx, DEVCTRL eDevCtl);
void BSN_RescureEvtHandle(StructEvent_IndRescued *pMag);

void CtrlPJPShotEventUploadHandle(STRUCT_Various_Elements *pMag);
#endif



