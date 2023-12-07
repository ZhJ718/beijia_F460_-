/******************************************************************
*版权所有 (C)2023, 江苏华如防务科技有限公司
*
*文件名称:  upgrade.h
*文件标识:
*内容摘要:
*其它说明:
*当前版本:
*作    者: xfw
*完成日期:  2023/2/27
*
*修改记录1:
*    修改日期:
*    版 本 号:
*    修 改 人:
*    修改内容:
******************************************************************/
#ifndef  _UPGRADE_H
#define  _UPGRADE_H
/******************************************************************
*                             头文件                              *
******************************************************************/
#include "config.h"

#define PROTOCAL_CMD_BOARD_CTRL	0x00	//板级外设控制
#define PROTOCAL_CMD_RESTART	0x02	//重启
#define PROTOCAL_CMD_CHANGE_UPCOM	0x03	//子设备上行通讯串口控制,切换控制口

#define PROTOCAL_CMD_GET_MAC    	0xF0	//获取设备MAC地址
#define PROTOCAL_CMD_GET_APPVER	0xF1	//获取应用程序版本号
#define PROTOCAL_CMD_GET_DEVID	0xF2	//获取设备身份信息（仅限APP）
#define PROTOCAL_CMD_SET_MAC    	0xF3	//设置MAC地址

#define PROTOCAL_CMD_RESET_DOWNLOAD_ADDR	0xF8	//重置数据缓存的位置(默认FLASH空间后半段)
#define PROTOCAL_CMD_RESET_REPLACE_ADDR	0xF9	//重置替换地址（默认应用起始地址）
#define PROTOCAL_CMD_DOWNLOAD_START	0xFA	//开始升级
#define PROTOCAL_CMD_DOWNLOAD_END	0xFB	//升级结束
#define PROTOCAL_CMD_CHECKSUM	0xFC	//FLASH程序镜像文件累加和
#define PROTOCAL_CMD_UPLOAD		0xFE	//应用程序上传 
#define PROTOCAL_CMD_DOWNLOAD	0xFF	//应用程序下载


typedef struct _UPDATE_MANAGER
{
    u32 IsUpload: 1;
    u32 IsDownloadStart: 1;
    u32 IsDownloadEnd: 1;
    u32 IsGetFileSize: 1;
    u32 IsGetFileCheckSum: 1;
    u32 IsRunApp: 1;
    u32 IsDownloadInExt: 1;	//下载数据到外部存储数据
    u32 IsRes: 25;

    //写操作
    uint32_t (*write)(uint8_t *, uint16_t );

    u32 ReplaceAddr;
    u32 DownloadAddr;
    u32 FlashAddr; 	//当前操作FLASH地址
    u32 FileSize; 		//文件大小
    u32 FileCheckSum; 	//文件校验和
    u32 FileDownloadSize; 		//文件已下载大小
    //u32 FlashWriteSize; 		//文件已写入大小
    u32 FileUploadSize;			//文件已上传大小
    u32 PageOffset;			//页缓冲中已经保存的数据大小

    #if defined(AT32F4xx)
    u8 PageBuffer[2048 + 1024]; //页缓存，比页尺寸大点，具体取值：假设单次数据长度l,页尺寸L，需满足，l*n<L且l*(n+1)>L
    #elif defined(HC32F46x)
    u8 PageBuffer[8 * 1024 + 1024];
    #endif


    u8 ProgramStep; //异步计步，标识当前进行到哪一步

    //u16 wLoastIndex[20];
    u8 byLastFrame[2048];
    u16 byLastFrameLen;
    u8 m_aDevMac[8];
    uint8_t m_byTType;
    uint8_t m_byTSubType;
    uint8_t m_byRID;
    uint16_t m_wPID;
} UPDATE_MANAGER, *PUPDATE_MANAGER;

extern UPDATE_MANAGER g_sUpdateMag;


/****************************************************************
@FUNCTION：升级进程初始化
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 初始化g_sUpdateMag
****************************************************************/
extern void Update_MagInit(void);

/****************************************************************
@FUNCTION：升级进程协议解析，甄别是否为升级包
@INPUT：pBuf pLen - 报文
@OUTPUT：pIsCutted - 报文是否完整
@RETURN: TRUE - 成功FALSE - 失败
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint32_t Update_ProtocalParse(const uint8_t *pBuf, uint16_t *pLen, uint32_t *pIsCutted, uint32_t *pIsLong55AA);


/****************************************************************
@FUNCTION：升级进程协议处理
@INPUT：pBSMag-后台结构体指针
@OUTPUT：pCmdBuf  CmdLen - 返回报文
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Update_ProtocalProcess(uint8_t *pCmdBuf, uint16_t CmdLen);


/****************************************************************
@FUNCTION：升级进程同步处理
@INPUT：pBSMag-后台结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
extern void DownloadProcess_Sync(UPDATE_MANAGER *pMag);


extern void StartDownloadProcess(UPDATE_MANAGER *pMag);


extern void StopDownloadProcess(UPDATE_MANAGER *pMag);


/****************************************************************
@FUNCTION：升级进程设置串口发送函数
@INPUT：无
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
extern void Update_ConfigSendFunc(uint32_t (*write)(uint8_t *, uint16_t ));


/****************************************************************
@FUNCTION：升级进程是否正在进行中
@INPUT：无
@OUTPUT：无
@RETURN:  TRUE - 正在进行中 FALSE -不在
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
extern uint32_t IsUpdateStart(void);

/****************************************************************
@FUNCTION：升级进程设置设备通讯MAC地址
@INPUT：无
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Update_SetDevMac(uint8_t *pDevMac, uint8_t DevMacLen);


/****************************************************************
@FUNCTION：升级进程设置设备实体类型
@INPUT：无
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Update_SetDevTType(uint8_t DevTType);

/****************************************************************
@FUNCTION：升级进程设置设备实体子类型
@INPUT：无
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Update_SetDevTSubType(uint8_t DevTSubType);

/****************************************************************
@FUNCTION：升级进程设置设备单位代字
@INPUT：无
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Update_SetDevRID(uint8_t DevRID);


/****************************************************************
@FUNCTION：升级进程设置设备编号
@INPUT：无
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void Update_SetDevPID(uint8_t DevPID);



extern void EraseNewData(void);

extern uint32_t CheckNewData(void);

void JumpToApp(void);


#endif


