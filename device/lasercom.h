/******************************************************************
*版权所有 (C)2023, 江苏华如防务科技有限公司
*
*文件名称:  lasercom.h
*文件标识:
*内容摘要:
*其它说明:
*当前版本:
*作    者:
*完成日期:  2023/4/4
*
*修改记录1:
*    修改日期:
*    版 本 号:
*    修 改 人:
*    修改内容:
******************************************************************/
#ifndef  _LASERCOM_H
#define  _LASERCOM_H
/******************************************************************
*                             头文件                              *
******************************************************************/
#include "config.h"

/******************************************************************
*                           结构体定义                          *
******************************************************************/

//硬件操作
typedef enum
{
    LASER_MODE_NORMAL = 0,
    LASER_MODE_TEST = 1,	//测试模式下，探头收到打击信息后灯常亮

} LASER_MODE;

//硬件操作
typedef struct
{
    //硬件初始化操作，包含IO口初始化、串口等
    void (*hw_init)(void);
    //硬件复位引脚拉低
    void (*gpio_power_on)(void);
    //硬件复位引脚拉高
    void (*gpio_power_off)(void);
    //读操作
    uint32_t (*read)(uint8_t *, uint16_t *);
    //写操作
    uint32_t (*write)(uint8_t *, uint16_t );

} LASER_HW_OPERATIONS;

typedef struct
{

    uint8_t		m_byIsPowerEn: 1;		//TRUE-电源使能
    uint8_t		m_byIsCommunicationSucc: 1;
    uint8_t 	m_byRes: 6;

    //uint32_t	m_dwNoCommunicationTimeout;	//无通信超时

    //uint8_t 	m_byPowerOnNowCnt;	//当前已完成上电时长

    //实现相关硬件操作，可读写
    COM_HW_OPERATIONS m_sHwOps;

} LASERCOM_LOWLEVEL_MAG;


LASERCOM_LOWLEVEL_MAG *LaserCom_GetLLMagHandle(void);

/****************************************************************
@FUNCTION：探头底层初始化
@INPUT：硬件接口函数结构体
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 绑定底层硬件操作函数，初始化，拉低电源使能引脚
****************************************************************/
extern void LaserCom_LowLevelInit(COM_HW_OPERATIONS a_sHwOps);


/****************************************************************
@FUNCTION：LASER接口上电
@INPUT：
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 等待一定时间后控制
****************************************************************/
void LaserCom_PowerOn(void);



/****************************************************************
@FUNCTION：LASER接口掉电
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void LaserCom_PowerOff(void);



/****************************************************************
@FUNCTION：LASER接口底层定时调度进程
@INPUT：
@OUTPUT：无
@RETURN: TRUE-正在执行底层定时调度,  FALSE-无底层定时调度, 可执行上层应用定时调度
@AUTHOR：xfw
@SPECIAL: 实现模块上电
****************************************************************/
extern uint32_t LaserCom_LowLevelTimedProcess(void);


#endif


