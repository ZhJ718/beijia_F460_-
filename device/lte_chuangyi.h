/******************************************************************
*版权所有 (C)2020, 江苏华如防务科技有限公司
*
*文件名称:  lte_chuangyi.h
*文件标识:
*内容摘要:
*其它说明:
*当前版本:
*作    者:
*完成日期:  2020/2/24
*
*修改记录1:
*    修改日期:
*    版 本 号:
*    修 改 人:
*    修改内容:
******************************************************************/
#ifndef  _LTE_CHUANGYI_H
#define  _LTE_CHUANGYI_H
/******************************************************************
*                             头文件                              *
******************************************************************/
#include "config.h"

/******************************************************************
*                           结构体定义                          *
******************************************************************/

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

} LTE_CHUANGYI_HW_OPERATIONS;

typedef struct
{

    uint8_t		m_byIsPowerEn: 1;		//TRUE-电源使能
    uint8_t 	m_byRes: 7;


    //实现相关硬件操作，可读写
    LTE_CHUANGYI_HW_OPERATIONS m_sHwOps;

} LTE_CHUANGYI_LOWLEVEL_MAG;


/****************************************************************
@FUNCTION：创毅4G模块底层初始化
@INPUT：硬件接口函数结构体，4G底层管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 绑定底层硬件操作函数，初始化，拉低电源使能引脚
****************************************************************/
extern void Lte_ChuangYi_LowLevelInit(LTE_CHUANGYI_HW_OPERATIONS a_sHwOps, LTE_CHUANGYI_LOWLEVEL_MAG *pMag);


/****************************************************************
@FUNCTION：创毅4G模块接口上电
@INPUT：Lte_ChuangYi底层管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Lte_ChuangYi_PowerOn(LTE_CHUANGYI_LOWLEVEL_MAG *pMag);

/****************************************************************
@FUNCTION：创毅4G模块接口下电
@INPUT：Lte_ChuangYi底层管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Lte_ChuangYi_PowerOff(LTE_CHUANGYI_LOWLEVEL_MAG *pMag);


#endif


