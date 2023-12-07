#include "IR_send.h"
#include "led.h"
//#include "device.h"

//#include "type_usr_defined.h"

//#include "N76E003.h"
//#include "SFR_Macro.h"
//#include "Function_define.h"
//#include "Common.h"
//#include "Delay.h"

//有无停止位
//先发高位或低位

//时隙数
#define DATA_0_HIGH_TIME_SLOT_NUM        11                                 //数据0高电平时隙数
#define DATA_0_LOW_TIME_SLOT_NUM         DATA_0_HIGH_TIME_SLOT_NUM          //数据0低电平时隙数
#define DATA_1_HIGH_TIME_SLOT_NUM        DATA_0_HIGH_TIME_SLOT_NUM          //数据1高电平时隙数
#define DATA_1_LOW_TIME_SLOT_NUM         (3 * DATA_0_HIGH_TIME_SLOT_NUM)    //数据1低电平时隙数
#define LEADING_HIGH_TIME_SLOT_NUM       (5 * DATA_0_HIGH_TIME_SLOT_NUM)    //前导高电平时隙数
#define LEADING_LOW_TIME_SLOT_NUM        (5 * DATA_0_HIGH_TIME_SLOT_NUM)    //前导低电平时隙数
#define ENDING_HIGH_TIME_SLOT_NUM        (3 * DATA_0_HIGH_TIME_SLOT_NUM)    //结尾高电平时隙数
#define ENDING_LOW_TIME_SLOT_NUM         (3 * DATA_0_HIGH_TIME_SLOT_NUM)    //结尾低电平时隙数

//红外发送状态
typedef enum
{
    IR_SEND_STATE_IDLE           = 0,   //红外状态空闲
    IR_SEND_STATE_LEADING_HIGH,         //红外状态发送前导高电平
    IR_SEND_STATE_LEADING_LOW,          //红外状态发送前导低电平
//    IR_SEND_STATE_DATA,                 //红外状态发送数据
    IR_SEND_STATE_DATA_0_HIGH,          //红外状态发送数据0高电平
    IR_SEND_STATE_DATA_0_LOW,           //红外状态发送数据0低电平
    IR_SEND_STATE_DATA_1_HIGH,          //红外状态发送数据1高电平
    IR_SEND_STATE_DATA_1_LOW,           //红外状态发送数据1低电平
    IR_SEND_STATE_ENDING_HIGH,          //红外状态发送结尾高电平
    IR_SEND_STATE_ENDING_LOW,           //红外状态发送结尾低电平
} IR_send_state_e;

typedef struct
{
    IR_send_state_e state;          //发送状态
//    uint8_t byte_cnt;               //发送到第几字节
    uint8_t bit_cnt;                //发送到第几位
    uint8_t time_slot_cnt;          //发送到第几个时隙
} IR_send_s;

IR_send_s g_IR_send = {IR_SEND_STATE_IDLE};

//发送的数据
typedef struct
{
    uint16_t buffer[8];
    uint8_t bit_num;
} send_data_t;
static send_data_t g_send_data;
typedef struct
{
    uint16_t data;
    uint8_t bit_num;
} IR_data_t;

//射击
static IR_data_t g_IR_data_Shot[] =
{
    {0,     2 },     //射击识别码
    {0x01,  6 },     //rid
    {0xffff,  10},     //pid
    {1,     6 },     //武器、弹药ID
    {0x0,     8 },     //授权码
    {1,     1 },     //红蓝方
    {0,     1 },     //奇校验
};

#define IR_DATA_SHOT_NUM     (sizeof(g_IR_data_Shot) / sizeof(g_IR_data_Shot[0]))

//调理
static IR_data_t g_IR_data_Adjust[] =
{
    {2,  2 },     //调理指令识别码
    {0x32,  6 },     //rid
    {0xffff,  10},     //pid
    {0,  3 },     //调理指令 0-判死 1-复活 2-连接超级蓝牙 3-程序升级
    {0x0,  8 },     //授权码
    {0,  1 },     //奇校验
};

#define IR_DATA_ADJUST_NUM     (sizeof(g_IR_data_Adjust) / sizeof(g_IR_data_Adjust[0]))

//弹量命令
static IR_data_t g_IR_data_Supplement[] =
{
    {3,  2 },     //弹量指令识别码
    {0x32,  6 },     //rid
    {0xffff,  10},     //pid
    {1,  8 },     //武器、弹药ID
    {45,  10},     //弹量
    {0,  2 },     //方式 0-补弹 1-初始化弹量 2-减弹量 3-归零
    {0x0,  8 },      //授权码
    {0,  1 },     //奇校验
};
#define IR_DATA_SUPPLEMENT_NUM     (sizeof(g_IR_data_Supplement) / sizeof(g_IR_data_Supplement[0]))


//近弹
static IR_data_t g_IR_data_ShotArround[] =
{
    {6,  4 },    //近弹
};
#define IR_DATA_SHOTARD_NUM     (sizeof(g_IR_data_ShotArround) / sizeof(g_IR_data_ShotArround[0]))


//#define IR_DATA_NUM     (sizeof(g_IR_data) / sizeof(g_IR_data[0]))


//uint8_t g_send_data[] = {0x5A, 0xA5, 0x10, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xAA};

typedef enum
{
    TIME_SLOT_LOW       = 0,
    TIME_SLOT_WAVE,
} time_slot_e;
static time_slot_e g_time_slot_type = TIME_SLOT_LOW;

static void PWM_time_slot_wave(void)
{
    g_time_slot_type = TIME_SLOT_WAVE;
}

static void PWM_time_slot_low(void)
{
    g_time_slot_type = TIME_SLOT_LOW;
}

//#define TIMER_DIV4_VALUE_8us    (65536 - 33)
//static void Timer2_init(void)
//{
//    TIMER2_DIV_4;
//    TIMER2_Auto_Reload_Delay_Mode;
//    RCMP2L = TIMER_DIV4_VALUE_8us;
//    RCMP2H = TIMER_DIV4_VALUE_8us >> 8;
//    TL2 = 0;
//    TH2 = 0;
////    set_PT2;                                  //定时器中断优先级
//    set_ET2;                                    // Enable Timer2 interrupt
//    set_EA;
//    set_TR2;                                    // Timer2 run
//}

//根据bit位，设置值，返回bit位为1的数量
static uint8_t set_value_by_bit(uint16_t *pBuffer, uint16_t data, uint8_t start_bit, uint8_t bit_num)
{
    uint8_t byte, bit;
    uint8_t i;
    uint8_t odd_bit_cnt = 0;

    for (i = 0; i < bit_num; ++i)
    {
        if (data & 1 << i)
        {
            byte = (i + start_bit) / 8;
            bit = (i + start_bit) % 8;
            pBuffer[byte] |= 1 << bit;
            odd_bit_cnt++;
        }
    }

    return odd_bit_cnt;
}
#if 0
void IR_send_data_init(void)
{
    int8_t i;
    uint8_t odd_bit_cnt = 0;
    send_data_t *pSend_data = &g_send_data;

    memset(pSend_data, 0, sizeof(send_data_t));

    for (i = IR_DATA_NUM - 1; i >= 0; --i)
    {
        IR_data_t *pIR_data = &g_IR_data[i];
        odd_bit_cnt += set_value_by_bit(pSend_data->buffer, pIR_data->data, pSend_data->bit_num, pIR_data->bit_num);
        pSend_data->bit_num += pIR_data->bit_num;
    }

    //奇校验
    if (6 != g_IR_data[0].data)  //近弹不需要校验
    {
        if (0 == odd_bit_cnt % 2)
        {
            pSend_data->buffer[0] |= 0x01;
        }
        else
        {
            pSend_data->buffer[0] &= 0xFE;
        }
    }
}
#endif

u8 g_byIRSwitch;
u8 g_byBase;

void IR_send_data_init(void)
{
    int8_t i, DataLen;
    uint8_t odd_bit_cnt = 0;
    send_data_t *pSend_data = &g_send_data;
    IR_data_t *pData = NULL;
    u8 byNowVal;

    memset(pSend_data, 0, sizeof(send_data_t));


    switch(g_byIRSwitch)
    {
            #if 1

        //射击
        case 1:
            pData = g_IR_data_Shot;
            DataLen = IR_DATA_SHOT_NUM;
            break;
            #endif

        //进入训练模式
        case 2:
            pData = g_IR_data_Adjust;
            g_IR_data_Adjust[3].data = 3;
            DataLen = IR_DATA_ADJUST_NUM;
            break;

        //判死
        case 3:
            pData = g_IR_data_Adjust;
            g_IR_data_Adjust[3].data = 0;
            DataLen = IR_DATA_ADJUST_NUM;
            break;

        //复活
        case 4:
            pData = g_IR_data_Adjust;
            g_IR_data_Adjust[3].data = 1;
            DataLen = IR_DATA_ADJUST_NUM;
            break;
            #if 0

        //连接超级蓝牙
        case K4_BIT_MASK:
            pData = g_IR_data_Adjust;
            g_IR_data_Adjust[3].data = 2;
            DataLen = IR_DATA_ADJUST_NUM;
            break;
            #endif

        //补弹
        case 5:
            pData = g_IR_data_Supplement;
            g_IR_data_Supplement[4].data = g_byBase;
            DataLen = IR_DATA_SUPPLEMENT_NUM;
            break;
            #if 0

        //初始化弹量
        case K6_BIT_MASK:
            pData = g_IR_data_Supplement;
            g_IR_data_Supplement[5].data = 1;
            DataLen = IR_DATA_SUPPLEMENT_NUM;
            break;

        //近弹
        case K7_BIT_MASK:
            pData = g_IR_data_ShotArround;
            DataLen = IR_DATA_SHOTARD_NUM;
            break;
            #endif

        //默认连接超级蓝牙
        default:
            pData = g_IR_data_Adjust;
            g_IR_data_Adjust[3].data = 2;
            DataLen = IR_DATA_ADJUST_NUM;
            break;
    }

    for (i = DataLen - 1; i >= 0; --i)
    {
        IR_data_t *pIR_data = &pData[i];
        odd_bit_cnt += set_value_by_bit(pSend_data->buffer, pIR_data->data, pSend_data->bit_num, pIR_data->bit_num);
        pSend_data->bit_num += pIR_data->bit_num;
    }

    //奇校验
    if (6 != pData[0].data)  //近弹不需要校验
    {
        if (0 == odd_bit_cnt % 2)
        {
            pSend_data->buffer[0] |= 0x01;
        }
        else
        {
            pSend_data->buffer[0] &= 0xFE;
        }
    }
}

//激光发射初始化-IO口产生脉冲，使能Systick中断
void IR_Init(void)
{
    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);	 //使能PA,PB，PC端口时钟

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;		 //推挽输出
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO口速度为50MHz

    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_5;		//端口配置
    GPIO_Init(GPIOA, &GPIO_InitStructure);				//根据设定参数初始化GPIOA

    //LASERA = 0;
    //Laser_B_On();
    Laser_A_Off();
    //Laser_B_Off();.

    PWM_time_slot_low();
    //闪光灯
    //GPIO_SetMode(FLASHLIGHT_GPIO, FLASHLIGHT_GPIO_PIN, GPIO_MODE_OUTPUT);
    //FLASHLIGHT = 0;
    //SysTick_Config(SystemCoreClock / TICK_PRESCALE);
}
static uint8_t get_next_bit(uint16_t *pData, uint8_t bit_num)
{
    uint8_t byte_num;
    byte_num = bit_num / 8;
    bit_num %= 8;

    if (pData[byte_num] & 1 << bit_num)
    {
        return 1;
    }

    return 0;
}

static void next_PWM(void)
{
    send_data_t *pSend_data = &g_send_data;
//    uint8_t *pData = pSend_data->buffer;
//    uint8_t data_num = pSend_data->bit_num;
    IR_send_s *pIR = &g_IR_send;

    ++pIR->time_slot_cnt;

    switch (pIR->state)
    {
        case IR_SEND_STATE_IDLE:
            PWM_time_slot_low();
            pIR->time_slot_cnt = 0;
            SysTick->CTRL = 0;//复位Systick
            break;

        case IR_SEND_STATE_LEADING_HIGH:
            PWM_time_slot_wave();

            if (pIR->time_slot_cnt >= LEADING_HIGH_TIME_SLOT_NUM)
            {
                pIR->state = IR_SEND_STATE_LEADING_LOW;
                pIR->time_slot_cnt = 0;
            }

            break;

        case IR_SEND_STATE_LEADING_LOW:
            PWM_time_slot_low();

            if (pIR->time_slot_cnt >= LEADING_LOW_TIME_SLOT_NUM)
            {
                pIR->time_slot_cnt = 0;
//            pIR->byte_cnt = 0;
                pIR->bit_cnt = 0;

                if (get_next_bit(pSend_data->buffer, pIR->bit_cnt))
                {
                    pIR->state = IR_SEND_STATE_DATA_1_HIGH;
                }
                else
                {
                    pIR->state = IR_SEND_STATE_DATA_0_HIGH;
                }
            }

            break;

        case IR_SEND_STATE_DATA_0_HIGH:
            PWM_time_slot_wave();

            if (pIR->time_slot_cnt >= DATA_0_HIGH_TIME_SLOT_NUM)
            {
                pIR->time_slot_cnt = 0;
                pIR->state = IR_SEND_STATE_DATA_0_LOW;
            }

            break;

        case IR_SEND_STATE_DATA_0_LOW:
            PWM_time_slot_low();

            if (pIR->time_slot_cnt >= DATA_0_LOW_TIME_SLOT_NUM)
            {
                pIR->time_slot_cnt = 0;
                ++pIR->bit_cnt;

                if (pIR->bit_cnt >= pSend_data->bit_num)
                {
                    pIR->state = IR_SEND_STATE_ENDING_HIGH;
                }
                else
                {
                    if (get_next_bit(pSend_data->buffer, pIR->bit_cnt))
                    {
                        pIR->state = IR_SEND_STATE_DATA_1_HIGH;
                    }
                    else
                    {
                        pIR->state = IR_SEND_STATE_DATA_0_HIGH;
                    }
                }
            }

            break;

        case IR_SEND_STATE_DATA_1_HIGH:
            PWM_time_slot_wave();

            if (pIR->time_slot_cnt >= DATA_1_HIGH_TIME_SLOT_NUM)
            {
                pIR->time_slot_cnt = 0;
                pIR->state = IR_SEND_STATE_DATA_1_LOW;
            }

            break;

        case IR_SEND_STATE_DATA_1_LOW:
            PWM_time_slot_low();

            if (pIR->time_slot_cnt >= DATA_1_LOW_TIME_SLOT_NUM)
            {
                pIR->time_slot_cnt = 0;
                ++pIR->bit_cnt;

                if (pIR->bit_cnt >= pSend_data->bit_num)
                {
                    pIR->state = IR_SEND_STATE_ENDING_HIGH;
                }
                else
                {
                    if (get_next_bit(pSend_data->buffer, pIR->bit_cnt))
                    {
                        pIR->state = IR_SEND_STATE_DATA_1_HIGH;
                    }
                    else
                    {
                        pIR->state = IR_SEND_STATE_DATA_0_HIGH;
                    }
                }
            }

            break;

        case IR_SEND_STATE_ENDING_HIGH:
            if (pIR->time_slot_cnt >= ENDING_HIGH_TIME_SLOT_NUM)
            {
                pIR->state = IR_SEND_STATE_ENDING_LOW;
                pIR->time_slot_cnt = 0;
            }
            else
            {
                PWM_time_slot_wave();
            }

            break;

        case IR_SEND_STATE_ENDING_LOW:
            PWM_time_slot_low();

            if (pIR->time_slot_cnt >= ENDING_LOW_TIME_SLOT_NUM)
            {
                pIR->state = IR_SEND_STATE_IDLE;
                pIR->time_slot_cnt = 0;
            }

            break;

        default:

            break;

    }
}


void IR_send(void)
{
    IR_send_s *pIR = &g_IR_send;

    if (IR_SEND_STATE_IDLE == pIR->state)
    {
        pIR->state = IR_SEND_STATE_LEADING_HIGH;
    }

    IR_send_data_init();
    SysTick_Config(SystemCoreClock / TICK_PRESCALE);
}
uint8_t IR_is_idle(void)
{
    if (IR_SEND_STATE_IDLE == g_IR_send.state)
    {
        return 1;
    }

    return 0;
}


#define set_IR() do{Laser_A_On();}while(0)	//LASERB=0;
#define clr_IR() do{Laser_A_Off();}while(0)	//LASERB=1;

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    static int i = 0, cnt = 0;

    ++i;

    switch (i)
    {
        case 1:
            if (TIME_SLOT_WAVE == g_time_slot_type)
            {
                set_IR();
            }
            else
            {
                clr_IR();
            }

            break;

        case 2:
            clr_IR();
            break;

        case 3:
            i = 0;
            next_PWM();
            break;

        default:
            break;
    }
}

static uint8_t g_IR_to_send_cnt = 0;       //红外待发送次数

//是否发送红外
uint8_t is_send_IR(void)
{
    if (g_IR_to_send_cnt > 0)
    {
        g_IR_to_send_cnt--;
        return 1;
    }

    return 0;
}

//定时检测激光是否需要发送数据
void IR_TimedProcess(void)
{
    if (IR_is_idle())           //红外发送空闲
    {
        //LowPowerLed_Off(); // 灯灭
        //SysTick->CTRL = 0;
        if (is_send_IR())       //需要发送红外
        {
            IR_send();          //发送红外
            // LowPowerLed_On();// 灯亮
        }
    }
}


//开启红外发送	SEND_CNT_ONE_CLICK 次
//id-识别码  0-连接超级蓝牙 1-射击 2-训练模式 3-死亡 4-复活  5-补弹
//RedOrBlue 1-红军 0-蓝军
void IR_StartSend(uint8_t id, uint8_t rid, uint16_t pid, uint8_t AmmoUniID, uint8_t Key, uint8_t RedOrBlue)
{
    g_IR_to_send_cnt += SEND_CNT_ONE_CLICK;

    //g_IR_data[0].data = id;
    //g_IR_data[1].data = rid;
    //g_IR_data[2].data = pid;
    //g_IR_data[3].data = AmmoUniID;
    //g_IR_data[4].data = Key;
    //g_IR_data[5].data = RedOrBlue;

    g_byIRSwitch = id;

    switch(g_byIRSwitch)
    {
        case 1:
            g_IR_data_Shot[1].data = rid;
            g_IR_data_Shot[2].data = pid;
            g_IR_data_Shot[3].data = AmmoUniID;
            g_IR_data_Shot[4].data = Key;
            g_IR_data_Shot[5].data = RedOrBlue;
            break;

        case 2:
        case 3:
        case 4:
            g_IR_data_Adjust[1].data = rid;
            g_IR_data_Adjust[2].data = pid;
            g_IR_data_Adjust[4].data = Key;
            break;

        case 5:
            g_IR_data_Supplement[1].data = rid;
            g_IR_data_Supplement[2].data = pid;
            g_IR_data_Supplement[6].data = Key;
            break;

        default:
            g_IR_data_Adjust[1].data = rid;
            g_IR_data_Adjust[2].data = pid;
            g_IR_data_Adjust[4].data = Key;
            break;
    }
}


