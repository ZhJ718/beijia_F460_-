/********************************************************************************
* @File name: main_task.c
* @Author: xfw
* @Version: 1.0
* @Date: 2021-7-1
* @Description:
********************************************************************************/
#include <time.h>
#include <stdlib.h>
#include "main_task.h"
#include "protocal.h"

#ifdef _APP
#include "lasercom_task.h"
#endif

#ifdef DEBUG

//���Կ�
void DebugComInit(void)
{
    UART_CONFIG Config = {0};
    UART_MANAGER *pUartMag = &g_sUartSetMag.Com[DEBUG_COM];

    memcpy((void *)&Config, (void *)&g_scUartDefConfig[DEBUG_COM], sizeof(UART_CONFIG));
    //��Ӧ��������
    Config.m_dwBaudRate = 115200;
    Config.TxMode = MODE_INTERRUPT;
    //Config.RxMode = MODE_INTERRUPT;

    Uart_ComConfiguration(Config, pUartMag);
    Uart_ComInit(pUartMag);
}

void printf_with_data(uint8_t *pBuf, uint16_t Len)
{
    #if 0
    uint8_t i;
    uint8_t temp[256];

    if(Len > 85 || Len == 0)
        return;

    memset(temp, '\0', sizeof(temp));

    for(i = 0; i < Len; i++)
    {
        sprintf((char *)&temp[3 * i], "%02X ", pBuf[i]);
    }

    LOG("%s\n\r", temp);
    #endif

    uint16_t i, TempLen;
    uint8_t *p = NULL;

    TempLen = 3 * Len + 5;
    p = sdram_malloc(0, TempLen);

    if(p == NULL)
    {
        return;
    }

    memset(p, '\0', TempLen);

    for(i = 0; i < Len; i++)
    {
        sprintf((char *)&p[3 * i], "%02X ", pBuf[i]);
    }

    #if 1

    if(i >= 250)
    {
        strcpy((char *)&p[3 * i], "......");
    }

    #endif
    p[3 * i] = 0x0d;
    p[3 * i + 1] = 0x0a;
    //LOG("%s\n\r", p);
    Uart_SendBuf(&g_sUartSetMag.Com[DEBUG_COM], p, 3 * i + 2);
    sdram_free(0, p);

}

#endif

void PowerOnOffInit(void)
{
    PwrKey_GpioInit();
    PwrEnable_GpioInit();
}

void PowerOnHandle(void)
{
    PwrEnable_On();
    //return;
}

void PowerOffHandle(void)
{
    PwrEnable_Off();

    //�ȴ������ɿ����ػ�
    while(1)
    {
        Delay_Ms(100);
        IWDG_Feed();
    }

    //return;
}






#ifdef _APP

#include "zigcom_task.h"
#include "rescom_task.h"
#include "lasercom_task.h"
#include "hmicom_task.h"
#include "battery_task.h"
#include "ir38com_task.h"


#if defined(HC32F46x)
void InitNVICSELRegister0_127(void)
{
    stc_intc_sel_field_t *stcIntSel;

    for(unsigned short i = Int000_IRQn; i <= Int127_IRQn; i++)
    {
        stcIntSel = (stc_intc_sel_field_t *)((uint32_t)(&M4_INTC->SEL0) + (4ul * i));
        stcIntSel->INTSEL = 0x1FFu;
    }
}
#endif

uint8_t BackStage_dataBuf[256] = {0};		//BackStage_dataBuf���ڴ���¼�����
#define BULLETUNIID_MAX (255)
#define WEAPONUNIID_MAX (255)

INJURY_MAG g_sInjuryMag;

const WEAPONUNID_T WeaponTypeUniID[WEAPONUNIID_MAX] =
{

    //̹�ˣ��ǻ��ģ�⣩
    {0x01, 0x01, 0},
    {0x01, 0x02, 0},
    {0x01, 0x03, 0},
    {0x01, 0x04, 0},
    {0x01, 0x05, 0},
    {0x01, 0x06, 0},
    {0x01, 0x07, 0},
    {0x01, 0x08, 0},
    {0x01, 0x09, 0},
    {0x01, 0x0a, 0},

    //̹�ˣ�Ƕ��ʽģ�⣩
    {0x02, 0x01, 0},

    //��ս��
    {0x03, 0x01, 0},
    {0x03, 0x02, 0},
    {0x03, 0x03, 0},
    {0x03, 0x04, 0},
    {0x03, 0x05, 0},
    {0x03, 0x06, 0},

    //װ�����ͳ�
    {0x04, 0x01, 0},
    {0x04, 0x02, 0},

    //ͻ����
    {0x05, 0x01, 0},
    {0x05, 0x02, 0},
    {0x05, 0x03, 0},
    {0x05, 0x04, 0},

    //ͻ����
    {0x06, 0x01, 0},
    {0x06, 0x02, 0},

    //��̹�˵���ϵͳ
    {0x07, 0x01, 1},
    {0x07, 0x02, 1},

    //ǣ������
    {0x10, 0x01, 0},
    {0x10, 0x02, 0},
    {0x10, 0x03, 0},

    //���л��ڣ������ֻ���
    {0x11, 0x01, 0},
    {0x11, 0x02, 0},

    //���л��ڣ����ֻ���
    {0x12, 0x01, 0},
    {0x12, 0x02, 0},
    {0x12, 0x03, 0},
    {0x12, 0x04, 0},
    {0x12, 0x05, 0},

    //�����
    {0x13, 0x01, 1},
    {0x13, 0x02, 1},
    {0x13, 0x03, 1},

    //�Ȼ���
    {0x14, 0x01, 40},
    {0x14, 0x02, 40},
    {0x14, 0x03, 40},
    {0x14, 0x04, 40},
    {0x14, 0x05, 40},

    //�ڱ����
    {0x15, 0x01, 0},
    {0x15, 0x02, 0},
    {0x15, 0x03, 0},
    {0x15, 0x04, 0},

    //���ڣ��л�أ�
    {0x20, 0x01, 0},
    {0x20, 0x02, 0},
    {0x20, 0x03, 0},
    {0x20, 0x04, 0},

    //���ڣ��޻�أ�
    {0x21, 0x01, 0},
    {0x21, 0x02, 0},
    {0x21, 0x03, 0},
    {0x21, 0x04, 0},

    //���յ���ϵ
    {0x22, 0x01, 1},
    {0x22, 0x02, 1},
    {0x22, 0x03, 1},
    {0x22, 0x04, 1},
    {0x22, 0x05, 1},

    //�����״�ϵͳ
    {0x23, 0x01, 0},
    {0x23, 0x02, 0},

    //����ָ��ϵͳ
    {0x24, 0x01, 0},
    {0x23, 0x02, 0},

    //�������յ���
    {0x25, 0x01, 1},
    {0x25, 0x02, 1},
    {0x25, 0x03, 1},
    {0x25, 0x04, 1},
    {0x25, 0x05, 1},

    //��Ա
    {0x30, 0x01, 0},
    {0x30, 0x02, 0},
    {0x30, 0x03, 0},

    //�Զ���ǹ
    {0x31, 0x01, 300},
    {0x31, 0x02, 300},
    {0x31, 0x03, 300},
    {0x31, 0x04, 300},
    {0x31, 0x05, 300},
    {0x31, 0x06, 300},
    {0x31, 0x07, 300},
    {0x31, 0x08, 300},
    {0x31, 0x09, 300},
    {0x31, 0x0a, 300},
    {0x31, 0x0b, 300},
    {0x31, 0x0c, 300},
    {0x31, 0x0d, 300},
    {0x31, 0x0e, 300},
    {0x31, 0x0f, 300},

    //���û�ǹ
    {0x32, 0x01, 1000},
    {0x32, 0x02, 1000},
    {0x32, 0x03, 1000},
    {0x32, 0x04, 1000},
    {0x32, 0x05, 1000},

    //�ѻ���ǹ
    {0x33, 0x01, 100},
    {0x33, 0x02, 100},
    {0x33, 0x03, 100},
    {0x33, 0x04, 100},
    {0x33, 0x05, 100},
    {0x33, 0x06, 100},
    {0x33, 0x07, 100},
    {0x33, 0x08, 100},
    {0x33, 0x09, 100},
    {0x33, 0x0a, 100},

    //΢�����ǹ
    {0x34, 0x01, 300},
    {0x34, 0x02, 300},
    {0x34, 0x03, 300},

    //����ǹ
    {0x35, 0x01, 0},
    {0x35, 0x02, 0},
    {0x35, 0x03, 0},

    //��ǹ
    {0x36, 0x01, 40},
    {0x36, 0x02, 40},

    //�����񵯷�����
    {0x37, 0x01, 60},
    {0x37, 0x02, 60},
    {0x37, 0x03, 60},
    {0x37, 0x04, 60},
    {0x37, 0x05, 60},
    {0x37, 0x06, 60},
    {0x37, 0x07, 60},
    {0x37, 0x08, 60},
    {0x37, 0x09, 60},
    {0x37, 0x0a, 60},
    {0x37, 0x0b, 60},

    //�ػ�ǹ
    {0x38, 0x01, 0},
    {0x38, 0x02, 0},
    {0x38, 0x03, 0},
    {0x38, 0x04, 0},

    //�����ǹ
    {0x39, 0x01, 0},
    {0x39, 0x02, 0},

    //��ѧ����豸
    {0x3a, 0x01, 0},
    {0x3a, 0x02, 0},

    //�������
    {0x40, 0x01, 1},
    {0x40, 0x02, 1},
    {0x40, 0x03, 1},
    {0x40, 0x04, 1},
    {0x40, 0x05, 1},

    //������̹�˻��
    {0x41, 0x01, 1},
    {0x41, 0x02, 1},
    {0x41, 0x03, 1},
    {0x41, 0x04, 1},

    //������̹�˵���
    {0x42, 0x01, 0},
    {0x42, 0x02, 0},
    {0x42, 0x03, 0},
    {0x42, 0x04, 0},

    //����
    {0x43, 0x01, 1},
    {0x43, 0x02, 1},
    {0x43, 0x03, 1},
    {0x43, 0x04, 1},

    //����������
    {0x44, 0x01, 0},
    {0x44, 0x02, 0},

    //����
    {0x45, 0x01, 0},
    {0x45, 0x02, 0},

    //���ױ�ը��
    {0x46, 0x01, 0},

    //���ھȻ�װ��
    {0x47, 0x01, 0},

    //�����豸
    {0x48, 0x01, 0},
    {0x48, 0x02, 0},
    {0x48, 0x03, 0},

    //ͨ�ó���
    {0x50, 0x01, 0},
    {0x50, 0x02, 0},

    //ָ�ӳ���
    {0x51, 0x01, 0},
    {0x51, 0x02, 0},
    {0x51, 0x03, 0},
    {0x51, 0x04, 0},
    {0x51, 0x05, 0},

    //ͨ�ų���
    {0x52, 0x01, 0},
    {0x52, 0x02, 0},

    //��ҩ���ͳ�
    {0x53, 0x01, 0},

    //�������䳵
    {0x54, 0x01, 0},

    //������׳�
    {0x55, 0x01, 0},

    //������쳵
    {0x56, 0x01, 0},
    {0x56, 0x02, 0},

    //�������ϳ�
    {0x57, 0x01, 0},
    {0x57, 0x02, 0},

    //�ۺ�ɨ�׳�
    {0x58, 0x01, 0},

    //���ͻ�е����
    {0x59, 0x01, 0},

    //���޳���
    {0x5a, 0x01, 0},

    //������ҵ����
    {0x5b, 0x01, 0},

    //ֱ����
    {0x60, 0x01, 0},
    {0x60, 0x02, 0},
    {0x60, 0x03, 0},
    {0x60, 0x04, 0},
    {0x60, 0x05, 0},
    {0x60, 0x06, 0},
    {0x60, 0x07, 0},
    {0x60, 0x08, 0},

    //���˻�
    {0x61, 0x01, 0},
    {0x61, 0x02, 0},
    {0x61, 0x03, 0},
    {0x61, 0x04, 0},
    {0x61, 0x05, 0},
    {0x61, 0x06, 0},

    //ս����/��ը��
    {0x62, 0x01, 0},
    {0x62, 0x02, 0},
    {0x62, 0x03, 0},
    {0x62, 0x04, 0},
    {0x62, 0x05, 0},
    {0x62, 0x06, 0},
    {0x62, 0x07, 0},
    {0x62, 0x08, 0},
    {0x62, 0x09, 0},
    {0x62, 0x0a, 0},
    {0x62, 0x0b, 0},
    {0x62, 0x0c, 0},
    {0x62, 0x0d, 0},
    {0x62, 0x0e, 0},
    {0x62, 0x0f, 0},
    {0x62, 0x10, 0},
    {0x62, 0x11, 0},
    {0x62, 0x12, 0},

    //���ӶԿ�
    {0x63, 0x01, 0},

    //������ʩ
    {0x64, 0x01, 0},
    {0x64, 0x02, 0},
    {0x64, 0x03, 0},
    {0x64, 0x04, 0},
    {0x64, 0x05, 0},
    {0x64, 0x06, 0},
    {0x64, 0x07, 0},
    {0x64, 0x08, 0},
    {0x64, 0x09, 0},
    {0x64, 0x0a, 0},
    {0x64, 0x0b, 0},
    {0x64, 0x0c, 0},
    {0x64, 0x0d, 0},
    {0x64, 0x0e, 0},
    {0x64, 0x0f, 0},
    {0x64, 0x10, 0},

    //ս���ϰ�
    {0x65, 0x01, 0},
    {0x65, 0x02, 0},
    {0x65, 0x03, 0},
    {0x65, 0x04, 0},
    {0x65, 0x05, 0},
    {0x65, 0x06, 0},
    {0x65, 0x07, 0},
    {0x65, 0x08, 0},
    {0x65, 0x09, 0},
    {0x65, 0x0a, 0},
    {0x65, 0x0b, 0},

};

const AmmuID AmmoTypeID[BULLETUNIID_MAX] =
{
    {0, 0}, //ռ�õ�һ�������±�0
    {1, 1}, //5.8mm ��ͨ�ӵ�
    {1, 2}, //7.62mm ��ͨ�ӵ�
    {1, 3}, //12.7mm ��ͨ�ӵ�
    {1, 4},  //5.56mm ��ͨ�ӵ�
    {1, 5},  //9mm ��ͨ�ӵ�

    {2, 1}, //5.8mm ���׵�
    {2, 2}, //12.7mm ���׵�

    {3, 1}, //5.8mm ����ȼ�յ�
    {3, 2}, //12.7mm ����ȼ�յ�

    {4, 1}, //5.8mm ȼ�յ�
    {4, 2}, //12.7mm ȼ�յ�

    {5, 1}, //12.7mm ���ױ�ըȼ�յ�

    //11
    {6, 1}, //12.7mm ˲����
    {7, 1}, //����
    {8, 0}, //����
    {9, 0}, //����
    {10, 0}, //����
    {11, 0}, //����
    {12, 0}, //����
    {13, 0}, //����
    {14, 0}, //����
    {15, 0}, //����

    //21
    {16, 1}, //85mm ̹��ɱ����
    {16, 2}, //125mm ̹��ɱ����
    {16, 3}, //120mm ̹��ɱ����
    {16, 4}, //105 ̹��ɱ����
    {16, 5}, //100 ̹��ɱ����
    {16, 6}, //73 ������ɱ����
    {16, 7}, //35mm ɱ����
    {16, 8}, //40mm ɱ����
    {17, 1}, //73mm �Ƽ�ɱ�˵�
    {18, 0}, //ȼ�յ�/ɱ��ȼ�յ� �����ͱ���

    //31
    {19, 0}, //��ըɱ�˵� �����ͱ���
    {20, 1}, //85mm ̹���Ƽ׵�
    {20, 2}, //125mm ̹���Ƽ׵�
    {20, 3}, //120mm ̹���Ƽ׵�
    {20, 4}, //105 ̹���Ƽ׵�
    {20, 5}, //100 ̹���Ƽ׵�
    {20, 6}, //120mm �Ƽ׵�
    {20, 7}, //82mm �Ƽ׵�
    {20, 8}, //40mm �Ƽ׵�
    {21, 1}, //80mm ���ᵯ
    //41
    {22, 1}, //93mm �Ʊ���
    {22, 2}, //80mm �Ʊ���
    {23, 1}, //80mm ɱ�˱���ȼ�յ�
    {24, 1}, //73mm ɱ�˱�����
    {25, 0}, //�Ƽ�ȼ�յ�  �����ͱ���
    {26, 1}, //85mm ̹�˴��׵�
    {26, 2}, //125mm ̹�˴��׵�
    {26, 3}, //120mm ̹�˴��׵�
    {26, 4}, //105 ̹�˴��׵�
    {26, 5}, //100 ̹�˴��׵�

    //51
    {26, 6}, //30mm ���׵�
    {27, 0}, //��׵�   �����ͱ���
    {28, 1}, //120mm ����;��
    {29, 0}, //����
    {30, 0}, //����
    {31, 0}, //����
    {48, 1}, //25mm ���ڴ��׵�
    {48, 2}, //35mm ���ڴ��׵�
    {48, 3}, //37mm ���ڴ��׵�
    {48, 4}, //57mm ���ڴ��׵�
    {48, 5}, //100mm ���ڴ��׵�
    {48, 6}, //20mm���ڴ��׵�

//61
    {48, 5}, //100mm ���ڴ��׵�
    {49, 0}, //���ڱ�����  �����ͱ���
    {50, 0}, //���ڴ��ױ���ȼ�յ�   �����ͱ���
    {51, 0}, //���ڶ�������Ԥ����Ƭ��  �����ͱ���
    {52, 0}, //����
    {53, 0}, //����
    {54, 0}, //����
    {55, 0}, //����
    {56, 0}, //����
    {57, 0}, //����

//71
    {58, 0}, //����
    {59, 0}, //����
    {60, 0}, //����
    {61, 0}, //����
    {62, 0}, //����
    {63, 0}, //����
    {64, 1}, //��� 73 ����
    {64, 2}, //��� 8 ����
    {64, 3}, //��� 9 ����
    {64, 4}, //��� 10 ����
//81
    {64, 5}, //ǰ��һ����
    {64, 6}, //125mm ̹�����䵼��
    {64, 7},  //������̹�˵���

    {65, 1}, //��ӧ 5 ���յ���
    {65, 2}, //��ӧ 6 ���յ���
    {65, 3}, //���12����
    {65, 4}, //��ǹFGM-148����
    {65, 5}, //����FIM92���յ���
    {65, 6}, //"������"��̹�˵���

    {66, 1}, //MK-82/84����
    {66, 2}, //250���ﺽ��
    {66, 3}, //500���ﺽ��
    {66, 4}, //�յػ����
    {66, 5}, //CBU-XX����ը��


    {67, 1}, //X-59M�����Ƶ�����
    {67, 2}, //X-29T���ӣ����⣩�Ƶ�����
    {67, 3}, //YJ-88�յص���
    {67, 4}, //YJ-63A/B�յص���
    {67, 5}, //"Сţ"�յص���

    {68, 0}, //����
    {69, 0}, //����
    {70, 0}, //����
    {71, 0}, //����
//91
    {72, 0}, //����
    {73, 0}, //����
    {74, 0}, //����
    {75, 0}, //����
    {76, 0}, //����
    {77, 0}, //����
    {78, 0}, //����
    {79, 0}, //����
    {80, 1}, //���ᵯ
    {81, 0}, //����
//101
    {82, 0}, //����
    {83, 0}, //����
    {84, 0}, //����
    {85, 0}, //����
    {86, 0}, //����
    {87, 0}, //����
    {88, 0}, //����
    {89, 0}, //����
    {90, 0}, //����
    {91, 0}, //����
//111
    {92, 0}, //����
    {93, 0}, //����
    {94, 0}, //����
    {95, 0}, //����
    {96, 0}, //����
    {97, 0}, //����
    {98, 0}, //����
    {99, 0}, //����
    {100, 0}, //����
    {101, 0}, //����
//121
    {102, 0}, //����
    {103, 0}, //����
    {104, 0}, //����
    {105, 0}, //����
    {106, 0}, //����
    {107, 0}, //����
    {108, 0}, //����
    {109, 0}, //����
    {110, 0}, //����
    {111, 0}, //����
//131
    {112, 1}, //82mm ɱ����
    {112, 2}, //100mm ɱ����
    {112, 3}, //105mm ɱ����
    {112, 4}, //120mm ɱ����


    {112, 5}, //122mm ɱ����
    {112, 6}, //130mm ɱ����
    {112, 7}, //152mm ɱ����
    {112, 8}, //155mm ɱ����
    {112, 9}, //203mm ɱ����
    {112, 10}, //240mm ɱ����

//141
    {112, 11}, //152mm ĩ�Ƶ��ڵ�
    {112, 12}, //60mm ɱ����
    {113, 1}, //122mm �����
    {113, 2}, //107mm �����


    {113, 3}, //���� 2000 �����
    {113, 4}, //����-6 �����
    {113, 5}, //300mm ɱ����
    {113, 6}, //300mm ��ĸ��
    {113, 7}, //300mm ĩ����
    {113, 8}, //300mm �Ʊ���
    {113, 9},//Mk.118 Mod0 HEDP�����

//151
    {114, 1}, //������׵�����̹���ף�
    {114, 2}, //������׵����������ף�
    {115, 1}, //������Ƶ�
    {116, 1}, //����������


    {116, 2}, //��̹�˵���
    {116, 3}, //����׳�
    {117, 0}, //����
    {118, 1}, //ɳ��
    {118, 2}, //����
    {118, 3}, //ά����˹
//161
    {118, 4}, //������
    {118, 5}, //������
    {118, 6}, //��·���
    {118, 7}, //��״������
    {118, 8}, //��״��·
    {119, 1}, //����

    {120, 0}, //���Ե�
    {121, 0}, //���
    {122, 0}, //�þ�

}  ;

const uint8_t DamageTableByBullet[][9] =
{

    //0
    {0, 0, 0, 0, 0, 0, 0, 0, 0}, //ռ�õ�һ�������±�0
    //5.8mm ��ͨ�ӵ�
    {100, 100, 80, 60/*����*/, 50/*����*/, 50, 50, 50, 50},

    //7.62mm ��ͨ�ӵ�
    {100, 100, 80, 60, 80, 50, 50, 50, 50},

    //12.7mm ��ͨ�ӵ�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //5.56mm�ӵ�
    {100, 100, 80, 60, 50, 50, 50, 50, 50},

    //9mm ��ͨ�ӵ�
    {100, 100, 90, 70, 90, 60, 60, 60, 60},

    //5.8mm ���׵�
    {100, 100, 80, 60, 80, 40, 50, 30, 30},

    //12.7mm ���׵�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //5.8mm ����ȼ�յ�
    {100, 100, 100, 90, 90, 50, 60, 30, 40},

    //12.7mm ����ȼ�յ�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //5.8mm ȼ�յ�
    {100, 100, 100, 90, 90, 50, 60, 30, 40},

    //12.7mm ȼ�յ�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //12.7mm ���ױ�ըȼ�յ�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

//11
    //12.7mm ˲����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
//21
    //85mm ̹��ɱ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //125mm  ̹��ɱ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //120mm  ̹��ɱ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //105mm  ̹��ɱ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //100mm  ̹��ɱ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //73 ������ɱ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //35mm ɱ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //40mm ɱ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //73mm �Ƽ�ɱ�˵�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //ȼ�յ�/ɱ��ȼ�յ�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
//31
    //��ըɱ�˵�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //85mm ̹���Ƽ׵�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //125mm ̹���Ƽ׵�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //120mm ̹���Ƽ׵�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //105mm ̹���Ƽ׵�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //100mm ̹���Ƽ׵�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //120mm �Ƽ׵�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //82mm �Ƽ׵�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //40mm �Ƽ׵�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //80mm ���ᵯ
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
//41
    //93mm �Ʊ���
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //80mm �Ʊ���
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //80mm ɱ�˱���ȼ�յ�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //73mm ɱ�˱�����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //�Ƽ�ȼ�յ���������
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //85mm ̹�˴��׵�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //125mm ̹�˴��׵�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //120mm ̹�˴��׵�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //105 ̹�˴��׵�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //100 ̹�˴��׵�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
//51
    //30mm ���׵�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //��׵�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //120mm ����;��
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //����
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},

    //25mm ���ڴ��׵�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //35mm ���ڴ��׵�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //37mm ���ڴ��׵�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //57mm ���ڴ��׵�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
//61
    //100mm ���ڴ��׵�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //20mm���ڴ��׵�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //���ڱ�����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //���ڴ��ױ���ȼ�յ�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //���ڶ�������Ԥ����Ƭ��
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
//71
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //��� 73 ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //���   8 ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //��� 9 ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //��� 10 ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
//81
    //ǰ��һ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //125mm ̹�����䵼��
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //������̹�˵���
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //��ӧ 5 ���յ���
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //��ӧ 6 ���յ���
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //���12����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //��ǹFGM-148����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //����FIM92���յ���
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //"������"��̹�˵���
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //MK-82/84����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //250���ﺽ��
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //500���ﺽ��
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //�յػ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //CBU-XX����ը��
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //X-59M�����Ƶ�����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //X-29T���ӣ����⣩�Ƶ�����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //YJ-88�յص���
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //YJ-63A/B�յص���
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //"Сţ"�յص���
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

//91
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //���ᵯ
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

//101
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
//111
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
//121
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

//131
    //82mm ɱ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //100mm ɱ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //105mm ɱ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //120mm ɱ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //122mm ɱ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //130mm ɱ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //152mm ɱ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //155mm  ɱ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //203mm  ɱ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //240mm   ɱ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
//141
    //152mm ĩ�Ƶ��ڵ�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //60mm  ɱ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //122mm �����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //107mm �����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //���� 2000 �����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //����-6 �����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //300mm ɱ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //300mm ��ĸ��
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //300mm  ĩ����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //300mm �Ʊ���
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //Mk.118 Mod0 HEDP�����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
//151
    //������׵�����̹���ף�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //������׵����������ף�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //������Ƶ�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //����������
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //��̹�˵���
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //����׳�
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //ɳ��
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //ά����˹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
//161
    //������
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //������
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //��·���
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //��״������
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //��״��·
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //����
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //���Ե�
    {0, 0, 0, 0, 0, 0, 0, 0, 0},

    //���
    {0, 0, 0, 0, 0, 0, 0, 0, 0},

    //�þ�
    {0, 0, 0, 0, 0, 0, 0, 0, 0},

    //����--
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
};

const uint8_t ID_PARTITION[][10] =
{
    {4, 2, 2, 5, 5, 5, 5, 3, 3, 4},
    {4, 2, 2, 5, 5, 5, 5, 3, 3, 4},
    {4, 2, 2, 5, 5, 5, 5, 3, 3, 4},
    {4, 2, 2, 5, 5, 5, 5, 3, 3, 4},
    {4, 2, 2, 5, 5, 5, 5, 3, 3, 4},
    {4, 2, 2, 5, 5, 5, 5, 3, 3, 4},
    {4, 2, 2, 5, 5, 5, 5, 3, 3, 4},
    {4, 2, 2, 5, 5, 5, 5, 3, 3, 4},
    {4, 2, 2, 5, 5, 5, 5, 3, 3, 4},
    {4, 2, 2, 5, 5, 5, 5, 3, 3, 4},
    {4, 2, 2, 5, 5, 5, 5, 3, 3, 4},
    //1����2���ء�3���ء�4������5������6������7������8���ء�9���ء�10����
//  PARTITION_NONE    = 0,
//	PARTITION_HEAD    = 1	,	//	ͷ
//	PARTITION_LCHEST  = 2	,	//  ����
//	PARTITION_RCHEST  = 3	,	//  ����
//	PARTITION_ABDOMEN = 4	,	//  ����
//	PARTITION_BACK    = 5	,	//  ��
//	PARTITION_LARM    = 6,	    //	���
//	PARTITION_RARM    = 7	,	//	�ұ�
//	PARTITION_LLEG    = 8	,	//	����
//	PARTITION_RLEG    = 9	,	//	����
//	PARTITION_MAX     = 10,
};

POWER_OFF_MAG 		g_sPowerOffMag;

//���ػ���ʼ������ɿ����߼����ú����ᵼ��ϵͳ����1s
void AppPowerOnOffInit(POWER_OFF_MAG *pMag)
{
    PwrKey_GpioInit();
    PwrEnable_GpioInit();
    pMag->m_bIsOff = FALSE;
    pMag->m_wTimer = 0;
    pMag->m_bPoweroff_Force = FALSE;
		pMag->m_bPoweroffPre = FALSE;
}

//���ػ���ʱ������ɹػ���� 100ms��ʱ����
void AppPowerOnOffTimedProcess(POWER_OFF_MAG *pMag)
{
    volatile u8 Sta = Is_PwrKeyOn();

    if(Sta == 1)
    {
        pMag->m_wTimer++;

        if(pMag->m_wTimer > POWEROFFCOUNT)
        {
            pMag->m_bIsOff = TRUE;
            #ifdef SEND_TO_UPCOM
            if(pMag->m_wTimer % 5 == 0)
            {
                ResComTask_SendToUpcom((uint8_t *)&Sta, 1, 0x02, CTRL_DI);
            }

            #endif
        }
        else if(pMag->m_wTimer > POWEROFFSTARTCOUNT)
        {

        }
    }
    else
    {
        pMag->m_wTimer = 0;
        pMag->m_bIsOff = FALSE;
    }

}
void AppPowerOffHandle(POWER_OFF_MAG *pMag)
{
		static uint8_t cnt = 0;
	
		if(pMag->m_bPoweroffPre == TRUE)
		{
			if(cnt++ >= 2)
				pMag->m_bPoweroff_Force = TRUE;
		}
    //����Ƿ��йػ���������ѵ��ģʽ
    if((pMag->m_bIsOff == TRUE) || (pMag->m_bPoweroff_Force == TRUE))
    {
				if(g_sUserParam.m_byEnableChangeBat == FALSE)
				{
					if(g_sUserParam.m_byMode == GAME_GOING && pMag->m_bPoweroff_Force == FALSE)
					{
							//����йػ�������������ϰģʽ
							Wtn6_Play(WTN6_Mode_Mano); 		//������ʾ����ϰģʽ��
							Wtn6_Play(WTN6_Death_Illegal_Stop);  	//������ʾ���Ƿ��ػ���
							pMag->m_bIsOff = FALSE;
							return;
					}
					else if(g_sUserParam.m_byMode == GAME_GOING && pMag->m_bPoweroff_Force == TRUE)
					{
							g_sUserParam.m_byMode = TRAIN;
							g_sUserParam.m_sTsChar1.m_bUnitmode = FALSE;
					}
				}
				
        IWDG_Feed();
				UserParam_Save();
				IWDG_Feed();
        Wtn6_Sound(WTN6_SystemStop);  // ������ʾ��ϵͳ�ػ���
				Delay_Ms(500);
        g_sRunledMag.m_sHwOps.led_off();//�ص�
        Motor_On(&g_sMotorMag, OPT_MODE_BLOCK, 0);
        Delay_Ms(500);
        PwrEnable_Off(); //�ػ�

        do
        {
            Delay_Ms(100);
            PwrEnable_Off(); //�ػ�
            Motor_On(&g_sMotorMag, OPT_MODE_BLOCK, 0);
            IWDG_Feed();//��ֹ���Ź�ҧ������ϵͳ��λ���ػ���һֱ����ϵͳ����
        }
        while(1);
    }

}

void DrillStateParamHandle(void)
{
    static uint8_t Cnt = 0;

    if(g_sUserParam.m_byMode != TRAIN)
    {
        if(Cnt == 5) //5s��ʱ��
        {
            Cnt = 0;
            SynchronizeState(GAME_GOING, g_sUserParam.m_byEnableChangeBat);

            //������Ϣ�б仯����Ҫ����
            if(g_sResComTask_Mag.BackupArea[0].XOR != g_sResComTask_Mag.BackupArea[1].XOR)
            {
								BackupDrillState();
            }
        }
				Cnt++;
    }
}

//�Ƿ��ػ������漰������������10s�����ж�
void IllegalPowerOffHandle(void)
{
    static u8 UpLoadFlg 	= 0;
    RCVHURTINFO_T HurtInfo	= {0};

		static uint8_t Times = 0;

		if(Times++ < 10)
			return;
				
		Times = 0;
		
    if(g_sUserParam.m_IsIllegalPowerOff == TRUE)
    {
        g_sUserParam.m_IsIllegalPowerOff = FALSE;

        Beep_On(&g_sBeepMag, OPT_MODE_NONBLOCK, 800);
        Wtn6_Play(WTN6_ILLEGAL_TURNOFF);
        UpLoadFlg = 1;
    }

    if(UpLoadFlg == 1)
    {
        //�ػ������¼�
        MasterPowerOffCheatStartUploadHandle();
        MasterPowerOffCheatEndUploadHandle();
        UpLoadFlg = 0;

				HurtInfo.m_RT_Type = 0xff;
				HurtInfo.m_RT_Subtype = 0xff;
				HurtInfo.m_A_Type = 0xff;
				HurtInfo.m_A_Subtype = 0xff;
				HurtInfo.m_HType = H_Type_I_BACKSTAGECTRL;
				HurtInfo.m_HurtBlood = 0;
				HurtInfo.m_RRID = IDParam_GetRid();;
				HurtInfo.m_wRPID = IDParam_GetPid();;
				UpdateDamage(HurtInfo);
    }
}

void FumeInstallHandle(void)
{
    static u8 Cnt = 0;
	
		if(g_sUserParam.m_byMode == TRAIN)
		{
        return;
		}
		
		if(g_sResComTask_Mag.DrillParam.DisableInstallFume == TRUE)
				return;
		
    Cnt++;
		
    if(Cnt >= 20)
    {
        Cnt = 0;

        if(UserParam_GetBlood() != 0 
					&& g_sUserParam.m_byMode == GAME_GOING
					&&g_sHmiComTask_Mag.m_byHelmetIsOnline == TRUE)
        {
						if(UserParam_GetFireInstallFlg() == FALSE)
            {
								if(g_sResComTask_Mag.Vios.FumeUnInstall == FALSE)
								{
										g_sResComTask_Mag.FumeInstallStartTime++;
										//1����
										if(g_sResComTask_Mag.FumeInstallStartTime >= 3)
										{
												g_sResComTask_Mag.Vios.FumeUnInstall = TRUE;
												g_sResComTask_Mag.FumeInstallStartTime = 0;
												g_sResComTask_Mag.FumeInstallEndTime = 0;
												//���׿�ʼ�¼�
												FumeCheatStartUploadHandle();
												Wtn6_Play(WTN6_EXPLOSIVE_DEVICE_NOT_INSTALL);
												g_sResComTask_Mag.FumeInstallTimeStamp = g_sUserParam.m_sUtcTime;
												g_sUserParam.m_sTsChar1.m_bFailureState = TRUE;
												g_sUserParam.m_sTsChar1.m_bLocked = TRUE;
												UserParam_SetTsChar1(g_sUserParam.m_sTsChar1);
												//���������
												InterConn_ProtDeal_Slaver_Weapon_State(TRUE);		//����״̬ͬ��
										}
								}
            }
            else 
						{
								if(g_sResComTask_Mag.Vios.FumeUnInstall == TRUE)
								{
										if(g_sResComTask_Mag.FumeInstallEndTime >= 3)
										{
												g_sResComTask_Mag.Vios.FumeUnInstall = FALSE;
												g_sResComTask_Mag.FumeInstallStartTime = 0;
												g_sResComTask_Mag.FumeInstallEndTime   = 0;
												g_sUserParam.m_sTsChar1.m_bFailureState = FALSE;
												//���׽����¼�
												FumeCheatEndUploadHandle();
										}
								}
						}
        }
    }
}

void DetectorLossHandle(void)
{
    if(g_sUserParam.m_byMode == TRAIN)
        return;

		if(((UserParam_GetActLaserCnt() +  UserParam_GetActLaserCnt1()) < 15)
			|| LaserComTask_HandleGet()->m_DectorShelterFlg == TRUE)
		{
				if(g_sResComTask_Mag.Vios.DetectorLoss == FALSE)
				{
						g_sResComTask_Mag.DetectorLossStartTime++;

						if(g_sResComTask_Mag.DetectorLossStartTime > 240)
						{
								g_sResComTask_Mag.Vios.DetectorLoss = TRUE;
								g_sResComTask_Mag.DetectorLossStartTime = 0;
								g_sResComTask_Mag.DetectorLossEndTime = 0;
								g_sUserParam.m_sTsChar1.m_bFailureState = TRUE;
								Wtn6_Play(WTN6_CHEAT_EVENT);
								//�ϱ���ʼ�¼�
								DetectorLossEvtStartUpload();
								//��¼�ϱ���ʼʱ��
								g_sResComTask_Mag.DetectorLossTimeStamp = g_sUserParam.m_sUtcTime;;
						}
				}
				else if(g_sResComTask_Mag.Vios.DetectorLoss == TRUE)
				{
						if(g_sResComTask_Mag.DetectorLossEndTime > 3)
						{
								g_sResComTask_Mag.Vios.DetectorLoss = FALSE;
								g_sResComTask_Mag.DetectorLossStartTime = 0;
								g_sResComTask_Mag.DetectorLossEndTime = 0;
								g_sUserParam.m_sTsChar1.m_bFailureState = FALSE;
								//�ϱ������¼�
								DetectorLossEvtStopUpload();
						}
				}
		}
}

//�Ƿ��ƶ�̽��
void IllegalMoveMonitor(void)
{
		double RemoteLon, RemoteLat, LocalLon, LocalLat, distance;	
		static int cnt = 0;
		
		if(g_sUserParam.m_byMode == TRAIN)
        return;
	
    if((GetInjuryDegree(g_sUserParam.m_byBlood) <= CI_SOLDIER_S9) && cnt == 0)
    {
        DeathRecord();
        cnt = 1;
    }
    else if(GetInjuryDegree(g_sUserParam.m_byBlood) > CI_SOLDIER_S9)
    {
        cnt = 0;
    }

		if(GAME_GOING == g_sUserParam.m_byMode)
		{
				if(IsDie(g_sUserParam.m_eStatus) || (GetInjuryDegree(g_sUserParam.m_byBlood) <= CI_SOLDIER_S9))
				{
						//���ؾ���
						LocalLon = g_sResComTask_Mag.m_sNmeaMag.m_byJD_Du + (g_sResComTask_Mag.m_sNmeaMag.m_byJD_Fen + g_sResComTask_Mag.m_sNmeaMag.m_dwJD_Fen_XiaoShu / 100000.0) / 60.0; //��������徭��
											/*
											g_sUserParam.m_sLPosition.m_byJD_DuHundred * 100 + 
											 g_sUserParam.m_sLPosition.m_byJD_DuTen * 10 + 
											 g_sUserParam.m_sLPosition.m_byJD_DuUnit + 
											 (g_sUserParam.m_sLPosition.m_byJD_FenTen * 10 + 
											 g_sUserParam.m_sLPosition.m_byJD_FenUnit + 
											 (g_sUserParam.m_sLPosition.m_byJD_FenTenth / 10000 + 
											 g_sUserParam.m_sLPosition.m_byJD_FenPercent / 1000+ 
											 g_sUserParam.m_sLPosition.m_byJD_FenThousandth / 100 + 
											 g_sUserParam.m_sLPosition.m_byJD_FenTenThousandth / 10) / 100000.0	 
											 ) / 60.0
											 ;
											*/	
						//����γ��	
						LocalLat = g_sResComTask_Mag.m_sNmeaMag.m_byWD_Du + (g_sResComTask_Mag.m_sNmeaMag.m_byWD_Fen + g_sResComTask_Mag.m_sNmeaMag.m_dwWD_Fen_XiaoShu / 100000.0) / 60.0; //����������γ��
											/*
											g_sUserParam.m_sLPosition.m_byWD_DuTen * 10 + 
											 g_sUserParam.m_sLPosition.m_byWD_DuUnit + 
											 (g_sUserParam.m_sLPosition.m_byWD_FenTen * 10 + 
											 g_sUserParam.m_sLPosition.m_byWD_FenUnit + 
											 (g_sUserParam.m_sLPosition.m_byWD_FenTenth / 10000 + 
											 g_sUserParam.m_sLPosition.m_byWD_FenPercent / 1000+ 
											 g_sUserParam.m_sLPosition.m_byWD_FenThousandth / 100 + 
											 g_sUserParam.m_sLPosition.m_byWD_FenTenThousandth / 10) / 100000.0	 
											 ) / 60.0
											 ;
											 */
						//����ʱ����
						RemoteLon = g_sUserParam.m_sLastDeathMag.m_fJD; 

						//����ʱγ��	
						RemoteLat = g_sUserParam.m_sLastDeathMag.m_fWD; 

						distance = get_distance(LocalLat, LocalLon, RemoteLat, RemoteLon);
						distance = fabs(distance)/10.0f;

						if(g_sResComTask_Mag.Vios.IllegalMove == FALSE)
						{
								g_sResComTask_Mag.IllegalMoveStartTime++;

								if((uint32_t)distance < MOVE_OFFSET_MAX)
								{
										g_sResComTask_Mag.IllegalMoveStartTime = 0;
								}

								if(g_sResComTask_Mag.IllegalMoveStartTime > 10)
								{
										g_sResComTask_Mag.Vios.IllegalMove = TRUE;
										g_sResComTask_Mag.IllegalMoveStartTime = 0;
										g_sResComTask_Mag.IllegalMoveEndTime = 0;
										//�ϱ���ʼ�¼�
										IllegalMoveStartUploadHandle();
										Wtn6_Play(WTN6_CHEAT_EVENT);
										Wtn6_Play(WTN6_ILLEGAL_MOVE);
										//��¼�ϱ���ʼʱ��
										g_sResComTask_Mag.IllegalMoveTimeStamp = g_sUserParam.m_sUtcTime;
								}
						}
						else if(g_sResComTask_Mag.Vios.IllegalMove == TRUE)
						{
								if((uint32_t)distance < (MOVE_OFFSET_MAX - 10))
								{
										g_sResComTask_Mag.IllegalMoveEndTime++;
								}

								if(g_sResComTask_Mag.IllegalMoveEndTime > 10)
								{
										g_sResComTask_Mag.Vios.IllegalMove = FALSE;
										g_sResComTask_Mag.IllegalMoveStartTime = 0;
										g_sResComTask_Mag.IllegalMoveEndTime = 0;
										//�ϱ������¼�
										IllegalMoveEndUploadHandle();
								}
						}
					}
					else
					{
							if(g_sResComTask_Mag.Vios.IllegalMove == TRUE)
							{
									g_sResComTask_Mag.Vios.IllegalMove = FALSE;
									g_sResComTask_Mag.IllegalMoveStartTime = 0;
									g_sResComTask_Mag.IllegalMoveEndTime = 0;
									//�ϱ������¼�
									IllegalMoveEndUploadHandle();
							}
					}
			}
}

//��ϰģʽ��ͷ�����ߴ���
void IllegalHelmetDisconHandle(void)
{
		SINGLE_SLAVER_MAG *pSlaverMag = NULL;
    uint8_t i = 0, HelMetBatResFlg = FALSE;
    static uint8_t Cnt = 0;
		
		if(g_sUserParam.m_byMode == TRAIN)
        return;
	
    if(g_sResComTask_Mag.HelmetLossFlg == TRUE)
    {
        Cnt++;

        if(Cnt >= 120)
        {
            Cnt = 0;
            g_sResComTask_Mag.HelmetLossFlg = FALSE;
        }
    }

		if(g_sHmiComTask_Mag.m_byHelmetIsOnline == FALSE)
		{
				for(i = 0; i < UserParam_GetSlaverCnt(); i++)
				{
						pSlaverMag = UserParam_GetSlaverMag(i);
						if(pSlaverMag->m_byTType == 0xFE && pSlaverMag->m_byTSubType == 0x08)
						{
								HelMetBatResFlg = TRUE;
						}
				}

				if(i >= UserParam_GetSlaverCnt())
				{
						HelMetBatResFlg = TRUE;
				}

				if(HelMetBatResFlg == TRUE)
				{
						if(g_sResComTask_Mag.Vios.HelmetLoss == FALSE && g_sResComTask_Mag.HelmetLossFlg == TRUE)
						{
								g_sResComTask_Mag.HelmetLossStartTime++;

								if(g_sResComTask_Mag.HelmetLossStartTime > 30)
								{
										g_sResComTask_Mag.HelmetLossStartTime = 0;
										g_sResComTask_Mag.HelmetLossEndTime  = 0;
										g_sResComTask_Mag.Vios.HelmetLoss = TRUE;
										g_sResComTask_Mag.HelmetLossFlg = FALSE;
										g_sUserParam.m_sTsChar1.m_bFailureState = TRUE;
										g_sUserParam.m_sTsChar1.m_bLocked = TRUE;
										UserParam_SetTsChar1(g_sUserParam.m_sTsChar1);
										Wtn6_Play(WTN6_CHEAT_EVENT);
										Wtn6_Play(WTN6_HELMET_NOT_POWERON);
										//���׿�ʼ�¼�
										HelmetCheatStartUploadHandle();
										g_sResComTask_Mag.HelmetLossTimeStamp = g_sUserParam.m_sUtcTime;
								}
						}
				}

		}
		else
		{
				if(g_sResComTask_Mag.Vios.HelmetLoss == TRUE && g_sUserParam.m_byMode == GAME_GOING)
				{
						g_sResComTask_Mag.HelmetLossEndTime++;

						if(g_sResComTask_Mag.HelmetLossEndTime > 60)
						{
								g_sResComTask_Mag.HelmetLossStartTime = 0;
								g_sResComTask_Mag.HelmetLossEndTime  = 0;
								g_sResComTask_Mag.Vios.HelmetLoss = FALSE;
								g_sUserParam.m_sTsChar1.m_bFailureState = FALSE;
								//���׽����¼�
								HelmetCheatEndUploadHandle();
						}
				}
		}
}

void IllegalEventHandle(void)
{
		if(g_sResComTask_Mag.DrillParam.DisableCheat == TRUE)
			return;
		
		FumeInstallHandle();		//����ðδ��װ����
		DetectorLossHandle();		//̽ͷδ��������
		IllegalMoveMonitor();		//������Ƿ��ƶ�����
		IllegalHelmetDisconHandle();		//ͷ��δ����
		IllegalPowerOffHandle();	//�Ƿ��ػ�
}

LED_MAG g_sRunledMag;

/****************************************************************
@FUNCTION�����еƳ�ʼ��
@INPUT��LED����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ���е�Ӳ����������ǰ����
			@@ RunLed_GpioInit
			@@ RunLed_On
			@@ RunLed_Off
****************************************************************/
extern void RunLed_Init(LED_MAG *pMag)
{
    DDL_ASSERT(pMag != NULL);

    memset(pMag, 0, sizeof(LED_MAG));

    pMag->m_sHwOps.led_hw_init = RunLed_GpioInit;
    pMag->m_sHwOps.led_on = RunLed_On;
    pMag->m_sHwOps.led_off = RunLed_Off;

    Led_Init(pMag);
}

LED_MAG g_sStaledMag;

/****************************************************************
@FUNCTION��״ָ̬ʾ�Ƴ�ʼ��
@INPUT��LED����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ���е�Ӳ����������ǰ����
			@@ RLed_GpioInit
			@@ RLed_On
			@@ RLed_Off
��˸ָʾ��

****************************************************************/
extern void StaLed_Init(LED_MAG *pMag)
{
    DDL_ASSERT(pMag != NULL);

    memset(pMag, 0, sizeof(LED_MAG));

    pMag->m_sHwOps.led_hw_init = RLed_GpioInit;
    pMag->m_sHwOps.led_on = RLed_On;
    pMag->m_sHwOps.led_off = RLed_Off;

    Led_Init(pMag);
}



BEEP_MAG g_sBeepMag;

/****************************************************************
@FUNCTION��������ģʽ����Դ���������ƹ��ܳ�ʼ��
@INPUT��BEEP����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern void Beep_Init(BEEP_MAG *pMag)
{
    BEEP_HW_OPERATIONS BeepHwOps =
    {
        .beep_hw_init 	= Beep_GpioInit,
        .beep_on      	= Beep_GpioOn,
        .beep_off     	= Beep_GpioOff,
        .beep_toggle  	= Beep_GpioToggle
    };

    Beep_LL_Init(BeepHwOps, TRUE, &g_sBeepMag);

}

MOTOR_MAG 			g_sMotorMag;

/****************************************************************
@FUNCTION������ģʽ�������ƹ��ܳ�ʼ��
@INPUT��MOTOR����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern void Motor_Init(MOTOR_MAG *pMag)
{
    MOTOR_HW_OPERATIONS MotorHwOps =
    {
        .motor_hw_init 	= Motor_GpioInit,
        .motor_on      	= Motor_GpioOn,
        .motor_off     	= Motor_GpioOff
    };

    Motor_LL_Init(MotorHwOps, &g_sMotorMag);
}


FUME_MAG    g_sFumeMag;

/****************************************************************
@FUNCTION������ģʽ�·��̹޿��ƹ��ܳ�ʼ��
@INPUT��FUME����ṹ��ָ��
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
extern void Fume_Init(FUME_MAG *pMag)
{
    FUME_HW_OPERATIONS FumeHwOps =
    {
        .hw_init      	= Fume_GpioInit,
        .on           	= Fume_GpioOn,
        .off          	= Fume_GpioOff,
        .is_installed 	= Fume_GpioIsInstalled
    };

    Fume_LL_Init(FumeHwOps, &g_sFumeMag);

}



void Lte_HardWareInit(void)
{
    UART_CONFIG Config = {0};
    UART_MANAGER *pUartMag = &g_sUartSetMag.Com[COM2];

    LteCom_GpioPowerOnInit();

    //��Ӧ��������
    memcpy((void *)&Config, (void *)&g_scUartDefConfig[COM2], sizeof(UART_CONFIG));
    Config.m_dwBaudRate = 115200;
//	Config.TxMode = MODE_INTERRUPT;
//	Config.RxMode = MODE_INTERRUPT;

    Uart_ComConfiguration(Config, pUartMag);
    Uart_ComInit(pUartMag);
}

uint32_t Lte_Write(uint8_t *pBuf, uint16_t Len)
{
    #ifdef LTE_LOG_OUTPUT
    LOG("LTE_4G_HUAWEI(%d)->", Len);
    printf_with_data(pBuf, Len);
    //printf_with_data(pUartMag->m_byRecvBuf,pUartMag->m_wRecvLen);
    #endif


    return Uart_SendBuf(&g_sUartSetMag.Com[COM2], pBuf, Len);

}

uint32_t Lte_Read(uint8_t *pBuf, uint16_t *pLen)
{
    UART_MANAGER *pUartMag = &g_sUartSetMag.Com[COM2];

    /**/
    if(TRUE == pUartMag->m_bIsComRecv)
    {
        if(pUartMag->m_wRecvLen == 0)
        {
            //pUartMag->m_bIsComRecv = FALSE;
            return FALSE;
        }

        #ifdef LTE_LOG_OUTPUT
        LOG("LTE_4G_HUAWEI(%d)<-", pUartMag->m_wRecvLen, pUartMag->m_byRecvBuf);
        printf_with_data(pUartMag->m_byRecvBuf, pUartMag->m_wRecvLen);
        #endif

        if(*pLen >= pUartMag->m_wRecvLen)
        {
            memcpy((void *)pBuf, (void *)pUartMag->m_byRecvBuf, pUartMag->m_wRecvLen);
            *pLen = pUartMag->m_wRecvLen;
        }
        else
        {
            memcpy((void *)pBuf, (void *)pUartMag->m_byRecvBuf, *pLen);
        }

        pUartMag->m_wRecvLen = 0;
        pUartMag->m_bIsComRecv = FALSE;

        return TRUE;
    }

    return FALSE;
}



USER_PARAM g_sUserParam;


/****************************************************************
@FUNCTION���ն�����ʱ��������Ĭ��ֵ
@INPUT����
@OUTPUT��g_sUserParam�ṹ�帳ֵ
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void UserParam_SetDefaultValue(USER_PARAM *pParam)
{
    if(pParam != NULL)
    {
        pParam->m_byBlood = 100;
        pParam->m_eBRChar = BRCHAR_YELLOW;
        pParam->m_eCIdegree = CI_SOLDIER_N13;
        pParam->m_wSlaverStaGetInt = 5000;
    }
}

/****************************************************************
@FUNCTION���ն�����ʱ������ʼ��
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void UserParam_Init(void)
{
    USER_PARAM ParamTmp;
    uint32_t i;
    SINGLE_SLAVER_PARAM *pSlaverMag;

    IFlash_Read(&g_sIFlashMag, FLASH_USER_ADDR, (uint8_t *)&ParamTmp, sizeof(USER_PARAM));
	
    if(ParamTmp.m_dwChk != GetIFlashCheckSum_U32((uint8_t *)&ParamTmp, sizeof(USER_PARAM) - 4))
    {
        memset((void *)&ParamTmp, 0, sizeof(USER_PARAM));
        UserParam_SetDefaultValue(&ParamTmp);
        ParamTmp.m_dwChk = GetIFlashCheckSum_U32((uint8_t *)&ParamTmp, sizeof(USER_PARAM) - 4);
        IFlash_Write(&g_sIFlashMag, FLASH_USER_ADDR, (uint8_t *)&ParamTmp, sizeof(USER_PARAM));
        memcpy((void *)&g_sUserParam, (void *)&ParamTmp, sizeof(USER_PARAM) - 4);
    }
    else
    {
				if(ParamTmp.m_byMode == GAME_GOING) //����ѵ��ģʽ
				{
						memcpy((void *)&g_sUserParam,(void *)&ParamTmp,sizeof(USER_PARAM));

						//��ϰģʽ������,�͵����Զ��ػ�,����Ӧ�ñ���ԭ��״̬
						if(g_sUserParam.m_byEnableChangeBat != TRUE)
						{
								g_sUserParam.m_IsIllegalPowerOff = TRUE;
						}
				}
				else
				{
						//�ָ�����
						memcpy((void *)&g_sUserParam, (void *)&ParamTmp, sizeof(USER_PARAM) - 4);
				}
				Wtn6_Play(WTN6_SystemStart);
				DrillStateRecovery();
		}
}

/****************************************************************
@FUNCTION���ն�����ʱ��������
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: �����ϳ�ʱ��
****************************************************************/
void UserParam_Save(void)
{
    g_sUserParam.m_dwChk = GetIFlashCheckSum_U32((uint8_t *)&g_sUserParam, sizeof(USER_PARAM) - 4);
    IFlash_Write(&g_sIFlashMag, FLASH_USER_ADDR, (uint8_t *)&g_sUserParam, sizeof(USER_PARAM));
}

/****************************************************************
@FUNCTION����ȡ�û������е����豸��
@INPUT����
@OUTPUT����
@RETURN:  ���豸��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint16_t UserParam_GetSlaverCnt(void)
{
    return g_sUserParam.m_wSingleCnt;
}


/****************************************************************
@FUNCTION�������û������е����豸��
@INPUT����ֵ
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void UserParam_SetSlaverCnt(uint8_t Cnt)
{
    if(Cnt <= SYS_SLAVER_CNT_MAX)
    {
        g_sUserParam.m_wSingleCnt = Cnt;
    }
}

/****************************************************************
@FUNCTION����ȡ�û������е����豸״̬��ȡ���
@INPUT����
@OUTPUT����
@RETURN:  ���豸��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint16_t UserParam_GetSlaverStatusInterval(void)
{
    return g_sUserParam.m_wSlaverStaGetInt;
}


/****************************************************************
@FUNCTION�������û������е����豸״̬��ȡ���
@INPUT�����
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void UserParam_SetSlaverStatusInterval(uint16_t Val)
{
    g_sUserParam.m_wSlaverStaGetInt = Val;
}

/****************************************************************
@FUNCTION����ȡ�û������еĹ���ṹ��
@INPUT����
@OUTPUT����
@RETURN: �������
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
SINGLE_SLAVER_MAG *UserParam_GetSlaverMag(uint8_t idx)
{
    if(idx < SYS_SLAVER_CNT_MAX)
    {
        return &g_sUserParam.m_saSingleMag[idx];
    }

    return NULL;
}

/****************************************************************
@FUNCTION�������û������е����豸����ṹ��
@INPUT�����豸����ṹ�壬���豸���
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void UserParam_SetSlaverMag(SINGLE_SLAVER_MAG * pVal, uint8_t idx)
{
    if(pVal != NULL && idx < SYS_SLAVER_CNT_MAX)
    {
        memcpy((void *)UserParam_GetSlaverMag(idx), pVal, sizeof(SINGLE_SLAVER_MAG));
    }
}

uint8_t UserParam_SetDayOrNight(uint8_t IsNight)
{
		g_sUserParam.m_IsNight = IsNight;
}

uint8_t UserParam_GetDayOrNight(void)
{
		return g_sUserParam.m_IsNight;
}

/****************************************************************
@FUNCTION����ȡ��ǰѪ��
@INPUT����
@OUTPUT����
@RETURN: Ѫ��
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint8_t UserParam_GetBlood(void)
{
    return g_sUserParam.m_byBlood;
}

/****************************************************************
@FUNCTION�����õ�ǰѪ��
@INPUT����ֵ
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetBlood(uint8_t Val)
{
    if(Val <= 100)
    {
        g_sUserParam.m_byBlood = Val;
    }
}

/****************************************************************
@FUNCTION����ȡ��ǰ��Կ
@INPUT����
@OUTPUT����
@RETURN: ��Կ
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint8_t UserParam_GetKey(void)
{
    return g_sUserParam.m_byKey;
}

/****************************************************************
@FUNCTION�����õ�ǰ��Կ
@INPUT����ֵ
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetKey(uint8_t Val)
{
    g_sUserParam.m_byKey = Val;
}

/****************************************************************
@FUNCTION����ȡ��ǰ����̽ͷ��
@INPUT����
@OUTPUT����
@RETURN: ����̽ͷ��
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint8_t UserParam_GetActLaserCnt(void)
{
    return g_sUserParam.m_byActLaserCnt;
}

/****************************************************************
@FUNCTION�����õ�ǰ����̽ͷ��
@INPUT����ֵ
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetActLaserCnt(uint8_t Val)
{
    g_sUserParam.m_byActLaserCnt = Val;
}



/****************************************************************
@FUNCTION����ȡ��ǰ��ݱ�ʶ
@INPUT����
@OUTPUT����
@RETURN: ��ݱ�ʶ
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
ENUM_BRCHAR UserParam_GetBRChar(void)
{
    return g_sUserParam.m_eBRChar;
}

/****************************************************************
@FUNCTION�����õ�ǰ��ݱ�ʶ
@INPUT����ֵ
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetBRChar(ENUM_BRCHAR eVal)
{
    g_sUserParam.m_eBRChar = eVal;
		g_sUserParam.m_sTsChar1.m_bIdentity = eVal;
}

/****************************************************************
@FUNCTION����ȡ�ۺ�����
@INPUT����
@OUTPUT����
@RETURN: �ۺ�����
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
ENUM_CI_INDIVIDUAL UserParam_GetCIdegree(void)
{
    return g_sUserParam.m_eCIdegree;
}

/****************************************************************
@FUNCTION�������ۺ�����
@INPUT����ֵ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetCIdegree(ENUM_CI_INDIVIDUAL eCI)
{
    g_sUserParam.m_eCIdegree = eCI;
}

/****************************************************************
@FUNCTION����ȡʵ��״̬��1
@INPUT����
@OUTPUT����
@RETURN: ʵ��״̬��1
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
STRUCT_TS_Character1 UserParam_GetTsChar1(void)
{
    return g_sUserParam.m_sTsChar1;
}

/****************************************************************
@FUNCTION������ʵ��״̬��1
@INPUT����ֵ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetTsChar1(STRUCT_TS_Character1 Val)
{
    g_sUserParam.m_sTsChar1 = Val;
}

/****************************************************************
@FUNCTION����ȡʵ��״̬��2
@INPUT����
@OUTPUT����
@RETURN: ʵ��״̬��2
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
STRUCT_TS_Character2 UserParam_GetTsChar2(void)
{
    return g_sUserParam.m_sTsChar2;
}

/****************************************************************
@FUNCTION������ʵ��״̬��2
@INPUT����ֵ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetTsChar2(STRUCT_TS_Character2 Val)
{
    g_sUserParam.m_sTsChar2 = Val;
}

/****************************************************************
@FUNCTION����ȡ����ģ��Mac��ַ
@INPUT����
@OUTPUT����
@RETURN: Zigbeeģ��Mac��ַ
@AUTHOR��xfw
@SPECIAL: 
****************************************************************/
uint8_t *UserParam_GetWMac(void)
{
    return g_sUserParam.m_aWMac;
}

/****************************************************************
@FUNCTION����������ģ��Mac��ַ
@INPUT�� ����ֵ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��Ӱ��ʵ��MAC
****************************************************************/
void UserParam_SetWMac(uint8_t * pVal, uint16_t ValLen)
{

    memcpy((void *)g_sUserParam.m_aWMac, pVal, MIN(ValLen,8));
  
}

/****************************************************************
@FUNCTION����ȡ��ǰ����ģ�鷢�书��
@INPUT����
@OUTPUT����
@RETURN: ��Կ
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint8_t UserParam_GetWTxPwr(void)
{
    return g_sUserParam.m_byWTxPwr;
}

/****************************************************************
@FUNCTION�����õ�ǰ����ģ�鷢�书��
@INPUT����ֵ
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL: ֻ��ֵ���ݣ����漰Ӳ������
****************************************************************/
void UserParam_SetWTxPwr(uint8_t Val)
{
    g_sUserParam.m_byWTxPwr = Val;
}

/****************************************************************
@FUNCTION����ȡ��γ������
@INPUT����
@OUTPUT����
@RETURN: ��γ������
@AUTHOR��xfw
@SPECIAL: 
****************************************************************/
STRUCT_LPOS *UserParam_GetLPos(void)
{
    return &g_sUserParam.m_sLPosition;
}

/****************************************************************
@FUNCTION�����þ�γ������
@INPUT�� ����ֵ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: 
****************************************************************/
void UserParam_SetLPos(STRUCT_LPOS * pVal)
{
    if(NULL != pVal)
    {
        memcpy((void *)&g_sUserParam.m_sLPosition, pVal, sizeof(STRUCT_LPOS));
    }
}

/****************************************************************
@FUNCTION����ȡUTCʱ��
@INPUT����
@OUTPUT����
@RETURN: UTCʱ��
@AUTHOR��xfw
@SPECIAL: 
****************************************************************/
STRUCT_TIME1 *UserParam_GetTime1(void)
{
    return &g_sUserParam.m_sUtcTime;
}

/****************************************************************
@FUNCTION������UTCʱ��
@INPUT�� ����ֵ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: 
****************************************************************/
void UserParam_SetTime1(STRUCT_TIME1 * pVal)
{
    if(NULL != pVal)
    {
        memcpy((void *)&g_sUserParam.m_sUtcTime, pVal, sizeof(STRUCT_TIME1));
    }
}

/****************************************************************
@FUNCTION����ȡ��λ״̬
@INPUT����
@OUTPUT����
@RETURN: ��Կ
@AUTHOR��xfw
@SPECIAL: 
****************************************************************/
uint8_t UserParam_GetPosType(void)
{
    return g_sUserParam.m_byPosType;
}

/****************************************************************
@FUNCTION�����ö�λ״̬
@INPUT����ֵ
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL: 
****************************************************************/
void UserParam_SetPosType(uint8_t Val)
{
    g_sUserParam.m_byPosType = Val;
}

/****************************************************************
@FUNCTION����ȡ��������
@INPUT����
@OUTPUT����
@RETURN: ��Կ
@AUTHOR��xfw
@SPECIAL: 
****************************************************************/
uint8_t UserParam_GetSatNum(void)
{
    return g_sUserParam.m_bySatNum;
}

/****************************************************************
@FUNCTION��������������
@INPUT����ֵ
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetSatNum(uint8_t Val)
{
    g_sUserParam.m_bySatNum = Val;
}

uint8_t UserParam_GetWeaponNum(void)
{
		uint8_t Last_WeaponType = 0x5A, Last_WeaponSubType = 0xA5;
		SINGLE_SLAVER_MAG *pMag = NULL;
	
		g_sUserParam.m_bySelfWeaponNum = 0;
		for(uint8_t i = 0; i < UserParam_GetSlaverCnt(); i++)
		{
				pMag = UserParam_GetSlaverMag(i);
			
				if(pMag->m_byTType == 0xFE)
					continue;
				
				if(Last_WeaponType == pMag->m_byTType && Last_WeaponSubType == pMag->m_byTSubType)
					continue;

				if((pMag->m_byTType == 0x00 && pMag->m_byTSubType == 0x00) || (pMag->m_byTType == 0xFF && pMag->m_byTSubType == 0xFF))
					break;
				
				Last_WeaponType = pMag->m_byTType;
				Last_WeaponSubType = pMag->m_byTSubType;
				g_sUserParam.m_bySelfWeaponNum++;
		}
		return g_sUserParam.m_bySelfWeaponNum;
}
/****************************************************************
@FUNCTION����ȡ��Ʒ��װ��־
@INPUT����
@OUTPUT����
@RETURN: ��Կ
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint8_t UserParam_GetFireInstallFlg(void)
{
    return g_sUserParam.m_bFireInstallFlg;
}

/****************************************************************
@FUNCTION�����û�Ʒ��װ��־
@INPUT����ֵ
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL: 
****************************************************************/
void UserParam_SetFireInstallFlg(uint8_t Val)
{
    g_sUserParam.m_bFireInstallFlg = Val;
}

/****************************************************************
@FUNCTION����ȡ�������ʵ������̽ͷ��(��Ե�����Ϊͷ��)
@INPUT����
@OUTPUT����
@RETURN: ��Կ
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint8_t UserParam_GetActLaserCnt1(void)
{
    return g_sUserParam.m_byActLaserCnt1;
}

/****************************************************************
@FUNCTION�����÷������ʵ������̽ͷ��(��Ե�����Ϊͷ��)
@INPUT����ֵ
@OUTPUT����
@RETURN:  ��
@AUTHOR��xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetActLaserCnt1(uint8_t Val)
{
    g_sUserParam.m_byActLaserCnt1 = Val;
}

//��ȡ���豸�Ƿ�����
uint8_t GetAllSlavesConnectStatus(void)
{
		SINGLE_SLAVER_MAG *pMag = NULL;
	
    for(uint8_t i = 0; i < UserParam_GetSlaverCnt(); i++)
    {
        pMag = UserParam_GetSlaverMag(i);

				if(pMag->m_byIsOnline == TRUE)
					continue;
				else
					return FALSE;
    }

    return TRUE;
}


/****************************************************************
@FUNCTION������ģʽ����
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��
****************************************************************/
void TesModeHandle(void)
{
    #if (BOARDID == 1)
    //����ģʽ
    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 1);				//�����
    Beep_On(&g_sBeepMag, OPT_MODE_NONBLOCK, 400);				//��������
    Led_SetRunMode(LED_MODE_BLINK_1, &g_sRunledMag);			//����
    Led_SetRunMode(LED_MODE_BLINK_2, &g_sStaledMag);			//����
    LaserCom_Prot_CtrlMode(&g_sLaserComTask_Mag, LASER_MODE_TEST);		//̽ͷ���������ģʽ
    Fume_On(&g_sFumeMag, 3);									//����ñ����v

    Wtn6_Play(WTN6_YX_ZDHX1);

    #elif  (BOARDID == 2)
    //����ģʽ
    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 1);				//�����
    Beep_On(&g_sBeepMag, OPT_MODE_NONBLOCK, 400);				//��������
    Led_SetRunMode(LED_MODE_BLINK_1, &g_sRunledMag);			//����
    Led_SetRunMode(LED_MODE_BLINK_2, &g_sStaledMag);			//����
    LaserCom_Prot_CtrlMode(LaserComTask_HandleGet(), LASER_MODE_TEST);		//̽ͷ���������ģʽ
    //Fume_On(&g_sFumeMag, 3);									//����ñ����v

    Wtn6_Play(WTN6_YX_ZDHX1);

    #endif
}

/****************************************************************
@FUNCTION����ȡ�弶�Զ���Ӳ��״̬1
@INPUT����
@OUTPUT����
@RETURN:  �Զ���Ӳ��״̬1
@AUTHOR��xfw
@SPECIAL: ��
****************************************************************/
uint8_t GetHardSta1(void)
{
    uint8_t HardSta = 0;
	
    #if (BOARDID == 1)
//    HardSta |= (ComTask_IsComOK(&g_sGpsComTask_Mag.m_sTaskMag) == FALSE ? 0x1 << IDX_HARD_GPS : 0);
//    HardSta |= (ComTask_IsComOK(LaserComTask_TaskHandleGet()) == FALSE ? 0x1 << IDX_HARD_LASER : 0);
//    HardSta |= (ComTask_IsComOK(ZigComTask_TaskHandleGet()) == FALSE ? 0x1 << IDX_HARD_ZIGBEE : 0);
    
    HardSta |= (&g_sGpsComTask_Mag.m_sTaskMag ->m_dwLastCommTime == FALSE ? 0x1 << IDX_HARD_GPS : 0);
    HardSta |= (LaserComTask_TaskHandleGet() ->m_dwLastCommTime == FALSE ? 0x1 << IDX_HARD_LASER : 0);
    HardSta |= (CZigComTask_TaskHandleGet() ->m_dwLastCommTime == FALSE ? 0x1 << IDX_HARD_ZIGBEE : 0);
//    HardSta |=  0x1 << IDX_HARD_BLE;
    #elif(BOARDID == 2)
//    HardSta |= (ComTask_IsComOK(LaserComTask_TaskHandleGet()) == FALSE ? 0x1 << IDX_HARD_LASER : 0);
//    HardSta |= (SComTask_IsComOK(ZigComTask_TaskHandleGet()) == FALSE ? 0x1 << IDX_HARD_ZIGBEE : 0);
//    HardSta |= (ComTask_IsComOK(ResComTask_TaskHandleGet()) == FALSE ? 0x1 << IDX_HARD_UPCOM : 0);
    //HardSta |= (ResComTask_TaskHandleGet()->m_dwLastCommTime == FALSE ? 0x1 << IDX_HARD_GPS : 0);
		HardSta |= (g_sUserParam.m_sLPosition.m_byLposState == FALSE ? 0x1 << IDX_HARD_GPS : 0);
		HardSta |= (LaserComTask_TaskHandleGet()->m_dwLastCommTime == FALSE ? 0x1 << IDX_HARD_LASER : 0);
    HardSta |= (ZigComTask_TaskHandleGet()->m_dwLastCommTime == FALSE ? 0x1 << IDX_HARD_ZIGBEE : 0);
    //HardSta |= (ResComTask_TaskHandleGet()->m_dwLastCommTime == FALSE ? 0x1 << IDX_HARD_UPCOM : 0);
		HardSta |= (g_sUserParam.m_bFireInstallFlg == FALSE ? 0x1 << IDX_HARD_YWG: 0);

		#endif
    return HardSta;
}

/****************************************************************
@FUNCTION����ȡ�弶�Զ���Ӳ��״̬2
@INPUT����
@OUTPUT����
@RETURN:  �Զ���Ӳ��״̬2
@AUTHOR��xfw
@SPECIAL: ��
****************************************************************/
uint8_t GetHardSta2(void)
{
    uint8_t HardSta = 0;
    HMICOMTASK_MAG *pMag = HmiComTask_HandleGet();
    
    #if (BOARDID == 1)

    #elif  (BOARDID == 2)
    HardSta |= (pMag->m_bIsOnline == FALSE ? 0x1 << (IDX_HARD_HMI - 0x08) : 0);  
    
    #endif
    return HardSta;
}

/****************************************************************
@FUNCTION����ȡ�弶�Զ���Ӳ��״̬3
@INPUT����
@OUTPUT����
@RETURN:  �Զ���Ӳ��״̬3
@AUTHOR��xfw
@SPECIAL: ��
****************************************************************/
uint8_t GetHardSta3(void)
{
    uint8_t HardSta = 0;
    MPU6050_MAG *pMpu = m_byg_sMpu6050Mag();
    #if (BOARDID == 1)

    #elif (BOARDID == 2)
    HardSta |= (pMpu->Flag == FALSE ? 0x1 << (IDX_HARD_MPU6050 - 0x10) : 0);
    #endif
    return HardSta;
}

/****************************************************************
@FUNCTION����ȡ�弶�Զ���Ӳ��״̬4
@INPUT����
@OUTPUT����
@RETURN:  �Զ���Ӳ��״̬4
@AUTHOR��xfw
@SPECIAL: ��
****************************************************************/
uint8_t GetHardSta4(void)
{
    uint8_t HardSta = 0;
    #if (BOARDID == 1)

    #elif  (BOARDID == 2)

    #endif
    return HardSta;
}

/****************************************************************
@FUNCTION����ȡ��̨�ź�ǿ��
@INPUT����
@OUTPUT����
@RETURN:  �ź�ǿ�ȣ�0-4��
@AUTHOR��xfw
@SPECIAL: ��
****************************************************************/
uint8_t GetDTRssi(void)
{
    uint8_t Rssi = 0;
    #if (BOARDID == 1)

    #elif  (BOARDID == 2)

    #endif
    return Rssi;
}


/****************************************************************
@FUNCTION���ڲ�������ͨͨ����������
@INPUT��pChanParam - ��ز���
@OUTPUT��
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ֡IDȡֵ����:IR38ͨ��-������<<24; ZIGͨ��-(������ << 24) | Ŀ���ַǰ���ֽ�
****************************************************************/
uint32_t InterChanSend(S_CONN_CHAN_PARAM *pChanParam)
{
    uint32_t Ret,ID,NowTime = GetSysRunMilliSecond();
    IR38COMTASK_MAG *pMagI = IR38ComTask_HandleGet();
    ZIGCOMTASK_MAG *pMagZ = ZigComTask_HandleGet();
    FRAME_BUF *pFrame = NULL;
    
    if(NULL == pChanParam || NULL == pChanParam->m_pDatBuf)
    {
        return FALSE;
    }

    pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

    if(pFrame == NULL)
    {
        return FALSE;
    }
    
    memset((void *)pFrame, 0, sizeof(FRAME_BUF));
    
    switch(pChanParam->m_eChan)
    {
        case CHAN_IR38:

            ID = (pChanParam->m_pDatBuf[1] << 24);
            pFrame->m_dwID = ID;    
            pFrame->m_wBufLen = MIN(pChanParam->m_wDatBufLen,FRAME_BUF_LEN_MAX);
            memcpy(pFrame->m_byBuf,pChanParam->m_pDatBuf,pFrame->m_wBufLen);
            LinkQueue_Append(pMagI->m_sTaskMag.m_pSendQueue, (void*)pFrame);
            Ret = TRUE;
            break;

        default:
            //�����ǹ㲥��ַ
            if(NULL == pChanParam->m_pDstAddr || pChanParam->m_byDstAddrLen <= 2)
            {
                Ret = FALSE;
                break;
            }
						/*
						if(pChanParam->m_byDstAddrLen == 2)
						{
								if(pChanParam->m_pDstAddr[0] == 0xFF && pChanParam->m_pDstAddr[1] == 0xFF)
								{
										memcpy(pFrame->m_byBuf, pChanParam->m_pDatBuf, pChanParam->m_wDatBufLen);
										
										pFrame->m_wBufLen = pChanParam->m_wDatBufLen;
										pFrame->m_dwID = (pChanParam->m_pDatBuf[7] << 24) | pChanParam->m_pDatBuf[8] << 16 | 0xFF << 8 | 0xFF;	
										pFrame->m_dwCnt = pChanParam->m_dwRepeatCnt;
										pFrame->m_dwScheTime = pChanParam->m_dwScheTime;
										pFrame->m_dwTime = NowTime;
										
										Ret = (SComTask_SendListAppendDiff(pMagZ->m_sTaskMag.m_pSendList, (void *)pFrame)<0) ? FALSE:TRUE ;
										break;
								}
								else
								{
										Ret = FALSE;
										break;
								}
						}
            */
            pFrame->m_wBufLen = sizeof(pFrame->m_byBuf);
            if(NewZigbee_MakeSendFrame(pChanParam->m_pDstAddr, pChanParam->m_byDstAddrLen,
                pChanParam->m_pDatBuf, pChanParam->m_wDatBufLen, pFrame->m_byBuf, &pFrame->m_wBufLen))
            {
                ID = 0;    //ע���ֵ

                //�ڲ�Ӳ������ָ��涨��ذ�Ϊԭ�����أ���IDΪ��֡��Ŀ�ĵ�ַ���ۼӺ�
                if(0x55 == pChanParam->m_pDatBuf[0])
                {
                    ID = Protocal_MakeU32Sum(pChanParam->m_pDatBuf,pChanParam->m_wDatBufLen);
                    ID += Protocal_MakeU32Sum(pChanParam->m_pDstAddr,pChanParam->m_byDstAddrLen);
                }
                else
                {
                    ID |= (pChanParam->m_pDatBuf[1] << 24) ;
                    ID |= (pChanParam->m_pDstAddr[pChanParam->m_byDstAddrLen - 3] << 16);
                    ID |= (pChanParam->m_pDstAddr[pChanParam->m_byDstAddrLen - 2] << 8);
                    ID |= (pChanParam->m_pDstAddr[pChanParam->m_byDstAddrLen - 1] << 0);
                }
					
                pFrame->m_dwID = ID;
                pFrame->m_dwCnt = pChanParam->m_dwRepeatCnt;
                pFrame->m_dwScheTime = pChanParam->m_dwScheTime;
								pFrame->m_dwTime = NowTime;
                Ret = (SComTask_SendListAppendDiff(pMagZ->m_sTaskMag.m_pSendList, (void *)pFrame)<0) ? FALSE:TRUE ;

            }
            else
            {
                Ret = FALSE;  
            }
            break;
						
    }

     if(FALSE == Ret)
    {
        sdram_free(0, (void *)pFrame);//��ֹ�ڴ�й©
    }
    return Ret; 
}

typedef struct
{
    uint8_t m_byDevldx;
    uint8_t m_byDevCtrl;
    uint8_t *m_pBuf;
    uint16_t m_wBufLen;
}STRUCT_SLAVER_CTRL;


/****************************************************************
@FUNCTION�����豸Ӳ������
@INPUT��pDstAddr - ���豸��ַ��DstAddrLen-���豸��ַ��pCtrlDat - Э�����ݣ�CtrlDatLen - ���ݳ���
@OUTPUT��
@RETURN: TRUE - �ɹ�/ FALSE - ʧ��
@AUTHOR��xfw
@SPECIAL: 
****************************************************************/
uint32_t  InterConn_SlaverHardCtrl(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pCtrlDat,uint16_t CtrlDatLen)
{
    uint8_t DatBuf[110];
    uint16_t DatBufLen;

    S_CONN_CHAN_PARAM ChanParam;

    if(NULL == pCtrlDat || CtrlDatLen > 100)
    {
        return FALSE;
    }

    //�����غ�
    DatBufLen = sizeof(DatBuf);
    if(TRUE == Protocal_ProtocolMakeShort55AA(pCtrlDat,CtrlDatLen,DatBuf,&DatBufLen))
    {
        ChanParam.m_eChan = eChan;
        ChanParam.m_pDstAddr = pDstAddr;
        ChanParam.m_byDstAddrLen = DstAddrLen;
        ChanParam.m_pDatBuf = DatBuf;
        ChanParam.m_wDatBufLen = DatBufLen;
        ChanParam.m_dwRepeatCnt = 5;  //
        ChanParam.m_dwScheTime = 200;  //��ʱ5s
       
        return InterChanSend(&ChanParam);
    }

    return FALSE;
}

/****************************************************************
@FUNCTION�����豸����
@INPUT��pDstAddr - ���豸��ַ��DstAddrLen-���豸��ַ��pHandle - �������
@OUTPUT��
@RETURN: TRUE - �ɹ�/ FALSE - ʧ��
@AUTHOR��xfw
@SPECIAL: ע���֡���Ǳ�ɾ��������һֱ�ᷢ��
****************************************************************/
uint32_t  InterConn_SlaverConnect(ENUM_CONN_CHAN eChan,SINGLE_SLAVER_MAG *pSingleMag)
{
    uint8_t DatBuf[50];
    uint8_t DatBufLen;
    STRUCT_TS_Character2 Char2;
    S_CONN_CHAN_PARAM ChanParam;

    if(NULL == pSingleMag)
    {
        return FALSE;
    }
    
		//if(pSingleMag->m_byIsOnline == TRUE)
		//	return TRUE;
		
    //�����غ�
    DatBufLen = 0;
    DatBuf[DatBufLen++] = 0xA2;
    DatBuf[DatBufLen++] = 0xA5;
    DatBuf[DatBufLen++] = 0;
    DatBuf[DatBufLen++] = pSingleMag->m_byTType;		//IDParam_GetTType(); 
    DatBuf[DatBufLen++] = pSingleMag->m_byTSubType;		//IDParam_GetTSubType();
    DatBuf[DatBufLen++] = IDParam_GetRid();
    DatBuf[DatBufLen++] = IDParam_GetPid() >>8 ;
    DatBuf[DatBufLen++] = IDParam_GetPid() &0xff;
    DatBuf[DatBufLen++] = UserParam_GetKey();
    Char2 = UserParam_GetTsChar2();
    memcpy((void *)&DatBuf[DatBufLen], (void *)&Char2, 1);
    DatBufLen += 1;

    DatBuf[2] = DatBufLen - 3;
    DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;
					
    ChanParam.m_eChan = eChan;
    ChanParam.m_pDstAddr = pSingleMag->m_aWMac;
    ChanParam.m_byDstAddrLen = 8;
    ChanParam.m_pDatBuf = DatBuf;
    ChanParam.m_wDatBufLen = DatBufLen;
    ChanParam.m_dwRepeatCnt = 0xFFFFFFFF;  //���޴��ط�
    ChanParam.m_dwScheTime = 500;  //��ʱ5s
    return InterChanSend(&ChanParam);
}

/****************************************************************
@FUNCTION�����豸״̬��ȡ
@INPUT��pDstAddr - ���豸��ַ��DstAddrLen-���豸��ַ��pHandle - �������
@OUTPUT��
@RETURN: TRUE - �ɹ�/ FALSE - ʧ��
@AUTHOR��xfw
@SPECIAL: 
****************************************************************/
uint32_t  InterConn_SlaverStaGet(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen)
{
    uint8_t DatBuf[50];
    uint8_t DatBufLen;
    S_CONN_CHAN_PARAM ChanParam;

    //�����غ�
    DatBufLen = 0;
    DatBuf[DatBufLen++] = 0xA2;
    DatBuf[DatBufLen++] = 0xA7;
    DatBuf[DatBufLen++] = 0;
    DatBuf[DatBufLen++] = 0xFF; 
    DatBuf[DatBufLen++] = 0xFF;
    DatBuf[DatBufLen++] = 0xFF;
    DatBuf[DatBufLen++] = 0xFF;
    DatBuf[DatBufLen++] = 0xFF;

    DatBuf[2] = DatBufLen - 3;
    DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;
    ChanParam.m_eChan = eChan;
    ChanParam.m_pDstAddr = pDstAddr;
    ChanParam.m_byDstAddrLen = DstAddrLen;
    ChanParam.m_pDatBuf = DatBuf;
    ChanParam.m_wDatBufLen = DatBufLen;
    ChanParam.m_dwRepeatCnt = 1;  
    ChanParam.m_dwScheTime = 100;  //��ʱ5s
   
    return InterChanSend(&ChanParam);
}

/****************************************************************
@FUNCTION�����豸״̬��ȡ
@INPUT��pDstAddr - ���豸��ַ��DstAddrLen-���豸��ַ��pHandle - �������
@OUTPUT��
@RETURN: TRUE - �ɹ�/ FALSE - ʧ��
@AUTHOR��xfw
@SPECIAL: 
****************************************************************/
uint32_t  InterConn_HelmetStaGet(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen)
{
    uint8_t DatBuf[50];
    uint8_t DatBufLen;
    S_CONN_CHAN_PARAM ChanParam;
    
    //�����غ�
    DatBufLen = 0;
    DatBuf[DatBufLen++] = 0xA2;
    DatBuf[DatBufLen++] = 0xC0;
    DatBuf[DatBufLen++] = 0;
    DatBuf[DatBufLen++] = 0xFF; 
    DatBuf[DatBufLen++] = 0xFF;
    DatBuf[DatBufLen++] = 0xFF;
    DatBuf[DatBufLen++] = 0xFF;
    DatBuf[DatBufLen++] = 0xFF;

    DatBuf[2] = DatBufLen - 3;
    DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;
	
    ChanParam.m_eChan = eChan;
    ChanParam.m_pDstAddr = pDstAddr;
    ChanParam.m_byDstAddrLen = DstAddrLen;
    ChanParam.m_pDatBuf = DatBuf;
    ChanParam.m_wDatBufLen = DatBufLen;
    ChanParam.m_dwRepeatCnt = 1;  
    ChanParam.m_dwScheTime = 100;  //��ʱ5s
   
    return InterChanSend(&ChanParam);
}


uint32_t InterConn_ProtDeal_MacGet(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
    uint8_t DatBuf[50];
    uint8_t DatBufLen;
    S_CONN_CHAN_PARAM ChanParam;

    uint8_t Offset, TType, TSubtype, RID;
    uint16_t PID;
    
    //�������У��
    if(pBuf == NULL || BufLen < 9 )
    {
        return FALSE;
    }

    Offset = 3;
    TType = pBuf[Offset++];
    TSubtype = pBuf[Offset++];
    RID = pBuf[Offset++];
    PID = (uint16_t)pBuf[Offset] << 8 | pBuf[Offset + 1];
    Offset += 2;

    //�Ƿ��Ƿ����Լ�
    if(TRUE != IDParam_IsIDValid(TType, TSubtype, RID, PID))
    {
        //return;
    }


    //����̾�ظ�
    DatBufLen = 0;
    DatBuf[DatBufLen++] = 0xA3;
    DatBuf[DatBufLen++]  = pBuf[1];
    DatBuf[DatBufLen++]  = 0;
    DatBuf[DatBufLen++]  = IDParam_GetTType();
    DatBuf[DatBufLen++]  = IDParam_GetTSubType();
    DatBuf[DatBufLen++]  = IDParam_GetRid();
    DatBuf[DatBufLen++]	 = IDParam_GetPid() >> 8;
    DatBuf[DatBufLen++]  = IDParam_GetPid() & 0xFF;
    DatBuf[DatBufLen++]  = IDParam_GetWType();
    memcpy(&DatBuf[DatBufLen], UserParam_GetWMac(), 8);
    DatBufLen += 8;

    DatBuf[2] = DatBufLen - 3;
    DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;
		
    ChanParam.m_eChan = eChan;
    ChanParam.m_pDstAddr = pDstAddr;
    ChanParam.m_byDstAddrLen = DstAddrLen;
    ChanParam.m_pDatBuf = DatBuf;
    ChanParam.m_wDatBufLen = DatBufLen;
    ChanParam.m_dwRepeatCnt = 0x1;  //ֻ��һ��
    ChanParam.m_dwScheTime = 100; 
    
    return InterChanSend(&ChanParam);
}


uint32_t InterConn_ProtDeal_InfoSend(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
		S_CONN_CHAN_PARAM ChanParam;
    SINGLE_SLAVER_MAG *pSlaverMag;
		uint8_t DatBuf[50];
    uint8_t DatBufLen;
    uint16_t i, Len, SlaverCnt;
	
		//�������У��
    if(pBuf == NULL || BufLen < 5 )
    {
        return FALSE;
    }
	
    DatBufLen = 0;
    DatBuf[DatBufLen++] = 0xA3;
    DatBuf[DatBufLen++] = 0xFF;
    DatBuf[DatBufLen++] = 0;
    DatBuf[DatBufLen++] = FIXVERSION;
    DatBuf[DatBufLen++] = BOARDID;
    DatBuf[DatBufLen++] = GetHardSta1();
    DatBuf[DatBufLen++] = GetHardSta2();
    DatBuf[DatBufLen++] = GetHardSta3();
    DatBuf[DatBufLen++] = GetHardSta4();
    DatBuf[DatBufLen++] = UserParam_GetActLaserCnt();
    // ��ص���������1000����
    DatBuf[DatBufLen++] = BatTask_GetExBatVol() >> 8;
    DatBuf[DatBufLen++] = BatTask_GetExBatVol() & 0xFF;
    // ����MAC��ַ
    memcpy(&DatBuf[DatBufLen],UserParam_GetWMac(),8);
    DatBufLen += 8;
    
    // ��̨�ź�ǿ�ȣ�0-4��
    DatBuf[DatBufLen++] = GetDTRssi();
    // �������豸����n(��Ϊ0�����޺����ֶ�)
    SlaverCnt = UserParam_GetSlaverCnt();
    DatBuf[DatBufLen++] = SlaverCnt;

    for(i = 0; i < MIN(SlaverCnt, SYS_SLAVER_CNT_MAX); i++)
    {
        pSlaverMag = UserParam_GetSlaverMag(i);
        DatBuf[DatBufLen++] = pSlaverMag->m_byTType;
        DatBuf[DatBufLen++] = pSlaverMag->m_byTSubType;
        DatBuf[DatBufLen++] = pSlaverMag->m_byWType;
        memcpy(&DatBuf[DatBufLen], pSlaverMag->m_aWMac, sizeof(pSlaverMag->m_aWMac));
        DatBufLen += sizeof(pSlaverMag->m_aWMac);
        memcpy(&DatBuf[DatBufLen], (uint8_t *)&pSlaverMag->m_sTsChar1, 1);
        DatBufLen += 1;
        DatBuf[DatBufLen++] = pSlaverMag->m_wBatVol >> 8;
        DatBuf[DatBufLen++] = pSlaverMag->m_wBatVol & 0xFF;
    }

    DatBuf[2] = DatBufLen - 3;
    DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;
		
    ChanParam.m_eChan = CHAN_IR38;
    ChanParam.m_pDstAddr = pDstAddr;
    ChanParam.m_byDstAddrLen = DstAddrLen;
    ChanParam.m_pDatBuf = DatBuf;
    ChanParam.m_wDatBufLen = DatBufLen;
    ChanParam.m_dwRepeatCnt = 0x1;  //ֻ��һ��
    ChanParam.m_dwScheTime = 100;  
    
    return InterChanSend(&ChanParam);

}

uint32_t InterConn_ProtDeal_IDSet(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
    uint8_t DatBuf[50];
    uint8_t DatBufLen;
    S_CONN_CHAN_PARAM ChanParam;

    uint8_t Offset, TType, TSubtype, RID, BrChar, Key, NeedSave;
    uint16_t PID;

    //�������У��
    if(pBuf == NULL || BufLen < 11 )
    {
        return FALSE;
    }

    Offset = 3;
    TType = pBuf[Offset++];
    TSubtype = pBuf[Offset++];
    RID = pBuf[Offset++];
    PID = (uint16_t)pBuf[Offset] << 8 | pBuf[Offset + 1];
    Offset += 2;
    BrChar = pBuf[Offset++];
    Key = pBuf[Offset++];

    NeedSave = FALSE;

    if(TRUE == IsParamValid((uint8_t *)&TType, 1))
    {
        IDParam_SetTType(TType);
        NeedSave = TRUE;
    }

    if(TRUE == IsParamValid((uint8_t *)&TSubtype, 1))
    {
        IDParam_SetTSubType(TSubtype);
        NeedSave = TRUE;
    }

    if(TRUE == IsParamValid((uint8_t *)&RID, 1))
    {
        IDParam_SetRid(RID);
        NeedSave = TRUE;
    }

    if(TRUE == IsParamValid((uint8_t *)&PID, 2))
    {
        IDParam_SetPid(PID);
        NeedSave = TRUE;
    }

    if(TRUE == IsParamValid((uint8_t *)&BrChar, 1))
    {
        UserParam_SetBRChar((ENUM_BRCHAR)BrChar);
        NeedSave = TRUE;
    }

    if(TRUE == IsParamValid((uint8_t *)&Key, 1))
    {
        UserParam_SetKey(Key);
        NeedSave = TRUE;
    }

    if(TRUE == NeedSave)
    {
        IWDG_Feed();
        IDParam_Save();
        IWDG_Feed();
        UserParam_Save();
        IWDG_Feed();
    }

    //����̾�ظ�
		return InterConn_ProtDeal_IR_SetOrRescue_ACK();
}

//A3 61
uint32_t InterConn_ProtDeal_IR_SetOrRescue_ACK(void)
{
		uint8_t DatBuf[50];
    uint8_t DatBufLen, Offset;
		S_CONN_CHAN_PARAM ChanParam;
	
		DatBufLen = 0;
    DatBuf[DatBufLen++] = 0xA3;
    DatBuf[DatBufLen++] = 0x61;
    DatBuf[DatBufLen++] = 0;
    DatBuf[DatBufLen++] = IDParam_GetTType();
    DatBuf[DatBufLen++] = IDParam_GetTSubType();
    DatBuf[DatBufLen++] = IDParam_GetRid();
    DatBuf[DatBufLen++] = IDParam_GetPid() >> 8;
    DatBuf[DatBufLen++] = IDParam_GetPid() & 0xFF;
		DatBuf[DatBufLen++]	= 0x01;
	
		DatBuf[2] = DatBufLen - 3;
    DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;

		ChanParam.m_eChan = CHAN_IR38;
    ChanParam.m_pDstAddr = NULL;
    ChanParam.m_byDstAddrLen = NULL;
    ChanParam.m_pDatBuf = DatBuf;
    ChanParam.m_wDatBufLen = DatBufLen;
    ChanParam.m_dwRepeatCnt = 0x1;  //ֻ��һ��
    ChanParam.m_dwScheTime = 100;  
		
    return InterChanSend(&ChanParam);
}

uint32_t InterConn_ProtDeal_SlaverClear(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
    uint8_t DatBuf[50];
    uint8_t DatBufLen;
    S_CONN_CHAN_PARAM ChanParam;

    uint8_t Offset, TType, TSubtype, RID, i;
    uint16_t PID;

    //�������У��
    if(pBuf == NULL || BufLen < 9 )
    {
        return FALSE;
    }

    Offset = 3;
    TType = pBuf[Offset++];
    TSubtype = pBuf[Offset++];
    RID = pBuf[Offset++];
    PID = (uint16_t)pBuf[Offset] << 8 | pBuf[Offset + 1];
    Offset += 2;

    //�Ƿ��Ƿ����Լ�
    if(TRUE != IDParam_IsIDValid(TType, TSubtype, RID, PID))
    {
        //return;
    }
		
		for(uint8_t i = 0; i < WEAPONKIND_MAX; i++)
		{
			memset(&g_sUserParam.m_saAmmuInfo[i], 0, sizeof(SOLDIERAMMU_T));
		}

    IWDG_Feed();
    SysParam_SetDefaultValue(NULL);
    SysParam_Save();
    IWDG_Feed();
		
		IWDG_Feed();
		UserParam_Save();
    IWDG_Feed();
		
		Wtn6_Play(WTN6_REMOVE_PAIR);
    //����̾�ظ�

    DatBufLen = 0;
    DatBuf[DatBufLen++] = 0xA3;
    DatBuf[DatBufLen++] = pBuf[1];
    DatBuf[DatBufLen++] = 0;
    DatBuf[DatBufLen++] = IDParam_GetTType();
    DatBuf[DatBufLen++] = IDParam_GetTSubType();
    DatBuf[DatBufLen++] = IDParam_GetRid();
    DatBuf[DatBufLen++] = IDParam_GetPid() >> 8;
    DatBuf[DatBufLen++] = IDParam_GetPid() & 0xFF;

    DatBuf[2] = DatBufLen - 3;
    DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;
		
    ChanParam.m_eChan = CHAN_IR38;
    ChanParam.m_pDstAddr = pDstAddr;
    ChanParam.m_byDstAddrLen = DstAddrLen;
    ChanParam.m_pDatBuf = DatBuf;
    ChanParam.m_wDatBufLen = DatBufLen;
    ChanParam.m_dwRepeatCnt = 0x1;  //ֻ��һ��
    ChanParam.m_dwScheTime = 100;  
    
    return InterChanSend(&ChanParam);
}

uint32_t InterConn_ProtDeal_IR_DB_Mac(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
		S_CONN_CHAN_PARAM ChanParam;
		
		uint8_t DatBuf[50];
    uint8_t DatBufLen, Offset;
		
		//�������У��
    if(pBuf == NULL || BufLen < 8)
    {
        return FALSE;
    }
		//����̾�ظ�

    DatBufLen = 0;
    DatBuf[DatBufLen++] = 0xA2;
    DatBuf[DatBufLen++] = 0x11;
    DatBuf[DatBufLen++] = 0;
    DatBuf[DatBufLen++] = g_sUserParam.m_aWMac[0];
    DatBuf[DatBufLen++] = g_sUserParam.m_aWMac[1];
    DatBuf[DatBufLen++] = g_sUserParam.m_aWMac[2];
    DatBuf[DatBufLen++] = g_sUserParam.m_aWMac[3];
    DatBuf[DatBufLen++] = g_sUserParam.m_aWMac[4];
		DatBuf[DatBufLen++] = g_sUserParam.m_aWMac[5];
		DatBuf[DatBufLen++] = IDParam_GetTType();;
		DatBuf[DatBufLen++] = IDParam_GetTSubType();
		
    DatBuf[2] = DatBufLen - 3;
    DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;
		
    ChanParam.m_eChan = CHAN_IR38;
    ChanParam.m_pDstAddr = pDstAddr;
    ChanParam.m_byDstAddrLen = DstAddrLen;
    ChanParam.m_pDatBuf = DatBuf;
    ChanParam.m_wDatBufLen = DatBufLen;
    ChanParam.m_dwRepeatCnt = 0x1;  //ֻ��һ��
    ChanParam.m_dwScheTime = 100;  
    
    return InterChanSend(&ChanParam);
}

//A2 51
uint32_t InterConn_ProtDeal_IR_DB_Injury_Inquire(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
		STRUCT_TS_Character1 Character1 = {0};
		S_CONN_CHAN_PARAM ChanParam;
		INJURY_MAG g_sInjury;
		
		uint8_t DatBuf[50];
    uint8_t DatBufLen, Offset;
		int8_t i;
		
		uint8_t TType, TSubtype, RID;
		uint16_t PID;
    uint32_t WriteAddr;
		
		//�������У��
    if(pBuf == NULL || BufLen < 5)
    {
        return FALSE;
    }
		
		Offset = 3;
		TType = pBuf[Offset++];
		TSubtype = pBuf[Offset++];
		RID = pBuf[Offset++];
		PID = pBuf[Offset] << 8 | pBuf[Offset + 1];
		Offset += 2;
		
		//����̾�ظ�

		if(UserParam_GetBlood() == 100 || g_sInjuryMag.m_byRcdCnt == 0)
			return FALSE;
		
    DatBufLen = 0;
    DatBuf[DatBufLen++] = 0xA3;
    DatBuf[DatBufLen++] = pBuf[1];
    DatBuf[DatBufLen++] = 0;
    DatBuf[DatBufLen++] = IDParam_GetTType();
    DatBuf[DatBufLen++] = IDParam_GetTSubType();
    DatBuf[DatBufLen++] = IDParam_GetRid();
    DatBuf[DatBufLen++] = IDParam_GetPid() >> 8;
    DatBuf[DatBufLen++] = IDParam_GetPid() & 0xFF;

		DatBuf[DatBufLen++] = UserParam_GetKey();
		
		//����̾�ظ�
		Character1.m_bLocked = g_sUserParam.m_sTsChar1.m_bLocked;
		Character1.m_bConnectionState = g_sUserParam.m_sTsChar1.m_bConnectionState;		//����������״̬0������	1���쳣
    Character1.m_bFailureState    = g_sUserParam.m_sTsChar1.m_bFailureState;		//0���޹���	1���й���
    Character1.m_bUnitmode		  	= g_sUserParam.m_sTsChar1.m_bUnitmode;		//0��ѵ��ģʽ	1����ϰģʽ
    Character1.m_bBatteryState	  = g_sUserParam.m_sTsChar1.m_bBatteryState;		//0����������	1��Ƿ��
    Character1.m_bIdentity     	  = g_sUserParam.m_sTsChar1.m_bIdentity;		//��� ENUM_BRCHAR
		Character1.m_bIsNight 				= g_sUserParam.m_sTsChar1.m_bIsNight;
		
		memcpy(&DatBuf[DatBufLen], &Character1, sizeof(STRUCT_TS_Character1));
    DatBufLen++;
		
		DatBuf[DatBufLen++] = GetInjuryDegree(g_sUserParam.m_byBlood);
		DatBuf[DatBufLen++] = g_sInjuryMag.m_byRcdCnt;

		for(i = g_sInjuryMag.m_byRcdCnt - 1; i >= 0; i--)
		{
				WriteAddr = W25X_PAGE_SIZE * i;
				IFlash_Read(&g_sIFlashMag, FLASH_BOOT_START + g_sIFlashMag.m_dwCapacity * 1024 - 2 * g_sIFlashMag.m_wPageSize + WriteAddr, (uint8_t *)&g_sInjury, sizeof(INJURY_MAG));
	
				DatBuf[DatBufLen++] = g_sInjury.m_byPart;
				DatBuf[DatBufLen++] = g_sInjury.m_byDamageKind;
				DatBuf[DatBufLen++] = GetInjuryDegree(g_sInjury.m_byResidueBlood);
		}
				
    DatBuf[2] = DatBufLen - 3;
    DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;
		
    ChanParam.m_eChan = CHAN_IR38;
    ChanParam.m_pDstAddr = pDstAddr;
    ChanParam.m_byDstAddrLen = DstAddrLen;
    ChanParam.m_pDatBuf = DatBuf;
    ChanParam.m_wDatBufLen = DatBufLen;
    ChanParam.m_dwRepeatCnt = 0x1;  //ֻ��һ��
    ChanParam.m_dwScheTime = 100;  
    
    return InterChanSend(&ChanParam);
}

//A2 52
uint32_t InterConn_ProtDeal_IR_DB_RescueResult_Inquire(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
		S_CONN_CHAN_PARAM ChanParam;
				
		RCVHURTINFO_T Info;

		STRUCT_TS_Character1 Character1 = {0};

		uint8_t DatBuf[50];
    uint8_t DatBufLen, Offset;

		uint8_t TType, TSubtype, RID;
		uint16_t PID;
	
		//�������У��
    if(pBuf == NULL || BufLen < 11)
    {
        return FALSE;
    }
		
		Offset = 3;
		TType = pBuf[Offset++];
		TSubtype = pBuf[Offset++];
		RID = pBuf[Offset++];
		PID = pBuf[Offset] << 8 | pBuf[Offset + 1];
		Offset += 2;

    if(g_sUserParam.m_byBlood == 0)
        return FALSE;
		
		g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_sRescueEndTime = g_sUserParam.m_sUtcTime;
		g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_byRT_Type = pBuf[3];
		g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_byRT_Subtype = pBuf[4];
		g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_byR_RID = pBuf[5];
		g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_aR_PID[0] = pBuf[6];
		g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_aR_PID[1] = pBuf[7];
		g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_byCM_Characte = pBuf[8];  //���η�ʽ
		g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_byCE_Characte = pBuf[9];  //���ν��

		//BSN_RescureEvtHandle(&g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued);  //�¼��ϴ�
		
		g_sUserParam.m_byAidEndFlag2 = TRUE;
		if(g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_byCE_Characte == 1)//���εõ����ظ�ս��
    {
        Wtn6_Play(WTN6_SAVE_SUCCESS);

        g_sUserParam.m_IsAid = TRUE;

        Info.m_A_Type    = 0x7A;
        Info.m_A_Subtype = 0x00;
        Info.m_byUniID   = 0;
        Info.m_HPart     = 0;
        Info.m_HType     = H_Type_I_BACKSTAGECTRL;
        Info.m_HurtBlood = 100;
        Info.m_RRID      = pBuf[5];
        Info.m_wRPID     = pBuf[6] << 8 | pBuf[7];
        Info.m_RT_Type   = 0x48;
        Info.m_RT_Subtype = 0x03;

        UpdateDamage(Info);
    }
    else if(g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_byCE_Characte == 2)	//���γɹ������ٶ�
    {
        Wtn6_Play(WTN6_SAVE_SUCCESS);
        g_sUserParam.m_IsAid = TRUE;
    }
    else if(g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_byCE_Characte == 3)	//����ʧ�ܣ������
    {
        Wtn6_Play(WTN6_SAVE_FALI);
        g_sUserParam.m_IsAid = FALSE;
    }
    else if(g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_byCE_Characte == 4)	//����ʧ�ܣ�����
    {
        Info.m_A_Type    = 0x7A;
        Info.m_A_Subtype = 0x00;
        Info.m_byUniID   = 0;
        Info.m_HPart     = 0;
        Info.m_HType     = H_Type_I_BACKSTAGECTRL;
        Info.m_HurtBlood = 0;
        Info.m_RRID      = pBuf[5];
        Info.m_wRPID     = pBuf[6] << 8 | pBuf[7];
        Info.m_RT_Type   = 0x48;
        Info.m_RT_Subtype = 0x03;

				Wtn6_Play(WTN6_SAVE_FALI);
				g_sUserParam.m_IsAid = FALSE;
			
        UpdateDamage(Info);
    }
		
		DatBufLen = 0;
    DatBuf[DatBufLen++] = 0xA3;
    DatBuf[DatBufLen++] = 0x61;
    DatBuf[DatBufLen++] = 0;
    DatBuf[DatBufLen++] = IDParam_GetTType();
    DatBuf[DatBufLen++] = IDParam_GetTSubType();
    DatBuf[DatBufLen++] = IDParam_GetRid();
    DatBuf[DatBufLen++] = IDParam_GetPid() >> 8;
    DatBuf[DatBufLen++] = IDParam_GetPid() & 0xFF;
		
		DatBuf[DatBufLen++] = 1;
		
		DatBuf[2] = DatBufLen - 3;
    DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;
		
    ChanParam.m_eChan = CHAN_IR38;
    ChanParam.m_pDstAddr = pDstAddr;
    ChanParam.m_byDstAddrLen = DstAddrLen;
    ChanParam.m_pDatBuf = DatBuf;
    ChanParam.m_wDatBufLen = DatBufLen;
    ChanParam.m_dwRepeatCnt = 0x1;  //ֻ��һ��
    ChanParam.m_dwScheTime = 100;  
    
    return InterChanSend(&ChanParam);
}

//A2 66
uint32_t InterConn_ProtDeal_IR_StateInfo_Inquire(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
		uint8_t DatBuf[50];
    uint8_t DatBufLen, Offset;
		S_CONN_CHAN_PARAM ChanParam;
	
		STRUCT_TS_Character1 Character1 = {0};

		uint8_t TType, TSubtype, RID;
		uint16_t PID;
	
		//�������У��
    if(pBuf == NULL || BufLen < 9)
    {
        return FALSE;
    }
		
		Offset = 3;
		TType = pBuf[Offset++];
		TSubtype = pBuf[Offset++];
		RID = pBuf[Offset++];
		PID = pBuf[Offset] << 8 | pBuf[Offset + 1];
		Offset += 2;
		
		Character1.m_bLocked = g_sUserParam.m_sTsChar1.m_bLocked;
		Character1.m_bConnectionState = g_sUserParam.m_sTsChar1.m_bConnectionState;		//����������״̬0������	1���쳣
    Character1.m_bFailureState    = g_sUserParam.m_sTsChar1.m_bFailureState;		//0���޹���	1���й���
    Character1.m_bUnitmode		  	= g_sUserParam.m_sTsChar1.m_bUnitmode;		//0��ѵ��ģʽ	1����ϰģʽ
    Character1.m_bBatteryState	  = g_sUserParam.m_sTsChar1.m_bBatteryState;		//0����������	1��Ƿ��
    Character1.m_bIdentity     	  = g_sUserParam.m_sTsChar1.m_bIdentity;		//��� ENUM_BRCHAR
		Character1.m_bIsNight 				= g_sUserParam.m_sTsChar1.m_bIsNight;
		
    DatBufLen = 0;
    DatBuf[DatBufLen++] = 0xA3;
    DatBuf[DatBufLen++] = pBuf[1];
    DatBuf[DatBufLen++] = 0;
    DatBuf[DatBufLen++] = IDParam_GetTType();
    DatBuf[DatBufLen++] = IDParam_GetTSubType();
    DatBuf[DatBufLen++] = IDParam_GetRid();
    DatBuf[DatBufLen++] = IDParam_GetPid() >> 8;
    DatBuf[DatBufLen++] = IDParam_GetPid() & 0xFF;
		
		memcpy(&DatBuf[DatBufLen], UserParam_GetLPos(), sizeof(STRUCT_LPOS));
		DatBufLen += sizeof(STRUCT_LPOS);
		
    memcpy(&DatBuf[DatBufLen], UserParam_GetTime1(), sizeof(STRUCT_TIME1));
    DatBufLen += sizeof(STRUCT_TIME1);
		
		DatBuf[DatBufLen++] = UserParam_GetCIdegree();
		
		memcpy(&DatBuf[DatBufLen], &g_sUserParam.m_sTsChar1, sizeof(STRUCT_TS_Character1));
    DatBufLen += sizeof(STRUCT_TS_Character1);
		
		memcpy(&DatBuf[DatBufLen], &g_sUserParam.m_sTsChar2, sizeof(STRUCT_TS_Character2));
    DatBufLen += sizeof(STRUCT_TS_Character2);
		
		memcpy(&DatBuf[DatBufLen], g_sUserParam.EP_Num, 3);
		DatBufLen += 3;
		
		memcpy(&DatBuf[DatBufLen], &g_sUserParam.m_sManoStart, sizeof(STRUCT_TIME1));
    DatBufLen += sizeof(STRUCT_TIME1);
		
		memcpy(&DatBuf[DatBufLen], &g_sUserParam.m_sManoEnd, sizeof(STRUCT_TIME1));
    DatBufLen += sizeof(STRUCT_TIME1);
		
		DatBuf[2] = DatBufLen - 3;
    DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;
		
    ChanParam.m_eChan = CHAN_IR38;
    ChanParam.m_pDstAddr = pDstAddr;
    ChanParam.m_byDstAddrLen = DstAddrLen;
    ChanParam.m_pDatBuf = DatBuf;
    ChanParam.m_wDatBufLen = DatBufLen;
    ChanParam.m_dwRepeatCnt = 0x1;  //ֻ��һ��
    ChanParam.m_dwScheTime = 100;  
    
    return InterChanSend(&ChanParam);
}

//A2 67
uint32_t InterConn_ProtDeal_IR_ID_Inquire(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
		uint8_t DatBuf[50];
    uint8_t DatBufLen, Offset;
		S_CONN_CHAN_PARAM ChanParam;
	
		STRUCT_TS_Character1 Character1 = {0};
		uint8_t TType, TSubtype, RID;
		uint16_t PID;
	
		//�������У��
    if(pBuf == NULL || BufLen < 9)
    {
        return FALSE;
    }
		
		Offset = 3;
		TType = pBuf[Offset++];
		TSubtype = pBuf[Offset++];
		RID = pBuf[Offset++];
		PID = pBuf[Offset] << 8 | pBuf[Offset + 1];
		Offset += 2;
	
		//����̾�ظ�
		Character1.m_bLocked = g_sUserParam.m_sTsChar1.m_bLocked;
		Character1.m_bConnectionState = g_sUserParam.m_sTsChar1.m_bConnectionState;		//����������״̬0������	1���쳣
    Character1.m_bFailureState    = g_sUserParam.m_sTsChar1.m_bFailureState;		//0���޹���	1���й���
    Character1.m_bUnitmode		  	= g_sUserParam.m_sTsChar1.m_bUnitmode;		//0��ѵ��ģʽ	1����ϰģʽ
    Character1.m_bBatteryState	  = g_sUserParam.m_sTsChar1.m_bBatteryState;		//0����������	1��Ƿ��
    Character1.m_bIdentity     	  = g_sUserParam.m_sTsChar1.m_bIdentity;		//��� ENUM_BRCHAR
		Character1.m_bIsNight 				= g_sUserParam.m_sTsChar1.m_bIsNight;
		
    DatBufLen = 0;
    DatBuf[DatBufLen++] = 0xA3;
    DatBuf[DatBufLen++] = 0x60;
    DatBuf[DatBufLen++] = 0;
    DatBuf[DatBufLen++] = IDParam_GetTType();
    DatBuf[DatBufLen++] = IDParam_GetTSubType();
    DatBuf[DatBufLen++] = IDParam_GetRid();
    DatBuf[DatBufLen++] = IDParam_GetPid() >> 8;
    DatBuf[DatBufLen++] = IDParam_GetPid() & 0xFF;
		
		memcpy(&DatBuf[DatBufLen], g_sUserParam.EP_Num, 3);
		DatBufLen += 3;
		
    memcpy(&DatBuf[DatBufLen], &g_sUserParam.m_sManoStart, sizeof(STRUCT_TIME1));
    DatBufLen += sizeof(STRUCT_TIME1);
		
    DatBuf[DatBufLen++] = UserParam_GetKey();

    memcpy(&DatBuf[DatBufLen], &Character1, sizeof(STRUCT_TS_Character1));
    DatBufLen++;
		
		DatBuf[2] = DatBufLen - 3;
    DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;
		
    ChanParam.m_eChan = CHAN_IR38;
    ChanParam.m_pDstAddr = pDstAddr;
    ChanParam.m_byDstAddrLen = DstAddrLen;
    ChanParam.m_pDatBuf = DatBuf;
    ChanParam.m_wDatBufLen = DatBufLen;
    ChanParam.m_dwRepeatCnt = 0x1;  //ֻ��һ��
    ChanParam.m_dwScheTime = 100;  
    
    return InterChanSend(&ChanParam);
}

//������ϰ��������ָ��
void InterConn_ProtDeal_ManoParam_Set(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
    uint8_t i, len;
    uint8_t offset;
		uint32_t LastTimeMs, NowTimeMs;
    Struct_CtrlManoParam_IndividualSoldierDownload ManoParam;
		
    memset(&ManoParam, 0, sizeof(Struct_CtrlManoParam_IndividualSoldierDownload));

		//�������У��
    if(pBuf == NULL || BufLen < 34)
    {
        return;
    }
		
		offset = 3;
    //Э�����ݽ���
    ManoParam.m_byT_Type 					= pBuf[offset++];
    ManoParam.m_byT_Subtype 			= pBuf[offset++];
    ManoParam.m_byRID 						= pBuf[offset++];
    ManoParam.m_aPID[0] 					= pBuf[offset++];
    ManoParam.m_aPID[1] 					= pBuf[offset++];
    ManoParam.m_byMasterT_Type 		= pBuf[offset++];
    ManoParam.m_byMasterT_Subtype = pBuf[offset++];
    ManoParam.m_byMasterRID 			= pBuf[offset++];
    ManoParam.m_wMasterPID 				= pBuf[offset] << 8 | pBuf[offset + 1];
    offset += 2;
    ManoParam.m_sStartTime.m_byYear  = pBuf[offset++];
    ManoParam.m_sStartTime.m_byMonth = pBuf[offset++];
    ManoParam.m_sStartTime.m_byDate  = pBuf[offset++];
    ManoParam.m_sStartTime.m_byHour  = pBuf[offset++];
    ManoParam.m_sStartTime.m_byMinute 		= pBuf[offset++];
    ManoParam.m_sStartTime.m_bySecond 		= pBuf[offset++];
    ManoParam.m_sDayStartTime.m_byHour 		= pBuf[offset++];
    ManoParam.m_sDayStartTime.m_byMinute 	= pBuf[offset++];
    ManoParam.m_sDayStartTime.m_bySecond 	= pBuf[offset++];
    ManoParam.m_sNightStartTime.m_byHour 	= pBuf[offset++];
    ManoParam.m_sNightStartTime.m_byMinute 	= pBuf[offset++];
    ManoParam.m_sNightStartTime.m_bySecond 	= pBuf[offset++];
    ManoParam.m_byKey = pBuf[offset++];
    ManoParam.m_byBR_Attribute = pBuf[offset++];
    ManoParam.m_byBulletMaxAmmuNum[0] = pBuf[offset++];
    ManoParam.m_byBulletMaxAmmuNum[1] = pBuf[offset++];
    ManoParam.m_byBulletMaxAmmuNum[2] = pBuf[offset++];
    ManoParam.m_byShellMaxAmmuNum[0]  = pBuf[offset++];
    ManoParam.m_byShellMaxAmmuNum[1]  = pBuf[offset++];
    ManoParam.m_byShellMaxAmmuNum[2]  = pBuf[offset++];
    ManoParam.m_byAmmuNum = pBuf[offset++];

    //��ֹ����Խ�磬Ŀǰ�����������
    if(ManoParam.m_byAmmuNum > 3)
        ManoParam.m_byAmmuNum = 3;

    for(i = 0; i < ManoParam.m_byAmmuNum; i++)
    {
        ManoParam.m_sAmmu[i].m_byWeapon_Type 	= pBuf[offset++];
        ManoParam.m_sAmmu[i].m_byWeapon_SubType = pBuf[offset++];
        ManoParam.m_sAmmu[i].m_byCfgBulletKind 	= pBuf[offset++];

        for(u8 j = 0; j < ManoParam.m_sAmmu[i].m_byCfgBulletKind; j++)
        {
            ManoParam.m_sAmmu[i].m_sBulletInfo[j].m_byBullet_Type 		= pBuf[offset++];
            ManoParam.m_sAmmu[i].m_sBulletInfo[j].m_byBullet_SubType 	= pBuf[offset++];
            ManoParam.m_sAmmu[i].m_sBulletInfo[j].m_aBulletCfgNum[0] 	= pBuf[offset++];
            ManoParam.m_sAmmu[i].m_sBulletInfo[j].m_aBulletCfgNum[1] 	= pBuf[offset++];
            ManoParam.m_sAmmu[i].m_sBulletInfo[j].m_aBulletCfgNum[2] 	= pBuf[offset++];
            ManoParam.m_sAmmu[i].m_sBulletInfo[j].m_byAmmuWearDepth 	= pBuf[offset++];
            ManoParam.m_sAmmu[i].m_sBulletInfo[j].m_byAmmuConcreteDepth = pBuf[offset++];
            ManoParam.m_sAmmu[i].m_sBulletInfo[j].m_byAmmuRange 		= pBuf[offset++];
        }
    }

    ManoParam.EP_Num[0] = pBuf[offset++];
    ManoParam.EP_Num[1] = pBuf[offset++];
    ManoParam.EP_Num[2] = pBuf[offset++];

    ManoParam.Thickness = pBuf[offset++];

    ManoParam.m_sEndTime.m_byYear 	= pBuf[offset++];
    ManoParam.m_sEndTime.m_byMonth 	= pBuf[offset++];
    ManoParam.m_sEndTime.m_byDate 	= pBuf[offset++];
    ManoParam.m_sEndTime.m_byHour 	= pBuf[offset++];
    ManoParam.m_sEndTime.m_byMinute = pBuf[offset++];
    ManoParam.m_sEndTime.m_bySecond = pBuf[offset++];

		//RID��PIDУ��
    if(CheckRID(ManoParam.m_byRID) && CheckPID((ManoParam.m_aPID[0] << 8 | ManoParam.m_aPID[1])))
    {
        //������վӦ�ó����������յ���ָͬ��
				LastTimeMs = g_sResComTask_Mag.m_sCmdTime.m_ulCtrlManoeuvre;
        NowTimeMs = GetSysRunMilliSecond();

        if((NowTimeMs < LastTimeMs) || ((NowTimeMs - LastTimeMs) < 1000))
        {
            return;
        }

        g_sResComTask_Mag.m_sCmdTime.m_ulCtrlManoeuvre = NowTimeMs;

        //���ν��յ����ĵ�utcʱ��
				RecordRcvBackStageProtocolTime(&g_sResComTask_Mag.m_sCmdTime.m_BSNulCtrlManoeuvre);
        
        //���ݴ���
        if(TRUE == BNSoldierManoParamSetHandle(ManoParam))
        {
						Wtn6_Play(WTN6_MANO_PARAM_CFG_SUCCESS);
            //�¼��ϴ�
            CtrlManoParamEventUploadHandle(&ManoParam);
        }
    }
		
		
}

//A2 71
uint32_t InterConn_ProtDeal_IR_AmmuInfo_Inquire(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
		uint8_t DatBuf[50];
    uint8_t DatBufLen, Offset, b, w;

		S_CONN_CHAN_PARAM ChanParam;
		SINGLE_SLAVER_MAG *pSlaverMag;
	
		uint8_t TType, TSubtype, RID;
    uint16_t PID;

    //�������У��
    if(pBuf == NULL || BufLen < 9)
    {
        return FALSE;
    }

    Offset = 3;
    TType = pBuf[Offset++];
    TSubtype = pBuf[Offset++];
    RID = pBuf[Offset++];
    PID = (uint16_t)pBuf[Offset] << 8 | pBuf[Offset + 1];
    Offset += 2;

		//����̾�ظ�
	
    DatBufLen = 0;
    DatBuf[DatBufLen++] = 0xA3;
    DatBuf[DatBufLen++] = pBuf[1];
    DatBuf[DatBufLen++] = 0;
    DatBuf[DatBufLen++] = IDParam_GetTType();
    DatBuf[DatBufLen++] = IDParam_GetTSubType();
    DatBuf[DatBufLen++] = IDParam_GetRid();
    DatBuf[DatBufLen++] = IDParam_GetPid() >> 8;
    DatBuf[DatBufLen++] = IDParam_GetPid() & 0xFF;

		memcpy(&DatBuf[DatBufLen], g_sUserParam.EP_Num, 3);
		DatBufLen += 3;
		
		memcpy(&DatBuf[DatBufLen], &g_sUserParam.m_sManoStart, sizeof(STRUCT_TIME1));
    DatBufLen += sizeof(STRUCT_TIME1);
		
		DatBuf[DatBufLen++] = UserParam_GetKey();
		
		memcpy(&DatBuf[DatBufLen], &g_sUserParam.m_sTsChar1, sizeof(STRUCT_TS_Character1));
    DatBufLen += sizeof(STRUCT_TS_Character1);
		
		if(g_sUserParam.m_byMode == TRAIN || g_sResComTask_Mag.m_ManoParamFlg == FALSE)
		{
				DatBuf[DatBufLen++] = UserParam_GetWeaponNum();
					
				DatBuf[DatBufLen++] = 0;
				
				UserParam_GetWeaponNum();
			
				for(w = 0; w < g_sUserParam.m_bySelfWeaponNum; w++)
				{
						for(b = 0; b < g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind; b++)
						{
								DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_Type;
								DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_SubType;
								DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[0];
								DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[1];
								DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[2];
						}
				}
		}
		else if(g_sUserParam.m_byMode == GAME_GOING && g_sResComTask_Mag.m_ManoParamFlg == TRUE)
		{
				DatBuf[DatBufLen++] = g_sUserParam.m_byBackstageCfgWeaponNum;
			
				DatBuf[DatBufLen++] = 0;
				
				for(w = 0; w < g_sUserParam.m_byBackstageCfgWeaponNum; w++)
				{
						for(b = 0; b < g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind; b++)
						{
								DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_Type;
								DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_SubType;
								DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[0];
								DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[1];
								DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[2];
						}
				}
		}
		
    DatBuf[2] = DatBufLen - 3;
    DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;
		
    ChanParam.m_eChan = eChan;
    ChanParam.m_pDstAddr = pDstAddr;
    ChanParam.m_byDstAddrLen = DstAddrLen;
    ChanParam.m_pDatBuf = DatBuf;
    ChanParam.m_wDatBufLen = DatBufLen;
    ChanParam.m_dwRepeatCnt = 0x1;  //ֻ��һ��
    ChanParam.m_dwScheTime = 100;  
    
    return InterChanSend(&ChanParam);
}

//A2 74
uint32_t InterConn_ProtDeal_IR_AmmuSupply(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
		uint8_t DatBuf[50];
    uint8_t DatBufLen, Offset, i;

		S_CONN_CHAN_PARAM ChanParam;
		
		Struct_CtrlSupplementDownload Supplement = {0};
		
    memset(&Supplement, 0, sizeof(Struct_CtrlSupplementDownload));

    //�������У��
    if(pBuf == NULL || BufLen < 11 )
    {
        return FALSE;
    }

    Offset = 3;
		
    Supplement.m_byChangeAmmuNum = pBuf[Offset++];
    Supplement.m_byChangeSuppleNum = pBuf[Offset++];

		for(i = 0; i < Supplement.m_byChangeAmmuNum; i++) //�仯�ĵ�ҩ������
    {
        Supplement.m_sChangeAmmu[i].m_byA_Type 			= pBuf[Offset++];
        Supplement.m_sChangeAmmu[i].m_byA_Subtype 		= pBuf[Offset++];
        Supplement.m_sChangeAmmu[i].m_aChangeAmmuNum[0] = pBuf[Offset++];
        Supplement.m_sChangeAmmu[i].m_aChangeAmmuNum[1] = pBuf[Offset++];
        Supplement.m_sChangeAmmu[i].m_aChangeAmmuNum[2] = pBuf[Offset++];
    }

    for(i = 0; i < Supplement.m_byChangeSuppleNum; i++) //�仯������������
    {
        Supplement.m_sChangeSupply[i].m_byM_Type 			= pBuf[Offset++];
        Supplement.m_sChangeSupply[i].m_byM_Subtype 		= pBuf[Offset++];
        Supplement.m_sChangeSupply[i].m_aRemainSupplyNum[0] = pBuf[Offset++];
        Supplement.m_sChangeSupply[i].m_aRemainSupplyNum[1] = pBuf[Offset++];
        Supplement.m_sChangeSupply[i].m_aRemainSupplyNum[2] = pBuf[Offset++];
    }
		IRAmmuGetHandle(&Supplement);
		
		return InterConn_ProtDeal_IR_SetOrRescue_ACK();
}

//A2 A4
uint32_t InterConn_ProtDeal_SlaverConfig(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
    uint8_t DatBuf[50];
    uint8_t DatBufLen;
    S_CONN_CHAN_PARAM ChanParam;

    uint8_t Offset, TType, TSubtype, RID, Idx, i;
    uint16_t PID;
    SINGLE_SLAVER_PARAM SlaverParam = {0};
		SINGLE_SLAVER_MAG *pMag = NULL;
	
    //�������У��
    if(pBuf == NULL || BufLen < 21 )
    {
        return FALSE;
    }

    Offset = 5;
    RID = pBuf[Offset++];
    PID = (uint16_t)pBuf[Offset] << 8 | pBuf[Offset + 1];
    Offset += 2;
    Idx = pBuf[Offset++];
		TType = pBuf[Offset++];
    TSubtype = pBuf[Offset++];
    //�Ƿ��Ƿ����Լ�
		/*
    if(TRUE != IDParam_IsIDValid(TType, TSubtype, RID, PID))
    {
        return;
    }
		*/

		/*
    if((Idx == 0) || Idx > SYS_SLAVER_CNT_MAX)
    {
        return FALSE;
    }
		*/
		
		for(i = 0; i < SYS_SLAVER_CNT_MAX; i++)
		{
				pMag = UserParam_GetSlaverMag(i);

				if((pMag->m_byTType == 0x00 && pMag->m_byTSubType == 0x00) || (pMag->m_byTType == 0xFF && pMag->m_byTSubType == 0xFF))
				{
						if(TType != 0xFE)
						{
							SlaverParam.m_byWeaponIdx = UserParam_GetWeaponNum();							
						}
						else
						{
							SlaverParam.m_byWeaponIdx = 0xFF;
						}
						break;
				}
			
				if(TType == pMag->m_byTType && TSubtype == pMag->m_byTSubType)
					break;
		}
		
    //�������ò���
    SlaverParam.m_byTType = TType;
    SlaverParam.m_byTSubType = TSubtype;
    SlaverParam.m_byWType = pBuf[Offset++];
		SlaverParam.m_byIdx = i;
		
    memcpy(SlaverParam.m_aWMac, &pBuf[Offset], sizeof(SlaverParam.m_aWMac));
    Offset += sizeof(SlaverParam.m_aWMac);
		
    SysParam_SetSlaverParam(&SlaverParam, i);
    IWDG_Feed();
    SysParam_Save();
    IWDG_Feed();

    //����̾�ظ�

    DatBufLen = 0;
    DatBuf[DatBufLen++] = 0xA3;
    DatBuf[DatBufLen++] = pBuf[1];
    DatBuf[DatBufLen++] = 0;
    DatBuf[DatBufLen++] = IDParam_GetTType();
    DatBuf[DatBufLen++] = IDParam_GetTSubType();
    DatBuf[DatBufLen++] = IDParam_GetRid();
    DatBuf[DatBufLen++] = IDParam_GetPid() >> 8;
    DatBuf[DatBufLen++] = IDParam_GetPid() & 0xFF;
    DatBuf[DatBufLen++] = Idx;
    DatBuf[DatBufLen++] = SlaverParam.m_byWType;
    memcpy(&DatBuf[DatBufLen], SlaverParam.m_aWMac, sizeof(SlaverParam.m_aWMac));
    DatBufLen += sizeof(SlaverParam.m_aWMac);

    DatBuf[2] = DatBufLen - 3;
    DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;
		
    ChanParam.m_eChan = eChan;
    ChanParam.m_pDstAddr = pDstAddr;
    ChanParam.m_byDstAddrLen = DstAddrLen;
    ChanParam.m_pDatBuf = DatBuf;
    ChanParam.m_wDatBufLen = DatBufLen;
    ChanParam.m_dwRepeatCnt = 0x1;  //ֻ��һ��
    ChanParam.m_dwScheTime = 100;  
    
    return InterChanSend(&ChanParam);
}

void ResCom_FourGParam_Set()
{
	RESCOMTASK_MAG *pMag = ResComTask_HandleGet();
		
	FRAME_BUF *pFrame = NULL;								
	
	/*����ip��ַ �˿ں� ��*/
  uint8_t pBuf[50] = {
													 0x5A, 0x28, 0xEE, 
													 0x31, 0x39, 0x32, 0x2E, 
													 0x31, 0x36, 0x38, 0x2E, 
													 0x31, 0x2E, 
													 0x36, 0x30,
													 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
				
													 0x34, 0x30, 0x30, 0x31, 0x00, 0x00, 0x00, 0x00, 
				
													 0x36, 0x35, 0x30, 0x31, 0x00, 0x00, 0x00, 0x00, 
													 0x31, 0x00, 0x31, 
													 0xAA
													};

	pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

	if(pFrame == NULL)
	{
			return;
	}

	//���
	pFrame->m_wBufLen = 0;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x5A;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x0;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xEE;
	
	sprintf((char*)&pFrame->m_byBuf[pFrame->m_wBufLen], "%d.%d.%d.%d",
		g_sIDParam.m_aServerIP[0], g_sIDParam.m_aServerIP[1], g_sIDParam.m_aServerIP[2], g_sIDParam.m_aServerIP[3]);
	pFrame->m_wBufLen += 20;
	
	sprintf((char*)&pFrame->m_byBuf[pFrame->m_wBufLen], "%d",
		g_sIDParam.m_wServerPort);
	pFrame->m_wBufLen += 8;

	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x36;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x35;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x30;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x31;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x31;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x30;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x31;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xAA;

  pFrame->m_byBuf[1] = pFrame->m_wBufLen;   

	pFrame->m_dwCnt = 2;
	pFrame->m_dwID = 0x5AAA5AAA;
	pFrame->m_dwScheTime = 100;
	pFrame->m_bySendNum = 0;
	SComTask_SendListAppend(pMag->m_sTaskMag.m_pSendList, (void*)pFrame);
}

uint32_t InterConn_ProtDeal_ParamSend(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
		//����̾�ظ�
		uint8_t DatBuf[50];
    uint8_t DatBufLen;
    S_CONN_CHAN_PARAM ChanParam;
	
		//�������У��
    if(pBuf == NULL || BufLen < 5 )
    {
        return FALSE;
    }
		
    DatBufLen = 0;
    DatBuf[DatBufLen++] = 0xA3;
    DatBuf[DatBufLen++] = 0xAE;
    DatBuf[DatBufLen++] = 0;
    DatBuf[DatBufLen++] = IDParam_GetTType();
    DatBuf[DatBufLen++] = IDParam_GetTSubType();
    DatBuf[DatBufLen++] = IDParam_GetRid();
    DatBuf[DatBufLen++] = IDParam_GetPid() >> 8;
    DatBuf[DatBufLen++] = IDParam_GetPid() & 0xFF;

    memcpy((void *)&DatBuf[DatBufLen], (void *)IDParam_GetServerIP(), 4);
    DatBufLen += 4;

    DatBuf[DatBufLen++] = IDParam_GetServerPort() >> 8;
    DatBuf[DatBufLen++] = IDParam_GetServerPort() & 0xFF;

    DatBuf[DatBufLen++] = IDParam_GetIsUDP();
    DatBuf[DatBufLen++] = UserParam_GetWTxPwr();

    memcpy((void *)&DatBuf[DatBufLen], (void *)UserParam_GetWMac(), 8);
    DatBufLen += 8;

    DatBuf[2] = DatBufLen - 3;
    DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;
    
		ChanParam.m_eChan = CHAN_IR38;
    ChanParam.m_pDstAddr = pDstAddr;
    ChanParam.m_byDstAddrLen = DstAddrLen;
    ChanParam.m_pDatBuf = DatBuf;
    ChanParam.m_wDatBufLen = DatBufLen;
    ChanParam.m_dwRepeatCnt = 0x1;  //ֻ��һ��
    ChanParam.m_dwScheTime = 100;  
    
    return InterChanSend(&ChanParam);
}

uint32_t InterConn_ProtDeal_ParamSet(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
    uint8_t DatBuf[50];
    uint8_t DatBufLen;
    S_CONN_CHAN_PARAM ChanParam;

    uint8_t Offset, TType, TSubtype, RID, IsUDP, TxPwr, NeedSave;
    uint16_t PID, SerPort;
    uint8_t SerIp[4], ZigMac[8];
		uint8_t IP_Modify_Flag = 0, Port_Modify_Flag = 0;

    //�������У��
    if(pBuf == NULL || BufLen < 25)
    {
        return FALSE;
    }

    Offset = 3;
    TType = pBuf[Offset++];
    TSubtype = pBuf[Offset++];
    RID = pBuf[Offset++];
    PID = (uint16_t)pBuf[Offset] << 8 | pBuf[Offset + 1];
    Offset += 2;

		/*
    //�Ƿ��Ƿ����Լ�
    if(TRUE != IDParam_IsIDValid(TType, TSubtype, RID, PID))
    {
        return;
    }
		*/
		
		memcpy(SerIp, &pBuf[Offset], 4);
    Offset += 4;
    SerPort = (uint16_t)pBuf[Offset] << 8 | pBuf[Offset + 1];
    Offset += 2;
    IsUDP = pBuf[Offset++];
    TxPwr = pBuf[Offset++];
    memcpy(ZigMac, &pBuf[Offset], 8);
    Offset += 8;

    NeedSave = FALSE;

    if(TRUE == IsParamValid(SerIp, sizeof(SerIp)))
    {
        IDParam_SetServerIP(SerIp, sizeof(SerIp));
				IP_Modify_Flag = 0x5A;
        NeedSave = TRUE;
    }

    if(TRUE == IsParamValid((uint8_t *)&SerPort, sizeof(SerPort)))
    {
        IDParam_SetServerPort(SerPort);
				Port_Modify_Flag = 0x5A;
        NeedSave = TRUE;
    }

    if(TRUE == IsParamValid((uint8_t *)&IsUDP, 1))
    {
        IDParam_SetIsUDP(IsUDP);
        NeedSave = TRUE;
    }

    if(TRUE == IsParamValid((uint8_t *)&TxPwr, 1))
    {
        IDParam_SetTxPwr(TxPwr);
        NeedSave = TRUE;
    }

    if(TRUE == IsParamValid(ZigMac, sizeof(ZigMac)))
    {
        IDParam_SetWMac(ZigMac, sizeof(ZigMac));
        NeedSave = TRUE;
    }

		if(IP_Modify_Flag == 0x5A && Port_Modify_Flag == 0x5A)
				ResCom_FourGParam_Set();
		
    if(TRUE == NeedSave)
    {
        IWDG_Feed();
        IDParam_Save();
        IWDG_Feed();
    }

    //����̾�ظ�

    DatBufLen = 0;
    DatBuf[DatBufLen++] = 0xA3;
    DatBuf[DatBufLen++] = pBuf[1];
    DatBuf[DatBufLen++] = 0;
    DatBuf[DatBufLen++] = IDParam_GetTType();
    DatBuf[DatBufLen++] = IDParam_GetTSubType();
    DatBuf[DatBufLen++] = IDParam_GetRid();
    DatBuf[DatBufLen++] = IDParam_GetPid() >> 8;
    DatBuf[DatBufLen++] = IDParam_GetPid() & 0xFF;

    memcpy((void *)&DatBuf[DatBufLen], (void *)IDParam_GetServerIP(), 4);
    DatBufLen += 4;

    DatBuf[DatBufLen++] = IDParam_GetServerPort() >> 8;
    DatBuf[DatBufLen++] = IDParam_GetServerPort() & 0xFF;

    DatBuf[DatBufLen++] = IDParam_GetIsUDP();
    DatBuf[DatBufLen++] = UserParam_GetWTxPwr();

    memcpy((void *)&DatBuf[DatBufLen], (void *)UserParam_GetWMac(), 8);
    DatBufLen += 8;

    DatBuf[2] = DatBufLen - 3;
    DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;
    
		ChanParam.m_eChan = eChan;
    ChanParam.m_pDstAddr = pDstAddr;
    ChanParam.m_byDstAddrLen = DstAddrLen;
    ChanParam.m_pDatBuf = DatBuf;
    ChanParam.m_wDatBufLen = DatBufLen;
    ChanParam.m_dwRepeatCnt = 0x1;  //ֻ��һ��
    ChanParam.m_dwScheTime = 100;  
    
    return InterChanSend(&ChanParam);
}

// A3 A5 ���豸����ȷ��
uint32_t InterConn_ProtDeal_SlaverConnectRet(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
    uint8_t Offset, TType, TSubtype, RID;
    uint16_t PID;
    uint32_t MagicID;
    SINGLE_SLAVER_MAG *pSlaverMag;

    SCOMTASK_MAG *pScomTaskMag;
    
    //�������У��
    if(pBuf == NULL || BufLen < 9 )
    {
        return FALSE;
    }

    Offset = 3;
    TType = pBuf[Offset++];
    TSubtype = pBuf[Offset++];
    RID = pBuf[Offset++];
    PID = (uint16_t)pBuf[Offset] << 8 | pBuf[Offset + 1];
    Offset += 2;

    //pSlaverMag = SlaverMag_GetHandleByTType(TType);
    pSlaverMag = SlaverMag_GetHandleByMac(pDstAddr,8);
    if(NULL != pSlaverMag)
    {
				if(pSlaverMag->m_byIsOnline == FALSE)
				{
						if(pSlaverMag->m_byTType != 0xFE)
						{
							//if(g_sHmiComTask_Mag.m_byHelmetIsOnline == TRUE)
							//	CtrlHelmetVoice((uint8_t)WTN6_WeaponConnected);
							//else
							Wtn6_Play(WTN6_WeaponConnected);
						}
						else if(pSlaverMag->m_byTType == 0xFE && pSlaverMag->m_byTSubType == 0x08)
						{
							g_sHmiComTask_Mag.m_byHelmetIsOnline = TRUE;
							//CtrlHelmetVoice((uint8_t)WTN6_Helmet_Already_Connect);
							Wtn6_Play(WTN6_Helmet_Already_Connect);
						}
						else
						{
							Wtn6_Play(WTN6_PAIR_SUCCESS);
						}
				}
				
				g_sUserParam.m_sTsChar1.m_bConnectionState = GetAllSlavesConnectStatus();

        pSlaverMag->m_byIsOnline = TRUE;
				pSlaverMag->m_byIsSleepFlg = FALSE;
				
        pSlaverMag->m_dwLastCommTimeMs = GetSysRunMilliSecond();
        memcpy((void *)&pSlaverMag->m_sTsChar1,(void *)&pBuf[Offset+1],sizeof(pSlaverMag->m_sTsChar1));
        if(CHAN_ZIGBEE == eChan)
        {
            MagicID = pBuf[1]<<24;
            MagicID |= (pDstAddr[5]<<16);
            MagicID |= (pDstAddr[6]<<8);
            MagicID |= (pDstAddr[7]<<0);
            pScomTaskMag = ZigComTask_TaskHandleGet();
            SComTask_SendListDeleteByFrameId(pScomTaskMag->m_pSendList,MagicID);    
        }
    }
		
    return TRUE;
}

//A2 A6
void InterConn_ProtDeal_Slaver_Weapon_State(uint8_t mode)
{
		uint32_t NowTime = GetSysRunMilliSecond();
		static uint32_t WeaponLockTime = 0;
	
		uint8_t DatBuf[50];
    uint8_t DatBufLen, i;

	  S_CONN_CHAN_PARAM ChanParam;
    SINGLE_SLAVER_MAG *pSlaverMag;

		for(i = 0; i < UserParam_GetSlaverCnt(); i++)
		{
				pSlaverMag = UserParam_GetSlaverMag(i);
				
				if(pSlaverMag->m_byTType == 0xFE)
					continue;

				if(pSlaverMag->m_byIsOnline == FALSE)
					continue;
				
				if(mode == FALSE)
				{
						pSlaverMag->m_byIsLock = 0;
				}
				else
				{	
						pSlaverMag->m_byIsLock = 1;
				}
				
				if((NowTime - WeaponLockTime) > 1000 && pSlaverMag->m_byIsSleepFlg == FALSE)
				{
					if(pSlaverMag->m_byIsLock == 1)
						Wtn6_Play(WTN6_OutRange);
					else
						Wtn6_Play(WTN6_WeaponUnlock);
					WeaponLockTime = NowTime;
				}
				
				DatBufLen = 0;
				DatBuf[DatBufLen++] = 0xA2;
				DatBuf[DatBufLen++] = 0xA6;
				DatBuf[DatBufLen++] = 0;
				DatBuf[DatBufLen++] = pSlaverMag->m_byTType;
				DatBuf[DatBufLen++] = pSlaverMag->m_byTSubType;
				DatBuf[DatBufLen++] = pSlaverMag->m_byRid;
				DatBuf[DatBufLen++] = pSlaverMag->m_wPid >> 8;
				DatBuf[DatBufLen++] = pSlaverMag->m_wPid & 0xFF;
				DatBuf[DatBufLen++] = mode;
				
				DatBuf[2] = DatBufLen - 3;
				DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
				DatBufLen++;
				 
				ChanParam.m_eChan = CHAN_ZIGBEE;
				ChanParam.m_pDstAddr = pSlaverMag->m_aWMac;
				ChanParam.m_byDstAddrLen = 8;
				ChanParam.m_pDatBuf = DatBuf;
				ChanParam.m_wDatBufLen = DatBufLen;
				ChanParam.m_dwRepeatCnt = 20; 
				ChanParam.m_dwScheTime = 200; 
				
				InterChanSend(&ChanParam);	
		}
}
//A3 A6
uint32_t InterConn_ProtDeal_Slaver_WeaponLockRet(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
		uint8_t DatBuf[50];
    uint8_t DatBufLen, Offset;

		uint32_t Magic_ID;
		
	  S_CONN_CHAN_PARAM ChanParam;
    SINGLE_SLAVER_MAG *pSlaverMag;
    SCOMTASK_MAG *pScomTaskMag;
	
		pSlaverMag = SlaverMag_GetHandleByMac(pDstAddr,8);

		if(NULL == pSlaverMag)
			return FALSE;

		pSlaverMag->m_dwLastCommTimeMs = GetSysRunMilliSecond();
		
		if(pSlaverMag->m_byIsSleepFlg == TRUE)
			return FALSE;
		
		
		Offset = 3;
		pSlaverMag->m_byTType = pBuf[Offset++];
    pSlaverMag->m_byTSubType = pBuf[Offset++];
    pSlaverMag->m_byRid = pBuf[Offset++];
    pSlaverMag->m_wPid = pBuf[Offset] << 8 | pBuf[Offset + 1];
		Offset += 2;
		pSlaverMag->m_byIsLock = pBuf[Offset];

		Magic_ID = 0;
		Magic_ID |= (0xA6 << 24) ;
    Magic_ID |= (pDstAddr[5] << 16);
    Magic_ID |= (pDstAddr[6] << 8);
    Magic_ID |= (pDstAddr[7] << 0);
		pScomTaskMag = ZigComTask_TaskHandleGet();
    SComTask_SendListDeleteByFrameId(pScomTaskMag->m_pSendList,Magic_ID);  
		
		return TRUE;
}

// A3 A7  �������豸״̬���ָ��
uint32_t InterConn_ProtDeal_SlaverStatusRet(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
		uint8_t Offset, TType, TSubtype, WType;

    SINGLE_SLAVER_MAG *pSlaverMag;

    //�������У��
    if(pBuf == NULL || BufLen < 9 )
    {
        return FALSE;
    }

    Offset = 8;
    TType = pBuf[Offset++];
    TSubtype = pBuf[Offset++];

    Offset += 9;

    pSlaverMag = SlaverMag_GetHandleByMac(pDstAddr,8);
    if(NULL != pSlaverMag)
    {
        pSlaverMag->m_dwLastCommTimeMs = GetSysRunMilliSecond();
				pSlaverMag->m_byIsSleepFlg = FALSE;
        memcpy((void *)&pSlaverMag->m_sTsChar1,(void *)&pBuf[Offset],sizeof(pSlaverMag->m_sTsChar1));
        Offset += 1;
        pSlaverMag->m_wBatVol = pBuf[Offset]<<8|pBuf[Offset+1];
				
        #if 0
        if(CHAN_ZIGBEE == eChan)
        {
            MagicID = pBuf[1]<<24;
            MagicID |= (pRetMac[5]<<16);
            MagicID |= (pRetMac[6]<<8);
            MagicID |= (pRetMac[7]<<0);
            pScomTaskMag = ZigComTask_TaskHandleGet();
            SComTask_SendListDeleteByFrameId(pScomTaskMag->m_pSendList,MagicID);    
        }
        #endif
    }

    return TRUE;
}

uint32_t InterConn_ProtDeal_Slaver_LightWeapon(SINGLE_SLAVER_MAG *pSlaverMag) 
{
		S_CONN_CHAN_PARAM ChanParam;
	
		uint8_t DatBuf[50];
    uint8_t i, w = 0, b, DatBufLen;
			
		w = pSlaverMag->m_byWeaponIdx;

		DatBufLen = 0;
		DatBuf[DatBufLen++] = 0xA2;
		DatBuf[DatBufLen++] = 0xA8;
		DatBuf[DatBufLen++] = 0;
		DatBuf[DatBufLen++] = pSlaverMag->m_byTType;//g_sUserParam.m_saAmmuInfo[w].m_byWeapon_Type;
		DatBuf[DatBufLen++] = pSlaverMag->m_byTSubType;//g_sUserParam.m_saAmmuInfo[w].m_byWeapon_SubType;
		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind;
		
		for(b = 0; b < g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind; b++)
		{
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_Type;
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_SubType;
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum[0];
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum[1];
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum[2];

			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[0];
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[1];
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[2];

			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuWearDepth;
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuConcreteDepth;
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuRange;
		}

		DatBuf[2] = DatBufLen - 3;
		DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;

		ChanParam.m_eChan = CHAN_ZIGBEE;
		ChanParam.m_pDstAddr = pSlaverMag->m_aWMac;
		ChanParam.m_byDstAddrLen = 8;
		ChanParam.m_pDatBuf = DatBuf;
		ChanParam.m_wDatBufLen = DatBufLen;
		ChanParam.m_dwRepeatCnt = 0xFFFFFFFF; 
		ChanParam.m_dwScheTime = 500; 

		InterChanSend(&ChanParam);
}

uint32_t InterConn_ProtDeal_Slaver_HeavyWeapon(SINGLE_SLAVER_MAG *pSlaverMag) 
{
		S_CONN_CHAN_PARAM ChanParam;
	
		uint8_t DatBuf[50];
    uint8_t i, w = 0, b, DatBufLen;
			
		w = pSlaverMag->m_byWeaponIdx;

		DatBufLen = 0;
		DatBuf[DatBufLen++] = 0xA2;
		DatBuf[DatBufLen++] = 0xA9;
		DatBuf[DatBufLen++] = 0;
		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_byWeapon_Type;
		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_byWeapon_SubType;
		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind;
		
		for(b = 0; b < g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind; b++)
		{
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_Type;
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_SubType;
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum[0];
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum[1];
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum[2];

			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[0];
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[1];
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[2];

			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuHitRate;
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuAngle;
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuSpeed;
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuWearDepth;
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuConcreteDepth;
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuRange;
		}

		DatBuf[2] = DatBufLen - 3;
		DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;
		 
		ChanParam.m_eChan = CHAN_ZIGBEE;
		ChanParam.m_pDstAddr = pSlaverMag->m_aWMac;
		ChanParam.m_byDstAddrLen = 8;
		ChanParam.m_pDatBuf = DatBuf;
		ChanParam.m_wDatBufLen = DatBufLen;
		ChanParam.m_dwRepeatCnt = 0xFFFF; 
		ChanParam.m_dwScheTime = 200; 

		InterChanSend(&ChanParam);
}

void PJP_BulletInfo_Sync(SINGLE_SLAVER_MAG *pMag)
{
		SINGLE_SLAVER_MAG *pSlaverMag;
	
		uint8_t DatBuf[50];
    uint8_t w, i, DatBufLen;

	  S_CONN_CHAN_PARAM ChanParam;
		
		for(i = 0; i < UserParam_GetSlaverCnt(); i++)
		{
				pSlaverMag = UserParam_GetSlaverMag(i);

				if(pSlaverMag->m_byTType == 0xFE && pSlaverMag->m_byTSubType == 0x06)
					break;
		}
		
		w = pMag->m_byWeaponIdx;
		
		DatBufLen = 0;
		DatBuf[DatBufLen++] = 0xA3;
		DatBuf[DatBufLen++] = 0xAB;
		DatBuf[DatBufLen++] = 0;
		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_byWeapon_Type;
		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_byWeapon_SubType;
		DatBuf[DatBufLen++] = IDParam_GetRid();
		DatBuf[DatBufLen++] = IDParam_GetPid() >> 8;
		DatBuf[DatBufLen++] = IDParam_GetPid() & 0xFF;

		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_byBullet_Type;
		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_byBullet_SubType;
		
		DatBuf[DatBufLen++] = 0x1;//g_sUserParam.m_saAmmuInfo[w].UnLockFlag;

		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_aBulletRemainNum[0];
		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_aBulletRemainNum[1];
		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_aBulletRemainNum[2];
		
		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_aBulletCfgNum[0];
		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_aBulletCfgNum[1];
		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_aBulletCfgNum[2];

		DatBuf[2] = DatBufLen - 3;
		DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;

		ChanParam.m_eChan = CHAN_ZIGBEE;
		ChanParam.m_pDstAddr = pSlaverMag->m_aWMac;
		ChanParam.m_byDstAddrLen = 8;
		ChanParam.m_pDatBuf = DatBuf;
		ChanParam.m_wDatBufLen = DatBufLen;
		ChanParam.m_dwRepeatCnt = 10; 
		ChanParam.m_dwScheTime = 200; 

		InterChanSend(&ChanParam);
		return;
}

//A3 A8
uint32_t InterConn_ProtDeal_Slaver_WeaponInfoRet(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
		uint8_t Offset, TType, TSubtype;
		uint8_t DatBuf[50];
    uint8_t w = 0, i = 0, b = 0, DatBufLen , R_Num[3], T_Num[3] = {0};
		
		uint32_t Rnum = 0, Tnum = 0, Magic_ID;
	  S_CONN_CHAN_PARAM ChanParam;
    SINGLE_SLAVER_MAG *pSlaverMag;
    SCOMTASK_MAG *pScomTaskMag;

    //�������У��
    if(pBuf == NULL || BufLen < 9 )
    {
        return FALSE;
    }

    Offset = 3;
    TType = pBuf[Offset++];
    TSubtype = pBuf[Offset++];

		pSlaverMag = SlaverMag_GetHandleByMac(pDstAddr,8);

		if(NULL == pSlaverMag)
			return FALSE;

		if(g_sUserParam.m_byMode == GAME_GOING && pSlaverMag->m_byTType != 0x41 && pSlaverMag->m_byTType != 0x42 && pSlaverMag->m_byTType != 0x25)
		{
				//ͬ��
				if(pSlaverMag->m_byIsAmmuInit == TRUE)
				{
					for(i = 0; i < g_sUserParam.m_byBackstageCfgWeaponNum; i++)
					{
							if(pSlaverMag->m_byTType == g_sUserParam.m_saBackstageCfgAmmuInfo[i].m_byWeapon_Type
								&& pSlaverMag->m_byTSubType == g_sUserParam.m_saBackstageCfgAmmuInfo[i].m_byWeapon_SubType
							)
							{
								memcpy(&g_sUserParam.m_saAmmuInfo[pSlaverMag->m_byWeaponIdx], &g_sUserParam.m_saBackstageCfgAmmuInfo[i], sizeof(SOLDIERAMMU_T));
							}
					}
				}
				else
				{
						if(pSlaverMag->m_byTType == TType && pSlaverMag->m_byTSubType == TSubtype) 
						{
								w = pSlaverMag->m_byWeaponIdx;	
								g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_byWeapon_Type = TType;
								g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_byWeapon_SubType = TSubtype;
								g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_byCfgBulletKind = pBuf[Offset++];
								
								for(b = 0; b < g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_byCfgBulletKind; b++)
								{
									g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_byBullet_Type = pBuf[Offset++];
									g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_byBullet_SubType = pBuf[Offset++];

									g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum[0] = pBuf[Offset++];
									g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum[1] = pBuf[Offset++];
									g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum[2] = pBuf[Offset++];

									g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[0] = pBuf[Offset++];
									g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[1] = pBuf[Offset++];
									g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[2] = pBuf[Offset++];
															
									memcpy(g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum, 
													 g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum ,3);

									g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_byAmmuWearDepth = pBuf[Offset++];
									g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_byAmmuConcreteDepth = pBuf[Offset++];
									g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_byAmmuRange = pBuf[Offset++];
								}
						}
				}
		}
		else
		{
			if(pSlaverMag->m_byTType == TType && pSlaverMag->m_byTSubType == TSubtype) 
			{
					w = pSlaverMag->m_byWeaponIdx;	
					g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_byWeapon_Type = TType;
					g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_byWeapon_SubType = TSubtype;
					g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_byCfgBulletKind = pBuf[Offset++];
					
					for(b = 0; b < g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_byCfgBulletKind; b++)
					{
						g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_byBullet_Type = pBuf[Offset++];
						g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_byBullet_SubType = pBuf[Offset++];

						g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum[0] = pBuf[Offset++];
						g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum[1] = pBuf[Offset++];
						g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum[2] = pBuf[Offset++];

						g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[0] = pBuf[Offset++];
						g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[1] = pBuf[Offset++];
						g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[2] = pBuf[Offset++];
												
						memcpy(g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum, 
										 g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum ,3);

						g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_byAmmuWearDepth = pBuf[Offset++];
						g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_byAmmuConcreteDepth = pBuf[Offset++];
						g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_byAmmuRange = pBuf[Offset++];
					}
				}

				g_sUserParam.m_bySelfWeaponNum = UserParam_GetWeaponNum();
				for(w = g_sUserParam.m_bySelfWeaponNum; w < WEAPONKIND_MAX; w++)    
				{
						memset(&g_sUserParam.m_saWeaponSelfAmmuInfo[w],0,sizeof(SOLDIERAMMU_T));
				}	
				
				//ͬ��
				memcpy(&g_sUserParam.m_saAmmuInfo, &g_sUserParam.m_saWeaponSelfAmmuInfo,
					 sizeof(SOLDIERAMMU_T) * g_sUserParam.m_bySelfWeaponNum);  //������һ�ε���������	
				g_sUserParam.m_byWeaponNum = g_sUserParam.m_bySelfWeaponNum;
				//IWDG_Feed();
				//UserParam_Save();
				//IWDG_Feed();		
		}
		if(TType == 0x14)
		{
				PJP_BulletInfo_Sync(pSlaverMag);
		}
		
		pSlaverMag->m_dwLastCommTimeMs = GetSysRunMilliSecond();
		
		Magic_ID = 0;
		Magic_ID |= (pBuf[1] << 24) ;
    Magic_ID |= (pDstAddr[5] << 16);
    Magic_ID |= (pDstAddr[6] << 8);
    Magic_ID |= (pDstAddr[7] << 0);
		pScomTaskMag = ZigComTask_TaskHandleGet();
    SComTask_SendListDeleteByFrameId(pScomTaskMag->m_pSendList,Magic_ID);  
		
		return TRUE;
}

//A3 A9
uint32_t InterConn_ProtDeal_Slaver_HeavyWeaponInfoRet(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
		uint8_t Offset, TType, TSubtype;
		uint8_t DatBuf[50];
    uint8_t w = 0, i = 0, b = 0, DatBufLen;

		uint32_t Magic_ID;
	  S_CONN_CHAN_PARAM ChanParam;
    SINGLE_SLAVER_MAG *pSlaverMag;
    SCOMTASK_MAG *pScomTaskMag;

    //�������У��
    if(pBuf == NULL || BufLen < 9 )
    {
        return FALSE;
    }

    Offset = 3;
    TType = pBuf[Offset++];
    TSubtype = pBuf[Offset++];

		pSlaverMag = SlaverMag_GetHandleByMac(pDstAddr,8);

		if(NULL == pSlaverMag)
			return FALSE;

		/*
		if(g_sUserParam.m_byMode == GAME_GOING)
		{
				//ͬ��
				if(pSlaverMag->m_byIsAmmuInit == TRUE)
				{
					for(i = 0; i < g_sUserParam.m_byBackstageCfgWeaponNum; i++)
					{
							if(pSlaverMag->m_byTType == g_sUserParam.m_saBackstageCfgAmmuInfo[i].m_byWeapon_Type
								&& pSlaverMag->m_byTSubType == g_sUserParam.m_saBackstageCfgAmmuInfo[i].m_byWeapon_SubType
							)
							{
								memcpy(&g_sUserParam.m_saAmmuInfo[pSlaverMag->m_byWeaponIdx], &g_sUserParam.m_saBackstageCfgAmmuInfo[i], sizeof(SOLDIERAMMU_T));
							}
					}
									
					for(w = g_sUserParam.m_byBackstageCfgWeaponNum; w < WEAPONKIND_MAX; w++)
							memset(&g_sUserParam.m_saAmmuInfo[w],0,sizeof(SOLDIERAMMU_T));
				}
				else
				{
						memset(&g_sUserParam.m_saAmmuInfo[pSlaverMag->m_byWeaponIdx].m_sBulletInfo, 0, sizeof(BULLETINFO_T));
						InterConn_ProtDeal_Slaver_Weapon_State(TRUE);		
				}
		}
		else
		{
		*/
				//if(FALSE == pSlaverMag->m_byIsSleepFlg)
				{
					if(pSlaverMag->m_byTType == TType && pSlaverMag->m_byTSubType == TSubtype) 
					{
						w = pSlaverMag->m_byWeaponIdx;
						g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_byWeapon_Type = TType;
						g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_byWeapon_SubType = TSubtype;
						g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_byCfgBulletKind = pBuf[Offset++];
						
						for(b = 0; b < g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_byCfgBulletKind; b++)
						{
							g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_byBullet_Type = pBuf[Offset++];
							g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_byBullet_SubType = pBuf[Offset++];
							g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum[0] = pBuf[Offset++];
							g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum[1] = pBuf[Offset++];
							g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum[2] = pBuf[Offset++];
							
							memcpy(g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum, 
										 g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum ,3);
							
							g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[0] = pBuf[Offset++];
							g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[1] = pBuf[Offset++];
							g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[2] = pBuf[Offset++];

							g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_byAmmuHitRate = pBuf[Offset++];
							g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_byAmmuAngle = pBuf[Offset++];
							g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_byAmmuSpeed = pBuf[Offset++];
							g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_byAmmuWearDepth = pBuf[Offset++];
							g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_byAmmuConcreteDepth = pBuf[Offset++];
							g_sUserParam.m_saWeaponSelfAmmuInfo[w].m_sBulletInfo[b].m_byAmmuRange = pBuf[Offset++];
						}
					}
				}

				g_sUserParam.m_bySelfWeaponNum = UserParam_GetWeaponNum();

				//ͬ��
				memcpy(&g_sUserParam.m_saAmmuInfo, &g_sUserParam.m_saWeaponSelfAmmuInfo,
					 sizeof(SOLDIERAMMU_T) * g_sUserParam.m_bySelfWeaponNum);  //������һ�ε���������	
				
				for(w = g_sUserParam.m_bySelfWeaponNum; w < WEAPONKIND_MAX; w++)    
				{
						memset(&g_sUserParam.m_saWeaponSelfAmmuInfo[w],0,sizeof(SOLDIERAMMU_T));
				}	
				
				IWDG_Feed();
				UserParam_Save();
				IWDG_Feed();			
		//}
		pSlaverMag->m_dwLastCommTimeMs = GetSysRunMilliSecond();
		
		Magic_ID = 0;
		Magic_ID |= (pBuf[1] << 24) ;
    Magic_ID |= (pDstAddr[5] << 16);
    Magic_ID |= (pDstAddr[6] << 8);
    Magic_ID |= (pDstAddr[7] << 0);
		pScomTaskMag = ZigComTask_TaskHandleGet();
    SComTask_SendListDeleteByFrameId(pScomTaskMag->m_pSendList,Magic_ID);  

		return TRUE;
}

void InterConn_ProtDeal_Slaver_AmmuSupply(SINGLE_SLAVER_MAG *pSlaverMag)
{
		S_CONN_CHAN_PARAM ChanParam;
		
		uint8_t DatBuf[50];
    uint8_t w = 0, b = 0, DatBufLen;
		uint32_t Ammu_RemainNum = 0;

		if(pSlaverMag->m_byIsSleepFlg == FALSE)
		{
			w = pSlaverMag->m_byWeaponIdx;
		}
		else
			return;
		
		if(g_sUserParam.m_saAmmuInfo[w].UnLockFlag != FALSE)
			return;
		
		DatBufLen = 0;
		DatBuf[DatBufLen++] = 0xA2;
		DatBuf[DatBufLen++] = 0xAA;
		DatBuf[DatBufLen++] = 0;
		DatBuf[DatBufLen++] = IDParam_GetTType();
		DatBuf[DatBufLen++] = IDParam_GetTSubType();
		DatBuf[DatBufLen++] = IDParam_GetRid();
		DatBuf[DatBufLen++] = IDParam_GetPid() >> 8;
		DatBuf[DatBufLen++] = IDParam_GetPid() & 0xFF;

		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_byBullet_Type;
		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_byBullet_SubType;
		DatBuf[DatBufLen++] = 0x01;

		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_aBulletTotalNum[0];
		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_aBulletTotalNum[1];
		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_aBulletTotalNum[2];

		DatBuf[2] = DatBufLen - 3;
		DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;
		Wtn6_Play(WTN6_ReplenishAmmunition); 
		ChanParam.m_eChan = CHAN_ZIGBEE;
		ChanParam.m_pDstAddr = pSlaverMag->m_aWMac;
		ChanParam.m_byDstAddrLen = 8;
		ChanParam.m_pDatBuf = DatBuf;
		ChanParam.m_wDatBufLen = DatBufLen;
		ChanParam.m_dwRepeatCnt = 10; 
		ChanParam.m_dwScheTime = 200; 

		InterChanSend(&ChanParam);
		return;		
}

void InterConn_ProtDeal_Slaver_AmmuSupply_Init(void)
{
		S_CONN_CHAN_PARAM ChanParam;
		SINGLE_SLAVER_MAG *pSlaverMag;
		uint8_t *pRetMac = ZigComTask_LastCommMacGet();
		uint8_t DatBuf[50];
    uint8_t w = 0, b = 0, DatBufLen;
		uint32_t Ammu_RemainNum = 0;
	
		pSlaverMag = SlaverMag_GetHandleByMac(pRetMac,8);  

		if(NULL != pSlaverMag)
		{
			if(pSlaverMag->m_byTType == 0xFE)
				return;
				
			for(w = 0; w <= UserParam_GetSlaverCnt(); w++)
			{
				if(pSlaverMag->m_byTType == g_sUserParam.m_saAmmuInfo[w].m_byWeapon_Type
					&& pSlaverMag->m_byTSubType == g_sUserParam.m_saAmmuInfo[w].m_byWeapon_SubType)
					break;
			}
			if(w > UserParam_GetSlaverCnt())
				return;
		}
		else
			return;
		
		if(pSlaverMag->m_byIsSleepFlg == FALSE)
		{
			for(b = 0; b < g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind; b++)
			{
				Ammu_RemainNum = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[0] << 16
											 | g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[1] << 8
											 | g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[2];
				
				if(Ammu_RemainNum == 0)
				{	
					break;
				}
			}
		}
		else
			return;
		
		if(b >= g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind)
			return;
		
		if(g_sUserParam.m_saAmmuInfo[w].UnLockFlag != FALSE)
			return;
		
		DatBufLen = 0;
		DatBuf[DatBufLen++] = 0xA2;
		DatBuf[DatBufLen++] = 0xAA;
		DatBuf[DatBufLen++] = 0;
		DatBuf[DatBufLen++] = IDParam_GetTType();
		DatBuf[DatBufLen++] = IDParam_GetTSubType();
		DatBuf[DatBufLen++] = IDParam_GetRid();
		DatBuf[DatBufLen++] = IDParam_GetPid() >> 8;
		DatBuf[DatBufLen++] = IDParam_GetPid() & 0xFF;

		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_Type;
		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_SubType;
		DatBuf[DatBufLen++] = 0x01;

		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum[0];
		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum[1];
		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum[2];

		DatBuf[2] = DatBufLen - 3;
		DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;
		Wtn6_Play(WTN6_ReplenishAmmunition); 
		ChanParam.m_eChan = CHAN_ZIGBEE;
		ChanParam.m_pDstAddr = ZigComTask_LastCommMacGet();
		ChanParam.m_byDstAddrLen = 8;
		ChanParam.m_pDatBuf = DatBuf;
		ChanParam.m_wDatBufLen = DatBufLen;
		ChanParam.m_dwRepeatCnt = 10; 
		ChanParam.m_dwScheTime = 100; 

		InterChanSend(&ChanParam);
		return;		
}

//A3 AA
uint32_t InterConn_ProtDeal_Slaver_AmmuSupplyRet(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
	  uint8_t Offset, TType, TSubtype, RID, AType, ASubtype, Weaponsta;
		uint8_t AmmuNum[3]; 
		uint8_t DatBuf[50];
    uint8_t w = 0, b = 0, DatBufLen;

		uint16_t PID;
		uint32_t Cnum, Rnum, Magic_ID;
	
	  S_CONN_CHAN_PARAM ChanParam;
    SINGLE_SLAVER_MAG *pSlaverMag;
    SCOMTASK_MAG *pScomTaskMag;

    //�������У��
    if(pBuf == NULL || BufLen < 9 )
    {
        return FALSE;
    }

    Offset = 3;
    TType = pBuf[Offset++];
    TSubtype = pBuf[Offset++];
    RID = pBuf[Offset++];
    PID = (uint16_t)pBuf[Offset] << 8 | pBuf[Offset + 1];
		Offset += 2;
		
    AType = pBuf[Offset++];
		ASubtype = pBuf[Offset++];
		
		Weaponsta = pBuf[Offset++];
		memcpy(AmmuNum, &pBuf[Offset], 3);
		Offset += 3;
		
		pSlaverMag = SlaverMag_GetHandleByMac(pDstAddr,8);

		if(NULL != pSlaverMag)
		{
			pSlaverMag->m_dwLastCommTimeMs = GetSysRunMilliSecond();
		}
		else
			return FALSE;

		if(pSlaverMag->m_byIsOnline == TRUE)
		{
			w = pSlaverMag->m_byWeaponIdx;
			for(b = 0; b < g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind; b++)
			{
				if((AType == g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_Type)
				&&(ASubtype == g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_SubType))
					break;
			}
			if(b >= g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind)
					return FALSE;
		}
		else
			return FALSE;

		g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[0] = AmmuNum[0];
		g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[1] = AmmuNum[1];
		g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[2] = AmmuNum[2];

		g_sUserParam.m_saAmmuInfo[w].UnLockFlag = TRUE;
		pSlaverMag->m_byIsSleepFlg = FALSE;
		
		if(pSlaverMag->m_byTType == 0x14)
		{
			PJP_BulletInfo_Sync(pSlaverMag);
		}
		//Wtn6_Play(WTN6_ReplenishAmmunition);
		
		Magic_ID = 0;
		Magic_ID |= (0xAA << 24) ;
    Magic_ID |= (pDstAddr[5] << 16);
    Magic_ID |= (pDstAddr[6] << 8);
    Magic_ID |= (pDstAddr[7] << 0);
		pScomTaskMag = ZigComTask_TaskHandleGet();
    SComTask_SendListDeleteByFrameId(pScomTaskMag->m_pSendList,Magic_ID);  
		
		return TRUE;
}

/****************************************************************
@FUNCTION���º�̨�����ֻ����ڷ����¼��������
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��MTY
@SPECIAL:
****************************************************************/
void NonDigitIndirectLanuchEventData_Spell(StructEvent_NonDigitIndirectLanuch *pMag, u8 *pBuf, u8 *Len)
{
    u8 num = 0;

    pBuf[num++] = BNEventType_NonDigitIndirectLanuch;
    pBuf[num++] = pMag->m_byT_Type;
    pBuf[num++] = pMag->m_byT_Subtype;
    pBuf[num++] = pMag->m_byRID;
    pBuf[num++] = pMag->m_aPID[0];
    pBuf[num++] = pMag->m_aPID[1];
    pBuf[num++] = pMag->m_byC_Idegree;
    memcpy(&pBuf[num], &pMag->m_sL_Position, sizeof(STRUCT_LPOS));
    num += sizeof(STRUCT_LPOS);
    memcpy(&pBuf[num], &pMag->m_sTime, sizeof(STRUCT_TIME1));
    num += sizeof(STRUCT_TIME1);
    pBuf[num++] = pMag->m_byA_Type;
    pBuf[num++] = pMag->m_byA_Subtype;
    pBuf[num++] = pMag->m_byG_Num;
    pBuf[num++] = pMag->m_byAmmunitionTemperature;
    pBuf[num++] = pMag->m_byIndirectTSpan_Select;
    pBuf[num++] = pMag->m_byIndirectT_Span;
    pBuf[num++] = pMag->m_byVelocity2;
    pBuf[num++] = pMag->m_byA_AmmuNum;
    pBuf[num++] = pMag->m_aArtilleryShootAngle[0];
    pBuf[num++] = pMag->m_aArtilleryShootAngle[1];
    pBuf[num++] = pMag->m_aArtilleryShootdirection[0];
    pBuf[num++] = pMag->m_aArtilleryShootdirection[1];
    pBuf[num++] = pMag->m_aGroundAirPressure[0];
    pBuf[num++] = pMag->m_aGroundAirPressure[1];
    pBuf[num++] = pMag->m_aGroundWindDirection[0];
    pBuf[num++] = pMag->m_aGroundWindDirection[1];
    pBuf[num++] = pMag->m_byGroundTemperature;

    *Len = num;
}

/****************************************************************
@FUNCTION���º�̨�����ֻ����ڷ����¼��ϴ�
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��MTY
@SPECIAL:
****************************************************************/
void NonDigitIndirectLanuchEventUpload_Send(Struct_EventUpload *pMag)
{
		RESCOMTASK_MAG *pMag1 = &g_sResComTask_Mag;
		FRAME_BUF *pFrame = NULL;
		
		uint8_t cut = 0,Len = 0;

		pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

		if(pFrame == NULL)
		{
				return;
		}		
		
		memset(BackStage_dataBuf,0,sizeof(BackStage_dataBuf));						
		
		pMag->m_aData_Frame[0] = 0xA8;
		pMag->m_aData_Frame[1] = BNCmd_Terminal_EventUpload;

		pFrame->m_wBufLen = 0;
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[1];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
		
		NonDigitIndirectLanuchEventData_Spell(&pMag->Event_Pack.m_sNonDigitIndirectLanuch, BackStage_dataBuf, &Len);
    memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen],BackStage_dataBuf,Len);
		pFrame->m_wBufLen += Len;
		
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[1];

		//���¼���

		pMag->m_byXor = 0;
		for(cut = 3 ;cut < pFrame->m_wBufLen - 2; cut++)
		{
			pMag->m_byXor ^= pFrame->m_byBuf[cut];
		}
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_byXor;
		
		pFrame->m_byBuf[2] = pFrame->m_wBufLen - 6;
		
		pFrame->m_dwCnt = 1;
		pFrame->m_dwID = (pFrame->m_byBuf[0] << 24 | pFrame->m_byBuf[1] << 16 |
											pFrame->m_byBuf[2] << 8 | pFrame->m_byBuf[3]);
		pFrame->m_dwScheTime = 1000;
		pFrame->m_dwTime = GetSysRunMilliSecond();
		pFrame->m_bySendNum = 0;
		SComTask_SendListAppend(pMag1->m_sTaskMag.m_pSendList, (void*)pFrame);
		g_sResComTask_Mag.EvtListResPonseNum[pMag->m_aES_Num[0] << 8 | pMag->m_aES_Num[1]] = TRUE;
	
		if(g_sUserParam.m_byMode == GAME_GOING)
		{
			g_sResComTask_Mag.EvtRspFlg[g_sResComTask_Mag.m_dwCtrlEvtNum / 8] |= (1 << (g_sResComTask_Mag.m_dwCtrlEvtNum % 8));
		
			if(g_sResComTask_Mag.m_dwCtrlEvtNum < 240)
			{
				EventStorageToFlash(pFrame, sizeof(FRAME_BUF), g_sResComTask_Mag.m_dwCtrlEvtNum);
			}
		}
}

/****************************************************************
@FUNCTION���º�̨���ֻ����ڷ����¼��������
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��LW
@SPECIAL:
****************************************************************/
void DigitIndirectLanuchEventData_Spell(StructEvent_DigitIndirectLanuch *pMag, u8 *pBuf, u8 *Len)
{
    u8 num = 0;

    pBuf[num++] = BNEventType_DigitIndirectLanuch;
    pBuf[num++] = pMag->m_byT_Type;
    pBuf[num++] = pMag->m_byT_Subtype;
    pBuf[num++] = pMag->m_byRID;
    pBuf[num++] = pMag->m_aPID[0];
    pBuf[num++] = pMag->m_aPID[1];
    pBuf[num++] = pMag->m_byC_Idegree;

    memcpy(&pBuf[num], &pMag->m_sL_Position, sizeof(STRUCT_LPOS));
    num += sizeof(STRUCT_LPOS);
    memcpy(&pBuf[num], &pMag->m_sTime, sizeof(STRUCT_TIME1));
    num += sizeof(STRUCT_TIME1);

    pBuf[num++] = pMag->m_byA_Type;
    pBuf[num++] = pMag->m_byA_Subtype;
    pBuf[num++] = pMag->m_byG_Num;
    pBuf[num++] = pMag->m_byAmmunitionTemperature;
    pBuf[num++] = pMag->m_byTSpan_Select;
    pBuf[num++] = pMag->m_T_SpanX;
    pBuf[num++] = pMag->m_byVelocity2;
    pBuf[num++] = pMag->m_byA_AmmuNum;
    pBuf[num++] = pMag->m_byR_Mode;

    memcpy(&pBuf[num], &pMag->m_sG_Position, sizeof(STRUCT_GPOS));
    num += sizeof(STRUCT_GPOS);

    *Len = num;

}

/****************************************************************
@FUNCTION���º�̨����Ա��װ���������ֻ����ڷ����¼��ϴ�
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��LW
@SPECIAL:
****************************************************************/
void DigitIndirectLanuchEventUpload_Send(Struct_EventUpload *pMag)
{
		RESCOMTASK_MAG *pMag1 = &g_sResComTask_Mag;
		FRAME_BUF *pFrame = NULL;
		
		uint8_t cut = 0,Len = 0;

		pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

		if(pFrame == NULL)
		{
				return;
		}		
		
		memset(BackStage_dataBuf,0,sizeof(BackStage_dataBuf));						
		
		pMag->m_aData_Frame[0] = 0xA8;
		pMag->m_aData_Frame[1] = BNCmd_Terminal_EventUpload_Bind;

		pFrame->m_wBufLen = 0;
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[1];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
		
		pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetTType();
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetTSubType();
		pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetRid();
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetPid() >> 8;
    pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetPid() & 0xff;
		DigitIndirectLanuchEventData_Spell(&pMag->Event_Pack.m_sDigitIndirectLanuch, BackStage_dataBuf, &Len);
    memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen],BackStage_dataBuf,Len);
		pFrame->m_wBufLen += Len;
		
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[1];

		//���¼���

		pMag->m_byXor = 0;
		for(cut = 8 ;cut < pFrame->m_wBufLen - 2; cut++)
		{
			pMag->m_byXor ^= pFrame->m_byBuf[cut];
		}
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_byXor;
		
		pFrame->m_byBuf[2] = pFrame->m_wBufLen - 6;
		
		pFrame->m_dwCnt = 1;
		pFrame->m_dwID = (pFrame->m_byBuf[0] << 24 | pFrame->m_byBuf[1] << 16 |
											pFrame->m_byBuf[2] << 8 | pFrame->m_byBuf[3]);
		pFrame->m_dwScheTime = 1000;
		pFrame->m_dwTime = GetSysRunMilliSecond();
		pFrame->m_bySendNum = 0;
		SComTask_SendListAppend(pMag1->m_sTaskMag.m_pSendList, (void*)pFrame);
		g_sResComTask_Mag.EvtListResPonseNum[pMag->m_aES_Num[0] << 8 | pMag->m_aES_Num[1]] = TRUE;
}

void PJPShotHandle(SOLDIERAMMU_T *Ammu)
{

		if(g_sVarious_Elements.m_byGS_or_ZY == 1) //�����ֻ�Э��
    {
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_byT_Type                = Ammu->m_byWeapon_Type;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_byT_Subtype             = Ammu->m_byWeapon_SubType;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_byRID                   = IDParam_GetRid();
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_aPID[0]                 = IDParam_GetPid() >> 8;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_aPID[1]                 = IDParam_GetPid() & 0xff;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_byC_Idegree             = GetInjuryDegree(g_sUserParam.m_byBlood);
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_sL_Position             = g_sUserParam.m_sLPosition;    //λ�ú͸߳�
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_sTime                   = g_sUserParam.m_sUtcTime;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_byA_Type                = Ammu->m_sBulletInfo[0].m_byBullet_Type;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_byA_Subtype             = Ammu->m_sBulletInfo[0].m_byBullet_SubType;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_byG_Num                 = g_sVarious_Elements.m_byG_Num;    //װҩ��
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_byAmmunitionTemperature = g_sVarious_Elements.m_byYaoTemperature;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_byIndirectTSpan_Select  = g_sVarious_Elements.m_byTspanSelect;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_byIndirectT_Span        = g_sVarious_Elements.m_byT_SpanX;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_byVelocity2             = g_sVarious_Elements.m_byPCVelocity2;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_byA_AmmuNum             = g_sVarious_Elements.m_byNum1;

        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_aArtilleryShootAngle[0] = g_sVarious_Elements.m_Angle1J[0];
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_aArtilleryShootAngle[1] = g_sVarious_Elements.m_Angle1J[1];
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_aArtilleryShootdirection[0] = g_sVarious_Elements.m_Angle1X[0];
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_aArtilleryShootdirection[1] = g_sVarious_Elements.m_Angle1X[1];
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_aGroundAirPressure[0]   = g_sVarious_Elements.m_Pressure[0];
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_aGroundAirPressure[1]   = g_sVarious_Elements.m_Pressure[1];
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_aGroundWindDirection[0] = g_sVarious_Elements.m_Angle2[0];
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_aGroundWindDirection[1] = g_sVarious_Elements.m_Angle2[1];
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_aGroundWindVelocity1 = g_sVarious_Elements.m_ZDDMVelocity1;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_byGroundTemperature = g_sVarious_Elements.m_ZDTemperature;

        NonDigitIndirectLanuchEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
    }
		else
		{
        //�͵����󶨣��Ȼ���ģ�ⵯû����ǰ�����յ���װ��״̬�ϴ���A8��92����һ���Ȼ���ģ�ⵯ�����ϣ�״̬�ϴ���A8��94���¼��ϴ���A8��95��
        //�Ȼ������ֻ����ڸ��ð��¼��ϴ������ֻ����ڷ����¼��������ڵ���ʵ������Ϊ0x14��ʵ��������Ϊ�����ڻ��ڣ�ȡֵ��Χ1-5.
				g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_byT_Type    = Ammu->m_byWeapon_Type;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_byT_Subtype = Ammu->m_byWeapon_SubType;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_byRID   = IDParam_GetRid();
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_aPID[0] = IDParam_GetPid() >> 8;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_aPID[1] = IDParam_GetPid() & 0xff;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_byC_Idegree = GetInjuryDegree(g_sUserParam.m_byBlood);
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_sL_Position = g_sUserParam.m_sLPosition;    //λ�ú͸߳�
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_sTime       = g_sUserParam.m_sUtcTime;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_byA_Type    = Ammu->m_sBulletInfo[0].m_byBullet_Type;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_byA_Subtype = Ammu->m_sBulletInfo[0].m_byBullet_SubType;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_byG_Num                 = g_sVarious_Elements.m_byG_Num;    //װҩ��
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_byAmmunitionTemperature = g_sVarious_Elements.m_byYaoTemperature;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_byTSpan_Select          = g_sVarious_Elements.m_byTspanSelect;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_T_SpanX                 = g_sVarious_Elements.m_byT_SpanX;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_byVelocity2             = g_sVarious_Elements.m_byPCVelocity2;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_byA_AmmuNum             = g_sVarious_Elements.m_byNum1;

        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_byR_Mode = g_sVarious_Elements.m_byRmode;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_sG_Position.m_byGS_X[0] = g_sVarious_Elements.m_byT_GUASS_X[0];
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_sG_Position.m_byGS_X[1] = g_sVarious_Elements.m_byT_GUASS_X[1];
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_sG_Position.m_byGS_X[2] = g_sVarious_Elements.m_byT_GUASS_X[2];
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_sG_Position.m_byGS_Y[0] = g_sVarious_Elements.m_byT_GUASS_Y[0];
				g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_sG_Position.m_byGS_Y[1] = g_sVarious_Elements.m_byT_GUASS_Y[1];
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_sG_Position.m_byGS_Y[2] = g_sVarious_Elements.m_byT_GUASS_Y[2];
				g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_sG_Position.m_byGS_Y[3] = g_sVarious_Elements.m_byT_GUASS_Y[3];
				g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_sG_Position.m_byGS_Lenth[0] = g_sVarious_Elements.m_byT_GUASS_H[0];
				g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_sG_Position.m_byGS_Lenth[1] = g_sVarious_Elements.m_byT_GUASS_H[1];
				
				g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
        g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
        g_sResComTask_Mag.m_dwCtrlEvtNum++;
				
				DigitIndirectLanuchEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
		}
}

/****************************************************************
@FUNCTION���º�ֱ̨�������������¼��������
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��MTY
@SPECIAL:
****************************************************************/
void HeavyWpnLaunchEventData_Spell(StructEvent_HeavyWpnLaunch *pMag, u8 *pBuf, u8 *Len)
{
    u8 num = 0;

    pBuf[num++] = BNEventType_HeavyWpnLaunch;
    pBuf[num++] = pMag->m_byT_Type;
    pBuf[num++] = pMag->m_byT_Subtype;
    pBuf[num++] = pMag->m_byRID;
    pBuf[num++] = pMag->m_aPID[0];
    pBuf[num++] = pMag->m_aPID[1];
    pBuf[num++] = pMag->m_byC_Idegree;
    memcpy(&pBuf[num], &pMag->m_sL_Position, sizeof(STRUCT_LPOS));
    num += sizeof(STRUCT_LPOS);
    memcpy(&pBuf[num], &pMag->m_sTime, sizeof(STRUCT_TIME1));
    num += sizeof(STRUCT_TIME1);
    pBuf[num++] = pMag->m_byA_Type;
    pBuf[num++] = pMag->m_byA_Subtype;

    *Len = num;
}


/****************************************************************
@FUNCTION���º�ֱ̨�������������¼��ϴ�
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��MTY
@SPECIAL:
****************************************************************/
void HeavyWpnLaunchEventUpload_Send(Struct_EventUpload *pMag)
{
		RESCOMTASK_MAG *pMag1 = &g_sResComTask_Mag;
		FRAME_BUF *pFrame = NULL;
		
		uint8_t cut = 0,Len = 0;

		pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

		if(pFrame == NULL)
		{
				return;
		}		
		
		memset(BackStage_dataBuf,0,sizeof(BackStage_dataBuf));						
		
		pMag->m_aData_Frame[0] = 0xA8;
		pMag->m_aData_Frame[1] = BNCmd_Terminal_EventUpload;

		pFrame->m_wBufLen = 0;
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[1];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
		
		HeavyWpnLaunchEventData_Spell(&pMag->Event_Pack.m_sHeavyWpnLaunch, BackStage_dataBuf, &Len);
    memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen],BackStage_dataBuf,Len);
		pFrame->m_wBufLen += Len;
		
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[1];

		//���¼���

		pMag->m_byXor = 0;
		for(cut = 3; cut < pFrame->m_wBufLen - 2; cut++)
		{
			pMag->m_byXor ^= pFrame->m_byBuf[cut];
		}
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_byXor;
		
		pFrame->m_byBuf[2] = pFrame->m_wBufLen - 6;
		
		pFrame->m_dwCnt = 1;
		pFrame->m_dwID = (pFrame->m_byBuf[0] << 24 | pFrame->m_byBuf[1] << 16 |
											pFrame->m_byBuf[2] << 8 | pFrame->m_byBuf[3]);
		pFrame->m_dwScheTime = 1000;
		pFrame->m_dwTime = GetSysRunMilliSecond();
		pFrame->m_bySendNum = 0;
		SComTask_SendListAppend(pMag1->m_sTaskMag.m_pSendList, (void*)pFrame);
		g_sResComTask_Mag.EvtListResPonseNum[pMag->m_aES_Num[0] << 8 | pMag->m_aES_Num[1]] = TRUE;
}

void HeavyWpnLaunchEventUploadHandle(SOLDIERAMMU_T *Ammu, uint8_t Num)
{
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sHeavyWpnLaunch.m_byT_Type = Ammu->m_byWeapon_Type;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sHeavyWpnLaunch.m_byT_Subtype = Ammu->m_byWeapon_SubType;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sHeavyWpnLaunch.m_byRID = IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sHeavyWpnLaunch.m_aPID[0] = IDParam_GetPid() >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sHeavyWpnLaunch.m_aPID[1] = IDParam_GetPid() & 0xff;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sHeavyWpnLaunch.m_byC_Idegree 	= GetInjuryDegree(g_sUserParam.m_byBlood);
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sHeavyWpnLaunch.m_sL_Position 	= g_sUserParam.m_sLPosition;    //λ�ú͸߳�
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sHeavyWpnLaunch.m_sTime = g_sUserParam.m_sUtcTime;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sHeavyWpnLaunch.m_byA_Type = Ammu->m_sBulletInfo[Num].m_byBullet_Type;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sHeavyWpnLaunch.m_byA_Subtype = Ammu->m_sBulletInfo[Num].m_byBullet_SubType;

    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;

    HeavyWpnLaunchEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}

//A3 AB
uint32_t InterConn_ProtDeal_Slaver_AmmuStausRet(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
    uint8_t Offset, TType, TSubtype, RID, AType, ASubtype, Weaponsta;
		uint8_t Remain_Anum[3], Total_Anum[3]; 
		
		uint8_t DatBuf[64];
    uint8_t w = 0, b = 0, DatBufLen;

		uint16_t PID;
		uint32_t NowTime = GetSysRunMilliSecond();
    static uint32_t Remain_Aum = 0, Total_Aum = 0, Timer_ExhaustAmmunition = 0, DBHJT_DJTime = 0;
	
	  S_CONN_CHAN_PARAM ChanParam;
    SINGLE_SLAVER_MAG *pSlaverMag;
    SCOMTASK_MAG *pScomTaskMag;
	
    //�������У��
    if(pBuf == NULL || BufLen < 18 )
    {
        return FALSE;
    }

    Offset = 3;
    TType = pBuf[Offset++];
    TSubtype = pBuf[Offset++];
    RID = pBuf[Offset++];
    PID = (uint16_t)pBuf[Offset] << 8 | pBuf[Offset + 1];
		Offset += 2;
		
    AType = pBuf[Offset++];
		ASubtype = pBuf[Offset++];
		
		Weaponsta = pBuf[Offset++];
		memcpy(Remain_Anum, &pBuf[Offset], 3);
		Offset += 3;
		memcpy(Total_Anum, &pBuf[Offset], 3);
		Offset += 3;

		pSlaverMag = SlaverMag_GetHandleByMac(pDstAddr,8);
		
		if(NULL == pSlaverMag)
			return FALSE;

		w = pSlaverMag->m_byWeaponIdx;
		
		if(pSlaverMag->m_byIsSleepFlg == FALSE)
		{
			for(b = 0; b < g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind; b++)
			{
				if((AType == g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_Type)
				&&(ASubtype == g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_SubType))
				{	
					break;
				}
			}
			if(b >= g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind)
					return FALSE;
			
			g_sUserParam.m_saAmmuInfo[w].m_byState = Weaponsta;
			
			if(pSlaverMag->m_byTType == 0x41 || pSlaverMag->m_byTType == 0x42 || pSlaverMag->m_byTType == 0x25)
			{
				g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum[0] = Total_Anum[0];
				g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum[1] = Total_Anum[1];
				g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum[2] = Total_Anum[2];
			}
			
			g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[0] = Remain_Anum[0];
			g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[1] = Remain_Anum[1];
			g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[2] = Remain_Anum[2];

			pSlaverMag->m_byShootingFlag = 0;
			pSlaverMag->m_dwLastCommTimeMs = NowTime;
			
			DatBufLen = 0;
			DatBuf[DatBufLen++] = 0xA2;
			DatBuf[DatBufLen++] = 0xAB;
			DatBuf[DatBufLen++] = 0;
			DatBuf[DatBufLen++] = IDParam_GetTType();
			DatBuf[DatBufLen++] = IDParam_GetTSubType();
			DatBuf[DatBufLen++] = IDParam_GetRid();
			DatBuf[DatBufLen++] = IDParam_GetPid() >> 8;
			DatBuf[DatBufLen++] = IDParam_GetPid() & 0xFF;
			
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_Type;
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_SubType;
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_byState;

			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[0];
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[1];
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[2];
			
			DatBuf[DatBufLen++] = Total_Anum[0];//g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum[0];
			DatBuf[DatBufLen++] = Total_Anum[1];//g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum[1];
			DatBuf[DatBufLen++] = Total_Anum[2];//g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum[2];

			DatBuf[2] = DatBufLen - 3;
			DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
			DatBufLen++;
			 
			ChanParam.m_eChan = CHAN_ZIGBEE;
			ChanParam.m_pDstAddr = pDstAddr;
			ChanParam.m_byDstAddrLen = 8;
			ChanParam.m_pDatBuf = DatBuf;
			ChanParam.m_wDatBufLen = DatBufLen;
			ChanParam.m_dwRepeatCnt = 10; 
			ChanParam.m_dwScheTime = 100; 

			InterChanSend(&ChanParam);	
			
			if(g_sVarious_Elements.m_byPJPLoactionFlag == TRUE 
						&& pSlaverMag->m_byTType == 0x14)
			{
				g_sVarious_Elements.m_byPJPLoactionFlag = FALSE;
				PJPShotHandle(&g_sUserParam.m_saAmmuInfo[w]);
				PJP_BulletInfo_Sync(pSlaverMag);
			}
			
			//��̨�����ϴ�
			if(pSlaverMag->m_byTType == 0x40)
			{
					HeavyWpnLaunchEventUploadHandle(&g_sUserParam.m_saAmmuInfo[w], b); //ֻ�е������������Ž�������¼��ϴ�
			}
						
			if(TType != 0x41 && TType != 0x42 && TType != 0x25)
			{
				Remain_Aum = 0x00 << 24 | Remain_Anum[0] << 16 | Remain_Anum[1] << 8 | (Remain_Anum[2] & 0xff);
				Total_Aum = 0x00 << 24 | Total_Anum[0] << 16 | Total_Anum[1] << 8 | (Total_Anum[2] & 0xff);

				if(g_sUserParam.m_sTsChar1.m_bLocked == FALSE && pSlaverMag->m_byIsOnline == TRUE)
				{
					if(pSlaverMag->AmmuLastRemainNum != Remain_Aum)
					{
							if(Remain_Aum != Total_Aum)
							{
									if(pSlaverMag->m_byTType == 0x31)
										Wtn6_StartLSound(WTN6_YX_Shoot2);
									else if(pSlaverMag->m_byTType == 0x32)
										Wtn6_StartLSound(WTN6_YX_JQ);
									else if(pSlaverMag->m_byTType == 0x34)
										Wtn6_StartLSound(WTN6_YX_JQ);
									else if(pSlaverMag->m_byTType == 0x35)
										Wtn6_StartLSound(WTN6_YX_Shoot2);
									else if(pSlaverMag->m_byTType == 0x36)
										Wtn6_StartLSound(WTN6_YX_Shoot1);
									else if(pSlaverMag->m_byTType == 0x37)
										Wtn6_StartLSound(WTN6_YX_Shoot2);
									else if(pSlaverMag->m_byTType == 0x38)
										Wtn6_StartLSound(WTN6_YX_ZJQ);
									else if(pSlaverMag->m_byTType == 0x39)
										Wtn6_StartLSound(WTN6_YX_JQ);
									else if(pSlaverMag->m_byTType == 0x14)
										Wtn6_StartLSound(WTN6_YX_ZDBZ2);
									else
										Wtn6_StartLSound(WTN6_YX_Shoot2);
							}
							pSlaverMag->AmmuLastRemainNum = Remain_Aum;
					}
					else if(pSlaverMag->m_byTType == 0x40)
					{
						if(g_sUserParam.m_saAmmuInfo[w].UnLockFlag == TRUE)
							Wtn6_StartLSound(WTN6_YX_ZDBZ2);
					}
				}
				
				if(Remain_Aum == 0)
				{
					if(NowTime - Timer_ExhaustAmmunition >= 2000)
					{
						g_sUserParam.m_saAmmuInfo[w].UnLockFlag = FALSE;
						if(pSlaverMag->m_byIsOnline == TRUE)
							Wtn6_Play(WTN6_ExhaustAmmunition);
						
						if(g_sUserParam.m_byMode == TRAIN)
						{
							InterConn_ProtDeal_Slaver_AmmuSupply(pSlaverMag);
						}
						Timer_ExhaustAmmunition = NowTime;
					}
				}
			}
			
			return TRUE;
		}
}

//A3 AC
uint32_t InterConn_ProtDeal_Slaver_WeaponShootingInquire(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
    uint8_t Offset, TType, TSubtype, RID, AType, ASubtype;
		uint8_t WeaponSta;
	
		uint8_t DatBuf[64];
    uint8_t w = 0, b = 0, DatBufLen;

		uint16_t PID;
		uint32_t NowTime = GetSysRunMilliSecond();
		static uint32_t LastTime = 0;
			
	  S_CONN_CHAN_PARAM ChanParam;
    SINGLE_SLAVER_MAG *pSlaverMag;
	
		if((NowTime - LastTime) <= 1000)
			return FALSE;
		
		LastTime = NowTime;
		
    //�������У��
    if(pBuf == NULL || BufLen < 10 )
    {
        return FALSE;
    }

    Offset = 3;
    TType = pBuf[Offset++];
    TSubtype = pBuf[Offset++];
    RID = pBuf[Offset++];
    PID = (uint16_t)pBuf[Offset] << 8 | pBuf[Offset + 1];
		Offset += 2;
		
    AType = pBuf[Offset++];
		ASubtype = pBuf[Offset++];

		pSlaverMag = SlaverMag_GetHandleByMac(pDstAddr,8);

		if(NULL == pSlaverMag)
			return FALSE;

		w = pSlaverMag->m_byWeaponIdx;
		
		if(pSlaverMag->m_byIsOnline == TRUE)
		{
			for(b = 0; b < g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind; b++)
			{
				if((AType == g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_Type)
				&&(ASubtype == g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_SubType))
				{	
					break;
				}
			}
			if(b >= g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind)
					return FALSE;	
			
			pSlaverMag->m_dwLastCommTimeMs = GetSysRunMilliSecond();
			
			pSlaverMag->m_byShootingMode = pBuf[Offset++];
			pSlaverMag->m_byShootingFlag = pBuf[Offset++];
			pSlaverMag->m_wPlayInterval = pBuf[Offset++];
			
			if(pSlaverMag->m_byTType != 0x14)
			{
				if(g_sUserParam.m_sTsChar1.m_bLocked == FALSE)
				{
						DatBufLen = 0;
						DatBuf[DatBufLen++] = 0xA2;
						DatBuf[DatBufLen++] = 0xAA;
						DatBuf[DatBufLen++] = 0;
						DatBuf[DatBufLen++] = IDParam_GetTType();
						DatBuf[DatBufLen++] = IDParam_GetTSubType();
						DatBuf[DatBufLen++] = IDParam_GetRid();
						DatBuf[DatBufLen++] = IDParam_GetPid() >> 8;
						DatBuf[DatBufLen++] = IDParam_GetPid() & 0xFF;

						DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_Type;
						DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_SubType;
						DatBuf[DatBufLen++] = 0x01;

						DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[0];
						DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[1];
						DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[2];

						DatBuf[2] = DatBufLen - 3;
						DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
						DatBufLen++;
						 
						ChanParam.m_eChan = CHAN_ZIGBEE;
						ChanParam.m_pDstAddr = pSlaverMag->m_aWMac;
						ChanParam.m_byDstAddrLen = 8;
						ChanParam.m_pDatBuf = DatBuf;
						ChanParam.m_wDatBufLen = DatBufLen;
						ChanParam.m_dwRepeatCnt = 5; 
						ChanParam.m_dwScheTime = 100; 

						InterChanSend(&ChanParam);
				}
				else
				{
					InterConn_ProtDeal_Slaver_Weapon_State(TRUE);
					return FALSE;
				}
				
				if(pSlaverMag->m_byIsSleepFlg == FALSE)
				{
					DatBufLen = 0;
					DatBuf[DatBufLen++] = 0xA2;
					DatBuf[DatBufLen++] = 0xAC;
					DatBuf[DatBufLen++] = 0;
					DatBuf[DatBufLen++] = TType;
					DatBuf[DatBufLen++] = TSubtype;
					DatBuf[DatBufLen++] = RID;
					DatBuf[DatBufLen++] = PID >> 8;
					DatBuf[DatBufLen++] = PID & 0xFF;
					
					DatBuf[DatBufLen++] = AType;
					DatBuf[DatBufLen++] = ASubtype;
					
					DatBuf[DatBufLen++] = pSlaverMag->m_byShootingMode;
					DatBuf[DatBufLen++] = pSlaverMag->m_byShootingFlag;
					DatBuf[DatBufLen++] = pSlaverMag->m_wPlayInterval;

					DatBuf[2] = DatBufLen - 3;
					DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
					DatBufLen++;
					 
					ChanParam.m_eChan = CHAN_ZIGBEE;
					ChanParam.m_pDstAddr = pDstAddr;
					ChanParam.m_byDstAddrLen = 8;
					ChanParam.m_pDatBuf = DatBuf;
					ChanParam.m_wDatBufLen = DatBufLen;
					ChanParam.m_dwRepeatCnt = 5; 
					ChanParam.m_dwScheTime = 100; 

					InterChanSend(&ChanParam);	
					
					return TRUE;
				}
				return FALSE;
			}
			else
			{
				if(g_sVarious_Elements.m_byPJPLoactionFlag == TRUE 
					&& g_sUserParam.m_sTsChar1.m_bLocked == FALSE)
				{
					DatBufLen = 0;
					DatBuf[DatBufLen++] = 0xA2;
					DatBuf[DatBufLen++] = 0xAC;
					DatBuf[DatBufLen++] = 0;
					DatBuf[DatBufLen++] = TType;
					DatBuf[DatBufLen++] = TSubtype;
					DatBuf[DatBufLen++] = RID;
					DatBuf[DatBufLen++] = PID >> 8;
					DatBuf[DatBufLen++] = PID & 0xFF;
					
					DatBuf[DatBufLen++] = AType;
					DatBuf[DatBufLen++] = ASubtype;
					
					DatBuf[DatBufLen++] = pSlaverMag->m_byShootingMode;
					DatBuf[DatBufLen++] = pSlaverMag->m_byShootingFlag;
					DatBuf[DatBufLen++] = pSlaverMag->m_wPlayInterval;

					DatBuf[2] = DatBufLen - 3;
					DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
					DatBufLen++;
					 
					ChanParam.m_eChan = CHAN_ZIGBEE;
					ChanParam.m_pDstAddr = pDstAddr;
					ChanParam.m_byDstAddrLen = 8;
					ChanParam.m_pDatBuf = DatBuf;
					ChanParam.m_wDatBufLen = DatBufLen;
					ChanParam.m_dwRepeatCnt = 5; 
					ChanParam.m_dwScheTime = 100; 

					InterChanSend(&ChanParam);	
					
					return TRUE;
				}
			}
		}
}

STRUCT_Various_Elements g_sVarious_Elements;

//�ϳ�Ӫ��ȡ��Ԫ��Ϣ
uint32_t InterConn_ProtDeal_SetUpVariousElements(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{  
		uint8_t TType, TSubtype, RID;
		uint8_t DatBuf[64];
    uint8_t DatBufLen, Offset;
		uint16_t PID;
	
	  S_CONN_CHAN_PARAM ChanParam;
	
		STRUCT_Various_Elements *Various_Elements = &g_sVarious_Elements; 
	
    //�������У��
    if(pBuf == NULL || BufLen < 11 )
    {
        return FALSE;
    }
    
		Offset = 3;
    TType = pBuf[Offset++];
    TSubtype = pBuf[Offset++];
    RID = pBuf[Offset++];
    PID = (uint16_t)pBuf[Offset] << 8 | pBuf[Offset + 1];
		Offset += 2;

    Various_Elements->m_byGS_or_ZY       = pBuf[Offset++];
    Various_Elements->m_byG_Num          = pBuf[Offset++];
    Various_Elements->m_byYaoTemperature = pBuf[Offset++];
    Various_Elements->m_byTspanSelect    = pBuf[Offset++];
    Various_Elements->m_byT_SpanX        = pBuf[Offset++];
    Various_Elements->m_byPCVelocity2    = pBuf[Offset++];
    Various_Elements->m_byNum1           = pBuf[Offset++];

    if(0 == Various_Elements->m_byGS_or_ZY)		//���ֻ�����˹��
    {
				Offset = 15;
        Various_Elements->m_byRmode = pBuf[Offset++];

        Various_Elements->m_byT_GUASS_X[0] = pBuf[Offset++];
        Various_Elements->m_byT_GUASS_X[1] = pBuf[Offset++];
        Various_Elements->m_byT_GUASS_X[2] = pBuf[Offset++];
        
				Offset = 20;
        Various_Elements->m_byT_GUASS_Y[0] = pBuf[Offset++];
        Various_Elements->m_byT_GUASS_Y[1] = pBuf[Offset++];
        Various_Elements->m_byT_GUASS_Y[2] = pBuf[Offset++];
        Various_Elements->m_byT_GUASS_Y[3] = pBuf[Offset++];
        
        Various_Elements->m_byT_GUASS_H[0] = 0;
        Various_Elements->m_byT_GUASS_H[1] = 0;
    }
    else //�����ֻ�����Ԫ��
    {
				Offset = 22;
        Various_Elements->m_Angle1J[0]  = pBuf[Offset++];
        Various_Elements->m_Angle1J[1]  = pBuf[Offset++];
        Various_Elements->m_Angle1X[0]  = pBuf[Offset++];
        Various_Elements->m_Angle1X[1]  = pBuf[Offset++];
        Various_Elements->m_Pressure[0] = pBuf[Offset++];
        Various_Elements->m_Pressure[1] = pBuf[Offset++];
        Various_Elements->m_Angle2[0]   = pBuf[Offset++];
        Various_Elements->m_Angle2[1]   = pBuf[Offset++];
        Various_Elements->m_ZDDMVelocity1 = pBuf[Offset++];
        Various_Elements->m_ZDTemperature = pBuf[Offset++];
    }
		
		Beep_On(&g_sBeepMag, OPT_MODE_NONBLOCK, 400);
		Wtn6_Play(WTN6_LOCATION_SUCCESS);

		g_sVarious_Elements.m_byPJPLoactionFlag = TRUE;
		
		return TRUE;
		//�ذ�  
		/*
		DatBufLen = 0;
		DatBuf[DatBufLen++] = 0xA2;
		DatBuf[DatBufLen++] = 0xB3;
		DatBuf[DatBufLen++] = 0;

		for(uint8_t i = 3; i < BufLen - 4; i++)
		{
				DatBuf[DatBufLen++] = pBuf[i];
		}
		
		DatBuf[2] = DatBufLen - 3;
		DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;
		
		ChanParam.m_eChan = CHAN_ZIGBEE;
		ChanParam.m_pDstAddr = pDstAddr;
		ChanParam.m_byDstAddrLen = 8;
		ChanParam.m_pDatBuf = DatBuf;
		ChanParam.m_wDatBufLen = DatBufLen;
		ChanParam.m_dwRepeatCnt = 5; 
		ChanParam.m_dwScheTime = 100; 

		InterChanSend(&ChanParam);
		return TRUE;
		*/
}

uint32_t InterConn_ProtDeal_Rescure_State(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
		uint8_t TType, TSubtype, RID, Rescue_Character;
		uint8_t DatBuf[64];
    uint8_t DatBufLen, Offset;
		uint16_t PID;
	
	  S_CONN_CHAN_PARAM ChanParam;
	
		//�������У��
    if(pBuf == NULL || BufLen < 10 )
    {
        return FALSE;
    }
    
		Offset = 3;
    TType = pBuf[Offset++];
    TSubtype = pBuf[Offset++];
    RID = pBuf[Offset++];
    PID = (uint16_t)pBuf[Offset] << 8 | pBuf[Offset + 1];
		Offset += 2;

		/*
		//�Ƿ��Ƿ����Լ�
    if(TRUE != IDParam_IsIDValid(TType, TSubtype, RID, PID))
    {
        return FALSE;
    }
		*/
		
		Rescue_Character = pBuf[Offset++];

		if(Rescue_Character == 1)
		{
				g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_sRescueStartTime = g_sUserParam.m_sUtcTime;
				Wtn6_Play(WTN6_Aiding);
				return FALSE;
		}
		
		g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_sRescueEndTime = g_sUserParam.m_sUtcTime;
		g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_byR_RID = RID;
		g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_aR_PID[0] = PID >> 8;
		g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_aR_PID[1] = PID & 0xff;
				
		g_sUserParam.m_byAidEndFlag1 = TRUE;
		return TRUE;
}

//A3 B8
uint32_t InterConn_ProtDeal_Slaver_Lightweapon_AmmuInfoSync(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
		uint8_t Offset, TType, TSubtype;
		uint8_t DatBuf[255];
    uint8_t w = 0, b = 0, DatBufLen, Remain_Ammu = 0, Total_AmmuNum[3] = {0};

	  S_CONN_CHAN_PARAM ChanParam;
    SINGLE_SLAVER_MAG *pSlaverMag;
    SCOMTASK_MAG *pScomTaskMag;

    //�������У��
    if(pBuf == NULL || BufLen < 9 )
    {
        return FALSE;
    }

    Offset = 3;
    TType = pBuf[Offset++];
    TSubtype = pBuf[Offset++];

		pSlaverMag = SlaverMag_GetHandleByMac(pDstAddr,8);  

		if(NULL == pSlaverMag)
		{
			return FALSE;
		}

		w = pSlaverMag->m_byWeaponIdx;
		g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind = pBuf[Offset++];

		if(g_sUserParam.m_byMode == TRAIN)
		{
				if(pSlaverMag->m_byTType == TType && pSlaverMag->m_byTSubType == TSubtype)
				{
					for(b = 0; b < g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind; b++)
					{
						g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_Type = pBuf[Offset++];
						g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_SubType = pBuf[Offset++];
						g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum[0] = pBuf[Offset++];
						g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum[1] = pBuf[Offset++];
						g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum[2] = pBuf[Offset++];
						//memcpy(Total_AmmuNum, &pBuf[Offset], 3);
						//Offset+=3;
						
						g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[0] = pBuf[Offset++];
						g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[1] = pBuf[Offset++];
						g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[2] = pBuf[Offset++];
						
						g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuWearDepth = pBuf[Offset++];
						g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuConcreteDepth = pBuf[Offset++];
						g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuRange = pBuf[Offset++];
					}
				}
				Remain_Ammu = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[0] << 16 |
											g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[1] << 8 |
											g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[2];

				if(Remain_Ammu != 0)
					g_sUserParam.m_saAmmuInfo[w].UnLockFlag = TRUE;
		}
		pSlaverMag->m_dwLastCommTimeMs = GetSysRunMilliSecond();
		
		DatBufLen = 0;
		DatBuf[DatBufLen++] = 0xA2;
		DatBuf[DatBufLen++] = 0xB8;
		DatBuf[DatBufLen++] = 0;
		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_byWeapon_Type;
		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_byWeapon_SubType;
		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind;
		for(b = 0; b < g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind; b++)
		{
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_Type;
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_SubType;
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum[0];//Total_AmmuNum[0];
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum[1];//Total_AmmuNum[1];
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum[2];//Total_AmmuNum[2];

			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[0];
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[1];
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[2];

			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuWearDepth;
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuConcreteDepth;
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuRange;
		}
		DatBuf[2] = DatBufLen - 3;
		DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;
		 
		ChanParam.m_eChan = CHAN_ZIGBEE;
		ChanParam.m_pDstAddr = pDstAddr;
		ChanParam.m_byDstAddrLen = 8;
		ChanParam.m_pDatBuf = DatBuf;
		ChanParam.m_wDatBufLen = DatBufLen;
		ChanParam.m_dwRepeatCnt = 1; 
		ChanParam.m_dwScheTime = 100; 

		InterChanSend(&ChanParam);	
		
		return TRUE;
}

uint32_t InterConn_ProtDeal_Slaver_Heavyweapon_AmmuInfoSync(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
		uint8_t Offset, TType, TSubtype;
		uint8_t DatBuf[128];
    uint8_t w = 0, b = 0, DatBufLen;

	  S_CONN_CHAN_PARAM ChanParam;
    SINGLE_SLAVER_MAG *pSlaverMag;
    SCOMTASK_MAG *pScomTaskMag;

    //�������У��
    if(pBuf == NULL || BufLen < 9 )
    {
        return FALSE;
    }

    Offset = 3;
    TType = pBuf[Offset++];
    TSubtype = pBuf[Offset++];
		
		pSlaverMag = SlaverMag_GetHandleByMac(pDstAddr,8);
		
		if(NULL == pSlaverMag || pSlaverMag->m_byIsSleepFlg == TRUE)
		{
			return FALSE;
		}

		w = pSlaverMag->m_byWeaponIdx;
		
		g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind = pBuf[Offset++];
		
		if(pSlaverMag->m_byTType == TType && pSlaverMag->m_byTSubType == TSubtype)
		{
			for(b = 0; b < g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind; b++)
			{
				g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_Type = pBuf[Offset++];
				g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_SubType = pBuf[Offset++];
				g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum[0] = pBuf[Offset++];
				g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum[1] = pBuf[Offset++];
				g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum[2] = pBuf[Offset++];

				g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[0] = pBuf[Offset++];
				g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[1] = pBuf[Offset++];
				g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[2] = pBuf[Offset++];

				g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuHitRate = pBuf[Offset++];
				g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuAngle = pBuf[Offset++];
				g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuSpeed = pBuf[Offset++];
				g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuWearDepth = pBuf[Offset++];
				g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuConcreteDepth = pBuf[Offset++];
				g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuRange = pBuf[Offset++];
			}
		}
		
		pSlaverMag->m_dwLastCommTimeMs = GetSysRunMilliSecond();
		
		DatBufLen = 0;
		DatBuf[DatBufLen++] = 0xA2;
		DatBuf[DatBufLen++] = 0xB9;
		DatBuf[DatBufLen++] = 0;
		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_byWeapon_Type;
		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_byWeapon_SubType;
		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind;
		for(b = 0; b < g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind; b++)
		{
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_Type;
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_SubType;
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum[0];
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum[1];
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum[2];

			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[0];
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[1];
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[2];

			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuHitRate;
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuAngle;
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuSpeed;
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuWearDepth;
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuConcreteDepth;
			DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byAmmuRange;
		}
		DatBuf[2] = DatBufLen - 3;
		DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;
		 
		ChanParam.m_eChan = CHAN_ZIGBEE;
		ChanParam.m_pDstAddr = pDstAddr;
		ChanParam.m_byDstAddrLen = 8;
		ChanParam.m_pDatBuf = DatBuf;
		ChanParam.m_wDatBufLen = DatBufLen;
		ChanParam.m_dwRepeatCnt = 1; 
		ChanParam.m_dwScheTime = 100; 
		
		InterChanSend(&ChanParam);	
		
		return TRUE;
}

//��ʱ�ϴ�����״̬
void InterConn_ProtDeal_SelfStatue(void)
{
		STRUCT_TS_Character1 Char1;
	  S_CONN_CHAN_PARAM ChanParam;
    SINGLE_SLAVER_MAG *pSlaverMag;
    SCOMTASK_MAG *pScomTaskMag;
    
		uint8_t DatBuf[90];
    uint8_t DatBufLen, i, w, b, WeaponNum = 0;
		uint32_t Rnum = 0, Snum = 0, Tnum = 0;

		for(i = 0; i < UserParam_GetSlaverCnt(); i++)
		{
				pSlaverMag = UserParam_GetSlaverMag(i);

				if(pSlaverMag->m_byIsOnline == FALSE)
					continue;

				DatBufLen = 0;
				DatBuf[DatBufLen++] = 0xA2;
				DatBuf[DatBufLen++] = 0xB0;
				DatBuf[DatBufLen++] = 0;
				DatBuf[DatBufLen++] = IDParam_GetTType();
				DatBuf[DatBufLen++] = IDParam_GetTSubType();
				DatBuf[DatBufLen++] = IDParam_GetRid();
				DatBuf[DatBufLen++] = IDParam_GetPid() >> 8;
				DatBuf[DatBufLen++] = IDParam_GetPid() &0xff;
				DatBuf[DatBufLen++] = UserParam_GetKey();
				
				memcpy(&DatBuf[DatBufLen], &g_sUserParam.m_sLPosition, sizeof(STRUCT_LPOS));
				DatBufLen += sizeof(STRUCT_LPOS);
				
				memcpy(&DatBuf[DatBufLen], &g_sUserParam.m_sUtcTime, sizeof(STRUCT_TIME1));
				DatBufLen += sizeof(STRUCT_TIME1);
				
				DatBuf[DatBufLen++] = GetInjuryDegree(g_sUserParam.m_byBlood);		//�ۺ�����
								
				Char1.m_bLocked          = g_sUserParam.m_sTsChar1.m_bLocked;
				Char1.m_bConnectionState = g_sUserParam.m_sTsChar1.m_bConnectionState;		//����������״̬0������	1���쳣
				Char1.m_bFailureState    = g_sUserParam.m_sTsChar1.m_bFailureState;		//0���޹���	1���й���
				Char1.m_bUnitmode		  	 = g_sUserParam.m_sTsChar1.m_bUnitmode;		//0��ѵ��ģʽ	1����ϰģʽ
				Char1.m_bBatteryState	   = g_sUserParam.m_sTsChar1.m_bBatteryState;		//0����������	1��Ƿ��
				Char1.m_bIdentity     	 = g_sUserParam.m_sTsChar1.m_bIdentity;		//��� ENUM_BRCHAR
				Char1.m_bIsNight 				 = g_sUserParam.m_sTsChar1.m_bIsNight;
				UserParam_SetTsChar1(Char1);
				memcpy(&DatBuf[DatBufLen],&Char1,sizeof(STRUCT_TS_Character1));
				DatBufLen += sizeof(STRUCT_TS_Character1);
				
				DatBuf[DatBufLen++] = g_sMpu6050Mag.Pose;
				
				DatBuf[DatBufLen++] = g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_aBulletMaxAmmuNum[0];
				DatBuf[DatBufLen++] = g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_aBulletMaxAmmuNum[1];
				DatBuf[DatBufLen++] = g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_aBulletMaxAmmuNum[2];
				
				DatBuf[DatBufLen++] = g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_aBulletMaxAmmuNum[0];
				DatBuf[DatBufLen++] = g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_aBulletMaxAmmuNum[1];
				DatBuf[DatBufLen++] = g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_aBulletMaxAmmuNum[2]; 
		
				DatBuf[DatBufLen++] = 0;
				DatBuf[DatBufLen++] = 0;
				DatBuf[DatBufLen++] = g_sResComTask_Mag.UnUpLoadEvtCnt;
				
				WeaponNum = UserParam_GetWeaponNum();

				DatBuf[DatBufLen++] = WeaponNum;
				
				for(w = 0; w < WeaponNum; w++)
				{
						DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_byWeapon_Type;
						DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_byWeapon_SubType;
						DatBuf[DatBufLen++] = IDParam_GetRid();
						DatBuf[DatBufLen++] = IDParam_GetPid() >> 8;
						DatBuf[DatBufLen++] = IDParam_GetPid();
						DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_byState;
						DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind;
						if(g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind != 0)//�ж��Ƿ������õ�ҩ
						{
							for(b = 0; b < g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind; b++)
							{
								
								Rnum = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[0] << 16 | 
											 g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[1] << 8 |
											 g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[2];
								Tnum = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum[0] << 16 | 
											 g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum[1] << 8 |
											 g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum[2];
								Snum = Tnum - Rnum;
								
								DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_Type;
								DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_SubType;
								DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[0];
								DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[1];
								DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[2];
								DatBuf[DatBufLen++] = Snum >> 16;
								DatBuf[DatBufLen++] = Snum >> 8;
								DatBuf[DatBufLen++] = Snum;
							}
						}
				}
				
				DatBuf[2] = DatBufLen - 3;
				DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
				DatBufLen++;
				 
				ChanParam.m_eChan = CHAN_ZIGBEE;
				ChanParam.m_pDstAddr = pSlaverMag->m_aWMac;
				ChanParam.m_byDstAddrLen = 8;
				ChanParam.m_pDatBuf = DatBuf;
				ChanParam.m_wDatBufLen = DatBufLen;
				ChanParam.m_dwRepeatCnt = 2; 
				ChanParam.m_dwScheTime = 100; 
				
				InterChanSend(&ChanParam);	
		}
}

// A3 C0  ͷ��״̬���ָ��
uint32_t InterConn_ProtDeal_HelmetStatusRet(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
    uint8_t Offset;
    SINGLE_SLAVER_MAG *pSlaverMag;
    SCOMTASK_MAG *pScomTaskMag;
    STRUCT_LPOS Pos;
    STRUCT_TIME1 Tim1;
    STRUCT_TS_Character1 Char1;
    uint16_t BatVol;
    
    //�������У��
    if(pBuf == NULL || BufLen < 34)
    {
        return FALSE;
    }
		//A3 C0 1E FE 08 11 00 39 00 00 00 00 00 00 00 00 00 00 80 00 00 00 00 00 00 00 00 01 00 C0 0D FE 05 69
    Offset = 8;
    UserParam_SetFireInstallFlg(pBuf[Offset++]);
    //memcpy((void *)&Pos,(void *)&pBuf[Offset],sizeof(STRUCT_LPOS));
    Offset += sizeof(STRUCT_LPOS);
    //UserParam_SetLPos(&Pos);

    //memcpy((void *)&Tim1,(void *)&pBuf[Offset],sizeof(STRUCT_TIME1));
    Offset += sizeof(STRUCT_TIME1);
    //UserParam_SetTime1(&Tim1);
    
    //UserParam_SetPosType(pBuf[Offset++]);
    //UserParam_SetSatNum(pBuf[Offset++]);
    Offset += 2;
		
    memcpy((void *)&pSlaverMag->m_sTsChar1 ,(void *)&pBuf[Offset],sizeof(STRUCT_TS_Character1));
    Offset += sizeof(STRUCT_TS_Character1);
    BatVol = pBuf[Offset]<<8|pBuf[Offset+1];
    Offset += 2;
    
    UserParam_SetActLaserCnt1(pBuf[Offset++]);
    
    //pSlaverMag = SlaverMag_GetHandleByTType(TType);
    pSlaverMag = SlaverMag_GetHandleByMac(pDstAddr,8);
    
    if(NULL != pSlaverMag)
    {
        pSlaverMag->m_dwLastCommTimeMs = GetSysRunMilliSecond();
				pSlaverMag->m_byIsSleepFlg = FALSE;
				g_sHmiComTask_Mag.m_byHelmetIsOnline = TRUE;
        memcpy((void *)&pSlaverMag->m_sTsChar1,(void *)&Char1,sizeof(pSlaverMag->m_sTsChar1));
        pSlaverMag->m_wBatVol = BatVol;
    }

    return TRUE;
}

//A3 C1
uint32_t InterConn_ProtDeal_Slaver_Poweron(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
		SINGLE_SLAVER_MAG *pSlaverMag;
		S_CONN_CHAN_PARAM ChanParam;
    uint8_t i, Offset, TType, TSubtype, RID, On_Flg = 0xFF, W_Type;

		uint16_t PID;
		uint32_t NowTime = GetSysRunMilliSecond();
		static uint32_t LastTime = 0;
	
		if((NowTime - LastTime) <= 1000)
			return FALSE;
		
		LastTime = NowTime;
		
    //�������У��
    if(pBuf == NULL || BufLen < 19)
    {
        return FALSE;
    }
			
    Offset = 3;
    TType = pBuf[Offset++];
    TSubtype = pBuf[Offset++];
		RID = pBuf[Offset++];
		PID = (uint16_t)pBuf[Offset] << 8 | pBuf[Offset + 1];
		Offset += 2;
		
		On_Flg = pBuf[Offset++];
		W_Type = pBuf[Offset++];

		for(i = 0; i < UserParam_GetSlaverCnt(); i++)
		{
				pSlaverMag = UserParam_GetSlaverMag(i);
		
				if(pSlaverMag->m_byTType == TType && pSlaverMag->m_byTSubType == TSubtype)
				{	
						if( pSlaverMag->m_aWMac[0] == pDstAddr[0] &&
								pSlaverMag->m_aWMac[1] == pDstAddr[1] &&
								pSlaverMag->m_aWMac[2] == pDstAddr[2] && 
								pSlaverMag->m_aWMac[3] == pDstAddr[3] &&
								pSlaverMag->m_aWMac[4] == pDstAddr[4] &&
								pSlaverMag->m_aWMac[5] == pDstAddr[5] &&
								pSlaverMag->m_aWMac[6] == pDstAddr[6] &&
								pSlaverMag->m_aWMac[7] == pDstAddr[7]
							)
							break;
						else
							return FALSE;
				}
		}
		if(i >= UserParam_GetSlaverCnt())
			return FALSE;
		
    if(NULL != pSlaverMag)
    {
        pSlaverMag->m_dwLastCommTimeMs = NowTime;

				if(On_Flg == 1)
				{
					pSlaverMag->m_byIsPowerOnFlg = TRUE;
				}
				else
				{
					pSlaverMag->m_byIsPowerOnFlg = FALSE;
				}
				
    }
    return TRUE;
}

//���ݱ�ը��λ���㱬ըѪ��
uint8_t GetInjuryBloodByExplodePart(ENUM_PARTITION BodyPart, int Distance, POSE_E Pose)
{
    uint8_t InjuryBlood = 0;

    SetRand();

    if(Distance > 5)
    {
        Distance = 5;
    }

    if(Pose == POSE_WO || Pose ==  POSE_PUFU)
    {
        switch(BodyPart)
        {
            case PARTITION_HEAD:
                InjuryBlood = 50;
                break;

            case PARTITION_LCHEST:
                InjuryBlood = 50;
                break;

            case PARTITION_RCHEST:
                InjuryBlood = 20 + rand() % 10 + (10 - 2 * Distance);
                break;

            case PARTITION_ABDOMEN:
                InjuryBlood = 20 + rand() % 10 + (10 - 2 * Distance);
                break;

            case PARTITION_BACK:
                InjuryBlood = 20 + rand() % 10 + (10 - 2 * Distance);
                break;

            case PARTITION_LARM:
                InjuryBlood = 10 + rand() % 10 + (10 - 2 * Distance);
                break;

            case PARTITION_RARM:
                InjuryBlood = 10 + rand() % 10 + (10 - 2 * Distance);
                break;

            case PARTITION_LLEG:
                InjuryBlood = 10 + rand() % 10 + (10 - 2 * Distance);
                break;

            case PARTITION_RLEG:
                InjuryBlood = 10 + rand() % 10 + (10 - 2 * Distance);
                break;

            case PARTITION_NONE:
                InjuryBlood = 10;
                break;

            case PARTITION_MAX:
                InjuryBlood = 10;
                break;

            default:
                InjuryBlood = 10;
                break;
        }
    }
    else
    {
        switch(BodyPart)
        {
            case PARTITION_HEAD:	//ͷ��
                InjuryBlood = 100;
                break;

            case PARTITION_LCHEST:	//����
                InjuryBlood = 100;
                break;

            case PARTITION_RCHEST:	//����
                InjuryBlood = 60 + rand() % 40;	//Σ����
                break;

            case PARTITION_ABDOMEN:	//����
                InjuryBlood = 27 + rand() % 32;	//�ض���
                break;

            case PARTITION_BACK:	//��
                InjuryBlood = 11 + rand() % 15;	//�ж���
                break;

            case PARTITION_LARM:	//���
                InjuryBlood = 0 + rand() % 8;	//����
                break;

            case PARTITION_RARM:	//�ұ�
                InjuryBlood = 0 + rand() % 8;	//����
                break;

            case PARTITION_LLEG:	//����
                InjuryBlood = 0 + rand() % 8;	//����
                break;

            case PARTITION_RLEG:	//����
                InjuryBlood = 0 + rand() % 8;	//����
                break;

            case PARTITION_NONE:
                InjuryBlood = 100;
                break;

            case PARTITION_MAX:
                InjuryBlood = 100;
                break;

            default:
                InjuryBlood = 100;
                break;
        }
    }

    if(InjuryBlood > 100)
    {
        InjuryBlood = 100;
    }

    return InjuryBlood;
}

//zigbee��ը��û���׵������ͱ��ģ��յ��׺�ը�˺������������
void GetExplodeMineAtypeASubtype(RCVHURTINFO_T *pHurtInfo)
{
    u8 index = 0;
    typedef struct MineInfoTab
    {
        u8 t_type;
        u8 t_subtype;
        u8 a_type;
        u8 a_subtype;
    } MineInfoTab;

    MineInfoTab Info[4] = {{0x43, 0x01, 0x74, 0x01},
													{0x43, 0x02, 0x74, 0x02},
													{0x43, 0x03, 0x74, 0x01},
													{0x43, 0x04, 0x74, 0x01}};

    pHurtInfo->m_A_Type 	= 0xff;
    pHurtInfo->m_A_Subtype 	= 0xff;

    for(index = 0; index < 4; index++)
    {
        if(pHurtInfo->m_RT_Type == Info[index].t_type && pHurtInfo->m_RT_Subtype == Info[index].t_subtype)
        {
            pHurtInfo->m_A_Type 	= Info[index].a_type;
            pHurtInfo->m_A_Subtype 	= Info[index].a_subtype;
            break;
        }
    }

    return;
}

uint32_t InterConn_ProtDeal_Explosive(uint8_t *pBuf, uint16_t BufLen)
{
		STRUCT_LPOS Position = {0};
		RCVHURTINFO_T HurtInfo = {0};
		
    uint8_t Key, ExplodeRadius = 0;
		uint32_t NowTime = GetSysRunMilliSecond();
		static uint32_t LastTime = 0;
		
    float distance = 0.0;
		
		
		if(UserParam_GetBlood() == 0)
		{
				return FALSE;
		}
		
		if((NowTime - LastTime) < 1000)
			return FALSE;

		LastTime = NowTime;
		
		HurtInfo.m_RT_Type    = pBuf[3];
		HurtInfo.m_RT_Subtype = pBuf[4];
		HurtInfo.m_RRID       = pBuf[5];
		HurtInfo.m_wRPID      = ((pBuf[6] << 8) | pBuf[7]);
		Key                   = pBuf[8];
		memcpy(&Position, &pBuf[9], 12);
		ExplodeRadius 		    = pBuf[25];
		
		//SetRand();
		srand(PARTITION_MAX);
		HurtInfo.m_HPart      = (ENUM_PARTITION)(rand() % 10);
		
		if(HurtInfo.m_HPart == PARTITION_NONE) 	//=PARTITION_NONEʱ��Ļ��λ������ʾ
		{
				HurtInfo.m_HPart = PARTITION_HEAD;
		}
		
		HurtInfo.m_HType      = (ENUM_H_Type_INDIVIDUAL)H_Type_I_Wounded;
		GetBulletUniID(HurtInfo.m_A_Type, HurtInfo.m_A_Subtype, &HurtInfo.m_byUniID);   //��ȡ����ͳһ����
		GetExplodeMineAtypeASubtype(&HurtInfo);
		HurtInfo.m_HurtBlood  = GetInjuryBloodByExplodePart(HurtInfo.m_HPart, (int)distance, g_sMpu6050Mag.Pose);

		g_sInjuryMag.m_Ttype    = HurtInfo.m_RT_Type;
		g_sInjuryMag.m_Tsubtype = HurtInfo.m_RT_Subtype;

		if(HurtInfo.m_HurtBlood > 100 )
		{
				HurtInfo.m_HurtBlood = 100;
		}

		if(HurtInfo.m_RT_Type == 0x45)
		{
				Wtn6_Play(WTN6_YX_SL);				//��������
		}
		else
		{
				Wtn6_Play(WTN6_YX_MineExplosion);	//��������
		}

		//��Ļ��ʾ
		HMI_GetInjury(&g_sHmiComTask_Mag, HurtInfo.m_HPart, HurtInfo.m_RT_Type, HurtInfo.m_RT_Subtype);
		//������ӳ��
		RemapHurtPart(&HurtInfo, 1);
		//���˽���
		UpdateDamage(HurtInfo);
		//�¼��ϴ�
		DamageReportEventUploadHandle(&HurtInfo);

		return TRUE;
}

uint32_t InterConn_ProtDeal_BroadcastBeacon(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
		S_CONN_CHAN_PARAM ChanParam;
		STRUCT_LPOS Position = {0};
		uint8_t i, Offset, TType, TSubtype, RID, Cdegree = 0, GetOnDistance = 0, S_Time[5] = {0};;

		uint8_t DatBufLen, DatBuf[50];
		uint16_t PID;

		float distance = 0.0;
		
		static uint8_t OutDistanceTimes = 0;
			
    //�������У��
    if(pBuf == NULL || BufLen < 32)
    {
        return FALSE;
    }
			
    Offset = 3;
    TType = pBuf[Offset++];
    TSubtype = pBuf[Offset++];
		RID = pBuf[Offset++];
		PID = (uint16_t)pBuf[Offset] << 8 | pBuf[Offset + 1];
		Offset += 2;
				
		Cdegree = pBuf[Offset++];
		
		memcpy(&Position,  &pBuf[Offset], 12);
		Offset += 12;
		memcpy(S_Time, &pBuf[Offset], 5);
		Offset += 5;
		Offset += 4;		//����
		GetOnDistance = pBuf[30];

		g_sUserParam.m_sBroadcastInfo.m_wRcvZigbeeBeaconTimeOutCnt = 0;
		
		if(g_sUserParam.m_sLPosition.m_byLposState == TRUE)
			distance = GetDistance(Position);
		else		//��Ա��λ�ã������ϳ�
		{
			g_sUserParam.m_sBroadcastInfo.m_byIsGetOn = FALSE;
						
			g_sUserParam.m_sBroadcastInfo.m_byVehicleTType = 0;
			g_sUserParam.m_sBroadcastInfo.m_byVehicleTSubtype	= 0;
			g_sUserParam.m_sBroadcastInfo.m_byVehicleRID = 0;
			g_sUserParam.m_sBroadcastInfo.m_wVehiclePID = 0;
			return FALSE;
		}

		//��Ա���³�
		if((uint8_t)distance < GetOnDistance)
		{				
				g_sUserParam.m_sBroadcastInfo.m_byRecBeacon = TRUE;
					
				g_sUserParam.m_sBroadcastInfo.m_byTargetVehicleTType = TType;
				g_sUserParam.m_sBroadcastInfo.m_byTargetVehicleTSubtype = TSubtype;
				g_sUserParam.m_sBroadcastInfo.m_byTargetVehicleRID = RID;
				g_sUserParam.m_sBroadcastInfo.m_wTargetVehiclePID = PID;
				
				memcpy(g_sUserParam.m_sBroadcastInfo.m_byTargetVehicleMac, pDstAddr, 8);
		}
		else if((uint8_t)distance > (GetOnDistance + 5)) 	//������볬����ֵ5
		{
				if(OutDistanceTimes++ > 30)
				{
						OutDistanceTimes = 0;
						g_sUserParam.m_sBroadcastInfo.m_byIsGetOn = FALSE;
						
						g_sUserParam.m_sBroadcastInfo.m_byVehicleTType = 0;
						g_sUserParam.m_sBroadcastInfo.m_byVehicleTSubtype	= 0;
						g_sUserParam.m_sBroadcastInfo.m_byVehicleRID = 0;
						g_sUserParam.m_sBroadcastInfo.m_wVehiclePID = 0;
						return FALSE;
				}
		}

		DatBufLen = 0;
		DatBuf[DatBufLen++] = 0xf5;
    DatBuf[DatBufLen++] = 0x11;
    DatBuf[DatBufLen++] = 0;
		
    DatBuf[DatBufLen++] = IDParam_GetTType();
    DatBuf[DatBufLen++] = IDParam_GetTSubType();
    DatBuf[DatBufLen++] = IDParam_GetRid();
    DatBuf[DatBufLen++] = IDParam_GetPid() >> 8;
    DatBuf[DatBufLen++] = IDParam_GetPid() & 0xff;
		
    DatBuf[DatBufLen++] = GetInjuryDegree(g_sUserParam.m_byBlood);
		
		memcpy(&DatBuf[DatBufLen], &g_sUserParam.m_sLPosition, sizeof(g_sUserParam.m_sLPosition));
    DatBufLen += sizeof(g_sUserParam.m_sLPosition);		//λ�ú͸߳�
		
    DatBuf[DatBufLen++] = g_sUserParam.m_sBroadcastInfo.m_byIsGetOn;
		
    DatBuf[DatBufLen++] = g_sUserParam.m_sBroadcastInfo.m_byVehicleTType;
    DatBuf[DatBufLen++] = g_sUserParam.m_sBroadcastInfo.m_byVehicleTSubtype;
    DatBuf[DatBufLen++] = g_sUserParam.m_sBroadcastInfo.m_byVehicleRID;
    DatBuf[DatBufLen++] = g_sUserParam.m_sBroadcastInfo.m_wVehiclePID >> 8;
    DatBuf[DatBufLen++] = g_sUserParam.m_sBroadcastInfo.m_wVehiclePID & 0xFF;
		
    DatBuf[2] = DatBufLen - 3;
    DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;
		
		ChanParam.m_eChan = eChan;
    ChanParam.m_pDstAddr = pDstAddr;
    ChanParam.m_byDstAddrLen = DstAddrLen;
    ChanParam.m_pDatBuf = DatBuf;
    ChanParam.m_wDatBufLen = DatBufLen;
    ChanParam.m_dwRepeatCnt = 10;
    ChanParam.m_dwScheTime = 100;  
    
    return InterChanSend(&ChanParam);
}

uint32_t InterConn_ProtDeal_BZ(uint8_t *pBuf, uint16_t BufLen)
{
		uint8_t i, Offset, TType, TSubtype, RID;

		uint8_t DatBufLen, DatBuf[50];
		uint16_t PID;
		
    //�������У��
    if(pBuf == NULL || BufLen < 9)
    {
        return FALSE;
    }
			
    Offset = 3;
    TType = pBuf[Offset++];
    TSubtype = pBuf[Offset++];
		RID = pBuf[Offset++];
		PID = (uint16_t)pBuf[Offset] << 8 | pBuf[Offset + 1];
		Offset += 2;
	
		//�Ƿ��Ƿ����Լ�
    if(TRUE != IDParam_IsIDValid(TType, TSubtype, RID, PID))
    {
        return FALSE;
    }
}

uint32_t InterConn_ProtDeal_Personnel_GetOn_Off(uint8_t *pBuf, uint16_t BufLen)
{
		uint8_t i, Offset, TType, TSubtype, RID, InOut_Ctrl;

		uint8_t DatBufLen, DatBuf[50];
		uint16_t PID;

    //�������У��
    if(pBuf == NULL || BufLen < 9)
    {
        return FALSE;
    }
			
    Offset = 3;
    TType = pBuf[Offset++];
    TSubtype = pBuf[Offset++];
		RID = pBuf[Offset++];
		PID = (uint16_t)pBuf[Offset] << 8 | pBuf[Offset + 1];
		Offset += 2;
	
		//�Ƿ��Ƿ����Լ�
		/*
    if(TRUE != IDParam_IsIDValid(TType, TSubtype, RID, PID))
    {
        return FALSE;
    }
		*/
		InOut_Ctrl = pBuf[Offset];
		
		if(InOut_Ctrl == 1)
		{
				g_sUserParam.m_sBroadcastInfo.m_byIsGetOn = TRUE;		//�ϳ�
				
				Offset = 9;
				g_sUserParam.m_sBroadcastInfo.m_byVehicleTType	= pBuf[Offset++];
				g_sUserParam.m_sBroadcastInfo.m_byVehicleTSubtype	= pBuf[Offset++];
				g_sUserParam.m_sBroadcastInfo.m_byVehicleRID = pBuf[Offset++];
				g_sUserParam.m_sBroadcastInfo.m_wVehiclePID = pBuf[Offset] << 8 | pBuf[Offset + 1];
		}
		else if(InOut_Ctrl == 2)
		{
				g_sUserParam.m_sBroadcastInfo.m_byIsGetOn = FALSE;		//�³�
			
				g_sUserParam.m_sBroadcastInfo.m_byVehicleTType = 0;
				g_sUserParam.m_sBroadcastInfo.m_byVehicleTSubtype	= 0;
				g_sUserParam.m_sBroadcastInfo.m_byVehicleRID = 0;
				g_sUserParam.m_sBroadcastInfo.m_wVehiclePID = 0;
		}
}

uint32_t InterConn_ProtDeal_Bomb(uint8_t *pBuf, uint16_t BufLen)
{
		STRUCT_LPOS Position = {0};
		RCVHURTINFO_T HurtInfo = {0};
		
    uint8_t Key, ExplodeRadius = 0, A_Pos[9] = {0}, S_Time[5] = {0};
    uint32_t NowTime = GetSysRunMilliSecond();
		static uint32_t LastTime = 0;
		
    float distance = 0.0;
		
		if(UserParam_GetBlood() == 0)
		{
				return FALSE;
		}
		
		if((NowTime - LastTime) < 1000)
			return FALSE;

		LastTime = NowTime;
		
		HurtInfo.m_RT_Type    = pBuf[3];
		HurtInfo.m_RT_Subtype = pBuf[4];
		HurtInfo.m_RRID       = pBuf[5];
		HurtInfo.m_wRPID      = ((pBuf[6] << 8) | pBuf[7]);
		Key                   = pBuf[8];
		HurtInfo.m_A_Type     = pBuf[9];
		HurtInfo.m_A_Subtype  = pBuf[10];
		memcpy(A_Pos, &pBuf[11], 9);
		memcpy(&Position, &pBuf[20], 12);
		//SetRand();
		srand(PARTITION_MAX);
		HurtInfo.m_HType      = H_Type_I_Wounded;
		HurtInfo.m_HPart      = (ENUM_PARTITION)(rand() % 10);

		if(HurtInfo.m_HPart == PARTITION_NONE) 	//=PARTITION_NONEʱ��Ļ��λ����ʾ
		{
				HurtInfo.m_HPart = PARTITION_HEAD;
		}

		HurtInfo.m_HurtBlood  = GetInjuryBloodByExplodePart(HurtInfo.m_HPart, (int)distance, g_sMpu6050Mag.Pose);

		GetExplodeMineAtypeASubtype(&HurtInfo);

		Wtn6_Play(WTN6_YX_MineExplosion);	//��������
		
		if(IsAlive(g_sUserParam.m_eStatus))// && Key == g_sUserParam.m_byKey)
    {
				GetBulletUniID(HurtInfo.m_A_Type, HurtInfo.m_A_Subtype, &HurtInfo.m_byUniID);   //��ȡ����ͳһ����
				g_sInjuryMag.m_Ttype    = HurtInfo.m_RT_Type;
				g_sInjuryMag.m_Tsubtype = HurtInfo.m_RT_Subtype;

				if(HurtInfo.m_HurtBlood > 100 )
				{
						HurtInfo.m_HurtBlood = 100;
				}

				//��Ļ��ʾ
				HMI_GetInjury(&g_sHmiComTask_Mag, HurtInfo.m_HPart, HurtInfo.m_RT_Type, HurtInfo.m_RT_Subtype);
				//������ӳ��
				RemapHurtPart(&HurtInfo, 1);
				//���˽���
				UpdateDamage(HurtInfo);
				//�¼��ϴ�
				DamageReportEventUploadHandle(&HurtInfo);
		}
}

void ModeChange_Reset(void)
{
		SINGLE_SLAVER_MAG *pSlaverMag = NULL;

		uint8_t i = 0, j = 0, w;
	
		for(i = 0; i < UserParam_GetSlaverCnt(); i++)
		{
				pSlaverMag = UserParam_GetSlaverMag(i);

				if(pSlaverMag->m_byTType == 0xFE)
					continue;
	
				if(pSlaverMag->m_byTType != 0x41 && pSlaverMag->m_byTType != 0x42 && pSlaverMag->m_byTType != 0x25)
				{
					if(g_sUserParam.m_byMode == GAME_GOING && pSlaverMag->m_byIsAmmuInit == TRUE)//g_sResComTask_Mag.m_ManoParamFlg == TRUE)
					{
							w = pSlaverMag->m_byWeaponIdx;
							for(j = 0; j < g_sUserParam.m_byBackstageCfgWeaponNum; j++)
							{
								if(pSlaverMag->m_byTType == g_sUserParam.m_saBackstageCfgAmmuInfo[j].m_byWeapon_Type
									&& pSlaverMag->m_byTSubType == g_sUserParam.m_saBackstageCfgAmmuInfo[j].m_byWeapon_SubType)
								{
									memset(&g_sUserParam.m_saAmmuInfo[w], 0, sizeof(SOLDIERAMMU_T));
									memcpy(&g_sUserParam.m_saAmmuInfo[w], &g_sUserParam.m_saBackstageCfgAmmuInfo[j],
										sizeof(SOLDIERAMMU_T));  //������һ�ε���������	
								}
							}
					}

					pSlaverMag->m_byIsPowerOnFlg = TRUE;
				}
		}
}

//���״���
void MineClearanceHandle(u8 RID_F, u16 PID_F, u8 M_A_TYPE, u8 M_A_SUBTYPE, u8 M_T_TYPE, u8 M_T_SUBTYPE)
{
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_byT_Type    	= IDParam_GetTType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_byT_Subtype 	= IDParam_GetTSubType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_byRID 			  = IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_aPID[0] 		  = IDParam_GetPid() >>8 ;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_aPID[1] 		  = IDParam_GetPid() &0xff;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_byC_Idegree 	= GetInjuryDegree(g_sUserParam.m_byBlood);
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_sL_Position 	= g_sUserParam.m_sLPosition;    //λ�ú͸߳�
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_sTime 			  = g_sUserParam.m_sUtcTime;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_byMT_Type 		= M_T_TYPE;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_byMT_Subtype = M_T_SUBTYPE;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_byM_RID 		  = RID_F;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_aM_PID[0] 		= PID_F >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_aM_PID[1] 		= PID_F & 0xff;

    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;
    PersonnelClearMineEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}

//���״���
void MineSetHandle(u8 RID_F, u16 PID_F, u8 M_A_TYPE, u8 M_A_SUBTYPE, u8 M_T_TYPE, u8 M_T_SUBTYPE)
{
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_byT_Type    	= IDParam_GetTType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_byT_Subtype 	= IDParam_GetTSubType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_byRID 			  = IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_aPID[0] 		  = IDParam_GetPid() >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_aPID[1] 		  = IDParam_GetPid() &0xff;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_byC_Idegree 	= GetInjuryDegree(g_sUserParam.m_byBlood);
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_sL_Position 	= g_sUserParam.m_sLPosition;    //λ�ú͸߳�
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_sTime 			  = g_sUserParam.m_sUtcTime;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_byMT_Type 		= M_T_TYPE;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_byMT_Subtype = M_T_SUBTYPE;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_byM_RID 		  = RID_F;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_aM_PID[0] 		= PID_F >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_aM_PID[1] 		= PID_F & 0xff;

    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;

    PersonnelLayMineEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}
//�ڲ�������ͨ
void InterConn_Protocol_Process(void)
{	   
		SINGLE_SLAVER_MAG *pSlaverMag;
		uint8_t i;
		uint32_t NowTime = GetSysRunMilliSecond();
		static uint8_t Flg1 = FALSE;
		static uint32_t Timer_ZigbeeHitch = 0, Timer_SelfState = 0;
		
		if(((NowTime - Timer_ZigbeeHitch) >= 5000) && ((NowTime - Timer_ZigbeeHitch) <= 6000))
		{
			if(ZigComTask_HandleGet()->m_sTaskMag.m_dwLastCommTime == 0)
			{
				if(Flg1 == FALSE)
				{
					Wtn6_Play(WTN6_HITCH);
					Wtn6_Play(WTN6_Number_1);
					Flg1 = TRUE;
				}
			}
			Timer_ZigbeeHitch = NowTime;
		}
		
		if(NowTime - Timer_SelfState >= 5000)
		{
			g_sUserParam.m_sBroadcastInfo.m_wRcvZigbeeBeaconTimeOutCnt++;
			if(g_sUserParam.m_sBroadcastInfo.m_wRcvZigbeeBeaconTimeOutCnt >= 6)
			{
				g_sUserParam.m_sBroadcastInfo.m_byIsGetOn = FALSE;

				g_sUserParam.m_sBroadcastInfo.m_byVehicleTType = 0;
				g_sUserParam.m_sBroadcastInfo.m_byVehicleTSubtype	= 0;
				g_sUserParam.m_sBroadcastInfo.m_byVehicleRID = 0;
				g_sUserParam.m_sBroadcastInfo.m_wVehiclePID = 0;
			}
	
			InterConn_ProtDeal_SelfStatue();
			Timer_SelfState = NowTime;
		}

		return;
}

//�ڲ�������ͨЭ�����
void InterConn_Analysis(ENUM_CONN_CHAN eChan, uint8_t *pBuf, uint16_t BufLen)
{
		uint8_t dat55[4];
    uint8_t TmpLen;
    uint16_t IR_Cmd;
    uint8_t *pDstAddr;

		//Uart_SendBuf(&g_sUartSetMag.Com[COM2], pBuf, BufLen);
		
		IR_Cmd = ((pBuf[0] << 8) | pBuf[1]);
		
    switch(IR_Cmd)
    {
				case 0xA201:
						InterConn_ProtDeal_IR_DB_Mac(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
						break;
				
				case 0xA251:
						InterConn_ProtDeal_IR_DB_Injury_Inquire(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
						break;
				
				case 0xA252:
						InterConn_ProtDeal_IR_DB_RescueResult_Inquire(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
						break;
				
				case 0xA266:
						InterConn_ProtDeal_IR_StateInfo_Inquire(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
						break;
				
				case 0xA267:
						InterConn_ProtDeal_IR_ID_Inquire(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
						break;
				
				case 0xA268:
						InterConn_ProtDeal_ManoParam_Set(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
						break;
				
				case 0xA271:
						InterConn_ProtDeal_IR_AmmuInfo_Inquire(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
						break;
				
				case 0xA274:
						InterConn_ProtDeal_IR_AmmuSupply(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
						break;
				
        case 0xA361:
            if(g_sUserParam.LayMines_Or_MineClearance == 0x01)		//����ָ��
            {
								g_sUserParam.LayMines_Or_MineClearance = 0x00;
                Wtn6_Play(WTN6_MINE_SET_SUCCESS);
                MineSetHandle(pBuf[5], pBuf[6] << 8 | pBuf[7], 0xFF, 0xFF, pBuf[3], pBuf[4]);
            }
            else if(g_sUserParam.LayMines_Or_MineClearance == 0x02)		//����ָ��
            {
                g_sUserParam.LayMines_Or_MineClearance = 0x00;
                Wtn6_Play(WTN6_Mine_Clearance);
                MineClearanceHandle(pBuf[5], pBuf[6] << 8 | pBuf[7], 0xFF, 0xFF, pBuf[3], pBuf[4]);
            }
            break;
       
        case 0xA281 : 
            g_sUserParam.LayMines_Or_MineClearance = 0x01;
            break;

        case 0xA282 :
            g_sUserParam.LayMines_Or_MineClearance = 0x02;          
            break;
				
        case 0xA2A0:	// 4.2 �����豸MAC��ַ��ȡָ��
            InterConn_ProtDeal_MacGet(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
            break;

        case 0xA2A1:	// 4.4 �����Ϣ����ָ�0xA2��0xA1��
            InterConn_ProtDeal_IDSet(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
            break;

        case 0xA2A3:	// 4.8 ����������豸ָ��
            InterConn_ProtDeal_SlaverClear(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
            SlaverMag_ListInit();
            break;

        case 0xA2A4:	// 4.10 �����������豸ָ�0xA2��0xA4��
            InterConn_ProtDeal_SlaverConfig(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
            SlaverMag_ListInit();
            break;

        case 0xA2AD:	// 4.28 ��ȡ������Ϣָ�0xA2��0xAD��
            InterConn_ProtDeal_InfoSend(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
            break;

        case 0xA2AE:	// 4.29 ʵ�������ȡָ�0xA2��0xAE�� ɾ��
						InterConn_ProtDeal_ParamSend(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
            break;

        case 0xA2AF:	// 4.31 ʵ���������ָ�0xA2��0xAF��
            InterConn_ProtDeal_ParamSet(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
            break;

        case 0xA2FF:	// �弶����ģʽ��0xA2��0xFF�� �Զ���
            InterConn_ProtDeal_InfoSend(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
            TesModeHandle();
            break;
				
        case 0xA3A5:
            InterConn_ProtDeal_SlaverConnectRet(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
            break;
				
				case 0xA3A6:
						InterConn_ProtDeal_Slaver_WeaponLockRet(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
						break;
				
        case 0xA3A7:		//�������豸״̬�ظ�
            InterConn_ProtDeal_SlaverStatusRet(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
            break;    
				
				case 0xA3A8:
						InterConn_ProtDeal_Slaver_WeaponInfoRet(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
						break;
				
				case 0xA3A9:
						InterConn_ProtDeal_Slaver_HeavyWeaponInfoRet(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
						break;
				
				case 0xA3AA:
						InterConn_ProtDeal_Slaver_AmmuSupplyRet(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
						break;
				
				case 0xA3AB:
						InterConn_ProtDeal_Slaver_AmmuStausRet(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
						break;
				
				case 0xA3AC:
						InterConn_ProtDeal_Slaver_WeaponShootingInquire(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
						break;
				
				case 0xA3B3:
						InterConn_ProtDeal_SetUpVariousElements(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
            break;
        
        case 0xA2B4:
						InterConn_ProtDeal_Rescure_State(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
            break;
				
				case 0xA3B8:
						InterConn_ProtDeal_Slaver_Lightweapon_AmmuInfoSync(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
						break;
				
				case 0xA3B9:
						InterConn_ProtDeal_Slaver_Heavyweapon_AmmuInfoSync(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
						break;
				
        case 0xA3C0:
            InterConn_ProtDeal_HelmetStatusRet(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
            break; 

				case 0xA3C1:
						InterConn_ProtDeal_Slaver_Poweron(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
						break;
				 
        case 0xF170:
						InterConn_ProtDeal_BroadcastBeacon(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
            break;
								
				case 0xF171:
						break;
				
				case 0xF172:
						break;
				
				case 0xF186:
						InterConn_ProtDeal_Explosive(pBuf, BufLen);
						break;
          
				case 0xF191:
						InterConn_ProtDeal_Bomb(pBuf, BufLen);
						break;
				
        case 0xF410:
						InterConn_ProtDeal_BZ(pBuf, BufLen);
            break;
				
				case 0xF510:
						InterConn_ProtDeal_Personnel_GetOn_Off(pBuf, BufLen);
            break;

        default:
            break;
    }
}
//������մ���
//����̾൯ҩ���ʲ���ָ���
void IRAmmuGetHandle(Struct_CtrlSupplementDownload *Supplement)
{
		uint8_t b, i, k;
    uint32_t BulletNum = 0, BulletTotalNum = 0, BulletAddNum = 0, BulletRemain = 0;
    SOLDIERAMMU_T *pAmmuInfo = NULL;

    //����ҩ��remain�Ͳ�������ٷ���remain
    for(b = 0; b < Supplement->m_byChangeAmmuNum; b++) //��ҩ�仯����
    {
        for(i = 0; i < WEAPONKIND_MAX; i++)
        {
            pAmmuInfo = &g_sUserParam.m_saAmmuInfo[i];

            for(k = 0; k < pAmmuInfo->m_byCfgBulletKind; k++)
            {
                if(pAmmuInfo->m_sBulletInfo[k].m_byBullet_Type 		== Supplement->m_sChangeAmmu[b].m_byA_Type 	&&
                        pAmmuInfo->m_sBulletInfo[k].m_byBullet_SubType 	== Supplement->m_sChangeAmmu[b].m_byA_Subtype)
                {
                    BulletRemain = pAmmuInfo->m_sBulletInfo[k].m_aBulletRemainNum[0] << 16 |
														 pAmmuInfo->m_sBulletInfo[k].m_aBulletRemainNum[1] << 8 |
														 pAmmuInfo->m_sBulletInfo[k].m_aBulletRemainNum[2];
							
										BulletTotalNum = pAmmuInfo->m_sBulletInfo[k].m_aBulletTotalNum[0] << 16 |
																		 pAmmuInfo->m_sBulletInfo[k].m_aBulletTotalNum[1] << 8 |
																		 pAmmuInfo->m_sBulletInfo[k].m_aBulletTotalNum[2]                    ;

										BulletAddNum = Supplement->m_sChangeAmmu[b].m_aChangeAmmuNum[0] << 16 |
																	 Supplement->m_sChangeAmmu[b].m_aChangeAmmuNum[1] << 8 |
																	 Supplement->m_sChangeAmmu[b].m_aChangeAmmuNum[2];

										BulletTotalNum = BulletTotalNum + BulletAddNum;
									
										pAmmuInfo->m_sBulletInfo[k].m_aBulletTotalNum[0]	=	BulletTotalNum >> 16;
										pAmmuInfo->m_sBulletInfo[k].m_aBulletTotalNum[1]	=	BulletTotalNum >> 8;
										pAmmuInfo->m_sBulletInfo[k].m_aBulletTotalNum[2]	=	BulletTotalNum & 0xff;

										BulletNum = BulletRemain + BulletAddNum;
										pAmmuInfo->m_sBulletInfo[k].m_aBulletRemainNum[0] = BulletNum >> 16;
										pAmmuInfo->m_sBulletInfo[k].m_aBulletRemainNum[1] = BulletNum >> 8;
										pAmmuInfo->m_sBulletInfo[k].m_aBulletRemainNum[2] = BulletNum & 0xff;

                    //Zigbee����
                    InterConn_ProtDeal_Slaver_AmmuSupplyNum(pAmmuInfo, k, BulletNum);

										//�¼��ϴ�
                    Supplement->m_byMasterT_Type 	  = 0x48;	//�����豸
                    Supplement->m_byMasterT_Subtype = 0x02;	//ģ��װ����
                    Supplement->m_byMasterRID 		= 0xff;
                    Supplement->m_aMasterPID[0] 	= 0xff;
                    Supplement->m_aMasterPID[1] 	= 0xff;
                    RSP_CtrlSupplementEventUploadHandle(Supplement);
                }
            }
        }
    }

    //������ʾ
    Wtn6_Play(WTN6_ReplenishAmmunition);
}

//��������
void CollateralDamage(RCVHURTINFO_T HurtInfo)
{
		uint8_t DatBuf[50];
    uint8_t DatBufLen;
		//F1 91 28 41 01 11 06 7B 00 14 06 81 18 53 69 04 32 04 21 30 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 02 03 04 05 BF
		DatBufLen = 0;
    DatBuf[DatBufLen++] = 0xF1;
    DatBuf[DatBufLen++] = 0x91;
    DatBuf[DatBufLen++] = 0;
    DatBuf[DatBufLen++] = HurtInfo.m_RT_Type;
    DatBuf[DatBufLen++] = HurtInfo.m_RT_Subtype;
    DatBuf[DatBufLen++] = HurtInfo.m_RRID;
    DatBuf[DatBufLen++] = HurtInfo.m_wRPID >> 8;
    DatBuf[DatBufLen++] = HurtInfo.m_wRPID & 0xff;
    DatBuf[DatBufLen++] = UserParam_GetKey();
    DatBuf[DatBufLen++] = HurtInfo.m_A_Type;
    DatBuf[DatBufLen++] = HurtInfo.m_A_Subtype;
    memcpy(&DatBuf[DatBufLen], &HurtInfo.m_sFPosition, sizeof(STRUCT_LPOS));
    DatBufLen += sizeof(STRUCT_LPOS);
    memcpy(&DatBuf[DatBufLen], &g_sUserParam.m_sLPosition, sizeof(STRUCT_LPOS));
    DatBufLen += sizeof(STRUCT_LPOS);
    DatBuf[DatBufLen++] = 1;
    DatBuf[DatBufLen++] = 2;
    DatBuf[DatBufLen++] = 3;
    DatBuf[DatBufLen++] = 4;
    DatBuf[DatBufLen++] = 5;

		DatBuf[2] = DatBufLen - 3;
    DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;
		ZigComTask_BroadCast(DatBuf, DatBufLen);
}

//�㲥
void ZigComTask_BroadCast(uint8_t *pBuf, uint16_t BufLen)
{
		S_CONN_CHAN_PARAM ChanParam;
    SCOMTASK_MAG *pScomTaskMag;

   	uint8_t DatBuf[50];
    uint8_t DatBufLen, BroadcastAddr = 0x05;
	
    if(BufLen < 250)
    {
				DatBufLen = 0;
				DatBuf[DatBufLen++] = 0x5A;
				DatBuf[DatBufLen++] = 0;
				DatBuf[DatBufLen++] = 0xFD;
				DatBuf[DatBufLen++] = 0x01;
				DatBuf[DatBufLen++] = BufLen;
				DatBuf[DatBufLen++] = 0xFF;
				DatBuf[DatBufLen++] = 0xFF;
				memcpy(&DatBuf[DatBufLen], pBuf, BufLen);
				DatBufLen += BufLen;
				DatBuf[DatBufLen++] = 0xAA;
				DatBuf[1] = DatBufLen;
    }
		//Uart_SendBuf(&g_sUartSetMag.Com[ZIG_COM], DatBuf, DatBufLen);

		ChanParam.m_eChan = CHAN_ZIGBEE;
		ChanParam.m_pDstAddr = &DatBuf[BroadcastAddr];
		ChanParam.m_byDstAddrLen = 2;
		ChanParam.m_pDatBuf = DatBuf;
		ChanParam.m_wDatBufLen = DatBufLen;
		ChanParam.m_dwRepeatCnt = 2; 
		ChanParam.m_dwScheTime = 100; 
		
		InterChanSend(&ChanParam);
}

//
void ZigComTask_SendToHelmet(uint8_t *pDstAddr, uint8_t DstAddrLen, uint8_t *pBuf, uint16_t BufLen, uint8_t DevIdx, DEVCTRL eDevCtl)
{
   	uint8_t DatBuf[50];
    uint8_t DatBufLen;
	
	  S_CONN_CHAN_PARAM ChanParam;
    SINGLE_SLAVER_MAG *pSlaverMag;
    SCOMTASK_MAG *pScomTaskMag;

		pSlaverMag = SlaverMag_GetHandleByMac(pDstAddr,8);

		if(NULL == pSlaverMag)
			return;

		if(pSlaverMag->m_byIsSleepFlg == TRUE)
			return;

    if(BufLen < 250)
    {
				DatBufLen = 0;
				DatBuf[DatBufLen++] = 0x55;
				DatBuf[DatBufLen++] = GetProtAddr();
				DatBuf[DatBufLen++] = 3 + BufLen;
				DatBuf[DatBufLen++] = PROTOCAL_CMD_BOARD_CTRL;
				DatBuf[DatBufLen++] = DevIdx;
				DatBuf[DatBufLen++] = (uint8_t)eDevCtl;
				memcpy(&DatBuf[DatBufLen], pBuf, BufLen);
				DatBufLen += BufLen;
				DatBuf[DatBufLen] = Protocal_MakeU8Sum(&DatBuf[3], 3 + BufLen);
				DatBufLen++;
				DatBuf[DatBufLen] = 0xAA;
				DatBufLen++;
			
				ChanParam.m_eChan = CHAN_ZIGBEE;
				ChanParam.m_pDstAddr = pDstAddr;
				ChanParam.m_byDstAddrLen = 8;
				ChanParam.m_pDatBuf = DatBuf;
				ChanParam.m_wDatBufLen = DatBufLen;
				ChanParam.m_dwRepeatCnt = 1; 
				ChanParam.m_dwScheTime = 100; 
				
				InterChanSend(&ChanParam);
    }
}

//Ӳ������Э�����
void InterConn_HardCtrl_Analysis(ENUM_CONN_CHAN eChan, uint8_t *pBuf, uint16_t BufLen)
{
	  SCOMTASK_MAG *pScomTaskMag;
		LASERCOMTASK_MAG *pMag = LaserComTask_HandleGet();
    uint8_t DatLen,DevIdx,DevCtrl;
    uint16_t IR_Cmd;
    uint32_t MagicID;

    if(NULL == pBuf)
    {
        return;
    }

    DatLen = pBuf[2];
    if(DatLen < 2)
    {
        return;
    }
    DevIdx= pBuf[4];
    DevCtrl= pBuf[5];
    
    switch(DevIdx)
    {
        case 0x06://48k̽ͷ����
            if(DevCtrl == 0x01 && pBuf[8] == 0x10 && DatLen > 6)
            {
                //pBuf[9] |= 0x80; //̽ͷ������λ��Ϊ1
								/*
								if((pBuf[7] < 16) && (pBuf[11] == 0x7A))
								{
									if(pBuf[19] == 0x01)
										CtrlHelmetLaserOnOff(pBuf[9], 1);
									else
										CtrlHelmetLaserOnOff(pBuf[9], 0);
								}
								*/
								LaserProtocalDeal(pMag, &pBuf[6], pBuf[7], TRUE);		//֧������̽ͷ��С̽ͷ
								ZigComTask_SendToHelmet(ZigComTask_LastCommMacGet(), 8 ,&pBuf[6], BufLen-8, 0x06, CTRL_RECV);
                //ResComTask_SendToUpcom(&pBuf[6], BufLen-8, 0x06, CTRL_RECV);
            }
            else if(DevCtrl == 0x01 && pBuf[8] == 0xAF)
            {
                //pBuf[9] |= 0x80; //̽ͷ������λ��Ϊ1
								//LaserProtocalDeal(pMag, &pBuf[8], DatLen);		//֧������̽ͷ��С̽ͷ
								ZigComTask_SendToHelmet(ZigComTask_LastCommMacGet(),8, &pBuf[6], BufLen-8, 0x06, CTRL_RECV);
                //ResComTask_SendToUpcom(&pBuf[6], BufLen-8, 0x06, CTRL_RECV);
            }
            
            break;
        default:
            if(CHAN_ZIGBEE == eChan)
            {
                MagicID = 0;
                MagicID = Protocal_MakeU32Sum(pBuf,BufLen);
                MagicID += Protocal_MakeU32Sum( ZigComTask_LastCommMacGet(),8);       
                pScomTaskMag = ZigComTask_TaskHandleGet();
                SComTask_SendListDeleteByFrameId(pScomTaskMag->m_pSendList,MagicID);    
            }
            break;
    }
    
}

void GetSlaverStatus(ENUM_CONN_CHAN eChan,SINGLE_SLAVER_MAG *pMag)
{
    uint16_t Type;
    if(NULL == pMag)
    {
        return;
    }
    
    Type = pMag->m_byTType<<8|pMag->m_byTSubType;

    switch(Type)
    {
        //ͷ��
        case 0xFE08:
            InterConn_HelmetStaGet(eChan, pMag->m_aWMac, sizeof(pMag->m_aWMac));
            break;

        default:
            InterConn_SlaverStaGet(eChan, pMag->m_aWMac, sizeof(pMag->m_aWMac));
            break;
    }
}

/****************************************************************
@FUNCTION�����豸״̬��ʼ�б��ʼ��
@INPUT����
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
void SlaverMag_ListInit(void)
{
    u8 i = 0;

    SINGLE_SLAVER_PARAM *pParam = NULL;
    SINGLE_SLAVER_MAG *pMag = NULL;

    UserParam_SetSlaverCnt(0);
        
    for(i = 0; i < SYS_SLAVER_CNT_MAX; i++)
    {
        pParam = SysParam_GetSlaverParam(i);

        if(pParam == NULL)
        {
            continue;
        }

        //ϵͳ�����е�MAC��Ч
        if(TRUE == IsParamValid(pParam->m_aWMac, sizeof(pParam->m_aWMac)))
        {
            //������ʱ����������������¼
            pMag = UserParam_GetSlaverMag(UserParam_GetSlaverCnt());
            memset((void *)pMag, 0, sizeof(SINGLE_SLAVER_MAG)) ;
            pMag->m_byTType = pParam->m_byTType;
            pMag->m_byTSubType = pParam->m_byTSubType;
            pMag->m_byWType = pParam->m_byWType;
						pMag->m_byIdx = pParam->m_byIdx;
						pMag->m_byWeaponIdx = pParam->m_byWeaponIdx;
						//pMag->m_byCfgFlag = FALSE;
						pMag->m_dwLastCommTimeMs = 0;
						pMag->m_dwLastGetStaTimeMs = 0;
            pMag->m_byRid = 0xFF;
            pMag->m_wPid = 0xFFFF;
            memcpy((void *)pMag->m_aWMac, (void *)pParam->m_aWMac, sizeof(pParam->m_aWMac)) ;

            UserParam_SetSlaverCnt(UserParam_GetSlaverCnt() + 1);

            //���豸����	
						if(pMag->m_byIsOnline == FALSE)
						{
							InterConn_SlaverConnect(CHAN_ZIGBEE,pMag);
							if(pMag->m_byTType != 0xFE)
							{
									if((pMag->m_byTType != 0x41) && (pMag->m_byTType != 0x42) && (pMag->m_byTType != 0x25))
									{
											InterConn_ProtDeal_Slaver_LightWeapon(pMag);
									}
									else
									{
											InterConn_ProtDeal_Slaver_HeavyWeapon(pMag);
									}
							}
						}
        }
    }
}

/****************************************************************
@FUNCTION�������豸״̬�б��ȡָ��MAC��ַ�����豸����ṹ��ָ��
@INPUT��MAC��ַ
@OUTPUT����
@RETURN: ���豸����ṹ��ָ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
SINGLE_SLAVER_MAG* SlaverMag_GetHandleByMac(uint8_t *pMacHex,uint8_t MacHexLen)
{
    uint8_t i;
    SINGLE_SLAVER_MAG *pMag = NULL;

    if(pMacHex == NULL || MacHexLen > 8)
    {
        return pMag;
    }    
    
    //��������ǰ���б�
    for(i = 0; i < UserParam_GetSlaverCnt(); i++)
    {
        pMag = UserParam_GetSlaverMag(i);
        if(0 == memcmp(pMag->m_aWMac, pMacHex, MIN(MacHexLen,8)))
        {
            return pMag;
        }
    }

    if(i == UserParam_GetSlaverCnt())
    {
        pMag = NULL;
    }

    return pMag;
}

/****************************************************************
@FUNCTION�������豸״̬�б��ȡָ��ʵ�����͵����豸����ṹ��ָ��
@INPUT��ʵ������+ ʵ�������Ǹ�
@OUTPUT����
@RETURN: ���豸����ṹ��ָ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
SINGLE_SLAVER_MAG* SlaverMag_GetHandleByType(uint8_t TType,uint8_t TSubType)
{
    uint8_t i;
    SINGLE_SLAVER_MAG *pMag = NULL;
    
    //��������ǰ���б�
    for(i = 0; i < UserParam_GetSlaverCnt(); i++)
    {
        pMag = UserParam_GetSlaverMag(i);

        if(pMag->m_byTType == TType && pMag->m_byTSubType == TSubType)
        {
            return pMag;
        }
    }

    if(i == UserParam_GetSlaverCnt())
    {
        pMag = NULL;
    }

    return pMag;
}
	
#define DISCONN_TIME_MS 60000//61 * 60000
//���豸ά��
void SlaverMag_TimedProcess(void)
{
    SINGLE_SLAVER_MAG *pMag = NULL;
		SCOMTASK_MAG *pScomTaskMag;
	  uint8_t i;
    uint32_t NowTimeMs = GetSysRunMilliSecond();
		uint32_t MagicID;
    
    //������ǰ���б�,�������ڸö�ʱ������
    for(i = 0; i < UserParam_GetSlaverCnt(); i++)
    {
        pMag = UserParam_GetSlaverMag(i);
        if(NULL == pMag || pMag->m_byIsOnline == FALSE || 0xFF == pMag->m_byTType || pMag->m_byTType == 0)
        {
            continue;
        }
				
        //�����ɹ������ߣ��ж�����
				if(pMag->m_byIsPowerOnFlg == TRUE)
				{
						//���豸����
						InterConn_SlaverConnect(CHAN_ZIGBEE,pMag);
						pMag->m_byIsPowerOnFlg = FALSE;
					
						if(pMag->m_byTType != 0xFE)
						{
							if((pMag->m_byTType != 0x41) && (pMag->m_byTType != 0x42) && (pMag->m_byTType != 0x25))
								InterConn_ProtDeal_Slaver_LightWeapon(pMag);
							else
								InterConn_ProtDeal_Slaver_HeavyWeapon(pMag);
						}
						continue;
				}		
								
				if(pMag->m_byTType != 0x41 && pMag->m_byTType != 0x42 
					&& pMag->m_byTType != 0x25)
				{
					if((NowTimeMs - pMag->m_dwLastCommTimeMs) > DISCONN_TIME_MS)
					{
							if(pMag->m_byTType == 0xFE && pMag->m_byTSubType == 0x08)
							{
									g_sHmiComTask_Mag.m_byHelmetIsOnline = FALSE;
							}
							pMag->m_byIsSleepFlg = TRUE;
					}
					else
					{
							pMag->m_byIsSleepFlg = FALSE;
					}
				}	
				
				//��ʱ��ȡ״̬
				//if(((NowTimeMs - pMag->m_dwLastCommTimeMs) > UserParam_GetSlaverStatusInterval())
				//		&&((NowTimeMs - pMag->m_dwLastGetStaTimeMs) > UserParam_GetSlaverStatusInterval()))
				if((NowTimeMs - pMag->m_dwLastGetStaTimeMs) > 5000)
				{
						GetSlaverStatus(CHAN_ZIGBEE,pMag);
						pMag->m_dwLastGetStaTimeMs = NowTimeMs;
				}		
    }
}

void ProtDeal_HardCtrl_48k(uint8_t *pBuf, uint16_t BufLen)
{
    uint8_t DevIdx, DevCtrl, Offset,Cmd;
    uint32_t IsCutted = FALSE;
    uint16_t Len;

    if(BufLen < 3)
        return;

    //��ת������
    ComTask_RetransmissionData(LaserComTask_TaskHandleGet(), pBuf, BufLen);
    
    DevIdx = pBuf[0];
    DevCtrl = pBuf[1];
    
    //����ת������ն�
    Len = BufLen-3;
    Offset = 3;
    
    //���׵ƿ�
    if((Len >= 6) && (0x30 == pBuf[Offset+2])&& (pBuf[Offset+4]>=0x80))
    {
        if(pBuf[Offset+4] == 0xFF)
        {
            CtrlHelmetLaserOnOff(pBuf[Offset+4],pBuf[Offset+3]);  
        }
        else
        {
            CtrlHelmetLaserOnOff(pBuf[Offset+4]-0x80,pBuf[Offset+3]);  
        }
    }
    //ģʽ����
    else if((Len >= 5) && (0xB1 == pBuf[Offset+2]))
    {
        HardCtrl_HelmetBase(pBuf,BufLen);
    }
#if 0
    //�ڵ�̽��
    else if((Len >= 10) && (0x31 == pBuf[Offset+2])&& (pBuf[Offset+3]>=0x80))
    {
        if(pBuf[Offset+3] == 0xFF)
        {
            HelmetHardCtrl(pBuf,BufLen);
        }
        else
        {
            pBuf[Offset+3] -= 0x80;
            HelmetHardCtrl(pBuf,BufLen);
        }
    }
#endif    
   
}

void ProtDeal_HardCtrl_Power(uint8_t *pBuf, uint16_t BufLen)
{
    uint8_t DevIdx, DevCtrl, Offset;

    if(BufLen < 3)
        return;

    DevIdx = pBuf[0];
    DevCtrl = pBuf[1];

    if(DevCtrl == CTRL_DO && pBuf[2] == 2)
    {
        g_sPowerOffMag.m_bPoweroff_Force = TRUE;
    }
}

void ProtDeal_HardCtrl_RunLed(uint8_t *pBuf, uint16_t BufLen)
{
    uint8_t DevIdx, DevCtrl, Offset;

    if(BufLen < 5)
        return;

    DevIdx = pBuf[0];
    DevCtrl = pBuf[1];

    if(DevCtrl == CTRL_PWM)
    {
        Led_SetBrightness(pBuf[4], &g_sRunledMag);
    }
}

void ProtDeal_HardCtrl_Voice(uint8_t *pBuf, uint16_t BufLen)
{
    uint8_t DevIdx, DevCtrl, Offset;

    if(BufLen < 3)
        return;

    DevIdx = pBuf[0];
    DevCtrl = pBuf[1];

    if(DevCtrl == CTRL_DAT)
    {
        Wtn6_Play(pBuf[2]);
    }
}

void ProtDeal_HardCtrl_Motor(uint8_t *pBuf, uint16_t BufLen)
{
    uint8_t DevIdx, DevCtrl, Offset;
    uint32_t Tim;


    if(BufLen < 7)
        return;

    DevIdx = pBuf[0];
    DevCtrl = pBuf[1];
    memcpy((void *)&Tim, &pBuf[3], 4);

    if(DevCtrl == CTRL_DO)
    {
        switch(pBuf[2])
        {
            case 0x01:
                Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, Tim / 100);
                break;

            case 0x02:
                Motor_Off(&g_sMotorMag, OPT_MODE_NONBLOCK);
                break;

            default:
                break;
        }
    }
}

void ProtDeal_HardCtrl_Fume(uint8_t *pBuf, uint16_t BufLen)
{
    uint8_t DevIdx, DevCtrl, Offset;
    uint32_t Tim;
    uint8_t Buf[30];
    uint8_t Off = 0;
    SINGLE_SLAVER_MAG *pSingleMag;
    
    if(BufLen < 2)
        return;
    
    DevIdx = pBuf[0];
    DevCtrl = pBuf[1];
    

    if(DevCtrl == CTRL_DO)
    {
        if(BufLen < 7)
            return;
        
        memcpy((void *)&Tim, &pBuf[3], 4);
        
        switch(pBuf[2])
        {
            
            default:
                CtrlHelmetFume((DEVCTRL)pBuf[2],Tim);
                break;
        }
    }
    else if(DevCtrl == CTRL_DI)
    {
        Off = UserParam_GetFireInstallFlg(); // 0-δ��װ��1-�Ѱ�װ
        ResComTask_SendToUpcom(&Off, 1, 0xA0, CTRL_DI);
    }
}

void HardCtrl_Slaver_PowerOff(void)
{
		uint8_t DatBuf[50];
		uint8_t DatBufLen, i, last_Weapon_Type = 0;

		S_CONN_CHAN_PARAM ChanParam;
		SINGLE_SLAVER_MAG *pSlaverMag;
		for(uint8_t i = 0; i < SYS_SLAVER_CNT_MAX; i++)
		{
				pSlaverMag = UserParam_GetSlaverMag(i);
				
				if(NULL == pSlaverMag)
					continue;

				if(pSlaverMag->m_byIsSleepFlg == TRUE)
					continue;
				DatBufLen = 0;
				DatBuf[DatBufLen++] = 0x55;
				DatBuf[DatBufLen++] = 0x01;
				DatBuf[DatBufLen++] = 0x01;
				DatBuf[DatBufLen++] = 0x02;
				DatBuf[DatBufLen++] = 0x02;
				DatBuf[DatBufLen++] = 0xAA;

				DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
				DatBufLen++;
				 
				ChanParam.m_eChan = CHAN_ZIGBEE;
				ChanParam.m_pDstAddr = pSlaverMag->m_aWMac;
				ChanParam.m_byDstAddrLen = 8;
				ChanParam.m_pDatBuf = DatBuf;
				ChanParam.m_wDatBufLen = DatBufLen;
				ChanParam.m_dwRepeatCnt = 20; 
				ChanParam.m_dwScheTime = 100; 
				
				InterChanSend(&ChanParam);	
		}
}

void HardCtrl_Slaver_Night(void)
{
		uint8_t DatBuf[50];
		uint8_t DatBufLen, i;

		S_CONN_CHAN_PARAM ChanParam;
		SINGLE_SLAVER_MAG *pSlaverMag;
	
		for(uint8_t i = 0; i < UserParam_GetSlaverCnt(); i++)
		{
				pSlaverMag = UserParam_GetSlaverMag(i);

				if(pSlaverMag->m_byIsSleepFlg == TRUE)
					continue;

				if(pSlaverMag->m_byTType != 0x30)
				{
					DatBufLen = 0;
					DatBuf[DatBufLen++] = 0x55;
					DatBuf[DatBufLen++] = 0x01;
					DatBuf[DatBufLen++] = 0x06;
					DatBuf[DatBufLen++] = 0x00;
					DatBuf[DatBufLen++] = 0x01;
					DatBuf[DatBufLen++] = 0x05;
					DatBuf[DatBufLen++] = 0x00;
					DatBuf[DatBufLen++] = 0x00;
					DatBuf[DatBufLen++] = 0x0A;

					DatBuf[DatBufLen++] = 0x10;

					DatBuf[DatBufLen++] = 0xAA;
	 
					ChanParam.m_eChan = CHAN_ZIGBEE;
					ChanParam.m_pDstAddr = pSlaverMag->m_aWMac;
					ChanParam.m_byDstAddrLen = 8;
					ChanParam.m_pDatBuf = DatBuf;
					ChanParam.m_wDatBufLen = DatBufLen;
					ChanParam.m_dwRepeatCnt = 20; 
					ChanParam.m_dwScheTime = 100; 
					
					InterChanSend(&ChanParam);	
			}
		}
}

void HardCtrl_Slaver_Day(void)
{
		uint8_t DatBuf[50];
		uint8_t DatBufLen, i;

		S_CONN_CHAN_PARAM ChanParam;
		SINGLE_SLAVER_MAG *pSlaverMag;
	
		for(uint8_t i = 0; i < UserParam_GetSlaverCnt(); i++)
		{
				pSlaverMag = UserParam_GetSlaverMag(i);

				if(pSlaverMag->m_byIsSleepFlg == TRUE)
					continue;

				if(pSlaverMag->m_byTType != 0x30)
				{
					DatBufLen = 0;
					DatBuf[DatBufLen++] = 0x55;
					DatBuf[DatBufLen++] = 0x01;
					DatBuf[DatBufLen++] = 0x06;
					DatBuf[DatBufLen++] = 0x00;
					DatBuf[DatBufLen++] = 0x01;
					DatBuf[DatBufLen++] = 0x05;
					DatBuf[DatBufLen++] = 0x00;
					DatBuf[DatBufLen++] = 0x00;
					DatBuf[DatBufLen++] = 0x64;

					DatBuf[DatBufLen++] = 0x6A;

					DatBuf[DatBufLen++] = 0xAA;
	 
					ChanParam.m_eChan = CHAN_ZIGBEE;
					ChanParam.m_pDstAddr = pSlaverMag->m_aWMac;
					ChanParam.m_byDstAddrLen = 8;
					ChanParam.m_pDatBuf = DatBuf;
					ChanParam.m_wDatBufLen = DatBufLen;
					ChanParam.m_dwRepeatCnt = 20; 
					ChanParam.m_dwScheTime = 100; 
					
					InterChanSend(&ChanParam);	
			}
		}
}

//����֪ͨ
void HardCtrl_PowerOn_Process(void)
{
    uint8_t OnSta = 1;

    
    ResComTask_SendToUpcom(&OnSta, 1, 0x19, CTRL_DI);
}

//λ��ͨ��
void HardCtrl_LocationNoti_Process(void)
{
    uint8_t Buf[30];
    uint8_t Off = 0;

    memcpy((void *)&Buf[Off],(void *)UserParam_GetLPos(),sizeof(STRUCT_LPOS));
    Off += sizeof(STRUCT_LPOS);
    memcpy((void *)&Buf[Off],(void *)UserParam_GetTime1(),sizeof(STRUCT_TIME1));
    Off += sizeof(STRUCT_TIME1);
    Buf[Off++] = UserParam_GetPosType();
    Buf[Off++] = UserParam_GetSatNum();
    //����֪ͨ
    ResComTask_SendToUpcom(Buf, Off, 0xF1, CTRL_DAT);
}

//ͷ��״̬ͨ��
void HardCtrl_HelmetNoti_Process(void)
{
    uint8_t Buf[30];
    uint8_t Off = 0;

    SINGLE_SLAVER_MAG *pSingleMag;

    pSingleMag = SlaverMag_GetHandleByType(0xFE,0x08);//�ҵ�ͷ��
    Buf[Off++] = pSingleMag->m_byIsOnline;
    Buf[Off++] = UserParam_GetActLaserCnt1();
    Buf[Off++] = GetBatRemainingCapacity_18650(pSingleMag->m_wBatVol, 1);
   
    //����֪ͨ
    ResComTask_SendToUpcom(Buf, Off, 0xE0, CTRL_DAT);
}

void HardStatusDetection(void)
{
		RESCOMTASK_MAG *pMag = &g_sResComTask_Mag;
		SINGLE_SLAVER_MAG *pSlaverMag = NULL;
	
		FRAME_BUF *pFrame = NULL;
		uint32_t Flg = 0;
		uint8_t cut = 0, Xor = 0, TsChar1;
				
		if(pMag->m_IsAccess == TRUE)
		{
				pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

				if(pFrame == NULL)
				{
						return;
				}

				Flg = g_sResComTask_Mag.DrillParam.DisableDrillStartVoice	<< 7 |
							g_sResComTask_Mag.DrillParam.LocationShowType << 6 |
							g_sResComTask_Mag.DrillParam.DisableAssistLaser << 5 |
							g_sResComTask_Mag.DrillParam.EnableDiffRid << 4 |
							g_sResComTask_Mag.DrillParam.DisableInstallFume << 3 |
							g_sResComTask_Mag.DrillParam.DisableCheat << 2 |
							g_sResComTask_Mag.DrillParam.DisableKillTeammate << 1 |
							g_sResComTask_Mag.DrillParam.DisableSuicide;
				
				//���
				pFrame->m_wBufLen = 0;
				pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xA8;
				pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x90;
				pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
				pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetTType();
				pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetTSubType();
				pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetRid();

				pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetPid() >> 8;
				pFrame->m_byBuf[pFrame->m_wBufLen++] = IDParam_GetPid() & 0xFF;

				pFrame->m_byBuf[pFrame->m_wBufLen++] = FIXVERSION;
				pFrame->m_byBuf[pFrame->m_wBufLen++] = BOARDID;

				pFrame->m_byBuf[pFrame->m_wBufLen++] = GetHardSta1();
				pFrame->m_byBuf[pFrame->m_wBufLen++] = GetHardSta2();
				pFrame->m_byBuf[pFrame->m_wBufLen++] = GetHardSta3();
				pFrame->m_byBuf[pFrame->m_wBufLen++] = GetHardSta4();
				
				pFrame->m_byBuf[pFrame->m_wBufLen++] = UserParam_GetActLaserCnt() + UserParam_GetActLaserCnt1();
				pFrame->m_byBuf[pFrame->m_wBufLen++] = BatTask_GetExBatVol()>> 8;
				pFrame->m_byBuf[pFrame->m_wBufLen++] = BatTask_GetExBatVol();
				memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen], g_sUserParam.m_aWMac, 8);
        pFrame->m_wBufLen += 8;
				
				pFrame->m_byBuf[pFrame->m_wBufLen++] = GetDTRssi();
				pFrame->m_byBuf[pFrame->m_wBufLen++] = UserParam_GetSlaverCnt();
				
				for(uint8_t i = 0; i < UserParam_GetSlaverCnt(); i++)
        {
						pSlaverMag = UserParam_GetSlaverMag(i);
            pFrame->m_byBuf[pFrame->m_wBufLen++] = pSlaverMag->m_byTType;
            pFrame->m_byBuf[pFrame->m_wBufLen++] = pSlaverMag->m_byTSubType;
            
            pFrame->m_byBuf[pFrame->m_wBufLen++] = 0x09;
            
						memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen], pSlaverMag->m_aWMac, 8);
            pFrame->m_wBufLen += 8;
					
						TsChar1 = pSlaverMag->m_sTsChar1.m_bLocked << 7 | 
											pSlaverMag->m_sTsChar1.m_bConnectionState << 6 |
											pSlaverMag->m_sTsChar1.m_bFailureState << 5 |
											pSlaverMag->m_sTsChar1.m_bUnitmode << 4 |
											pSlaverMag->m_sTsChar1.m_bBatteryState << 3 |
											pSlaverMag->m_sTsChar1.m_bIdentity << 1 |
											pSlaverMag->m_sTsChar1.m_bIsNight;
					
            pFrame->m_byBuf[pFrame->m_wBufLen++] = TsChar1;
            
            pFrame->m_byBuf[pFrame->m_wBufLen++] = pSlaverMag->m_wBatVol >> 8;
            pFrame->m_byBuf[pFrame->m_wBufLen++] = pSlaverMag->m_wBatVol & 0xFF;
       }
				
				pFrame->m_byBuf[pFrame->m_wBufLen++] = Flg;
				pFrame->m_byBuf[pFrame->m_wBufLen++] = g_sResComTask_Mag.DrillParam.CfgFSHHurtDegree;;
				pFrame->m_byBuf[2] = pFrame->m_wBufLen - 3;   

				for(cut = 3; cut < pFrame->m_wBufLen; cut++)
				{
						Xor ^= pFrame->m_byBuf[cut];
				}
				pFrame->m_byBuf[pFrame->m_wBufLen++] = Xor;

				pFrame->m_dwCnt = 1;
				pFrame->m_dwID = 0xA890;
				pFrame->m_dwScheTime = 100;
				pFrame->m_bySendNum = 0;
				SComTask_SendListAppend(pMag->m_sTaskMag.m_pSendList, (void*)pFrame);
		}
}

void EnableChangeBatHandle(void )
{
		SINGLE_SLAVER_MAG *pSlaverMag = NULL;
	
		static uint16_t cnt = 0;
	
		if(cnt++ < 300)		//5���Ӽ��һ��
			return;
		
		cnt = 0;
		
		if(GetBatRemainingCapacity_18650(BatTask_GetExBatVol(), 1) < 3)
		{
			Wtn6_Play(WTN6_LowBattery_Cloth);
			Wtn6_Play(WTN6_PLS_CHANGE_BATTERY);
		}
		
    //���豸
		for(uint8_t i = 0; i < UserParam_GetSlaverCnt(); i++)
    {
				pSlaverMag = UserParam_GetSlaverMag(i);

				if(pSlaverMag->m_byIsOnline == FALSE|| pSlaverMag->m_byIsSleepFlg == TRUE)
					continue;
				
				if(pSlaverMag->m_byTType == 0x14 ||
					pSlaverMag->m_byTType == 0x31 || 
					pSlaverMag->m_byTType == 0x32 ||
					pSlaverMag->m_byTType == 0x33 ||
					pSlaverMag->m_byTType == 0x34 ||
					pSlaverMag->m_byTType == 0x36 ||
					pSlaverMag->m_byTType == 0x37 ||
					pSlaverMag->m_byTType == 0x38 ||
					pSlaverMag->m_byTType == 0x39 ||
					pSlaverMag->m_byTType == 0x40)
				{
						if(pSlaverMag->m_wBatVol <= 1550)
						{
								Wtn6_Play(WTN6_LowBattery_IR);
								Wtn6_Play(WTN6_PLS_CHANGE_BATTERY);
						}
				}
				else if(pSlaverMag->m_byTType == 0xFE && pSlaverMag->m_byTSubType == 0x08)
				{
						if(pSlaverMag->m_wBatVol <= 3100)
						{
								Wtn6_Play(WTN6_LowBattery_Helmet);
								Wtn6_Play(WTN6_PLS_CHANGE_BATTERY);
						}
				}
		}
}

//Ӳ������Э�鴦��55AAЭ��0�����룩
void Prot_HardCtrl_Analysis(uint8_t *pBuf, uint16_t BufLen, void *pHandle)
{
    RESCOMTASK_MAG *pMag = (RESCOMTASK_MAG *)pHandle;
    uint8_t DevIdx, DevCtrl, Offset;
    DevIdx = pBuf[1];
    DevCtrl = pBuf[2];


    switch(pBuf[0])
    {

        case 0x0:
            switch(DevIdx)
            {

                case 0x05:
                    //ComTask_RetransmissionData(LaserComTask_TaskHandleGet(), &pBuf[3], BufLen - 3);
                    
                    //�漰ͷ��̽ͷ����
                    ProtDeal_HardCtrl_48k(&pBuf[0], BufLen);
                    break;

                case 0x07:
                    ComTask_RetransmissionData(IR38ComTask_TaskHandleGet(), &pBuf[3], BufLen - 3);
                    break;

                case 0x13:
                    SComTask_RetransmissionData(HmiComTask_TaskHandleGet(), &pBuf[3], BufLen - 3);
                    break;

                //��Դ
                case 0x19:
                    ProtDeal_HardCtrl_Power(&pBuf[1], BufLen - 1);
                    break;

                //���е�
                case 0x01:
                    ProtDeal_HardCtrl_RunLed(&pBuf[1], BufLen - 1);
                    break;

                //����оƬ
                case 0x0D:
                case 0x0E:
                case 0x0F:
                    ProtDeal_HardCtrl_Voice(&pBuf[1], BufLen - 1);
                    break;

                //���
                case 0x04:
                    ProtDeal_HardCtrl_Motor(&pBuf[1], BufLen - 1);
                    break;

                //���̹�1
                case 0xA0:
                    ProtDeal_HardCtrl_Fume(&pBuf[1], BufLen - 1);
                    break;


                    #if 0

                case 0x0A:
                    ComTask_RetransmissionData(&g_sZigComTask_Mag.m_sTaskMag, &pBuf[3], BufLen - 3);
                    break;

                case 0x0B:
                    ComTask_RetransmissionData(&g_sGpsComTask_Mag.m_sTaskMag, &pBuf[3], BufLen - 3);
                    break;
                    #endif

                default:
                    break;
            }

            break;


        default:
            break;
    }


}

//ͷ��Ӳ�����ƻ�������
void HardCtrl_HelmetBase(uint8_t *pBuf, uint16_t BufLen)
{
    SINGLE_SLAVER_MAG *pSingleMag;

    pSingleMag = SlaverMag_GetHandleByType(0xFE,0x08);//�ҵ�ͷ��
    if(NULL != pSingleMag && FALSE == pSingleMag->m_byIsSleepFlg)
    {
        InterConn_SlaverHardCtrl(CHAN_ZIGBEE,pSingleMag->m_aWMac,sizeof(pSingleMag->m_aWMac),pBuf,BufLen);
    }
}


//����ͷ�����̹޷���
void CtrlHelmetFume(DEVCTRL eCtrl, uint16_t Tim100Ms)
{
    uint8_t Off,Buf[20];
    uint32_t Tim = Tim100Ms;
    
    Off = 0;
    Buf[Off++] = 0x0;
    Buf[Off++] = 0xA0;
    Buf[Off++] = 0x03;
    Buf[Off++] = (uint8_t)eCtrl;
    memcpy((void *)&Buf[Off],(void *)&Tim100Ms,4);
    Off += 4;
    
    HardCtrl_HelmetBase(Buf,Off);
}

//����ͷ��̽ͷ��
void CtrlHelmetLaserOff()
{
    uint8_t Off,Buf[20];
    
    Off = 0;
    Buf[Off++] = 0x00;
    Buf[Off++] = 0x05;
    Buf[Off++] = (uint8_t)CTRL_SEND;
    Buf[Off++] = 0x5A;
    Buf[Off++] = 0x06;
    Buf[Off++] = 0x30;
    Buf[Off++] = 0x00;		// 1-����0-��
    Buf[Off++] = 0xFF;		// ̽ͷ��ţ���1��ʼ
		//Buf[Off++] = 0;
		//Buf[Off++] = 0;
		//Buf[Off++] = 0;
		//Buf[Off++] = 0;
		//Buf[Off++] = 0;
    Buf[Off++] = 0xAA;
    
		HardCtrl_HelmetBase(Buf,Off);
}

//����ͷ��̽ͷ��
void CtrlHelmetLaserOn()
{
    uint8_t Off,Buf[20];
    
    Off = 0;
    Buf[Off++] = 0x00;
    Buf[Off++] = 0x05;
    Buf[Off++] = (uint8_t)CTRL_SEND;
    Buf[Off++] = 0x5A;
    Buf[Off++] = 0x06;
    Buf[Off++] = 0x30;
    Buf[Off++] = 0x01;		// 1-����0-��
    Buf[Off++] = 0xFF;		// ̽ͷ��ţ���1��ʼ
		//Buf[Off++] = 1;
		//Buf[Off++] = 1;
		//Buf[Off++] = 1;
		//Buf[Off++] = 1;
		//Buf[Off++] = 1;
    Buf[Off++] = 0xAA;
    
		HardCtrl_HelmetBase(Buf,Off);
}

//���Ƶ���ͷ��̽ͷ��
void CtrlHelmetLaserOnOff(uint8_t LedIdx, uint8_t IsOn)
{
    uint8_t Off,Buf[20];
    
    Off = 0;
    Buf[Off++] = 0x00;
    Buf[Off++] = 0x05;
    Buf[Off++] = (uint8_t)CTRL_SEND;
    Buf[Off++] = 0x5A;
    Buf[Off++] = 0x06;
    Buf[Off++] = 0x30;
    Buf[Off++] = IsOn?1:0;// 1-����0-��
    Buf[Off++] = LedIdx; // ̽ͷ��ţ���1��ʼ
    Buf[Off++] = 0xAA;
    
		HardCtrl_HelmetBase(Buf,Off);
}

//����ͷ��������
void CtrlHelmetVoice(uint8_t VoiceIdx)
{
    uint8_t Off,Buf[20];
    
    Off = 0;
    Buf[Off++] = 0x00;
    Buf[Off++] = 0x0F;
    Buf[Off++] = (uint8_t)CTRL_DO;
    Buf[Off++] = 0x01;
    Buf[Off++] = VoiceIdx;
    
		HardCtrl_HelmetBase(Buf,Off);
}

void CtrlManoParamIndividualSoldier_EventData_Spell(StructEvent_RSP_CtrlManoParam_IndividualSoldier *pMag, u8 *pBuf, u8 *Len)
{
  u8 num = 0;

  pBuf[num++] = BNEventType_RSP_CtrlManoParam_IndividualSoldier;		//�¼�����
  pBuf[num++] = pMag->m_byT_Type;
  pBuf[num++] = pMag->m_byT_Subtype;
  pBuf[num++] = pMag->m_byRID;
  pBuf[num++] = pMag->m_aPID[0];
  pBuf[num++] = pMag->m_aPID[1];
  
  memcpy(&pBuf[num],&pMag->m_sRecCmdTime,sizeof(STRUCT_TIME1));
  num += sizeof(STRUCT_TIME1);
  memcpy(&pBuf[num],&pMag->m_sStartTime,sizeof(STRUCT_TIME1));
  num += sizeof(STRUCT_TIME1);
  
  pBuf[num++] = pMag->m_byST_Type;
  pBuf[num++] = pMag->m_byST_Subtype;
  pBuf[num++] = pMag->m_byS_RID;
  pBuf[num++] = pMag->m_aS_PID[0];
  pBuf[num++] = pMag->m_aS_PID[1];
  
  memcpy(&pBuf[num],&pMag->m_sDayStartTime,sizeof(STRUCT_TIME2));
  num += sizeof(STRUCT_TIME2);
  memcpy(&pBuf[num],&pMag->m_sNightStartTime,sizeof(STRUCT_TIME2));
  num += sizeof(STRUCT_TIME2);

  pBuf[num++] = pMag->m_byKey;
  pBuf[num++] = pMag->m_byBR_Attribute;
  pBuf[num++] = pMag->m_aBulletMaxAmmuNum[0];
  pBuf[num++] = pMag->m_aBulletMaxAmmuNum[1];
  pBuf[num++] = pMag->m_aBulletMaxAmmuNum[2];
  pBuf[num++] = pMag->m_aShellMaxAmmuNum[0];
  pBuf[num++] = pMag->m_aShellMaxAmmuNum[1];
  pBuf[num++] = pMag->m_aShellMaxAmmuNum[2];
  pBuf[num++] = pMag->m_byWeaponNum;
  
	for(u8 w = 0; w < pMag->m_byWeaponNum; w++)
	{
			pBuf[num++] = pMag->m_sWeapon[w].m_byW_Type;
			pBuf[num++] = pMag->m_sWeapon[w].m_byW_Subtype;
			pBuf[num++] = pMag->m_sWeapon[w].m_byAmmuNum;

			for(u8 b = 0; b < pMag->m_sWeapon[w].m_byAmmuNum; b++)
			{
					pBuf[num++] = pMag->m_sWeapon[w].m_aAmmu[b].m_byBullet_Type;
					pBuf[num++] = pMag->m_sWeapon[w].m_aAmmu[b].m_byBullet_SubType;
					pBuf[num++] = pMag->m_sWeapon[w].m_aAmmu[b].m_aBulletCfgNum[0];
					pBuf[num++] = pMag->m_sWeapon[w].m_aAmmu[b].m_aBulletCfgNum[1];
					pBuf[num++] = pMag->m_sWeapon[w].m_aAmmu[b].m_aBulletCfgNum[2];
					pBuf[num++] = pMag->m_sWeapon[w].m_aAmmu[b].m_byAmmuWearDepth;
					pBuf[num++] = pMag->m_sWeapon[w].m_aAmmu[b].m_byAmmuConcreteDepth;
					pBuf[num++] = pMag->m_sWeapon[w].m_aAmmu[b].m_byAmmuRange;
			}
  }

	pBuf[num++] = pMag->m_byEP_Num[0];
	pBuf[num++] = pMag->m_byEP_Num[1];
	pBuf[num++] = pMag->m_byEP_Num[2];
    
  pBuf[num++] = pMag->m_byThickness;
  
	memcpy(&pBuf[num],&pMag->m_sEndTime,sizeof(STRUCT_TIME1));
  num += sizeof(STRUCT_TIME1);
  *Len = num;  
}

/****************************************************************
@FUNCTION���º�̨��ϰ�������óɹ��¼��ϴ�
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��LW
@SPECIAL:
****************************************************************/
void CtrlManoParamEventUpload_Send(Struct_EventUpload *pMag)
{
	RESCOMTASK_MAG *pMag1 = &g_sResComTask_Mag;
	FRAME_BUF *pFrame = NULL;
	uint8_t cut = 0,Len = 0;

	pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

	if(pFrame == NULL)
	{
			return;
	}

  //֡ͷ��ֵ
	pMag->m_aData_Frame[0] = 0xA8;
	pMag->m_aData_Frame[1] = BNCmd_Terminal_EventUpload;

	pFrame->m_wBufLen = 0;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[0];
	pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[1];
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;

  //�¼��������
	memset(BackStage_dataBuf,0,sizeof(BackStage_dataBuf));						
	CtrlManoParamIndividualSoldier_EventData_Spell(&pMag->Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier,BackStage_dataBuf,&Len);
	memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen], BackStage_dataBuf, Len);
	pFrame->m_wBufLen += Len;
	
  //�¼�֡���к�
	pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[0];
	pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[1];

  //��������
	pMag->m_byXor = 0;
	for(cut = 3; cut < pFrame->m_wBufLen - 2; cut++)		//�¼����кŲ�����У��
	{
		pMag->m_byXor ^= pFrame->m_byBuf[cut];
	}
	pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_byXor;

  //����֡���ݳ���  
	pFrame->m_byBuf[2] = pFrame->m_wBufLen - 6;	
	
	pFrame->m_dwCnt = 1;
	pFrame->m_dwID = (pFrame->m_byBuf[0] << 24 | pFrame->m_byBuf[1] << 16 |
										pFrame->m_byBuf[2] << 8 | pFrame->m_byBuf[3]);
	pFrame->m_dwScheTime = 1000;
	pFrame->m_bySendNum = 0;
	pFrame->m_dwTime = GetSysRunMilliSecond();

	SComTask_SendListAppend(pMag1->m_sTaskMag.m_pSendList, (void*)pFrame);
	g_sResComTask_Mag.EvtListResPonseNum[pMag->m_aES_Num[0] << 8 | pMag->m_aES_Num[1]] = TRUE;
	
	if(g_sUserParam.m_byMode == GAME_GOING)
	{
		g_sResComTask_Mag.EvtRspFlg[g_sResComTask_Mag.m_dwCtrlEvtNum / 8] |= (1 << (g_sResComTask_Mag.m_dwCtrlEvtNum % 8));
	
		if(g_sResComTask_Mag.m_dwCtrlEvtNum < 240)
		{
			EventStorageToFlash(pFrame, sizeof(FRAME_BUF), g_sResComTask_Mag.m_dwCtrlEvtNum);
		}
	}
}

void CtrlManoParamEventUploadHandle(Struct_CtrlManoParam_IndividualSoldierDownload *ManoParam)
{
    //������ֵ
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_byT_Type = IDParam_GetTType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_byT_Subtype = IDParam_GetTSubType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_byRID = IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_aPID[0] = IDParam_GetPid() >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_aPID[1] = IDParam_GetPid() & 0xff;
    memcpy(&g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_sRecCmdTime, &g_sResComTask_Mag.m_sCmdTime.m_BSNulCtrlManoeuvre, sizeof(STRUCT_TIME1)); //����ָ��ʱ��
    memcpy(&g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_sStartTime, &ManoParam->m_sStartTime, sizeof(STRUCT_TIME1)); //��ϰ��ʼʱ����
		memcpy(&g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_sEndTime, &ManoParam->m_sEndTime, sizeof(STRUCT_TIME1));   	 //��ϰ����ʱ����

    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_byST_Type = ManoParam->m_byMasterT_Type;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_byST_Subtype = ManoParam->m_byMasterT_Subtype;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_byS_RID = ManoParam->m_byMasterRID;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_aS_PID[0] = ManoParam->m_wMasterPID >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_aS_PID[1] = ManoParam->m_wMasterPID & 0xff;
    memcpy(&g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_sDayStartTime, &ManoParam->m_sDayStartTime, sizeof(STRUCT_TIME2));
    memcpy(&g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_sNightStartTime, &ManoParam->m_sNightStartTime, sizeof(STRUCT_TIME2));
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_byKey = ManoParam->m_byKey;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_byBR_Attribute = ManoParam->m_byBR_Attribute;
    memcpy(&g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_aBulletMaxAmmuNum, &ManoParam->m_byBulletMaxAmmuNum, 3);
    memcpy(&g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_aShellMaxAmmuNum, &ManoParam->m_byShellMaxAmmuNum, 3);
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_byWeaponNum = ManoParam->m_byAmmuNum;

    for(u8 w = 0; w < g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_byWeaponNum; w++)
    {
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_sWeapon[w].m_byW_Type =  ManoParam->m_sAmmu[w].m_byWeapon_Type;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_sWeapon[w].m_byW_Subtype = ManoParam->m_sAmmu[w].m_byWeapon_SubType;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_sWeapon[w].m_byAmmuNum = ManoParam->m_sAmmu[w].m_byCfgBulletKind;

        for(u8 b = 0; b < g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_sWeapon[w].m_byAmmuNum; b++)
        {
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_sWeapon[w].m_aAmmu[b].m_byBullet_Type = ManoParam->m_sAmmu[w].m_sBulletInfo[b].m_byBullet_Type;
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_sWeapon[w].m_aAmmu[b].m_byBullet_SubType = ManoParam->m_sAmmu[w].m_sBulletInfo[b].m_byBullet_SubType;
            memcpy(&g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_sWeapon[w].m_aAmmu[b].m_aBulletCfgNum, &ManoParam->m_sAmmu[w].m_sBulletInfo[b].m_aBulletCfgNum, 3);
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_sWeapon[w].m_aAmmu[b].m_byAmmuWearDepth = ManoParam->m_sAmmu[w].m_sBulletInfo[b].m_byAmmuWearDepth;
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_sWeapon[w].m_aAmmu[b].m_byAmmuConcreteDepth = ManoParam->m_sAmmu[w].m_sBulletInfo[b].m_byAmmuConcreteDepth;
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_sWeapon[w].m_aAmmu[b].m_byAmmuRange = ManoParam->m_sAmmu[w].m_sBulletInfo[b].m_byAmmuRange;
        }
    }

		memcpy(&g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_byEP_Num, &ManoParam->EP_Num, 3);
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_byThickness = ManoParam->Thickness;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;

    CtrlManoParamEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}

/****************************************************************
@FUNCTION���º�̨�ϴ��������óɹ��¼��������
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��LW
@SPECIAL:
****************************************************************/
void RSP_CtrlIntervalEventData_Spell(StructEvent_RSP_CtrlInterval *pMag,u8 *pBuf,u8 *Len)
{
	uint8_t num = 0;

  //���
	pBuf[num++] = BNEventType_RSP_CtrlInterval;
	pBuf[num++] = pMag->m_byT_Type;
	pBuf[num++] = pMag->m_byT_Subtype;
	pBuf[num++] = pMag->m_byRID;
	pBuf[num++] = pMag->m_aPID[0];
	pBuf[num++] = pMag->m_aPID[1];
	
	memcpy(&pBuf[num],&pMag->m_sRecCmdTime,sizeof(STRUCT_TIME1));
	num += sizeof(STRUCT_TIME1);
	
	pBuf[num++] = pMag->m_byST_Type;
	pBuf[num++] = pMag->m_byST_Subtype;
	pBuf[num++] = pMag->m_byS_RID;
	pBuf[num++] = pMag->m_aS_PID[0];
	pBuf[num++] = pMag->m_aS_PID[1];

	pBuf[num++] = pMag->m_bySelfStatusDataUploadCycle;
	pBuf[num++] = pMag->m_byDynamometerDisplayStatusDataUploadCycle;
	pBuf[num++] = pMag->m_byEquipmentAndIdentityDataUploadCycle;

	*Len = num;
}

/****************************************************************
@FUNCTION���º�̨�������óɹ��¼��ϴ�
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��LW
@SPECIAL:
****************************************************************/
void RSP_CtrlIntervalEventUpload_Send(Struct_EventUpload *pMag)
{
	RESCOMTASK_MAG *pMag1 = &g_sResComTask_Mag;
	FRAME_BUF *pFrame = NULL;
	
	uint8_t cut = 0,Len = 0;

	pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

	if(pFrame == NULL)
	{
			return;
	}
			
	memset(BackStage_dataBuf,0,sizeof(BackStage_dataBuf));						

	pMag->m_byXor = 0;
	pMag->m_aData_Frame[0] = 0xA8;
	pMag->m_aData_Frame[1] = BNCmd_Terminal_EventUpload;

	pFrame->m_wBufLen = 0;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[0];
	pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[1];
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
	
	RSP_CtrlIntervalEventData_Spell(&pMag->Event_Pack.m_sRSP_CtrlInterval, BackStage_dataBuf, &Len);
	memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen], BackStage_dataBuf, Len);
	pFrame->m_wBufLen += Len;
	
	pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[0];
	pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[1];

	for(cut = 3 ;cut < pFrame->m_wBufLen - 2; cut++)
	{
		pMag->m_byXor ^= pFrame->m_byBuf[cut];
	}
	pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_byXor;
			
	pFrame->m_byBuf[2] = pFrame->m_wBufLen - 6;
	
	pFrame->m_dwCnt = 1;
	pFrame->m_dwID = (pFrame->m_byBuf[0] << 24 | pFrame->m_byBuf[1] << 16 |
										pFrame->m_byBuf[2] << 8 | pFrame->m_byBuf[3]);
	pFrame->m_dwScheTime = 1000;
	pFrame->m_bySendNum = 0;
	pFrame->m_dwTime = GetSysRunMilliSecond();
	SComTask_SendListAppend(pMag1->m_sTaskMag.m_pSendList, (void*)pFrame);
	g_sResComTask_Mag.EvtListResPonseNum[pMag->m_aES_Num[0] << 8 | pMag->m_aES_Num[1]] = TRUE;
	
	if(g_sUserParam.m_byMode == GAME_GOING)
	{
		g_sResComTask_Mag.EvtRspFlg[g_sResComTask_Mag.m_dwCtrlEvtNum / 8] |= (1 << (g_sResComTask_Mag.m_dwCtrlEvtNum % 8));
	
		if(g_sResComTask_Mag.m_dwCtrlEvtNum < 240)
		{
			EventStorageToFlash(pFrame, sizeof(FRAME_BUF), g_sResComTask_Mag.m_dwCtrlEvtNum);
		}
	}
}

//����δ�ϴ��¼����
void UnUpLoadEvtNumCalcu(void)
{
		uint8_t Num_CurrentNoRsp = 0, Num_LastNoRsp = 0;

		for(uint8_t i = 0; i < sizeof(g_sResComTask_Mag.EvtRspFlgBak) / sizeof(g_sResComTask_Mag.EvtRspFlgBak[0]); i++)
    {
        if(g_sResComTask_Mag.EvtRspFlg[i] != 0)
        {
            for(uint8_t j = 0; j < 8; j++)
            {
                if((g_sResComTask_Mag.EvtRspFlg[i] >> j) & 0x01)
                {
                    Num_LastNoRsp++;
                }
            }
        }
    }
		
		if(Num_LastNoRsp != 0)
		{
			g_sResComTask_Mag.UnUpLoadEvtCnt = Num_LastNoRsp;
			return;
		}
		
    for(uint8_t i = 0; i < sizeof(g_sResComTask_Mag.EvtRspFlg) / sizeof(g_sResComTask_Mag.EvtRspFlg[0]); i++)
    {
        if(g_sResComTask_Mag.EvtRspFlg[i] != 0)
        {
            for(uint8_t j = 0; j < 8; j++)
            {
                if((g_sResComTask_Mag.EvtRspFlg[i] >> j) & 0x01)
                {
                    Num_CurrentNoRsp++;
                }
            }
        }
    }
		
		g_sResComTask_Mag.UnUpLoadEvtCnt = Num_LastNoRsp;
}

uint8_t SynchronizeState(uint8_t ModeFlg, uint8_t IsBatteryLow)
{
    uint8_t Xor = 0;

		g_sResComTask_Mag.BackupArea[0].HeadFlg[0] = 0x5A;
    g_sResComTask_Mag.BackupArea[0].HeadFlg[1] = 0xA5;
    g_sResComTask_Mag.BackupArea[0].Blood = UserParam_GetBlood();
    g_sResComTask_Mag.BackupArea[0].IsDrill = ModeFlg;
    g_sResComTask_Mag.BackupArea[0].IsEnablePowerOff = g_sUserParam.m_byMode == TRAIN ? 1 : IsBatteryLow;
    g_sResComTask_Mag.BackupArea[0].m_byWeaponNum = g_sUserParam.m_byBackstageCfgWeaponNum;
    g_sResComTask_Mag.BackupArea[0].IsNight = UserParam_GetDayOrNight();
    g_sResComTask_Mag.BackupArea[0].Belong = UserParam_GetBRChar();
	
    g_sResComTask_Mag.BackupArea[0].RptInterval = g_sResComTask_Mag.m_dwRptInterval;
    //UnUpLoadEvtNumCalcu();
		g_sResComTask_Mag.BackupArea[0].NoRspEvtNum = g_sResComTask_Mag.UnUpLoadEvtCnt;
    g_sResComTask_Mag.BackupArea[0].ES_Num = g_sResComTask_Mag.m_dwCtrlEvtNum;
    g_sResComTask_Mag.BackupArea[0].Key = UserParam_GetKey();
    g_sResComTask_Mag.BackupArea[0].T_Subtype = IDParam_GetTSubType();
		g_sResComTask_Mag.BackupArea[0].Vios.DetectorLoss = g_sResComTask_Mag.Vios.DetectorLoss;
    g_sResComTask_Mag.BackupArea[0].Vios.FumeUnInstall = g_sResComTask_Mag.Vios.FumeUnInstall;
    g_sResComTask_Mag.BackupArea[0].Vios.HelmetLoss = g_sResComTask_Mag.Vios.HelmetLoss;
    g_sResComTask_Mag.BackupArea[0].Vios.IllegalMove = g_sResComTask_Mag.Vios.IllegalMove;

    //δ�ϴ��¼���Ӧ���
		memcpy(g_sResComTask_Mag.BackupArea[0].EvtRspFlgBak, g_sResComTask_Mag.EvtRspFlg, sizeof(g_sResComTask_Mag.EvtRspFlg));

    //��ʼ����ʱ��
    memcpy(&g_sResComTask_Mag.BackupArea[0].m_sManoStart, &g_sUserParam.m_sManoStart, sizeof(STRUCT_TIME1));
    //��ϰ����ʱ��
		memcpy(&g_sResComTask_Mag.BackupArea[0].m_sManoEnd, &g_sUserParam.m_sManoEnd, sizeof(STRUCT_TIME1));
    //�л����������ʼʱ��
    memcpy(&g_sResComTask_Mag.BackupArea[0].m_sDayTime, &g_sUserParam.m_sDayStartTime, sizeof(STRUCT_TIME2));
    //�л�����ҹ����ʼʱ��
    memcpy(&g_sResComTask_Mag.BackupArea[0].m_sNightTime, &g_sUserParam.m_sNightStartTime, sizeof(STRUCT_TIME2));

		memcpy(&g_sResComTask_Mag.BackupArea[0].DrillParam, &g_sResComTask_Mag.DrillParam, sizeof(DRILL_PARAM));
    memcpy(&g_sResComTask_Mag.BackupArea[0].Shell, g_sLaserComTask_Mag.Shell, sizeof(u16)*PARTITION_MAX);
    memcpy(g_sResComTask_Mag.BackupArea[0].EP_Num, &g_sUserParam.EP_Num, 3);
  
		g_sResComTask_Mag.BackupArea[0].InjuryCnt = g_sHmiComTask_Mag.m_byHurtCountFlg;

		for(uint8_t i = 0; i < g_sHmiComTask_Mag.m_byHurtCountFlg; i++)
		{
				g_sResComTask_Mag.BackupArea[0].m_byInjuryPart[i] = g_sHmiComTask_Mag.m_sInjury[i].m_byPart;
		}

		g_sResComTask_Mag.BackupArea[0].InjuryKind = g_sHmiComTask_Mag.m_byHurtKind;

		if(UserParam_GetBlood() != 100)
		{
				//̽ͷ�ƴ��ָ�
				for(u8 i = 0; i < UserParam_GetActLaserCnt(); i++)
				{
						g_sResComTask_Mag.BackupArea[0].Decter[i] = g_sLaserComTask_Mag.m_aLedCtrlMag[i].m_bIsCtrlOn;
				}
		}
		
    for(uint8_t i = 0; i < g_sResComTask_Mag.BackupArea[0].m_byWeaponNum; i++)
    {
				g_sResComTask_Mag.BackupArea[0].WeaponInfo[i].m_byWeapon_Type = g_sUserParam.m_saBackstageCfgAmmuInfo[i].m_byWeapon_Type;
        g_sResComTask_Mag.BackupArea[0].WeaponInfo[i].m_byWeapon_SubType = g_sUserParam.m_saBackstageCfgAmmuInfo[i].m_byWeapon_SubType;
        g_sResComTask_Mag.BackupArea[0].WeaponInfo[i].m_byCfgBulletKind = g_sUserParam.m_saBackstageCfgAmmuInfo[i].m_byCfgBulletKind;
        g_sResComTask_Mag.BackupArea[0].WeaponInfo[i].m_byState = g_sUserParam.m_saBackstageCfgAmmuInfo[i].m_byState;
        memcpy(&g_sResComTask_Mag.BackupArea[0].WeaponInfo[i].m_sBulletInfo[0], &g_sUserParam.m_saBackstageCfgAmmuInfo[i].m_sBulletInfo[0], sizeof(BULLETINFO_T));
    }

    //����У���
    for(uint32_t i = 0; i < sizeof(PARAMATER_BACKUP) - 1; i++)
    {
        Xor ^= ((uint8_t*)(&g_sResComTask_Mag.BackupArea[0]))[i];
    }

    g_sResComTask_Mag.BackupArea[0].XOR = Xor;

}

void TrainModeInit(void)
{
    g_sUserParam.m_byMode = TRAIN;
		Wtn6_Play(WTN6_Mode_Train);
		g_sUserParam.m_sTsChar1.m_bUnitmode = FALSE;
		g_sUserParam.m_sTsChar1.m_bLocked = FALSE;
	
		g_sUserParam.m_sLPosition.m_byLposState = FALSE;
	
    g_sUserParam.m_eStatus = Sta_Normal;
	
    UserParam_SetBlood(100);
	  UserParam_SetBRChar(BRCHAR_YELLOW);
    UserParam_SetKey(0);
		UserParam_SetDayOrNight(FALSE);
	
		UserParam_SetActLaserCnt(0);
		//��Ա���³�
		g_sUserParam.m_sBroadcastInfo.m_byRecBeacon = FALSE;
		g_sUserParam.m_sBroadcastInfo.m_byIsGetOn = 0;
		g_sUserParam.m_sBroadcastInfo.m_byVehicleTType = 0;
		g_sUserParam.m_sBroadcastInfo.m_byVehicleTSubtype	= 0;
		g_sUserParam.m_sBroadcastInfo.m_byVehicleRID = 0;
		g_sUserParam.m_sBroadcastInfo.m_wVehiclePID = 0;
		g_sUserParam.m_sBroadcastInfo.m_byTargetVehicleTType = 0;
		g_sUserParam.m_sBroadcastInfo.m_byTargetVehicleTSubtype = 0;
		g_sUserParam.m_sBroadcastInfo.m_byTargetVehicleRID = 0;
		g_sUserParam.m_sBroadcastInfo.m_wTargetVehiclePID = 0;
		
		//����ɼ�
		g_sInjuryMag.m_byRcdCnt = 0;
		
		g_sUserParam.Thickness = 0;
    g_sUserParam.m_IsIllegalPowerOff = FALSE;

    //��ϰ��ʼʱ��
    memset(&g_sUserParam.m_sManoStart, 0, sizeof(STRUCT_TIME1));
    //��ϰ����ʱ��
    memset(&g_sUserParam.m_sManoEnd, 0, sizeof(STRUCT_TIME1));

    memset(&g_sUserParam.EP_Num, 0, 3);

    g_sResComTask_Mag.UnUpLoadEvtCnt  = 0;
    g_sResComTask_Mag.m_dwCtrlEvtNum  = 0;

    //���δ��Ӧ�¼����
    memset(g_sResComTask_Mag.EvtRspFlg, 0, sizeof(g_sResComTask_Mag.EvtRspFlg));
		memset(g_sResComTask_Mag.EvtRspFlgBak, 0, sizeof(g_sResComTask_Mag.EvtRspFlg));

    g_sResComTask_Mag.Vios.DetectorLoss = FALSE;
    g_sResComTask_Mag.Vios.FumeUnInstall = FALSE;
    g_sResComTask_Mag.Vios.HelmetLoss = FALSE;
    g_sResComTask_Mag.Vios.IllegalMove = FALSE;
}

void AssignStateFromBuffer(PARAMATER_BACKUP *BK)
{
    //���������ֵͬ��
    if(g_sResComTask_Mag.BackupArea[0].HeadFlg[0] == 0x5A && g_sResComTask_Mag.BackupArea[0].HeadFlg[1] == 0xA5)
    {
        memcpy(&g_sResComTask_Mag.BackupArea[1], &g_sResComTask_Mag.BackupArea[0], sizeof(PARAMATER_BACKUP));
    }
    else if(g_sResComTask_Mag.BackupArea[1].HeadFlg[0] == 0x5A && g_sResComTask_Mag.BackupArea[1].HeadFlg[1] == 0xA5)
    {
        memcpy(&g_sResComTask_Mag.BackupArea[0], &g_sResComTask_Mag.BackupArea[1], sizeof(PARAMATER_BACKUP));
    }

		/*
    //�ж���ϰ�����ǲ���0
    if(BK->EP_Num[0] == 0 && BK->EP_Num[1] == 0 && BK->EP_Num[2] == 0)
    {
        TrainModeInit();
        return;
    }
		*/
    //Ϊѵ��ģʽ�¹ػ�
    if(BK->IsDrill == TRAIN)
    {
        g_sUserParam.m_byMode = TRAIN;
				Wtn6_Play(WTN6_Mode_Train);
				g_sUserParam.m_sTsChar1.m_bUnitmode = FALSE;
				g_sUserParam.m_sTsChar1.m_bLocked = FALSE;
				g_sUserParam.m_sTsChar1.m_bFailureState = FALSE;
        
				g_sUserParam.m_sLPosition.m_byLposState = FALSE;
				//��Ա���³�
				g_sUserParam.m_sBroadcastInfo.m_byRecBeacon = FALSE;
				g_sUserParam.m_sBroadcastInfo.m_byIsGetOn = 0;
				g_sUserParam.m_sBroadcastInfo.m_byVehicleTType = 0;
				g_sUserParam.m_sBroadcastInfo.m_byVehicleTSubtype	= 0;
				g_sUserParam.m_sBroadcastInfo.m_byVehicleRID = 0;
				g_sUserParam.m_sBroadcastInfo.m_wVehiclePID = 0;
				g_sUserParam.m_sBroadcastInfo.m_byTargetVehicleTType = 0;
				g_sUserParam.m_sBroadcastInfo.m_byTargetVehicleTSubtype = 0;
				g_sUserParam.m_sBroadcastInfo.m_byTargetVehicleRID = 0;
				g_sUserParam.m_sBroadcastInfo.m_wTargetVehiclePID = 0;
				//�¼��ϴ�
				g_sInjuryMag.m_byRcdCnt = 0;
			
				g_sUserParam.m_eStatus = Sta_Normal;
        UserParam_SetBlood(100);
        UserParam_SetBRChar(BRCHAR_YELLOW);
			  UserParam_SetKey(0);
				UserParam_SetDayOrNight(FALSE);
				UserParam_SetActLaserCnt(0);
			
        g_sUserParam.Thickness = 0;

				g_sUserParam.m_IsIllegalPowerOff = FALSE;
        g_sResComTask_Mag.Vios.DetectorLoss = FALSE;
        g_sResComTask_Mag.Vios.FumeUnInstall = FALSE;
        g_sResComTask_Mag.Vios.HelmetLoss = FALSE;
        g_sResComTask_Mag.Vios.IllegalMove = FALSE;

        //��ϰ��ʼʱ��
        memcpy(&g_sUserParam.m_sManoStart, &g_sResComTask_Mag.BackupArea[0].m_sManoStart, sizeof(STRUCT_TIME1));
        //��ϰ����ʱ��
        memcpy(&g_sUserParam.m_sManoEnd, &g_sResComTask_Mag.BackupArea[0].m_sManoEnd, sizeof(STRUCT_TIME1));

        //�л����������ʼʱ��
        memcpy(&g_sUserParam.m_sDayStartTime, &BK->m_sDayTime, sizeof(Struct_Time2));
        //�л�����ҹ����ʼʱ��
        memcpy(&g_sUserParam.m_sNightStartTime, &BK->m_sNightTime, sizeof(Struct_Time2));

        if(g_sUserParam.m_sManoStart.byYear == 0xff)
        {
            memset(&g_sUserParam.m_sManoStart, 0, sizeof(StructUTC));
            memset(&g_sResComTask_Mag.BackupArea[0].m_sManoStart, 0, sizeof(STRUCT_TIME1));
        }

        if(g_sUserParam.m_sManoEnd.byYear == 0xff)
        {
            memset(&g_sUserParam.m_sManoEnd, 0, sizeof(StructUTC));
            memset(&g_sResComTask_Mag.BackupArea[0].m_sManoEnd, 0, sizeof(STRUCT_TIME1));
        }

        memcpy(&g_sUserParam.EP_Num, g_sResComTask_Mag.BackupArea[0].EP_Num, 3);
				
        g_sResComTask_Mag.UnUpLoadEvtCnt  = 0;
        g_sResComTask_Mag.m_dwCtrlEvtNum  = 0;

        //���δ��Ӧ�¼����
        memset(g_sResComTask_Mag.EvtRspFlg, 0, sizeof(g_sResComTask_Mag.EvtRspFlg));
				memset(g_sResComTask_Mag.EvtRspFlgBak, 0, sizeof(g_sResComTask_Mag.EvtRspFlg));		
				
				g_sUserParam.m_byBackstageCfgWeaponNum = BK->m_byWeaponNum;				
			  if(g_sUserParam.m_byBackstageCfgWeaponNum != 0)
				{
					for(u8 i = 0; i < BK->m_byWeaponNum; i++)
					{
							g_sUserParam.m_saBackstageCfgAmmuInfo[i].m_byWeapon_Type 	= BK->WeaponInfo[i].m_byWeapon_Type;
							g_sUserParam.m_saBackstageCfgAmmuInfo[i].m_byWeapon_SubType 	= BK->WeaponInfo[i].m_byWeapon_SubType;
							g_sUserParam.m_saBackstageCfgAmmuInfo[i].m_byCfgBulletKind 	= BK->WeaponInfo[i].m_byCfgBulletKind;
							g_sUserParam.m_saBackstageCfgAmmuInfo[i].m_byState = BK->WeaponInfo[i].m_byState;
							memcpy(&g_sUserParam.m_saBackstageCfgAmmuInfo[i].m_sBulletInfo[0], &BK->WeaponInfo[i].m_sBulletInfo[0], sizeof(BULLETINFO_T) * g_sUserParam.m_saAmmuInfo[i].m_byCfgBulletKind);
					}
				}
    }
    else 	//��ϰģʽ
    {
        IDParam_SetTSubType(BK->T_Subtype);
				
        g_sUserParam.m_byMode = GAME_GOING;
				Wtn6_Play(WTN6_Mode_Mano);
				g_sUserParam.m_sTsChar1.m_bUnitmode = TRUE;
        UserParam_SetBlood(BK->Blood);
				
				if(BK->Blood == 0)
					g_sUserParam.m_sTsChar1.m_bLocked = TRUE;
				else
					g_sUserParam.m_sTsChar1.m_bLocked = FALSE;
				
				g_sUserParam.m_sTsChar1.m_bFailureState = FALSE;
				g_sUserParam.m_sLPosition.m_byLposState = FALSE;
				
				UserParam_SetBRChar(BK->Belong);
			  UserParam_SetKey(BK->Key);
				UserParam_SetDayOrNight(BK->IsNight);
				UserParam_SetActLaserCnt(0);
        //��ҹģʽ��Ҫ�ָ�
        if(g_sUserParam.m_IsNight == TRUE)
        {
            NightModeHandle(TRUE);
        }
        else
        {
            DayModeHandle(TRUE);
        }
        g_sResComTask_Mag.m_dwRptInterval = BK->RptInterval;

        g_sResComTask_Mag.Vios.DetectorLoss = g_sResComTask_Mag.BackupArea[0].Vios.DetectorLoss;
        g_sResComTask_Mag.Vios.FumeUnInstall = g_sResComTask_Mag.BackupArea[0].Vios.FumeUnInstall;
        g_sResComTask_Mag.Vios.HelmetLoss = g_sResComTask_Mag.BackupArea[0].Vios.HelmetLoss;
        g_sResComTask_Mag.Vios.IllegalMove = g_sResComTask_Mag.BackupArea[0].Vios.IllegalMove;

        g_sUserParam.m_IsIllegalPowerOff = !(BK->IsEnablePowerOff);

        memcpy(g_sLaserComTask_Mag.Shell, &BK->Shell, sizeof(u16)*PARTITION_MAX);
        memcpy(&g_sResComTask_Mag.DrillParam, &g_sResComTask_Mag.BackupArea[0].DrillParam, sizeof(DRILL_PARAM));
        
				//��ϰ��ʼʱ��
				memcpy(&g_sUserParam.m_sManoStart, &g_sResComTask_Mag.BackupArea[0].m_sManoStart, sizeof(STRUCT_TIME1));
				//��ϰ����ʱ��
        memcpy(&g_sUserParam.m_sManoEnd, &g_sResComTask_Mag.BackupArea[0].m_sManoEnd, sizeof(STRUCT_TIME1));

        //�л����������ʼʱ��
        memcpy(&g_sUserParam.m_sDayStartTime, &BK->m_sDayTime, sizeof(STRUCT_TIME2));
        //�л�����ҹ����ʼʱ��
        memcpy(&g_sUserParam.m_sNightStartTime, &BK->m_sNightTime, sizeof(STRUCT_TIME2));

        if(g_sUserParam.m_sManoStart.byYear == 0xff)
        {
            memset(&g_sUserParam.m_sManoStart, 0, sizeof(StructUTC));
            memset(&g_sResComTask_Mag.BackupArea[0].m_sManoStart, 0, sizeof(STRUCT_TIME1));
        }

        if(g_sUserParam.m_sManoEnd.byYear == 0xff)
        {
            memset(&g_sUserParam.m_sManoEnd, 0, sizeof(STRUCT_TIME1));
            memset(&g_sResComTask_Mag.BackupArea[0].m_sManoEnd, 0, sizeof(STRUCT_TIME1));
        }
				
				g_sResComTask_Mag.m_dwCtrlEvtNum = BK->ES_Num;
				
        memcpy(g_sResComTask_Mag.EvtRspFlgBak, BK->EvtRspFlgBak, sizeof(g_sResComTask_Mag.EvtRspFlgBak));
        memset(g_sResComTask_Mag.EvtRspFlg, 0, sizeof(g_sResComTask_Mag.EvtRspFlg));
        
				UnUpLoadEvtNumCalcu();

        memcpy(&g_sUserParam.EP_Num, g_sResComTask_Mag.BackupArea[0].EP_Num, 3);

        if(g_sUserParam.m_byBlood != 100)
        {
						g_sHmiComTask_Mag.m_HurtKind = BK->InjuryKind;
        
            g_sHmiComTask_Mag.m_byHurtCountFlg = BK->InjuryCnt;	//����ͷ�������Ͽ��ָ���ָ�֮ǰ���˲�ͼƬ

            for(u8 i = 0; i < g_sHmiComTask_Mag.m_byHurtCountFlg; i++)
            {
                g_sHmiComTask_Mag.m_sInjury[i].m_byPart = BK->m_byInjuryPart[i];
            }

            //̽ͷ�ƴ��ָ�
            for(u8 i = 0; i < sizeof(BK->Decter); i++)
            {
                if(BK->Decter[i] == TRUE)
                {
                    Laser_CtrlLedOn(i, &g_sLaserComTask_Mag); //������
                }
            }
        }
				
				g_sUserParam.m_byBackstageCfgWeaponNum = BK->m_byWeaponNum;
				if(g_sUserParam.m_byBackstageCfgWeaponNum != 0)
				{
					for(u8 i = 0; i < BK->m_byWeaponNum; i++)
					{
							g_sUserParam.m_saBackstageCfgAmmuInfo[i].m_byWeapon_Type 	= BK->WeaponInfo[i].m_byWeapon_Type;
							g_sUserParam.m_saBackstageCfgAmmuInfo[i].m_byWeapon_SubType 	= BK->WeaponInfo[i].m_byWeapon_SubType;
							g_sUserParam.m_saBackstageCfgAmmuInfo[i].m_byCfgBulletKind 	= BK->WeaponInfo[i].m_byCfgBulletKind;
							g_sUserParam.m_saBackstageCfgAmmuInfo[i].m_byState = BK->WeaponInfo[i].m_byState;
							memcpy(&g_sUserParam.m_saBackstageCfgAmmuInfo[i].m_sBulletInfo[0], &BK->WeaponInfo[i].m_sBulletInfo[0], sizeof(BULLETINFO_T) * g_sUserParam.m_saAmmuInfo[i].m_byCfgBulletKind);
					}
				}
    }

}

//�����ָ�
u8 DrillStateReadFromOutFlash(uint8_t *pBuf, uint32_t Len, uint8_t Area)
{
    uint32_t WriteAddr = 0;
    uint8_t Xor = 0;

    if(Area == 1)
    {
				WriteAddr = g_sIFlashMag.m_dwCapacity * 1024 - g_sIFlashMag.m_wPageSize * 4;		//���ݴ洢�ĵ�ַ
    }
    else if(Area == 2)
    {
        WriteAddr = g_sIFlashMag.m_dwCapacity * 1024 - g_sIFlashMag.m_wPageSize * 3;		//���ݴ洢�ĵ�ַ
    }
		else
			return FALSE;

		IFlash_Read(&g_sIFlashMag, FLASH_BOOT_START + WriteAddr, pBuf, Len);
}

//��ϰ�����ָ�
uint8_t DrillStateRecovery(void)
{
    uint8_t XorA = 0, XorB = 0;

    //��flash A���ָ�����
    DrillStateReadFromOutFlash((u8*)(&g_sResComTask_Mag.BackupArea[0]), sizeof(PARAMATER_BACKUP), 1);
    //��flash B���ָ�����
    DrillStateReadFromOutFlash((u8*)(&g_sResComTask_Mag.BackupArea[1]), sizeof(PARAMATER_BACKUP), 2);

    //����У���
    for(uint32_t i = 0; i < sizeof(PARAMATER_BACKUP) - 1; i++)
    {
        XorA ^= ((u8*)(&g_sResComTask_Mag.BackupArea[0]))[i];
    }

    //����У���
    for(uint32_t i = 0; i < sizeof(PARAMATER_BACKUP) - 1; i++)
    {
        XorB ^= ((u8*)(&g_sResComTask_Mag.BackupArea[1]))[i];

    }
		
    if(g_sResComTask_Mag.BackupArea[0].HeadFlg[0] == 0x5A &&
            g_sResComTask_Mag.BackupArea[0].HeadFlg[1] == 0xA5 &&
            XorA == g_sResComTask_Mag.BackupArea[0].XOR)
    {
        AssignStateFromBuffer(&g_sResComTask_Mag.BackupArea[0]);

    }
    else if(g_sResComTask_Mag.BackupArea[1].HeadFlg[0] == 0x5A &&
            g_sResComTask_Mag.BackupArea[1].HeadFlg[1] == 0xA5 &&
            XorB == g_sResComTask_Mag.BackupArea[1].XOR)
    {
        AssignStateFromBuffer(&g_sResComTask_Mag.BackupArea[1]);

    }
    else
    {
        //û�в�����תΪѵ��ģʽ����
        TrainModeInit();
    }
}

//��������
u8 DrillStateWriteToOutFlash(u8 *pBuf, u32 Len,u8 Area)
{
	uint32_t WriteAddr = 0;
	uint8_t ret = 0;
	uint8_t Xor = 0;

	if(Area == 1)
	{
			WriteAddr = g_sIFlashMag.m_dwCapacity * 1024 - g_sIFlashMag.m_wPageSize * 4;
	}
	else if(Area == 2)
	{
			WriteAddr = g_sIFlashMag.m_dwCapacity * 1024 - g_sIFlashMag.m_wPageSize * 3;
	}

	//����У���
	for(uint32_t i = 0; i < Len - 1;i++)
	{
		Xor ^= pBuf[i];
	}
	pBuf[Len-1]	 = Xor;

	IFlash_Write(&g_sIFlashMag, FLASH_BOOT_START + WriteAddr, pBuf, Len);

	return ret;	
}

void BackupDrillState(void)
{
	//A��Ϣ������B��Ϣ
	memcpy(&g_sResComTask_Mag.BackupArea[1],&g_sResComTask_Mag.BackupArea[0],sizeof(PARAMATER_BACKUP));
	
	//����������ⲿflash
	IWDG_Feed();
	DrillStateWriteToOutFlash((uint8_t*)(&g_sResComTask_Mag.BackupArea[0]),sizeof(PARAMATER_BACKUP),1);
	IWDG_Feed();
	DrillStateWriteToOutFlash((uint8_t*)(&g_sResComTask_Mag.BackupArea[1]),sizeof(PARAMATER_BACKUP),2);
	IWDG_Feed();
}

void FullConditionRecovery()
{
    PARAMATER_BACKUP *BK = NULL;

    //���������ֵͬ��
    if(g_sResComTask_Mag.BackupArea[0].HeadFlg[0] == 0x5A && g_sResComTask_Mag.BackupArea[0].HeadFlg[1] == 0xA5)
    {
        memcpy(&g_sResComTask_Mag.BackupArea[1], &g_sResComTask_Mag.BackupArea[0], sizeof(PARAMATER_BACKUP));
        BK = &g_sResComTask_Mag.BackupArea[0];
    }
    else if(g_sResComTask_Mag.BackupArea[1].HeadFlg[0] == 0x5A && g_sResComTask_Mag.BackupArea[1].HeadFlg[1] == 0xA5)
    {
        memcpy(&g_sResComTask_Mag.BackupArea[0], &g_sResComTask_Mag.BackupArea[1], sizeof(PARAMATER_BACKUP));
        BK = &g_sResComTask_Mag.BackupArea[1];
    }

    if(BK != NULL)
    {

        //�ж���ϰ�����ǲ���0
				/*
        if(BK->EP_Num[0] == 0 && BK->EP_Num[1] == 0 && BK->EP_Num[2] == 0)
        {
            return;
        }
				*/
        g_sUserParam.m_byMode = GAME_GOING;
				g_sUserParam.m_sTsChar1.m_bUnitmode = TRUE;
        UserParam_SetBlood(100);
        UserParam_SetKey(BK->Key);
        IDParam_SetTSubType(BK->T_Subtype);
        g_sUserParam.m_byBackstageCfgWeaponNum = BK->m_byWeaponNum;
        UserParam_SetBRChar(BK->Belong);
        g_sResComTask_Mag.m_dwRptInterval = BK->RptInterval;

        g_sResComTask_Mag.Vios.DetectorLoss 	= FALSE;
        g_sResComTask_Mag.Vios.FumeUnInstall	= FALSE;
        g_sResComTask_Mag.Vios.HelmetLoss 	= FALSE;
        g_sResComTask_Mag.Vios.IllegalMove 	= FALSE;

        //�л����������ʼʱ��
        memcpy(&g_sUserParam.m_sDayStartTime, &BK->m_sDayTime, sizeof(Struct_Time2));
        //�л�����ҹ����ʼʱ��
        memcpy(&g_sUserParam.m_sNightStartTime, &BK->m_sNightTime, sizeof(Struct_Time2));

        memcpy(g_sLaserComTask_Mag.Shell, &BK->Shell, sizeof(u16)*PARTITION_MAX);
        memcpy(&g_sResComTask_Mag.DrillParam, &g_sResComTask_Mag.BackupArea[0].DrillParam, sizeof(DRILL_PARAM));
        memcpy(&g_sUserParam.m_sManoStart, &g_sResComTask_Mag.BackupArea[0].m_sManoStart, sizeof(STRUCT_TIME1));
        memcpy(&g_sUserParam.m_sManoEnd, &g_sResComTask_Mag.BackupArea[0].m_sManoEnd, sizeof(STRUCT_TIME1));

        if(g_sUserParam.m_sManoStart.byYear == 0xff)
        {
            memset(&g_sUserParam.m_sManoStart, 0, sizeof(StructUTC));
            memset(&g_sResComTask_Mag.BackupArea[0].m_sManoStart, 0, sizeof(STRUCT_TIME1));
        }

        if(g_sUserParam.m_sManoEnd.byYear == 0xff)
        {
            memset(&g_sUserParam.m_sManoEnd, 0, sizeof(StructUTC));
            memset(&g_sResComTask_Mag.BackupArea[0].m_sManoEnd, 0, sizeof(STRUCT_TIME1));
        }

        memcpy(&g_sUserParam.EP_Num, g_sResComTask_Mag.BackupArea[0].EP_Num, 3);

        for(u8 i = 0; i < BK->m_byWeaponNum; i++)
        {
            g_sUserParam.m_saBackstageCfgAmmuInfo[i].m_byWeapon_Type = BK->WeaponInfo[i].m_byWeapon_Type;
            g_sUserParam.m_saBackstageCfgAmmuInfo[i].m_byWeapon_SubType = BK->WeaponInfo[i].m_byWeapon_SubType;
            g_sUserParam.m_saBackstageCfgAmmuInfo[i].m_byCfgBulletKind = BK->WeaponInfo[i].m_byCfgBulletKind;
            g_sUserParam.m_saBackstageCfgAmmuInfo[i].m_byState = BK->WeaponInfo[i].m_byState;
						for(u8 b = 0; b < g_sUserParam.m_saAmmuInfo[i].m_byCfgBulletKind; b++)
						{			
								memcpy(&g_sUserParam.m_saBackstageCfgAmmuInfo[i].m_sBulletInfo[b], &BK->WeaponInfo[i].m_sBulletInfo[b], sizeof(BULLETINFO_T));
								memcpy(&g_sUserParam.m_saBackstageCfgAmmuInfo[i].m_sBulletInfo[b].m_aBulletRemainNum, &g_sUserParam.m_saAmmuInfo[i].m_sBulletInfo[b].m_aBulletCfgNum, 3);
								memset(&g_sUserParam.m_saBackstageCfgAmmuInfo[i].m_sBulletInfo[b].m_aBulletShotNum, 0, 3);
						}
        }
    }
}

uint8_t SaveDrillState(uint8_t ModeFlg, uint8_t IsBatteryLow)
{
    SynchronizeState(ModeFlg, IsBatteryLow);
    BackupDrillState();
	
		return TRUE;
}

void InitShell(uint8_t thickness)
{
    for(uint8_t i = 0; i < PARTITION_MAX; i++)
    {
        g_sLaserComTask_Mag.Shell[i] = g_sUserParam.Thickness < 20 ? g_sUserParam.Thickness : (g_sUserParam.Thickness - 18) * 10;
    }
}

//��¼���ν��պ�̨Э���UTCʱ��
void RecordRcvBackStageProtocolTime(StructUTC * Time)
{
    Time->byYear   = g_sUserParam.m_sUtcTime.m_byYear;
    Time->byMonth  = g_sUserParam.m_sUtcTime.m_byMonth;
    Time->byDay    = g_sUserParam.m_sUtcTime.m_byDate;
    Time->byHour   = g_sUserParam.m_sUtcTime.m_byHour;
    Time->byMin    = g_sUserParam.m_sUtcTime.m_byMinute;
    Time->bySecond = g_sUserParam.m_sUtcTime.m_bySecond;
}

//��̨���ղ����������ݴ���
uint8_t BNSoldierManoParamSetHandle(Struct_CtrlManoParam_IndividualSoldierDownload ManoParam)
{
		SINGLE_SLAVER_MAG *pMag = NULL;
	
    uint8_t StartBuf[70] = {0}, i, j, k;
    uint8_t EndBuf[70] = {0};

    uint32_t BackupAreaEpNum = 0, RecvEpNum = 0;

    BackupAreaEpNum = g_sUserParam.EP_Num[0] << 16 	|
                      g_sUserParam.EP_Num[1] << 8		|
                      g_sUserParam.EP_Num[2];

    RecvEpNum		= ManoParam.EP_Num[0] << 16	|
                  ManoParam.EP_Num[1] << 8	|
                  ManoParam.EP_Num[2];

    //��ʼʱ��
    sprintf((char*)StartBuf, "%d%02d%02d%02d%02d",
            ManoParam.m_sStartTime.m_byYear,
            ManoParam.m_sStartTime.m_byMonth,
            ManoParam.m_sStartTime.m_byDate,
            ManoParam.m_sStartTime.m_byHour,
            ManoParam.m_sStartTime.m_byMinute);

    //����ʱ��
    sprintf((char*)EndBuf, "%d%02d%02d%02d%02d",
            ManoParam.m_sEndTime.m_byYear,
            ManoParam.m_sEndTime.m_byMonth,
            ManoParam.m_sEndTime.m_byDate,
            ManoParam.m_sEndTime.m_byHour,
            ManoParam.m_sEndTime.m_byMinute);

    //��ϰ����ʱ��С����ϰ��ʼʱ�䣬������ʵ��
    if(0 <= memcmp(StartBuf, EndBuf, sizeof(StartBuf)))
    {
        return FALSE;
    }
   
		//������Ϣ,ͬ������״̬
		//g_sUserParam.m_bySelfWeaponNum = UserParam_GetWeaponNum();
		//����һ�εĺ�̨�������ñ���
		//memcpy(&g_sUserParam.m_saWeaponSelfAmmuInfo, &g_sUserParam.m_saAmmuInfo, sizeof(SOLDIERAMMU_T) * g_sUserParam.m_bySelfWeaponNum); //������һ�ε���������
		/*
		for(uint8_t w = g_sUserParam.m_bySelfWeaponNum; w < WEAPONKIND_MAX; w++) //����������Ҫ����Ϣ,��ֹ�ϴ�����
		{
				memset(&g_sUserParam.m_saWeaponSelfAmmuInfo[w], 0, sizeof(SOLDIERAMMU_T));
		}
		*/
		//��ϰ��ʼʱ��
		memcpy(&g_sUserParam.m_sManoStart, &ManoParam.m_sStartTime, sizeof(STRUCT_TIME1));

		//��ϰ����ʱ��
		memcpy(&g_sUserParam.m_sManoEnd, &ManoParam.m_sEndTime, sizeof(STRUCT_TIME1));

		//�л����������ʼʱ��
		memcpy(&g_sUserParam.m_sDayStartTime, &ManoParam.m_sDayStartTime, sizeof(STRUCT_TIME2));

		//�л�����ҹ����ʼʱ��
		memcpy(&g_sUserParam.m_sNightStartTime, &ManoParam.m_sNightStartTime, sizeof(STRUCT_TIME2));

		//��Կ
		UserParam_SetKey(ManoParam.m_byKey);
		//��������
		UserParam_SetBRChar(ManoParam.m_byBR_Attribute);

		//ͬ����ϰ������������
		IDParam_SetTSubType(ManoParam.m_byT_Subtype);

		//��ֵ�����ã����3����������ֹ���鳬��
		g_sUserParam.m_byBackstageCfgWeaponNum = ManoParam.m_byAmmuNum;

		if(g_sUserParam.m_byBackstageCfgWeaponNum > WEAPONKIND_MAX)
				g_sUserParam.m_byBackstageCfgWeaponNum = WEAPONKIND_MAX;

		memcpy(&g_sUserParam.m_saBackstageCfgAmmuInfo, &ManoParam.m_sAmmu, sizeof(SOLDIERAMMU_T) * g_sUserParam.m_byBackstageCfgWeaponNum); //���������ø��µ�������Ϣ����

		//�����е�ҩ��remainҲ���
		for(uint8_t w = 0; w < g_sUserParam.m_byBackstageCfgWeaponNum; w++)
		{
				for(uint8_t b = 0; b < g_sUserParam.m_saBackstageCfgAmmuInfo[w].m_byCfgBulletKind; b++)
				{
						memcpy(g_sUserParam.m_saBackstageCfgAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum, g_sUserParam.m_saBackstageCfgAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum, 3);
						memcpy(g_sUserParam.m_saBackstageCfgAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum, g_sUserParam.m_saBackstageCfgAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum, 3);
				}
		}
		
		for(uint8_t w = g_sUserParam.m_byBackstageCfgWeaponNum; w < WEAPONKIND_MAX; w++) //����������Ҫ����Ϣ,��ֹ�ϴ�����
		{
				memset(&g_sUserParam.m_saBackstageCfgAmmuInfo[w], 0, sizeof(SOLDIERAMMU_T));
		}
		
		for(i = 0;i < UserParam_GetSlaverCnt(); i++)
		{
				for(j = 0; j < g_sUserParam.m_byBackstageCfgWeaponNum; j++)
				{
					if(g_sUserParam.m_saSingleMag[i].m_byTType == g_sUserParam.m_saBackstageCfgAmmuInfo[j].m_byWeapon_Type
						&& g_sUserParam.m_saSingleMag[i].m_byTSubType == g_sUserParam.m_saBackstageCfgAmmuInfo[j].m_byWeapon_SubType)
					{
							g_sUserParam.m_saSingleMag[i].m_byIsAmmuInit = TRUE;
					}
					else
					{
							if(g_sUserParam.m_saSingleMag[i].m_byTType == 0x14 
								&& g_sUserParam.m_saBackstageCfgAmmuInfo[j].m_byWeapon_Type == 0x14)
							{
								g_sUserParam.m_saSingleMag[i].m_byTSubType = g_sUserParam.m_saBackstageCfgAmmuInfo[j].m_byWeapon_SubType;
								g_sUserParam.m_saSingleMag[i].m_byIsAmmuInit = TRUE;
								UserParam_SetSlaverMag(&g_sUserParam.m_saSingleMag[i], i);
								IWDG_Feed();
								SysParam_Save();
								IWDG_Feed();
							}
					}
				}
		}

		//���÷������
		memcpy(&g_sUserParam.EP_Num, &ManoParam.EP_Num, 3);
		//ʵ���������
		g_sUserParam.Thickness = ManoParam.Thickness;
		InitShell(g_sUserParam.Thickness);

		g_sResComTask_Mag.m_ManoParamFlg = TRUE;  //�����ù���ϰ�����ı��

		//������ϰ������flash
		SaveDrillState(g_sUserParam.m_byMode, g_sUserParam.m_byEnableChangeBat);

		InterConn_ProtDeal_SelfStatue();
		return TRUE;
}

//�ϴ���������ָ���
void BNIntervalSetHandle(Struct_CtrlIntervalDownload Interval)
{
    uint8_t Xor = 0;
	
    //ʵ������״̬�����ϴ�����
    g_sUserParam.m_byUploadInterval = Interval.m_bySelfStatusDataUploadCycle;//��
    g_sResComTask_Mag.m_dwRptInterval = g_sUserParam.m_byUploadInterval * 1000; //תΪ����
    
		//ʵ���ը����ʾ��״̬�����ϴ�����
    g_sUserParam.m_byDynamometerDisplayStatusDataUploadCycle = Interval.m_byDynamometerDisplayStatusDataUploadCycle;
    //����Ա��װ������������������ϴ�����
    g_sUserParam.m_byEquipmentAndIdentityDataUploadCycle = Interval.m_byEquipmentAndIdentityDataUploadCycle;

    //����״̬
    g_sResComTask_Mag.BackupArea[0].RptInterval = g_sResComTask_Mag.m_dwRptInterval;

    //����У���
    for(uint32_t i = 0; i < sizeof(PARAMATER_BACKUP) - 1; i++)
    {
        Xor ^= ((u8*)(&g_sResComTask_Mag.BackupArea[0]))[i];
    }

    g_sResComTask_Mag.BackupArea[0].XOR = Xor;
}

void RSP_CtrlIntervalEventUploadHandle(Struct_CtrlIntervalDownload *Interval)
{
    //������ֵ
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_byT_Type =	IDParam_GetTType();		//������ʵ������
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_byT_Subtype = IDParam_GetTSubType();		//������ʵ��������
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_byRID = IDParam_GetRid();		//������ʵ�嵥λ����
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_aPID[0] = IDParam_GetPid() >> 8;		//������ʵ���豸���
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_aPID[1] = IDParam_GetPid() & 0xff;	
    memcpy(&g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_sRecCmdTime, &g_sResComTask_Mag.m_sCmdTime.m_sBSNulCtrlInterval, sizeof(STRUCT_TIME1));		//����ָ��ʱ��
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_byST_Type = Interval->m_byMasterT_Type;		//ʵʩ���÷�ʵ������
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_byST_Subtype = Interval->m_byMasterT_Subtype;		//ʵʩ���÷�ʵ��������
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_byS_RID = Interval->m_byMasterRID;		//ʵʩ���÷�ʵ�嵥λ����
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_aS_PID[0] = Interval->m_aMasterPID[0];		//ʵʩ���÷�ʵ���豸���
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_aS_PID[1] = Interval->m_aMasterPID[1];

    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_bySelfStatusDataUploadCycle = Interval->m_bySelfStatusDataUploadCycle;	//ʵ������״̬�����ϴ����� 	��
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_byDynamometerDisplayStatusDataUploadCycle = Interval->m_byDynamometerDisplayStatusDataUploadCycle;		//ʵ���ը����ʾ��״̬�����ϴ�����  ��
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_byEquipmentAndIdentityDataUploadCycle = Interval->m_byEquipmentAndIdentityDataUploadCycle;			//����Ա��װ������������������ϴ�����  ��

    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;

    RSP_CtrlIntervalEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}

void InterConn_ProtDeal_Slaver_AmmuSupplyNum(SOLDIERAMMU_T *WeaponInfo, uint8_t b, uint32_t AmmuNum)
{
		S_CONN_CHAN_PARAM ChanParam;
		SINGLE_SLAVER_MAG *pSlaverMag;
	
		uint8_t *pRetMac;
		uint8_t DatBuf[50];
    uint8_t i = 0, w = 0, DatBufLen;

		for(i = 0; i < SYS_SLAVER_CNT_MAX; i++)
		{
			pSlaverMag = UserParam_GetSlaverMag(i);
			
			if(pSlaverMag->m_byTType == WeaponInfo->m_byWeapon_Type
				&& pSlaverMag->m_byTSubType == WeaponInfo->m_byWeapon_SubType)
				break;
		}
		if(i >= SYS_SLAVER_CNT_MAX)
			return;
		
		if(pSlaverMag->m_byIsSleepFlg == TRUE)
				return;
		
		w = pSlaverMag->m_byWeaponIdx;

		DatBufLen = 0;
		DatBuf[DatBufLen++] = 0xA2;
		DatBuf[DatBufLen++] = 0xAA;
		DatBuf[DatBufLen++] = 0;
		DatBuf[DatBufLen++] = IDParam_GetTType();
		DatBuf[DatBufLen++] = IDParam_GetTSubType();
		DatBuf[DatBufLen++] = IDParam_GetRid();
		DatBuf[DatBufLen++] = IDParam_GetPid() >> 8;
		DatBuf[DatBufLen++] = IDParam_GetPid() & 0xFF;

		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_Type;
		DatBuf[DatBufLen++] = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_SubType;
		DatBuf[DatBufLen++] = 0x01;

		DatBuf[DatBufLen++] = AmmuNum >> 16;
		DatBuf[DatBufLen++] = AmmuNum >> 8;
		DatBuf[DatBufLen++] = AmmuNum;

		DatBuf[2] = DatBufLen - 3;
		DatBuf[DatBufLen] = Xor_Calc(&DatBuf[3], DatBufLen - 3);
		DatBufLen++;
		
		Wtn6_Play(WTN6_ReplenishAmmunition);
		ChanParam.m_eChan = CHAN_ZIGBEE;
		ChanParam.m_pDstAddr = pSlaverMag->m_aWMac;
		ChanParam.m_byDstAddrLen = 8;
		ChanParam.m_pDatBuf = DatBuf;
		ChanParam.m_wDatBufLen = DatBufLen;
		ChanParam.m_dwRepeatCnt = 10; 
		ChanParam.m_dwScheTime = 100; 

		InterChanSend(&ChanParam);
		return;		
}

//��̨��ҩ���ʲ���ָ���
void BNSupplementHandle(Struct_CtrlSupplementDownload *Supplement)
{
		SOLDIERAMMU_T *pAmmuInfo = NULL;
    uint32_t BulletNum = 0, BulletTotalNum = 0, BulletAddNum = 0, BulletRemain = 0;
		
		g_sUserParam.m_byWeaponNum = UserParam_GetWeaponNum();
    //����ҩ��remain�Ͳ�������ٷ���remain
    for(u8 b = 0; b < Supplement->m_byChangeAmmuNum; b++) //��ҩ�仯����
    {
			for(u8 i = 0; i < g_sUserParam.m_byWeaponNum; i++)
			{
				pAmmuInfo = &g_sUserParam.m_saAmmuInfo[i];

				for(u8 k = 0; k < pAmmuInfo->m_byCfgBulletKind; k++)
				{
					if(pAmmuInfo->m_sBulletInfo[k].m_byBullet_Type == Supplement->m_sChangeAmmu[b].m_byA_Type &&
									pAmmuInfo->m_sBulletInfo[k].m_byBullet_SubType == Supplement->m_sChangeAmmu[b].m_byA_Subtype)
					{
							BulletRemain = pAmmuInfo->m_sBulletInfo[k].m_aBulletRemainNum[0] << 16 |
														 pAmmuInfo->m_sBulletInfo[k].m_aBulletRemainNum[1] << 8 |
														 pAmmuInfo->m_sBulletInfo[k].m_aBulletRemainNum[2];
							
							BulletTotalNum = pAmmuInfo->m_sBulletInfo[k].m_aBulletTotalNum[0] << 16 |
														   pAmmuInfo->m_sBulletInfo[k].m_aBulletTotalNum[1] << 8 |
														   pAmmuInfo->m_sBulletInfo[k].m_aBulletTotalNum[2]                    ;

							BulletAddNum = Supplement->m_sChangeAmmu[b].m_aChangeAmmuNum[0] << 16 |
														 Supplement->m_sChangeAmmu[b].m_aChangeAmmuNum[1] << 8 |
														 Supplement->m_sChangeAmmu[b].m_aChangeAmmuNum[2];

							BulletTotalNum = BulletTotalNum + BulletAddNum;
						
							pAmmuInfo->m_sBulletInfo[k].m_aBulletTotalNum[0]	=	BulletTotalNum >> 16;
							pAmmuInfo->m_sBulletInfo[k].m_aBulletTotalNum[1]	=	BulletTotalNum >> 8;
							pAmmuInfo->m_sBulletInfo[k].m_aBulletTotalNum[2]	=	BulletTotalNum & 0xff;

							BulletNum = BulletRemain + BulletAddNum;
							pAmmuInfo->m_sBulletInfo[k].m_aBulletRemainNum[0] = BulletNum >> 16;
							pAmmuInfo->m_sBulletInfo[k].m_aBulletRemainNum[1] = BulletNum >> 8;
							pAmmuInfo->m_sBulletInfo[k].m_aBulletRemainNum[2] = BulletNum & 0xff;

							//���͸��������豸
							InterConn_ProtDeal_Slaver_AmmuSupplyNum(pAmmuInfo, k, BulletNum);
					}
				}
			}
    }
		InterConn_ProtDeal_SelfStatue();
    //Wtn6_Play(WTN6_ReplenishAmmunition);
}

/****************************************************************
@FUNCTION���º�̨��ҩ�����ʣ��ַ�/�����¼��������
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��LW
@SPECIAL:
****************************************************************/
void RSP_CtrlSupplementEventData_Spell(StructEvent_RSP_CtrlSupplement *pMag,u8 *pBuf,u8 *Len)
{
	u8 num = 0,cut = 0;
	 
  //���
	pBuf[num++] = BNEventType_RSP_CtrlSupplement;
	pBuf[num++] = pMag->m_byT_Type;
	pBuf[num++] = pMag->m_byT_Subtype;
	pBuf[num++] = pMag->m_byRID;
	pBuf[num++] = pMag->m_aPID[0];
	pBuf[num++] = pMag->m_aPID[1];
	pBuf[num++] = pMag->m_byC_Idegree;

  memcpy(&pBuf[num],&pMag->m_sL_Position,sizeof(STRUCT_LPOS));
	num += sizeof(STRUCT_LPOS);
	memcpy(&pBuf[num],&pMag->m_sRecCmdTime,sizeof(STRUCT_TIME1));
	num += sizeof(STRUCT_TIME1);
	
	pBuf[num++] = pMag->m_byST_Type;
	pBuf[num++] = pMag->m_byST_Subtype;
	pBuf[num++] = pMag->m_byS_RID;
	pBuf[num++] = pMag->m_aS_PID[0];
	pBuf[num++] = pMag->m_aS_PID[1];
	pBuf[num++] = pMag->m_byIO_Idegree;
	pBuf[num++] = pMag->m_byChangeAmmuNum;
  pBuf[num++] = pMag->m_byChangeSuppleNum;
	
	for(cut=0; cut<pMag->m_byChangeAmmuNum; cut++)
	{
		memcpy(&pBuf[num],&pMag->m_sChangeAmmu[cut],sizeof(STRUCT_AMMUSUPPLY_t));
		num += sizeof(STRUCT_AMMUSUPPLY_t);
	}
	for(cut=0; cut<pMag->m_byChangeSuppleNum; cut++)
	{
		memcpy(&pBuf[num],&pMag->m_sChangeSupply[cut],sizeof(Struct_Supply));
		num += sizeof(Struct_Supply);
	}

	*Len = num;
	
}

/****************************************************************
@FUNCTION���º�̨��ҩ�����ʣ��ַ�/�����¼��ϴ�
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��LW
@SPECIAL:
****************************************************************/
void RSP_CtrlSupplementEventUpload_Send(Struct_EventUpload *pMag)
{
		RESCOMTASK_MAG *pMag1 = &g_sResComTask_Mag;
		FRAME_BUF *pFrame = NULL;
		u8 cut = 0,Len = 0;
		
		pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

		if(pFrame == NULL)
		{
				return;
		}
		
		memset(BackStage_dataBuf,0,sizeof(BackStage_dataBuf));						
		
		pMag->m_aData_Frame[0] = 0xA8;
		pMag->m_aData_Frame[1] = BNCmd_Terminal_EventUpload;

		pFrame->m_wBufLen = 0;
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[1];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
		
		//���
		RSP_CtrlSupplementEventData_Spell(&pMag->Event_Pack.m_sRSP_CtrlSupplement,BackStage_dataBuf,&Len);
		memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen], BackStage_dataBuf, Len);
		pFrame->m_wBufLen += Len;
		
		//�¼�֡���к�
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[1];
		
		//���¼���
		pMag->m_byXor = 0;	
		for(cut = 3; cut < pFrame->m_wBufLen - 2; cut++)
		{
			pMag->m_byXor ^= pFrame->m_byBuf[cut];
		}
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_byXor;
		
		pFrame->m_byBuf[2] = pFrame->m_wBufLen - 6;
		pFrame->m_dwCnt = 1;
		pFrame->m_dwID = (pFrame->m_byBuf[0] << 24 | pFrame->m_byBuf[1] << 16 |
										  pFrame->m_byBuf[2] << 8 | pFrame->m_byBuf[3]);
		pFrame->m_dwScheTime = 1000;
		pFrame->m_bySendNum = 0;
		pFrame->m_dwTime = GetSysRunMilliSecond();
		SComTask_SendListAppend(pMag1->m_sTaskMag.m_pSendList, (void*)pFrame);
		g_sResComTask_Mag.EvtListResPonseNum[pMag->m_aES_Num[0] << 8 | pMag->m_aES_Num[1]] = TRUE;
		if(g_sUserParam.m_byMode == GAME_GOING)
		{
			g_sResComTask_Mag.EvtRspFlg[g_sResComTask_Mag.m_dwCtrlEvtNum / 8] |= (1 << (g_sResComTask_Mag.m_dwCtrlEvtNum % 8));
		
			if(g_sResComTask_Mag.m_dwCtrlEvtNum < 240)
			{
				EventStorageToFlash(pFrame, sizeof(FRAME_BUF), g_sResComTask_Mag.m_dwCtrlEvtNum);
			}
		}
}

void RSP_CtrlSupplementEventUploadHandle(Struct_CtrlSupplementDownload *Supplement)
{
    //������ֵ
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_byT_Type =	IDParam_GetTType();		//������ʵ������
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_byT_Subtype = IDParam_GetTSubType();		//������ʵ��������
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_byRID = IDParam_GetRid();		//������ʵ�嵥λ����
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_aPID[0] = IDParam_GetPid() >> 8;		//������ʵ���豸���
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_aPID[1] = IDParam_GetPid() & 0xff;
		g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_byC_Idegree = GetInjuryDegree(g_sUserParam.m_byBlood);  	//�ۺ����� ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_sL_Position = g_sUserParam.m_sLPosition; //��γ�� �߶� ��λ״̬
    memcpy(&g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_sRecCmdTime, &g_sResComTask_Mag.m_sCmdTime.m_BSNulCtrlManoeuvre, sizeof(STRUCT_TIME1));  //����ָ��ʱ��
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_byST_Type = Supplement->m_byMasterT_Type;		//����ҩ/���ʱ仯��ʵ������
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_byST_Subtype = Supplement->m_byMasterT_Subtype;		//����ҩ/���ʱ仯��ʵ��������
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_byS_RID = Supplement->m_byMasterRID;		//����ҩ/���ʱ仯��ʵ�嵥λ����
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_aS_PID[0] = Supplement->m_aMasterPID[0];		//����ҩ/���ʱ仯��ʵ���豸���
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_aS_PID[1] = Supplement->m_aMasterPID[1];
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_byIO_Idegree = IOCHAR_IN;		//��ҩ (����)������ʶ��	ENUM_IOCHAR
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_byChangeAmmuNum = Supplement->m_byChangeAmmuNum;		//�����仯��ҩ��������m��m=0 ʱ����ҩ�����������ݣ�
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_byChangeSuppleNum = Supplement->m_byChangeSuppleNum;		//�����仯������������n��n=0 ʱ�����������������ݣ�

    for(u8 w = 0; w < g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_byChangeAmmuNum; w++)
    {
        memcpy(&g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_sChangeAmmu[w], &Supplement->m_sChangeAmmu[w], sizeof(STRUCT_AMMUSUPPLY_t));    //��ҩ��Ϣ
    }

    for(u8 s = 0; s < g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_byChangeSuppleNum; s++)
    {
        memcpy(&g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_sChangeSupply[s], &Supplement->m_sChangeSupply, sizeof(Struct_Supply));  //������Ϣ
    }

    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;

    RSP_CtrlSupplementEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}

/****************************************************************
@FUNCTION���º�̨��ϰģʽת���¼��������
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��MTY
@SPECIAL:
****************************************************************/
void ChangeToManoEventData_Spell(StructEvent_ChangeToMano *pMag, u8 *pBuf, u8 *Len)
{
    u8 num = 0;

    pBuf[num++] = BNEventType_ChangeToMano;
    pBuf[num++] = pMag->m_byT_Type;
    pBuf[num++] = pMag->m_byT_Subtype;
    pBuf[num++] = pMag->m_byRID;
    pBuf[num++] = pMag->m_aPID[0];
    pBuf[num++] = pMag->m_aPID[1];
    memcpy(&pBuf[num], &pMag->m_sL_Position, sizeof(STRUCT_LPOS));
    num += sizeof(STRUCT_LPOS);
    memcpy(&pBuf[num], &pMag->m_sChangeToManoTime, sizeof(STRUCT_TIME1));
    num += sizeof(STRUCT_TIME1);
    pBuf[num++] = pMag->m_byCT_Type;
    pBuf[num++] = pMag->m_byCT_Subtype;
    pBuf[num++] = pMag->m_byC_RID;
    pBuf[num++] = pMag->m_aC_PID[0];
    pBuf[num++] = pMag->m_aC_PID[1];
    memcpy(&pBuf[num++], &pMag->m_sTS_Character1, sizeof(STRUCT_TS_Character1));
    *Len = num;
}

/****************************************************************
@FUNCTION���º�̨��ϰģʽת���¼��ϴ�
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��MTY
@SPECIAL:
****************************************************************/
void ChangeToManoEventUpload_Send(Struct_EventUpload *pMag)
{
		RESCOMTASK_MAG *pMag1 = &g_sResComTask_Mag;
		FRAME_BUF *pFrame = NULL;
		u8 cut = 0,Len = 0;
		
		pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

		if(pFrame == NULL)
		{
				return;
		}
		
		memset(BackStage_dataBuf,0,sizeof(BackStage_dataBuf));						
		
		pMag->m_aData_Frame[0] = 0xA8;
		pMag->m_aData_Frame[1] = BNCmd_Terminal_EventUpload;

		pFrame->m_wBufLen = 0;
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[1];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
		
		//���
		ChangeToManoEventData_Spell(&pMag->Event_Pack.m_sChangeToMano, BackStage_dataBuf, &Len);
    memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen], BackStage_dataBuf, Len);
		pFrame->m_wBufLen += Len;
		
		//�¼�֡���к�
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[1];
		
		pMag->m_byXor = 0;
		
		//���¼���

		for(cut = 3; cut < pFrame->m_wBufLen - 2; cut++)
		{
			pMag->m_byXor ^= pFrame->m_byBuf[cut];
		}
		
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_byXor;

		pFrame->m_byBuf[2] = pFrame->m_wBufLen - 6;
		
		pFrame->m_dwCnt = 1;
		pFrame->m_dwID = (pFrame->m_byBuf[0] << 24 | pFrame->m_byBuf[1] << 16 |
											pFrame->m_byBuf[2] << 8 | pFrame->m_byBuf[3]);
		pFrame->m_dwScheTime = 1000;
		pFrame->m_bySendNum = 0;
		pFrame->m_dwTime = GetSysRunMilliSecond();
		SComTask_SendListAppend(pMag1->m_sTaskMag.m_pSendList, (void*)pFrame);
		g_sResComTask_Mag.EvtListResPonseNum[pMag->m_aES_Num[0] << 8 | pMag->m_aES_Num[1]] = TRUE;
		
		if(g_sUserParam.m_byMode == GAME_GOING)
		{
			g_sResComTask_Mag.EvtRspFlg[g_sResComTask_Mag.m_dwCtrlEvtNum / 8] |= (1 << (g_sResComTask_Mag.m_dwCtrlEvtNum % 8));
		
			if(g_sResComTask_Mag.m_dwCtrlEvtNum < 240)
			{
				EventStorageToFlash(pFrame, sizeof(FRAME_BUF), g_sResComTask_Mag.m_dwCtrlEvtNum);
			}
		}
}

//����
void BSN_Judge_DeathHandle(RCVHURTINFO_T Info)
{
		if(UserParam_GetBlood() == 0)
			return;
    Info.m_HurtBlood = 0;
    UpdateDamage(Info);
}

//����
void BSN_Judge_ReviveHandle(RCVHURTINFO_T Info)
{
		if(UserParam_GetBlood() == 100)
			return;
    Info.m_HurtBlood = 100;
    UpdateDamage(Info);
}

//��λ
void BSN_Judge_RestartHandle(RCVHURTINFO_T Info)
{
    Info.m_HurtBlood = 100;
    UpdateDamage(Info);
    InterConn_ProtDeal_Slaver_Weapon_State(FALSE);
		InterConn_ProtDeal_SelfStatue();
    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 2);
}

//��������
void ShotAroundHandle(u8 Part)
{
    switch(Part)
    {
        case 0:		// ������
						Wtn6_Play(WTN6_YX_ZDHX1);
            //Wtn6_StartLSound(WTN6_YX_ZDHX1);
            break;

        case 1:		// �Ҳ����
						Wtn6_Play(WTN6_YX_ZDHX1);
            //Wtn6_StartRSound(WTN6_YX_ZDHX1);
            break;

        case 2:		//ͷ������
            Wtn6_Play(WTN6_YX_ZDHX1);
            break;
    }

}

void BSN_Judge_InvalidHandle(void)
{
		g_sUserParam.m_sTsChar1.m_bLocked = TRUE;
		UserParam_SetTsChar1(g_sUserParam.m_sTsChar1);
		InterConn_ProtDeal_Slaver_Weapon_State(TRUE);
		InterConn_ProtDeal_SelfStatue();
    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 1);
}

void BSN_Judge_TolastHandle(void)
{
    //��ҩ�ָ�����ֵ
		SINGLE_SLAVER_MAG *pSlaverMag = NULL;
		uint8_t i, w;

		for(i = 0; i < UserParam_GetSlaverCnt(); i++)
		{
				pSlaverMag = UserParam_GetSlaverMag(i);

				if(pSlaverMag->m_byTType == 0xFE) 
					continue;

				if(pSlaverMag->m_byIsOnline == FALSE)
					continue;
	
				if(g_sUserParam.m_byMode == GAME_GOING && pSlaverMag->m_byIsAmmuInit == TRUE)
				{
						//����̨���ù�������ҩ�����
						
						w = pSlaverMag->m_byWeaponIdx;
						g_sUserParam.m_saAmmuInfo[w].UnLockFlag = FALSE;
					
						g_sUserParam.m_saAmmuInfo[w].m_byState = 0x01;
						
						memcpy(g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_aBulletTotalNum, g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_aBulletCfgNum, 3);
						memcpy(g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_aBulletRemainNum, g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_aBulletTotalNum, 3);
						
						Wtn6_Play(WTN6_ReplenishAmmunition);
						InterConn_ProtDeal_Slaver_AmmuSupply(pSlaverMag);
				}
				else
				{
						//��������ҩ����
						w = pSlaverMag->m_byWeaponIdx;
						g_sUserParam.m_saAmmuInfo[w].UnLockFlag = FALSE;
						
						memcpy(g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_aBulletTotalNum, g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_aBulletCfgNum, 3);
						memcpy(g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_aBulletRemainNum, g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[0].m_aBulletCfgNum, 3);
						InterConn_ProtDeal_Slaver_AmmuSupply(pSlaverMag);
				}

				Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 2);
		}
}

void ManoStartHandle(void)
{
    static uint32_t LastTime = 0;
		STRUCT_TS_Character2 Char2;
    RCVHURTINFO_T HurtInfo = {0};

    //��ǰʱ�����ϴε���ʱ�����10s
    if((GetSysRunSecond() > LastTime) && ((GetSysRunSecond() - LastTime) > 4))
    {
        LastTime = GetSysRunSecond();

        //if(g_sUserParam.m_byMode != GAME_GOING)
        {
            g_sUserParam.m_byMode = GAME_GOING;
										
            if(TRUE == g_sResComTask_Mag.DrillParam.DisableDrillStartVoice)
            {
                g_sResComTask_Mag.DrillParam.DisableDrillStartVoiceTime = 50;
            }
						
						Char2.m_bUnitmode = g_sUserParam.m_byMode == GAME_GOING ? 1 : 0;
						Char2.m_bIdentity = UserParam_GetBRChar();
						Char2.m_bKey = UserParam_GetKey();
						UserParam_SetTsChar2(Char2);

						g_sUserParam.m_sTsChar1.m_bUnitmode = g_sUserParam.m_byMode == GAME_GOING ? 1 : 0;
						g_sUserParam.m_sTsChar1.m_bIdentity = UserParam_GetBRChar();
					
						InterConn_ProtDeal_SelfStatue();
						ModeChange_Reset();
            Wtn6_Play(WTN6_Mode_Mano);

            g_sResComTask_Mag.m_dwCtrlEvtNum = 0;
            g_sResComTask_Mag.UnUpLoadEvtCnt = 0;
            //δ�ϴ��¼���Ӧ�������
            memset(g_sResComTask_Mag.EvtRspFlg, 0, sizeof(g_sResComTask_Mag.EvtRspFlg));
					
            //��ϰģʽת���¼�
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_byT_Type    = IDParam_GetTType();
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_byT_Subtype = IDParam_GetTSubType();
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_byRID       = IDParam_GetRid();
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_aPID[0]     = IDParam_GetPid() >>8 ;
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_aPID[1]     = IDParam_GetPid() &0xff;
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_sL_Position = g_sUserParam.m_sLPosition;    //λ�ú͸߳�
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_sChangeToManoTime = g_sUserParam.m_sUtcTime;
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_byCT_Type   	= 0xFF;
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_byCT_Subtype = 0xFF;
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_byC_RID 		  = 0x01;		//��ʱ
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_aC_PID[0] 		= 0xFF;
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_aC_PID[1] 		= 0xFF;

						g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_sTS_Character1.m_bLocked = g_sUserParam.m_sTsChar1.m_bLocked;
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_sTS_Character1.m_bConnectionState = GetAllSlavesConnectStatus();		//����������״̬0������	1���쳣
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_sTS_Character1.m_bFailureState = g_sUserParam.m_sTsChar1.m_bFailureState;		//0���޹���	1���й���
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_sTS_Character1.m_bUnitmode     = g_sUserParam.m_sTsChar1.m_bUnitmode;		//0��ѵ��ģʽ	1����ϰģʽ
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_sTS_Character1.m_bBatteryState = g_sUserParam.m_sTsChar1.m_bBatteryState;		//0����������	1��Ƿ��
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_sTS_Character1.m_bIdentity     = g_sUserParam.m_sTsChar1.m_bIdentity;		//��� ENUM_BRCHAR
						g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_sTS_Character1.m_bIsNight      = g_sUserParam.m_sTsChar1.m_bIsNight;
            
						g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
            g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
            g_sResComTask_Mag.m_dwCtrlEvtNum++;
            ChangeToManoEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);

            HurtInfo.m_RT_Type 		= 0xff;
            HurtInfo.m_RT_Subtype 	= 0xff;
            HurtInfo.m_HType        = H_Type_I_BACKSTAGECTRL;
            HurtInfo.m_HurtBlood    = 100;

            UpdateDamage(HurtInfo);
						
            if(FALSE == g_sHmiComTask_Mag.m_byHelmetIsOnline)
            {
                g_sResComTask_Mag.HelmetLossFlg = TRUE;
            }
						
            InitShell(g_sUserParam.Thickness);
						UserParam_Save();
        }
    }
}

void ManoEndHandle(void)
{
		STRUCT_TS_Character2 Char2;

    RCVHURTINFO_T HurtInfo = {0};

    HurtInfo.m_RT_Type 		= 0xff;
    HurtInfo.m_RT_Subtype = 0xff;
    HurtInfo.m_HType 		  = H_Type_I_BACKSTAGECTRL;
    HurtInfo.m_HurtBlood 	= 100;

		g_sUserParam.m_byMode = TRAIN;
		g_sUserParam.m_sTsChar1.m_bUnitmode = FALSE;
		Wtn6_Play(WTN6_Mode_Train);

		UserParam_SetBRChar(BRCHAR_YELLOW);
		IDParam_SetTSubType(0x01);
	
		Char2.m_bUnitmode = g_sUserParam.m_byMode;
		Char2.m_bIdentity = UserParam_GetBRChar();
		Char2.m_bKey = UserParam_GetKey();
		UserParam_SetTsChar2(Char2);
	
		UpdateDamage(HurtInfo);

		g_sUserParam.m_sTsChar1.m_bUnitmode = g_sUserParam.m_byMode == GAME_GOING ? 1 : 0;
		g_sUserParam.m_sTsChar1.m_bIdentity = UserParam_GetBRChar();
		
		InterConn_ProtDeal_SelfStatue();
		ModeChange_Reset();
		
		InitShell(g_sUserParam.Thickness);
		g_sResComTask_Mag.Vios.FumeUnInstall = FALSE;
		g_sResComTask_Mag.Vios.HelmetLoss = FALSE;
		g_sResComTask_Mag.Vios.DetectorLoss = FALSE;
		g_sResComTask_Mag.Vios.IllegalMove = FALSE;
		//������ϰʱ��ͷ������
		memset(&g_sUserParam.m_sManoStart, 0, sizeof(STRUCT_TIME1));
		memset(&g_sUserParam.EP_Num, 0, 3);
		g_sResComTask_Mag.m_dwCtrlEvtNum = 0;
		g_sResComTask_Mag.UnUpLoadEvtCnt = 0;
		//δ�ϴ��¼���Ӧ�������
		memset(g_sResComTask_Mag.EvtRspFlg, 0, sizeof(g_sResComTask_Mag.EvtRspFlg));
		memset(g_sResComTask_Mag.EvtRspFlgBak, 0, sizeof(g_sResComTask_Mag.EvtRspFlg));
		UserParam_Save();

        Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 1);
}

//����ģʽ����
void DayModeHandle(uint8_t IsEnForce)
{
	if(TRUE == IsEnForce)
	{
		g_sUserParam.m_IsNight = TRUE;
	}
		
	if(TRUE == g_sUserParam.m_IsNight)
	{
		g_sUserParam.m_byDayOrNightJudge = TRUE;
		g_sUserParam.m_IsNight = FALSE;
		g_sUserParam.m_sTsChar1.m_bIsNight = FALSE;
		Wtn6_DaySound();
		Led_SetBrightness(100, &g_sRunledMag);
		Led_SetBrightness(100, &g_sStaledMag);
		InterConn_ProtDeal_SelfStatue();
		//HardCtrl_Slaver_Day();
    Wtn6_Play(WTN6_DAY_MODEL);
	}
}

//ҹ��ģʽ����
void NightModeHandle(uint8_t IsEnForce)
{
	if(TRUE == IsEnForce)
	{
		g_sUserParam.m_IsNight = FALSE;
	}
	
	if(FALSE == g_sUserParam.m_IsNight)
	{
			g_sUserParam.m_byDayOrNightJudge = TRUE;
	    g_sUserParam.m_IsNight = TRUE;
			g_sUserParam.m_sTsChar1.m_bIsNight = TRUE;
	    Wtn6_NightSound();
	    Led_SetBrightness(5, &g_sRunledMag);
	    Led_SetBrightness(10, &g_sStaledMag);
			InterConn_ProtDeal_SelfStatue();
	    //HardCtrl_Slaver_Night();
			Wtn6_Play(WTN6_NIGHT_MODE);
	}	
}

//����
void BSN_Judge_SlightHandle(RCVHURTINFO_T Info)
{
    srand(9);
    Info.m_HurtBlood = 91 + rand() % 9;
    //��ӦѪ����������0��Σ���ˣ�1-40���ض��ˣ�41-73���ж��ˣ�74-90�����ˣ�91-99��������100
    UpdateDamage(Info);
    Wtn6_Play(WTN6_Slight_Injury);
}

//�ж���
void BSN_Judge_ModerateHandle(RCVHURTINFO_T Info)
{
    srand(16);
    Info.m_HurtBlood = 74 + rand() % 16;
    UpdateDamage(Info);
    Wtn6_Play(WTN6_Moderate_Injury);
}

//�ض���
void BSN_Judge_SevereHandle(RCVHURTINFO_T Info)
{
    srand(32);
    Info.m_HurtBlood = 41 + rand() % 32;
    UpdateDamage(Info);
    Wtn6_Play(WTN6_Severe_Injury);
}

//Σ����
void BSN_Judge_CriticalHandle(RCVHURTINFO_T Info)
{
    srand(39);
    Info.m_HurtBlood = 1 + rand() % 39;
    UpdateDamage(Info);
		Wtn6_Play(WTN6_Injury_Worse);
}

//���Ƶȼ�����
void BSN_Judge_WorseHandle(RCVHURTINFO_T Info)
{
    Wtn6_Play(WTN6_Injury_Worse);
    Info.m_HurtBlood = GetWorseLevelBlood(g_sUserParam.m_byBlood);
    UpdateDamage(Info);
}

uint8_t GetWorseLevelBlood(uint8_t Blood)
{
    uint8_t InjuryBlood;
	
    //��ӦѪ����������0��Σ���ˣ�1-40���ض��ˣ�41-73���ж��ˣ�74-90�����ˣ�91-99��������100
    if(100 == Blood)
    {
				srand(9);
        InjuryBlood = 91 + rand() % 9;
        return InjuryBlood;
    }
    else if(91 <= Blood && Blood <= 99)
    {
				srand(16);
        InjuryBlood = 74 + rand() % 16;
        return InjuryBlood;
    }
    else if(74 <= Blood && Blood <= 90)
    {
				srand(32);
        InjuryBlood = 41 + rand() % 32;
        return InjuryBlood;
    }
    else if(41 <= Blood && Blood <= 73)
    {
				srand(39);
        InjuryBlood = 1 + rand() % 39;
        return InjuryBlood;
    }
    else if(1 <= Blood && Blood <= 40)
        return 0;
    else
        return 0;
}

//���Ƶȼ�����
void BSN_Judge_ReliefHandle(RCVHURTINFO_T Info)
{
    Info.m_HurtBlood = GetReliefBlood(g_sUserParam.m_byBlood);
    UpdateDamage(Info);
}

uint8_t GetReliefBlood(uint8_t Blood)
{
    uint8_t InjuryBlood;
    SetRand();

    //��ӦѪ����������0��Σ���ˣ�1-40���ض��ˣ�41-73���ж��ˣ�74-90�����ˣ�91-99��������100
    if(100 == Blood)
        return 100;
    else if(91 <= Blood && Blood <= 99)
        return 100;
    else if(74 <= Blood && Blood <= 90)
    {
        InjuryBlood = 91 + rand() % 9;
        return InjuryBlood;
    }
    else if(41 <= Blood && Blood <= 73)
    {
        InjuryBlood = 74 + rand() % 16;
        return InjuryBlood;
    }
    else if(1 <= Blood && Blood <= 40)
    {
        InjuryBlood = 41 + rand() % 32;
        return InjuryBlood;
    }
    else
    {
        InjuryBlood = 1 + rand() % 39;
        return InjuryBlood;
    }

}

void BSN_Judge_ToRedHandle(void)
{
		UserParam_SetBRChar(BRCHAR_RED);
    //BleSyncInfo();
    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 1);
}

void BSN_Judge_ToBlueHandle(void)
{
    UserParam_SetBRChar(BRCHAR_BLUE);
    //BleSyncInfo();
    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 1);
}

//��̨�þ�����
uint8_t BackStageJudgeHandle(uint8_t JudgeKeyWord, RCVHURTINFO_T Info)
{
		RCVHURTINFO_T HurtInfo;
	
    switch(JudgeKeyWord)
    {
        case ACHAR_TEST:    //����
            break;

        case ACHAR_DEATH:   //��Ա������װ������
            BSN_Judge_DeathHandle(Info);
            break;

        case ACHAR_REVIVE:  //����ָ���������ҩ��������ǰ״̬��
            BSN_Judge_ReviveHandle(Info);
            break;

        case ACHAR_RESTART: //��Ա/װ����λ
            BSN_Judge_RestartHandle(Info);
            break;

        case ACHAR_INVALID: //����װ��ʧЧ
            BSN_Judge_InvalidHandle();
            break;

        case ACHAR_TOLAST:  //�ָ���ҩ �ָ�����һ�����õ�ҩ��
            BSN_Judge_TolastHandle();
            break;

        case ACHAR_MANO:    //��ϰģʽ
            if(g_sUserParam.m_byMode == TRAIN)
            {
                g_sUserParam.m_byJudge = TRUE;
								ManoStartHandle();
								//FullConditionRecovery();
								SaveDrillState(g_sUserParam.m_byMode, g_sUserParam.m_byEnableChangeBat);
            }
            break;

        case ACHAR_TRAIN:   //ѵ��ģʽ
            if(g_sUserParam.m_byMode == GAME_GOING)
            {
                g_sUserParam.m_byJudge = TRUE;
								ManoEndHandle();
								//����״̬
								SaveDrillState(g_sUserParam.m_byMode, FALSE);
            }
            break;

        case ACHAR_DAY:     //����ģʽ
            DayModeHandle(TRUE);
            break;

        case ACHAR_NIGHT:   //ҹ��ģʽ
            NightModeHandle(TRUE);
            break;

        case ACHAR_PERSON_SLIGHT:   //��Ա����
            BSN_Judge_SlightHandle(Info);
            break;

        case ACHAR_PERSON_MODERATE: //��Ա�ж���
            BSN_Judge_ModerateHandle(Info);
            break;

        case ACHAR_PERSON_SEVERE:   //��Ա�ض���
            BSN_Judge_SevereHandle(Info);
            break;

        case ACHAR_PERSON_CRITICAL: //��ԱΣ����
            BSN_Judge_CriticalHandle(Info);
            break;

        case ACHAR_ARMOUR_LOCK_WEAPON:  //�������ⷢ��/��ҵ��������
            BSN_Judge_InvalidHandle();
            break;

        case ACHAR_RESTOE:  //��Ա�Ȼ�/װ��ά�޳ɹ����ָ�ս��
            BSN_Judge_RestartHandle(Info);//��ͬ����Աװ����λ
            break;

        case ACHAR_NEAR:    //����
            //ShotAroundHandle(0x02);
            break;

        case ACHAR_NUCLEAR: //����в
            BSN_Judge_NCBHandle(NCBTTYPE_N, Info);
            break;

        case ACHAR_BIOLOGY: //����в
            BSN_Judge_NCBHandle(NCBTTYPE_C, Info);
            break;

        case ACHAR_CHEMISTRY: //����в
            BSN_Judge_NCBHandle(NCBTTYPE_B, Info);
            break;

        case ACHAR_PHOTOELE:  //�����в
            break;

        case ACHAR_WORSE:   //���Ƶȼ�����
            BSN_Judge_WorseHandle(Info);
            break;

        case ACHAR_RELIEF:  //���Ƶȼ�����
            BSN_Judge_ReliefHandle(Info);
            break;

        case ACHAR_TO_RED:  //תΪ���
            BSN_Judge_ToRedHandle();
            break;

        case ACHAR_TO_BLUE: //תΪ����
            BSN_Judge_ToBlueHandle();
            break;

        case ACHAR_DETAIN:  //��Ѻ
            break;

        case ACHAR_SUSPEND_DRILL:	//��ϰ��ͣ
            break;

        case ACHAR_CONTINUE_DRILL:	//��ϰ��ʼ
            break;

        case ACHAR_RELOAD_OIL:		//��������
            break;

        case ACHAR_LOW_VOICE:		//΢��ģʽ
            Wtn6_NightSound();
            //BleWtn6Send(0xE2);
            break;

        case ACHAR_NORMAL_VOICE:	//��������ģʽ
            Wtn6_DaySound();
            //BleWtn6Send(0xEF);
            break;

        case ACHAR_POWER_OFF:		//�ػ�
            g_sPowerOffMag.m_bPoweroff_Force = TRUE;
            break;

        default:
            break;
    }

    return 0;
}

/****************************************************************
@FUNCTION���º�̨���زþ��¼��������
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��LW
@SPECIAL:
****************************************************************/
void BNJudgeEventData_Spell(StructEvent_BNJudge *pMag,u8 *pBuf,u8 *Len)
{
	u8 num = 0;
	
  //���
	pBuf[num++] = BNEventType_Judge;		//���زþ��¼�
	pBuf[num++] = pMag->m_byT_Type;
	pBuf[num++] = pMag->m_byT_Subtype;
	pBuf[num++] = pMag->m_byRID;
	pBuf[num++] = pMag->m_aPID[0];
	pBuf[num++] = pMag->m_aPID[1];
	pBuf[num++] = pMag->m_byC_Idegree; //�ۺ�����

	memcpy(&pBuf[num],&pMag->m_sL_Position,sizeof(STRUCT_LPOS)); //��γ�ȸ߶ȶ�λ
	num += sizeof(STRUCT_LPOS);
	memcpy(&pBuf[num],&pMag->m_sRecCmdTime,sizeof(STRUCT_TIME1)); //ʱ��
	num += sizeof(STRUCT_TIME1);
	
	pBuf[num++] = pMag->m_byFT_Type;
	pBuf[num++] = pMag->m_byFT_Subtype;
	pBuf[num++] = pMag->m_byF_RID;
	pBuf[num++] = pMag->m_aF_PID[0];
	pBuf[num++] = pMag->m_aF_PID[1];
	pBuf[num++] = pMag->m_byAC_Character;
  
	*Len = num;
}

/****************************************************************
@FUNCTION���º�̨���زþ��¼��ϴ�
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��LW
@SPECIAL:
****************************************************************/
void BNJudgeEventUpload_Send(Struct_EventUpload *pMag)
{
	RESCOMTASK_MAG *pMag1 = &g_sResComTask_Mag;
	FRAME_BUF *pFrame = NULL;
	
	uint8_t cut = 0,Len = 0;
	
	pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

	if(pFrame == NULL)
	{
			return;
	}
	
	memset(BackStage_dataBuf,0,sizeof(BackStage_dataBuf));						

	pMag->m_aData_Frame[0] = 0xA8;
	pMag->m_aData_Frame[1] = BNCmd_Terminal_EventUpload; 

	pFrame->m_wBufLen = 0;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[0];
	pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[1];
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0; //����
	
	BNJudgeEventData_Spell(&pMag->Event_Pack.m_sJudge, BackStage_dataBuf,&Len);
	memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen],BackStage_dataBuf,Len);
	pFrame->m_wBufLen += Len;
	
	pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[0]; 
	pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[1];

	pMag->m_byXor = 0;
	for(cut = 3 ; cut < pFrame->m_wBufLen - 2; cut++)
	{
		pMag->m_byXor ^= pFrame->m_byBuf[cut];
	}
	pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_byXor;
	
	pFrame->m_byBuf[2] = pFrame->m_wBufLen - 6;
	
	pFrame->m_dwCnt = 1;
	pFrame->m_dwID = (pFrame->m_byBuf[0] << 24 | pFrame->m_byBuf[1] << 16 |
										pFrame->m_byBuf[2] << 8 | pFrame->m_byBuf[3]);
	pFrame->m_dwScheTime = 1000;
	pFrame->m_dwTime = GetSysRunMilliSecond();
	pFrame->m_bySendNum = 0;

	SComTask_SendListAppend(pMag1->m_sTaskMag.m_pSendList, (void*)pFrame);
	g_sResComTask_Mag.EvtListResPonseNum[pMag->m_aES_Num[0] << 8 | pMag->m_aES_Num[1]] = TRUE;
	
	if(g_sUserParam.m_byMode == GAME_GOING)
	{
		g_sResComTask_Mag.EvtRspFlg[g_sResComTask_Mag.m_dwCtrlEvtNum / 8] |= (1 << (g_sResComTask_Mag.m_dwCtrlEvtNum % 8));
	
		if(g_sResComTask_Mag.m_dwCtrlEvtNum < 240)
		{
			EventStorageToFlash(pFrame, sizeof(FRAME_BUF), g_sResComTask_Mag.m_dwCtrlEvtNum);
		}
	}
}

void BNJudgeEventUploadHandle(Struct_JudgeDownload *Judge)
{
    //������ֵ
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sJudge.m_byT_Type = IDParam_GetTType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sJudge.m_byT_Subtype = IDParam_GetTSubType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sJudge.m_byRID = IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sJudge.m_aPID[0] = IDParam_GetPid() >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sJudge.m_aPID[1] = IDParam_GetPid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sJudge.m_byC_Idegree = GetInjuryDegree(g_sUserParam.m_byBlood);
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sJudge.m_sL_Position = g_sUserParam.m_sLPosition;
    memcpy(&g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sJudge.m_sRecCmdTime, &Judge->m_judgeTime, sizeof(STRUCT_TIME1));
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sJudge.m_byFT_Type = Judge->m_byMasterT_Type;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sJudge.m_byFT_Subtype = Judge->m_byMasterT_Subtype;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sJudge.m_byF_RID = Judge->m_byMasterRID;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sJudge.m_aF_PID[0] = Judge->m_aMasterPID[0];
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sJudge.m_aF_PID[1] = Judge->m_aMasterPID[1];
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sJudge.m_byAC_Character = Judge->m_byAC_Character;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;

    BNJudgeEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}

/****************************************************************
@FUNCTION������A_Type��A_Subtype��ȡ������ҩͳһ����
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��MTY
@SPECIAL:
****************************************************************/
//��ȡ��ҩ���ͺ������͵�ͳһ����
void GetBulletUniID(u8 A_Type, u8 A_Subtype, u8* pUid)
{
    u8 i;

    for(i = 0; i < WEAPONUNIID_MAX; i++)
    {
        if((AmmoTypeID[i].A_TypeID == A_Type) && (AmmoTypeID[i].Sub_A_TypeID == A_Subtype))
        {
            if(pUid != NULL)
            {
                *pUid = i;
            }

            break;
        }
    }

    if(i == WEAPONUNIID_MAX)
    {
        *pUid = 0x00;
    }
}

//��ȡ�������ͺ����������͵�ͳһ����
void GetWeaponUniID(u8 T_Type, u8 T_Subtype, u8* pUid)
{
    u8 i;

    for(i = 0; i < WEAPONUNIID_MAX; i++)
    {
        if((WeaponTypeUniID[i].T_Type == T_Type) && (WeaponTypeUniID[i].T_Subtype == T_Subtype))
        {
            if(pUid != NULL)
            {
                *pUid = i;
            }

            break;
        }
    }
}


//����ֱ�������͵�ҩ��ź����˲�λ��ü���Ѫ��
uint8_t GetDirectWeaponDamage(uint16_t UniID, uint8_t Part)
{
    //�ж��Ƿ�Ϊֱ������
    if((Part > PARTITION_NONE) && (Part < PARTITION_MAX))
    {
        return DamageTableByBullet[UniID][Part - 1];
    }

    return 0;
}

ENUM_PARTITION ReverseMapHurtPart(uint8_t Part)
{
    ENUM_PARTITION RePart = PARTITION_NONE;

    switch(Part)
    {
        case 1:		//ͷ
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
            RePart = PARTITION_HEAD;
            break;

        case 17:
        case 18:	//�ز�
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
            RePart = PARTITION_LCHEST;
            break;

        case 28:
        case 29:
        case 30:
        case 31:
        case 32:	//����
        case 33:
        case 34:
        case 35:
        case 36:
        case 37:
        case 38:
        case 39:
        case 40:
        case 41:
        case 42:
        case 59:
        case 60:
            RePart = PARTITION_ABDOMEN;
            break;

        case 44:	//��֫
        case 45:
        case 46:
        case 47:
        case 48:
        case 49:
        case 50:
        case 51:
        case 52:
        case 53:
            RePart = PARTITION_LARM;
            break;

        case 43:
        case 54:
        case 55:
        case 56:	//����
        case 57:
        case 58:
            RePart = PARTITION_BACK;
            break;

        case 61:	//�Ȳ�
        case 62:
        case 63:
        case 64:
        case 65:
        case 66:
        case 67:
        case 68:
        case 69:
        case 70:
        case 71:
        case 72:
            RePart = PARTITION_LLEG;
            break;

        default:	//����
            RePart = PARTITION_ABDOMEN;
            break;
    }

    return RePart;
}

void DeathRecord(void)
{
	g_sUserParam.m_sLastDeathMag.m_sTimer.year 	= g_sUserParam.m_sUtcTime.m_byYear - 2000;
	g_sUserParam.m_sLastDeathMag.m_sTimer.month = g_sUserParam.m_sUtcTime.m_byMonth;
	g_sUserParam.m_sLastDeathMag.m_sTimer.date 	= g_sUserParam.m_sUtcTime.m_byDate;
	g_sUserParam.m_sLastDeathMag.m_sTimer.hour 	= g_sUserParam.m_sUtcTime.m_byHour;
	g_sUserParam.m_sLastDeathMag.m_sTimer.min   = g_sUserParam.m_sUtcTime.m_byMinute;
	g_sUserParam.m_sLastDeathMag.m_sTimer.sec	  =	g_sUserParam.m_sUtcTime.m_bySecond;

	g_sUserParam.m_sLastDeathMag.m_byDamageKind = g_sInjuryMag.m_byDamageKind;
	g_sUserParam.m_sLastDeathMag.m_byPart 			= g_sInjuryMag.m_byPart;
	g_sUserParam.m_sLastDeathMag.m_byFRid				= g_sInjuryMag.m_byFRid;
	g_sUserParam.m_sLastDeathMag.m_wFPid 				= g_sInjuryMag.m_wFPid;
	g_sUserParam.m_sLastDeathMag.m_byUniID 			= g_sInjuryMag.m_byDamageKind;

	g_sUserParam.m_sLastDeathMag.m_fJD = 	g_sResComTask_Mag.m_sNmeaMag.m_byJD_Du + (g_sResComTask_Mag.m_sNmeaMag.m_byJD_Fen + g_sResComTask_Mag.m_sNmeaMag.m_dwJD_Fen_XiaoShu / 100000.0) / 60.0; //��������徭��
											 /*
											 g_sUserParam.m_sLPosition.m_byJD_DuHundred * 100 + 
											 g_sUserParam.m_sLPosition.m_byJD_DuTen * 10 + 
											 g_sUserParam.m_sLPosition.m_byJD_DuUnit + 
											 (g_sUserParam.m_sLPosition.m_byJD_FenTen * 10 + 
											 g_sUserParam.m_sLPosition.m_byJD_FenUnit + 
											 (g_sUserParam.m_sLPosition.m_byJD_FenTenth / 10000 + 
											 g_sUserParam.m_sLPosition.m_byJD_FenPercent / 1000+ 
											 g_sUserParam.m_sLPosition.m_byJD_FenThousandth / 100 + 
											 g_sUserParam.m_sLPosition.m_byJD_FenTenThousandth / 10) / 100000.0	 
											 ) / 60.0
											 ;
											 */
	//���������γ��
	g_sUserParam.m_sLastDeathMag.m_fWD = g_sResComTask_Mag.m_sNmeaMag.m_byWD_Du + (g_sResComTask_Mag.m_sNmeaMag.m_byWD_Fen + g_sResComTask_Mag.m_sNmeaMag.m_dwWD_Fen_XiaoShu / 100000.0) / 60.0; //���������γ��
											/*
											 g_sUserParam.m_sLPosition.m_byWD_DuTen * 10 + 
											 g_sUserParam.m_sLPosition.m_byWD_DuUnit + 
											 (g_sUserParam.m_sLPosition.m_byWD_FenTen * 10 + 
											 g_sUserParam.m_sLPosition.m_byWD_FenUnit + 
											 (g_sUserParam.m_sLPosition.m_byWD_FenTenth / 10000 + 
											 g_sUserParam.m_sLPosition.m_byWD_FenPercent / 1000+ 
											 g_sUserParam.m_sLPosition.m_byWD_FenThousandth / 100 + 
											 g_sUserParam.m_sLPosition.m_byWD_FenTenThousandth / 10) / 100000.0	 
											 ) / 60.0
											 ;
											 */
}

//�����:1���� 2���� 3ͬ��״̬
void ReviveHandle(void)
{
		LASERCOMTASK_MAG *pMag = LaserComTask_HandleGet();
		uint8_t i;

		Wtn6_Play(WTN6_Revive);
		g_sUserParam.m_byBlood = 100;
		g_sUserParam.m_sTsChar1.m_bLocked = FALSE;
		UserParam_SetTsChar1(g_sUserParam.m_sTsChar1);
		g_sHmiComTask_Mag.m_byHurtCountFlg = 0;
		g_sHmiComTask_Mag.m_byHurtfinishFlg = FALSE;
		for(i = 0; i < HMI_INJURYLEN; i++)
			g_sHmiComTask_Mag.m_sInjury[i].m_byPart = PARTITION_NONE;
		pMag->m_sRcvHurtInfo.m_HPart = PARTITION_NONE;
		g_sUserParam.m_eStatus = Sta_Normal;
		
		g_sResComTask_Mag.NBC_Strength = 0;
		g_sResComTask_Mag.m_byNuclearFlg = 0;
		g_sResComTask_Mag.m_byChemistryFlg = 0;				
		g_sResComTask_Mag.m_byBiologyFlg = 0;
		
		Detector_Led_AllOff();
		Detector_Led_AllOff();
		
		CtrlHelmetLaserOff();
		InterConn_ProtDeal_Slaver_Weapon_State(FALSE);
}

//��������:1���� 2���� 3ͬ��״̬ 4 �����¼��ϴ�
void DeathHandle(void)
{
		g_sUserParam.m_byBlood = 0;
		g_sUserParam.m_sTsChar1.m_bLocked = TRUE;
		UserParam_SetTsChar1(g_sUserParam.m_sTsChar1);
		play_Status(g_sUserParam.m_eStatus);

		//��Ա�ڳ��ϲ��ܵ������
		if(FALSE == g_sUserParam.m_sBroadcastInfo.m_byIsGetOn && g_sResComTask_Mag.DrillParam.DisableInstallFume == FALSE)
		{
				CtrlHelmetFume(1, 10);
		}
		
		CtrlHelmetLaserOn();
		
		if(g_sUserParam.m_byEnableChangeBat == FALSE)
			Detector_Led_AllOn();
		
		Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 2);
		InterConn_ProtDeal_Slaver_Weapon_State(TRUE);

		DeathRecord();
}

/*
 ************************************************************************************************************************************************************************     
 *��������: EventNoRspRestore
 *��������: ��ȡ�ڴ���δ�ϴ��¼����ύ
 *��������: ��
 *�������: ��
 *��������: ��
 *��������: XFW
 *ȫ������: ��
 *����˵��: 
 ************************************************************************************************************************************************************************       
 */
void EventNoRspRestore(void)
{
	RESCOMTASK_MAG *pMag = ResComTask_HandleGet();
	FRAME_BUF *pFrame = NULL;
	uint8_t Xor = 0, Temp[W25X_PAGE_SIZE] = {0}, cs;
	uint8_t DatBuf[W25X_PAGE_SIZE], BufLen = 0;
	uint16_t Num_NoRsp = 0;
	uint32_t TempLen = 0;
	static uint8_t Num = 0, Times = 0;
	
	if(g_sResComTask_Mag.m_IsAccess == FALSE)
		return;
	
	Times++;
	if(Num > 240 && Times < 2)
		return;
	
	if((g_sResComTask_Mag.EvtRspFlgBak[Num / 8] & (1 << (Num % 8))) == (1 << (Num % 8)))
			ExtFlash_Read(&g_sExtFlashMag, g_sExtFlashMag.m_dwBaseAddr + W25X_PAGE_SIZE * Num, Temp ,sizeof(FRAME_BUF) + 2);
	else
	{
			Times = 0;
			Num++;	
			return;
	}
	
	if(Temp[0] == 1) 
	{
		TempLen = 12 + (Temp[10] << 8 | Temp[11]);

		for(cs = 0; cs < TempLen; cs++)
		{
			Xor ^= Temp[cs+2];
		}

		if(Temp[1] == Xor)
		{		
			BufLen = (Temp[10] << 8 | Temp[11]);
			memcpy(&DatBuf, &Temp[12],  BufLen);
			Uart_SendBuf(&g_sUartSetMag.Com[COM2], DatBuf, BufLen);

			ExtFlash_PageErase(&g_sExtFlashMag, g_sExtFlashMag.m_dwBaseAddr + W25X_PAGE_SIZE * Num);
			g_sResComTask_Mag.EvtRspFlgBak[Num / 8] &= ~(1 << (Num % 8));
		}
	}
	Num++;	
	Times = 0;
}

//�¼��洢
void EventStorageToFlash(FRAME_BUF *pFrame, uint32_t Len, uint8_t Num)
{
	static uint32_t WriteAddr = 0;
	uint16_t TempLen = 0;
  uint8_t Temp[W25X_PAGE_SIZE] = {0}, Xor = 0, cs;
    
	assert_param(pFrame != NULL);
        
	if(Len <= (W25X_PAGE_SIZE - TempLen - 2))
	{
		Temp[0] = 1;
		Temp[1] = 0xff;
		TempLen += 2;
		
		//д������
		Temp[TempLen++] = (pFrame->m_dwID >> 24);
		Temp[TempLen++] = (pFrame->m_dwID >> 16);
		Temp[TempLen++] = (pFrame->m_dwID >> 8);
		Temp[TempLen++] = (pFrame->m_dwID);
		Temp[TempLen++] = (pFrame->m_dwScheTime >> 24);
		Temp[TempLen++] = (pFrame->m_dwScheTime >> 16);
		Temp[TempLen++] = (pFrame->m_dwScheTime >> 8);
		Temp[TempLen++] = (pFrame->m_dwScheTime);
		Temp[TempLen++] = (pFrame->m_wBufLen >> 8);
		Temp[TempLen++] = (pFrame->m_wBufLen);
		
		memcpy(&Temp[TempLen],pFrame->m_byBuf,pFrame->m_wBufLen);	
		TempLen += pFrame->m_wBufLen;
		
		//д��У��
		for(cs = 0; cs < sizeof(FRAME_BUF); cs++)
		{
				Xor ^= Temp[cs + 2];
		}
		
		Temp[1] = Xor;
       
		WriteAddr = W25X_PAGE_SIZE * Num; 
		
		ExtFlash_Write(&g_sExtFlashMag, g_sExtFlashMag.m_dwBaseAddr + WriteAddr, Temp, TempLen);	
	}
}

/*
 ************************************************************************************************************************************************************************
 *��������: TerminalInjury_Save
 *��������: ������˼�¼
 *��������: ���˼�¼����ṹ�壬���������˲����Ƶ�
 *�������: ������ɹ������˼�¼��������1
 *��������: TRUE-�ɹ� FALSE-ʧ��
 *��������: XFW 20200429
 *ȫ������:
 *����˵��: �Զ���䵱ǰUTCʱ��
 ************************************************************************************************************************************************************************
*/

#define	RECORD_INJURY_MAX		20

uint32_t TerminalInjury_Save(INJURY_MAG *a_psMag)
{
    uint16_t TempLen;
    uint8_t *p = (uint8_t *)a_psMag;
    uint32_t WriteAddr;

    //���UTCʱ��
    memcpy((char *)&a_psMag->m_sUtc, &g_sUserParam.m_sUtcTime, sizeof(STRUCT_TIME1));

		a_psMag->m_byChkSum = 0;
    for(TempLen = 0; TempLen < (sizeof(INJURY_MAG) - 1); TempLen++)
    {
        a_psMag->m_byChkSum += *p++;
    }

    //�ύ���洢�豸
		WriteAddr = FLASH_BOOT_START + g_sIFlashMag.m_dwCapacity * 1024 - 2 * g_sIFlashMag.m_wPageSize + W25X_PAGE_SIZE * (a_psMag->m_byRcdCnt % RECORD_INJURY_MAX);
		
		if(WriteAddr < (FLASH_BOOT_START + g_sIFlashMag.m_dwCapacity * 1024))
		{
				IFlash_Write(&g_sIFlashMag, WriteAddr, (uint8_t *)a_psMag, TempLen);
				a_psMag->m_byRcdCnt += 1;
				return TRUE;
		}
    return FALSE;
}

/****************************************************************
@FUNCTION�����˹���
@INPUT�������RID/PID,�������࣬�����λ�����Ѫ�������������ҩ
@OUTPUT����
@RETURN: ��
@AUTHOR��xfw
@SPECIAL: ��־��ȡ��֧�ָ������(a_Blood=-100),��֧�ָֻ�����Ѫ������
****************************************************************/
void UpdateDamage(RCVHURTINFO_T Info)
{
    g_sInjuryMag.m_byDamageKind = (uint8_t)Info.m_HType;
    g_sInjuryMag.m_byPart       = (uint8_t)Info.m_HPart;
    g_sInjuryMag.m_cBlood       = Info.m_HurtBlood;
    g_sInjuryMag.m_byFRid       = Info.m_RRID;
    g_sInjuryMag.m_wFPid        = Info.m_wRPID;
    g_sInjuryMag.m_byUniID      = Info.m_byUniID;
    g_sInjuryMag.m_Ttype        = Info.m_RT_Type;
    g_sInjuryMag.m_Tsubtype     = Info.m_RT_Subtype;

    Beep_On(&g_sBeepMag, OPT_MODE_NONBLOCK, 400);

    //����ǹ���ߺ�̨����
    if(Info.m_HType == H_Type_I_BACKSTAGECTRL || Info.m_HType == H_Type_I_TLQCTRL)
    {
        if(Info.m_HurtBlood == 100) 	//����
        {
            g_sUserParam.m_eStatus = (Info.m_HType == H_Type_I_BACKSTAGECTRL ? Sta_BackstageRevive :
                                     (Info.m_HType == H_Type_I_TLQCTRL ? Sta_JudgeRevive : Sta_Normal));
            ReviveHandle();
        }
        else if(Info.m_HurtBlood == 0) 	//����
        {
            if(g_sUserParam.m_byBlood == 0)
                return;

            g_sUserParam.m_eStatus = (Info.m_HType == H_Type_I_BACKSTAGECTRL ? Sta_BackstageDie :
                                     (Info.m_HType == H_Type_I_TLQCTRL ? Sta_JudgeDie : Sta_Normal));
            DeathHandle();

            if(g_sUserParam.m_byBlood == 0)
            {
                g_sHmiComTask_Mag.m_byHurtKind = HMI_HURTKIND_DT;
            }
						g_sInjuryMag.m_byResidueBlood = g_sUserParam.m_byBlood;
						TerminalInjury_Save(&g_sInjuryMag); //���˼�¼����
        }
        else 	//����
        {
            g_sUserParam.m_eStatus = Sta_Normal;
            Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 2);
            g_sUserParam.m_byBlood = Info.m_HurtBlood;
            Laser_CtrlLedToggle(0, &g_sLaserComTask_Mag);
            g_sUserParam.m_IsAid = FALSE;
            g_sHmiComTask_Mag.m_byHurtKind = HMI_HURTKIND_DT;
						g_sInjuryMag.m_byResidueBlood = g_sUserParam.m_byBlood;
						TerminalInjury_Save(&g_sInjuryMag); //���˼�¼����
        }
    }
    else if(IsAlive(g_sUserParam.m_eStatus)) 	//������
    {
        switch(Info.m_HType)
        {
            case H_Type_I_NuclearChemicalBiological:          //�˻���
                //����
                g_sUserParam.m_byBlood -= MIN(g_sUserParam.m_byBlood, Info.m_HurtBlood);

                //���˺�����
                if(g_sUserParam.m_byBlood == 0)
                {
                    g_sUserParam.m_eStatus = (Info.m_HType == KIND_BACKSTAGE_CTRL ? Sta_BackstageDie :
                                             (Info.m_HType == KIND_TLQ_CTRL ? Sta_JudgeDie : Sta_Die));
                    DeathHandle();
                }
                else if(g_sUserParam.m_byBlood < 100) //���˺�����
                {
                    Wtn6_Play(WTN6_Injury);
                    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 2);
                    InterConn_ProtDeal_SelfStatue();
                }

                Laser_CtrlLedToggle(0, &g_sLaserComTask_Mag);

                if(Info.m_HType == (ENUM_H_Type_INDIVIDUAL)NCBTTYPE_N)
                {
                    g_sHmiComTask_Mag.m_byHurtKind = HMI_HURTKIND_HFS;
                }
                else if(Info.m_HType == (ENUM_H_Type_INDIVIDUAL)NCBTTYPE_B)
                {
                    g_sHmiComTask_Mag.m_byHurtKind = HMI_HURTKIND_SWZS;
                }
                else if(Info.m_HType == (ENUM_H_Type_INDIVIDUAL)NCBTTYPE_C)
                {
                    g_sHmiComTask_Mag.m_byHurtKind = HMI_HURTKIND_HXZS;
                }

                break;														 //��������

            case H_Type_I_ShockInjury:	   //����
                //����
                g_sUserParam.m_byBlood -= MIN(g_sUserParam.m_byBlood, Info.m_HurtBlood);

                //���˺�����
                if(g_sUserParam.m_byBlood == 0)
                {
                    g_sUserParam.m_eStatus = Sta_Die;
                    DeathHandle();
                }
                else if(g_sUserParam.m_byBlood < 100) //���˺�����
                {
                    Wtn6_Play(WTN6_Injury);
                    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 2);
                    InterConn_ProtDeal_SelfStatue();
                }

                Laser_CtrlLedToggle(0, &g_sLaserComTask_Mag);
                g_sHmiComTask_Mag.m_byHurtKind = HMI_HURTKIND_ZDS;
                break;

            case H_Type_I_BulletWounded:		//ǹ����
                //����
                g_sUserParam.m_byBlood -= MIN(g_sUserParam.m_byBlood, Info.m_HurtBlood);

                if(g_sUserParam.m_byBlood == 0)
                {
                    g_sUserParam.m_eStatus = Sta_Die;
                    DeathHandle();
                }
                else if(g_sUserParam.m_byBlood < 100)
                {
                    Wtn6_Play(WTN6_Injury);
                    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 2);
                    InterConn_ProtDeal_SelfStatue();
                }

                g_sHmiComTask_Mag.m_byHurtKind = HMI_HURTKIND_QDS;
                break;

            case H_Type_I_Wounded:												  //ը��
                //����
                g_sUserParam.m_byBlood -= MIN(g_sUserParam.m_byBlood, Info.m_HurtBlood);
                Laser_CtrlLedToggle(0, &g_sLaserComTask_Mag);

                if(g_sUserParam.m_byBlood == 0)
                {
                    g_sUserParam.m_eStatus = Sta_Die;
                    DeathHandle();
                }
                else if(g_sUserParam.m_byBlood < 100)
                {
                    Wtn6_Play(WTN6_Injury);
                    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 2);
                    InterConn_ProtDeal_SelfStatue();
                }

                g_sHmiComTask_Mag.m_byHurtKind = HMI_HURTKIND_ZS;
                break;

            case H_Type_I_Burn:													  //����
                //����
                g_sUserParam.m_byBlood -= MIN(g_sUserParam.m_byBlood, Info.m_HurtBlood);
                Laser_CtrlLedToggle(0, &g_sLaserComTask_Mag);

                if(g_sUserParam.m_byBlood == 0)
                {
                    g_sUserParam.m_eStatus = Sta_Die;
                    DeathHandle();
                }
                else if(g_sUserParam.m_byBlood < 100)
                {
                    Wtn6_Play(WTN6_Injury);
                    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 2);
                    InterConn_ProtDeal_SelfStatue();
                }

                g_sHmiComTask_Mag.m_byHurtKind = HMI_HURTKIND_SS;
                break;

            case H_Type_I_WORSE:
            case H_Type_I_MultipleTrauma:
                //����
                g_sUserParam.m_byBlood -= MIN(g_sUserParam.m_byBlood, Info.m_HurtBlood);

                if(g_sUserParam.m_byBlood == 0)
                {
                    g_sUserParam.m_eStatus = Sta_Die;
                    DeathHandle();
                }
                else if(g_sUserParam.m_byBlood < 100)
                {
                    Wtn6_Play(WTN6_Injury);
                    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 2);
                    InterConn_ProtDeal_SelfStatue();
                }

                g_sHmiComTask_Mag.m_byHurtKind = HMI_HURTKIND_DFS;
                break;

            case H_Type_I_Dislocated:				//�Ѿ�
                //����
                g_sUserParam.m_byBlood -= MIN(g_sUserParam.m_byBlood, Info.m_HurtBlood);

                if(g_sUserParam.m_byBlood == 0)
                {
                    g_sUserParam.m_eStatus = Sta_Die;
                    DeathHandle();
                }
                else if(g_sUserParam.m_byBlood < 100)
                {
                    Wtn6_Play(WTN6_Injury);
                    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 2);
                    InterConn_ProtDeal_SelfStatue();
                }

                g_sHmiComTask_Mag.m_byHurtKind = HMI_HURTKIND_TJ;
                break;

            case H_Type_I_LimbSevered:			    //֫�����
                //����
                g_sUserParam.m_byBlood -= MIN(g_sUserParam.m_byBlood, Info.m_HurtBlood);

                if(g_sUserParam.m_byBlood == 0)
                {
                    g_sUserParam.m_eStatus = Sta_Die;
                    DeathHandle();
                }
                else if(g_sUserParam.m_byBlood < 100)
                {
                    Wtn6_Play(WTN6_Injury);
                    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 2);
                    InterConn_ProtDeal_SelfStatue();
                }

                g_sHmiComTask_Mag.m_byHurtKind = HMI_HURTKIND_ZTLD;
                break;

            case H_Type_I_Fracture:					// ����
                //����
                g_sUserParam.m_byBlood -= MIN(g_sUserParam.m_byBlood, Info.m_HurtBlood);

                if(g_sUserParam.m_byBlood == 0)
                {
                    g_sUserParam.m_eStatus = Sta_Die;
                    DeathHandle();
                }
                else if(g_sUserParam.m_byBlood < 100)
                {
                    Wtn6_Play(WTN6_Injury);
                    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 2);
                    InterConn_ProtDeal_SelfStatue();
                }

                g_sHmiComTask_Mag.m_byHurtKind = HMI_HURTKIND_GZ;
                break;

            case H_Type_I_CollisionInjury:	        //ײ����
                //����
                g_sUserParam.m_byBlood -= MIN(g_sUserParam.m_byBlood, Info.m_HurtBlood);

                if(g_sUserParam.m_byBlood == 0)
                {
										g_sUserParam.m_eStatus = Sta_Die;
                    DeathHandle();
                }
                else if(g_sUserParam.m_byBlood < 100)
                {
                    Wtn6_Play(WTN6_Injury);
                    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 2);
                    InterConn_ProtDeal_SelfStatue();
                }

                g_sHmiComTask_Mag.m_byHurtKind = HMI_HURTKIND_ZJS;
                break;

            case H_Type_I_Sprain_Strain:		//Ť��/����
                //����
                g_sUserParam.m_byBlood -= MIN(g_sUserParam.m_byBlood, Info.m_HurtBlood);

                if(g_sUserParam.m_byBlood == 0)
                {
                    g_sUserParam.m_eStatus = Sta_Die;
                    DeathHandle();
                }
                else if(g_sUserParam.m_byBlood < 100)
                {
                    Wtn6_Play(WTN6_Injury);
                    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 2);
                    InterConn_ProtDeal_SelfStatue();
                }

                g_sHmiComTask_Mag.m_byHurtKind = HMI_HURTKIND_NS;
                break;

            case H_Type_I_Dislocation_Dislocation:	//��λ/��λ
                //����
                g_sUserParam.m_byBlood -= MIN(g_sUserParam.m_byBlood, Info.m_HurtBlood);

                if(g_sUserParam.m_byBlood == 0)
                {
                    g_sUserParam.m_eStatus = Sta_Die;
                    DeathHandle();
                }
                else if(g_sUserParam.m_byBlood < 100)
                {
                    Wtn6_Play(WTN6_Injury);
                    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 2);
                    InterConn_ProtDeal_SelfStatue();
                }

                g_sHmiComTask_Mag.m_byHurtKind = HMI_HURTKIND_TJ;
                break;

            default:
                return;

        }
				
        if(Info.m_HurtBlood != 0)
        {
            g_sUserParam.m_IsAid = FALSE;
        }
        //�������
        g_sInjuryMag.m_byResidueBlood = g_sUserParam.m_byBlood;
        TerminalInjury_Save(&g_sInjuryMag);
    }
    else
    {

    }
		
    //���˱���
		if(g_sUserParam.m_byMode == GAME_GOING)
		{
				UserParam_Save();
		}

    //��ϰģʽ�²����������������������
    if((0 < g_sUserParam.m_byBlood) &&
            (g_sUserParam.m_byBlood <= (100 - GetInjuryBloodByHurtDegree(g_sResComTask_Mag.DrillParam.CfgFSHHurtDegree))))
    {
        //���������
        InterConn_ProtDeal_Slaver_Weapon_State(TRUE);		//����״̬ͬ��
    }
}

/****************************************************************
@FUNCTION���º�̨���������¼��������
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��LW
@SPECIAL:
****************************************************************/
void DamageReportEventData_Spell(StructEvent_DamageReport *pMag,u8 *pBuf,u8 *Len)
{
	u8 num = 0;
	
	pBuf[num++] = BNEventType_DamageReport;
	pBuf[num++] = pMag->m_byT_Type;
	pBuf[num++] = pMag->m_byT_Subtype;
	pBuf[num++] = pMag->m_byRID;
	pBuf[num++] = pMag->m_aPID[0];
	pBuf[num++] = pMag->m_aPID[1];
	pBuf[num++] = pMag->m_byC_Idegree;

  memcpy(&pBuf[num],&pMag->m_sL_Position,sizeof(STRUCT_LPOS));
	num += sizeof(STRUCT_LPOS);
	memcpy(&pBuf[num],&pMag->m_sAttackTime,sizeof(STRUCT_TIME1));
	num += sizeof(STRUCT_TIME1);
	
	pBuf[num++] = pMag->m_byFT_Type;
	pBuf[num++] = pMag->m_byFT_Subtype;
	pBuf[num++] = pMag->m_byF_RID;
	pBuf[num++] = pMag->m_aF_PID[0];
	pBuf[num++] = pMag->m_aF_PID[1];
	pBuf[num++] = pMag->m_byFA_Type;
	pBuf[num++] = pMag->m_byFA_Subtype;
  pBuf[num++] = pMag->m_byH_Part;
	pBuf[num++] = pMag->m_byH_Type;
	pBuf[num++] = pMag->m_byH_Idegree;
    
  memcpy(&pBuf[num],&pMag->m_sFL_Position,sizeof(STRUCT_LPOS));
	num += sizeof(STRUCT_LPOS);

	pBuf[num++] = g_sUserParam.m_sBroadcastInfo.m_byVehicleTType;
	pBuf[num++] = g_sUserParam.m_sBroadcastInfo.m_byVehicleTSubtype;
	pBuf[num++] = g_sUserParam.m_sBroadcastInfo.m_byVehicleRID;
	pBuf[num++] = g_sUserParam.m_sBroadcastInfo.m_wVehiclePID >> 8;
	pBuf[num++] = g_sUserParam.m_sBroadcastInfo.m_wVehiclePID;	
	
	*Len = num;
	
}

/****************************************************************
@FUNCTION���º�̨���������¼��ϴ�
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��LW
@SPECIAL:
****************************************************************/
void DamageReportEventUpload_Send(Struct_EventUpload *pMag)													                                                                 
{
	RESCOMTASK_MAG *pMag1 = &g_sResComTask_Mag;
	FRAME_BUF *pFrame = NULL;
	
	uint8_t cut = 0,Len = 0;

	pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

	if(pFrame == NULL)
	{
			return;
	}		
	
	memset(BackStage_dataBuf,0,sizeof(BackStage_dataBuf));						
	
	pMag->m_aData_Frame[0] = 0xA8;
	pMag->m_aData_Frame[1] = BNCmd_Terminal_EventUpload;

	pFrame->m_wBufLen = 0;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[0];
	pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[1];
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
	
	DamageReportEventData_Spell(&pMag->Event_Pack.m_sDamageReport,BackStage_dataBuf,&Len);
	memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen],BackStage_dataBuf,Len);
	pFrame->m_wBufLen += Len;
	
	pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[0];
	pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[1];
		
  //���¼���
	pMag->m_byXor = 0;
	
	for(cut = 3 ;cut < pFrame->m_wBufLen - 2; cut++)
	{
		pMag->m_byXor ^= pFrame->m_byBuf[cut];
	}
	pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_byXor;

	pFrame->m_byBuf[2] = pFrame->m_wBufLen - 6;
	
	pFrame->m_dwCnt = 1;
	pFrame->m_dwID = (pFrame->m_byBuf[0] << 24 | pFrame->m_byBuf[1] << 16 |
										pFrame->m_byBuf[2] << 8 | pFrame->m_byBuf[3]);
	pFrame->m_dwScheTime = 1000;
	pFrame->m_bySendNum = 0;
	pFrame->m_dwTime = GetSysRunMilliSecond();
	SComTask_SendListAppend(pMag1->m_sTaskMag.m_pSendList, (void*)pFrame);
	g_sResComTask_Mag.EvtListResPonseNum[pMag->m_aES_Num[0] << 8 | pMag->m_aES_Num[1]] = TRUE;

	if(g_sUserParam.m_byMode == GAME_GOING)
	{
		g_sResComTask_Mag.EvtRspFlg[g_sResComTask_Mag.m_dwCtrlEvtNum / 8] |= (1 << (g_sResComTask_Mag.m_dwCtrlEvtNum % 8));
	
		if(g_sResComTask_Mag.m_dwCtrlEvtNum < 240)
		{
			EventStorageToFlash(pFrame, sizeof(FRAME_BUF), g_sResComTask_Mag.m_dwCtrlEvtNum);
		}
	}
}

void DamageReportEventUploadHandle(RCVHURTINFO_T *HurtInfo)
{
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDamageReport.m_byT_Type    = IDParam_GetTType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDamageReport.m_byT_Subtype = IDParam_GetTSubType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDamageReport.m_byRID 			= IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDamageReport.m_aPID[0] 		= IDParam_GetPid() >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDamageReport.m_aPID[1] 		= IDParam_GetPid() & 0xff;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDamageReport.m_byC_Idegree 	= GetInjuryDegree(g_sUserParam.m_byBlood);
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDamageReport.m_sL_Position 	= g_sUserParam.m_sLPosition;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDamageReport.m_sAttackTime 	= g_sUserParam.m_sUtcTime;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDamageReport.m_byFT_Type 		= HurtInfo->m_RT_Type;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDamageReport.m_byFT_Subtype 	= HurtInfo->m_RT_Subtype;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDamageReport.m_byF_RID 		= HurtInfo->m_RRID;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDamageReport.m_aF_PID[0] 		= HurtInfo->m_wRPID >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDamageReport.m_aF_PID[1] 		= HurtInfo->m_wRPID & 0xff ;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDamageReport.m_byFA_Type 		= HurtInfo->m_A_Type;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDamageReport.m_byFA_Subtype = HurtInfo->m_A_Subtype;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDamageReport.m_byH_Part 		= HurtInfo->m_HPart;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDamageReport.m_byH_Type 		= HurtInfo->m_HType;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDamageReport.m_byH_Idegree 	= GetInjuryDegree(100 - HurtInfo->m_HurtBlood);
    memcpy(&g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDamageReport.m_sFL_Position, &HurtInfo->m_sFPosition, sizeof(STRUCT_LPOS));
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;

    DamageReportEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}

void NCBTEvent_Handle(Struct_NCBTDownload *NCBT)
{
    //NCBTTYPE_N==�� NCBTTYPE_C==�� NCBTTYPE_B==��
    if(NCBT->m_byNCBT_Type == NCBTTYPE_N) //��
    {
        g_sResComTask_Mag.m_byNuclearFlg = (NCBT->m_byELR_Character = 1 ? 1 : 0); //1=���룬2=�뿪
    }
    else if(NCBT->m_byNCBT_Type == NCBTTYPE_C)  //��
    {
        g_sResComTask_Mag.m_byBiologyFlg = (NCBT->m_byELR_Character = 1 ? 1 : 0); //1=���룬2=�뿪
    }
    else if(NCBT->m_byNCBT_Type == NCBTTYPE_B)  //��
    {
        g_sResComTask_Mag.m_byChemistryFlg = (NCBT->m_byELR_Character = 1 ? 1 : 0); //1=���룬2=�뿪
    }

		g_sResComTask_Mag.NBC_Strength = NCBT->m_byStrength;
}

/****************************************************************
@FUNCTION���º�̨����/�뿪�˻���մȾ���¼��������
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��LW
@SPECIAL:
****************************************************************/
void NCBTEventData_Spell(StructEvent_NCBT *pMag, uint8_t *pBuf, uint8_t *Len)
{
    uint8_t num = 0;

    pBuf[num++] = BNEventType_NCBT;
    pBuf[num++] = pMag->m_byT_Type;
    pBuf[num++] = pMag->m_byT_Subtype;
    pBuf[num++] = pMag->m_byRID;
    pBuf[num++] = pMag->m_aPID[0];
    pBuf[num++] = pMag->m_aPID[1];
    pBuf[num++] = pMag->m_byC_Idegree;

    memcpy(&pBuf[num], &pMag->m_sL_Position, sizeof(STRUCT_LPOS));
    num += sizeof(STRUCT_LPOS);
    memcpy(&pBuf[num], &pMag->m_sNCBTTime, sizeof(STRUCT_TIME1));
    num += sizeof(STRUCT_TIME1);

    pBuf[num++] = pMag->m_byFT_Type;
    pBuf[num++] = pMag->m_byFT_Subtype;
    pBuf[num++] = pMag->m_byF_RID;
    pBuf[num++] = pMag->m_aF_PID[0];
    pBuf[num++] = pMag->m_aF_PID[1];
    pBuf[num++] = pMag->m_byNCBT_Type;
    pBuf[num++] = pMag->m_byStrength;
    pBuf[num++] = pMag->m_byELR_Character;

    *Len = num;

}

/****************************************************************
@FUNCTION���º�̨����/�뿪�˻���մȾ���¼��ϴ�
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��LW
@SPECIAL:
****************************************************************/
void NCBTEventUpload_Send(Struct_EventUpload *pMag)
{
		RESCOMTASK_MAG *pMag1 = &g_sResComTask_Mag;
		FRAME_BUF *pFrame = NULL;
		
		uint8_t cut = 0,Len = 0;

		pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

		if(pFrame == NULL)
		{
				return;
		}		
		
		memset(BackStage_dataBuf,0,sizeof(BackStage_dataBuf));						
		
		pMag->m_aData_Frame[0] = 0xA8;
		pMag->m_aData_Frame[1] = BNCmd_Terminal_EventUpload;
		
		pFrame->m_wBufLen = 0;
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[1];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
		
		NCBTEventData_Spell(&pMag->Event_Pack.m_sNCBT, BackStage_dataBuf, &Len);
		memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen],BackStage_dataBuf,Len);
		pFrame->m_wBufLen += Len;
		
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[1];
					
		//���¼���
		pMag->m_byXor = 0;
		
		for(cut = 3 ;cut < pFrame->m_wBufLen - 2; cut++)
		{
			pMag->m_byXor ^= pFrame->m_byBuf[cut];
		}
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_byXor;

		pFrame->m_byBuf[2] = pFrame->m_wBufLen - 3;
		
		pFrame->m_dwCnt = 1;
		pFrame->m_dwID = (pFrame->m_byBuf[0] << 24 | pFrame->m_byBuf[1] << 16 |
										  pFrame->m_byBuf[2] << 8 | pFrame->m_byBuf[3]);
		pFrame->m_dwScheTime = 1000;
		pFrame->m_bySendNum = 0;
		pFrame->m_dwTime = GetSysRunMilliSecond();
		SComTask_SendListAppend(pMag1->m_sTaskMag.m_pSendList, (void*)pFrame);
		g_sResComTask_Mag.EvtListResPonseNum[pMag->m_aES_Num[0] << 8 | pMag->m_aES_Num[1]] = TRUE;

		if(g_sUserParam.m_byMode == GAME_GOING)
		{
			g_sResComTask_Mag.EvtRspFlg[g_sResComTask_Mag.m_dwCtrlEvtNum / 8] |= (1 << (g_sResComTask_Mag.m_dwCtrlEvtNum % 8));
		
			if(g_sResComTask_Mag.m_dwCtrlEvtNum < 240)
			{
				EventStorageToFlash(pFrame, sizeof(FRAME_BUF), g_sResComTask_Mag.m_dwCtrlEvtNum);
			}
		}
}

void NCBTEventUpload_Handle(Struct_NCBTDownload *NCBT)
{
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCBT.m_byT_Type    = IDParam_GetTType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCBT.m_byT_Subtype = IDParam_GetTSubType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCBT.m_byRID       = IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCBT.m_aPID[0]     = IDParam_GetPid() >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCBT.m_aPID[1]     = IDParam_GetPid() & 0xff;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCBT.m_byC_Idegree = GetInjuryDegree(g_sUserParam.m_byBlood);
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCBT.m_sL_Position = g_sUserParam.m_sLPosition;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCBT.m_sNCBTTime   = g_sUserParam.m_sUtcTime;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCBT.m_byFT_Type   = NCBT->m_byFT_Type;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCBT.m_byFT_Subtype = NCBT->m_byFT_Subtype;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCBT.m_byF_RID      = NCBT->m_byF_RID;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCBT.m_aF_PID[0]    = NCBT->m_aF_PID[0];
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCBT.m_aF_PID[1]    = NCBT->m_aF_PID[1];
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCBT.m_byNCBT_Type  = NCBT->m_byNCBT_Type;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCBT.m_byStrength   = NCBT->m_byStrength;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCBT.m_byELR_Character = NCBT->m_byELR_Character;

    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;

    NCBTEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}

void RSP_CtrVoiceEventUploadHandle(Struct_HelicopterVoiceCtr *HVC)
{

}

void BNHelicopterVoiceCtrlHandle(Struct_HelicopterVoiceCtr HVC)
{
    //TRUE = ���룬FALSE = �뿪
    if(1 == HVC.ELR_Character)
    {
        if(HVC.m_byFT_Type == 0x60/*ֱ����*/)
        {
            g_sResComTask_Mag.SoundSimulation.HelicopterVoiceFlg = TRUE;
        }
        else if(HVC.m_byFT_Type == 0x61/*���˻�*/ || HVC.m_byFT_Type == 0x62/*ս����*/)
        {
            g_sResComTask_Mag.SoundSimulation.AeroplaneVoiceFlg = TRUE;
        }
        else  /*����*/
        {
            g_sResComTask_Mag.SoundSimulation.IndirectGunVoiceFlg = TRUE;
        }
    }
    else if(2 == HVC.ELR_Character)
    {
        if(HVC.m_byFT_Type == 0x60/*ֱ����*/)
        {
            g_sResComTask_Mag.SoundSimulation.HelicopterVoiceFlg = FALSE;
        }
        else if(HVC.m_byFT_Type == 0x61/*���˻�*/ || HVC.m_byFT_Type == 0x62/*ս����*/)
        {
            g_sResComTask_Mag.SoundSimulation.AeroplaneVoiceFlg = FALSE;
        }
        else  /*����*/
        {
            g_sResComTask_Mag.SoundSimulation.IndirectGunVoiceFlg = FALSE;
        }
    }
}

/****************************************************************
@FUNCTION���º�̨Υ�濪ʼ�¼��������
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��MTY
@SPECIAL:
****************************************************************/
void AgaistRuleStartEventData_Spell(StructEvent_AgaistRuleStart *pMag, u8 *pBuf, u8 *Len)
{
    u8 num = 0;

    pBuf[num++] = BNEventType_AgaistRuleStart;
    pBuf[num++] = pMag->m_byT_Type;
    pBuf[num++] = pMag->m_byT_Subtype;
    pBuf[num++] = pMag->m_byRID;
    pBuf[num++] = pMag->m_aPID[0];
    pBuf[num++] = pMag->m_aPID[1];
    pBuf[num++] = pMag->m_byC_Idegree;
    memcpy(&pBuf[num], &pMag->m_sL_Position, sizeof(STRUCT_LPOS));
    num += sizeof(STRUCT_LPOS);
    memcpy(&pBuf[num], &pMag->m_sTime, sizeof(STRUCT_TIME1));
    num += sizeof(STRUCT_TIME1);
    pBuf[num++] = pMag->m_byIE_Type;

    *Len = num;
}

/****************************************************************
@FUNCTION���º�̨Υ������¼��������
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��MTY
@SPECIAL:
****************************************************************/
void AgaistRuleStopEventData_Spell(StructEvent_AgaistRuleStop *pMag, u8 *pBuf, u8 *Len)
{
    u8 num = 0;

    pBuf[num++] = BNEventType_AgaistRuleStop;
    pBuf[num++] = pMag->m_byT_Type;
    pBuf[num++] = pMag->m_byT_Subtype;
    pBuf[num++] = pMag->m_byRID;
    pBuf[num++] = pMag->m_aPID[0];
    pBuf[num++] = pMag->m_aPID[1];
    pBuf[num++] = pMag->m_byC_Idegree;
    memcpy(&pBuf[num], &pMag->m_sL_Position, sizeof(STRUCT_LPOS));
    num += sizeof(STRUCT_LPOS);
    memcpy(&pBuf[num], &pMag->m_sStartTime, sizeof(STRUCT_TIME1));
    num += sizeof(STRUCT_TIME1);
    memcpy(&pBuf[num], &pMag->m_sStopTime, sizeof(STRUCT_TIME1));
    num += sizeof(STRUCT_TIME1);
    pBuf[num++] = pMag->m_byIE_Type;

    *Len = num;
}
/****************************************************************
@FUNCTION���º�̨Υ�濪ʼ�¼��ϴ�
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��MTY
@SPECIAL:
****************************************************************/
void AgaistRuleStartEventUpload_Send(Struct_EventUpload *pMag)
{
		RESCOMTASK_MAG *pMag1 = &g_sResComTask_Mag;
		FRAME_BUF *pFrame = NULL;
		
		uint8_t cut = 0,Len = 0;

		pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

		if(pFrame == NULL)
		{
				return;
		}		
		
		memset(BackStage_dataBuf,0,sizeof(BackStage_dataBuf));						
		
		pMag->m_aData_Frame[0] = 0xA8;
		pMag->m_aData_Frame[1] = BNCmd_Terminal_EventUpload;

		pFrame->m_wBufLen = 0;
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[1];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
		
		AgaistRuleStartEventData_Spell(&pMag->Event_Pack.m_sAgaistRuleStart, BackStage_dataBuf, &Len);
		memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen],BackStage_dataBuf,Len);
		pFrame->m_wBufLen += Len;
		
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[1];
			
		//���¼���
		pMag->m_byXor = 0;
		for(cut = 3 ;cut < pFrame->m_wBufLen - 2; cut++)
		{
			pMag->m_byXor ^= pFrame->m_byBuf[cut];
		}
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_byXor;
		
		pFrame->m_byBuf[2] = pFrame->m_wBufLen - 6;
		
		pFrame->m_dwCnt = 1;
		pFrame->m_dwID = (pFrame->m_byBuf[0] << 24 | pFrame->m_byBuf[1] << 16 |
											pFrame->m_byBuf[2] << 8 | pFrame->m_byBuf[3]);
		pFrame->m_dwScheTime = 1000;
		pFrame->m_bySendNum = 0;
		pFrame->m_dwTime = GetSysRunMilliSecond();
		SComTask_SendListAppend(pMag1->m_sTaskMag.m_pSendList, (void*)pFrame);
		g_sResComTask_Mag.EvtListResPonseNum[pMag->m_aES_Num[0] << 8 | pMag->m_aES_Num[1]] = TRUE;

		if(g_sUserParam.m_byMode == GAME_GOING)
		{
			g_sResComTask_Mag.EvtRspFlg[g_sResComTask_Mag.m_dwCtrlEvtNum / 8] |= (1 << (g_sResComTask_Mag.m_dwCtrlEvtNum % 8));
		
			if(g_sResComTask_Mag.m_dwCtrlEvtNum < 240)
			{
				EventStorageToFlash(pFrame, sizeof(FRAME_BUF), g_sResComTask_Mag.m_dwCtrlEvtNum);
			}
		}
}

/****************************************************************
@FUNCTION���º�̨Υ������¼��ϴ�
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��MTY
@SPECIAL:
****************************************************************/
void AgaistRuleStopEventUpload_Send(Struct_EventUpload *pMag)
{
		RESCOMTASK_MAG *pMag1 = &g_sResComTask_Mag;
		FRAME_BUF *pFrame = NULL;
		
		uint8_t cut = 0,Len = 0;

		pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

		if(pFrame == NULL)
		{
				return;
		}		
		
		memset(BackStage_dataBuf,0,sizeof(BackStage_dataBuf));						
		
		pMag->m_aData_Frame[0] = 0xA8;
		pMag->m_aData_Frame[1] = BNCmd_Terminal_EventUpload;

		pFrame->m_wBufLen = 0;
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[1];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
		
		AgaistRuleStopEventData_Spell(&pMag->Event_Pack.m_sAgaistRuleStop, BackStage_dataBuf, &Len);
		memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen],BackStage_dataBuf,Len);
		pFrame->m_wBufLen += Len;
		
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[1];

		//���¼���

		pMag->m_byXor = 0;
		for(cut = 3 ;cut < pFrame->m_wBufLen - 2; cut++)
		{
			pMag->m_byXor ^= pFrame->m_byBuf[cut];
		}
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_byXor;
		
		pFrame->m_byBuf[2] = pFrame->m_wBufLen - 6;
		
		pFrame->m_dwCnt = 1;
		pFrame->m_dwID = (pFrame->m_byBuf[0] << 24 | pFrame->m_byBuf[1] << 16 |
											pFrame->m_byBuf[2] << 8 | pFrame->m_byBuf[3]);
		pFrame->m_dwScheTime = 1000;
		pFrame->m_dwTime = GetSysRunMilliSecond();
		pFrame->m_bySendNum = 0;
		SComTask_SendListAppend(pMag1->m_sTaskMag.m_pSendList, (void*)pFrame);
		g_sResComTask_Mag.EvtListResPonseNum[pMag->m_aES_Num[0] << 8 | pMag->m_aES_Num[1]] = TRUE;

		if(g_sUserParam.m_byMode == GAME_GOING)
		{
			g_sResComTask_Mag.EvtRspFlg[g_sResComTask_Mag.m_dwCtrlEvtNum / 8] |= (1 << (g_sResComTask_Mag.m_dwCtrlEvtNum % 8));
		
			if(g_sResComTask_Mag.m_dwCtrlEvtNum < 240)
			{
				EventStorageToFlash(pFrame, sizeof(FRAME_BUF), g_sResComTask_Mag.m_dwCtrlEvtNum);
			}
		}
}

void BSN_RescureEvtData_Spell(StructEvent_IndRescued *pMag, u8 *pBuf, u8 *Len)
{
    u8 num = 0;

    //���
    pBuf[num++] = BNEventType_IndRescued;
    pBuf[num++] = pMag->m_byT_Type;
    pBuf[num++] = pMag->m_byT_Subtype;
    pBuf[num++] = pMag->m_byRID;
    pBuf[num++] = pMag->m_aPID[0];
    pBuf[num++] = pMag->m_aPID[1];
    pBuf[num++] = pMag->m_byC_Idegree;
    memcpy(&pBuf[num], &pMag->m_sL_Position, sizeof(STRUCT_LPOS));
    num += sizeof(STRUCT_LPOS);
    memcpy(&pBuf[num], &pMag->m_sRescueStartTime, sizeof(STRUCT_TIME1));
    num += sizeof(STRUCT_TIME1);

    memcpy(&pBuf[num], &pMag->m_sRescueEndTime, sizeof(STRUCT_TIME1));
    num += sizeof(STRUCT_TIME1);

    pBuf[num++] = pMag->m_byRT_Type;
    pBuf[num++] = pMag->m_byRT_Subtype;
    pBuf[num++] = pMag->m_byR_RID;
    pBuf[num++] = pMag->m_aR_PID[0];
    pBuf[num++] = pMag->m_aR_PID[1];
    pBuf[num++] = pMag->m_byCM_Characte;
    pBuf[num++] = pMag->m_byCE_Characte;

    *Len = num;
}

void BSN_RescureEvtUpload_Send(Struct_EventUpload *pMag)
{
		RESCOMTASK_MAG *pMag1 = &g_sResComTask_Mag;
		FRAME_BUF *pFrame = NULL;
		
		uint8_t cut = 0,Len = 0;

		pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

		if(pFrame == NULL)
		{
				return;
		}		
		
		memset(BackStage_dataBuf,0,sizeof(BackStage_dataBuf));						
		
		pMag->m_aData_Frame[0] = 0xA8;
		pMag->m_aData_Frame[1] = BNCmd_Terminal_EventUpload;

		pFrame->m_wBufLen = 0;
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[1];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
		
		BSN_RescureEvtData_Spell(&pMag->Event_Pack.m_sIndRescued, BackStage_dataBuf, &Len);
		memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen],BackStage_dataBuf,Len);
		pFrame->m_wBufLen += Len;
		
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[1];

		//���¼���

		pMag->m_byXor = 0;
		for(cut = 3 ;cut < pFrame->m_wBufLen - 2; cut++)
		{
			pMag->m_byXor ^= pFrame->m_byBuf[cut];
		}
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_byXor;
		
		pFrame->m_byBuf[2] = pFrame->m_wBufLen - 6;
		
		pFrame->m_dwCnt = 1;
		pFrame->m_dwID = (pFrame->m_byBuf[0] << 24 | pFrame->m_byBuf[1] << 16 |
											pFrame->m_byBuf[2] << 8 | pFrame->m_byBuf[3]);
		pFrame->m_dwScheTime = 1000;
		pFrame->m_dwTime = GetSysRunMilliSecond();
		pFrame->m_bySendNum = 0;
		SComTask_SendListAppend(pMag1->m_sTaskMag.m_pSendList, (void*)pFrame);
		g_sResComTask_Mag.EvtListResPonseNum[pMag->m_aES_Num[0] << 8 | pMag->m_aES_Num[1]] = TRUE;

		if(g_sUserParam.m_byMode == GAME_GOING)
		{
			g_sResComTask_Mag.EvtRspFlg[g_sResComTask_Mag.m_dwCtrlEvtNum / 8] |= (1 << (g_sResComTask_Mag.m_dwCtrlEvtNum % 8));
		
			if(g_sResComTask_Mag.m_dwCtrlEvtNum < 240)
			{
				EventStorageToFlash(pFrame, sizeof(FRAME_BUF), g_sResComTask_Mag.m_dwCtrlEvtNum);
			}
		}
}

void RSP_InjuryWorseEventData_Spell(StructEvent_InjuryWorse *pMag, u8 *pBuf, u8 *Len)
{
    u8 num = 0;

    //���
    pBuf[num++] = BNBNEventType_InjuryWorse;
    pBuf[num++] = pMag->m_byT_Type;
    pBuf[num++] = pMag->m_byT_Subtype;
    pBuf[num++] = pMag->m_byRID;
    pBuf[num++] = pMag->m_aPID[0];
    pBuf[num++] = pMag->m_aPID[1];
    pBuf[num++] = pMag->m_byC_Idegree;
    memcpy(&pBuf[num], &pMag->L_m_sPosition, sizeof(STRUCT_LPOS));
    num += sizeof(STRUCT_LPOS);
    memcpy(&pBuf[num], &pMag->m_sTime, sizeof(STRUCT_TIME1));
    num += sizeof(STRUCT_TIME1);

    *Len = num;
}

//���ƶ�
void RSP_InjuryWorseEventUpload_Send(Struct_EventUpload *pMag)
{
		RESCOMTASK_MAG *pMag1 = &g_sResComTask_Mag;
		FRAME_BUF *pFrame = NULL;
		
		uint8_t cut = 0,Len = 0;

		pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

		if(pFrame == NULL)
		{
				return;
		}		
		
		memset(BackStage_dataBuf,0,sizeof(BackStage_dataBuf));						
		
		//pMag->m_aData_Frame[0] = 0xA8;
		//pMag->m_aData_Frame[1] = BNCmd_Terminal_EventUpload;

		pFrame->m_wBufLen = 0;
		pFrame->m_byBuf[pFrame->m_wBufLen++] = 0xA8;//pMag->m_aData_Frame[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = BNCmd_Terminal_EventUpload;//pMag->m_aData_Frame[1];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
		
		RSP_InjuryWorseEventData_Spell(&pMag->Event_Pack.m_sInjuryWorse, BackStage_dataBuf, &Len);
		memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen],BackStage_dataBuf,Len);
		pFrame->m_wBufLen += Len;
		
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[1];

		//���¼���
		pMag->m_byXor = 0;
		for(cut = 3 ;cut < pFrame->m_wBufLen - 2; cut++)
		{
			pMag->m_byXor ^= pFrame->m_byBuf[cut];
		}
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_byXor;
		
		pFrame->m_byBuf[2] = pFrame->m_wBufLen - 6;
		
		pFrame->m_dwCnt = 1;
		pFrame->m_dwID = (pFrame->m_byBuf[0] << 24 | pFrame->m_byBuf[1] << 16 |
											pFrame->m_byBuf[2] << 8 | pFrame->m_byBuf[3]);
		pFrame->m_dwScheTime = 1000;
		pFrame->m_bySendNum = 0;
		pFrame->m_dwTime = GetSysRunMilliSecond();
		SComTask_SendListAppend(pMag1->m_sTaskMag.m_pSendList, (void*)pFrame);
		g_sResComTask_Mag.EvtListResPonseNum[pMag->m_aES_Num[0] << 8 | pMag->m_aES_Num[1]] = TRUE;

		if(g_sUserParam.m_byMode == GAME_GOING)
		{
			g_sResComTask_Mag.EvtRspFlg[g_sResComTask_Mag.m_dwCtrlEvtNum / 8] |= (1 << (g_sResComTask_Mag.m_dwCtrlEvtNum % 8));
		
			if(g_sResComTask_Mag.m_dwCtrlEvtNum < 240)
			{
				EventStorageToFlash(pFrame, sizeof(FRAME_BUF), g_sResComTask_Mag.m_dwCtrlEvtNum);
			}
		}
}

/****************************************************************
@FUNCTION���º�̨�˻��������¼��������
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��LW
@SPECIAL:
****************************************************************/
void NCB_InjuryEventData_Spell(StructEvent_NCB_Injury *pMag, u8 *pBuf, u8 *Len)
{
    u8 num = 0;

    pBuf[num++] = BNEventType_NCB_Injury;
    pBuf[num++] = pMag->m_byT_Type;
    pBuf[num++] = pMag->m_byT_Subtype;
    pBuf[num++] = pMag->m_byRID;
    pBuf[num++] = pMag->m_aPID[0];
    pBuf[num++] = pMag->m_aPID[1];
    pBuf[num++] = pMag->m_byC_Idegree;

    memcpy(&pBuf[num], &pMag->m_sL_Position, sizeof(STRUCT_LPOS));
    num += sizeof(STRUCT_LPOS);
    memcpy(&pBuf[num], &pMag->m_sNCBPTime, sizeof(STRUCT_TIME1));
    num += sizeof(STRUCT_TIME1);

    pBuf[num++] = pMag->m_byFT_Type;
    pBuf[num++] = pMag->m_byFT_Subtype;
    pBuf[num++] = pMag->m_byF_RID;
    pBuf[num++] = pMag->m_aF_PID[0];
    pBuf[num++] = pMag->m_aF_PID[1];
    pBuf[num++] = pMag->m_byNCBT_Type;
    pBuf[num++] = pMag->m_byH_Part;
    pBuf[num++] = pMag->m_byH_Type;
    pBuf[num++] = pMag->m_byH_Idegree;

    *Len = num;
}

/****************************************************************
@FUNCTION���º�̨�˻��������¼��ϴ�
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��LW
@SPECIAL:
****************************************************************/
void NCB_InjuryEventUpload_Send(Struct_EventUpload *pMag)
{
		RESCOMTASK_MAG *pMag1 = &g_sResComTask_Mag;
		FRAME_BUF *pFrame = NULL;
		
		uint8_t cut = 0,Len = 0;

		pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

		if(pFrame == NULL)
		{
				return;
		}		
		
		memset(BackStage_dataBuf,0,sizeof(BackStage_dataBuf));						
		
		pMag->m_aData_Frame[0] = 0xA8;
		pMag->m_aData_Frame[1] = BNCmd_Terminal_EventUpload;

		pFrame->m_wBufLen = 0;
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[1];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
		
		NCB_InjuryEventData_Spell(&pMag->Event_Pack.m_sNCB_Injury, BackStage_dataBuf, &Len);
    memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen],BackStage_dataBuf,Len);
		pFrame->m_wBufLen += Len;
		
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[1];

		//���¼���
		pMag->m_byXor = 0;
		
		for(cut = 3 ;cut < pFrame->m_wBufLen - 2; cut++)
		{
			pMag->m_byXor ^= pFrame->m_byBuf[cut];
		}
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_byXor;

		pFrame->m_byBuf[2] = pFrame->m_wBufLen - 6;
		
		pFrame->m_dwCnt = 1;
		pFrame->m_dwID = (pFrame->m_byBuf[0] << 24 | pFrame->m_byBuf[1] << 16 |
											pFrame->m_byBuf[2] << 8 | pFrame->m_byBuf[3]);
		pFrame->m_dwScheTime = 1000;
		pFrame->m_bySendNum = 0;
		pFrame->m_dwTime = GetSysRunMilliSecond();
		SComTask_SendListAppend(pMag1->m_sTaskMag.m_pSendList, (void*)pFrame);
		g_sResComTask_Mag.EvtListResPonseNum[pMag->m_aES_Num[0] << 8 | pMag->m_aES_Num[1]] = TRUE;

		if(g_sUserParam.m_byMode == GAME_GOING)
		{
			g_sResComTask_Mag.EvtRspFlg[g_sResComTask_Mag.m_dwCtrlEvtNum / 8] |= (1 << (g_sResComTask_Mag.m_dwCtrlEvtNum % 8));
		
			if(g_sResComTask_Mag.m_dwCtrlEvtNum < 240)
			{
				EventStorageToFlash(pFrame, sizeof(FRAME_BUF), g_sResComTask_Mag.m_dwCtrlEvtNum);
			}
		}
}

/****************************************************************
@FUNCTION���º�̨��ϰ��������(����)
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��MTY
@SPECIAL:
****************************************************************/
void BNSoldierManoParamSet(uint8_t *pBuf)
{
		Struct_CtrlManoParam_IndividualSoldierDownload ManoParam;
    uint8_t i;
    uint8_t offset = 3;
    
    uint32_t LastTimeMs, NowTimeMs;

    memset(&ManoParam, 0, sizeof(Struct_CtrlManoParam_IndividualSoldierDownload));

    //Э�����ݽ���
    ManoParam.m_byT_Type	  = pBuf[offset++];
    ManoParam.m_byT_Subtype = pBuf[offset++];
    ManoParam.m_byRID   = pBuf[offset++];
    ManoParam.m_aPID[0] = pBuf[offset++];
    ManoParam.m_aPID[1] = pBuf[offset++];
    ManoParam.m_byMasterT_Type    = pBuf[offset++];
    ManoParam.m_byMasterT_Subtype = pBuf[offset++];
    ManoParam.m_byMasterRID = pBuf[offset++];
    ManoParam.m_wMasterPID  = pBuf[offset] << 8 | pBuf[offset + 1];
    offset += 2;
		
    ManoParam.m_sStartTime.m_byYear  = pBuf[offset++];
    ManoParam.m_sStartTime.m_byMonth = pBuf[offset++];
    ManoParam.m_sStartTime.m_byDate  = pBuf[offset++];
    ManoParam.m_sStartTime.m_byHour  = pBuf[offset++];
    ManoParam.m_sStartTime.m_byMinute = pBuf[offset++];
    ManoParam.m_sStartTime.m_bySecond = pBuf[offset++];

    ManoParam.m_sDayStartTime.m_byHour   = pBuf[offset++];
    ManoParam.m_sDayStartTime.m_byMinute = pBuf[offset++];
    ManoParam.m_sDayStartTime.m_bySecond = pBuf[offset++];
    ManoParam.m_sNightStartTime.m_byHour = pBuf[offset++];
    ManoParam.m_sNightStartTime.m_byMinute = pBuf[offset++];
    ManoParam.m_sNightStartTime.m_bySecond = pBuf[offset++];

    ManoParam.m_byKey = pBuf[offset++];
    ManoParam.m_byBR_Attribute = pBuf[offset++];
    ManoParam.m_byBulletMaxAmmuNum[0] = pBuf[offset++];
    ManoParam.m_byBulletMaxAmmuNum[1] = pBuf[offset++];
    ManoParam.m_byBulletMaxAmmuNum[2] = pBuf[offset++];
		
    ManoParam.m_byShellMaxAmmuNum[0] = pBuf[offset++];
    ManoParam.m_byShellMaxAmmuNum[1] = pBuf[offset++];
    ManoParam.m_byShellMaxAmmuNum[2] = pBuf[offset++];
		
    ManoParam.m_byAmmuNum = pBuf[offset++];

    //��ֹ����Խ�磬Ŀǰ�����������
    if(ManoParam.m_byAmmuNum > 3)
        ManoParam.m_byAmmuNum = 3;

    for(i = 0; i < ManoParam.m_byAmmuNum; i++)
    {
        ManoParam.m_sAmmu[i].m_byWeapon_Type 	= pBuf[offset++];
        ManoParam.m_sAmmu[i].m_byWeapon_SubType = pBuf[offset++];
        ManoParam.m_sAmmu[i].m_byCfgBulletKind 	= pBuf[offset++];

        for(uint8_t j = 0; j < ManoParam.m_sAmmu[i].m_byCfgBulletKind; j++)
        {
            ManoParam.m_sAmmu[i].m_sBulletInfo[j].m_byBullet_Type 		= pBuf[offset++];
            ManoParam.m_sAmmu[i].m_sBulletInfo[j].m_byBullet_SubType 	= pBuf[offset++];
            ManoParam.m_sAmmu[i].m_sBulletInfo[j].m_aBulletCfgNum[0] 	= pBuf[offset++];
            ManoParam.m_sAmmu[i].m_sBulletInfo[j].m_aBulletCfgNum[1] 	= pBuf[offset++];
            ManoParam.m_sAmmu[i].m_sBulletInfo[j].m_aBulletCfgNum[2] 	= pBuf[offset++];
					
						memcpy(ManoParam.m_sAmmu[i].m_sBulletInfo[j].m_aBulletTotalNum, ManoParam.m_sAmmu[i].m_sBulletInfo[j].m_aBulletCfgNum, 3);
            
						ManoParam.m_sAmmu[i].m_sBulletInfo[j].m_byAmmuWearDepth 	= pBuf[offset++];
            ManoParam.m_sAmmu[i].m_sBulletInfo[j].m_byAmmuConcreteDepth = pBuf[offset++];
            ManoParam.m_sAmmu[i].m_sBulletInfo[j].m_byAmmuRange 		= pBuf[offset++];
        }
    }

    ManoParam.EP_Num[0] = pBuf[offset++];
    ManoParam.EP_Num[1] = pBuf[offset++];
    ManoParam.EP_Num[2] = pBuf[offset++];

    ManoParam.Thickness = pBuf[offset++];
		
		/*
		if((g_sUserParam.EP_Num[0] == ManoParam.EP_Num[0]) &&
		 (g_sUserParam.EP_Num[1] == ManoParam.EP_Num[1]) &&
		 (g_sUserParam.EP_Num[2] == ManoParam.EP_Num[2]))
		{
				return;
		}
		*/
		
    ManoParam.m_sEndTime.m_byYear 	= pBuf[offset++];
    ManoParam.m_sEndTime.m_byMonth 	= pBuf[offset++];
    ManoParam.m_sEndTime.m_byDate 	= pBuf[offset++];
    ManoParam.m_sEndTime.m_byHour 	= pBuf[offset++];
    ManoParam.m_sEndTime.m_byMinute = pBuf[offset++];
    ManoParam.m_sEndTime.m_bySecond = pBuf[offset++];

    //RID��PIDУ��
    if(CheckRID(ManoParam.m_byRID) && CheckPID((ManoParam.m_aPID[0] << 8 | ManoParam.m_aPID[1])))
    {
        //������վӦ�ó����������յ���ָͬ��
				LastTimeMs = g_sResComTask_Mag.m_sCmdTime.m_ulCtrlManoeuvre;
        NowTimeMs = GetSysRunMilliSecond();

        if((NowTimeMs < LastTimeMs) || ((NowTimeMs - LastTimeMs) < 2000))
        {
            return;
        }

        g_sResComTask_Mag.m_sCmdTime.m_ulCtrlManoeuvre = NowTimeMs;

        //���ν��յ����ĵ�utcʱ��
        RecordRcvBackStageProtocolTime(&g_sResComTask_Mag.m_sCmdTime.m_BSNulCtrlManoeuvre);
        
        //���ݴ���
        if(TRUE == BNSoldierManoParamSetHandle(ManoParam))
        {
						Wtn6_Play(WTN6_TERMINAL_ONLINE);
						Wtn6_Play(WTN6_MANO_PARAM_CFG_SUCCESS);
						g_sResComTask_Mag.m_IsAccess = 	TRUE;
            //�¼��ϴ�
            CtrlManoParamEventUploadHandle(&ManoParam);
        }
    }
}

/****************************************************************
@FUNCTION���º�̨�ϴ���������
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��MTY
@SPECIAL:
****************************************************************/
void BNIntervalSet(u8 *pBuf)
{
		Struct_CtrlIntervalDownload Interval;
    uint32_t LastTimeMs, NowTimeMs;
    uint8_t offset = 3;

    //���ݽ���
    Interval.m_byT_Type = pBuf[offset++];
    Interval.m_byT_Subtype = pBuf[offset++];
    Interval.m_byRID = pBuf[offset++];
    Interval.m_aPID[0] = pBuf[offset++];
    Interval.m_aPID[1] = pBuf[offset++];
    Interval.m_byMasterT_Type = pBuf[offset++];
    Interval.m_byMasterT_Subtype = pBuf[offset++];
    Interval.m_byMasterRID = pBuf[offset++];
    Interval.m_aMasterPID[0] = pBuf[offset++];
    Interval.m_aMasterPID[1] = pBuf[offset++];
    Interval.m_bySelfStatusDataUploadCycle = pBuf[offset++];
    Interval.m_byDynamometerDisplayStatusDataUploadCycle = pBuf[offset++];
    Interval.m_byEquipmentAndIdentityDataUploadCycle = pBuf[offset++];

    //RID��PIDУ��
    if(CheckRID(Interval.m_byRID) && CheckPID((Interval.m_aPID[0] << 8 | Interval.m_aPID[1])))
    {
        //������վӦ�ó����������յ���ָͬ��
        LastTimeMs = g_sResComTask_Mag.m_sCmdTime.m_ulCtrlInterval;
        NowTimeMs = GetSysRunMilliSecond();

        if((NowTimeMs < LastTimeMs) || ((NowTimeMs - LastTimeMs) < 1000))
        {
            return;
        }

        g_sResComTask_Mag.m_sCmdTime.m_ulCtrlInterval = NowTimeMs;
        //���ν��յ����ĵ�utcʱ��
        RecordRcvBackStageProtocolTime(&g_sResComTask_Mag.m_sCmdTime.m_sBSNulCtrlInterval);

        //���ݴ���
        BNIntervalSetHandle(Interval);

				//���ݱ���,�ϴ���������ָ���·�ʱ�����������20220722
				SaveDrillState(g_sUserParam.m_byMode, g_sUserParam.m_byEnableChangeBat);

        //�¼��ϴ�
        RSP_CtrlIntervalEventUploadHandle(&Interval);
    }
}

/****************************************************************
@FUNCTION���º�̨��ҩ���ʲ���
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��MTY
@SPECIAL:
****************************************************************/
void BNSupplement(u8 *pBuf)
{
		Struct_CtrlSupplementDownload Supplement;
    u8 i, offset = 3;
    uint32_t LastTimeMs, NowTimeMs;

    memset(&Supplement, 0, sizeof(Struct_CtrlSupplementDownload));

    //���ݽ���
    Supplement.m_byT_Type = pBuf[offset++];
    Supplement.m_byT_Subtype = pBuf[offset++];
    Supplement.m_byRID = pBuf[offset++];
    Supplement.m_aPID[0] = pBuf[offset++];
    Supplement.m_aPID[1] = pBuf[offset++];
    Supplement.m_byMasterT_Type = pBuf[offset++];
    Supplement.m_byMasterT_Subtype = pBuf[offset++];
    Supplement.m_byMasterRID = pBuf[offset++];
    Supplement.m_aMasterPID[0] = pBuf[offset++];
    Supplement.m_aMasterPID[1] = pBuf[offset++];
    Supplement.m_byChangeAmmuNum = pBuf[offset++];
    Supplement.m_byChangeSuppleNum = pBuf[offset++];

    for(i = 0; i < Supplement.m_byChangeAmmuNum; i++) //�仯�ĵ�ҩ������
    {
        Supplement.m_sChangeAmmu[i].m_byA_Type = pBuf[offset++];
        Supplement.m_sChangeAmmu[i].m_byA_Subtype = pBuf[offset++];
        Supplement.m_sChangeAmmu[i].m_aChangeAmmuNum[0] = pBuf[offset++];
        Supplement.m_sChangeAmmu[i].m_aChangeAmmuNum[1] = pBuf[offset++];
        Supplement.m_sChangeAmmu[i].m_aChangeAmmuNum[2] = pBuf[offset++];
    }

    for(i = 0; i < Supplement.m_byChangeSuppleNum; i++) //�仯������������
    {
        Supplement.m_sChangeSupply[i].m_byM_Type = pBuf[offset++];
        Supplement.m_sChangeSupply[i].m_byM_Subtype = pBuf[offset++];
        Supplement.m_sChangeSupply[i].m_aRemainSupplyNum[0] = pBuf[offset++];
        Supplement.m_sChangeSupply[i].m_aRemainSupplyNum[1] = pBuf[offset++];
        Supplement.m_sChangeSupply[i].m_aRemainSupplyNum[2] = pBuf[offset++];
    }

    //RID��PIDУ��
    if(CheckRID(Supplement.m_byRID) && CheckPID((Supplement.m_aPID[0] << 8 | Supplement.m_aPID[1])))
    {
        //������վӦ�ó����������յ���ָͬ��
        LastTimeMs = g_sResComTask_Mag.m_sCmdTime.m_ulCtrlSupplement;
        NowTimeMs = GetSysRunMilliSecond();

        if((NowTimeMs < LastTimeMs) || ((NowTimeMs - LastTimeMs) < 1000))
        {
            return;
        }

        g_sResComTask_Mag.m_sCmdTime.m_ulCtrlSupplement = NowTimeMs;

        //���ν��յ����ĵ�utcʱ��
        RecordRcvBackStageProtocolTime(&g_sResComTask_Mag.m_sCmdTime.m_sBSNulCtrlSupplement);

        //���ݴ���
        BNSupplementHandle(&Supplement);

        //�¼��ϴ�
        RSP_CtrlSupplementEventUploadHandle(&Supplement);
    }
}

/****************************************************************
@FUNCTION���º�̨ϵͳ�þ�
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��MTY
@SPECIAL:
****************************************************************/
void BNJudge(uint8_t *pBuf)
{
	  Struct_JudgeDownload Judge;
    RCVHURTINFO_T Info = {0};
    uint32_t LastTimeMs, NowTimeMs;
    uint8_t offset = 3;

    //���ݽ���
    Judge.m_byT_Type = pBuf[offset++];
    Judge.m_byT_Subtype = pBuf[offset++];
    Judge.m_byRID = pBuf[offset++];
    Judge.m_aPID[0] = pBuf[offset++];
    Judge.m_aPID[1] = pBuf[offset++];
    Judge.m_byMasterT_Type = pBuf[offset++];
    Judge.m_byMasterT_Subtype = pBuf[offset++];
    Judge.m_byMasterRID = pBuf[offset++];
    Judge.m_aMasterPID[0] = pBuf[offset++];
    Judge.m_aMasterPID[1] = pBuf[offset++];
    Judge.m_byAC_Character = pBuf[offset++];

    //RID��PIDУ��
    if(CheckRID(Judge.m_byRID) && CheckPID((Judge.m_aPID[0] << 8 | Judge.m_aPID[1])))
    {
        //������վӦ�ó����������յ���ָͬ��
        LastTimeMs = g_sResComTask_Mag.m_sCmdTime.m_ulCtrlBloodVolume;
        NowTimeMs = GetSysRunMilliSecond();

        if((NowTimeMs < LastTimeMs) || ((NowTimeMs - LastTimeMs) < 1000))
        {
            return;
        }

        g_sResComTask_Mag.m_sCmdTime.m_ulCtrlBloodVolume = NowTimeMs;

        //���ν��յ����ĵ�utcʱ��
        RecordRcvBackStageProtocolTime(&g_sResComTask_Mag.m_sCmdTime.m_sBSNJudge);
        memcpy(&Judge.m_judgeTime, &g_sResComTask_Mag.m_sCmdTime.m_sBSNJudge, sizeof(STRUCT_TIME1));
        //���ݴ���
        Info.m_A_Type 		= 0xff;
        Info.m_A_Subtype 	= 0x01;
        Info.m_HType 		  = H_Type_I_BACKSTAGECTRL;
        Info.m_RT_Subtype = Judge.m_byMasterT_Type;
        Info.m_RT_Subtype = Judge.m_byMasterT_Subtype;
        Info.m_RRID 		= Judge.m_byMasterRID;
        Info.m_wRPID 		= Judge.m_aMasterPID[0] | Judge.m_aMasterPID[1];
        BackStageJudgeHandle(Judge.m_byAC_Character, Info);
        
				//�¼��ϴ�
        BNJudgeEventUploadHandle(&Judge);
    }
}

/****************************************************************
@FUNCTION���º�̨����ģ����ָ��
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��MTY
@SPECIAL:
****************************************************************/
void BNIndirectAim(uint8_t *pBuf)
{
		Struct_DamageReportDownload IndirectAim;
		RCVHURTINFO_T HurtInfo;
    uint32_t LastTimeMs, NowTimeMs;
    uint8_t HurtBlood = 0;
    uint8_t UniID;
    uint8_t offset = 3;
	
    //���ݽ���
    IndirectAim.m_byT_Type = pBuf[offset++];
    IndirectAim.m_byT_Subtype = pBuf[offset++];
    IndirectAim.m_byRID = pBuf[offset++];
    IndirectAim.m_aPID[0] = pBuf[offset++];
    IndirectAim.m_aPID[1] = pBuf[offset++];
    IndirectAim.m_byH_Part = pBuf[offset++];
    IndirectAim.m_byH_Type = pBuf[offset++];
    IndirectAim.m_byH_Idegree = pBuf[offset++];
    IndirectAim.m_sAttackTime.m_byYear = pBuf[offset++];
    IndirectAim.m_sAttackTime.m_byMonth = pBuf[offset++];
    IndirectAim.m_sAttackTime.m_byDate = pBuf[offset++];
    IndirectAim.m_sAttackTime.m_byHour = pBuf[offset++];
    IndirectAim.m_sAttackTime.m_byMinute = pBuf[offset++];
    IndirectAim.m_sAttackTime.m_bySecond = pBuf[offset++];
    IndirectAim.m_byFT_Type = pBuf[offset++];     //������ʵ������
    IndirectAim.m_byFT_Subtype = pBuf[offset++];
    IndirectAim.m_byF_RID = pBuf[offset++];
    IndirectAim.m_aF_PID[0] = pBuf[offset++];
    IndirectAim.m_aF_PID[1] = pBuf[offset++];
    IndirectAim.m_aFA_Type = pBuf[offset++];      //��������ҩ����
    IndirectAim.m_aFA_Subtype = pBuf[offset++];

    memcpy(&IndirectAim.m_sL_Position, &pBuf[offset++], sizeof(STRUCT_LPOS)) ;

    //RID��PIDУ��
    if(CheckRID(IndirectAim.m_byRID) && CheckPID((IndirectAim.m_aPID[0] << 8 | IndirectAim.m_aPID[1])))
    {
        if(g_sUserParam.m_byBlood == 0)
        {
            return;
        }

        //������վӦ�ó����������յ���ָͬ��
        LastTimeMs = g_sResComTask_Mag.m_sCmdTime.m_ulCtrlSimulateAim;
        NowTimeMs = GetSysRunMilliSecond();

        if((NowTimeMs < LastTimeMs) || ((NowTimeMs - LastTimeMs) < 2000))
        {
            return;
        }

        g_sResComTask_Mag.m_sCmdTime.m_ulCtrlInterval = NowTimeMs;
        //���ν��յ����ĵ�utcʱ��
        RecordRcvBackStageProtocolTime(&g_sResComTask_Mag.m_sCmdTime.m_sBSNulCtrlInterval);

        //���ݴ���
        //Ѫ��
        HurtBlood = GetInjuryBloodByHurtDegree((ENUM_CI_INDIVIDUAL)IndirectAim.m_byH_Idegree);
        //ͳһ����ID
        GetBulletUniID(IndirectAim.m_aFA_Type, IndirectAim.m_aFA_Subtype, &UniID);		//��ȡ����ͳһ����

        //�¼��ϴ�
        HurtInfo.m_RT_Type = IndirectAim.m_byFT_Type;
        HurtInfo.m_RT_Subtype = IndirectAim.m_byFT_Subtype;
        HurtInfo.m_RRID = IndirectAim.m_byF_RID;
        HurtInfo.m_wRPID = IndirectAim.m_aF_PID[0] << 8 | IndirectAim.m_aF_PID[1];
        HurtInfo.m_A_Type = IndirectAim.m_aFA_Type;
        HurtInfo.m_A_Subtype = IndirectAim.m_aFA_Subtype;
        HurtInfo.m_HurtBlood = GetInjuryBloodByHurtDegree((ENUM_CI_INDIVIDUAL)IndirectAim.m_byH_Idegree);
        HurtInfo.m_HPart = (ENUM_PARTITION)IndirectAim.m_byH_Part;
        HurtInfo.m_HType = (ENUM_H_Type_INDIVIDUAL)IndirectAim.m_byH_Type;
        HurtInfo.m_HurtBlood = HurtBlood;
				memcpy(&HurtInfo.m_sFPosition, &IndirectAim.m_sL_Position, sizeof(STRUCT_LPOS)) ;

        //����
        IndirectAim.m_byH_Part = ReverseMapHurtPart(IndirectAim.m_byH_Part);
        HMI_GetInjury(&g_sHmiComTask_Mag, IndirectAim.m_byH_Part, IndirectAim.m_byFT_Type, IndirectAim.m_byFT_Subtype);
				Wtn6_Play(WTN6_ShellHit);
        UpdateDamage(HurtInfo);
        DamageReportEventUploadHandle(&HurtInfo);
    }
}

/****************************************************************
@FUNCTION���º�̨��������в״̬
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��MTY
@SPECIAL:
****************************************************************/
void BNNCBT(uint8_t *pBuf)
{
		Struct_NCBTDownload NCBT;
    uint32_t LastTimeMs, NowTimeMs;
    uint8_t offset = 3;
	
    //���ݽ���
    NCBT.m_byT_Type = pBuf[offset++];
    NCBT.m_byT_Subtype = pBuf[offset++];
    NCBT.m_byRID = pBuf[offset++];
    NCBT.m_aPID[0] = pBuf[offset++];
    NCBT.m_aPID[1] = pBuf[offset++];
    NCBT.m_byFT_Type = pBuf[offset++];
    NCBT.m_byFT_Subtype = pBuf[offset++];
    NCBT.m_byF_RID = pBuf[offset++];
    NCBT.m_aF_PID[0] = pBuf[offset++];
    NCBT.m_aF_PID[1] = pBuf[offset++];
    NCBT.m_byNCBT_Type = pBuf[offset++];      //��������ʶ��
    NCBT.m_byStrength = pBuf[offset++];       //մȾ����вǿ��
    NCBT.m_byELR_Character = pBuf[offset++];  //���롢�뿪�˻���մȾ�����ʶ��

    //RID��PIDУ��
    if(CheckRID(NCBT.m_byRID) && CheckPID((NCBT.m_aPID[0] << 8 | NCBT.m_aPID[1])))
    {
        //������վӦ�ó����������յ���ָͬ��
        LastTimeMs = g_sResComTask_Mag.m_sCmdTime.m_ulCtrlInOutNBCArea;
        NowTimeMs = GetSysRunMilliSecond();

        if((NowTimeMs < LastTimeMs) || ((NowTimeMs - LastTimeMs) < 1000))
        {
            return;
        }

        g_sResComTask_Mag.m_sCmdTime.m_ulCtrlInOutNBCArea = NowTimeMs;

        //���ݴ���
        NCBTEvent_Handle(&NCBT);

        //�¼��ϴ�
        NCBTEventUpload_Handle(&NCBT);
    }
}

void BNHelicopterVoiceCtrl(uint8_t *pBuf)
{
		Struct_HelicopterVoiceCtr HVC;
    uint32_t LastTimeMs, NowTimeMs;
    uint8_t offset = 3;

    //���ݽ���
    HVC.m_byFT_Type = pBuf[offset++];
    HVC.m_byFT_Subtype = pBuf[offset++];
    HVC.m_byF_RID = pBuf[offset++];
    HVC.m_aF_PID[0] = pBuf[offset++];
    HVC.m_aF_PID[1] = pBuf[offset++];
    HVC.m_byT_Type = pBuf[offset++];	//����ʵ������
    HVC.m_byT_Subtype = pBuf[offset++];	//����ʵ��������
    HVC.m_byRID = pBuf[offset++];
    HVC.m_aPID[0] = pBuf[offset++];
    HVC.m_aPID[1] = pBuf[offset++];
    memcpy(&HVC.m_sL_Position, pBuf, sizeof(STRUCT_LPOS));
    offset += sizeof(STRUCT_LPOS);
    HVC.ELR_Character = pBuf[offset++];
    HVC.m_byXor 			= pBuf[offset++];

    if(CheckRID(HVC.m_byRID) && CheckPID((HVC.m_aPID[0] << 8 | HVC.m_aPID[1] )))
    {
        //������վӦ�ó����������յ���ָͬ��
        LastTimeMs = g_sResComTask_Mag.m_sCmdTime.m_ulCtrlHelicopterVoice;
        NowTimeMs = GetSysRunMilliSecond();

        if((NowTimeMs < LastTimeMs) || ((NowTimeMs - LastTimeMs) < 1000))
        {
            return;
        }

        g_sResComTask_Mag.m_sCmdTime.m_ulCtrlHelicopterVoice = NowTimeMs;
        //���ν��յ����ĵ�utcʱ��
        RecordRcvBackStageProtocolTime(&g_sResComTask_Mag.m_sCmdTime.m_sBSNulCtrlInterval);
        //���ݴ���
        BNHelicopterVoiceCtrlHandle(HVC);
        
        Wtn6_Play(WTN6_AirAttack);
        //�¼��ϴ�
				//RSP_CtrVoiceEventUploadHandle(&HVC);
    }
}

/****************************************************************
@FUNCTION���º�̨�¼�����Ӧ��
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��MTY
@SPECIAL:
****************************************************************/
void BNResponce(uint8_t *pBuf)
{
	Struct_EventResponceDownload Responce;

	uint16_t num = 0;

	Responce.m_byT_Type = pBuf[3];
	Responce.m_byT_Subtype = pBuf[4];
	Responce.m_byRID = pBuf[5];
	Responce.m_aPID[0] = pBuf[6];
	Responce.m_aPID[1] = pBuf[7];
	Responce.m_byES_Num[0] = pBuf[8];
  Responce.m_byES_Num[1] = pBuf[9];

	num = Responce.m_byES_Num[0]<<8 | Responce.m_byES_Num[1];
	g_sResComTask_Mag.EvtRspFlg[num / 8] &= ~(1 << (num % 8));
	
  //RID��PIDУ��
	if(CheckRID(Responce.m_byRID) && CheckPID((Responce.m_aPID[0] << 8 | Responce.m_aPID[1])))
	{
		//����ǵ�һ��Ӧ��esnum=0��˵����̨��Ӧ����������
		if((Responce.m_byES_Num[0] << 8 | Responce.m_byES_Num[1]) == 0)
		{
			//�����ɹ�
			g_sResComTask_Mag.EvtListResPonseNum[Responce.m_byES_Num[0]<<8 | Responce.m_byES_Num[1]] = FALSE;
			g_sResComTask_Mag.m_IsAccess = TRUE;
			//if(g_sResComTask_Mag.m_ManoParamFlg == TRUE)
			//		Wtn6_Play(WTN6_TERMINAL_ONLINE);
		}
		else
		{
			g_sResComTask_Mag.EvtListResPonseNum[Responce.m_byES_Num[0]<<8 | Responce.m_byES_Num[1]] = FALSE;
		}
	}
}

//�ն˲�������
void BNTerminalParamHandle(uint8_t *pBuf)
{
    uint8_t offset = 3;
    uint8_t ttype = 0, tsubtype = 0, rid = 0, pid0 = 0, pid1 = 0;
    uint8_t tmnpara = 0, C_Lock1 = 0;
    uint8_t Xor = 0;

    ttype = pBuf[offset++];
    tsubtype = pBuf[offset++];
    rid = pBuf[offset++];
    pid0 = pBuf[offset++];
    pid1 = pBuf[offset++];
    tmnpara = pBuf[offset++];
    C_Lock1 = pBuf[offset++];

    if((ttype == IDParam_GetTType()) &&
			(tsubtype == IDParam_GetTSubType()) &&
			(rid == IDParam_GetRid()) &&
			(pid0 == ((IDParam_GetPid() >> 8) & 0xff)) &&
			(pid1 == (IDParam_GetPid() & 0xff)))
    {		
        g_sResComTask_Mag.DrillParam.DisableSuicide = tmnpara & 0x01;		//�Ƿ�������ɱ
        g_sResComTask_Mag.DrillParam.DisableKillTeammate = (tmnpara >> 1) & 0x01;		//�Ƿ�����ɱ�˶���
        g_sResComTask_Mag.DrillParam.DisableCheat = (tmnpara >> 2) & 0x01;		//�Ƿ���������
        g_sResComTask_Mag.DrillParam.DisableInstallFume = (tmnpara >> 3) & 0x01;		//�Ƿ�װ����ð
        g_sResComTask_Mag.DrillParam.EnableDiffRid = (tmnpara >> 4) & 0x01;		//�Ƿ�����ͬrid��ս
        g_sResComTask_Mag.DrillParam.DisableAssistLaser = (tmnpara >> 5) & 0x01;		//�Ƿ�����������
        g_sResComTask_Mag.DrillParam.LocationShowType = (tmnpara >> 6) & 0x01;		//��ʾ����ķ�ʽ��γ�Ȼ��Ǹ�˹
        g_sResComTask_Mag.DrillParam.DisableDrillStartVoice = (tmnpara >> 7) & 0x01;		//�仯��ʾ���Ƿ�ر�
        //g_sResComTask_Mag.DrillParam.CfgFSHHurtDegree = C_Lock1;		//�����������������

				if(g_sResComTask_Mag.DrillParam.DisableDrillStartVoice == TRUE)
					Wtn6_NightSound();
				else
					Wtn6_DaySound();
				
				/*
				if(g_sResComTask_Mag.DrillParam.CfgFSHHurtDegree == TRUE)
					InterConn_ProtDeal_Slaver_Weapon_State(TRUE);		//����״̬ͬ��
				else
					InterConn_ProtDeal_Slaver_Weapon_State(FALSE);		//����״̬ͬ��
        */
				
				//���汸����
        memcpy(&g_sResComTask_Mag.BackupArea[0].DrillParam, &g_sResComTask_Mag.DrillParam, sizeof(DRILL_PARAM));

        //����У���
        for(uint32_t i = 0; i < sizeof(PARAMATER_BACKUP) - 1; i++)
        {
            Xor ^= ((u8*)(&g_sResComTask_Mag.BackupArea[0]))[i];
        }
        g_sResComTask_Mag.BackupArea[0].XOR = Xor;
				
				SaveDrillState(g_sUserParam.m_byMode, g_sUserParam.m_byEnableChangeBat);      //BackupDrillState();
    }
}

//�ն���������
void BNSetTerminalTypeHandle(uint8_t *pBuf)
{
    uint8_t offset = 3;
    uint8_t ttype = 0, tsubtype = 0, rid = 0, pid0 = 0, pid1 = 0;
    uint8_t newtsubtype = 0, newrid = 0, newpid0 = 0, newpid1 = 0;
    uint8_t SaveParam_Flag = 0;

    ttype = pBuf[offset++];
    tsubtype = pBuf[offset++];
    rid = pBuf[offset++];
    pid0 = pBuf[offset++];
    pid1 = pBuf[offset++];
    newtsubtype = pBuf[offset++];
    newrid = pBuf[offset++];
    newpid0 = pBuf[offset++];
    newpid1 = pBuf[offset++];

    if((ttype == IDParam_GetTType()) &&
			(tsubtype == IDParam_GetTSubType()) &&
			(rid == IDParam_GetRid()) &&
			(pid0 == ((IDParam_GetPid() >> 8) & 0xff)) &&
			(pid1 == (IDParam_GetPid() & 0xff)))
    {
        if(newtsubtype != 0xff)
        {
            IDParam_SetTSubType(newtsubtype);
            SaveParam_Flag = 1;
        }

        if(newrid != 0xff)
        {
            IDParam_SetRid(newrid);
            SaveParam_Flag = 1;
        }

        if(newpid0 != 0xff && newpid1 != 0xff)
        {
            IDParam_SetPid(newpid0 << 8 | newpid1);
            SaveParam_Flag = 1;
        }

        if(SaveParam_Flag == 1)
        {
            IDParam_Save();
        }
    }
}

//���������·�
void HeartBeatHandle(uint8_t *pBuf)
{
    uint8_t offset = 3;
    uint8_t ttype = 0, tsubtype = 0, rid = 0, pid0 = 0, pid1 = 0;

    ttype = pBuf[offset++];
    tsubtype = pBuf[offset++];
    rid = pBuf[offset++];
    pid0 = pBuf[offset++];
    pid1 = pBuf[offset++];

    if((ttype == IDParam_GetTType()) &&
			(tsubtype == IDParam_GetTSubType()) &&
			(rid == IDParam_GetRid()) &&
			(pid0 == ((IDParam_GetPid() >> 8) & 0xff)) &&
			(pid1 == (IDParam_GetPid() & 0xff)))
    {
				//TODO
    }
}

//���������¼����
void PersonnelLayMineEventData_Spell(StructEvent_IndLayMine *pMag, u8 *pBuf, u8 *Len)
{
    u8 num = 0;

    pBuf[num++] = BNEventType_IndLayMine;
    pBuf[num++] = pMag->m_byT_Type;
    pBuf[num++] = pMag->m_byT_Subtype;
    pBuf[num++] = pMag->m_byRID;
    pBuf[num++] = pMag->m_aPID[0];
    pBuf[num++] = pMag->m_aPID[1];
    pBuf[num++] = pMag->m_byC_Idegree;
    memcpy(&pBuf[num], &pMag->m_sL_Position, sizeof(STRUCT_LPOS));
    num += sizeof(STRUCT_LPOS);
    memcpy(&pBuf[num], &pMag->m_sTime, sizeof(STRUCT_TIME1));
    num += sizeof(STRUCT_TIME1);
    pBuf[num++] = pMag->m_byMT_Type;
    pBuf[num++] = pMag->m_byMT_Subtype;
    pBuf[num++] = pMag->m_byM_RID;
    pBuf[num++] = pMag->m_aM_PID[0];
    pBuf[num++] = pMag->m_aM_PID[1];

    *Len = num;
}


//���������¼��ϴ�
void PersonnelLayMineEventUpload_Send(Struct_EventUpload *pMag)
{
		RESCOMTASK_MAG *pMag1 = &g_sResComTask_Mag;
		FRAME_BUF *pFrame = NULL;
		
		uint8_t cut = 0,Len = 0;

		pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

		if(pFrame == NULL)
		{
				return;
		}		
		
		memset(BackStage_dataBuf,0,sizeof(BackStage_dataBuf));						
		
		pMag->m_aData_Frame[0] = 0xA8;
		pMag->m_aData_Frame[1] = BNCmd_Terminal_EventUpload;

		pFrame->m_wBufLen = 0;
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[1];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
		
		PersonnelLayMineEventData_Spell(&pMag->Event_Pack.m_sIndLayMine, BackStage_dataBuf, &Len);
    memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen],BackStage_dataBuf,Len);
		pFrame->m_wBufLen += Len;
		
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[1];
			
		//���¼���
		pMag->m_byXor = 0;
		for(cut = 3 ;cut < pFrame->m_wBufLen - 2; cut++)
		{
			pMag->m_byXor ^= pFrame->m_byBuf[cut];
		}
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_byXor;
		
		pFrame->m_byBuf[2] = pFrame->m_wBufLen - 6;
		
		pFrame->m_dwCnt = 1;
		pFrame->m_dwID = (pFrame->m_byBuf[0] << 24 | pFrame->m_byBuf[1] << 16 |
											pFrame->m_byBuf[2] << 8 | pFrame->m_byBuf[3]);
		pFrame->m_dwScheTime = 1000;
		pFrame->m_bySendNum = 0;
		pFrame->m_dwTime = GetSysRunMilliSecond();
		SComTask_SendListAppend(pMag1->m_sTaskMag.m_pSendList, (void*)pFrame);
		g_sResComTask_Mag.EvtListResPonseNum[pMag->m_aES_Num[0] << 8 | pMag->m_aES_Num[1]] = TRUE;

		if(g_sUserParam.m_byMode == GAME_GOING)
		{
			g_sResComTask_Mag.EvtRspFlg[g_sResComTask_Mag.m_dwCtrlEvtNum / 8] |= (1 << (g_sResComTask_Mag.m_dwCtrlEvtNum % 8));
		
			if(g_sResComTask_Mag.m_dwCtrlEvtNum < 240)
			{
				EventStorageToFlash(pFrame, sizeof(FRAME_BUF), g_sResComTask_Mag.m_dwCtrlEvtNum);
			}
		}
}

//���������¼����
void PersonnelClearMineEventData_Spell(StructEvent_IndCleanMine *pMag, u8 *pBuf, u8 *Len)
{
    u8 num = 0;

    pBuf[num++] = BNEventType_IndCleanMine;
    pBuf[num++] = pMag->m_byT_Type;
    pBuf[num++] = pMag->m_byT_Subtype;
    pBuf[num++] = pMag->m_byRID;
    pBuf[num++] = pMag->m_aPID[0];
    pBuf[num++] = pMag->m_aPID[1];
    pBuf[num++] = pMag->m_byC_Idegree;
    memcpy(&pBuf[num], &pMag->m_sL_Position, sizeof(STRUCT_LPOS));
    num += sizeof(STRUCT_LPOS);
    memcpy(&pBuf[num], &pMag->m_sTime, sizeof(STRUCT_TIME1));
    num += sizeof(STRUCT_TIME1);
    pBuf[num++] = pMag->m_byMT_Type;
    pBuf[num++] = pMag->m_byMT_Subtype;
    pBuf[num++] = pMag->m_byM_RID;
    pBuf[num++] = pMag->m_aM_PID[0];
    pBuf[num++] = pMag->m_aM_PID[1];

    *Len = num;
}


//���������¼��ϴ�
void PersonnelClearMineEventUpload_Send(Struct_EventUpload *pMag)
{
		RESCOMTASK_MAG *pMag1 = &g_sResComTask_Mag;
		FRAME_BUF *pFrame = NULL;
		
		uint8_t cut = 0,Len = 0;

		pFrame = (FRAME_BUF *)sdram_malloc(0, sizeof(FRAME_BUF));

		if(pFrame == NULL)
		{
				return;
		}		
		
		memset(BackStage_dataBuf,0,sizeof(BackStage_dataBuf));						
		
		pMag->m_aData_Frame[0] = 0xA8;
		pMag->m_aData_Frame[1] = BNCmd_Terminal_EventUpload;

		pFrame->m_wBufLen = 0;
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[1];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;
		
		PersonnelClearMineEventData_Spell(&pMag->Event_Pack.m_sIndCleanMine, BackStage_dataBuf, &Len);
		memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen],BackStage_dataBuf,Len);
		pFrame->m_wBufLen += Len;
		
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[1];
			
		//���¼���
		pMag->m_byXor = 0;
		for(cut = 3 ;cut < pFrame->m_wBufLen - 2; cut++)
		{
			pMag->m_byXor ^= pFrame->m_byBuf[cut];
		}
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_byXor;
		
		pFrame->m_byBuf[2] = pFrame->m_wBufLen - 6;
		
		pFrame->m_dwCnt = 1;
		pFrame->m_dwID = (pFrame->m_byBuf[0] << 24 | pFrame->m_byBuf[1] << 16 |
											pFrame->m_byBuf[2] << 8 | pFrame->m_byBuf[3]);
		pFrame->m_dwScheTime = 1000;
		pFrame->m_bySendNum = 0;
		pFrame->m_dwTime = GetSysRunMilliSecond();
		SComTask_SendListAppend(pMag1->m_sTaskMag.m_pSendList, (void*)pFrame);
		g_sResComTask_Mag.EvtListResPonseNum[pMag->m_aES_Num[0] << 8 | pMag->m_aES_Num[1]] = TRUE;

		if(g_sUserParam.m_byMode == GAME_GOING)
		{
			g_sResComTask_Mag.EvtRspFlg[g_sResComTask_Mag.m_dwCtrlEvtNum / 8] |= (1 << (g_sResComTask_Mag.m_dwCtrlEvtNum % 8));
		
			if(g_sResComTask_Mag.m_dwCtrlEvtNum < 240)
			{
				EventStorageToFlash(pFrame, sizeof(FRAME_BUF), g_sResComTask_Mag.m_dwCtrlEvtNum);
			}
		}
}

void CtrlPJPShotEventUploadHandle(STRUCT_Various_Elements *pMag)
{
	
}

void BSN_Judge_NCBHandle(ENUM_NCBTTYPE NBCtype, RCVHURTINFO_T Info)
{
    uint8_t HurtBlood;

    SetRand();

    if(NBCtype == NCBTTYPE_N)		//��
    {
        Info.m_HType = (ENUM_H_Type_INDIVIDUAL)NCBTTYPE_N;
        HurtBlood = 56 + rand() % 8;
    }
    else if(NBCtype == NCBTTYPE_B)		//��
    {
        Info.m_HType = (ENUM_H_Type_INDIVIDUAL)NCBTTYPE_B;
        HurtBlood = 56 + rand() % 8;
    }
    else if(NBCtype == NCBTTYPE_C)		//��
    {
        Info.m_HType = (ENUM_H_Type_INDIVIDUAL)NCBTTYPE_C;
        HurtBlood = 56 + rand() % 8;
    }

		Info.m_HurtBlood = HurtBlood;
    Info.m_HType = H_Type_I_NuclearChemicalBiological;
    UpdateDamage(Info);
		
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCB_Injury.m_byT_Type        = IDParam_GetTType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCB_Injury.m_byT_Subtype     = IDParam_GetTSubType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCB_Injury.m_byRID           = IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCB_Injury.m_aPID[0]         = IDParam_GetPid() >>8 ;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCB_Injury.m_aPID[1]         = IDParam_GetPid() &0xff;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCB_Injury.m_byC_Idegree     = GetInjuryDegree(UserParam_GetBlood());
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCB_Injury.m_sL_Position     = g_sUserParam.m_sLPosition;    //λ�ú͸߳�
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCB_Injury.m_sNCBPTime       = g_sUserParam.m_sUtcTime;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCB_Injury.m_byFT_Type       = Info.m_RT_Type;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCB_Injury.m_byFT_Subtype    = Info.m_RT_Subtype;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCB_Injury.m_byF_RID         = Info.m_RRID;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCB_Injury.m_aF_PID[0]       = Info.m_wRPID >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCB_Injury.m_aF_PID[1]       = Info.m_wRPID & 0xff;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCB_Injury.m_byNCBT_Type     = NBCtype;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCB_Injury.m_byH_Part        = PARTITION_HEAD;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCB_Injury.m_byH_Type        = H_Type_I_NuclearChemicalBiological;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCB_Injury.m_byH_Idegree     = HurtBlood;

    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;

    NCB_InjuryEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}


//���ƶ��¼��ϴ�
void BNInjuryWorseHandle(void)
{
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sInjuryWorse.m_byT_Type = IDParam_GetTType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sInjuryWorse.m_byT_Subtype = IDParam_GetTSubType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sInjuryWorse.m_byRID = IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sInjuryWorse.m_aPID[0] = IDParam_GetPid() >>8 ;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sInjuryWorse.m_aPID[1] = IDParam_GetPid() &0xff;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sInjuryWorse.m_byC_Idegree = GetInjuryDegree(g_sUserParam.m_byBlood);
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sInjuryWorse.L_m_sPosition = g_sUserParam.m_sLPosition;    //λ�ú͸߳�
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sInjuryWorse.m_sTime = g_sUserParam.m_sUtcTime;

    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;

    RSP_InjuryWorseEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}

//����ñδ��װ�����¼��ϴ�
void FumeCheatStartUploadHandle(void)
{
		g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byT_Type    = IDParam_GetTType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byT_Subtype = IDParam_GetTSubType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byRID       = IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_aPID[0]     = IDParam_GetPid() >>8 ;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_aPID[1]     = IDParam_GetPid() &0xff;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byC_Idegree = GetInjuryDegree(g_sUserParam.m_byBlood);
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_sL_Position = g_sUserParam.m_sLPosition;    //λ�ú͸߳�
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_sTime       = g_sUserParam.m_sUtcTime;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byIE_Type   = IE_SMOKEPOT_DISCONN;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;
    AgaistRuleStartEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}

//����ñδ��װ�����¼��ϴ�
void FumeCheatEndUploadHandle(void)
{
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byT_Type    = IDParam_GetTType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byT_Subtype = IDParam_GetTSubType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byRID       = IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_aPID[0]     = IDParam_GetPid() >>8 ;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_aPID[1]     = IDParam_GetPid() &0xff;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byC_Idegree = GetInjuryDegree(g_sUserParam.m_byBlood);
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_sL_Position = g_sUserParam.m_sLPosition;    //λ�ú͸߳�
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_sStartTime  = g_sResComTask_Mag.FumeInstallTimeStamp;;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_sStopTime   = g_sUserParam.m_sUtcTime;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byIE_Type   = IE_SMOKEPOT_DISCONN;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;
    AgaistRuleStopEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}

void MasterPowerOffCheatStartUploadHandle(void)
{
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byT_Type    = IDParam_GetTType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byT_Subtype = IDParam_GetTSubType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byRID       = IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_aPID[0]     = IDParam_GetPid() >>8 ;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_aPID[1]     = IDParam_GetPid() &0xff;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byC_Idegree = GetInjuryDegree(g_sUserParam.m_byBlood);
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_sL_Position = g_sUserParam.m_sLPosition;    //λ�ú͸߳�
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_sTime       = g_sUserParam.m_sUtcTime;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byIE_Type   = IE_POWEROFF;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;
    AgaistRuleStartEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}

void MasterPowerOffCheatEndUploadHandle(void)	
{
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byT_Type    = IDParam_GetTType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byT_Subtype = IDParam_GetTSubType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byRID       = IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_aPID[0]     = IDParam_GetPid() >>8 ;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_aPID[1]     = IDParam_GetPid() &0xff;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byC_Idegree = GetInjuryDegree(g_sUserParam.m_byBlood);
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_sL_Position = g_sUserParam.m_sLPosition;    //λ�ú͸߳�
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_sStartTime  = g_sUserParam.m_sUtcTime;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_sStopTime   = g_sUserParam.m_sUtcTime;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byIE_Type   = IE_POWEROFF;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;
    AgaistRuleStopEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}

void DetectorLossEvtStartUpload(void)
{
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byT_Type    = IDParam_GetTType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byT_Subtype = IDParam_GetTSubType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byRID       = IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_aPID[0]     = IDParam_GetPid() >>8 ;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_aPID[1]     = IDParam_GetPid() &0xff;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byC_Idegree = GetInjuryDegree(g_sUserParam.m_byBlood);
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_sL_Position = g_sUserParam.m_sLPosition;    //λ�ú͸߳�
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_sTime       = g_sUserParam.m_sUtcTime;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byIE_Type   = IE_DT_DISCONN;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;
    AgaistRuleStartEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}

void DetectorLossEvtStopUpload(void)
{
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byT_Type    = IDParam_GetTType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byT_Subtype = IDParam_GetTSubType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byRID       = IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_aPID[0]     = IDParam_GetPid() >>8 ;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_aPID[1]     = IDParam_GetPid() &0xff;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byC_Idegree = GetInjuryDegree(g_sUserParam.m_byBlood);
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_sL_Position = g_sUserParam.m_sLPosition;    //λ�ú͸߳�
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_sStartTime  = g_sResComTask_Mag.DetectorLossTimeStamp;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_sStopTime   = g_sUserParam.m_sUtcTime;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byIE_Type   = IE_DT_DISCONN;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;
    AgaistRuleStopEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}

void IllegalMoveStartUploadHandle(void)
{
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byT_Type    = IDParam_GetTType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byT_Subtype = IDParam_GetTSubType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byRID       = IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_aPID[0]     = IDParam_GetPid() >>8 ;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_aPID[1]     = IDParam_GetPid() &0xff;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byC_Idegree = GetInjuryDegree(g_sUserParam.m_byBlood);
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_sL_Position = g_sUserParam.m_sLPosition;    //λ�ú͸߳�
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_sTime       = g_sUserParam.m_sUtcTime;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byIE_Type   = IE_MOVE_SOLDIER;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;
    AgaistRuleStartEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}

void IllegalMoveEndUploadHandle(void)
{
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byT_Type    = IDParam_GetTType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byT_Subtype = IDParam_GetTSubType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byRID       = IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_aPID[0]     = IDParam_GetPid() >>8 ;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_aPID[1]     = IDParam_GetPid() &0xff;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byC_Idegree = GetInjuryDegree(g_sUserParam.m_byBlood);
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_sL_Position = g_sUserParam.m_sLPosition;    //λ�ú͸߳�
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_sStartTime  = g_sResComTask_Mag.IllegalMoveTimeStamp;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_sStopTime   = g_sUserParam.m_sUtcTime;
		g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byIE_Type   = IE_MOVE_SOLDIER;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;
    AgaistRuleStopEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}

void HelmetCheatStartUploadHandle(void)
{
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byT_Type    = IDParam_GetTType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byT_Subtype = IDParam_GetTSubType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byRID       = IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_aPID[0]     = IDParam_GetPid() >>8 ;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_aPID[1]     = IDParam_GetPid() &0xff;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byC_Idegree = GetInjuryDegree(g_sUserParam.m_byBlood);
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_sL_Position = g_sUserParam.m_sLPosition;    //λ�ú͸߳�
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_sTime       = g_sUserParam.m_sUtcTime;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byIE_Type   = IE_HELMET_DISCONN;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;
    AgaistRuleStartEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}

void HelmetCheatEndUploadHandle(void)
{
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byT_Type    = IDParam_GetTType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byT_Subtype = IDParam_GetTSubType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byRID       = IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_aPID[0]     = IDParam_GetPid() >>8 ;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_aPID[1]     = IDParam_GetPid() &0xff;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byC_Idegree = GetInjuryDegree(g_sUserParam.m_byBlood);
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_sL_Position = g_sUserParam.m_sLPosition;    //λ�ú͸߳�
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_sStartTime  = g_sUserParam.m_sUtcTime;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_sStopTime   = g_sUserParam.m_sUtcTime;
		g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byIE_Type   = IE_HELMET_DISCONN;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;
    AgaistRuleStopEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}

void BSN_RescureEvtHandle(StructEvent_IndRescued *pMag)
{
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_byT_Type = IDParam_GetTType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_byT_Subtype = IDParam_GetTSubType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_byRID = IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_aPID[0] = IDParam_GetPid() >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_aPID[1] = IDParam_GetPid() & 0xff;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_byC_Idegree = GetInjuryDegree(g_sUserParam.m_byBlood);
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_sL_Position = g_sUserParam.m_sLPosition;    //λ�ú͸߳�
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_sRescueStartTime = pMag->m_sRescueStartTime;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_sRescueEndTime = pMag->m_sRescueEndTime;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_byRT_Type = pMag->m_byT_Type;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_byRT_Subtype = pMag->m_byT_Subtype;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_byR_RID = pMag->m_byR_RID;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_aR_PID[0] = pMag->m_aR_PID[0];
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_aR_PID[1] = pMag->m_aR_PID[1];
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_byCM_Characte = pMag->m_byCM_Characte;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_byCE_Characte = pMag->m_byCE_Characte;

    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;

    BSN_RescureEvtUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}
/****************************************************************
@FUNCTION����̨Э�����
@INPUT��pBuf pLen - ����
@OUTPUT��pIsCutted - �����Ƿ�����
@RETURN: TRUE - �ɹ�FALSE - ʧ��
@AUTHOR��xfw
@SPECIAL:
****************************************************************/
uint32_t Backstage_ProtocalParse(const uint8_t *pBuf, uint16_t *pLen, uint32_t *pIsCutted)
{
    return Protocol_FindC1XORFrame(pBuf, pLen, pIsCutted, 2052 - 4);
}

//�ڲ�������ͨ
uint32_t BackStage_Frame_Parse(uint8_t *pBuf, uint16_t *pLen, uint32_t *pIsCutted)
{
	  uint8_t len;

    DDL_ASSERT((pBuf != NULL) && (pLen != NULL));

    if(pBuf[0] == 0x5A)
    {
        //���ݳ���̫��
        if(*pLen < 5)
        {
            if(pIsCutted != NULL)
            {
                *pIsCutted = TRUE;
            }

            return	TRUE;
        }

        if(pBuf[1] == 0xA5)
        {
            len = pBuf[2];

            if(*pLen < len)
            {
                if(pIsCutted != NULL)
                {
                    *pIsCutted = TRUE;
                }

                return	TRUE;
            }

            if(pBuf[len - 1] == 0xAA)
            {
                *pLen = len;

                if(pIsCutted != NULL)
                {
                    *pIsCutted = FALSE;
                }

                return TRUE;
            }

        }

    }

    if (pIsCutted != NULL)
    {
        *pIsCutted	= FALSE;
    }

    return FALSE;
}
/****************************************************************
@FUNCTION���º�̨����
@INPUT��
@OUTPUT��
@RETURN:
@AUTHOR��ZEG
@SPECIAL:
****************************************************************/
uint8_t BackStageNewAnalysis(uint8_t *pBuf,uint8_t len)
{
	uint8_t cmd = pBuf[1];	
	
	switch(cmd)
	{
			case BNCmd_CtrlManoParam_NotIndividualSoldier:		//��ϰ��������(@�ǵ���)
					//BNManoParamSet(pBuf);
					break;

			case BNCmd_CtrlManoParam_IndividualSoldier:		//��ϰ��������(@����)
					BNSoldierManoParamSet(pBuf);
					break;

			case BNCmd_CtrlInterval:		//�ϴ���������ָ��
					BNIntervalSet(pBuf);
					break;

			case BNCmd_CtrlSupplement:		//��ҩ���ʲ���ָ��
					BNSupplement(pBuf);
					break;

			case BNCmd_CtrlJudge:		//����ϵͳ�þ�ָ��
					BNJudge(pBuf);
					break;

			case BNCmd_CtrlIndirectAim:		//����ģ����ָ��
					BNIndirectAim(pBuf);
					break;

			case BNCmd_CtrlNCBT:		//����/�뿪�˻���մȾ��ͨ��ָ��
					BNNCBT(pBuf);
					break;

			case BNCmd_CtrlHelicopter:		//����/�뿪ֱ�������ɻ��ȸ���ͨ��ָ��
					BNHelicopterVoiceCtrl(pBuf);
					break;

			case BNCmd_BSResponce:		//�¼�����Ӧ��
					BNResponce(pBuf);
					break;

			case BNCmd_CtrlTerminalParam:		//�ն�����ָ��
					BNTerminalParamHandle(pBuf);
					break;

			case BNCmd_CtrlSetTerminalType:		//�����ն�����
					BNSetTerminalTypeHandle(pBuf);
					break;

			case BNCmd_CtrlHeartBeat:		//����
					HeartBeatHandle(pBuf);
					break;

			default:
					break;
	}
	return 0;
}

//�þ�
void Laser_JudgeHandle(LASERCOMTASK_MAG *pMag,uint8_t IsMaster,uint8_t Sta)
{ 
	Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 2);
	
	if(IsMaster)
	{
		switch(Sta)
		{
			case ACHAR_TEST:		//����
					pMag->m_TestMode = TRUE;
					LaserCom_Prot_CtrlMode(LaserComTask_HandleGet(), LASER_MODE_TEST);		//̽ͷ���������ģʽ
          Wtn6_Play(WTN6_TEST_MODEL);
					break;
			
			case ACHAR_DEATH:		//����
					BSN_Judge_DeathHandle(pMag->m_sRcvHurtInfo);
					break;
			
			case ACHAR_REVIVE:		//�л�
					BSN_Judge_ReviveHandle(pMag->m_sRcvHurtInfo);
					break;
			
			case ACHAR_RESTART:		//��λ
					BSN_Judge_RestartHandle(pMag->m_sRcvHurtInfo);
					break;
			
			case ACHAR_INVALID:		//����ʧЧ
					BSN_Judge_InvalidHandle();
					break;
			
			case ACHAR_TOLAST:  //�ָ���ҩ �ָ�����һ�����õ�ҩ��
					BSN_Judge_TolastHandle();
					break;
			
			case ACHAR_MANO:    //��ϰģʽ
					//if(g_sUserParam.m_byMode == TRAIN)
					{
							g_sUserParam.m_byJudge = TRUE;
							ManoStartHandle();
							//����״̬
							FullConditionRecovery();
							SaveDrillState(g_sUserParam.m_byMode, g_sUserParam.m_byEnableChangeBat);
					}
					break;
					
			case ACHAR_TRAIN:		//ѵ��ģʽ
					//if(g_sUserParam.m_byMode == GAME_GOING)
					{
							g_sUserParam.m_byJudge = TRUE;

							ManoEndHandle();

							//����״̬
							SaveDrillState(g_sUserParam.m_byMode, FALSE);
					}
					break;
			
			case ACHAR_DAY:		//����ģʽ 
					DayModeHandle(TRUE);
					break;
			
			case ACHAR_NIGHT:		//ҹ��ģʽ
					NightModeHandle(TRUE);
					break;    
			
			case ACHAR_PERSON_SLIGHT:		//��Ա����
					BSN_Judge_SlightHandle(pMag->m_sRcvHurtInfo);
					break;
			
			case ACHAR_PERSON_MODERATE:		//��Ա�ж���
					BSN_Judge_ModerateHandle(pMag->m_sRcvHurtInfo);
					break;
			
			case ACHAR_PERSON_SEVERE:		//��Ա�ض���
					BSN_Judge_SevereHandle(pMag->m_sRcvHurtInfo);
					break;
			
			case ACHAR_PERSON_CRITICAL:		//��ԱΣ����
					BSN_Judge_CriticalHandle(pMag->m_sRcvHurtInfo);
					break;
			
			case ACHAR_ARMOUR_LOCK_WEAPON:		//�������ⷢ��/��ҵ��������
					BSN_Judge_InvalidHandle();
					break;
			
			case ACHAR_RESTOE:		//��Ա�Ȼ�/װ��ά�޳ɹ����ָ�ս��
					BSN_Judge_RestartHandle(pMag->m_sRcvHurtInfo);//��ͬ����Աװ����λ
					break;
			
			case ACHAR_NEAR:		//����
					ShotAroundHandle(0x02);
					break;
			
			case ACHAR_NUCLEAR:		//����в
					BSN_Judge_NCBHandle(NCBTTYPE_N, pMag->m_sRcvHurtInfo);
					break;
			
			case ACHAR_BIOLOGY:		//����в
					BSN_Judge_NCBHandle(NCBTTYPE_C, pMag->m_sRcvHurtInfo);
					break;
			
			case ACHAR_CHEMISTRY:		//����в
					BSN_Judge_NCBHandle(NCBTTYPE_B, pMag->m_sRcvHurtInfo);
					break;
			
			case ACHAR_PHOTOELE:		//�����в 
					break;
			
			case ACHAR_WORSE:		//���Ƶȼ�����
					BSN_Judge_WorseHandle(pMag->m_sRcvHurtInfo);
					break;
			
			case ACHAR_RELIEF:		//���Ƶȼ�����
					BSN_Judge_ReliefHandle(pMag->m_sRcvHurtInfo);
					break;
			
			case ACHAR_TO_RED:		//תΪ���
					BSN_Judge_ToRedHandle();
					break;
			
			case ACHAR_TO_BLUE:		//תΪ����
					BSN_Judge_ToBlueHandle();
					break;
			
			case ACHAR_DETAIN:		//��Ѻ
					break;

			case ACHAR_SUSPEND_DRILL:	//��ϰ��ͣ
					break;

			case ACHAR_CONTINUE_DRILL:	//��ϰ��ʼ
					break;

			case ACHAR_RELOAD_OIL:		//��������
					break;

			case ACHAR_LOW_VOICE:		//΢��ģʽ
					Wtn6_NightSound();
					break;

			case ACHAR_NORMAL_VOICE:	//��������ģʽ
					Wtn6_DaySound();
					break;
				
			case ACHAR_POWER_OFF:		//�ػ�//55 01 01 02 02 AA
					HardCtrl_Slaver_PowerOff();
					g_sPowerOffMag.m_bPoweroffPre = TRUE;
          break;
				
			default:
				  break;			
		}			
	}
}

/****************************************************************
@FUNCTION�����ݺ������ģ��ID��ȡ��ǰ����ģ�����ڷ���
@INPUT��
	> IsMaster: 1-����ģ��(ͷ��) 0-����
	> byID: �������ģ����
@OUTPUT��
	< pPartition:���ڷ���
@RETURN:��
@AUTHOR��
@SPECIAL:
****************************************************************/
void Laser_GetPartitionByID(u8 IsMaster, u8 byID, ENUM_PARTITION *pPartition)
{
    ENUM_PARTITION PartTemp = PARTITION_NONE;

    if(IsMaster)
    {
        PartTemp = PARTITION_HEAD;

    }
    else  //��ʵ�������ӷ���
    {

        if(byID == 0)
        {
            PartTemp = PARTITION_NONE;
        }
        else
        {
            PartTemp = (ENUM_PARTITION)ID_PARTITION[UserParam_GetActLaserCnt()][byID - 1];
        }
    }

    if(pPartition != NULL)
    {
        *pPartition = PartTemp;
    }

}

void Laser_InsertPartitionAndID(u8 byID, u8 byPartition, LASERCOMTASK_MAG *pMag)
{
    u8 byPartIndex = byPartition - 1;

    assert_param(pMag != NULL);

		if(pMag->m_sPartIDNum[byPartIndex] < LASER_PART_ID_MAX)
    {
        pMag->m_sPartID[byPartIndex][pMag->m_sPartIDNum[byPartIndex]] = byID;
        pMag->m_sPartIDNum[byPartIndex] += 1;
    }
}

// �󻡶�
double radian(double d)
{
    return d * PI / 180.0;   //�Ƕ�1? = �� / 180
}

//�������
double get_distance(double lat1/*γ��1*/, double lng1/*����1*/, double lat2/*γ��2*/, double lng2/*����2*/)
{
    double radLat1 = radian(lat1);
    double radLat2 = radian(lat2);
    double a = radLat1 - radLat2;
    double b = radian(lng1) - radian(lng2);

    double dst = 2 * asin((sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2) )));

    dst = dst * EARTH_RADIUS;

    dst = round(dst * 10000);	//����
    return dst;
}

//�з����Ȼ���
double GetPositionLon(unsigned char* pdata)
{
    unsigned short du    = (pdata[0] & 0x0f) * 100 + ((pdata[1] >> 4) & 0x0f) * 10 + (pdata[1] & 0x0f);
    unsigned short fen   = ((pdata[2] >> 4) & 0x0f) * 10 + (pdata[2] & 0x0f);
    double fen_decimals1 = ((pdata[3] >> 4) & 0x0f) * 1.0 / 10 + (pdata[3] & 0x0f) * 1.0 / 100; //�ֵ�ʮ��λ �ٷ�λ
    double fen_decimals2 = ((pdata[4] >> 4) & 0x0f) * 1.0 / 1000 + (pdata[4] & 0x0f) * 1.0 / 10000; //�ֵ�ǧ��λ ���λ
    double lon = (du + (fen + fen_decimals1 + fen_decimals2) / 60);
    return lon;
}

//�з�γ�Ȼ���
double GetPositionLat(unsigned char* pdata)
{
    unsigned short du    = ((pdata[0] >> 4) & 0x0f) * 10 + (pdata[0] & 0x0f);
    unsigned short fen   = ((pdata[1] >> 4) & 0x0f) * 10 + (pdata[1] & 0x0f);
    double fen_decimals1 = ((pdata[2] >> 4) & 0x0f) * 1.0 / 10 + (pdata[2] & 0x0f) * 1.0 / 100; //�ֵ�ʮ��λ �ٷ�λ
    double fen_decimals2 = ((pdata[3] >> 4) & 0x0f) * 1.0 / 1000 + (pdata[3] & 0x0f) * 1.0 / 10000; //�ֵ�ǧ��λ ���λ
    double lat = (du + (fen + fen_decimals1 + fen_decimals2) / 60);
    return lat;
}


//�з���γ�ȸ߳�
double GetPositionHeight(unsigned char* pdata)
{
    double thousand = (pdata[0] & 0x0f) * 1000 + ((pdata[1] >> 4) & 0x0f) * 100 + (pdata[1] & 0x0f) * 10 \
                      + pdata[2] + pdata[2] * 1.0 / 10;
    return thousand;
}

/*������Ч���,��λ��50��*/
uint8_t WeaponDistanceJudge(uint8_t Range, STRUCT_LPOS Position)
{
		double RemoteLon, RemoteLat, LocalLon, LocalLat, distance;

    //���ؾ���
    LocalLon = g_sResComTask_Mag.m_sNmeaMag.m_byJD_Du + (g_sResComTask_Mag.m_sNmeaMag.m_byJD_Fen + g_sResComTask_Mag.m_sNmeaMag.m_dwJD_Fen_XiaoShu / 100000.0) / 60.0; //��������徭��
							/*	
							g_sUserParam.m_sLPosition.m_byJD_DuHundred * 100 + 
							 g_sUserParam.m_sLPosition.m_byJD_DuTen * 10 + 
							 g_sUserParam.m_sLPosition.m_byJD_DuUnit + 
							 (g_sUserParam.m_sLPosition.m_byJD_FenTen * 10 + 
							 g_sUserParam.m_sLPosition.m_byJD_FenUnit + 
							 (g_sUserParam.m_sLPosition.m_byJD_FenTenth / 10000 + 
							 g_sUserParam.m_sLPosition.m_byJD_FenPercent / 1000+ 
							 g_sUserParam.m_sLPosition.m_byJD_FenThousandth / 100 + 
							 g_sUserParam.m_sLPosition.m_byJD_FenTenThousandth / 10) / 100000.0	 
							 ) / 60.0
							 ;
							 */
    //����γ��
    LocalLat = g_sResComTask_Mag.m_sNmeaMag.m_byWD_Du + (g_sResComTask_Mag.m_sNmeaMag.m_byWD_Fen + g_sResComTask_Mag.m_sNmeaMag.m_dwWD_Fen_XiaoShu / 100000.0) / 60.0; //����������γ��
							/*
							g_sUserParam.m_sLPosition.m_byWD_DuTen * 10 + 
							 g_sUserParam.m_sLPosition.m_byWD_DuUnit + 
							 (g_sUserParam.m_sLPosition.m_byWD_FenTen * 10 + 
							 g_sUserParam.m_sLPosition.m_byWD_FenUnit + 
							 (g_sUserParam.m_sLPosition.m_byWD_FenTenth / 10000 + 
							 g_sUserParam.m_sLPosition.m_byWD_FenPercent / 1000+ 
							 g_sUserParam.m_sLPosition.m_byWD_FenThousandth / 100 + 
							 g_sUserParam.m_sLPosition.m_byWD_FenTenThousandth / 10) / 100000.0	 
							 ) / 60.0
							 ;
							 */
		
    if((Range < 1u) || (LocalLon == 0 && LocalLat == 0) ||
      (Position.m_byJD_DuHundred * 100 + Position.m_byJD_DuTen * 10 + Position.m_byJD_DuUnit <= 1u)
      )
    {
        return TRUE;
    }
									
    //Զ�˾���
    RemoteLon = GetPositionLon((unsigned char*)&Position);
    //Զ��γ��
    RemoteLat = GetPositionLat(&(((unsigned char*)&Position)[5]));
    //�������
    distance = get_distance(LocalLat, LocalLon, RemoteLat, RemoteLon);

    distance = distance / 10.0;	//��Ϊ��λ

    if(RemoteLon < 1.0f || RemoteLat < 1.0f)	//����з�û�о�γ�ȣ����Դ��
    {
        return TRUE;
    }

		if(fabs(distance - (float)(50 * Range)) < 5.0f)	//���С��10��
    {
        return TRUE;
    }
    else if((distance - (float)(50 * Range)) > 0.0f)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

float GetDistance(STRUCT_LPOS Position)
{
		double RemoteLon, RemoteLat, LocalLon, LocalLat, distance;

    //���ؾ���
    LocalLon = g_sResComTask_Mag.m_sNmeaMag.m_byJD_Du + (g_sResComTask_Mag.m_sNmeaMag.m_byJD_Fen + g_sResComTask_Mag.m_sNmeaMag.m_dwJD_Fen_XiaoShu / 100000.0) / 60.0; //��������徭��
							 /*
							 g_sUserParam.m_sLPosition.m_byJD_DuHundred * 100 + 
							 g_sUserParam.m_sLPosition.m_byJD_DuTen * 10 + 
							 g_sUserParam.m_sLPosition.m_byJD_DuUnit + 
							 (g_sUserParam.m_sLPosition.m_byJD_FenTen * 10 + 
							 g_sUserParam.m_sLPosition.m_byJD_FenUnit + 
							 (g_sUserParam.m_sLPosition.m_byJD_FenTenth / 10000 + 
							 g_sUserParam.m_sLPosition.m_byJD_FenPercent / 1000+ 
							 g_sUserParam.m_sLPosition.m_byJD_FenThousandth / 100 + 
							 g_sUserParam.m_sLPosition.m_byJD_FenTenThousandth / 10) / 100000.0	 
							 ) / 60.0
							 ;
							 */
    //����γ��
    LocalLat = g_sResComTask_Mag.m_sNmeaMag.m_byWD_Du + (g_sResComTask_Mag.m_sNmeaMag.m_byWD_Fen + g_sResComTask_Mag.m_sNmeaMag.m_dwWD_Fen_XiaoShu / 100000.0) / 60.0; //����������γ��
							 /*
	             g_sUserParam.m_sLPosition.m_byWD_DuTen * 10 + 
							 g_sUserParam.m_sLPosition.m_byWD_DuUnit + 
							 (g_sUserParam.m_sLPosition.m_byWD_FenTen * 10 + 
							 g_sUserParam.m_sLPosition.m_byWD_FenUnit + 
							 (g_sUserParam.m_sLPosition.m_byWD_FenTenth / 10000 + 
							 g_sUserParam.m_sLPosition.m_byWD_FenPercent / 1000+ 
							 g_sUserParam.m_sLPosition.m_byWD_FenThousandth / 100 + 
							 g_sUserParam.m_sLPosition.m_byWD_FenTenThousandth / 10) / 100000.0	 
							 ) / 60.0
							 ;
							 */
							 
		
    if(LocalLon == 0 && LocalLat == 0)
    {
        return 0.0;
    }
									
    //Զ�˾���
    RemoteLon = GetPositionLon((uint8_t *)&Position);
    //Զ��γ��
    RemoteLat = GetPositionLat(&(((uint8_t *)&Position)[5]));
    //�������
    distance = get_distance(LocalLat, LocalLon, RemoteLat, RemoteLon);

    distance = distance / 10.0;	//��Ϊ��λ

    return distance;
}

//�ж��Ƿ�Ϊ�����豸����Ϊ��������������ӧ6 /���73  /�Ȼ��ڵ�����
uint32_t IsMaster(uint8_t TType)
{
    uint8_t ret;

    switch(TType)
    {
        case TType_PersonalEquipment:
        case TType_HJ73G_FTKDD:
        case TType_HY6_DD:
        case TType_89_82PaiJiPao: //89 ʽ 82mm �Ȼ���
        case TType_89_100PaiJiPao://89 ʽ 100mm �Ȼ���
        case TType_120PaiJiPao:   //120mm �Ȼ���
        case TType_89_60PaiJiPao: //89 ʽ 60mm �Ȼ���
        case TType_82SSPaiJiPao:  //82mm �����Ȼ���
            ret = TRUE;
            break;

        default:
            ret = FALSE;
            break;
    }
    return ret;
}

//̽ͷЭ��
void LaserProtocalDeal(LASERCOMTASK_MAG *pMag, uint8_t *pBuf, uint8_t len, uint8_t IsMaster)
{
	Struct_JudgeDownload Judge;
	STRUCT_LPOS Position;
	ENUM_PARTITION Part;
	Struct_CtrlSupplementDownload Supplement;
	
	uint32_t LastTimeMs, NowTimeMs, SupplyBulletLastTimeMs, SupplyBulletNowTimeMs;
	uint32_t BulletRemain = 0, BulletCfg = 0, BulletSupply = 0;
	uint8_t Id, FA_Type, FA_Subtype, FT_Type, FT_Subtype, FRid, AcChar, GameMode;
	uint8_t BulletSupplyNum[3] = {0};
	uint8_t OutPos[9] = {0};
	uint8_t Range, Velocity3 = 0, Thickness1 = 0, Thickness2 = 0,Belong = 0, TsCharacter2 = 0, Key = 0;		//������Ч����
	uint16_t wFPid, Thickness16 = 0;
	
	if(len < 8)
	{
		return;  
	}
	
	//ģ��װ��������
	if(len == 0x0B)
	{
		//���������յ���ָͬ��
		SupplyBulletLastTimeMs = pMag->m_dwSupplyBulletTimes;
		SupplyBulletNowTimeMs = GetSysRunMilliSecond();
		
		if((SupplyBulletNowTimeMs < SupplyBulletLastTimeMs) || ((SupplyBulletNowTimeMs-SupplyBulletLastTimeMs)<5000))   //�þ����2s
		{
				return;
		}
		Id         = pBuf[3];		//̽ͷID
		FA_Type    = pBuf[5];		//��ҩ����
		FA_Subtype = pBuf[6];		//��ҩ������
		BulletSupplyNum[0] = pBuf[7];
		BulletSupplyNum[1] = pBuf[8];
		BulletSupplyNum[2] = pBuf[9];
		for(u8 w=0;w<UserParam_GetWeaponNum();w++)
		{
			for(u8 b=0;b<g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind;b++)
			{
				if(g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_Type == FA_Type && g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_SubType == FA_Subtype)
				{
					BulletRemain = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[2]<<16 |
												 g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[1]<<8  |
												 g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[0];
					
					BulletCfg = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum[2]<<16 |
											g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum[1]<<8  |
											g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum[0]; 
					
					BulletSupply = BulletSupplyNum[2]<<16 |
												 BulletSupplyNum[1]<<8  |
												 BulletSupplyNum[0];
					
					if(BulletCfg >= (BulletRemain+BulletSupply))
					{
						g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[0] = (BulletRemain+BulletSupply);
						g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[1] = (BulletRemain+BulletSupply)>>8;
						g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[2] = (BulletRemain+BulletSupply)>>16;
					}
					else
					{
						g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[0] =g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum[0];
						g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[1] =g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum[1];
						g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum[2] =g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum[2];
						
						BulletSupply = BulletCfg - BulletRemain;
					}
					
					Wtn6_Play(WTN6_ReplenishAmmunition); //�����ɹ�

					Supplement.m_byT_Type = g_sUserParam.m_saAmmuInfo[w].m_byWeapon_Type;
					Supplement.m_byT_Subtype = g_sUserParam.m_saAmmuInfo[w].m_byWeapon_SubType;
					Supplement.m_byRID = IDParam_GetRid();
					Supplement.m_aPID[0] = IDParam_GetPid() >> 8;
					Supplement.m_aPID[1] = IDParam_GetPid() & 0xFF;
					Supplement.m_byMasterT_Type = 0xff;
					Supplement.m_byMasterT_Subtype = 0xff;
					Supplement.m_byMasterRID = 0xff;
					Supplement.m_aMasterPID[0] = 0xff;
					Supplement.m_aMasterPID[1] = 0xff;
					Supplement.m_byChangeAmmuNum = 1;
					Supplement.m_byChangeSuppleNum = 0;

					for(u8 i=0; i<Supplement.m_byChangeAmmuNum; i++)   //�仯�ĵ�ҩ������
					{
						Supplement.m_sChangeAmmu[i].m_byA_Type = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_Type;
						Supplement.m_sChangeAmmu[i].m_byA_Subtype = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_SubType;
						Supplement.m_sChangeAmmu[i].m_aChangeAmmuNum[0] = BulletSupply;
						Supplement.m_sChangeAmmu[i].m_aChangeAmmuNum[1] = BulletSupply>>8;
						Supplement.m_sChangeAmmu[i].m_aChangeAmmuNum[2] = BulletSupply>>16;
					}

					for(u8 i=0; i<Supplement.m_byChangeSuppleNum; i++)     //�仯������������
					{
						Supplement.m_sChangeSupply[i].m_byM_Type = 0;
						Supplement.m_sChangeSupply[i].m_byM_Subtype = 0;
						Supplement.m_sChangeSupply[i].m_aRemainSupplyNum[0] = 0;
						Supplement.m_sChangeSupply[i].m_aRemainSupplyNum[1] = 0;
						Supplement.m_sChangeSupply[i].m_aRemainSupplyNum[2] = 0;
					}
					RSP_CtrlSupplementEventUploadHandle(&Supplement);					
				}
			}  
		}
		pMag->m_dwSupplyBulletTimes = GetSysRunMilliSecond();
		return;
	}
	
	Id = pBuf[3];		//̽ͷID
	
	if(pBuf[4] != 3)		//��Ч
		return;
	
	FA_Type		   = pBuf[5];		//��ҩ����
	FA_Subtype   = pBuf[6];		//��ҩ������
	FT_Type 		 = pBuf[7];		//��������
	FT_Subtype   = pBuf[8];		//����������
	FRid 				 = pBuf[9];		//RID
	wFPid 			 = pBuf[10] << 8 | pBuf[11];		//PID

	TsCharacter2 = pBuf[12]; 
	memcpy(&Position, &pBuf[13], sizeof(STRUCT_LPOS) - 3);		//�������γ��
	
	Velocity3  = pBuf[len - 6];	
	Thickness1 = pBuf[len - 4];
	Range      = pBuf[len - 2];	
	GameMode   = (TsCharacter2 & 0x80) == 0 ? TRAIN: GAME_GOING;
	Belong     = (TsCharacter2 >> 5) & 0x03;
	Key			 	 = TsCharacter2 & 0x1F;

	//������������
	if(len < 16)		
	{
		//���������յ���ָͬ��
		LastTimeMs = pMag->m_dwLaserCtrlTimes;
		NowTimeMs = GetSysRunMilliSecond();
		
		if((NowTimeMs < LastTimeMs) || ((NowTimeMs - LastTimeMs) < 2000))   //�þ����2s
		{
			return;
		}

		switch(FA_Type) 
		{ 				
			//���Ե�
			case 0x78:
				break;
			
			//���
			case 0x79:		
				break;
			
			//�þ�	
			case 0x7A:
				AcChar = pBuf[13];
				
				pMag->m_sRcvHurtInfo.m_RT_Type = FT_Type;
				pMag->m_sRcvHurtInfo.m_RT_Subtype = FT_Subtype;
				pMag->m_sRcvHurtInfo.m_RRID = FRid;
				pMag->m_sRcvHurtInfo.m_wRPID = wFPid;
				pMag->m_sRcvHurtInfo.m_HType = H_Type_I_TLQCTRL;
				Laser_JudgeHandle(pMag, 1, AcChar);  

				//�¼��ϴ�            
				Judge.m_byMasterT_Type = FT_Type;
				Judge.m_byMasterT_Subtype = FT_Subtype;
				Judge.m_byMasterRID = FRid;
				Judge.m_aMasterPID[0] = wFPid >> 8;
				Judge.m_aMasterPID[1] = wFPid;
				Judge.m_byAC_Character = AcChar;
				memcpy(&Judge.m_judgeTime, UserParam_GetTime1(), sizeof(STRUCT_TIME1));
				
				BNJudgeEventUploadHandle(&Judge);

				break;
					
			default:
				break;
		}
					
		pMag->m_dwLaserCtrlTimes = GetSysRunMilliSecond();    //������һ���յ��þ���ʱ��
	}
	else
	{
			if(FA_Type == 0x79) 	//����벻����
			{
					return;
			}
		
			if((Key != (g_sUserParam.m_byKey & 0x1F)) && (g_sUserParam.m_byMode == GAME_GOING))
				return;
		
			//�е�
			if(IsAlive(g_sUserParam.m_eStatus))
			{
					pMag->m_byLaserTimer = 0;
					pMag->m_sRcvHurtInfo.m_RT_Type    = FT_Type;		//������ʵ������
					pMag->m_sRcvHurtInfo.m_RT_Subtype = FT_Subtype;		//������ʵ��������
					pMag->m_sRcvHurtInfo.m_RRID       = FRid;		//��������λ����
					pMag->m_sRcvHurtInfo.m_wRPID      = wFPid;		//�������豸���
					pMag->m_sRcvHurtInfo.m_A_Type     = FA_Type;		//��������ҩ����
					pMag->m_sRcvHurtInfo.m_A_Subtype  = FA_Subtype;		//��������ҩ������
					memcpy(&pMag->m_sRcvHurtInfo.m_sFPosition, &Position, sizeof(STRUCT_LPOS) - 3);		//����������λ��
					GetBulletUniID(FA_Type, FA_Subtype, &pMag->m_byUniID);		//��ȡ����ͳһ����
					Laser_GetPartitionByID(IsMaster, Id, &Part);		//��ȡ�е�����
					Laser_CtrlLedToggle(Id - 1, pMag);
					pMag->m_sRcvHurtInfo.m_HPart = Part;
					
					//�����ѵ��ģʽ�������κ��жϣ����Ա����
					if(g_sUserParam.m_byMode == TRAIN)
					{
							pMag->m_bIsShoted = TRUE;
							Laser_InsertPartitionAndID(Id, (u8)Part, pMag);		//[�����еĲ�λ][���иò�λ�ļ������]=�ò�λ�ļ���ID
					}
					else if(g_sUserParam.m_byMode == GAME_GOING)
					{			
													
							if(Belong == UserParam_GetBRChar())
							{	
								if(g_sResComTask_Mag.DrillParam.DisableKillTeammate == TRUE)
										return;
							}
							
							if(wFPid == IDParam_GetPid())
							{ 
								if(g_sResComTask_Mag.DrillParam.DisableSuicide == TRUE)
										return;
							}

							if(FRid != IDParam_GetRid())
							{
								if(g_sResComTask_Mag.DrillParam.EnableDiffRid == FALSE)
										return;
							}

							if(Range == 0 || TRUE == WeaponDistanceJudge(Range, Position) || 
								(FT_Type == 0x40 && FT_Subtype == 0x02) || 
								(FT_Type == 0x33 && FT_Subtype == 0x01))
							{
									//���73�����Χ400-3000��
									if(0x42 == FT_Type && 0x01 == FT_Subtype)
									{
											//�������
											pMag->HJ73Distance = GetDistance(Position);

											//���㵯�����ʱ��
											if(Velocity3)
											{
													pMag->ButtletTime = pMag->HJ73Distance / (Velocity3 * 10) * 10; //��100msΪ��λ
											}

											//��������
											Thickness16 = Thickness1 < 20 ? Thickness1 : (Thickness1 - 18) * 10;

											if(pMag->Shell[Part] >= Thickness16 && Thickness16 != 0)
											{
													pMag->Shell[Part] -= Thickness16;
											}
											else
											{
													pMag->Shell[Part] = 0;

													if(pMag->ButtletTime != 0)
													{
															pMag->DamaCalcLater = TRUE;	//�ӳٽ����˺�
															pMag->m_byLaserID 	= Id;
															pMag->HJ73ShotCount++;
															Laser_InsertPartitionAndID(Id, (u8)Part, pMag);
													}
													else
													{
															pMag->m_bIsShoted = TRUE;
															Laser_InsertPartitionAndID(Id, (u8)Part, pMag);
													}
											}

											return;
									}
	
									//��������
									Thickness16 = Thickness1 < 20 ? Thickness1 : (Thickness1 - 18) * 10;

									if(FT_Type == 0x43 && FT_Subtype == 0x04)
									{
											pMag->Shell[Part] = 0;
											pMag->m_bIsShoted = TRUE;
											Laser_InsertPartitionAndID(Id, (u8)Part, pMag);	//[�����еĲ�λ][���иò�λ�ļ������]=�ò�λ�ļ���ID
											return;
									}

									if(pMag->Shell[Part] >= Thickness16 && Thickness16 != 0)
									{
											pMag->Shell[Part] -= Thickness16;
									}
									else
									{
											pMag->Shell[Part] = 0;
											pMag->m_bIsShoted = TRUE;
											Laser_InsertPartitionAndID(Id, (u8)Part, pMag);	//[�����еĲ�λ][���иò�λ�ļ������]=�ò�λ�ļ���ID
									}
							}
					}
			}
	}
}

void DectorCheatUploadHandle(void)
{
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byT_Type    = IDParam_GetTType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byT_Subtype = IDParam_GetTSubType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byRID       = IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_aPID[0]     = IDParam_GetPid() >>8 ;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_aPID[1]     = IDParam_GetPid() &0xff;;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byC_Idegree = GetInjuryDegree(g_sUserParam.m_byBlood);
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_sL_Position = g_sUserParam.m_sLPosition;    //λ�ú͸߳�
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_sTime       = g_sUserParam.m_sUtcTime;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byIE_Type   = IE_DT_COVER;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;
    AgaistRuleStartEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}

//���ƶ񻯣���ȡ��ѪƵ��
uint8_t GetBloodLossFrequency(void)
{
    ENUM_CI_INDIVIDUAL HurtDegree = GetInjuryDegree(g_sUserParam.m_byBlood);
    uint8_t frequency = 0;

    switch(HurtDegree)
    {
        case CI_SOLDIER_N13:
            frequency = 0;	//��Ѫ���
            break;

        case CI_SOLDIER_L12:
            frequency = 12;
            break;

        case CI_SOLDIER_ME11:
            frequency = 11;
            break;

        case CI_SOLDIER_ME10:
            frequency = 10;
            break;

        case CI_SOLDIER_S9:
            frequency = 9;
            break;

        case CI_SOLDIER_S8:
            frequency = 8;
            break;

        case CI_SOLDIER_S7:
            frequency = 7;
            break;

        case CI_SOLDIER_S6:
            frequency = 6;
            break;

        case CI_SOLDIER_C5:
            frequency = 5;
            break;

        case CI_SOLDIER_C4:
            frequency = 4;
            break;

        case CI_SOLDIER_C3:
            frequency = 3;
            break;

        case CI_SOLDIER_C2:
            frequency = 2;
            break;

        case CI_SOLDIER_C1:
            frequency = 1;
            break;

        case CI_SOLDIER_D0:
            frequency = 0;	//Ѫ��Ϊ0�����
            break;

        default:
            frequency = 0;
            break;
    }
    return frequency;
}

void Train_Mode_Revive(void)
{
		RCVHURTINFO_T Info;
		
		static uint8_t cnt = 0;

	  if(g_sUserParam.m_byMode == TRAIN && UserParam_GetBlood() == 0) 
		{
				if(cnt++ < 60)
					return;
				
				cnt = 0;
				
				Info.m_RT_Type = 0xff;
				Info.m_RT_Subtype = 0xff;
				Info.m_HType = H_Type_I_BACKSTAGECTRL;
				Info.m_HurtBlood = 100;
				
				UpdateDamage(Info);
		}
}
//ҽ�ƾȻ� 1min��ʱ��飬��δ���Ȼ����������
void MedicalAidCheck(void)
{
		RCVHURTINFO_T HurtInfo = {0};

		uint8_t frequency = 0;
    static uint8_t BloodLossCnt = 0;
    
    if(0 == g_sUserParam.m_byBlood)
        return;

    if(100 == g_sUserParam.m_byBlood)
        BloodLossCnt = 0;

    frequency = GetBloodLossFrequency();

    if(frequency)
    {
        //δ�����Σ��������
        if(g_sUserParam.m_IsAid != TRUE)
        {
            if(BloodLossCnt == frequency)
            {
                HurtInfo.m_RT_Type = 0xff;
                HurtInfo.m_RT_Subtype = 0xff;
                HurtInfo.m_HType = H_Type_I_MultipleTrauma;
                HurtInfo.m_HurtBlood = frequency;
                UpdateDamage(HurtInfo);
                BNInjuryWorseHandle();
								BloodLossCnt = 0;
            }
						BloodLossCnt++;
        }
    }
}

void NCBT_DamageCheck(void)
{
    uint8_t BloodTemp;
    RCVHURTINFO_T HurtInfo = {0};

    if(0 == g_sUserParam.m_byBlood)
        return;

    if(g_sResComTask_Mag.m_byNuclearFlg == 1 || 
				g_sResComTask_Mag.m_byBiologyFlg == 1 || 
				 g_sResComTask_Mag.m_byChemistryFlg == 1)
    {
        SetRand();
        BloodTemp = g_sResComTask_Mag.NBC_Strength;
        HurtInfo.m_RT_Type = 0xff;
        HurtInfo.m_RT_Subtype = 0xff;
        HurtInfo.m_HType = H_Type_I_NuclearChemicalBiological;
        HurtInfo.m_HurtBlood = BloodTemp;
        UpdateDamage(HurtInfo);
    }
}

void DayOrNight_Judge(void)
{
		time_t DayTime = 0, NightTime = 0, CurrentTime = 0;
		struct tm fixed_Day_date = {0}, fixed_Night_date = {0}, current_date = {0};

		if(g_sResComTask_Mag.m_ManoParamFlg == FALSE || g_sUserParam.m_byDayOrNightJudge == TRUE)
				return;

		if((g_sUserParam.m_sDayStartTime.m_byHour == 0)
		  &&(g_sUserParam.m_sDayStartTime.m_byMinute == 0) 
			&& (g_sUserParam.m_sDayStartTime.m_bySecond == 0))
				return;
		
		fixed_Day_date.tm_year = 0;
		fixed_Day_date.tm_mon = 0;
		fixed_Day_date.tm_mday = 0;
		fixed_Day_date.tm_hour = g_sUserParam.m_sDayStartTime.m_byHour;
		fixed_Day_date.tm_min = g_sUserParam.m_sDayStartTime.m_byMinute;
		fixed_Day_date.tm_sec = g_sUserParam.m_sDayStartTime.m_bySecond;
		
		DayTime = mktime(&fixed_Day_date);
		
		fixed_Night_date.tm_year = 0;
		fixed_Night_date.tm_mon = 0;
		fixed_Night_date.tm_mday = 0;
		fixed_Night_date.tm_hour = g_sUserParam.m_sNightStartTime.m_byHour;
		fixed_Night_date.tm_min = g_sUserParam.m_sNightStartTime.m_byMinute;
		fixed_Night_date.tm_sec = g_sUserParam.m_sNightStartTime.m_bySecond;
		
		NightTime = mktime(&fixed_Night_date);
		
		current_date.tm_year = 0;
		current_date.tm_mon = 0;
		current_date.tm_mday = 0;
		current_date.tm_hour = g_sUserParam.m_sUtcTime.m_byHour;
		current_date.tm_min = g_sUserParam.m_sUtcTime.m_byMinute;
		current_date.tm_sec = g_sUserParam.m_sUtcTime.m_bySecond;
		
		CurrentTime = mktime(&current_date);
		
		if((CurrentTime >= DayTime) && (CurrentTime < NightTime))
		{
				DayModeHandle(FALSE);
		}
		
		if(CurrentTime >= NightTime)
		{
				NightModeHandle(FALSE);
		}

		return;
}

void Mano_Start_Judge(void)
{
		time_t ManoTime = 0, CurrentTime = 0;
		struct tm fixed_date = {0}, current_date = {0};

		if(g_sResComTask_Mag.m_ManoParamFlg == FALSE || g_sUserParam.m_byJudge == TRUE)
				return;

		if((g_sUserParam.m_sUtcTime.m_byYear == 0) && (g_sUserParam.m_sUtcTime.m_byMonth == 0)
			&&(g_sUserParam.m_sUtcTime.m_byDate == 0) && (g_sUserParam.m_sUtcTime.m_byHour == 0)
		  &&(g_sUserParam.m_sUtcTime.m_byMinute == 0) && (g_sUserParam.m_sUtcTime.m_bySecond == 0))
				return;
		
		fixed_date.tm_year = g_sUserParam.m_sManoStart.byYear - 1900;
		fixed_date.tm_mon = g_sUserParam.m_sManoStart.byMonth - 1;
		fixed_date.tm_mday = g_sUserParam.m_sManoStart.byDay;
		fixed_date.tm_hour = g_sUserParam.m_sManoStart.byHour;
		fixed_date.tm_min = g_sUserParam.m_sManoStart.byMin;
		fixed_date.tm_sec = g_sUserParam.m_sManoStart.bySecond;
		
		ManoTime = mktime(&fixed_date);
		
		current_date.tm_year = g_sUserParam.m_sUtcTime.m_byYear - 1900;
		current_date.tm_mon = g_sUserParam.m_sUtcTime.m_byMonth - 1;
		current_date.tm_mday = g_sUserParam.m_sUtcTime.m_byDate;
		current_date.tm_hour = g_sUserParam.m_sUtcTime.m_byHour;
		current_date.tm_min = g_sUserParam.m_sUtcTime.m_byMinute;
		current_date.tm_sec = g_sUserParam.m_sUtcTime.m_bySecond;
		
		CurrentTime = mktime(&current_date);
		
		if(CurrentTime < ManoTime)
		{
			g_sUserParam.m_sTsChar1.m_bUnitmode = FALSE;
			g_sUserParam.m_sTsChar2.m_bUnitmode = FALSE;
			g_sUserParam.m_byMode = TRAIN;
		}
		else 
		{
			g_sUserParam.m_sTsChar1.m_bUnitmode = TRUE;
			g_sUserParam.m_sTsChar2.m_bUnitmode = TRUE;
			g_sUserParam.m_byMode = GAME_GOING;
		}
		return;
}

#if 1

/*
 ************************************************************************************************************************************************************************
 *��������: BatteryReplacementProcess
 *��������: ��������жϺʹ���,���ñ���
 *��������: �ⲿ��ص�ѹ
 *�������: change bat flag
 *��������: TRUE�ύ�ɹ�/FALSE�ύʧ��
 *��������: XFW
 *ȫ������: ��
 *����˵��:
 ************************************************************************************************************************************************************************
 */
uint32_t BatteryReplacementTask(float ExtBatVol, u8 *pEnableChangeBat)
{
    static uint8_t LowFlag, VeryLowFlag, LowCnt;

    if(ExtBatVol < BAT_LOW)
    {
        LowFlag = 1;

        if(ExtBatVol < BAT_VERY_LOW)
        {
            VeryLowFlag = 1;
            LowCnt = 0;
        }
        else if(ExtBatVol > (BAT_VERY_LOW + BAT_OFFSET))
        {
            VeryLowFlag = 0;		//��ѹ�ָ�
        }
    }
    else if(ExtBatVol > (BAT_LOW + BAT_OFFSET))
    {
        LowFlag = 0;		//��ѹ�ָ�
        VeryLowFlag = 0;
        LowCnt = 0;
    }

    if((LowFlag == 0) && (VeryLowFlag == 0)) //��ص�ѹ����
    {
        if(pEnableChangeBat != NULL)
        {
            *pEnableChangeBat = FALSE;
        }

        Led_SetRunMode(LED_MODE_BLINK_ALWAYS_OFF, &g_sStaledMag);

        g_sUserParam.m_sTsChar1.m_bBatteryState = FALSE;
    }
    else if((LowFlag == 1) && (VeryLowFlag == 0)) //��ص�ѹ����������ص�ѹ
    {
        //�͵���ʱ��Ʒ�ת
        Led_SetRunMode(LED_MODE_BLINK_1, &g_sStaledMag);

        if(pEnableChangeBat != NULL)
        {
            *pEnableChangeBat = TRUE;
        }
				g_sUserParam.m_sTsChar1.m_bBatteryState = TRUE;
    }
    else  //��ص�ѹ�����ػ���ѹ,���ػ����رյ�̨��̽ͷ��Դ
    {
        if(pEnableChangeBat != NULL)
        {
            *pEnableChangeBat = TRUE;
        }
				ResComTask_HandleGet()->m_pLLMag->m_sHwOps.gpio_power_off();
        g_sUserParam.m_sTsChar1.m_bBatteryState = TRUE;
    }

    return FALSE;
}

#endif

//���ط���
float DistanceCalcu()
{
    double NowLon = 0;
    double NowLat = 0;
    static double LastLon = 0;
    static double LastLat = 0;
    double distance = 0;


    //���ؾ���
		NowLon = g_sResComTask_Mag.m_sNmeaMag.m_byJD_Du + (g_sResComTask_Mag.m_sNmeaMag.m_byJD_Fen + g_sResComTask_Mag.m_sNmeaMag.m_dwJD_Fen_XiaoShu / 100000.0) / 60.0; //��������徭��
						 /*
						 g_sUserParam.m_sLPosition.m_byJD_DuHundred * 100 + 
						 g_sUserParam.m_sLPosition.m_byJD_DuTen * 10 + 
						 g_sUserParam.m_sLPosition.m_byJD_DuUnit + 
						 (g_sUserParam.m_sLPosition.m_byJD_FenTen * 10 + 
						 g_sUserParam.m_sLPosition.m_byJD_FenUnit + 
						 (g_sUserParam.m_sLPosition.m_byJD_FenTenth / 10000 + 
						 g_sUserParam.m_sLPosition.m_byJD_FenPercent / 1000+ 
						 g_sUserParam.m_sLPosition.m_byJD_FenThousandth / 100 + 
						 g_sUserParam.m_sLPosition.m_byJD_FenTenThousandth / 10) / 100000.0	 
						 ) / 60.0
						 ;
						 */
	//����γ��	
	NowLat = g_sResComTask_Mag.m_sNmeaMag.m_byWD_Du + (g_sResComTask_Mag.m_sNmeaMag.m_byWD_Fen + g_sResComTask_Mag.m_sNmeaMag.m_dwWD_Fen_XiaoShu / 100000.0) / 60.0; //����������γ��
						/*
						g_sUserParam.m_sLPosition.m_byWD_DuTen * 10 + 
						 g_sUserParam.m_sLPosition.m_byWD_DuUnit + 
						 (g_sUserParam.m_sLPosition.m_byWD_FenTen * 10 + 
						 g_sUserParam.m_sLPosition.m_byWD_FenUnit + 
						 (g_sUserParam.m_sLPosition.m_byWD_FenTenth / 10000 + 
						 g_sUserParam.m_sLPosition.m_byWD_FenPercent / 1000+ 
						 g_sUserParam.m_sLPosition.m_byWD_FenThousandth / 100 + 
						 g_sUserParam.m_sLPosition.m_byWD_FenTenThousandth / 10) / 100000.0	 
						 ) / 60.0
						 ;
						 */

    if(LastLon == 0 && LastLat == 0)
    {
        LastLon 	= NowLon;
        LastLat 	= NowLat;
        distance 	= 0;
    }
    else
    {
        //�������
        distance 	= get_distance(NowLat, NowLon, LastLat, LastLon);
        LastLon 	= NowLon;
        LastLat 	= NowLat;
    }

    return distance;
}

POSE_E PoseAnalyse(MPU6050_MAG *pMag)
{
		#define ZOU_L  1.0  //2.0	//1.0 //3.0
		#define ZOU_H	 2.0 //2.5	//10.0

		#define WO_H 	 3.0  //0.2

    //Ҫʶ�����̬��վ���ߣ��ܣ��ԣ�����
    if(pMag->m_fRoll > -130.0 && pMag->m_fRoll < -60.0)
    {

        if((pMag->m_fGx > -0.6 && pMag->m_fGx < 0.6) && pMag->m_fGy < -0.5)
        {
						return POSE_ZHAN;
        }
				else if(pMag->m_fGy > -6.0)
				{
						//Beep_On(&g_sBeepMag, OPT_MODE_NONBLOCK, 400);
						return POSE_PAO;
				}
				else
						//Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 1);
						return POSE_ZOU;
					
				/*
        //������վ���ߡ���
        if((DistanceCalcu() / 10.0) <= ZOU_L)
        {
            return POSE_ZHAN;
        }
        else if(ZOU_L < (DistanceCalcu() / 10.0) <= ZOU_H)
        {
            return POSE_ZOU;
        }
        else if((DistanceCalcu() / 10.0) > ZOU_H)
        {
            return POSE_PAO;
        }
				*/
    }
    else if(pMag->m_fRoll < -150.0 || pMag->m_fRoll > 150.0)
    {

        if(pMag->m_fGz < -11.0 || pMag->m_fGz > -8.0)
        {
            return POSE_WO;
        }
				else
				{
						return POSE_PUFU;
					/*
					if(DistanceCalcu() > 3.0)
						return POSE_PUFU;
					else
						return POSE_WO;
					*/
				}
    }
}

void SetLPosition(NMEA_MAG *pMag)
{ 
		g_sUserParam.m_sLPosition.m_byLposState = pMag->m_byJD_Du == 0?0:1;
    g_sUserParam.m_sLPosition.m_byJD_DuHundred = pMag->m_byJD_Du/100%10;		//���Ȱ�λ
    g_sUserParam.m_sLPosition.m_byJD_DuTen = pMag->m_byJD_Du/10%10;		//����ʮλ
    g_sUserParam.m_sLPosition.m_byJD_DuUnit = pMag->m_byJD_Du%10;		//���ȸ�λ
    g_sUserParam.m_sLPosition.m_byJD_FenTen = pMag->m_byJD_Fen/10%10;		//���ȷֵ�ʮλ
    g_sUserParam.m_sLPosition.m_byJD_FenUnit = pMag->m_byJD_Fen%10;		//���ȷֵĸ�λ
    g_sUserParam.m_sLPosition.m_byJD_FenTenth = pMag->m_dwJD_Fen_XiaoShu/10000%10;		//���ȷֵ�ʮ��λ
    g_sUserParam.m_sLPosition.m_byJD_FenPercent = pMag->m_dwJD_Fen_XiaoShu/1000%10;		//���ȷֵİٷ�λ
    g_sUserParam.m_sLPosition.m_byJD_FenThousandth = pMag->m_dwJD_Fen_XiaoShu/100%10;		//���ȷֵ�ǧ��λ
    g_sUserParam.m_sLPosition.m_byJD_FenTenThousandth = pMag->m_dwJD_Fen_XiaoShu/10%10;		//���ȷֵ����λ
    g_sUserParam.m_sLPosition.m_byWD_DuTen = pMag->m_byWD_Du/10%10;		//γ��ʮλ
    g_sUserParam.m_sLPosition.m_byWD_DuUnit = pMag->m_byWD_Du%10;		//γ�ȸ�λ
    g_sUserParam.m_sLPosition.m_byWD_FenTen = pMag->m_byWD_Fen/10%10;		//γ�ȷֵ�ʮλ
    g_sUserParam.m_sLPosition.m_byWD_FenUnit = pMag->m_byWD_Fen%10;		//γ�ȷֵĸ�λ
    g_sUserParam.m_sLPosition.m_byWD_FenTenth = pMag->m_dwWD_Fen_XiaoShu/10000%10;		//γ�ȷֵ�ʮ��λ
    g_sUserParam.m_sLPosition.m_byWD_FenPercent = pMag->m_dwWD_Fen_XiaoShu/1000%10;		//γ�ȷֵİٷ�λ
    g_sUserParam.m_sLPosition.m_byWD_FenThousandth = pMag->m_dwWD_Fen_XiaoShu/100%10;		//γ�ȷֵ�ǧ��λ
    g_sUserParam.m_sLPosition.m_byWD_FenTenThousandth = pMag->m_dwWD_Fen_XiaoShu/10%10;		//γ�ȷֵ����λ
	
    g_sUserParam.m_sLPosition.m_byLenthSign     = pMag->m_dwAltitude>0.0?0:1;
    g_sUserParam.m_sLPosition.m_byLenthThousand = pMag->m_dwAltitude/10000%10;		//�߶�ǧλ
    g_sUserParam.m_sLPosition.m_byLenthHundred  = pMag->m_dwAltitude/1000%10;		//�߶Ȱ�λ
    g_sUserParam.m_sLPosition.m_byLenthTen      = pMag->m_dwAltitude/100%10;		//�߶�ʮλ
    g_sUserParam.m_sLPosition.m_byLenthUnit     = pMag->m_dwAltitude/10%10;		//�߶ȸ�λ
    g_sUserParam.m_sLPosition.m_byLenthTenth    = pMag->m_dwAltitude%10;		//�߶�ʮ��λ 
}

/******************************************************************
*��������:NMEA_GNRMC_Analysis
*��������:����GNRMC��Ϣ
*�������:gpsx:nmea��Ϣ�ṹ�壬buf:���յ���GPS���ݻ������׵�ַ
*�������:
*����ֵ:                  
*����˵��:
*�޸�����    �汾��   �޸���    �޸�����
*---------------------------------------------------
*                              ���

GPRMC

�Ƽ���С��������GPS��Ϣ��RecommendedMinimumSpecificGPS/TRANSITData��

������

$GPRMC,161229.487,A,3723.2475,N,12158.3416,W,0.13,309.62,120598,,,A*10

��ʽ��

$GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>*<13>

������

$--RMC��ϢID��$GPRMC��RMCЭ��ͷ��

<1>��λ���UTCʱ�䣬161229.487����ʽ��hhmmss.sss

<2>��λ״̬��A��A=��λ��V=����

<3>γ�ȣ�3723.2475����ʽ��ddmm.mmmm

<4>γ�ȷ���N��N=��γ��S=��γ

<5>���ȣ�12158.3416����ʽ��dddmm.mmmm

<6>���ȷ���W��W=������E=����

<7>�Եغ��٣�0.13��Knots����Χ��000.0--999.9��1Knot���ڣ�=1852��/ʱ

<8>�Եغ���309.62���ȣ���Χ��000.0--359.9�����汱Ϊ�ο���׼��

<9>��λ���UTC���ڣ�120598����ʽ��ddmmyy�������꣩

<10>��ƫ�ǣ���000.0--180.0�ȣ�ǰ���0Ҳ�������䣩

<11>��ƫ�Ƿ���E��������W������

<12>ģʽָʾ��A����NMEA-0183 3.0�汾���������A=������λ��D=��֣�E=���㣬N=��Ч���ݣ�

<13>У��ͣ�*10

�س����н���Ϣ������
******************************************************************/		
void NMEA_GNRMC_Analysis(NMEA_MAG *pMag, uint8_t *buf, uint16_t len)
{		 
	uint32_t temp;	 
	uint8_t StrLen[15], StrTemp[15][12]={0}; 
	uint8_t i, j, StrIdx = 0;

	if(0 == strncmp((char *)buf, "$GNRMC", 6))
	{
		//����","�ָ��ַ������ַ������ȴ���StrLen[]��
		for(i = 0, j = 0; i < len; i++)
		{
			if(buf[i] == ',')
			{
				StrLen[StrIdx++] = j;
				j = 0;
			}
			else
			{
				StrTemp[StrIdx][j++] = buf[i];
			}	
		}
		StrLen[StrIdx++] = j;
		
		StrIdx = 1;
		if(StrLen[StrIdx] >= 10)
		{
			sscanf((char *)StrTemp[StrIdx], "%d", &temp);

			pMag->m_sUtcTime.m_byHour = temp / 10000 + 8;
			pMag->m_sUtcTime.m_byMinute = temp % 10000 / 100;
			pMag->m_sUtcTime.m_bySecond = temp % 100;
		}
		
		StrIdx = 3;
		if(StrLen[StrIdx] >= 9)
		{
			pMag->m_byWD_Du = (StrTemp[StrIdx][0]-'0')*10+(StrTemp[StrIdx][1]-'0');
			pMag->m_byWD_Fen = (StrTemp[StrIdx][2]-'0')*10+(StrTemp[StrIdx][3]-'0');
			pMag->m_dwWD_Fen_XiaoShu = (StrTemp[StrIdx][5]-'0')*10000;
			pMag->m_dwWD_Fen_XiaoShu += (StrTemp[StrIdx][6]-'0')*1000;
			pMag->m_dwWD_Fen_XiaoShu += (StrTemp[StrIdx][7]-'0')*100;
			pMag->m_dwWD_Fen_XiaoShu += (StrTemp[StrIdx][8]-'0')*10;
			
			//pMag->m_dwWD_Fen_XiaoShu *= 10;
			if(StrLen[StrIdx] >= 10)
			{
				pMag->m_dwWD_Fen_XiaoShu += (StrTemp[StrIdx][9]-'0');
			}
		}
		
		StrIdx = 4;
		if(StrLen[StrIdx] >= 1)
		{
			pMag->m_byNorS = StrTemp[StrIdx][0];
		}
		
		StrIdx = 5;
		if(StrLen[StrIdx] >= 10)
		{
			pMag->m_byJD_Du = (StrTemp[StrIdx][0] - '0') * 100;
			pMag->m_byJD_Du += (StrTemp[StrIdx][1] - '0') * 10;
			pMag->m_byJD_Du += (StrTemp[StrIdx][2] - '0');
			
			pMag->m_byJD_Fen = (StrTemp[StrIdx][3] - '0') * 10 + (StrTemp[StrIdx][4] - '0');
			
			pMag->m_dwJD_Fen_XiaoShu = (StrTemp[StrIdx][6]-'0')*10000;
			pMag->m_dwJD_Fen_XiaoShu += (StrTemp[StrIdx][7]-'0')*1000;
			pMag->m_dwJD_Fen_XiaoShu += (StrTemp[StrIdx][8]-'0')*100;
			pMag->m_dwJD_Fen_XiaoShu += (StrTemp[StrIdx][9]-'0')*10;
			
			//pMag->m_dwJD_Fen_XiaoShu *= 10;
			if(StrLen[StrIdx] >= 11)
			{
				pMag->m_dwJD_Fen_XiaoShu += (StrTemp[StrIdx][10]-'0');
			}
		}
		
		if(pMag->m_byWD_Du == 0 && pMag->m_byJD_Du == 0)
		{
			g_sUserParam.m_sLPosition.m_byLposState = FALSE;
			g_sUserParam.m_sTsChar1.m_bFailureState = TRUE;
		}
		else
		{
			g_sUserParam.m_sLPosition.m_byLposState = TRUE;
			g_sUserParam.m_sTsChar1.m_bFailureState = FALSE;
		}
		
		StrIdx = 6;
		if(StrLen[StrIdx] >= 1)
		{
			pMag->m_byEorW = StrTemp[StrIdx][0];
		}
		
		StrIdx = 9;
		if(StrLen[StrIdx] >= 6)
		{
			sscanf((char *)StrTemp[StrIdx], "%d", &temp);
			
			pMag->m_sUtcTime.m_byDate = temp / 10000;
			pMag->m_sUtcTime.m_byMonth = temp % 10000 / 100;
			pMag->m_sUtcTime.m_byYear = temp % 100;
		}

		memcpy(&g_sUserParam.m_sUtcTime, &pMag->m_sUtcTime, sizeof(STRUCT_TIME1));
		SetLPosition(pMag);
		
		StrIdx = 12;
		
		if(StrLen[StrIdx] > 0)
		{
			pMag->RTKMode = StrTemp[StrIdx][0] == 'R' ? TRUE: FALSE;
		}
		if(StrTemp[StrIdx][0] == 'A')
		{
				pMag->m_byPositionType = 0x01;
		}
		else if(StrTemp[StrIdx][0] == 'D')
		{
				pMag->m_byPositionType = 0x02;
		}
		else if(StrTemp[StrIdx][0] == 'F')
		{
				pMag->m_byPositionType = 0x05;
		}
		else if(StrTemp[StrIdx][0] == 'N')
		{
				pMag->m_byPositionType = 0x00;
		}
		else if(StrTemp[StrIdx][0] == 'R' )
		{
				pMag->m_byPositionType = 0x04;
		}
	}
}

//����������ַ������Ƿ�����NMEA0183Э�飬�����㣬����Э���ַ������ȣ�֧���ж��ַ������ضϵ����
uint32_t GPS_FindNMEA0183String(uint8_t *pBuf, uint16_t *pLen, uint32_t *pIsCutted)
{
	uint8_t *p2 = NULL;
	uint8_t i, chk; 

	if((pBuf != NULL) && (pLen != NULL))
	{
		if(pBuf[0] == '$')
		{
			if(*pLen < 4)
			{
				if(pIsCutted != NULL)
				{
					*pIsCutted = TRUE;
				}
				return	TRUE;
			}
			else if(*pLen >= 0x800)
			{
				*pLen = 0;
				*pIsCutted = FALSE;
				return FALSE;
			}
			p2 = (uint8_t*)strstr((const char *)pBuf, "*");
			if(p2 == NULL)
			{
				if(pIsCutted != NULL)
				{
					*pIsCutted = TRUE;
				}
				return	TRUE;
			}
			for(i = 0, chk = 0;i < (p2 - pBuf - 1); i++)
			{
				chk ^= pBuf[i+1];
			}
			if(chk == strtoul((const char *)p2+1, NULL, 16))
			{
				*pLen = p2 - pBuf + 3;
				if(pIsCutted != NULL)
				{
					*pIsCutted = FALSE;
				}
				return TRUE;
			}		
		}
	}
	if (pIsCutted != NULL)
	{
		*pIsCutted	= FALSE;
	}

	return FALSE;
}

void BackStageFactoryDebug(uint8_t *pRecvBuf, uint16_t RecvLen)
{

    uint8_t Cmd = pRecvBuf[3];

    switch(Cmd)
    {
        case Cmd_GetRidPid:
            //Factory_GetRidPid(pMag, pRecvBuf, RecvLen);
            break;

        case Cmd_SetRidPid:
            //Factory_SetRidPid(pMag, pRecvBuf, RecvLen);
            break;

        case Cmd_GetBleSlaverMac:
            //Factory_GetBleSlaverMac(pMag, pRecvBuf, RecvLen);
            break;

        case Cmd_SetBleSlaverMac:
            //Factory_SetBleSlaverMac(pMag, pRecvBuf, RecvLen);
            break;

        case Cmd_SaveBleSlaverMac:
            //Factory_SaveBleSlaverMac(pMag, pRecvBuf, RecvLen);
            break;

        case Cmd_GetLog:
            //Factory_GetLog(pMag, pRecvBuf, RecvLen);
            break;

        case Cmd_GetVersion:
            //Factory_GetVersion(pMag, pRecvBuf, RecvLen);
            break;

        case Cmd_GetBackstageMag:
            //Factory_GetBackstageMag(pMag, pRecvBuf, RecvLen);
            break;

        case Cmd_ZigbeeJoinedNewNet:
            //Factory_ZigbeeJoinedNewNet(pMag, pRecvBuf, RecvLen);
            break;

        case Cmd_4GServerConfig:
            //Factory_4GServerConfig(pMag, pRecvBuf, RecvLen);
            break;

        case Cmd_GetHardwareStatus:
            //Factory_HardwareStatus(pMag, pRecvBuf, RecvLen);
            break;

        case Cmd_CtrlPowerOff:
						HardCtrl_Slaver_PowerOff();
						g_sPowerOffMag.m_bPoweroff_Force = TRUE;
            //BackStage_CtrlPowerOffHandle(pMag, pRecvBuf, RecvLen);
            break;

        case Cmd_CtrlFSJ:
            //BackStage_CtrlFSJ(pMag, pRecvBuf, RecvLen);
            break;

        case Cmd_ProgramVersion: //��ȡ�汾�ţ��£�
            //BSN_SendProgramVersion(g_sUserPara.m_byT_Type, g_sUserPara.m_byT_Subtype, MAIN_VERSION, SUB_VERSION, FIX_VERSION, pRecvBuf);
            break;

        case Cmd_TransmitProgram:
            //BackStageTransmitProgress(pRecvBuf);
            break;

        case Cmd_HardWareTest:
            //TestCom_Analysis(pMag, pRecvBuf, RecvLen);
            break;

        default:
            break;
    }
}

#endif

