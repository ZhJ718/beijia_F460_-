#include "IR_send.h"
#include "led.h"
//#include "device.h"

//#include "type_usr_defined.h"

//#include "N76E003.h"
//#include "SFR_Macro.h"
//#include "Function_define.h"
//#include "Common.h"
//#include "Delay.h"

//����ֹͣλ
//�ȷ���λ���λ

//ʱ϶��
#define DATA_0_HIGH_TIME_SLOT_NUM        11                                 //����0�ߵ�ƽʱ϶��
#define DATA_0_LOW_TIME_SLOT_NUM         DATA_0_HIGH_TIME_SLOT_NUM          //����0�͵�ƽʱ϶��
#define DATA_1_HIGH_TIME_SLOT_NUM        DATA_0_HIGH_TIME_SLOT_NUM          //����1�ߵ�ƽʱ϶��
#define DATA_1_LOW_TIME_SLOT_NUM         (3 * DATA_0_HIGH_TIME_SLOT_NUM)    //����1�͵�ƽʱ϶��
#define LEADING_HIGH_TIME_SLOT_NUM       (5 * DATA_0_HIGH_TIME_SLOT_NUM)    //ǰ���ߵ�ƽʱ϶��
#define LEADING_LOW_TIME_SLOT_NUM        (5 * DATA_0_HIGH_TIME_SLOT_NUM)    //ǰ���͵�ƽʱ϶��
#define ENDING_HIGH_TIME_SLOT_NUM        (3 * DATA_0_HIGH_TIME_SLOT_NUM)    //��β�ߵ�ƽʱ϶��
#define ENDING_LOW_TIME_SLOT_NUM         (3 * DATA_0_HIGH_TIME_SLOT_NUM)    //��β�͵�ƽʱ϶��

//���ⷢ��״̬
typedef enum
{
    IR_SEND_STATE_IDLE           = 0,   //����״̬����
    IR_SEND_STATE_LEADING_HIGH,         //����״̬����ǰ���ߵ�ƽ
    IR_SEND_STATE_LEADING_LOW,          //����״̬����ǰ���͵�ƽ
//    IR_SEND_STATE_DATA,                 //����״̬��������
    IR_SEND_STATE_DATA_0_HIGH,          //����״̬��������0�ߵ�ƽ
    IR_SEND_STATE_DATA_0_LOW,           //����״̬��������0�͵�ƽ
    IR_SEND_STATE_DATA_1_HIGH,          //����״̬��������1�ߵ�ƽ
    IR_SEND_STATE_DATA_1_LOW,           //����״̬��������1�͵�ƽ
    IR_SEND_STATE_ENDING_HIGH,          //����״̬���ͽ�β�ߵ�ƽ
    IR_SEND_STATE_ENDING_LOW,           //����״̬���ͽ�β�͵�ƽ
} IR_send_state_e;

typedef struct
{
    IR_send_state_e state;          //����״̬
//    uint8_t byte_cnt;               //���͵��ڼ��ֽ�
    uint8_t bit_cnt;                //���͵��ڼ�λ
    uint8_t time_slot_cnt;          //���͵��ڼ���ʱ϶
} IR_send_s;

IR_send_s g_IR_send = {IR_SEND_STATE_IDLE};

//���͵�����
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

//���
static IR_data_t g_IR_data_Shot[] =
{
    {0,     2 },     //���ʶ����
    {0x01,  6 },     //rid
    {0xffff,  10},     //pid
    {1,     6 },     //��������ҩID
    {0x0,     8 },     //��Ȩ��
    {1,     1 },     //������
    {0,     1 },     //��У��
};

#define IR_DATA_SHOT_NUM     (sizeof(g_IR_data_Shot) / sizeof(g_IR_data_Shot[0]))

//����
static IR_data_t g_IR_data_Adjust[] =
{
    {2,  2 },     //����ָ��ʶ����
    {0x32,  6 },     //rid
    {0xffff,  10},     //pid
    {0,  3 },     //����ָ�� 0-���� 1-���� 2-���ӳ������� 3-��������
    {0x0,  8 },     //��Ȩ��
    {0,  1 },     //��У��
};

#define IR_DATA_ADJUST_NUM     (sizeof(g_IR_data_Adjust) / sizeof(g_IR_data_Adjust[0]))

//��������
static IR_data_t g_IR_data_Supplement[] =
{
    {3,  2 },     //����ָ��ʶ����
    {0x32,  6 },     //rid
    {0xffff,  10},     //pid
    {1,  8 },     //��������ҩID
    {45,  10},     //����
    {0,  2 },     //��ʽ 0-���� 1-��ʼ������ 2-������ 3-����
    {0x0,  8 },      //��Ȩ��
    {0,  1 },     //��У��
};
#define IR_DATA_SUPPLEMENT_NUM     (sizeof(g_IR_data_Supplement) / sizeof(g_IR_data_Supplement[0]))


//����
static IR_data_t g_IR_data_ShotArround[] =
{
    {6,  4 },    //����
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
////    set_PT2;                                  //��ʱ���ж����ȼ�
//    set_ET2;                                    // Enable Timer2 interrupt
//    set_EA;
//    set_TR2;                                    // Timer2 run
//}

//����bitλ������ֵ������bitλΪ1������
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

    //��У��
    if (6 != g_IR_data[0].data)  //��������ҪУ��
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

        //���
        case 1:
            pData = g_IR_data_Shot;
            DataLen = IR_DATA_SHOT_NUM;
            break;
            #endif

        //����ѵ��ģʽ
        case 2:
            pData = g_IR_data_Adjust;
            g_IR_data_Adjust[3].data = 3;
            DataLen = IR_DATA_ADJUST_NUM;
            break;

        //����
        case 3:
            pData = g_IR_data_Adjust;
            g_IR_data_Adjust[3].data = 0;
            DataLen = IR_DATA_ADJUST_NUM;
            break;

        //����
        case 4:
            pData = g_IR_data_Adjust;
            g_IR_data_Adjust[3].data = 1;
            DataLen = IR_DATA_ADJUST_NUM;
            break;
            #if 0

        //���ӳ�������
        case K4_BIT_MASK:
            pData = g_IR_data_Adjust;
            g_IR_data_Adjust[3].data = 2;
            DataLen = IR_DATA_ADJUST_NUM;
            break;
            #endif

        //����
        case 5:
            pData = g_IR_data_Supplement;
            g_IR_data_Supplement[4].data = g_byBase;
            DataLen = IR_DATA_SUPPLEMENT_NUM;
            break;
            #if 0

        //��ʼ������
        case K6_BIT_MASK:
            pData = g_IR_data_Supplement;
            g_IR_data_Supplement[5].data = 1;
            DataLen = IR_DATA_SUPPLEMENT_NUM;
            break;

        //����
        case K7_BIT_MASK:
            pData = g_IR_data_ShotArround;
            DataLen = IR_DATA_SHOTARD_NUM;
            break;
            #endif

        //Ĭ�����ӳ�������
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

    //��У��
    if (6 != pData[0].data)  //��������ҪУ��
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

//���ⷢ���ʼ��-IO�ڲ������壬ʹ��Systick�ж�
void IR_Init(void)
{
    GPIO_InitType  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);	 //ʹ��PA,PB��PC�˿�ʱ��

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;		 //�������
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;		 //IO���ٶ�Ϊ50MHz

    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_5;		//�˿�����
    GPIO_Init(GPIOA, &GPIO_InitStructure);				//�����趨������ʼ��GPIOA

    //LASERA = 0;
    //Laser_B_On();
    Laser_A_Off();
    //Laser_B_Off();.

    PWM_time_slot_low();
    //�����
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
            SysTick->CTRL = 0;//��λSystick
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

static uint8_t g_IR_to_send_cnt = 0;       //��������ʹ���

//�Ƿ��ͺ���
uint8_t is_send_IR(void)
{
    if (g_IR_to_send_cnt > 0)
    {
        g_IR_to_send_cnt--;
        return 1;
    }

    return 0;
}

//��ʱ��⼤���Ƿ���Ҫ��������
void IR_TimedProcess(void)
{
    if (IR_is_idle())           //���ⷢ�Ϳ���
    {
        //LowPowerLed_Off(); // ����
        //SysTick->CTRL = 0;
        if (is_send_IR())       //��Ҫ���ͺ���
        {
            IR_send();          //���ͺ���
            // LowPowerLed_On();// ����
        }
    }
}


//�������ⷢ��	SEND_CNT_ONE_CLICK ��
//id-ʶ����  0-���ӳ������� 1-��� 2-ѵ��ģʽ 3-���� 4-����  5-����
//RedOrBlue 1-��� 0-����
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


