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
#define LASER_COM_FRAME_LEN_MAX 512  //最大支持的报文长度

#define LASER_CTRL_LED_ON 50	//训练模式下LED持续时间

typedef struct _LASER_LED_MANAGER
{
	u8 m_bIsCtrlOn:1;	//控制LED亮
	u8 m_bIsCtrlOff:1;	//控制LED灭
	u8 m_bIsOn:1;		//LED状态指示
	u8 m_bIsToggle:1;
	u8 m_bRes:4;
	
	u16 m_wLedOnTimer;	//LED持续时间
}LASER_LED_MANAGER, *PLASER_LED_MANAGER;

typedef struct
{

    LASERCOM_LOWLEVEL_MAG *m_pLLMag;

    uint32_t m_bIsInit: 1;   //任务初始化
    uint32_t m_bIsRecvNew: 1; //接收到新数据
				
		uint32_t m_bIsOK: 1;		//自检OK
    uint32_t m_bIsAutoNumbered: 1;		//自检OK
    uint32_t m_bIsShoted: 1;		//TRUE-接收到中弹信息
    uint32_t m_bIsShotArround: 1;		//TRUE-接收到近弹信息
    uint32_t m_bIsJudge: 1;		//TRUE-接收到复活判死指令
    uint32_t m_bIsAmmuSupply: 1;		//TRUE-接收到补弹指令
    uint32_t m_bIsAmmuSet: 1;		//TRUE-接收到弹量初始化指令
    uint32_t m_bIsSuperScan: 1;		//TRUE-接收到连接超级蓝牙指令
    uint32_t m_bIsUpdate: 1;		//TRUE-接收到升级指令
    uint32_t m_TestMode: 1;		//TRUE-测试模式
    uint32_t m_TestOnlineMode: 1;		//TRUE-测试开机
    uint32_t m_bRes: 19;

    COMTASK_MAG m_sTaskMag;
		LASER_LED_MANAGER m_aLedCtrlMag[100];		//40
		
		RCVHURTINFO_T m_sRcvHurtInfo;   //接收到的打击信息

		uint8_t m_byActualNum; //探头实际数量
    uint8_t m_byConfigNum; //探头配置数量
		
		uint8_t m_byUniID;		//打击武器弹药统一编码
    uint8_t m_byLaserID;		//打击探头编号
    uint8_t m_byKey;		//打击方授权码
		uint8_t m_byDectorShelterID;
		uint8_t m_byLaserTimer;		//用于红外帧间计时，由于打一枪有多个接收器接收到数据，需要从中选择伤害最重的部位
   
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
    uint8_t m_byActualNum; //探头实际数量
    uint8_t m_byConfigNum; //探头配置数量
    RCVHURTINFO_T m_sRcvHurtInfo;   //接收到的打击信息
    uint8_t m_byUniID;	      //打击武器弹药统一编码
    uint8_t m_byLaserID;	  //打击探头编号
    uint8_t m_byKey;	      //打击方授权码
    uint8_t m_byLaserTimer;     //用于红外帧间计时，由于打一枪有多个接收器接收到数据，需要从中选择伤害最重的部位
    uint8_t m_byShotArroundTimer;
    uint8_t m_byJudgeTimer;
    uint8_t m_byAmmuSupplyTimer;
    uint8_t m_byAmmuSetTimer;
    uint8_t m_bySuperScanTimer;
    uint8_t m_byUpdateTimer;
    uint8_t m_aFrameTemp[15];
    uint32_t m_ulLaserCtrlInterval;
    LASER_LED_MANAGER m_aLedCtrlMag[40];
    uint8_t m_sPartID[LASER_PART_MAX][LASER_PART_ID_MAX];	//计算受伤分区
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
@FUNCTION：获取激光接收端口管理句柄
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 

****************************************************************/
LASERCOMTASK_MAG *LaserComTask_HandleGet(void);

/****************************************************************
@FUNCTION：获取激光接收端口任务管理句柄
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 

****************************************************************/
COMTASK_MAG *LaserComTask_TaskHandleGet(void);


/****************************************************************
@FUNCTION：激光接收串口功能初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sLaserComTask_Mag

****************************************************************/
void LaserComTask_Init(void);


/****************************************************************
@FUNCTION：激光接收串口功能反初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sLaserComTask_Mag

****************************************************************/
void LaserComTask_DeInit(void);


/****************************************************************
@FUNCTION：激光接收串口功能任务主进程
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sZigComTask_Mag

****************************************************************/
void LaserComTask_Process(void);

/****************************************************************
@FUNCTION：激光接收串口功能获取当前在线探头数
@INPUT： 无
@OUTPUT: 无
@RETURN: 在线探头数
@AUTHOR：xfw
@SPECIAL: g_sZigComTask_Mag

***************************************************************
uint8_t LaserComTask_GetActualNum(void);*/


/****************************************************************
@FUNCTION：获取探头带在线探头数
@INPUT：
    @param  MaxCnt - 可能的最大探头数
    @param  IntVal - 单个探头基础超时，单位10ms。若为0,则取默认值10
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 若回复多个报文，可以增大MaxCnt或者IntVal值
****************************************************************/
void LaserCom_Prot_GetOnlineNum(LASERCOMTASK_MAG *pMag, uint8_t MaxCnt, uint8_t IntVal);

/****************************************************************
@FUNCTION：LASER发送模式切换命令
@INPUT：LASER底层管理结构体指针，模式
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:无
****************************************************************/
void LaserCom_Prot_CtrlMode(LASERCOMTASK_MAG *pMag, LASER_MODE a_Mod);


//RGB进入白光，闪烁
void RGB(LASERCOMTASK_MAG *pMag);

//探头
void Laser_GetShotHandle_Master(LASERCOMTASK_MAG *pMag);
void Laser_GetShotHandle_Slave(LASERCOMTASK_MAG *pMag);

void LaserProtocalDeal(LASERCOMTASK_MAG *pMag, uint8_t *pBuf, uint8_t len, uint8_t IsMaster);

void Laser_JudgeHandle(LASERCOMTASK_MAG *pMag,uint8_t IsMaster,uint8_t Sta);

void Laser_CtrlLedOn(u8 Id, LASERCOMTASK_MAG *pMag);
void Laser_CtrlLedOff(uint8_t Id,LASERCOMTASK_MAG *pMag);
//探头闪灯指示，最后常亮
void Laser_CtrlLedToggle(uint8_t Id, LASERCOMTASK_MAG *pMag);
void Detector_Led_AllOn();
void Detector_Led_AllOff() ;
void Laser_InsertPartitionAndID(u8 byID, u8 byPartition, LASERCOMTASK_MAG *pMag);
void RemapHurtPart(RCVHURTINFO_T *RcvHurtInfo, u8 Where);
#endif



