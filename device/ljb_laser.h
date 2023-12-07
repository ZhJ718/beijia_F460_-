/******************************************************************
*版权所有 (C)2020, 江苏华如防务科技有限公司
*
*文件名称:  ljb_laser.h
*文件标识:
*内容摘要:
*其它说明:
*当前版本:
*作    者:
*完成日期:  2020/3/28
*
*修改记录1:
*    修改日期:
*    版 本 号:
*    修 改 人:
*    修改内容:
******************************************************************/
#ifndef  _LJB_LASER_H
#define  _LJB_LASER_H
/******************************************************************
*                             头文件                              *
******************************************************************/

#include "config.h"



/******************************************************************
*                           宏定义                                *
******************************************************************/

/*

PCA 输入捕获 时钟64MHZ 32分频 -> 2MHz
48K脉冲
				对应实际捕获值 允许捕获值
	1T=20.8us	41.6			30 - 60
	2T			83.2			70 - 100
	3T			124.8			110 - 140
	4T			166.4			150 - 180
	5T			208				190 - 220



#define TSEQ_1T_MIN	30		//
#define TSEQ_1T_MAX	60//80
#define TSEQ_2T_MIN	70
#define TSEQ_2T_MAX	100//145
#define TSEQ_3T_MIN	110
#define TSEQ_3T_MAX	140//210
#define TSEQ_4T_MIN	150
#define TSEQ_4T_MAX	180//270
#define TSEQ_5T_MIN	190
#define TSEQ_5T_MAX	220//330
*/

#define TSEQ_1T_MIN	35	//
#define TSEQ_1T_MAX	75	//
#define TSEQ_2T_MIN	89
#define TSEQ_2T_MAX	129//
#define TSEQ_3T_MIN	144
#define TSEQ_3T_MAX	184//
#define TSEQ_4T_MIN	199
#define TSEQ_4T_MAX	239//
#define TSEQ_5T_MIN	253
#define TSEQ_5T_MAX	293//

#define LJB_LASER_SEND_LEN_MAX 25
#define LJB_LASER_RECV_LEN_MAX LJB_LASER_SEND_LEN_MAX


#define TIMING_SEQUENCE_MAX	(3*6*(2*LJB_LASER_RECV_LEN_MAX+2)+5) //缓存12个时序，正常6个时序，多点给程序更多时间去处理
//#define TIMING_SEQUENCE_DEAL_MAX	70 //缓存12个时序，正常6个时序，多点给程序更多时间去处理
#define RECV_ENCODE_MAX (3*(2*LJB_LASER_RECV_LEN_MAX+2))

typedef struct
{
    uint16_t m_wPushOff;
    uint16_t m_wPopOff;
    uint8_t m_aSeq[TIMING_SEQUENCE_MAX];

} TIMING_SEQ_BUF_MAG;


//uint8_t g_IsRecvNew;
//uint8_t g_IsRecvTimeout;

typedef enum
{
    ENCODE_HIGH_0000,
    ENCODE_HIGH_0001,
    ENCODE_HIGH_0010,
    ENCODE_HIGH_0011,
    ENCODE_HIGH_0100,
    ENCODE_HIGH_0101,
    ENCODE_HIGH_0110,
    ENCODE_HIGH_0111,
    ENCODE_HIGH_1000,
    ENCODE_HIGH_1001,
    ENCODE_HIGH_1010,
    ENCODE_HIGH_1011,
    ENCODE_HIGH_1100,
    ENCODE_HIGH_1101,
    ENCODE_HIGH_1110,
    ENCODE_HIGH_1111,
    ENCODE_LOW_0000,
    ENCODE_LOW_0001,
    ENCODE_LOW_0010,
    ENCODE_LOW_0011,
    ENCODE_LOW_0100,
    ENCODE_LOW_0101,
    ENCODE_LOW_0110,
    ENCODE_LOW_0111,
    ENCODE_LOW_1000,
    ENCODE_LOW_1001,
    ENCODE_LOW_1010,
    ENCODE_LOW_1011,
    ENCODE_LOW_1100,
    ENCODE_LOW_1101,
    ENCODE_LOW_1110,
    ENCODE_LOW_1111,
    ENCODE_HEAD,
    ENCODE_TAIL,
    ENCODE_MAX,
} LJB_LASER_ENCODE;

typedef struct
{
    uint8_t m_aTSeq[6];
} LJB_LASER_ENCODE_MAG;


//为了加快查找速度，将上述数据有序排放
static const uint8_t gs_LaserEncodeIdx[ENCODE_MAX] =
{
    ENCODE_LOW_1011,
    ENCODE_HIGH_0110,
    ENCODE_LOW_0100,
    ENCODE_HIGH_1100,
    ENCODE_HIGH_1000,
    ENCODE_LOW_0011,
    ENCODE_HEAD,
    ENCODE_HIGH_0111,
    ENCODE_HIGH_1011,
    ENCODE_LOW_0010,
    ENCODE_HIGH_1110,
    ENCODE_HIGH_0100,
    ENCODE_TAIL,
    ENCODE_LOW_1110,
    ENCODE_LOW_1111,
    ENCODE_LOW_0000,
    ENCODE_HIGH_0000,
    ENCODE_LOW_1000,
    ENCODE_LOW_0110,
    ENCODE_LOW_1101,
    ENCODE_HIGH_1001,
    ENCODE_HIGH_0101,
    ENCODE_HIGH_1101,
    ENCODE_HIGH_1111,
    ENCODE_HIGH_0011,
    ENCODE_LOW_1001,
    ENCODE_HIGH_1010,
    ENCODE_LOW_1010,
    ENCODE_LOW_0001,
    ENCODE_HIGH_0001,
    ENCODE_LOW_0101,
    ENCODE_HIGH_0010,
    ENCODE_LOW_1100,
    ENCODE_LOW_0111,
};

static const LJB_LASER_ENCODE_MAG gs_LaserEncode[ENCODE_MAX] =
{
    //ENCODE_LOW_1011	2 1 1 5 1 1
    {2, 1, 1, 5, 1, 1},
    //ENCODE_HIGH_0110	2 1 2 1 4 1
    {2, 1, 2, 1, 4, 1},
    //ENCODE_LOW_0100	2 1 2 2 3 1
    {2, 1, 2, 2, 3, 1},
    //ENCODE_HIGH_1100 	2 1 3 2 2 1
    {2, 1, 3, 2, 2, 1},
    //ENCODE_HIGH_1000	2 1 3 3 1 1
    {2, 1, 3, 3, 1, 1},
    //ENCODE_LOW_0011	2 1 4 2 1 1
    {2, 1, 4, 2, 1, 1},
    //ENCODE_HEAD	2 1 5 1 1 1
    {2, 1, 5, 1, 1, 1},
    //ENCODE_HIGH_0111	2 2 1 2 3 1
    {2, 2, 1, 2, 3, 1},
    //ENCODE_HIGH_1011	2 2 1 3 2 1
    {2, 2, 1, 3, 2, 1},
    //ENCODE_LOW_0010	2 2 1 4 1 1
    {2, 2, 1, 4, 1, 1},
    //ENCODE_HIGH_1110	2 2 2 1 3 1
    {2, 2, 2, 1, 3, 1},
    //ENCODE_HIGH_0100	2 2 2 3 1 1
    {2, 2, 2, 3, 1, 1},
    //ENCODE_TAIL	2 2 3 2 1 1
    {2, 2, 3, 2, 1, 1},
    //ENCODE_LOW_1110	2 2 4 1 1 1
    {2, 2, 4, 1, 1, 1},
    //ENCODE_LOW_1111	2 3 1 3 1 1
    {2, 3, 1, 3, 1, 1},
    //ENCODE_LOW_0000	2 3 2 2 1 1
    {2, 3, 2, 2, 1, 1},
    //ENCODE_HIGH_0000 	2 3 3 1 1 1
    {2, 3, 3, 1, 1, 1},
    //ENCODE_LOW_1000	2 4 1 2 1 1
    {2, 4, 1, 2, 1, 1},
    //ENCODE_LOW_0110	2 4 2 1 1 1
    {2, 4, 2, 1, 1, 1},
    //ENCODE_LOW_1101	2 5 1 1 1 1
    {2, 5, 1, 1, 1, 1},
    //ENCODE_HIGH_1001 	3 1 2 3 1 1
    {3, 1, 2, 3, 1, 1},
    //ENCODE_HIGH_0101	3 1 3 2 1 1
    {3, 1, 3, 2, 1, 1},
    //ENCODE_HIGH_1101	3 2 1 3 1 1
    {3, 2, 1, 3, 1, 1},
    //ENCODE_HIGH_1111	3 2 2 2 1 1
    {3, 2, 2, 2, 1, 1},
    //ENCODE_HIGH_0011	3 2 3 1 1 1
    {3, 2, 3, 1, 1, 1},
    //ENCODE_LOW_1001	3 3 1 2 1 1
    {3, 3, 1, 2, 1, 1},
    //ENCODE_HIGH_1010	3 3 2 1 1 1
    {3, 3, 2, 1, 1, 1},
    //ENCODE_LOW_1010	3 4 1 1 1 1
    {3, 4, 1, 1, 1, 1},
    //ENCODE_LOW_0001	4 1 1 1 3 1
    {4, 1, 1, 1, 3, 1},
    //ENCODE_HIGH_0001	4 1 2 2 1 1
    {4, 1, 2, 2, 1, 1},
    //ENCODE_LOW_0101	4 1 3 1 1 1
    {4, 1, 3, 1, 1, 1},
    //ENCODE_HIGH_0010	4 2 2 1 1 1
    {4, 2, 2, 1, 1, 1},
    //ENCODE_LOW_1100	4 3 1 1 1 1
    {4, 3, 1, 1, 1, 1},
    //ENCODE_LOW_0111	5 2 1 1 1 1
    {5, 2, 1, 1, 1, 1},
};



typedef struct
{
    uint8_t m_IsRecvStart: 1;
    uint8_t m_IsRecv: 1;
    uint8_t m_IsRecvNew: 1;
    uint8_t m_IsRecvStep: 1;	//单步输入捕获中断标记
    uint8_t m_IsABCABCABC: 1; //接收模式
    uint8_t m_IsOK: 1;		//接收到完整报文
    uint8_t m_IsRes: 2;

    uint8_t m_byRecvTimeoutMs;
    uint16_t m_wNowCnt;	//当前寄存器捕获值
    uint16_t m_wLastCnt;	//上次寄存器捕获值

    TIMING_SEQ_BUF_MAG m_sTSeqMag;

    uint16_t m_wRecvEncodeCnt;	//接收到的编码数（1字节->2编码）
    uint8_t m_byRecvEncode[RECV_ENCODE_MAX];
    uint8_t m_byRepeatCnt;	//重复次数

    uint8_t m_byRecvBuf[LJB_LASER_RECV_LEN_MAX];//解码后的实际数据
    uint16_t m_wRecvBufLen;
} LJB_LASER_RECV_MAG;


//环形缓冲中偏移位置后移
#define RING_OFFSET_ADD(x,off,lsize)	(x=(x+off)%lsize)

#define RING_OFF(push,pop,lsize) ((push+lsize-pop)%lsize)

//void LaserRecvInit(void);


void LaserRecvTimedProcess(LJB_LASER_RECV_MAG *pLjbMag);

void LaserRecvStepProcess(LJB_LASER_RECV_MAG *pLjbMag);

void LaserRecvProcess(LJB_LASER_RECV_MAG *pLjbMag);


#if 0
typedef enum
{
    IR_L = 0,	//左侧
    IR_D,	//下侧
    IR_M,	//中心
    IR_U,	//上侧
    IR_R,	//右侧
    IR_MAX,
} IR_DIRECTION;
#endif

typedef enum
{
    IR_F = 0,	//前
    IR_L,	//左
    IR_R,	//右
    IR_B,	//后
    IR_MAX,
} IR_DIRECTION;


extern LJB_LASER_RECV_MAG g_sLaserRecvMag[IR_MAX];


typedef struct
{
    en_port_t m_ePort;
    en_pin_t	m_ePin;
    en_port_func_t m_ePortFunc;
    M4_TMRA_TypeDef *m_pTimTypeDef;

    en_timera_channel_t	m_eTimCh;
    en_timera_flag_type_t m_eTimFlg;
    en_timera_irq_type_t m_eTimIrq;

    en_int_src_t m_enIntSrc;
    IRQn_Type m_enIRQn;
    int8_t m_byIrqPrity;
} IR_CFG;


/*************
IRA1		PB5		TMRA3_CH2
IRA2		PC14	TMRA4_CH5	XTAL32_OUT
IRA3		PA11	TMRA1_CH4
IRA4		PB10	TMRA2_CH3
*************/
static const IR_CFG gs_IRCfg[IR_MAX] =
{
    {PortB, Pin05, Func_Tima0, M4_TMRA3, TimeraCh2, TimeraFlagCaptureOrCompareCh2, TimeraIrqCaptureOrCompareCh2, INT_TMRA3_CMP, Int001_IRQn, 1},
    {PortC, Pin14, Func_Tima0, M4_TMRA4, TimeraCh5, TimeraFlagCaptureOrCompareCh5, TimeraIrqCaptureOrCompareCh5, INT_TMRA4_CMP, Int002_IRQn, 2},
    {PortA, Pin11, Func_Tima0, M4_TMRA1, TimeraCh4, TimeraFlagCaptureOrCompareCh4, TimeraIrqCaptureOrCompareCh4, INT_TMRA1_CMP, Int003_IRQn, 3},
    {PortB, Pin10, Func_Tima0, M4_TMRA2, TimeraCh3, TimeraFlagCaptureOrCompareCh3, TimeraIrqCaptureOrCompareCh3, INT_TMRA2_CMP, Int004_IRQn, 4},
};

void IR_Cap_Init(void);


void LaserRecvInit_FourDir(void);


#endif

