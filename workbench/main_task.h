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

    //头盔板子
    #if BOARDID == 0x01
        #define DEBUG_COM COM5
        //背甲板子
    #elif BOARDID == 0x02
        #define DEBUG_COM COM2
    #endif

    #define LOG(fmt,...) Uart_DebugPrintf(&g_sUartSetMag.Com[DEBUG_COM],fmt, ##__VA_ARGS__)

    //调试口
    void DebugComInit(void);

    void printf_with_data(uint8_t *pBuf, uint16_t Len);

#else
    #define LOG(fmt,...)
#endif




//开关机初始化
void PowerOnOffInit(void);

void PowerOnHandle(void);

void PowerOffHandle(void);





#ifdef _APP

#define CheckRID(rid) (((rid)==(g_sIDParam.m_byRid)) ||((rid)==0xff))
#define CheckPID(pid) (((pid)==(g_sIDParam.m_wPid)) ||((pid)==0xffff))

#define LASER_PART_MAX PARTITION_MAX		//受伤部位处理时最多支持10个部位同时处理
#define LASER_PART_ID_MAX 10		//受伤部位处理时最多支持10个部位ID
#define PI             3.1415926
#define EARTH_RADIUS   6378.137        //地球近似半径

#define MOVE_OFFSET_MAX 100		//调节移动范围

#if defined(HC32F46x)
    void InitNVICSELRegister0_127(void);
#endif

#define POWEROFFSTARTCOUNT 10  //连续按键按下3s，开始关机
#define POWEROFFCOUNT 20  //连续按键按下5s，正式关机

//	定义关机事件管理结构体
__packed typedef struct
{
    uint16_t m_wTimer;	 //累计关机按键时间
    uint8_t m_bPoweroff_Force: 1; // 1表示强制关机
    uint8_t m_bIsStartOff: 1;
    uint8_t m_bIsOff: 1;	// 1表示关机标志
    uint8_t m_bPoweroffPre: 1;
		uint8_t m_bIsRes: 4;
} POWER_OFF_MAG;

extern POWER_OFF_MAG g_sPowerOffMag;

//开关机初始化，完成开机逻辑，该函数会导致系统堵塞1s
void AppPowerOnOffInit(POWER_OFF_MAG *pMag);


//开关机定时任务，完成关机检测 100ms定时调度
void AppPowerOnOffTimedProcess(POWER_OFF_MAG *pMag);


void AppPowerOffHandle(POWER_OFF_MAG *pMag);


#include "led.h"

/****************************************************************
@FUNCTION：运行灯初始化
@INPUT：LED管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 运行灯硬件操作需提前定义
			@@ RunLed_GpioInit
			@@ RunLed_On
			@@ RunLed_Off
****************************************************************/
extern void RunLed_Init(LED_MAG *pMag);

extern LED_MAG g_sRunledMag;
/****************************************************************
@FUNCTION：状态指示灯初始化
@INPUT：LED管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 运行灯硬件操作需提前定义
			@@ RLed_GpioInit
			@@ RLed_On
			@@ RLed_Off
闪烁指示：

****************************************************************/
extern void StaLed_Init(LED_MAG *pMag);

extern LED_MAG g_sStaledMag;


#include "beep.h"

extern BEEP_MAG g_sBeepMag;

/****************************************************************
@FUNCTION：非阻塞模式下无源蜂鸣器控制功能初始化
@INPUT：BEEP管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
extern void Beep_Init(BEEP_MAG *pMag);


#include "motor.h"

extern MOTOR_MAG    g_sMotorMag;

/****************************************************************
@FUNCTION：阻塞模式下马达控制功能初始化
@INPUT：MOTOR管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
extern void Motor_Init(MOTOR_MAG *pMag);

#include "fume.h"

extern FUME_MAG    g_sFumeMag;

/****************************************************************
@FUNCTION：阻塞模式下发烟罐控制功能初始化
@INPUT：FUME管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
extern void Fume_Init(FUME_MAG *pMag);


#include "wtn6.h"

typedef struct
{
    uint8_t m_byIsOnline: 1; //在线标记
    uint8_t m_byIsLock: 1; //锁定标记
    uint8_t m_byIsSetTxPwr: 1; //修改发射功率
		uint8_t m_byIsPowerOnFlg:1;
		uint8_t m_byIsSleepFlg:1;
    uint8_t m_byIsRes: 3; //保留

    uint8_t m_byTType;		//子设备实体类型
    uint8_t m_byTSubType;		//子设备实体子类型（0xFF-不关心，适用于配置发射机的时候）
    uint8_t m_byWType;		//子设备无线类型 (0x0 -有线，0x01 - 蓝牙，0x02 - 新标准zigbee，0x03 - lora设备)
    uint8_t m_aWMac[8];		//子设备MAC地址（高位在前）12
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
    uint16_t m_wBatVol;		//电池电压（高位在前，扩大1000倍） 16
		uint16_t AmmuLastRemainNum;
		uint16_t m_wPlayInterval;
    uint32_t m_dwLastCommTimeMs;    //上次通讯时间
    uint32_t m_dwLastGetStaTimeMs;  //上次发送获取状态时间
} SINGLE_SLAVER_MAG;


/****************************************************************
@FUNCTION：子设备状态初始列表初始化
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void SlaverMag_ListInit(void);

/****************************************************************
@FUNCTION：从子设备状态列表获取指定MAC地址的子设备管理结构体指针
@INPUT：MAC地址
@OUTPUT：无
@RETURN: 子设备管理结构体指针
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
SINGLE_SLAVER_MAG* SlaverMag_GetHandleByMac(uint8_t *pMacHex,uint8_t MacHexLen);

/****************************************************************
@FUNCTION：从子设备状态列表获取指定实体类型的子设备管理结构体指针
@INPUT：实体类型
@OUTPUT：无
@RETURN: 子设备管理结构体指针
@AUTHOR：xfw
@SPECIAL: 找到的第一个,注意组件类(0xFE) 终端
****************************************************************/
SINGLE_SLAVER_MAG* SlaverMag_GetHandleByTType(uint8_t TType);

__packed typedef struct{
    uint8_t m_byWeapon_Type;		//武器实体类型
    uint8_t m_byWeapon_SubType;		//武器实体子类型
    uint8_t m_byCfgBulletKind;		//配置弹药种类数

    uint8_t m_byState;		//武器状态

    uint8_t LockAck;		//锁定响应
    uint8_t UnLockFlag;		//武器解锁
    uint8_t LockBlood0;		//血量为0锁死
    BULLETINFO_T m_sBulletInfo[BULLETKIND_MAX];          //武器弹药信息(最多6中)
}SOLDIERAMMU_T;

//时间结构体
typedef struct
{
    //公历日月年周
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
    TIMER_MAG m_sTimer;		//死亡时间
    uint8_t m_byDamageKind;		//伤类ENUM_DAMAGE_KIND
    uint8_t m_byPart;		//伤部ENUM_PARTITION
    uint8_t m_byFRid;		//打击方RID
    uint16_t m_wFPid;		//打击方PID
    uint8_t m_byUniID;		//打击方武器弹药类型

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

// 定义终端用户数据,保持四字节对齐
typedef struct
{
    uint16_t m_wSingleCnt;		//系统子设备数
    uint16_t m_wSlaverStaGetInt;		//子设备状态获取时间间隔
    SINGLE_SLAVER_MAG m_saSingleMag[SYS_SLAVER_CNT_MAX];		//子设备管理
    
    uint8_t m_aWMac[8];		// 无线模组实时Mac地址
    uint8_t m_byWTxPwr;		//无线模块实时发射功率
		
		ENUM_UNITE_STATE m_eStatus;		// 1-正常 2-阵亡锁死 3-后台锁死 4-后台复活 5-裁判锁死 6-裁判复活 7-超距锁死 8-缺弹锁死
    
    uint8_t m_byBlood;		//单元血量 0~100,100表示状态正常，0表示死亡，其它表示伤的程度
    
		uint8_t	m_byMode;		//演习/训练模式,TRAIN--训练模式 GAME_PREP--演习预备模式    GAME_GOING   GAME_OVER
		uint8_t m_byJudge;
		uint8_t m_byDayOrNightJudge;
		//uint8_t m_IsNight:1;
	
		uint8_t m_byKey;		//授权码
	
		uint8_t Thickness;		//实体防护能力
		uint8_t LayMines_Or_MineClearance;		//布、排雷
		uint8_t EP_Num[3];		//方案编号
		
    uint8_t m_byActLaserCnt;		//实际在线探头数
		
		uint8_t m_IsIllegalMove: 1;	   //是否上传非法移动
    uint8_t m_IsIllegalPowerOff: 1;
    uint8_t m_IsAid: 1; 	           //是否被救助
    uint8_t m_IsNight: 1; 	       //是否是夜间模式
    uint8_t m_IsTerminalOnline: 1;   //是否配对终端在线
    uint8_t m_IsManoClothDis: 1;	   //演习模式下背甲离线
    uint8_t m_IsTestMode: 1;	//测试模式
    uint8_t m_IsBulletInit: 1;

		uint8_t m_byAidEndFlag1;		//救护结束标志
		uint8_t m_byAidEndFlag2;		//救护结束标志
		
    ENUM_CI_INDIVIDUAL m_eCIdegree;		//综合伤势
    ENUM_BRCHAR m_eBRChar;		//身份标识
    STRUCT_TS_Character1 m_sTsChar1;		//实体状态字1
    STRUCT_TS_Character2 m_sTsChar2;		//实体状态字2
    
    STRUCT_TIME1 m_sUtcTime;		//当前UTC时间
		StructUTC m_sManoStart;	//演习开始时间
    StructUTC m_sManoEnd;	//演习结束时间
		Struct_Time2 m_sDayStartTime;		//切换到白天的起始时间
    Struct_Time2 m_sNightStartTime;		//切换到黑夜的起始时间
		
    STRUCT_LPOS m_sLPosition;		//当前位置
    uint8_t m_byPosType;		//定位状态
    uint8_t m_bySatNum;		//卫星数量
    uint8_t m_bFireInstallFlg;		//火工品安装标志
    uint8_t m_byActLaserCnt1;		//分体组件实际在线探头数(针对单兵，为头盔)
    
		uint8_t m_byWeaponNum;		//武器携带数量
		uint8_t m_byBackstageCfgWeaponNum;		//后台配置的武器数量
    uint8_t m_bySelfWeaponNum;		//默认武器数量
    SOLDIERAMMU_T m_saBackstageCfgAmmuInfo[WEAPONKIND_MAX];		//后台配置武器信息
    SOLDIERAMMU_T m_saWeaponSelfAmmuInfo[WEAPONKIND_MAX];		//默认武器信息
		SOLDIERAMMU_T m_saAmmuInfo[WEAPONKIND_MAX];		//武器信息

		DEATH_MAG m_sLastDeathMag;
		
		uint8_t	m_byUploadInterval;		//上传时间
		uint8_t m_byDynamometerDisplayStatusDataUploadCycle;		//实体绑定炸点显示器状态数据上传周期
    uint8_t m_byEquipmentAndIdentityDataUploadCycle;		//绑定人员(装备)数量和身份数据上传周期
    
		STRUCT_BroadcastBeacon m_sBroadcastInfo;
		
		uint8_t m_byEnableChangeBat;

    uint32_t	m_dwChk;    //校验
} USER_PARAM;


extern USER_PARAM g_sUserParam;

__packed typedef struct
{
    uint8_t m_byGS_or_ZY;       //打高斯或者打朱元的标志：0高斯，1朱元
    uint8_t m_byG_Num;          //装药号
    uint8_t m_byYaoTemperature;	//药温度
    uint8_t m_byTspanSelect;    //间瞄弹药的引信延时时长选择
    uint8_t m_byT_SpanX;        //间瞄弹药的引信延时时间
    uint8_t m_byPCVelocity2;    //速度偏差
    uint8_t m_byNum1;           //发射弹药数量
    //数字化打击
    uint8_t m_byRmode;          //射界模式
    uint8_t m_byT_GUASS_X[3];   //高斯坐标x
    uint8_t m_byT_GUASS_Y[4];   //高斯坐标y
    uint8_t m_byT_GUASS_H[2];   //高斯坐标高程
    //非数字化打击
    uint8_t m_Angle1J[2];       //火炮射角
    uint8_t m_Angle1X[2];       //火炮射向
    uint8_t m_Pressure[2];      //阵地地面气压
    uint8_t m_Angle2[2];        //阵地地面风向
    uint8_t m_ZDDMVelocity1;    //阵地地面风速
    uint8_t m_ZDTemperature;    //阵地气温
		uint8_t m_byPJPLoactionFlag;
}STRUCT_Various_Elements;

extern STRUCT_Various_Elements g_sVarious_Elements;

__packed typedef struct
{
    u8 m_byA_Type;				//弹药类型
    u8 m_byA_Subtype;			//弹药子类型
    u8 m_aChangeAmmuNum[3];	    //变化的弹量

} STRUCT_AMMUSUPPLY_t;

__packed typedef struct
{
    u8 m_byM_Type;				//物资类型
    u8 m_byM_Subtype;			//物资子类型
    u8 m_aRemainSupplyNum[3];	//物资剩余
} Struct_Supply;				//物资信息

__packed typedef struct
{
    uint8_t		m_aData_Frame[2];		//帧头（固定为0xC1,0xF2）
    uint8_t 	m_byD_Len;				//数据帧长度
    uint8_t 	m_byT_Type;				//实体类型
    uint8_t 	m_byT_Subtype;			//实体子类型
    uint8_t 	m_byRID;				//单位代字
    uint8_t 	m_aPID[2];					//设备编号(大端模式)
    uint8_t 	m_byMasterT_Type;		//主控系统实体类型
    uint8_t 	m_byMasterT_Subtype;	//主控系统实体子类型
    uint8_t 	m_byMasterRID;			//主控系统单位代字
    uint16_t 	m_wMasterPID;			//主控系统设备编号(大端模式)

    uint8_t m_byKey;							//密钥
    uint8_t m_byBR_Attribute;				//红蓝属性 ENUM_BRCHAR
	
		STRUCT_TIME1 m_sStartTime	;		//演习开始时间
    STRUCT_TIME1 m_sEndTime	;		//演习开始时间
    STRUCT_TIME2 m_sDayStartTime;		//切换到白天的起始时间
    STRUCT_TIME2 m_sNightStartTime;		//切换到黑夜的起始时间
	
    uint8_t m_byBulletMaxAmmuNum[3];			//枪弹最大携带数量
    uint8_t m_byShellMaxAmmuNum[3];			//炮弹/火箭弹最大携带数量
    uint8_t m_byAmmuNum;						//携带的武器数量（m,m=0 时，后续无内容）
    //uint8_t m_bySupplyNum;					//携行物资种类数（n，n=0 时，物资数据区无内容）
    SOLDIERAMMU_T m_sAmmu[WEAPONKIND_MAX];	        //武器信息
    //Struct_Supply m_sSupply[6];		//物资信息
		
    uint8_t EP_Num[3];						//方案编号
    uint8_t Thickness;					    //实体防护能力（等效均质装甲厚度或混凝土厚度）;  0-19 	取值*1mm 	范围：0-19mm ;  20-255 	（取值-19）*10mm 	范围：20mm-2360mm

    uint8_t 	m_byXor;		//数据帧异或和
} Struct_CtrlManoParam_IndividualSoldierDownload;			//演习参数设置（单兵）

__packed typedef struct
{
    uint8_t m_aData_Frame[2];		//帧头（固定为0xC1,0xF3）
    uint8_t m_byD_Len;				//数据帧长度
    uint8_t m_byT_Type;				//实体类型
    uint8_t m_byT_Subtype;			//实体子类型
    uint8_t m_byRID;				//单位代字
    uint8_t m_aPID[2];				//设备编号(大端模式)
    uint8_t m_byMasterT_Type;		//主控系统实体类型
    uint8_t m_byMasterT_Subtype;	//主控系统实体子类型
    uint8_t m_byMasterRID;			//主控系统单位代字
    uint8_t m_aMasterPID[2];			//主控系统设备编号(大端模式)

    uint8_t m_bySelfStatusDataUploadCycle;					//实体自身状态数据上传周期 	秒
    uint8_t m_byDynamometerDisplayStatusDataUploadCycle;		//实体绑定炸点显示器状态数据上传周期  分
    uint8_t m_byEquipmentAndIdentityDataUploadCycle;			//绑定人员（装备）数量和身份数据上传周期  分

    uint8_t 	m_byXor;					//数据帧异或和
} Struct_CtrlIntervalDownload;				//上传周期设置

__packed typedef struct
{
    uint8_t		m_aData_Frame[2];		//帧头（固定为0xC1,0xF4）
    uint8_t 	m_byD_Len;				//数据帧长度
    uint8_t 	m_byT_Type;				//实体类型
    uint8_t 	m_byT_Subtype;			//实体子类型
    uint8_t 	m_byRID;				//单位代字
    uint8_t 	m_aPID[2];				//设备编号(大端模式)
    uint8_t 	m_byMasterT_Type;		//主控系统实体类型
    uint8_t 	m_byMasterT_Subtype;	//主控系统实体子类型
    uint8_t 	m_byMasterRID;			//主控系统单位代字
    uint8_t 	m_aMasterPID[2];		//主控系统设备编号(大端模式)
    u8 m_byChangeAmmuNum;				//发生变化弹药种类数（m，m=0 时，弹药数据区无内容）
    u8 m_byChangeSuppleNum;				//发生变化物资种类数（n，n=0 时，物资数据区无内容）

    STRUCT_AMMUSUPPLY_t m_sChangeAmmu[6];		//弹药信息
    Struct_Supply m_sChangeSupply[6];	//物资信息

    uint8_t 	m_byXor;				//数据帧异或和
} Struct_CtrlSupplementDownload;			//弹药（物资）分发/补给

typedef struct
{
    uint8_t H_Part;
    uint8_t H_Type;
    uint8_t H_Degree;
	
}INJURYIFNO_T;

__packed typedef struct{
	uint8_t		m_aData_Frame[2];		//帧头（固定为0xC1,0xF5）
	uint8_t 	m_byD_Len;				//数据帧长度 
	uint8_t 	m_byT_Type;				//实体类型
	uint8_t 	m_byT_Subtype;			//实体子类型 
	uint8_t 	m_byRID;				//单位代字 
	uint8_t 	m_aPID[2];					//设备编号(大端模式)
	uint8_t 	m_byMasterT_Type;		//主控系统实体类型
	uint8_t 	m_byMasterT_Subtype;	//主控系统实体子类型 
	uint8_t 	m_byMasterRID;			//主控系统单位代字 
	uint8_t 	m_aMasterPID[2];			//主控系统设备编号(大端模式)	
	
	uint8_t 	m_byAC_Character;		//裁决命令字 	ENUM_ACCHAR		
  STRUCT_TIME1 m_judgeTime;		//裁决时间    
	uint8_t 	m_byXor;				//数据帧异或和
	
}Struct_JudgeDownload;					//导控裁决

__packed typedef struct
{
    uint8_t m_aData_Frame[2];		//帧头（固定为0xC1,0xF6）
    uint8_t m_byD_Len;		//数据帧长度
    uint8_t m_byT_Type;		//被打击实体类型
    uint8_t m_byT_Subtype;		//被打击实体子类型
    uint8_t m_byRID;		//被打击实体单位代字
    uint8_t m_aPID[2];		//被打击实体设备编号
    uint8_t m_byH_Part;		//被打击实体命中部位
    uint8_t m_byH_Type;		//被打击实体伤类
    uint8_t m_byH_Idegree;		//被打击实体命中伤势
    STRUCT_TIME1 m_sAttackTime;		//被打击指令时间
    uint8_t m_byFT_Type;		//攻击方实体类型
    uint8_t m_byFT_Subtype;		//攻击方实体子类型
    uint8_t m_byF_RID;		//攻击方实体单位代字
    uint8_t m_aF_PID[2];		//攻击方实体设备编号
    uint8_t m_aFA_Type;		//攻击方弹药类型
    uint8_t m_aFA_Subtype;		//攻击方弹药子类型
    STRUCT_LPOS m_sL_Position ;		//经纬度 高度 定位状态

    uint8_t 	m_byXor;		//数据帧异或和
} Struct_DamageReportDownload;		

__packed typedef struct
{
    uint8_t m_aData_Frame[2];		//帧头（固定为0xC1,0xF7）
    uint8_t m_byD_Len;		//数据帧长度
    uint8_t m_byT_Type;		//实体类型
    uint8_t m_byT_Subtype;		//实体子类型
    uint8_t m_byRID;		//单位代字
    uint8_t m_aPID[2];		//设备编号(大端模式)

    uint8_t m_byFT_Type;		//发起核生化攻击的实体类型
    uint8_t m_byFT_Subtype;		//发起核生化攻击的实体子类型
    uint8_t m_byF_RID;		//发起核生化攻击的实体单位代字
    uint8_t m_aF_PID[2];		//发起核生化攻击的实体设备编号
    uint8_t m_byNCBT_Type;		//核化生标识字 	ENUM_NCBTTYPE
    uint8_t m_byStrength ;		//沾染区威胁强度  	0-100
    uint8_t m_byELR_Character ;		//进入/离开核化生沾染地域标识字  	ENUM_ELRCHAR

    uint8_t m_byXor;		//数据帧异或和

} Struct_NCBTDownload;		//进入/离开核化生沾染区

__packed typedef struct
{
    uint8_t	m_aData_Frame[2];		//帧头（固定为0xC1,0xF6）
    uint8_t m_byD_Len;		//数据帧长度
    u8 m_byT_Type;		//实体类型
    u8 m_byT_Subtype;		//实体子类型
    u8 m_byRID;		//实体单位代字
    u8 m_aPID[2];		//实体设备编号
    u8 m_byFT_Type;		//附近实体类型
    u8 m_byFT_Subtype;		//附近实体子类型
    u8 m_byF_RID;		//附近实体单位代字
    u8 m_aF_PID[2];		//附近实体设备编号
    STRUCT_LPOS m_sL_Position ;		//附近实体地理位置
    u8 ELR_Character;		//进入/离开实体附近地域标识字
    uint8_t m_byXor;		//数据帧异或和
} Struct_HelicopterVoiceCtr;		//进入直升机指令

__packed typedef struct
{
    uint8_t		m_aData_Frame[2];			//帧头（固定为0xC1,0xFD）
    uint8_t 	m_byD_Len;					//数据帧长度
    uint8_t 	m_byT_Type;					//实体类型
    uint8_t 	m_byT_Subtype;				//实体子类型
    uint8_t 	m_byRID;					//单位代字
    uint8_t 	m_aPID[2];					//设备编号(大端模式)

    uint8_t 	m_byES_Num[2];					//事件数据帧序列号

    uint8_t 	m_byXor;				    //数据帧异或和

} Struct_EventResponceDownload;				//事件接收应答指令
/******************************************************************
*                           事件数据                         *
******************************************************************/
typedef enum
{
    BNEventType_RSP_CtrlManoParam_NotIndividualSoldier = 0x01, //演习参数设置成功事件（非单兵）
    BNEventType_RSP_CtrlManoParam_IndividualSoldier = 0x02, //演习参数设置成功事件（单兵）
    BNEventType_RSP_CtrlInterval = 0x03, //上传周期设置成功事件
    BNEventType_RSP_CtrlSupplement = 0x04, //弹药（物资）分发/补给事件
    BNEventType_DamageReport = 0x05, //火力毁伤事件
    BNEventType_Judge = 0x06, //导控裁决事件
    BNEventType_NCBT = 0x07, //进入/离开核化生沾染区事件
    BNEventType_NCB_Protection = 0x08, //核化生防护事件
    BNEventType_NCB_Injury = 0x09, //核化生毁伤事件
    BNEventType_PE_Threat = 0x0A, //光电威胁事件
    BNEventType_PE_Protection = 0x0B, //烟雾/诱导防护事件
    BNEventType_DigitIndirectLanuch = 0x0C, //数字化火炮发射事件
    BNEventType_NonDigitIndirectLanuch = 0x0D, //非数字化火炮发射事件(迫击炮)
    BNEventType_Radar = 0x0E, //炮位侦察校射雷达侦察/校射事件
    BNEventType_HeavyWpnLaunch = 0x0F, //直瞄重武器发射事件
    BNEventType_IndLayMine = 0x10, //单兵布雷事件
    BNEventType_IndCleanMine = 0x11, //单兵排雷事件
    BNEventType_RocketLayMine = 0x12, //火箭布雷事件
    BNEventType_CarCleanMine = 0x13, //扫雷车扫雷事件
    BNEventType_RocketCleanMine = 0x14, //火箭扫雷事件
    BNBNEventType_InjuryWorse = 0x15, //人员伤势恶化事件
    BNEventType_IndRescued = 0x16, //单兵被救护事件
    BNEventType_ArrangeHinder = 0x17, //战场障碍设置事件
    BNEventType_RSP_CtrlFirePoint = 0x18, //火力点设置事件
    BNEventType_StatusRecovery = 0x19, //状态自动恢复事件
    BNEventType_RSP_CtrlEngineerRecon = 0x1A, //工程侦察事件
    BNEventType_EquipRepair = 0x1B, //装备抢修事件
    BNEventType_ChangeToMano = 0x1C, //演习模式转换事件
    BNEventType_IndWeaponOnline = 0x1D, //单兵武器绑定事件
    BNEventType_TerminalOnOff = 0x1E, //开关机事件
    BNEventType_AgaistRuleStart = 0x1F, //违规开始事件
    BNEventType_AgaistRuleStop = 0x20, //违规结束事件
    //自定义扩展
    BNEventType_Alarm = 0x81, //告警事件
} EnumBNEventType;

typedef enum{
	//发给后台
	BNCmd_Terminal_WorkConditionInfoUpload = 0x90,		//工况信息上传
	BNCmd_Terminal_AccessRequest =	0x91,		//终端入网请求
	BNCmd_Terminal_StatusUpload  =	0x92,		//状态上传
	BNCmd_Terminal_EventUpload   =	0x93,		//事件上传
	BNCmd_Terminal_StatusUpload_Bind = 0x94,		//绑定人员（装备）的状态上传指令
	BNCmd_Terminal_EventUpload_Bind  = 0x95,		//绑定人员（装备）的事件上传指令
	
	//后台发送
	//Cmd_BS_CtrlFSJ = 0xef,		//发射机控制
	BNCmd_CtrlHelicopter = 0xE3,
	BNCmd_CtrlTerminalParam = 0xE4,		//终端参数控制指令
	BNCmd_CtrlSetTerminalType = 0xE5,		//更改终端类型
	BNCmd_CtrlHeartBeat = 0xE6,		//心跳	
	BNCmd_CtrlSupplementary_Mano_Param = 0xE8,		//补充演习参数设置指令
	BNCmd_CtrlAccessAuthorization	=	0xE9,		//入网授权指令
	BNCmd_CtrlManoParam_NotIndividualSoldier	=	0xF1,		//演习参数设置指令（非单兵）
	BNCmd_CtrlManoParam_IndividualSoldier     =	0xF2,		//演习参数设置指令（单兵）
	BNCmd_CtrlInterval    =	0xF3,		//上传周期设置指令
	BNCmd_CtrlSupplement  =	0xF4,		//弹药/物资补给指令
	BNCmd_CtrlJudge				=	0xF5,		//主控系统裁决指令
	BNCmd_CtrlIndirectAim =	0xF6,		//仿真模拟打击指令
	BNCmd_CtrlNCBT				=	0xF7,		//进入/离开核化生沾染区通报指令
	BNCmd_CtrlBurstPointDisplay =	0xF8,		//炸点显示器触发控制指令
	BNCmd_CtrlPointFall			=	0xF9,		//侦察/校射信息下发指令（炮位侦察雷达和炮兵侦察）
	BNCmd_CtrlFirePoint			=	0xFA,		//火力点参数设置指令
	BNCmd_CtrlEngineerRecon	=	0xFB,		//侦察信息下发指令（工程侦察）
	BNCmd_CtrlMinefield			=	0xFC,		//雷区控制指令
	BNCmd_BSResponce			  =	0xFD,		//主控应答指令
	 
	// add by xfw 20191026	
	BNCmd_GetRidPid				=	0x80,		//获取RID PID
	BNCmd_SetRidPid				=	0x81,		//设置RID PID
	BNCmd_GetBleSlaverMac =	0x82,		//获取从设备
	BNCmd_SetBleSlaverMac =	0x83,		//配置从设备
	BNCmd_SaveBleSlaverMac = 0x84,		//保存从设备
	BNCmd_GetLog = 0x85,		//获取日志
	BNCmd_GetVersion = 0x86,		//获取版本号
	BNCmd_GetBackstageMag =	0x87,		//获取后台管理结构体
	BNCmd_ZigbeeJoinedNewNet =	0x88,	//Zigbee模块离开当前网络，加入新的网络
	BNCmd_4GServerConfig =	0x89,		//4G模块配置
	BNCmd_GetHardwareStatus =	0x8A,		//获取板级硬件状态
	BNCmd_CtrlTType =	0x8B,		//终端类型重置
	BNCmd_CtrlPowerOff =	0x8C,		//关机指令	
}EnumNewBackStage_CMD;

typedef struct  
{										    
		STRUCT_TIME1 m_sUtcTime;		//UTC时间
	
		uint32_t m_dwLatitude;		//纬度 （暂不提供，需要用时可以换算）
		uint8_t  m_byNorS;		//北纬/南纬,'N':北纬;'S':南纬				  
		uint32_t m_dwLongitude;		//经度 （暂不提供，需要用时可以换算）
		uint8_t  m_byEorW;		//东经/西经,E:东经;W:西经
		int32_t  m_dwAltitude;		//海拔高度,放大了10倍,实际除以10.单位:0.1m  （暂不提供）	 
		uint16_t m_wSpeed;		//地面速率,放大了1000倍,实际除以10.单位:0.001公里/小时	   （暂不提供）
		uint8_t  m_byPositionType;		//当前定位类型
		uint8_t  m_byJD_Du;
		uint8_t  m_byJD_Fen;
		uint32_t m_dwJD_Fen_XiaoShu;		//保留小数点五位小数
		uint8_t  m_byWD_Du;
		uint8_t  m_byWD_Fen;
		uint32_t m_dwWD_Fen_XiaoShu;		//保留小数点五位小数
		uint8_t	 m_byNumSv;		//用于定位的卫星数目，00~24
		uint8_t  RTKMode;
}NMEA_MAG;

#define USER_PARAM_MEMBER_GET(name) UserParam_Get##name()
#define FUNC_USER_PARAM_MEMBER_GETBY(name)  uint8_t UserParam_Get##name(void) {return g_sUserParam.m_b##name;}


/****************************************************************
@FUNCTION：终端运行时参数设置默认值
@INPUT：无
@OUTPUT：g_sUserParam结构体赋值
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 若数据取出失败，则重置数据
****************************************************************/
void UserParam_SetDefaultValue(USER_PARAM *pParam);

/****************************************************************
@FUNCTION：终端运行时参数初始化
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 若数据取出失败，则重置数据
****************************************************************/
void UserParam_Init(void);

/****************************************************************
@FUNCTION：终端运行时参数保存
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 阻塞较长时间
****************************************************************/
void UserParam_Save(void);


/****************************************************************
@FUNCTION：获取用户参数中的子设备数
@INPUT：无
@OUTPUT：无
@RETURN:  子设备数
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint16_t UserParam_GetSlaverCnt(void);

/****************************************************************
@FUNCTION：获取用户参数中的子设备数
@INPUT：无
@OUTPUT：无
@RETURN:  子设备数
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void UserParam_SetSlaverCnt(uint8_t Cnt);

/****************************************************************
@FUNCTION：获取用户参数中的子设备状态获取间隔
@INPUT：无
@OUTPUT：无
@RETURN:  子设备数
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint16_t UserParam_GetSlaverStatusInterval(void);

/****************************************************************
@FUNCTION：设置用户参数中的子设备状态获取间隔
@INPUT：间隔
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void UserParam_SetSlaverStatusInterval(uint16_t Val);

/****************************************************************
@FUNCTION：获取用户参数中的管理结构体
@INPUT：无
@OUTPUT：无
@RETURN: 操作句柄
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
SINGLE_SLAVER_MAG *UserParam_GetSlaverMag(uint8_t idx);


/****************************************************************
@FUNCTION：设置用户参数中的子设备管理结构体
@INPUT：子设备管理结构体，子设备序号
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void UserParam_SetSlaverMag(SINGLE_SLAVER_MAG * pVal, uint8_t idx);


/****************************************************************
@FUNCTION：获取当前血量
@INPUT：无
@OUTPUT：无
@RETURN: 血量
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint8_t UserParam_GetBlood(void) ;

/****************************************************************
@FUNCTION：设置当前血量
@INPUT：新值
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetBlood(uint8_t Val) ;

/****************************************************************
@FUNCTION：获取当前密钥
@INPUT：无
@OUTPUT：无
@RETURN: 密钥
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint8_t UserParam_GetKey(void) ;

/****************************************************************
@FUNCTION：设置当前密钥
@INPUT：新值
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetKey(uint8_t Val) ;

/****************************************************************
@FUNCTION：获取当前在线探头数
@INPUT：无
@OUTPUT：无
@RETURN: 在线探头数
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint8_t UserParam_GetActLaserCnt(void);

/****************************************************************
@FUNCTION：设置当前在线探头数
@INPUT：新值
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetActLaserCnt(uint8_t Val);

/****************************************************************
@FUNCTION：获取当前身份标识
@INPUT：无
@OUTPUT：无
@RETURN: 身份标识
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
ENUM_BRCHAR UserParam_GetBRChar(void) ;

/****************************************************************
@FUNCTION：设置当前身份标识
@INPUT：新值
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetBRChar(ENUM_BRCHAR eVal) ;

/****************************************************************
@FUNCTION：获取综合伤势
@INPUT：无
@OUTPUT：无
@RETURN: 综合伤势
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
ENUM_CI_INDIVIDUAL UserParam_GetCIdegree(void);

/****************************************************************
@FUNCTION：设置综合伤势
@INPUT：新值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetCIdegree(ENUM_CI_INDIVIDUAL eCI);

/****************************************************************
@FUNCTION：获取实体状态字1
@INPUT：无
@OUTPUT：无
@RETURN: 实体状态字1
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
STRUCT_TS_Character1 UserParam_GetTsChar1(void);

/****************************************************************
@FUNCTION：设置实体状态字1
@INPUT：新值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetTsChar1(STRUCT_TS_Character1 Val);

/****************************************************************
@FUNCTION：获取实体状态字2
@INPUT：无
@OUTPUT：无
@RETURN: 实体状态字2
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
STRUCT_TS_Character2 UserParam_GetTsChar2(void);

/****************************************************************
@FUNCTION：设置实体状态字2
@INPUT：新值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetTsChar2(STRUCT_TS_Character2 Val);

/****************************************************************
@FUNCTION：获取无线模组Mac地址
@INPUT：无
@OUTPUT：无
@RETURN: Zigbee模组Mac地址
@AUTHOR：xfw
@SPECIAL: 
****************************************************************/
uint8_t *UserParam_GetWMac(void);

/****************************************************************
@FUNCTION：设置无线模组Mac地址
@INPUT： 设置值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 不影响实际MAC
****************************************************************/
void UserParam_SetWMac(uint8_t * pVal, uint16_t ValLen);

/****************************************************************
@FUNCTION：获取当前无线模块发射功率
@INPUT：无
@OUTPUT：无
@RETURN: 密钥
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint8_t UserParam_GetWTxPwr(void);

/****************************************************************
@FUNCTION：设置当前无线模块发射功率
@INPUT：新值
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL: 只是值传递，不涉及硬件操作
****************************************************************/
void UserParam_SetWTxPwr(uint8_t Val);

/****************************************************************
@FUNCTION：获取经纬度坐标
@INPUT：无
@OUTPUT：无
@RETURN: 经纬度坐标
@AUTHOR：xfw
@SPECIAL: 
****************************************************************/
STRUCT_LPOS *UserParam_GetLPos(void);

/****************************************************************
@FUNCTION：设置经纬度坐标
@INPUT： 设置值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 
****************************************************************/
void UserParam_SetLPos(STRUCT_LPOS * pVal);

/****************************************************************
@FUNCTION：获取UTC时间
@INPUT：无
@OUTPUT：无
@RETURN: UTC时间
@AUTHOR：xfw
@SPECIAL: 
****************************************************************/
STRUCT_TIME1 *UserParam_GetTime1(void);

/****************************************************************
@FUNCTION：设置UTC时间
@INPUT： 设置值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 
****************************************************************/
void UserParam_SetTime1(STRUCT_TIME1 * pVal);

/****************************************************************
@FUNCTION：获取定位状态
@INPUT：无
@OUTPUT：无
@RETURN: 密钥
@AUTHOR：xfw
@SPECIAL: 
****************************************************************/
uint8_t UserParam_GetPosType(void);

/****************************************************************
@FUNCTION：设置定位状态
@INPUT：新值
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL: 
****************************************************************/
void UserParam_SetPosType(uint8_t Val);

/****************************************************************
@FUNCTION：获取卫星数量
@INPUT：无
@OUTPUT：无
@RETURN: 密钥
@AUTHOR：xfw
@SPECIAL: 
****************************************************************/
uint8_t UserParam_GetSatNum(void);

/****************************************************************
@FUNCTION：设置卫星数量
@INPUT：新值
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetSatNum(uint8_t Val);

/****************************************************************
@FUNCTION：获取火工品安装标志
@INPUT：无
@OUTPUT：无
@RETURN: 密钥
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint8_t UserParam_GetFireInstallFlg(void);

/****************************************************************
@FUNCTION：设置火工品安装标志
@INPUT：新值
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL: 
****************************************************************/
void UserParam_SetFireInstallFlg(uint8_t Val);

/****************************************************************
@FUNCTION：获取分体组件实际在线探头数(针对单兵，为头盔)
@INPUT：无
@OUTPUT：无
@RETURN: 密钥
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint8_t UserParam_GetActLaserCnt1(void);

/****************************************************************
@FUNCTION：设置分体组件实际在线探头数(针对单兵，为头盔)
@INPUT：新值
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetActLaserCnt1(uint8_t Val);





/****************************************************************
@FUNCTION：测试模式处理
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 无
****************************************************************/
void TesModeHandle(void);

/****************************************************************
@FUNCTION：获取板级自定义硬件状态1
@INPUT：无
@OUTPUT：无
@RETURN:  自定义硬件状态1
@AUTHOR：xfw
@SPECIAL: 无
****************************************************************/
uint8_t GetHardSta1(void);

/****************************************************************
@FUNCTION：获取板级自定义硬件状态2
@INPUT：无
@OUTPUT：无
@RETURN:  自定义硬件状态2
@AUTHOR：xfw
@SPECIAL: 无
****************************************************************/
uint8_t GetHardSta2(void);

/****************************************************************
@FUNCTION：获取板级自定义硬件状态3
@INPUT：无
@OUTPUT：无
@RETURN:  自定义硬件状态3
@AUTHOR：xfw
@SPECIAL: 无
****************************************************************/
uint8_t GetHardSta3(void);

/****************************************************************
@FUNCTION：获取板级自定义硬件状态4
@INPUT：无
@OUTPUT：无
@RETURN:  自定义硬件状态4
@AUTHOR：xfw
@SPECIAL: 无
****************************************************************/
uint8_t GetHardSta4(void);

/****************************************************************
@FUNCTION：获取电台信号强度
@INPUT：无
@OUTPUT：无
@RETURN:  信号强度（0-4）
@AUTHOR：xfw
@SPECIAL: 无
****************************************************************/
uint8_t GetDTRssi(void);


typedef struct
{
    ENUM_CONN_CHAN m_eChan;   //通道选择
    uint8_t *m_pDstAddr;  //若通道选择ZIGBEE，则填入目标MAC地址
    uint8_t m_byDstAddrLen; //目标地址长度，若为2，且地址为0xFFFF，则为广播
    uint8_t *m_pDatBuf;   //数据载荷
    uint16_t m_wDatBufLen; //数据载荷长度
    //FRAME_BUF *m_pFrame; //发送数据帧
    uint32_t  m_dwRepeatCnt;    //最大重发次数
    uint32_t  m_dwScheTime;    //重发间隔
}S_CONN_CHAN_PARAM;


typedef enum
{
    //发给后台
    Cmd_AccessRequest	=	0x91,	    //终端入网请求
    Cmd_StatusUpload	=	0x92,	    //状态上传
    Cmd_EventUpload	=	0x93,	        //事件上传
    Cmd_Response	=	0x94,		    //通信应答
    //后台发送
    Cmd_CtrlFSJ		=	0xef,		    //发射机控制
    Cmd_AccessAuthorization	=	0xf1,	//入网授权指令
    Cmd_CtrlManoeuvre	=	0xf2,		//演习配置指令
    Cmd_CtrlInterval	=	0xf3,		//状态信息上传间隔时间设置指令
    Cmd_CtrlIndirectAim	=	0xf4,		//导控打击指令
    Cmd_CtrlStartEnd	=	0xf5,		//5.开始结束指令
    Cmd_CtrlSupplement	=	0xf6,		//6.弹药物资补充指令
    Cmd_CtrlStartEndTime =	0xf7,		//7.演习时间重配置（修改）指令
    Cmd_CtrlBloodVolume	=	0xf8,		//血量配置指令
    Cmd_CtrlSync		=	0xf9,		//心跳同步指令
    Cmd_BSResponce		=	0xfa,		//主控应答指令
    Cmd_CtrlTType		=	0xfc,		//终端类型重置
    Cmd_CtrlDayAndNight	=	0xfd,		//昼夜模式切换指令
    Cmd_CtrlPowerOff	=	0xfe,		//关机指令
    Cmd_CtrlPointFall	=	0xed,		//火炮落点反馈
    // add by xfw 20191026
    Cmd_GetRidPid		=	0x80,		//获取RID PID
    Cmd_SetRidPid		=	0x81,		//设置RID PID

    Cmd_GetBleSlaverMac	=	0x82,		//获取从设备
    Cmd_SetBleSlaverMac	=	0x83,		//配置从设备
    Cmd_SaveBleSlaverMac =	0x84,		//保存从设备

    Cmd_GetLog			=	0x85,		//获取日志
    Cmd_GetVersion		=	0x86,		//获取版本号
    Cmd_GetBackstageMag	=	0x87,		//获取后台管理结构体
    Cmd_ZigbeeJoinedNewNet =	0x88,	//Zigbee模块离开当前网络，加入新的网络
    Cmd_4GServerConfig  =	0x89,		//4G模块配置
    Cmd_GetHardwareStatus =	0x8A,		//获取板级硬件状态
    Cmd_ProgramVersion  = 0x8B,         //获取程序版本号
    Cmd_TransmitProgram = 0x8C,         //协助传输程序
    Cmd_HardWareTest    = 0x66,			//测试硬件状态
} EnumBackStage_CMD;

/****************************************************************
@FUNCTION：内部互联互通通道发送数据
@INPUT：pChanParam - 相关参数
@OUTPUT：
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 帧ID取值规则:IR38通道-功能码<<24; ZIG通道-(功能码 << 24) | 目标地址前三字节
****************************************************************/
uint32_t InterChanSend(S_CONN_CHAN_PARAM *pChanParam);

/****************************************************************
@FUNCTION：子设备硬件控制
@INPUT：pDstAddr - 子设备地址，DstAddrLen-子设备地址，pCtrlDat - 协议数据，CtrlDatLen - 数据长度
@OUTPUT：
@RETURN: TRUE - 成功/ FALSE - 失败
@AUTHOR：xfw
@SPECIAL: 
****************************************************************/
uint32_t  InterConn_SlaverHardCtrl(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pCtrlDat,uint16_t CtrlDatLen);


/****************************************************************
@FUNCTION：子设备建链
@INPUT：pDstAddr - 子设备地址，DstAddrLen-子设备地址，pHandle - 操作句柄
@OUTPUT：
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 注意该帧除非被删除，否则一直会发送
****************************************************************/
//uint32_t InterConn_SlaverConnect(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen);
uint32_t  InterConn_SlaverConnect(ENUM_CONN_CHAN eChan,SINGLE_SLAVER_MAG *pSingleMag);

//内部互联互通协议解析
void InterConn_Analysis(ENUM_CONN_CHAN eChan, uint8_t *pBuf, uint16_t BufLen);

//硬件控制协议处理（55AA协议0功能码）
void Prot_HardCtrl_Analysis(uint8_t *pBuf, uint16_t BufLen, void *pHandle);

void InterConn_HardCtrl_Analysis(ENUM_CONN_CHAN eChan, uint8_t *pBuf, uint16_t BufLen);

/****************************************************************
@FUNCTION：从子设备状态列表获取指定MAC地址的子设备管理结构体指针
@INPUT：MAC地址
@OUTPUT：无
@RETURN: 子设备管理结构体指针
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
SINGLE_SLAVER_MAG* SlaverMag_GetHandleByMac(uint8_t *pMacHex,uint8_t MacHexLen);


/****************************************************************
@FUNCTION：从子设备状态列表获取指定实体类型的子设备管理结构体指针
@INPUT：实体类型+ 实体子类那个
@OUTPUT：无
@RETURN: 子设备管理结构体指针
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
SINGLE_SLAVER_MAG* SlaverMag_GetHandleByType(uint8_t TType,uint8_t TSubType);

//头盔硬件控制基础函数
void HardCtrl_HelmetBase(uint8_t *pBuf, uint16_t BufLen);

//控制头盔发烟罐发烟
void CtrlHelmetFume(DEVCTRL eCtrl, uint16_t Tim100Ms);

//控制头盔探头灯
void CtrlHelmetLaserOnOff(uint8_t LedIdx, uint8_t IsOn);
void CtrlHelmetLaserOn();
//声音
void CtrlHelmetVoice(uint8_t VoiceIdx);

void HardCtrl_Slaver_PowerOff(void);

//Zigbee交互
void SlaverMag_TimedProcess(void);
uint8_t GetAllSlavesConnectStatus(void);
//Zigbee发送处理
void InterConn_Protocol_Process(void);

void HardCtrl_Slaver_Day(void);
void HardCtrl_Slaver_Night(void);
void InterConn_ProtDeal_Slaver_AmmuSupply_Init(void);
void InterConn_ProtDeal_Slaver_AmmuSupply(SINGLE_SLAVER_MAG *pSlaverMag);
void InterConn_ProtDeal_Slaver_AmmuSupplyNum(SOLDIERAMMU_T *WeaponInfo, uint8_t b, uint32_t AmmuNum);
void InterConn_ProtDeal_Slaver_Weapon_State(uint8_t mode);
void InterConn_ProtDeal_SelfStatue(void);
void ZigComTask_BroadCast(uint8_t *pBuf, uint16_t BufLen);
//连带毁伤
void CollateralDamage(RCVHURTINFO_T HurtInfo);
uint8_t UserParam_GetWeaponNum(void);
void Mano_Start_Judge(void);
uint8_t Convert_HEXASCII(uint8_t *p1, uint8_t *p2, uint8_t *length);

double get_distance(double lat1/*纬度1*/, double lng1/*经度1*/, double lat2/*纬度2*/, double lng2/*经度2*/);
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

//红外
void IRAmmuGetHandle(Struct_CtrlSupplementDownload *Supplement);
uint32_t InterConn_ProtDeal_IR_SetOrRescue_ACK(void);

//后台
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



