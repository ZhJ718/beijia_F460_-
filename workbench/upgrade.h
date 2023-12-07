/******************************************************************
*��Ȩ���� (C)2023, ���ջ������Ƽ����޹�˾
*
*�ļ�����:  upgrade.h
*�ļ���ʶ:
*����ժҪ:
*����˵��:
*��ǰ�汾:
*��    ��: xfw
*�������:  2023/2/27
*
*�޸ļ�¼1:
*    �޸�����:
*    �� �� ��:
*    �� �� ��:
*    �޸�����:
******************************************************************/
#ifndef  _UPGRADE_H
#define  _UPGRADE_H
/******************************************************************
*                             ͷ�ļ�                              *
******************************************************************/
#include "config.h"

#define PROTOCAL_CMD_BOARD_CTRL	0x00	//�弶�������
#define PROTOCAL_CMD_RESTART	0x02	//����
#define PROTOCAL_CMD_CHANGE_UPCOM	0x03	//���豸����ͨѶ���ڿ���,�л����ƿ�

#define PROTOCAL_CMD_GET_MAC    	0xF0	//��ȡ�豸MAC��ַ
#define PROTOCAL_CMD_GET_APPVER	0xF1	//��ȡӦ�ó���汾��
#define PROTOCAL_CMD_GET_DEVID	0xF2	//��ȡ�豸�����Ϣ������APP��
#define PROTOCAL_CMD_SET_MAC    	0xF3	//����MAC��ַ

#define PROTOCAL_CMD_RESET_DOWNLOAD_ADDR	0xF8	//�������ݻ����λ��(Ĭ��FLASH�ռ����)
#define PROTOCAL_CMD_RESET_REPLACE_ADDR	0xF9	//�����滻��ַ��Ĭ��Ӧ����ʼ��ַ��
#define PROTOCAL_CMD_DOWNLOAD_START	0xFA	//��ʼ����
#define PROTOCAL_CMD_DOWNLOAD_END	0xFB	//��������
#define PROTOCAL_CMD_CHECKSUM	0xFC	//FLASH�������ļ��ۼӺ�
#define PROTOCAL_CMD_UPLOAD		0xFE	//Ӧ�ó����ϴ� 
#define PROTOCAL_CMD_DOWNLOAD	0xFF	//Ӧ�ó�������


typedef struct _UPDATE_MANAGER
{
    u32 IsUpload: 1;
    u32 IsDownloadStart: 1;
    u32 IsDownloadEnd: 1;
    u32 IsGetFileSize: 1;
    u32 IsGetFileCheckSum: 1;
    u32 IsRunApp: 1;
    u32 IsDownloadInExt: 1;	//�������ݵ��ⲿ�洢����
    u32 IsRes: 25;

    //д����
    uint32_t (*write)(uint8_t *, uint16_t );

    u32 ReplaceAddr;
    u32 DownloadAddr;
    u32 FlashAddr; 	//��ǰ����FLASH��ַ
    u32 FileSize; 		//�ļ���С
    u32 FileCheckSum; 	//�ļ�У���
    u32 FileDownloadSize; 		//�ļ������ش�С
    //u32 FlashWriteSize; 		//�ļ���д���С
    u32 FileUploadSize;			//�ļ����ϴ���С
    u32 PageOffset;			//ҳ�������Ѿ���������ݴ�С

    #if defined(AT32F4xx)
    u8 PageBuffer[2048 + 1024]; //ҳ���棬��ҳ�ߴ��㣬����ȡֵ�����赥�����ݳ���l,ҳ�ߴ�L�������㣬l*n<L��l*(n+1)>L
    #elif defined(HC32F46x)
    u8 PageBuffer[8 * 1024 + 1024];
    #endif


    u8 ProgramStep; //�첽�Ʋ�����ʶ��ǰ���е���һ��

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
@FUNCTION���������̳�ʼ��
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��ʼ��g_sUpdateMag
****************************************************************/
extern void Update_MagInit(void);

/****************************************************************
@FUNCTION����������Э�����������Ƿ�Ϊ������
@INPUT��pBuf pLen - ����
@OUTPUT��pIsCutted - �����Ƿ�����
@RETURN: TRUE - �ɹ�FALSE - ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint32_t Update_ProtocalParse(const uint8_t *pBuf, uint16_t *pLen, uint32_t *pIsCutted, uint32_t *pIsLong55AA);


/****************************************************************
@FUNCTION����������Э�鴦��
@INPUT��pBSMag-��̨�ṹ��ָ��
@OUTPUT��pCmdBuf  CmdLen - ���ر���
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Update_ProtocalProcess(uint8_t *pCmdBuf, uint16_t CmdLen);


/****************************************************************
@FUNCTION����������ͬ������
@INPUT��pBSMag-��̨�ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern void DownloadProcess_Sync(UPDATE_MANAGER *pMag);


extern void StartDownloadProcess(UPDATE_MANAGER *pMag);


extern void StopDownloadProcess(UPDATE_MANAGER *pMag);


/****************************************************************
@FUNCTION�������������ô��ڷ��ͺ���
@INPUT����
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern void Update_ConfigSendFunc(uint32_t (*write)(uint8_t *, uint16_t ));


/****************************************************************
@FUNCTION�����������Ƿ����ڽ�����
@INPUT����
@OUTPUT����
@RETURN:  TRUE - ���ڽ����� FALSE -����
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern uint32_t IsUpdateStart(void);

/****************************************************************
@FUNCTION���������������豸ͨѶMAC��ַ
@INPUT����
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Update_SetDevMac(uint8_t *pDevMac, uint8_t DevMacLen);


/****************************************************************
@FUNCTION���������������豸ʵ������
@INPUT����
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Update_SetDevTType(uint8_t DevTType);

/****************************************************************
@FUNCTION���������������豸ʵ��������
@INPUT����
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Update_SetDevTSubType(uint8_t DevTSubType);

/****************************************************************
@FUNCTION���������������豸��λ����
@INPUT����
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Update_SetDevRID(uint8_t DevRID);


/****************************************************************
@FUNCTION���������������豸���
@INPUT����
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void Update_SetDevPID(uint8_t DevPID);



extern void EraseNewData(void);

extern uint32_t CheckNewData(void);

void JumpToApp(void);


#endif


