#ifndef __CONFIG_H_
#define __CONFIG_H_


#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include "metadata.h"


#if defined(AT32F4xx)
#include "at32f4xx.h"
#elif defined(HC32F46x)
#include "hc32f46x.h"
#include "hc32_ddl.h"
#endif

#include "drv_config.h"


#define GETDWORD(addr) (*(uint32_t *)(addr))
#define GETWORD(addr) (*(uint16_t *)(addr))

#define SWAP_DWORD(x)	((((x)<<24)&0xFF000000)|(((x)<<8)&0xFF0000)|(((x)>>8)&0xFF00)|(((x)>>24)&0xFF))
#define SWAP_WORD(x)	((((x)>>8)&0xFF)|(((x)<<8)&0xFF00))
#define SWAP_LONG(x)	((int32_t)((((x)<<24)&0xFF000000)|(((x)<<8)&0xFF0000)|(((x)>>8)&0xFF00)|(((x)>>24)&0xFF)))
#define SWAP_SHORT(x)	((int32_t)((((x)>>8)&0xFF)|(((x)<<8)&0xFF00)))


typedef unsigned int u32;
typedef unsigned short u16; 
typedef unsigned char u8;

//函数指针
typedef void (*PTRFUNC)(void); 


//#define __STATIC_INLINE static __INLINE

#define TRUE 1
#define FALSE 0

#define RET_TRUE  0
#define RET_FALSE -1


#define MAINVERSION 0x06
//合成营项目
#define SUBVERSION 0x01
//5308项目
//#define SUBVERSION 0x02
//海陆项目
//#define SUBVERSION 0x03
//新疆11S
//#define SUBVERSION 0x04

#define FIXVERSION 0x01

//#define BOARDID 0x01		//板子编号01,头盔
#define BOARDID 0x02		//板子编号02，背甲

//#define SEND_TO_UPCOM   //上传到上行口

#ifdef _APP
//日志输出控制
#define OPEN_MPU6050				//打开姿态模块功能
//#define DEBUG    //使能打印输出 
#undef DEBUG

#endif

//#define TEST_SHELL
#ifdef DEBUG

//#define ZIG_COM_LOG_OUTPUT  //使能zigbee模块日志输出
//#define LASER_COM_LOG_OUTPUT  //使能zigbee模块日志输出
//#define GPS_COM_LOG_OUTPUT  //使能GPS模块日志输出
//#define IR38_COM_LOG_OUTPUT  //使能红外短距模块日志输出
//#define HMI_COM_LOG_OUTPUT //使能HMI模块日志输出

#endif

#ifndef assert_param
#define assert_param(expr) ((void)0)
#endif

#ifndef DDL_ASSERT
#define DDL_ASSERT 
#endif

#define WEAPONKIND_MAX  3
#define BULLETKIND_MAX  6

#define NO_WARRANT
#define REMAINDER_DEFAULT	50	//试用期可开机次数


#define		HIBYTE(x)				(uint8_t)(((x)>>8)&0xFF)
#define		LOBYTE(x)				(uint8_t)((x)&0xFF)
#define		WORD_TO_BCD(wdata)		(((wdata)%10)|((((wdata)/10)%10)<<4)|((((wdata)/100)%10)<<8)|(((wdata)/1000)<<12))		/*用于转换最多9999范围的BCD，转换后以WORD存储*/

#define		GET_TAB_LEN(pTab)			(sizeof(pTab)/sizeof(pTab[0]))	

#ifndef HC32F46x
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))
    
//终端状态
#define TRAIN 			0		// 训练
#define GAME_PREP 		1		// 演习 预备
#define GAME_GOING 		2		//演习 开始
#define GAME_OVER 		3		//演习 结束

//终端状态1 -正常 2-阵亡锁死 3-后台锁死 4-后台复活 5-裁判锁死 6-裁判复活 7-超距锁死 8-缺弹锁死  
typedef enum{
	Sta_Normal=1,
	Sta_Die,
	Sta_BackstageDie,
	Sta_BackstageRevive,
	Sta_JudgeDie,
	Sta_JudgeRevive,
	Sta_OutRangeLock,
	Sta_LackAmmLock,
}ENUM_UNITE_STATE;

#define IsDie(x) (((x)==Sta_Die)||((x)==Sta_JudgeDie)||((x)==Sta_BackstageDie))
#define IsAlive(x) (((x)==Sta_Normal)||((x)==Sta_BackstageRevive)||((x)==Sta_JudgeRevive))


#define S_MEMBER_BY(name) uint8_t m_by##name
#define S_MEMBER_W(name) uint16_t m_w##name
#define S_MEMBER_DW(name) uint8_t m_dw##name
#define S_MEMBER_S(name) uint32_t m_s##name
#define S_MEMBER_A(name) uint32_t m_a##name
#define S_MEMBER_P(name) uint32_t m_p##name
#define S_MEMBER_E(name) uint32_t m_e##name
#define S_MEMBER_B(name,bitn) uint32_t m_b##name:##bitn


//操作模式
typedef enum{
	OPT_MODE_BLOCK=0,		//阻塞模式
	OPT_MODE_NONBLOCK	//非阻塞模式
}OPT_MODE;



//硬件操作
typedef struct
{
    //硬件初始化操作
    void (*hw_init)(void);
    
    //读操作
    uint32_t (*read)(uint8_t *, uint16_t *);
    //写操作
    uint32_t (*write)(uint8_t *, uint16_t );
    
    //上电
    void (*gpio_power_on)(void);
    //断电
    void (*gpio_power_off)(void);

    //硬件复位引脚拉低
    void (*gpio_reset)(void);
    //硬件复位引脚拉高
    void (*gpio_set)(void);

} COM_HW_OPERATIONS;

#define FRAME_BUF_LEN_MAX 230

typedef struct 
{
    uint32_t m_dwID;  //身份信息，区别该帧
    uint32_t m_dwCnt;  //该帧最大重发次数
    uint32_t m_dwTime;  //上次发送时间
    uint32_t m_dwScheTime;  //调度时间
		uint16_t m_wBufLen;
		uint8_t m_bySendNum;
    uint8_t m_byBuf[FRAME_BUF_LEN_MAX];
}FRAME_BUF;


//板级硬件外设编号
typedef enum{
	IDX_HARD_GPS = 0,	// GPS
	IDX_HARD_LASER,	// 激光探头
	IDX_HARD_ZIGBEE,	// zigbee模块
	IDX_HARD_BLE,		// 蓝牙模块
	IDX_HARD_UPCOM,	// 4G或者电台等上行口
	IDX_HARD_CNJ,		// 舱内机
	IDX_HARD_SGD,		// 声光弹
	IDX_HARD_YWG,		// 烟雾罐
	IDX_HARD_FSJ,		// 主炮发射机
	IDX_HARD_YT,		//云台
	IDX_HARD_JDY,		//角度仪
	IDX_HARD_RTK ,		// RTK
	IDX_HARD_HMI,		// 串口屏
	IDX_HARD_OLED,		// OLED显示屏
	IDX_HARD_SPIFLASH,//存储芯片
  IDX_HARD_DCT_LED, //一体式四面探头+ 警示灯
  IDX_HARD_MPU6050,//MPU6050
  IDX_HARD_WT902,     //九轴
	IDX_HARD_IR38T,//红外短距发送
	IDX_HARD_IR38R,//红外短距接收
}ENUM_HARDWARE_IDX;


typedef enum
{
    CTRL_RES = 0,
    CTRL_RECV ,
    CTRL_SEND,
    CTRL_DO ,
    CTRL_DI,
    CTRL_PWM,
    CTRL_DAT,
}DEVCTRL;




#endif
