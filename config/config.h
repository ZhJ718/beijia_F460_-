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

//����ָ��
typedef void (*PTRFUNC)(void); 


//#define __STATIC_INLINE static __INLINE

#define TRUE 1
#define FALSE 0

#define RET_TRUE  0
#define RET_FALSE -1


#define MAINVERSION 0x06
//�ϳ�Ӫ��Ŀ
#define SUBVERSION 0x01
//5308��Ŀ
//#define SUBVERSION 0x02
//��½��Ŀ
//#define SUBVERSION 0x03
//�½�11S
//#define SUBVERSION 0x04

#define FIXVERSION 0x01

//#define BOARDID 0x01		//���ӱ��01,ͷ��
#define BOARDID 0x02		//���ӱ��02������

//#define SEND_TO_UPCOM   //�ϴ������п�

#ifdef _APP
//��־�������
#define OPEN_MPU6050				//����̬ģ�鹦��
//#define DEBUG    //ʹ�ܴ�ӡ��� 
#undef DEBUG

#endif

//#define TEST_SHELL
#ifdef DEBUG

//#define ZIG_COM_LOG_OUTPUT  //ʹ��zigbeeģ����־���
//#define LASER_COM_LOG_OUTPUT  //ʹ��zigbeeģ����־���
//#define GPS_COM_LOG_OUTPUT  //ʹ��GPSģ����־���
//#define IR38_COM_LOG_OUTPUT  //ʹ�ܺ���̾�ģ����־���
//#define HMI_COM_LOG_OUTPUT //ʹ��HMIģ����־���

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
#define REMAINDER_DEFAULT	50	//�����ڿɿ�������


#define		HIBYTE(x)				(uint8_t)(((x)>>8)&0xFF)
#define		LOBYTE(x)				(uint8_t)((x)&0xFF)
#define		WORD_TO_BCD(wdata)		(((wdata)%10)|((((wdata)/10)%10)<<4)|((((wdata)/100)%10)<<8)|(((wdata)/1000)<<12))		/*����ת�����9999��Χ��BCD��ת������WORD�洢*/

#define		GET_TAB_LEN(pTab)			(sizeof(pTab)/sizeof(pTab[0]))	

#ifndef HC32F46x
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))
    
//�ն�״̬
#define TRAIN 			0		// ѵ��
#define GAME_PREP 		1		// ��ϰ Ԥ��
#define GAME_GOING 		2		//��ϰ ��ʼ
#define GAME_OVER 		3		//��ϰ ����

//�ն�״̬1 -���� 2-�������� 3-��̨���� 4-��̨���� 5-�������� 6-���и��� 7-�������� 8-ȱ������  
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


//����ģʽ
typedef enum{
	OPT_MODE_BLOCK=0,		//����ģʽ
	OPT_MODE_NONBLOCK	//������ģʽ
}OPT_MODE;



//Ӳ������
typedef struct
{
    //Ӳ����ʼ������
    void (*hw_init)(void);
    
    //������
    uint32_t (*read)(uint8_t *, uint16_t *);
    //д����
    uint32_t (*write)(uint8_t *, uint16_t );
    
    //�ϵ�
    void (*gpio_power_on)(void);
    //�ϵ�
    void (*gpio_power_off)(void);

    //Ӳ����λ��������
    void (*gpio_reset)(void);
    //Ӳ����λ��������
    void (*gpio_set)(void);

} COM_HW_OPERATIONS;

#define FRAME_BUF_LEN_MAX 230

typedef struct 
{
    uint32_t m_dwID;  //�����Ϣ�������֡
    uint32_t m_dwCnt;  //��֡����ط�����
    uint32_t m_dwTime;  //�ϴη���ʱ��
    uint32_t m_dwScheTime;  //����ʱ��
		uint16_t m_wBufLen;
		uint8_t m_bySendNum;
    uint8_t m_byBuf[FRAME_BUF_LEN_MAX];
}FRAME_BUF;


//�弶Ӳ��������
typedef enum{
	IDX_HARD_GPS = 0,	// GPS
	IDX_HARD_LASER,	// ����̽ͷ
	IDX_HARD_ZIGBEE,	// zigbeeģ��
	IDX_HARD_BLE,		// ����ģ��
	IDX_HARD_UPCOM,	// 4G���ߵ�̨�����п�
	IDX_HARD_CNJ,		// ���ڻ�
	IDX_HARD_SGD,		// ���ⵯ
	IDX_HARD_YWG,		// �����
	IDX_HARD_FSJ,		// ���ڷ����
	IDX_HARD_YT,		//��̨
	IDX_HARD_JDY,		//�Ƕ���
	IDX_HARD_RTK ,		// RTK
	IDX_HARD_HMI,		// ������
	IDX_HARD_OLED,		// OLED��ʾ��
	IDX_HARD_SPIFLASH,//�洢оƬ
  IDX_HARD_DCT_LED, //һ��ʽ����̽ͷ+ ��ʾ��
  IDX_HARD_MPU6050,//MPU6050
  IDX_HARD_WT902,     //����
	IDX_HARD_IR38T,//����̾෢��
	IDX_HARD_IR38R,//����̾����
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
