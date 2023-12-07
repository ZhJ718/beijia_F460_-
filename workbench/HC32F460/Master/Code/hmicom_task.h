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
#define HMI_COM_FRAME_LEN_MAX 512  //最大支持的报文长度


#define HMI_INJURYLEN 11

__packed typedef struct
{
    uint8_t m_byRcdCnt;
    STRUCT_TIME1 m_sUtc;
    uint8_t m_byDamageKind;		//伤类ENUM_DAMAGE_KIND
    uint8_t m_byPart;		//伤部ENUM_PARTITION
    uint8_t	m_cBlood;		//毁伤
    uint8_t	m_byResidueBlood;	//剩余血量
    uint8_t m_byFRid;		//打击方RID
    uint16_t m_wFPid;		//打击方PID
    uint8_t  m_byUniID;		//打击方武器弹药类型
    uint8_t  m_Ttype;		//武器类型
    uint8_t  m_Tsubtype;		//武器子类型
    uint8_t  m_byChkSum;		//校验和
	
} INJURY_MAG;

extern INJURY_MAG g_sInjuryMag;


typedef struct
{

    HMI268B_LOWLEVEL_MAG *m_pLLMag;

    uint32_t 	m_bIsInit: 1;		//任务初始化
    uint32_t 	m_bIsOnline: 1;		//接收到新数据
    uint32_t 	m_bRes: 30;

    SCOMTASK_MAG m_sTaskMag;
		
		STRUCT_LPOS m_sLastPos;		//上一次的gps信息
		double m_Jingdu;		//要显示的经度
    double m_Weidu;		//要显示的纬度
	
		uint8_t  m_byHurtKind;		//伤类
		uint8_t  m_byLastMode;		//上一次的模式
    uint8_t  m_byLastIsNight;		//上一次的昼夜模式
    uint16_t m_wLastPID;		//上一次主体PID
    uint8_t  m_byLastBelong;		//上一次归属方：蓝=0，红=1，黄=2，白=3
    uint8_t  m_byLastBlood;		//上一次血量
		uint8_t  m_HurtKind;
		INJURY_MAG m_sInjury[11];		//伤情信息
		
		uint8_t  m_byHurtNextFlg;		//请求伤情信息 下一条标
    uint8_t  m_byHurtCountFlg;		//记录请求伤情的次数
    uint8_t  m_byHurtfinishFlg;		//伤情信息求取完成的标志
    uint8_t  m_byHurtNone;		//请求伤情第一个是0xff的标志
		
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
@FUNCTION：获取HMI端口任务管理句柄
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 

****************************************************************/
HMICOMTASK_MAG *HmiComTask_HandleGet(void);

/****************************************************************
@FUNCTION：获取HMI端口任务管理句柄
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 

****************************************************************/
SCOMTASK_MAG *HmiComTask_TaskHandleGet(void);


/****************************************************************
@FUNCTION：HMI 串口功能初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:

****************************************************************/
void HmiComTask_Init(void);


/****************************************************************
@FUNCTION：HMI 串口功能反初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:

****************************************************************/
void HmiComTask_DeInit(void);

/****************************************************************
@FUNCTION：HMI 串口功能任务主进程
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:

****************************************************************/
void HmiComTask_Process(void);

/****************************************************************
@FUNCTION：HMI 串口功能定时任务
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:

****************************************************************/
void HmiCom_TimedTask(HMICOMTASK_MAG *pMag);

/****************************************************************
@FUNCTION：HMI 串口串口协议处理
@INPUT：
    @param pBuf - 数据内容
    @param BufLen - 数据内容长度
    @param pHandle - 处理句柄
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:

****************************************************************/
void HmiCom_Analysis(uint8_t *pBuf, uint16_t BufLen, void *pHandle);


/****************************************************************
@FUNCTION：获取联机状态
@INPUT：
    @param
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void HmiCom_Prot_CheckOnline(HMICOMTASK_MAG *pMag, uint16_t Cnt, uint32_t ScheTim);

/****************************************************************
@FUNCTION：显示单张或多张图片
@INPUT：
    @param eIdx - 图片编号
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
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



