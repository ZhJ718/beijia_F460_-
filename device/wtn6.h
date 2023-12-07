/******************************************************************
*版权所有 (C)2018, 北京华如科技南京分公司
*
*文件名称:  wtn6.h
*文件标识:
*内容摘要:
*其它说明:
*当前版本:
*作    者:
*完成日期:  2018/7/11
*
*修改记录1:
*    修改日期:
*    版 本 号:
*    修 改 人:
*    修改内容:
******************************************************************/
#ifndef  _WTN6_H
#define  _WTN6_H
/******************************************************************
*                             头文件                              *
******************************************************************/

#include "config.h"


/******************************************************************
*                           宏定义                                *
******************************************************************/

#define ENABLE_WTN6_1   //背甲默认
//#define ENABLE_WTN6_2 //头盔默认

/******************************************************************
*                           结构体定义                          *
******************************************************************/

typedef enum
{
    WTN6_Reserve = 0,		//留空
    WTN6_SystemStart,		//系统启动
    WTN6_SystemStop,		//系统关机
    WTN6_ClothConnected,		//背甲已连接
    WTN6_WeaponConnected,		//武器已连接
    WTN6_SQConnected,		//手枪已连接
    WTN6_BQConnected,		//步枪已连接
    WTN6_JQConnected,		//机枪已连接
    WTN6_JJQConnected,		//狙击枪已连接
    WTN6_XDQConnected,		//霰弹枪已连接
    WTN6_LDFSQConnected = 10,		//榴弹发射器已连接
    WTN6_DBHJTConnected,		//单兵火箭筒已连接
    WTN6_ClothDisconnected,		//背甲连接断开
    WTN6_WeaponDisconnected,		//武器连接断开
    WTN6_SQDisconnected,		//手枪连接断开
    WTN6_BQDisconnected,		//步枪连接断开
    WTN6_JQDisconnected,		//机枪连接断开
    WTN6_JJQDisconnected,		//狙击枪连接断开
    WTN6_XDQDisconnected,		//霰弹枪连接断开
    WTN6_LDFSQDisconnected,		//榴弹发射器连接断开
    WTN6_DBHJTDisconnected = 20,		//单兵火箭筒连接断开

    WTN6_Shot_Head,		//头部中弹
    WTN6_Shot_LeftArm,		//左臂中弹
    WTN6_Shot_RightArm,		//右臂中弹
    WTN6_Shot_LeftLeg,		//左腿中弹
    WTN6_Shot_RightLeg,		//右腿中弹
    WTN6_Shot_LeftChest,		//左胸中弹
    WTN6_Shot_RightChest,		//右胸中弹
    WTN6_Shot_Back,		//后背中弹
    WTN6_Shot_Abdomen,		//腹部中弹

    WTN6_GrenadeExplosion = 30,		//手雷爆炸
    WTN6_ShellHit,		//间瞄炮弹击中
    WTN6_AirAttack,		//空中火力袭击

    WTN6_OutRange,		//超距武器锁死
    WTN6_WeaponUnlock,		//武器解锁

    WTN6_ExhaustAmmunition,		//弹量用尽
    WTN6_ReplenishAmmunition,		//补弹成功
    WTN6_Revive,		//裁判复活
    WTN6_JudgeDie,		//裁判死亡
    WTN6_Injury,		//已受伤
    WTN6_Killed = 40,		//已阵亡

    WTN6_YX_Shoot1,		//步枪射击1
    WTN6_YX_Shoot2,		//步枪射击2
    WTN6_YX_MineExplosion,		//地雷爆炸

    WTN6_YX_JQ,		//机枪
    WTN6_YX_JJ1,		//狙击1
    WTN6_YX_JJ2,		//狙击2
    WTN6_YX_PDFG,		//炮弹飞过
    WTN6_YX_PDBZ,		//炮弹飞过落地爆炸
    WTN6_YX_SL,		//手雷爆炸
    WTN6_YX_ZDBZ = 50,		//炸弹爆炸
    WTN6_YX_ZDBZ2,		//炸弹爆炸2
    WTN6_YX_ZDBZ3,		//炸弹爆炸3
    WTN6_YX_ZJQ,		//重机枪
    WTN6_YX_ZDHX1,		//子弹呼啸1
    WTN6_YX_ZDHX2,		//子弹呼啸2

		WTN6_LowBattery_Helmet,			//头盔电量不足。
		WTN6_Helmet_Already_Connect,	//头盔已连接。
		WTN6_Helmet_Disconnect,			//头盔连接断开。
		WTN6_CHANGE_CLIP,				//更换弹夹。	
		WTN6_CLIP_CHANGE_SUCCESS = 60,	//弹夹更换成功。TODO
		
    //WTN6_LowBattery_Master,		//头盔电量不足
    //WTN6_ReplaceBat_10Min_Master,		//头盔十分钟内更换电池
    //WTN6_ReplaceBat_5Min_Master,		//头盔五分钟内更换电池
    //WTN6_ReplaceBat_3Min_Master,		//头盔三分钟内更换电池
    //WTN6_LowPowerStop_Master,		//头盔电量不足自动关机

    WTN6_Connect_SuperBle,		//连接超级蓝牙设备

    WTN6_YX_MineExplosion_Ex,		//地雷爆炸
    WTN6_Mine_Clearance,		//排雷成功
    WTN6_Aiding,		//卫勤救护中
    WTN6_Aid_Finish,		//卫勤救护结束

    WTN6_Slight_Injury,			//轻伤需要救护
    WTN6_Moderate_Injury,		//中度伤需要救护
    WTN6_Severe_Injury,		//重度伤需要救护
    WTN6_Injury_Worse,		//伤情加重
    WTN6_Number_1 = 70,		//数字1
    WTN6_Number_2,		//数字2
    WTN6_Number_3,		//数字3
    WTN6_Death_Cheat,		//作弊判死
    //WTN6_Death_Timeout_Start,		//超时开机判死
		WTN6_HELMET_DISCONNECT,			//头盔连接断开
    WTN6_Death_Illegal_Stop,		//非法关机判死
    WTN6_GPSTrouble,		//定位模块故障

    WTN6_Cloth_IRTrouble,		//背甲探头故障
    WTN6_Master_IRTrouble,		//头部探头故障
    WTN6_ClothTrouble,		//背甲主控故障
    WTN6_MasterTrouble = 80,		//头部主控故障
    //WTN6_WeaponTrouble,		//武器故障
		WNT6_MINE_REMOVE_FAIL,          //排雷失败。
    WTN6_Mode_Train,		//训练模式
    WTN6_Mode_Mano,		//演习模式

    WTN6_LowBattery_IR,		//发射机电量不足
		
		WTN6_MINE_SET_SUCCESS,		//布雷成功.
		WTN6_MINE_SET_FAIL,		//布雷失败.
		WTN6_SAVE_SUCCESS,		//救护成功.
		WTN6_SAVE_FALI,		//救护失败.
		
		WTN6_LowBattery_Cloth,		//背甲电量不足.
    WTN6_ReplaceBat_10Min_Cloth,		//背甲十分钟内更换电池
    WTN6_ReplaceBat_5Min_Cloth,		//背甲五分钟内更换电池
    WTN6_ReplaceBat_3Min_Cloth,		//背甲三分钟内更换电池
    WTN6_LowPowerStop_Cloth,		//背甲电量不足自动关机
		
		WTN6_START_SAVE = 90,		//开始救护.
		WTN6_STOP_SAVE,		//结束救护.
		WTN6_DAY_MODEL,		//白天模式.
		WTN6_NIGHT_MODE,		//夜间模式.
		WTN6_CHEAT_EVENT,		//作弊事件.
		WTN6_HITCH,		//故障。
		WTN6_MANO_PARAM_CFG_SUCCESS,		//演习参数配置成功。
		WTN6_TERMINAL_ONLINE,		//终端入网。
		WTN6_TERMINAL_OFFLINE,		//终端离线。
		WTN6_LOCATION_SUCCESS,		//定位成功。
		WTN6_EXPLOSIVE_DEVICE_NOT_INSTALL=100,		//火工品未安装。
		WTN6_ILLEGAL_MOVE,		//非法移动。
		WTN6_ILLEGAL_TURNOFF,		//非法关机。
		WTN6_TEST_MODEL,		//测试模式。
		WTN6_PAIR_SUCCESS,		//配对成功。
		WTN6_REMOVE_PAIR,		//解除配对。
		WTN6_ATUO_POWEROFF,		//自动关机。
		WTN6_PLS_CHANGE_BATTERY,		//请更换电池。
		WTN6_DEVICE_CONNECT,		//设备已连接。
		WTN6_DEVICE_DISCONNECT,		//设备连接断开。
		WTN6_NULL = 110,		//【留空】。
		WTN6_HELMET_NOT_POWERON,		//头盔未开机。
		WTN6_BLE_DEVICE_HITCH,		//蓝牙模块故障。
	
    //WTN6_ReplaceBat_10Min_IR,		//发射机十分钟内更换电池
    //WTN6_ReplaceBat_5Min_IR,		//发射机五分钟内更换电池
    //WTN6_ReplaceBat_3Min_IR,		//发射机三分钟内更换电池
    //WTN6_LowPowerStop_IR,		//发射机电量不足自动关机

} ENUM_WTN6_CONTENT;


/******************************************************************
*                           全局变量声明                          *
******************************************************************/


//缓存需要播放声音的环形缓冲区
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
*                         全局函数声明                            *
******************************************************************/

//语音芯片硬件操作
typedef struct
{
    //硬件接口初始化函数
    void (*hw_init)(void);
    //数据线拉高操作
    void (*data_high)(void);
    //数据线拉低操作
    void (*data_low)(void);
    //忙检测
    uint32_t (*is_busy)(void);
} WTN6_HW_OPERATIONS;


//语音芯片逻辑操作
typedef struct
{
    //us休眠函数
    void (*sleep_us)(uint16_t);
    //语音芯片时序操作，异步
    void (*step_ops)(void *);

} WTN6_SOFT_OPERATIONS;

typedef struct
{
    volatile u8 m_IsHigh: 1;	//数据线状态 0-低电平 1-高电平
    volatile u8 m_IsRes: 7;

    volatile u8 m_byRes;		//保留
    volatile u16 m_wDelayUs;//延时时间
} WTN6_STEP_MAG;

#define WTN6_MAG_STEP_MAX 20 //最大分步

typedef struct
{
    WTN6_HW_OPERATIONS m_sHwOps;
    WTN6_SOFT_OPERATIONS m_sSoftOps;

    volatile u8 m_byNowStep; 	//当前进行到哪步
    volatile u8 m_byMaxStep;		//总共多少步
    volatile u8 m_byRes2;
    volatile u8 m_byRes3;	//保留字节，对齐用

    WTN6_STEP_MAG m_sSingle[WTN6_MAG_STEP_MAX];
} WTN6_MAG;

#if defined(ENABLE_WTN6_1)
    extern WTN6_MAG g_sWTN6RightMag;
#endif

#if defined(ENABLE_WTN6_2)
    extern WTN6_MAG g_sWTN6LeftMag;
#endif

/****************************************************************
@FUNCTION：完成左右声道语音模块初始化
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 初始化结构体g_sWTN6LeftMag、g_sWTN6RightMag，填充硬件操作函数和异步操作逻辑函数
****************************************************************/
void Wtn6_1_2_Init(void);

/****************************************************************
@FUNCTION：语音芯片定时任务，根据播放队列中内容，开启播放
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 枪声可能会打断当前播放，其他语音等待上次语音播放完毕才会播放
****************************************************************/
void  Wtn6_TimedProcess(void);


/****************************************************************
@FUNCTION：右声道播放语音
@INPUT：语音编号
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 此函数调用后，语音可能不会立即播放
****************************************************************/
void Wtn6_RPlay(u8 Voice);

/****************************************************************
@FUNCTION：左声道播放语音
@INPUT：语音编号
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 此函数调用后，语音可能不会立即播放
****************************************************************/
void Wtn6_LPlay(u8 Voice);



/****************************************************************
@FUNCTION：根据传入的语音，分到左右声道，插入播放队列
@INPUT：语音编号
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 此函数调用后，语音可能不会立即播放
****************************************************************/
void Wtn6_Play(u8 Voice);



/******************************************************************
*函数名称:play_online
*功能描述:播放装备上线语音
*输入参数:
*输出参数:
*返回值:
*其它说明:
*修改日期    版本号   修改人    修改内容
*---------------------------------------------------
*                              查飞
******************************************************************/
void play_online(u8 equipemtid);


/******************************************************************
*函数名称:play_offline
*功能描述:播放装备下线语音
*输入参数:
*输出参数:
*返回值:
*其它说明:
*修改日期    版本号   修改人    修改内容
*---------------------------------------------------
*                              查飞
******************************************************************/
void play_offline(u8 equipemtid);


/******************************************************************
*函数名称:play_Status
*功能描述:播放主体或武器当前状态
*输入参数:当前状态 参见 ENUM_UNITE_STATE //1-正常 2-阵亡锁死 3-后台锁死 4-后台复活 5-裁判锁死 6-裁判复活 7-超距锁死 8-缺弹锁死
*输出参数:
*返回值:
*其它说明:
*修改日期    版本号   修改人    修改内容
*---------------------------------------------------
*                              查飞
******************************************************************/
void play_Status(u8 Sta);


/******************************************************************
*函数名称:play_StruckPart
*功能描述:播放中弹部位
*输入参数:
*输出参数:
*返回值:
*其它说明:
*修改日期    版本号   修改人    修改内容
*---------------------------------------------------
*                              查飞
******************************************************************/
void play_StruckPart(ENUM_PARTITION part);



//播放射击声
void play_Shot(u8 weaponid, u8 Sta, u8 Num);



extern void Wtn6_StartLSound(uint8_t Dat);


extern void Wtn6_StartRSound(uint8_t Dat);

extern void Wtn6_Sound(uint8_t Dat);

//夜间音量
extern void Wtn6_NightSound(void);

//白天音量
extern void Wtn6_DaySound(void);

#endif

