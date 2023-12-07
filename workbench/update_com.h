/******************************************************************
*版权所有 (C)2020, 江苏华如防务科技有限公司
*
*文件名称:  upcom.h
*文件标识:
*内容摘要:
*其它说明:
*当前版本:
*作    者:
*完成日期:  2021/02/03
*
*修改记录1:
*    修改日期:
*    版 本 号:
*    修 改 人:
*    修改内容:
******************************************************************/
#ifndef  _UPDATE_COM_H
#define  _UPDATE_COM_H
/******************************************************************
*                             头文件                              *
******************************************************************/
#include "config.h"

//#define UPDATECOM_PROT_BUFFER_LEN_MAX 512

//硬件操作
typedef struct
{
    //硬件初始化操作
    void (*hw_init)(void);

    //读操作
    uint32_t (*read)(uint8_t *, uint16_t *);
    //写操作
    uint32_t (*write)(uint8_t *, uint16_t );

    #if 0
    //上电
    void (*gpio_power_on)(void);
    //断电
    void (*gpio_power_off)(void);

    //硬件复位引脚拉低
    void (*gpio_reset)(void);
    //硬件复位引脚拉高
    void (*gpio_set)(void);
    #endif

} UPDATECOM_HW_OPERATIONS;

typedef struct
{

    UPDATECOM_HW_OPERATIONS m_sHwOps;

    uint32_t 	m_bIsInit: 1;
    uint32_t 	m_bRes: 31;

    uint32_t	m_dwNoCommTimeout;	//通讯超时监控


    //接收缓冲区
    //uint8_t	m_aProtBuf[UPDATECOM_PROT_BUFFER_LEN_MAX];
    uint8_t     *m_pProtBuf;   //根据实际情况申请
    uint16_t    m_wProtBufMaxLen; //最大不超过65535
    uint16_t	m_wProtBufLen;

} UPDATECOM_MAG;

/****************************************************************
@FUNCTION：升级口设置最大缓存协议长度
@INPUT：
	UPCOM_MAG	-	通讯口管理指针，不能为空
    Len   - 最大长度
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 需要指明缓冲区最大缓存长度，最大不超过8k

****************************************************************/
void UpdateCom_SetMaxPackLen(UPDATECOM_MAG *pMag, uint32_t Len);


/****************************************************************
@FUNCTION：升级口初始化
@INPUT：
	UPCOM_HW_OPERATIONS - 上行通讯口相关底层操作函数
	UPCOM_MAG	-	上行通讯口管理指针，不能为空
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:

****************************************************************/
void UpdateCom_Init(UPDATECOM_HW_OPERATIONS a_sHwOps, UPDATECOM_MAG *pMag);

/****************************************************************
@FUNCTION：升级口初始化销毁
@INPUT：
	UPCOM_MAG	-	上行通讯口管理指针，不能为空
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 善始善终

****************************************************************/
void UpdateCom_DeInit(UPDATECOM_MAG *pMag);

/****************************************************************
@FUNCTION：升级串口接收处理进程
@INPUT：
	UPDATECOM_MAG	-	升级串口管理指针
@OUTPUT: 无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 支持升级

****************************************************************/
void UpdateCom_RecvProcess(UPDATECOM_MAG *pMag);

/****************************************************************
@FUNCTION：升级串口接收处理 - 找到符合协议格式的报文
@INPUT：
	UPDATECOM_MAG	-	升级串口管理指针
@OUTPUT: 无
@RETURN: 0 -  未找到,  1 - 找到
@AUTHOR：xfw
@SPECIAL:

****************************************************************/
uint32_t UpdateCom_Find55AAFrame(UPDATECOM_MAG *pMag);

#endif


