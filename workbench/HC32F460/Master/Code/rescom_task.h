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
#define RES_COM_FRAME_LEN_MAX 2048  //最大支持的报文长度

typedef struct
{

    uint8_t 	m_byRes: 8;


    //实现相关硬件操作，可读写
    COM_HW_OPERATIONS m_sHwOps;

} RESCOM_LOWLEVEL_MAG;


#define HEXBUF_DST_LENMAX 512

typedef struct
{
		uint8_t m_4GParamFlg;		//获取电台参数的标志
    uint8_t m_4GNetFlg;
    uint8_t m_4GLoginNetFlg;		//4G入网的标志
    uint8_t m_4GestablishSocketFlg;		//4G向服务器连接socket的标志
    uint8_t m_4GcloseListenFlg;		//4G关闭监听的标志
    uint8_t m_4GcloseLinkFlg;		//4G关闭链接的标志
    uint8_t m_4GlogoutNetFlg;		//4G退网的标志
    uint8_t m_4GwriteIMSIFlg;		//4G写IMSI号的标志
    uint8_t m_4GsaveParamFlg;		//4G保存参数的标志
    uint8_t m_4GrestartFlg;		//重启4G模块;
    uint8_t m_4GDsmsFlg;		//模块检测到基站标志 0x4
    uint8_t m_4GAskDRXFlg;		//查询模块模式设置，取消睡眠应设为0，默认是2
    uint8_t m_4GWriteDRXFlg;		//写入模块睡眠模式;

    uint8_t m_4GStepFlg;		//4G顺序执行时的标志
    uint8_t m_4GNetCloseFlg;		//电台口发送的关网指令标记

    uint8_t m_time;		//指令间隔时间
    uint8_t m_4GneedSaveAndRst[2];		//是否需要保存和重启的标志 两个标志，[DRX相同=1][ISMI相同=1]
    uint8_t m_4GAskImsiFlg;		//查询模块的IMSI号码
    uint8_t m_StationATFlg;		//电台口发来AT命令的标志
    uint8_t m_AtOpenTime;		//控制发送atopen命令时间的标志
    uint8_t m_4GReadyFlg;		//4G模块上电启动成功

    uint8_t m_LedGFlg;		//绿灯控制标志
    uint8_t m_LedRFlg;		//红灯控制标志
    uint8_t m_LedGTimeFlg;		//绿灯时间标志
    uint8_t m_LedRTimeFlg;		//红灯时间标志
    uint8_t m_LedGStartBlinkFlg;		//绿灯开始闪烁使能
    uint8_t m_LedGRoundCountFlg;		//绿灯闪烁次数控制
    uint8_t m_LedRCloseNetFlg;		//主动关闭网络时，让红灯亮
		
    uint8_t m_socketID[2];		//socketID 1-5
    uint8_t m_4GTCP_or_UDP;		//4G网络类型选择的标志，默认1为udp
    uint8_t m_farIp[20];		//远端服务器的IP地址
    uint8_t m_farPort[8];		//远端服务器的端口号
    uint8_t m_myPort[8];		//本机端口号
    uint8_t m_imsi[20];		//本机的imsi
    uint8_t m_myIP[20];		//本机的IP地址

    uint8_t debugrst;		//测试重启用

    uint8_t m_bySendFailCnt;		//连续发送失败次数
    uint16_t m_wDataCovertLen;		//数据被转换的长度;

    uint8_t hexbuf_dst[HEXBUF_DST_LENMAX];
    UART_MANAGER *pUartMag;

    uint8_t m_bIsConfiguration: 1;		//是否处在配置状态，若是，此时针对0D 0A处理
    uint8_t m_bIsEnterATMode: 1;		//是否已进入AT指令模式
    uint8_t m_bIsSetServerIP: 1;		//是否已设置服务器IP
    uint8_t m_bIsSetMode: 1;		//是否已设置通信方式
    uint8_t m_bIsSave: 1;		//是否已保存

    uint8_t m_bIsCFUN0: 1;		//AT+CFUN=0
    uint8_t m_bIsSSWEN1: 1;		//AT^SSWEN=1
    uint8_t m_bIsSSIM: 1;		//AT^SSIM=3,460516840000004,0,00000000000000000000000000000000,00000000000000000000000000000000

    uint8_t m_bIsSSWEN0: 1;		//AT^SSWEN=0
    uint8_t m_bIsCFUN1: 1;	//AT+CFUN=1
    uint8_t m_byIsCommunicationSucc: 1;
    uint8_t m_bRes: 5;

    void *m_pPrivateData;

    uint8_t m_byEnterATModeTimeout;	//进入AT模式超时
    uint32_t m_dwNoCommunicateTimeOut; //无数据超时，重新入网

    uint32_t ModeRcvTime;
    uint16_t m_RssiLevel;
} FOURGMAG;

__packed typedef struct
{
    uint8_t m_byWeapon_Type;		//武器实体类型
    uint8_t m_byWeapon_SubType;		//武器实体子类型
    uint8_t m_byCfgBulletKind;		//配置弹药种类数
    uint8_t m_byState;		//武器状态
	
    BULLETINFO_T m_sBulletInfo[WEAPONKIND_MAX];		//武器弹药信息(最多6中)
} WeaponInfoBk;

__packed typedef struct
{
    uint8_t DisableSuicide: 1;		//1=不允许自杀 0=允许自杀
    uint8_t DisableKillTeammate: 1;		//1=不允许杀伤队友 0=允许杀伤队友
    uint8_t DisableCheat: 1;		//1=关闭防止作弊 0=打开防作弊
    uint8_t DisableInstallFume: 1;		//1=不必装火工品 0=装火工品
    uint8_t EnableDiffRid: 1;		//1=允许不同RID交战 0=不允许不同rid交战
    uint8_t DisableAssistLaser: 1;		//1=演习下关闭辅助激光 0=演习下不关闭辅助激光
    uint8_t LocationShowType: 1;		//1=经纬度坐标 0=高斯坐标
    uint8_t DisableDrillStartVoice: 1;		//1=禁止演习训练模式音效 0=允许演习训练模式音效

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

	uint32_t m_ulAccessAuthorization;		//上次入网授权指令时间(ms)
	uint32_t m_ulCtrlManoeuvre;		//上次演习配置指令时间
	uint32_t m_ulCtrlInterval;		//上次状态时间上传间隔配置指令时间
	uint32_t m_ulCtrlIndirectAim;			//上次导控打击指令时间
	uint32_t m_ulCtrlStartEnd;		//上次开始结束指令时间
	uint32_t m_ulCtrlSupplement;		//上次弹药物资补充指令时间
	uint32_t m_ulCtrlStartEndTime;		//上次演习时间重配置指令时间
	uint32_t m_ulCtrlBloodVolume;		//上次血量配置指令时间
	uint32_t m_ulCtrlSync;		//上次心跳同步指令时间
	uint32_t m_ulCtrlTType;		//上次终端类型重置指令时间
	uint32_t m_ulCtrlDayAndNight;		//上次昼夜模式切换指令
	uint32_t m_ulCtrlPowerOff;		//上次关机指令时间
	uint32_t m_ulCtrlFSJ;		//上次发射机控制指令时间
	uint32_t m_ulCtrlInOutNBCArea;		//进入、离开核化生区域指令时间
	uint32_t m_ulCtrlSimulateAim;		//上次仿真打击指令时间
	uint32_t m_ulCtrlHelicopterVoice;		//上次控制飞机音效指令时间
	
	StructUTC m_BSNulCtrlManoeuvre;		//上次演习配置指令时间
	StructUTC m_sBSNulCtrlInterval;		//间隔
	StructUTC m_sBSNulCtrlSupplement;		//补给
	StructUTC m_sBSNJudge;		//后台裁决指令
}BackStage_CmdTimeMag_t;

__packed typedef struct{
	uint8_t m_byA_Type;		//弹药类型			
	uint8_t m_byA_Subtype;		//弹药子类型				
	uint8_t m_aRemainAmmuNum[3];		//弹药剩余量						
	uint8_t m_aUsedAmmuNum[3];		//弹药剩余量	
}Struct_AmmoMsg;		//弹药信息

__packed typedef struct
{
    u8 m_byW_Type;		//武器类型
    u8 m_byW_Subtype;		//武器子类型
    u8 m_byW_RID;		//武器单位代字
    u8 m_aW_PID[2];		//武器设备编号
    u8 m_byL_Character;		//武器链接情况 ENUM_LCHAR
    u8 m_byAmmuNum;		//携行弹药种类数（m，m=0 时，弹药数据区无内容）
    BULLETINFO_T m_aAmmu[3];		//弹药信息
} Struct_Weapon;		//武器信息

__packed typedef struct{
	uint8_t m_aData_Frame[2];		//数据帧头，为 0xA8，0x92
	uint8_t m_byD_Len;		//数据帧长度
	uint8_t m_byD_Type;		//状态类型序号，为 0x01;   
	uint8_t m_byT_Type;		//实体类型
	uint8_t m_byT_Subtype;		//实体子类型
	uint8_t m_byRID ;		//单位代字
	uint8_t m_aPID[2];		//设备编号
	uint8_t m_byKey;		//密钥
	STRUCT_LPOS m_sL_Position;		//经纬度 高度 定位状态
	STRUCT_TIME1 m_sTime;		//时刻
	uint8_t m_byC_Idegree;		//综合伤势	ENUM_CI_ARMOR  ENUM_CI_SOLDIER  ENUM_CI_OTHER
	STRUCT_TS_Character1 m_sTS_Character1;		//实体状态字 1
	uint8_t m_byBP_Character;
	uint8_t m_byAmmuNum;		//携行弹药种类数（m，m=0 时，弹药数据区无内容） 
	uint8_t m_bySupplyNum;		//携行物资种类数（n，n=0 时，物资数据区无内容）
	Struct_AmmoMsg m_sAmmu[6];		//弹药信息
	Struct_Supply m_sSupply[6];		//物资信息
	
	uint8_t m_byXor;		//异或和
}Struct_UniversalStatusUpload;		//通用状态上传

__packed typedef struct
{
    uint8_t HeadFlg[2];
    uint8_t Blood;
    uint8_t IsNight;
    uint8_t InjuryCnt;
    uint8_t InjuryKind;
    uint8_t Decter[10 + 1];
    uint8_t m_byInjuryPart[11];		//伤情信息
    uint8_t m_byWeaponNum;		//武器携带数量
    WeaponInfoBk WeaponInfo[WEAPONKIND_MAX];		//武器信息
    DRILL_PARAM DrillParam;
    uint16_t	Shell[PARTITION_MAX];
    uint8_t EP_Num[3];		//方案编号
    STRUCT_TIME1 m_sManoStart;		//演习开始时间
    STRUCT_TIME1 m_sManoEnd;		//演习开始时间
    Struct_Time2 m_sDayTime;
    Struct_Time2 m_sNightTime;
    uint16_t NoRspEvtNum;		//未上传事件数
    uint16_t ES_Num;		//当前事件编号
    uint8_t EvtRspFlg[30];		//上传事件响应标记合成营修改事件为240条
		uint8_t EvtRspFlgBak[30];
    uint8_t IsDrill;
    uint8_t Belong;
    uint8_t IsEnablePowerOff;
    uint8_t Key;
    uint8_t T_Subtype;
		uint32_t RptInterval;	//上报间隔
    VIOLATIONS Vios;
    uint8_t XOR;
		
} PARAMATER_BACKUP;

__packed typedef struct{
	u8 m_byT_Type;		//实体类型			
	u8 m_byT_Subtype;		//实体子类型		
	u8 m_byRID;		//实体单位代字 
	u8 m_aPID[2];		//实体设备编号	
	STRUCT_TIME1 m_sRecCmdTime;		//接收指令时间
	STRUCT_TIME1 m_sStartTime;		//演习开始时间
	u8 m_byST_Type;		//设置方实体类型			
	u8 m_byST_Subtype;		//设置方实体子类型		
	u8 m_byS_RID;		//设置方实体单位代字 
	u8 m_aS_PID[2];		//设置方实体设备编号	
	Struct_Time2 m_sDayStartTime;		//切换到白天的起始时间
	Struct_Time2 m_sNightStartTime;		//切换到黑夜的起始时间
	u8 m_byKey;		//密钥
	u8 m_byBR_Attribute;		//红蓝属性 ENUM_BRCHAR
	u8 m_aBulletMaxAmmuNum[3];		//枪弹最大携带数量
	u8 m_aShellMaxAmmuNum[3];		//炮弹最大携带数量
	u8 m_byAmmuNum;		//携行弹药种类数（m，m=0 时，弹药数据区无内容） 
	u8 m_bySupplyNum;		//携行物资种类数（n，n=0 时，物资数据区无内容）
	Struct_AmmoMsg m_sAmmu[6];		//弹药信息
	Struct_Supply m_sSupply[6];		//物资信息
	
	u8 m_byEP_Num[3];		//方案编号
	u8 m_byThickness;		//实体防护能力（等效均质装甲厚度或混凝土厚度）;  0-19 	取值*1mm 	范围：0-19mm ;  20-255 	（取值-19）*10mm 	范围：20mm-2360mm 
}StructEvent_RSP_CtrlManoParam_NotIndividualSoldier;		//演习参数设置成功事件（非单兵）(E_Type = EventType_RSP_CtrlManoParam_NotIndividualSoldier) 

__packed typedef struct{
	u8 m_byT_Type;						//被设置实体类型			
	u8 m_byT_Subtype;					//被设置实体子类型		
	u8 m_byRID;							//被设置实体单位代字 
	u8 m_aPID[2];						//被设置实体设备编号	
	STRUCT_TIME1 m_sRecCmdTime;		//接收指令时间
	STRUCT_TIME1 m_sStartTime;			//演习开始时间
	STRUCT_TIME1 m_sEndTime;			//演习结束时间
	u8 m_byST_Type;						//实施设置方实体类型			
	u8 m_byST_Subtype;					//实施设置方实体子类型		
	u8 m_byS_RID;						//实施设置方实体单位代字 
	u8 m_aS_PID[2];					//实施设置方实体设备编号	
	Struct_Time2 m_sDayStartTime;		//切换到白天的起始时间
	Struct_Time2 m_sNightStartTime;	//切换到黑夜的起始时间
	u8 m_byKey;							//密钥
	u8 m_byBR_Attribute;				//红蓝属性 ENUM_BRCHAR
	u8 m_aBulletMaxAmmuNum[3];			//枪弹最大携带数量
	u8 m_aShellMaxAmmuNum[3];			//炮弹最大携带数量
	u8 m_byWeaponNum;					//武器数量
	Struct_Weapon m_sWeapon[3];		//武器信息
	//Struct_Ammu m_sAmmu[6];			//弹药信息
	//Struct_Supply m_sSupply[6];		//物资信息
	
	u8 m_byEP_Num[3];						//方案编号
	u8 m_byThickness;					//实体防护能力（等效均质装甲厚度或混凝土厚度）;  0-19 	取值*1mm 	范围：0-19mm ;  20-255 	（取值-19）*10mm 	范围：20mm-2360mm 
}StructEvent_RSP_CtrlManoParam_IndividualSoldier;				//演习参数设置成功事件（单兵）(E_Type = EventType_RSP_CtrlManoParam_IndividualSoldier) 

__packed typedef struct{
	u8 m_byT_Type;						//被设置实体类型			
	u8 m_byT_Subtype;					//被设置实体子类型		
	u8 m_byRID;							//被设置实体单位代字 
	u8 m_aPID[2];						//被设置实体设备编号	
	STRUCT_TIME1 m_sRecCmdTime;		//接收指令时间
	u8 m_byST_Type;						//实施设置方实体类型			
	u8 m_byST_Subtype;					//实施设置方实体子类型		
	u8 m_byS_RID;						//实施设置方实体单位代字 
	u8 m_aS_PID[2];					//实施设置方实体设备编号	
	
	u8 m_bySelfStatusDataUploadCycle;	//实体自身状态数据上传周期 	秒
	u8 m_byDynamometerDisplayStatusDataUploadCycle;		//实体绑定炸点显示器状态数据上传周期  分
	u8 m_byEquipmentAndIdentityDataUploadCycle;			//绑定人员（装备）数量和身份数据上传周期  分
}StructEvent_RSP_CtrlInterval;				//上传周期设置成功事件 (E_Type = EventType_RSP_CtrlInterval) 

__packed typedef struct{
	u8 m_byT_Type;						//被设置实体类型			
	u8 m_byT_Subtype;					//被设置实体子类型		
	u8 m_byRID;							//被设置实体单位代字 
	u8 m_aPID[2];						//被设置实体设备编号	
	u8 m_byC_Idegree;					//综合伤势 ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;		//经纬度 高度 定位状态
	STRUCT_TIME1 m_sRecCmdTime;		//接收指令时间
	u8 m_byST_Type;						//引起弹药/物资变化的实体类型			
	u8 m_byST_Subtype;					//引起弹药/物资变化的实体子类型		
	u8 m_byS_RID;						//引起弹药/物资变化的实体单位代字 
	u8 m_aS_PID[2];					//引起弹药/物资变化的实体设备编号		
	u8 m_byIO_Idegree;					//弹药 (物资)补给标识字	ENUM_IOCHAR
	u8 m_byChangeAmmuNum;				//发生变化弹药种类数（m，m=0 时，弹药数据区无内容）
	u8 m_byChangeSuppleNum;				//发生变化物资种类数（n，n=0 时，物资数据区无内容）
	
	STRUCT_AMMUSUPPLY_t m_sChangeAmmu[6];		//弹药信息
	Struct_Supply m_sChangeSupply[6];	//物资信息
}StructEvent_RSP_CtrlSupplement;				//弹药（物资）分发/补给事件(E_Type = EventType_RSP_CtrlSupplement) 

__packed typedef struct{
	u8 m_byT_Type;							 //被打击实体类型			
	u8 m_byT_Subtype;						 //被打击实体子类型		
	u8 m_byRID;								 //被打击实体单位代字 
	u8 m_aPID[2];							 //被打击实体设备编号	
	u8 m_byC_Idegree;						 //综合伤势 ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //经纬度 高度 定位状态
	STRUCT_TIME1 m_sAttackTime;			 //被打击指令时间
	u8 m_byFT_Type;							 //攻击方实体类型			
	u8 m_byFT_Subtype;						 //攻击方实体子类型		
	u8 m_byF_RID;							 //攻击方实体单位代字 
	u8 m_aF_PID[2];						 //攻击方实体设备编号		
	
	u8 m_byFA_Type;							 //攻击方弹药类型
	u8 m_byFA_Subtype;						 //攻击方弹药子类型 
	u8 m_byH_Part;							 //被打击实体命中部位
	u8 m_byH_Type;							 //被打击实体伤类 
	u8 m_byH_Idegree;						 //被打击实体命中伤势
	
	STRUCT_LPOS m_sFL_Position ;			 //攻击方地理位置 

}StructEvent_DamageReport;				 //火力毁伤事件(E_Type = EventType_DamageReport) 



__packed typedef struct{
	u8 m_byT_Type;							 //实体类型			
	u8 m_byT_Subtype;						 //实体子类型		
	u8 m_byRID;								 //单位代字 
	u8 m_aPID[2];							 //实体设备编号	
	u8 m_byC_Idegree;						 //综合伤势 ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //经纬度 高度 定位状态
	STRUCT_TIME1 m_sRecCmdTime;			 //接收指令时间
	u8 m_byFT_Type;							 //实施裁决的实体类型			
	u8 m_byFT_Subtype;						 //实施裁决的实体子类型		
	u8 m_byF_RID;							 //实施裁决的实体单位代字 
	u8 m_aF_PID[2];						 //实施裁决的实体设备编号		
	
	u8 m_byAC_Character;					 //裁决命令字 	ENUM_ACCHAR					

}StructEvent_BNJudge;				     //导控裁决事件(E_Type = EventType_Judge) 

__packed typedef struct{
	u8 m_byT_Type;							 //实体类型			
	u8 m_byT_Subtype;						 //实体子类型		
	u8 m_byRID;								 //单位代字 
	u8 m_aPID[2];							 //实体设备编号	
	u8 m_byC_Idegree;						 //综合伤势 ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //经纬度 高度 定位状态
	STRUCT_TIME1 m_sNCBTTime;				 //进入/离开沾染区时间
	u8 m_byFT_Type;							 //发起核生化攻击的实体类型			
	u8 m_byFT_Subtype;						 //发起核生化攻击的实体子类型		
	u8 m_byF_RID;							 //发起核生化攻击的实体单位代字 
	u8 m_aF_PID[2];						 //发起核生化攻击的实体设备编号		
	u8 m_byNCBT_Type;						 //核化生标识字 	ENUM_NCBTTYPE
	u8 m_byStrength ;						 //沾染区威胁强度  	0-100 
	u8 m_byELR_Character ;					 //进入/离开核化生沾染地域标识字  	ENUM_ELRCHAR

}StructEvent_NCBT;				         //进入/离开核化生沾染区事件(E_Type = EventType_NCBT) 

__packed typedef struct{
	u8 m_byT_Type;							 //实体类型			
	u8 m_byT_Subtype;						 //实体子类型		
	u8 m_byRID;								 //单位代字 
	u8 m_aPID[2];							 //实体设备编号	
	u8 m_byC_Idegree;						 //综合伤势 ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //经纬度 高度 定位状态
	STRUCT_TIME1 m_sNCBPTime;				 //实施核化生防护时间
	u8 m_byNCBP_Type ;						 //进入/离开核化生沾染地域标识字  	ENUM_NCBPTYPE

}StructEvent_NCB_Protection;			 //核化生防护事件(E_Type = EventType_NCB_Protection) 

__packed typedef struct{
	u8 m_byT_Type;							 //被毁伤实体类型			
	u8 m_byT_Subtype;						 //被毁伤实体子类型		
	u8 m_byRID;								 //被毁伤单位代字 
	u8 m_aPID[2];							 //被毁伤实体设备编号	
	u8 m_byC_Idegree;						 //综合伤势 ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //被毁伤地理位置 经纬度 高度 定位状态
	STRUCT_TIME1 m_sNCBPTime;				 //实体被毁伤的时间 
	u8 m_byFT_Type;							 //发起核生化攻击的实体类型			
	u8 m_byFT_Subtype;						 //发起核生化攻击的实体子类型		
	u8 m_byF_RID;							 //发起核生化攻击的实体单位代字 
	u8 m_aF_PID[2];						 //发起核生化攻击的实体设备编号		
	u8 m_byNCBT_Type;						 //核化生标识字 	ENUM_NCBTTYPE
	
	u8 m_byH_Part;							 //命中部位
	u8 m_byH_Type;							 //伤类 
	u8 m_byH_Idegree;						 //命中伤势

}StructEvent_NCB_Injury;				 //核化生毁伤事件(EType = EventType_NCB_Injury) 

__packed typedef struct{
	u8 m_byT_Type;							 //实体类型			
	u8 m_byT_Subtype;						 //实体子类型		
	u8 m_byRID;								 //单位代字 
	u8 m_aPID[2];							 //实体设备编号	
	u8 m_byC_Idegree;						 //综合伤势 ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //经纬度 高度 定位状态
	STRUCT_TIME1 m_sPETTime;				 //受到光电威胁时间
	u8 m_byFT_Type;							 //威胁方实体类型			
	u8 m_byFT_Subtype;						 //威胁方实体子类型		
	u8 m_byF_RID;							 //威胁方实体单位代字 
	u8 m_aF_PID[2];						 //威胁方实体设备编号		
	STRUCT_LPOS m_sFL_Position ;			 //威胁方经纬度 高度 定位状态
	u8 m_byLT_Type;							 //光电威胁类型 	ENUM_LTTYPE
	u8 m_byDirection ;						 //光电威胁的来源方向 ENUM_DIR

}StructEvent_PE_Threat;				     //光电威胁事件(E_Type = EventType_PE_Threat) 


__packed typedef struct{
	u8 m_byT_Type;							 //实体类型			
	u8 m_byT_Subtype;						 //实体子类型		
	u8 m_byRID;								 //单位代字 
	u8 m_aPID[2];							 //实体设备编号	
	u8 m_byC_Idegree;						 //综合伤势 ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //经纬度 高度 定位状态
	STRUCT_TIME1 m_sTime;					 //时间
	u8 m_bySIP_Type;						 //光电防护事件子类型；使用诱导激光进行防护时，后续字段数据为 0   ENUM_SIPTYPE
	u8 m_byFA_Type;							 //防护使用弹药类型
	u8 m_byFA_Subtype;						 //防护使用弹药子类型
	u8 m_byFAmmuNum;						 //防护使用弹药数量
}StructEvent_PE_Protection;			 	 //烟雾/诱导防护事件(E_Type = EventType_PE_Protection) 

__packed typedef struct{
	u8 m_byT_Type;							 //发射炮弹的实体类型			
	u8 m_byT_Subtype;						 //发射炮弹的实体子类型		
	u8 m_byRID;								 //发射炮弹的单位代字 
	u8 m_aPID[2];							 //发射炮弹的实体设备编号	
	u8 m_byC_Idegree;						 //发射炮弹的综合伤势 ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //发射炮弹的 经纬度 高度 定位状态
	STRUCT_TIME1 m_sTime;					 //发射炮弹的时间
	
	u8 m_byA_Type;							 //发射弹药类型
	u8 m_byA_Subtype;						 //发射弹药子类型
	u8 m_byG_Num;							 //装药号  ENUM_GNUM
	signed char m_byAmmunitionTemperature;	 //药温 -80-+80
	u8 m_byTSpan_Select; 					 //间瞄弹药的引信延时时长选择  ENUM_FUSE_TYPE
	u8 m_T_SpanX;                            //间瞄弹药的引信延时时间
	u8 m_byVelocity2; 						 //初速偏差  0-255 单位：0.1 米/秒
	u8 m_byA_AmmuNum;						 //发射弹药数量
	u8 m_byR_Mode ;							 //射界模式 
	STRUCT_GPOS m_sG_Position ;				 //目标阵地高斯坐标 
	
}StructEvent_DigitIndirectLanuch;		 //数字化火炮发射事件(E_Type = EventType_DigitIndirectLanuch) 


__packed typedef struct{
	u8 m_byT_Type;							 //发射炮弹的实体类型			
	u8 m_byT_Subtype;						 //发射炮弹的实体子类型		
	u8 m_byRID;								 //发射炮弹的单位代字 
	u8 m_aPID[2];							 //发射炮弹的实体设备编号	
	u8 m_byC_Idegree;						 //发射炮弹的综合伤势 ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //发射炮弹的 经纬度 高度 定位状态
	STRUCT_TIME1 m_sTime;					 //发射炮弹的时间
	
	u8 m_byA_Type;							 //发射弹药类型
	u8 m_byA_Subtype;						 //发射弹药子类型
	u8 m_byG_Num;							 //装药号  ENUM_GNUM
	signed char m_byAmmunitionTemperature;	 //药温 -80-+80
	u8 m_byIndirectTSpan_Select; 			 //间瞄弹药的引信延时时长选择  ENUM_FUSE_TYPE
	u8 m_byIndirectT_Span; 					 //间瞄弹药的引信延时时间     0-9 	取值*0.1 秒 	范围：0.1 秒-0.9 秒 ; 10-69 （取值-9）*1 秒 	范围：1 秒-60 秒 
	u8 m_byVelocity2; 						 //初速偏差  0-255 单位：0.1 米/秒
	
	u8 m_byA_AmmuNum;						 //发射弹药数量
	u8 m_aArtilleryShootAngle[2];			 //火炮射角 0-6000 
	u8 m_aArtilleryShootdirection[2];		 //火炮射向 0-6000 
	u8 m_aGroundAirPressure[2];			 //阵地地面气压
	u8 m_aGroundWindDirection[2];			 //阵地地面风向 -360?360
	u8 m_aGroundWindVelocity1;				//阵地地面风速
	signed char m_byGroundTemperature;	     //阵地气温 -80-+80
	
}StructEvent_NonDigitIndirectLanuch;	 //非数字化火炮发射事件(E_Type = EventType_NonDigitIndirectLanuch) 


__packed typedef struct{
	u8 m_byT_Type;							 //炮位侦察校射雷达实体类型			
	u8 m_byT_Subtype;						 //炮位侦察校射雷达实体子类型		
	u8 m_byRID;								 //炮位侦察校射雷达单位代字 
	u8 m_aPID[2];							 //炮位侦察校射雷达实体设备编号	
	u8 m_byC_Idegree;						 //综合伤势 ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //开始或停止侦察/校射操作时的地理位置 经纬度 高度 定位状态
	STRUCT_TIME1 m_sTime;					 //开始或停止侦察/校射操作的时间
	u8 m_bySAM_Character;					 //侦察/校射模式标识字  ENUM_SAMCHAR
	u8 m_bySS_Character;					 //侦察/校射开关操作  ENUM_SSCHAR
	u8 m_aRadarAzimuth[2];					 //雷达方位角  0-6000 
	u8 m_aRadarPitchAngle[2];				 //雷达俯仰角  0-6000 
	
}StructEvent_Radar;	 //炮位侦察校射雷达侦察/校射事件(E_Type = EventType_Radar) 

__packed typedef struct{
	u8 m_byT_Type;							 //实体类型			
	u8 m_byT_Subtype;						 //实体子类型		
	u8 m_byRID;								 //单位代字 
	u8 m_aPID[2];							 //实体设备编号	
	u8 m_byC_Idegree;						 //综合伤势 ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //发射弹药时的地理位置 经纬度 高度 定位状态
	STRUCT_TIME1 m_sTime;					 //发射弹药的时间
	u8 m_byA_Type;							 //发射弹药类型
	u8 m_byA_Subtype;						 //发射弹药子类型
	
}StructEvent_HeavyWpnLaunch;			 //直瞄重武器发射事件(E_Type = EventType_HeavyWpnLaunch) 


__packed typedef struct{
	u8 m_byT_Type;							 //实体类型			
	u8 m_byT_Subtype;						 //实体子类型		
	u8 m_byRID;								 //单位代字 
	u8 m_aPID[2];							 //实体设备编号	
	u8 m_byC_Idegree;						 //综合伤势 ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //布雷时的地理位置 经纬度 高度 定位状态
	STRUCT_TIME1 m_sTime;					 //布雷的时间
	u8 m_byMT_Type;							 //地雷实体类型			
	u8 m_byMT_Subtype;						 //地雷实体子类型		
	u8 m_byM_RID;							 //地雷单位代字 
	u8 m_aM_PID[2];						 //地雷实体设备编号	
}StructEvent_IndLayMine;				 //单兵布雷事件(E_Type = EventType_IndLayMine) 

__packed typedef struct{
	u8 m_byT_Type;							 //实体类型			
	u8 m_byT_Subtype;						 //实体子类型		
	u8 m_byRID;								 //单位代字 
	u8 m_aPID[2];							 //实体设备编号	
	u8 m_byC_Idegree;						 //综合伤势 ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //排雷时的地理位置 经纬度 高度 定位状态
	STRUCT_TIME1 m_sTime;					 //排雷的时间
	u8 m_byMT_Type;							 //地雷实体类型			
	u8 m_byMT_Subtype;						 //地雷实体子类型		
	u8 m_byM_RID;							 //地雷单位代字 
	u8 m_aM_PID[2];						 //地雷实体设备编号	
}StructEvent_IndCleanMine;				 //单兵排雷事件(E_Type = EventType_IndCleanMine) 


__packed typedef struct{
	u8 m_byT_Type;							 //火箭布雷车实体类型			
	u8 m_byT_Subtype;						 //火箭布雷车实体子类型		
	u8 m_byRID;								 //火箭布雷车单位代字 
	u8 m_aPID[2];							 //火箭布雷车实体设备编号	
	u8 m_byC_Idegree;						 //火箭布雷车综合伤势 ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //发射火箭布雷弹时的地理位置 经纬度 高度 定位状态
	STRUCT_TIME1 m_sTime;					 //发射火箭布雷弹的时间
	u8 m_byA_Type;							 //发射弹药类型		
	u8 m_byA_Subtype;						 //发射弹药子类型
	u8 m_aRocketLayMineDirection[2];		 //发射火箭布雷弹的射向 0-6000 
	u8 m_aRocketLayMineAngle[2];			 //发射火箭布雷弹的射角 0-6000 
	u8 m_byRocketLayMineAmmuNum;			 //单次发射火箭布雷弹数量 
	u8 m_byOpenPositionT_Pick;				 //开仓引信延时时长选择  ENUM_FUSE_TYPE
	u8 m_byOpenPositionT_Span; 				 //开仓引信延时时间     0-9 	取值*0.1 秒 	范围：0.1 秒-0.9 秒 ; 10-69 （取值-9）*1 秒 	范围：1 秒-60 秒 
	u8 m_byMineDetonationT_Span; 			 //地雷自毁时间      0-9 	取值*0.1 秒 	范围：0.1 秒-0.9 秒 ; 10-69 （取值-9）*1 秒 	范围：1 秒-60 秒 
}StructEvent_RocketLayMine;				 //火箭布雷事件(E_Type = EventType_RocketLayMine) 


__packed typedef struct{
	u8 m_byT_Type;							 //扫雷车实体类型			
	u8 m_byT_Subtype;						 //扫雷车实体子类型		
	u8 m_byRID;								 //扫雷车单位代字 
	u8 m_aPID[2];							 //扫雷车实体设备编号	
	u8 m_byC_Idegree;						 //扫雷车综合伤势  ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //实施扫雷操作的地理位置 经纬度 高度 定位状态
	STRUCT_TIME1 m_sTime;					 //实施扫雷操作的时间
	u8 m_byMCM_Character;					 //扫雷方式标识字  ENUM_MCMCHAR
	u8 m_bySS_Character;					 //扫雷开关操作  ENUM_SSCHAR
}StructEvent_CarCleanMine;				 //扫雷车扫雷事件(E_Type = EventType_CarCleanMine) 

__packed typedef struct{
	u8 m_byT_Type;							 //破障车实体类型			
	u8 m_byT_Subtype;						 //破障车实体子类型		
	u8 m_byRID;								 //破障车单位代字 
	u8 m_aPID[2];							 //破障车实体设备编号	
	u8 m_byC_Idegree;						 //破障车综合伤势  ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //破障车的地理位置 经纬度 高度 定位状态
	STRUCT_TIME1 m_sTime;					 //发射时间 
	u8 m_byA_Type;							 //发射弹药类型		
	u8 m_byA_Subtype;						 //发射弹药子类型
	u8 m_aRocketCleanMineDirection[2];		 //发射火箭爆破弹的射向 0-6000 
	u8 m_aRocketCleanMineAngle[2];			 //发射火箭爆破弹的射角 0-6000 
	u8 m_byRocketCleanMineAmmuNum;			 //单次发射火箭爆破弹数量 
}StructEvent_RocketCleanMine;			 //火箭扫雷事件(E_Type = EventType_RocketCleanMine) 


__packed typedef struct{
	u8 m_byT_Type;							 //实体类型			
	u8 m_byT_Subtype;						 //实体子类型		
	u8 m_byRID;								 //单位代字 
	u8 m_aPID[2];							 //实体设备编号	
	u8 m_byC_Idegree;						 //恶化后综合伤势  ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS L_m_sPosition ;			 //伤势恶化的地理位置 经纬度 高度 定位状态
	STRUCT_TIME1 m_sTime;					 //伤势恶化时间 
}StructEvent_InjuryWorse;				 //人员伤势恶化事件(E_Type = EventType_InjuryWorse) 

__packed typedef struct{
	u8 m_byT_Type;							 //人员实体类型			
	u8 m_byT_Subtype;						 //人员实体子类型		
	u8 m_byRID;								 //人员单位代字 
	u8 m_aPID[2];							 //人员实体设备编号	
	u8 m_byC_Idegree;						 //被救护后综合伤势 ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //被救护时地理位置 经纬度 高度 定位状态
	STRUCT_TIME1 m_sRescueStartTime;		 //救护开始的时间 
	STRUCT_TIME1 m_sRescueEndTime;			 //救护结束的时间 
	u8 m_byRT_Type;							 //施救人员实体类型			
	u8 m_byRT_Subtype;						 //施救人员实体子类型		
	u8 m_byR_RID;							 //施救人员单位代字 
	u8 m_aR_PID[2];						 //施救人员实体设备编号	
	u8 m_byCM_Characte;						 //救治方式标识字	ENUM_CMCHAR
	u8 m_byCE_Characte;						 //人员救治结果标识字	ENUM_CECHAR
	
}StructEvent_IndRescued;				 //单兵救护事件(E_Type = EventType_IndRescued) 


__packed typedef struct{
	u8 m_byT_Type;							 //战场障碍实体类型			
	u8 m_byT_Subtype;						 //战场障碍实体子类型		
	u8 m_byRID;								 //战场障碍单位代字 
	u8 m_aPID[2];							 //战场障碍实体设备编号	
	u8 m_bySlaveNum;						 //战场障碍从机数量（n，n=0 时，从机地理位置区无内容） 
	STRUCT_LPOS m_sL_Position ;			 //战场障碍主机地理位置 经纬度 高度 定位状态
	STRUCT_LPOS m_sSL_Position[3] ;		 //战场障碍从机地理位置 经纬度 高度 定位状态
	STRUCT_TIME1 m_sArrangeHinderTime;		 //战场障碍设置完成时间  
}StructEvent_ArrangeHinder;				 //战场障碍设置事件(E_Type = EventType_ArrangeHinder) 


__packed typedef struct{
	u8 m_byT_Type;							 //火力点实体类型			
	u8 m_byT_Subtype;						 //火力点实体子类型		
	u8 m_byRID;								 //火力点单位代字 
	u8 m_aPID[2];							 //火力点实体设备编号	
	u8 m_byC_Idegree;						 //火力点综合伤势 ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //火力点地理位置 经纬度 高度 定位状态
	STRUCT_TIME1 m_sCtrlFirePointTime;		 //火力点设置完成时间
	STRUCT_TIME1 m_sFireStartTime;	 		 //火力点开始扫射的时间（当该值全为 0 时，火力点通过手动发射，发射弹药间隔 0.2 秒，轮流时间 2 秒）
	STRUCT_TIME1 m_sFireStopTime;	 		 //火力点开始停止的时间（当该值全为 0 时，火力点通过手动发射，发射弹药间隔 0.2 秒，轮流时间 2 秒）
	u8 m_byFireInterval;					 //火力点发射弹药时间间隔   10 毫秒
	u8 m_byTurnFireInterval;				 //火力点轮流发射间隔时间   0-9 	取值*0.1 秒 	范围：0.1 秒-0.9 秒 ;	10-69 	（取值-9）*1 秒 	范围：1 秒-60 秒 
	u8 m_byA_Type;							 //发射弹药类型		
	u8 m_byA_Subtype;						 //发射弹药子类型
	
}StructEvent_RSP_CtrlFirePoint;				 //火力点设置事件(E_Type = EventType_RSP_CtrlFirePoint) 

__packed typedef struct{
	u8 m_byT_Type;							 //实体类型			
	u8 m_byT_Subtype;						 //实体子类型		
	u8 m_byRID;								 //单位代字 
	u8 m_aPID[2];							 //实体设备编号	
	STRUCT_LPOS m_sL_Position ;			 //地理位置 经纬度 高度 定位状态
	STRUCT_TIME1 m_sTime;					 //时间 
}StructEvent_StatusRecovery;			 //状态自动恢复事件(E_Type = EventType_StatusRecovery) 

__packed typedef struct{
	u8 m_byT_Type;							 //工程侦察车实体类型			
	u8 m_byT_Subtype;						 //工程侦察车实体子类型		
	u8 m_byRID;								 //工程侦察车单位代字 
	u8 m_aPID[2];							 //工程侦察车实体设备编号	
	u8 m_byC_Idegree;						 //工程侦察车综合伤势 ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //工程侦察车地理位置 经纬度 高度 定位状态
	STRUCT_TIME1 m_sTime;					 //时间
	u8 m_byMT_Subtype;						 //雷场类型
	STRUCT_LPOS m_sML_Position[3] ;		 //雷场顶点位置
	
}StructEvent_RSP_CtrlEngineerRecon;		 //工程侦察事件(E_Type = EventType_RSP_CtrlEngineerRecon) 

__packed typedef struct{
	u8 m_byT_Type;							 //车辆实体类型			
	u8 m_byT_Subtype;						 //车辆实体子类型		
	u8 m_byRID;								 //车辆单位代字 
	u8 m_aPID[2];							 //车辆实体设备编号	
	u8 m_byC_Idegree;						 //车辆综合伤势 ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //实施抢修地理位置 经纬度 高度 定位状态
	STRUCT_TIME1 m_sRepairStartTime;		 //抢修开始的时间 
	STRUCT_TIME1 m_sRepairEndTime;			 //抢修结束的时间 
	u8 m_byET_Type;							 //抢修车实体类型			
	u8 m_byET_Subtype;						 //抢修车实体子类型		
	u8 m_byE_RID;							 //抢修车单位代字 
	u8 m_aE_PID[2];						 //抢修车实体设备编号	
	
}StructEvent_EquipRepair;				 //装备抢修事件(E_Type = EventType_EquipRepair) 

__packed typedef struct{
	u8 m_byT_Type;							 //实体类型			
	u8 m_byT_Subtype;						 //实体子类型		
	u8 m_byRID;								 //单位代字 
	u8 m_aPID[2];							 //实体设备编号	
//	u8 m_byC_Idegree;						 //综合伤势 ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //模式转换时的地理位置  经纬度 高度 定位状态
	STRUCT_TIME1 m_sChangeToManoTime;		 //模式转换的时间
	
	u8 m_byCT_Type;							 //实施模式转换实体的类型			
	u8 m_byCT_Subtype;						 //实施模式转换实体的子类型	
	u8 m_byC_RID;							 //实施模式转换实体的单位代字  
	u8 m_aC_PID[2];						     //实施模式转换实体的设备编号
  STRUCT_TS_Character1 m_sTS_Character1;  //实体状态字 1
	
}StructEvent_ChangeToMano;				 //演习模式转换事件(E_Type = EventType_ChangeToMano) 

__packed typedef struct{
	u8 m_byT_Type;							 //人员实体类型			
	u8 m_byT_Subtype;						 //人员实体子类型		
	u8 m_byRID;								 //人员单位代字 
	u8 m_aPID[2];							 //人员实体设备编号	
	STRUCT_LPOS m_sL_Position ;			 //人员地理位置  经纬度 高度 定位状态
	STRUCT_TIME1 m_sWeaponOnlineTime;		 //绑定武器时间
	u8 m_byCT_Type;							 //绑定武器实体类型		
	u8 m_byCT_Subtype;						 //绑定武器实体子类型
	u8 m_byC_RID;							 //绑定武器单位代字  
	u8 m_aC_PID[2];						 //绑定武器设备编号
	
}StructEvent_IndWeaponOnline;			 //单兵武器绑定事件(E_Type = EventType_IndWeaponOnline) 


__packed typedef struct{
	u8 m_byT_Type;							 //实体类型			
	u8 m_byT_Subtype;						 //实体子类型		
	u8 m_byRID;								 //单位代字 
	u8 m_aPID[2];							 //实体设备编号	
	u8 m_byC_Idegree;						 //综合伤势 ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //地理位置  经纬度 高度 定位状态
	STRUCT_TIME1 m_sOnOffTime;		 		 //开机/关机时间
	u8 m_byPO_Character ;					 //开机/关机操作标识字		 ENUM_POCHAR
	
}StructEvent_TerminalOnOff;				 //开关机事件(E_Type = EventType_TerminalOnOff) 

__packed typedef struct{
	u8 m_byT_Type;							 //实体类型			
	u8 m_byT_Subtype;						 //实体子类型		
	u8 m_byRID;								 //单位代字 
	u8 m_aPID[2];							 //实体设备编号	
	u8 m_byC_Idegree;						 //综合伤势 ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //地理位置  经纬度 高度 定位状态
	STRUCT_TIME1 m_sTime;		 		 	 //时间
	u8 m_byIE_Type ;					 	 //违规事件子类		 ENUM_IE_TYPE
	
}StructEvent_AgaistRuleStart;			 //违规开始事件(E_Type = EventType_AgaistRuleStart) 

__packed typedef struct{
	u8 m_byT_Type;							 //实体类型			
	u8 m_byT_Subtype;						 //实体子类型		
	u8 m_byRID;								 //单位代字 
	u8 m_aPID[2];							 //实体设备编号	
	u8 m_byC_Idegree;						 //综合伤势 ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER 
	STRUCT_LPOS m_sL_Position ;			 //地理位置  经纬度 高度 定位状态
	STRUCT_TIME1 m_sStartTime;	 			 //开始时间
	STRUCT_TIME1 m_sStopTime;	 			 //结束时间
	u8 m_byIE_Type ;					 	 //违规事件子类		 ENUM_IE_TYPE	
}StructEvent_AgaistRuleStop;			 //违规结束事件(E_Type = EventType_AgaistRuleStop) 

__packed typedef struct
{
    u8 m_aData_Frame[2];			//数据帧头，为 0xA8，0x93
    u8 m_byD_Len;					//数据帧长度

    __packed union
    {
        StructEvent_RSP_CtrlManoParam_NotIndividualSoldier m_sRSP_CtrlManoParam_NotIndividualSoldier;	//演习参数设置成功事件（非单兵） EventType_RSP_CtrlManoParam_NotIndividualSoldier
        StructEvent_RSP_CtrlManoParam_IndividualSoldier m_sRSP_CtrlManoParam_IndividualSoldier;		//演习参数设置成功事件（单兵）	EventType_RSP_CtrlManoParam_IndividualSoldier
        StructEvent_RSP_CtrlInterval m_sRSP_CtrlInterval;												//上传周期设置成功事件	EventType_RSP_CtrlInterval
        StructEvent_RSP_CtrlSupplement m_sRSP_CtrlSupplement;											//弹药（物资）分发/补给事件  EventType_RSP_CtrlSupplement
        StructEvent_DamageReport m_sDamageReport;														//火力毁伤事件  EventType_DamageReport
        StructEvent_BNJudge m_sJudge;																	//导控裁决事件  EventType_Judge
        StructEvent_NCBT m_sNCBT;																		//进入/离开核化生沾染区事件  EventType_NCBT
        StructEvent_NCB_Protection m_sNCB_Protection;													//核化生防护事件  EventType_NCB_Protection
        StructEvent_NCB_Injury m_sNCB_Injury;															//核化生毁伤事件  EventType_NCB_Injury
        StructEvent_PE_Threat m_sPE_Threat;															//光电威胁事件  EventType_PE_Threat
        StructEvent_PE_Protection m_sPE_Protection;													//烟雾/诱导防护事件  EventType_PE_Protection
        StructEvent_DigitIndirectLanuch m_sDigitIndirectLanuch;										//数字化火炮发射事件  EventType_DigitIndirectLanuch
        StructEvent_NonDigitIndirectLanuch m_sNonDigitIndirectLanuch;									//非数字化火炮发射事件  EventType_NonDigitIndirectLanuch
        StructEvent_Radar m_sRadar;																	//炮位侦察校射雷达侦察/校射事件  EventType_Radar
        StructEvent_HeavyWpnLaunch m_sHeavyWpnLaunch;													//直瞄重武器发射事件  EventType_HeavyWpnLaunch
        StructEvent_IndLayMine m_sIndLayMine;															//单兵布雷事件  EventType_IndLayMine
        StructEvent_IndCleanMine m_sIndCleanMine;														//单兵排雷事件  EventType_IndCleanMine
        StructEvent_RocketLayMine m_sRocketLayMine;												    //火箭布雷事件  EventType_RocketLayMine
        StructEvent_CarCleanMine m_sCarCleanMine;													    //扫雷车扫雷事件  EventType_CarCleanMine
        StructEvent_RocketCleanMine m_sRocketCleanMine;												//火箭扫雷事件  EventType_RocketCleanMine
        StructEvent_InjuryWorse m_sInjuryWorse;														//人员伤势恶化事件  EventType_InjuryWorse
        StructEvent_IndRescued m_sIndRescued;												    		//单兵救护事件  StructEvent_IndRescued
        StructEvent_ArrangeHinder m_sArrangeHinder;													//战场障碍设置事件  EventType_ArrangeHinder
        StructEvent_RSP_CtrlFirePoint m_sRSP_CtrlFirePoint;											//火力点设置事件  EventType_RSP_CtrlFirePoint
        StructEvent_StatusRecovery m_sStatusRecovery;													//状态自动恢复事件  EventType_StatusRecovery
        StructEvent_RSP_CtrlEngineerRecon m_sRSP_CtrlEngineerRecon;									//工程侦察事件  EventType_RSP_CtrlEngineerRecon
        StructEvent_EquipRepair m_sEquipRepair;														//装备抢修事件   EventType_EquipRepair
        StructEvent_ChangeToMano m_sChangeToMano;														//演习模式转换事件   EventType_ChangeToMano
        StructEvent_IndWeaponOnline m_sIndWeaponOnline;												//单兵武器绑定事件   EventType_IndWeaponOnline
        StructEvent_TerminalOnOff m_sTerminalOnOff;													//开关机事件   EventType_TerminalOnOff
        StructEvent_AgaistRuleStart m_sAgaistRuleStart;												//违规开始事件   EventType_AgaistRuleStart
        StructEvent_AgaistRuleStop m_sAgaistRuleStop;													//违规结束事件   EventType_AgaistRuleStop

        u8 m_aBuffer[120];
    } Event_Pack;				//事件数据（长度 n = D_Len）
    u8 m_aES_Num[2];				//事件数据帧序列号
    u8 m_byXor  ;					//异或和
} Struct_EventUpload;			//事件上传

typedef struct
{
    u8 HelicopterVoiceFlg: 1;
    u8 AeroplaneVoiceFlg: 1;
    u8 IndirectGunVoiceFlg: 1;
    u8 res: 5;

    u8 HelicopterVoiceCnt;
    u8 AeroplaneVoiceCnt;
    u8 IndirectGunVoiceCnt;
} SOUND_SIMU_CTRL;	//模拟声效

typedef struct
{

    RESCOM_LOWLEVEL_MAG *m_pLLMag;

    uint32_t 	m_bIsInit: 1;   //任务初始化
    uint32_t 	m_bIsRecvNew: 1; //接收到新数据
    uint32_t 	m_bRes1: 30;

    SCOMTASK_MAG m_sTaskMag;

    uint8_t m_IsAccess: 1;		//入网标记
		uint8_t m_ManoParamFlg:1;
    uint8_t m_byRes1: 6;		//保留
		uint8_t m_byNuclearFlg;     //核威胁标志
    uint8_t m_byBiologyFlg;     //生物威胁标志
    uint8_t m_byChemistryFlg;   //化学威胁标志
    uint8_t NBC_Strength;
	
		DRILL_PARAM DrillParam;		//演习配置参数

		uint8_t EvtListResPonseNum[240];		//事件链接收到回复
		uint8_t EvtRspFlg[30];		//上传事件响应标记改为30条
		uint8_t EvtRspFlgBak[30];
		uint16_t UnUpLoadEvtCnt;

		Struct_EventUpload m_sEvtUploadMag;
						
		SOUND_SIMU_CTRL SoundSimulation;
		
		uint32_t m_dwCtrlEvtNum;		//导控事件编号
		
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
		uint8_t m_byRes2: 5;		//保留
		
		NMEA_MAG m_sNmeaMag;
		FOURGMAG m_sFourGMag;
		
		PARAMATER_BACKUP BackupArea[2];
		BackStage_CmdTimeMag_t m_sCmdTime;
} RESCOMTASK_MAG;

extern RESCOMTASK_MAG g_sResComTask_Mag;

typedef struct
{
	uint8_t		m_byHead;		//帧头（固定为0xA8）
	uint8_t		m_byCmd;		//命令码（固定为 Cmd_Terminal_AccessRequest）
	uint8_t 	m_byD_Len;		//数据帧长度 
	uint8_t 	m_byT_Type;		//实体类型
	uint8_t 	m_byT_Subtype;	//实体子类型 
	uint8_t 	m_byRID;		//单位代字 
	uint16_t 	m_wPID;			//设备编号(大端模式)
	uint8_t 	m_byXor;		//数据帧异或和
}StructBNTerminal_AccessRequest;

uint8_t GetInjuryDegree(uint8_t Blood);
uint8_t GetInjuryBloodByHurtDegree(ENUM_CI_INDIVIDUAL InjuryDegree);
/****************************************************************
@FUNCTION：获取保留端口任务管理句柄
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 

****************************************************************/
RESCOMTASK_MAG *ResComTask_HandleGet(void);
SCOMTASK_MAG *ResComTask_TaskHandleGet(void);

/****************************************************************
@FUNCTION：保留串口功能初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sResComMag

****************************************************************/
void ResComTask_Init(void);

/****************************************************************
@FUNCTION：保留串口功能反初始化
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sResComMag

****************************************************************/
void ResComTask_DeInit(void);

/****************************************************************
@FUNCTION：保留串口功能任务主进程
@INPUT： 无
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sResComMag

****************************************************************/
void ResComTask_Process(void);

void RecordRcvBackStageProtocolTime(StructUTC * Time);
uint8_t BNSoldierManoParamSetHandle(Struct_CtrlManoParam_IndividualSoldierDownload ManoParam);
void CtrlManoParamEventUploadHandle(Struct_CtrlManoParam_IndividualSoldierDownload *ManoParam);
//单兵布雷事件上传
void PersonnelLayMineEventUpload_Send(Struct_EventUpload *pMag);
//排雷
void PersonnelClearMineEventUpload_Send(Struct_EventUpload *pMag);
void ResComTask_SendToUpcom(uint8_t *pBuf, uint16_t BufLen, uint8_t DevIdx, DEVCTRL eDevCtl);
void BSN_RescureEvtHandle(StructEvent_IndRescued *pMag);

void CtrlPJPShotEventUploadHandle(STRUCT_Various_Elements *pMag);
#endif



