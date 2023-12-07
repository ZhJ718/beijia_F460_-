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

//调试口
void DebugComInit(void)
{
    UART_CONFIG Config = {0};
    UART_MANAGER *pUartMag = &g_sUartSetMag.Com[DEBUG_COM];

    memcpy((void *)&Config, (void *)&g_scUartDefConfig[DEBUG_COM], sizeof(UART_CONFIG));
    //对应串口配置
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

    //等待按键松开，关机
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

uint8_t BackStage_dataBuf[256] = {0};		//BackStage_dataBuf用于存放事件数据
#define BULLETUNIID_MAX (255)
#define WEAPONUNIID_MAX (255)

INJURY_MAG g_sInjuryMag;

const WEAPONUNID_T WeaponTypeUniID[WEAPONUNIID_MAX] =
{

    //坦克（非火控模拟）
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

    //坦克（嵌入式模拟）
    {0x02, 0x01, 0},

    //步战车
    {0x03, 0x01, 0},
    {0x03, 0x02, 0},
    {0x03, 0x03, 0},
    {0x03, 0x04, 0},
    {0x03, 0x05, 0},
    {0x03, 0x06, 0},

    //装甲输送车
    {0x04, 0x01, 0},
    {0x04, 0x02, 0},

    //突击车
    {0x05, 0x01, 0},
    {0x05, 0x02, 0},
    {0x05, 0x03, 0},
    {0x05, 0x04, 0},

    //突击炮
    {0x06, 0x01, 0},
    {0x06, 0x02, 0},

    //反坦克导弹系统
    {0x07, 0x01, 1},
    {0x07, 0x02, 1},

    //牵引火炮
    {0x10, 0x01, 0},
    {0x10, 0x02, 0},
    {0x10, 0x03, 0},

    //自行火炮（非数字化）
    {0x11, 0x01, 0},
    {0x11, 0x02, 0},

    //自行火炮（数字化）
    {0x12, 0x01, 0},
    {0x12, 0x02, 0},
    {0x12, 0x03, 0},
    {0x12, 0x04, 0},
    {0x12, 0x05, 0},

    //火箭炮
    {0x13, 0x01, 1},
    {0x13, 0x02, 1},
    {0x13, 0x03, 1},

    //迫击炮
    {0x14, 0x01, 40},
    {0x14, 0x02, 40},
    {0x14, 0x03, 40},
    {0x14, 0x04, 40},
    {0x14, 0x05, 40},

    //炮兵侦察
    {0x15, 0x01, 0},
    {0x15, 0x02, 0},
    {0x15, 0x03, 0},
    {0x15, 0x04, 0},

    //高炮（有火控）
    {0x20, 0x01, 0},
    {0x20, 0x02, 0},
    {0x20, 0x03, 0},
    {0x20, 0x04, 0},

    //高炮（无火控）
    {0x21, 0x01, 0},
    {0x21, 0x02, 0},
    {0x21, 0x03, 0},
    {0x21, 0x04, 0},

    //防空导弹系
    {0x22, 0x01, 1},
    {0x22, 0x02, 1},
    {0x22, 0x03, 1},
    {0x22, 0x04, 1},
    {0x22, 0x05, 1},

    //防空雷达系统
    {0x23, 0x01, 0},
    {0x23, 0x02, 0},

    //防空指挥系统
    {0x24, 0x01, 0},
    {0x23, 0x02, 0},

    //单兵防空导弹
    {0x25, 0x01, 1},
    {0x25, 0x02, 1},
    {0x25, 0x03, 1},
    {0x25, 0x04, 1},
    {0x25, 0x05, 1},

    //人员
    {0x30, 0x01, 0},
    {0x30, 0x02, 0},
    {0x30, 0x03, 0},

    //自动步枪
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

    //班用机枪
    {0x32, 0x01, 1000},
    {0x32, 0x02, 1000},
    {0x32, 0x03, 1000},
    {0x32, 0x04, 1000},
    {0x32, 0x05, 1000},

    //狙击步枪
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

    //微声冲锋枪
    {0x34, 0x01, 300},
    {0x34, 0x02, 300},
    {0x34, 0x03, 300},

    //霰弹枪
    {0x35, 0x01, 0},
    {0x35, 0x02, 0},
    {0x35, 0x03, 0},

    //手枪
    {0x36, 0x01, 40},
    {0x36, 0x02, 40},

    //单兵榴弹发射器
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

    //重机枪
    {0x38, 0x01, 0},
    {0x38, 0x02, 0},
    {0x38, 0x03, 0},
    {0x38, 0x04, 0},

    //高射机枪
    {0x39, 0x01, 0},
    {0x39, 0x02, 0},

    //光学侦察设备
    {0x3a, 0x01, 0},
    {0x3a, 0x02, 0},

    //单兵火箭
    {0x40, 0x01, 1},
    {0x40, 0x02, 1},
    {0x40, 0x03, 1},
    {0x40, 0x04, 1},
    {0x40, 0x05, 1},

    //单兵反坦克火箭
    {0x41, 0x01, 1},
    {0x41, 0x02, 1},
    {0x41, 0x03, 1},
    {0x41, 0x04, 1},

    //单兵反坦克导弹
    {0x42, 0x01, 0},
    {0x42, 0x02, 0},
    {0x42, 0x03, 0},
    {0x42, 0x04, 0},

    //地雷
    {0x43, 0x01, 1},
    {0x43, 0x02, 1},
    {0x43, 0x03, 1},
    {0x43, 0x04, 1},

    //单兵排雷器
    {0x44, 0x01, 0},
    {0x44, 0x02, 0},

    //手雷
    {0x45, 0x01, 0},
    {0x45, 0x02, 0},

    //简易爆炸物
    {0x46, 0x01, 0},

    //卫勤救护装备
    {0x47, 0x01, 0},

    //调理设备
    {0x48, 0x01, 0},
    {0x48, 0x02, 0},
    {0x48, 0x03, 0},

    //通用车辆
    {0x50, 0x01, 0},
    {0x50, 0x02, 0},

    //指挥车辆
    {0x51, 0x01, 0},
    {0x51, 0x02, 0},
    {0x51, 0x03, 0},
    {0x51, 0x04, 0},
    {0x51, 0x05, 0},

    //通信车辆
    {0x52, 0x01, 0},
    {0x52, 0x02, 0},

    //弹药输送车
    {0x53, 0x01, 0},

    //油料运输车
    {0x54, 0x01, 0},

    //火箭布雷车
    {0x55, 0x01, 0},

    //工程侦察车
    {0x56, 0x01, 0},
    {0x56, 0x02, 0},

    //轻型破障车
    {0x57, 0x01, 0},
    {0x57, 0x02, 0},

    //综合扫雷车
    {0x58, 0x01, 0},

    //重型机械化桥
    {0x59, 0x01, 0},

    //抢修车辆
    {0x5a, 0x01, 0},

    //工程作业车辆
    {0x5b, 0x01, 0},

    //直升机
    {0x60, 0x01, 0},
    {0x60, 0x02, 0},
    {0x60, 0x03, 0},
    {0x60, 0x04, 0},
    {0x60, 0x05, 0},
    {0x60, 0x06, 0},
    {0x60, 0x07, 0},
    {0x60, 0x08, 0},

    //无人机
    {0x61, 0x01, 0},
    {0x61, 0x02, 0},
    {0x61, 0x03, 0},
    {0x61, 0x04, 0},
    {0x61, 0x05, 0},
    {0x61, 0x06, 0},

    //战斗机/轰炸机
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

    //电子对抗
    {0x63, 0x01, 0},

    //工事设施
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

    //战场障碍
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
    {0, 0}, //占用第一个数组下标0
    {1, 1}, //5.8mm 普通子弹
    {1, 2}, //7.62mm 普通子弹
    {1, 3}, //12.7mm 普通子弹
    {1, 4},  //5.56mm 普通子弹
    {1, 5},  //9mm 普通子弹

    {2, 1}, //5.8mm 穿甲弹
    {2, 2}, //12.7mm 穿甲弹

    {3, 1}, //5.8mm 穿甲燃烧弹
    {3, 2}, //12.7mm 穿甲燃烧弹

    {4, 1}, //5.8mm 燃烧弹
    {4, 2}, //12.7mm 燃烧弹

    {5, 1}, //12.7mm 穿甲爆炸燃烧弹

    //11
    {6, 1}, //12.7mm 瞬爆弹
    {7, 1}, //霰弹
    {8, 0}, //保留
    {9, 0}, //保留
    {10, 0}, //保留
    {11, 0}, //保留
    {12, 0}, //保留
    {13, 0}, //保留
    {14, 0}, //保留
    {15, 0}, //保留

    //21
    {16, 1}, //85mm 坦克杀伤榴弹
    {16, 2}, //125mm 坦克杀伤榴弹
    {16, 3}, //120mm 坦克杀伤榴弹
    {16, 4}, //105 坦克杀伤榴弹
    {16, 5}, //100 坦克杀伤榴弹
    {16, 6}, //73 滑膛炮杀伤榴弹
    {16, 7}, //35mm 杀伤榴弹
    {16, 8}, //40mm 杀伤榴弹
    {17, 1}, //73mm 破甲杀伤弹
    {18, 0}, //燃烧弹/杀伤燃烧弹 子类型保留

    //31
    {19, 0}, //空炸杀伤弹 子类型保留
    {20, 1}, //85mm 坦克破甲弹
    {20, 2}, //125mm 坦克破甲弹
    {20, 3}, //120mm 坦克破甲弹
    {20, 4}, //105 坦克破甲弹
    {20, 5}, //100 坦克破甲弹
    {20, 6}, //120mm 破甲弹
    {20, 7}, //82mm 破甲弹
    {20, 8}, //40mm 破甲弹
    {21, 1}, //80mm 攻坚弹
    //41
    {22, 1}, //93mm 云爆弹
    {22, 2}, //80mm 云爆弹
    {23, 1}, //80mm 杀伤爆破燃烧弹
    {24, 1}, //73mm 杀伤爆破榴弹
    {25, 0}, //破甲燃烧弹  子类型保留
    {26, 1}, //85mm 坦克穿甲弹
    {26, 2}, //125mm 坦克穿甲弹
    {26, 3}, //120mm 坦克穿甲弹
    {26, 4}, //105 坦克穿甲弹
    {26, 5}, //100 坦克穿甲弹

    //51
    {26, 6}, //30mm 穿甲弹
    {27, 0}, //碎甲弹   子类型保留
    {28, 1}, //120mm 多用途弹
    {29, 0}, //保留
    {30, 0}, //保留
    {31, 0}, //保留
    {48, 1}, //25mm 高炮穿甲弹
    {48, 2}, //35mm 高炮穿甲弹
    {48, 3}, //37mm 高炮穿甲弹
    {48, 4}, //57mm 高炮穿甲弹
    {48, 5}, //100mm 高炮穿甲弹
    {48, 6}, //20mm高炮穿甲弹

//61
    {48, 5}, //100mm 高炮穿甲弹
    {49, 0}, //高炮爆破榴弹  子类型保留
    {50, 0}, //高炮穿甲爆破燃烧弹   子类型保留
    {51, 0}, //高炮多束定向预制破片弹  子类型保留
    {52, 0}, //保留
    {53, 0}, //保留
    {54, 0}, //保留
    {55, 0}, //保留
    {56, 0}, //保留
    {57, 0}, //保留

//71
    {58, 0}, //保留
    {59, 0}, //保留
    {60, 0}, //保留
    {61, 0}, //保留
    {62, 0}, //保留
    {63, 0}, //保留
    {64, 1}, //红箭 73 导弹
    {64, 2}, //红箭 8 导弹
    {64, 3}, //红箭 9 导弹
    {64, 4}, //红箭 10 导弹
//81
    {64, 5}, //前卫一导弹
    {64, 6}, //125mm 坦克炮射导弹
    {64, 7},  //米兰反坦克导弹

    {65, 1}, //红缨 5 防空导弹
    {65, 2}, //红缨 6 防空导弹
    {65, 3}, //红箭12导弹
    {65, 4}, //标枪FGM-148导弹
    {65, 5}, //毒刺FIM92防空导弹
    {65, 6}, //"地狱火"反坦克导弹

    {66, 1}, //MK-82/84航弹
    {66, 2}, //250公斤航弹
    {66, 3}, //500公斤航弹
    {66, 4}, //空地火箭弹
    {66, 5}, //CBU-XX集束炸弹


    {67, 1}, //X-59M电视制导导弹
    {67, 2}, //X-29T电视（激光）制导导弹
    {67, 3}, //YJ-88空地导弹
    {67, 4}, //YJ-63A/B空地导弹
    {67, 5}, //"小牛"空地导弹

    {68, 0}, //保留
    {69, 0}, //保留
    {70, 0}, //保留
    {71, 0}, //保留
//91
    {72, 0}, //保留
    {73, 0}, //保留
    {74, 0}, //保留
    {75, 0}, //保留
    {76, 0}, //保留
    {77, 0}, //保留
    {78, 0}, //保留
    {79, 0}, //保留
    {80, 1}, //催泪弹
    {81, 0}, //保留
//101
    {82, 0}, //保留
    {83, 0}, //保留
    {84, 0}, //保留
    {85, 0}, //保留
    {86, 0}, //保留
    {87, 0}, //保留
    {88, 0}, //保留
    {89, 0}, //保留
    {90, 0}, //保留
    {91, 0}, //保留
//111
    {92, 0}, //保留
    {93, 0}, //保留
    {94, 0}, //保留
    {95, 0}, //保留
    {96, 0}, //保留
    {97, 0}, //保留
    {98, 0}, //保留
    {99, 0}, //保留
    {100, 0}, //保留
    {101, 0}, //保留
//121
    {102, 0}, //保留
    {103, 0}, //保留
    {104, 0}, //保留
    {105, 0}, //保留
    {106, 0}, //保留
    {107, 0}, //保留
    {108, 0}, //保留
    {109, 0}, //保留
    {110, 0}, //保留
    {111, 0}, //保留
//131
    {112, 1}, //82mm 杀爆榴弹
    {112, 2}, //100mm 杀爆榴弹
    {112, 3}, //105mm 杀爆榴弹
    {112, 4}, //120mm 杀爆榴弹


    {112, 5}, //122mm 杀爆榴弹
    {112, 6}, //130mm 杀爆榴弹
    {112, 7}, //152mm 杀爆榴弹
    {112, 8}, //155mm 杀爆榴弹
    {112, 9}, //203mm 杀爆榴弹
    {112, 10}, //240mm 杀爆榴弹

//141
    {112, 11}, //152mm 末制导炮弹
    {112, 12}, //60mm 杀爆榴弹
    {113, 1}, //122mm 火箭弹
    {113, 2}, //107mm 火箭弹


    {113, 3}, //雷霆 2000 火箭弹
    {113, 4}, //工蜂-6 火箭弹
    {113, 5}, //300mm 杀爆弹
    {113, 6}, //300mm 子母弹
    {113, 7}, //300mm 末敏弹
    {113, 8}, //300mm 云爆弹
    {113, 9},//Mk.118 Mod0 HEDP火箭弹

//151
    {114, 1}, //火箭布雷弹（反坦克雷）
    {114, 2}, //火箭布雷弹（防步兵雷）
    {115, 1}, //火箭爆破弹
    {116, 1}, //防单兵地雷


    {116, 2}, //防坦克地雷
    {116, 3}, //混合雷场
    {117, 0}, //保留
    {118, 1}, //沙林
    {118, 2}, //梭曼
    {118, 3}, //维埃克斯
//161
    {118, 4}, //氢氰酸
    {118, 5}, //芥子气
    {118, 6}, //芥路混合
    {118, 7}, //胶状芥子气
    {118, 8}, //胶状芥路
    {119, 1}, //烟雾弹

    {120, 0}, //测试弹
    {121, 0}, //测距
    {122, 0}, //裁决

}  ;

const uint8_t DamageTableByBullet[][9] =
{

    //0
    {0, 0, 0, 0, 0, 0, 0, 0, 0}, //占用第一个数组下标0
    //5.8mm 普通子弹
    {100, 100, 80, 60/*腹部*/, 50/*背部*/, 50, 50, 50, 50},

    //7.62mm 普通子弹
    {100, 100, 80, 60, 80, 50, 50, 50, 50},

    //12.7mm 普通子弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //5.56mm子弹
    {100, 100, 80, 60, 50, 50, 50, 50, 50},

    //9mm 普通子弹
    {100, 100, 90, 70, 90, 60, 60, 60, 60},

    //5.8mm 穿甲弹
    {100, 100, 80, 60, 80, 40, 50, 30, 30},

    //12.7mm 穿甲弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //5.8mm 穿甲燃烧弹
    {100, 100, 100, 90, 90, 50, 60, 30, 40},

    //12.7mm 穿甲燃烧弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //5.8mm 燃烧弹
    {100, 100, 100, 90, 90, 50, 60, 30, 40},

    //12.7mm 燃烧弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //12.7mm 穿甲爆炸燃烧弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

//11
    //12.7mm 瞬爆弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //霰弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
//21
    //85mm 坦克杀伤榴弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //125mm  坦克杀伤榴弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //120mm  坦克杀伤榴弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //105mm  坦克杀伤榴弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //100mm  坦克杀伤榴弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //73 滑膛炮杀伤榴弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //35mm 杀伤榴弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //40mm 杀伤榴弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //73mm 破甲杀伤弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //燃烧弹/杀伤燃烧弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
//31
    //空炸杀伤弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //85mm 坦克破甲弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //125mm 坦克破甲弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //120mm 坦克破甲弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //105mm 坦克破甲弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //100mm 坦克破甲弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //120mm 破甲弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //82mm 破甲弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //40mm 破甲弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //80mm 攻坚弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
//41
    //93mm 云爆弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //80mm 云爆弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //80mm 杀伤爆破燃烧弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //73mm 杀伤爆破榴弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //破甲燃烧弹（保留）
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //85mm 坦克穿甲弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //125mm 坦克穿甲弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //120mm 坦克穿甲弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //105 坦克穿甲弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //100 坦克穿甲弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
//51
    //30mm 穿甲弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //碎甲弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //120mm 多用途弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //保留
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},

    //25mm 高炮穿甲弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //35mm 高炮穿甲弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //37mm 高炮穿甲弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //57mm 高炮穿甲弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
//61
    //100mm 高炮穿甲弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //20mm高炮穿甲弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //高炮爆破榴弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //高炮穿甲爆破燃烧弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //高炮多束定向预制破片弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
//71
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //红箭 73 导弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //红箭   8 导弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //红箭 9 导弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //红箭 10 导弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
//81
    //前卫一导弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //125mm 坦克炮射导弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //米兰反坦克导弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //红缨 5 防空导弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //红缨 6 防空导弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //红箭12导弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //标枪FGM-148导弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //毒刺FIM92防空导弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //"地狱火"反坦克导弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //MK-82/84航弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //250公斤航弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //500公斤航弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //空地火箭弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //CBU-XX集束炸弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //X-59M电视制导导弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //X-29T电视（激光）制导导弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //YJ-88空地导弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //YJ-63A/B空地导弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //"小牛"空地导弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

//91
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //催泪弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

//101
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
//111
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
//121
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

//131
    //82mm 杀爆榴弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //100mm 杀爆榴弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //105mm 杀爆榴弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //120mm 杀爆榴弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //122mm 杀爆榴弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //130mm 杀爆榴弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //152mm 杀爆榴弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //155mm  杀爆榴弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //203mm  杀爆榴弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //240mm   杀爆榴弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
//141
    //152mm 末制导炮弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //60mm  杀爆榴弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //122mm 火箭弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //107mm 火箭弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //雷霆 2000 火箭弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //工蜂-6 火箭弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //300mm 杀爆弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //300mm 子母弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //300mm  末敏弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //300mm 云爆弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //Mk.118 Mod0 HEDP火箭弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
//151
    //火箭布雷弹（反坦克雷）
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //火箭布雷弹（防步兵雷）
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //火箭爆破弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //防单兵地雷
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //防坦克地雷
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //混合雷场
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //保留
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //沙林
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //梭曼
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
    //维埃克斯
    {100, 100, 100, 100, 100, 100, 100, 100, 100},
//161
    //氢氰酸
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //芥子气
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //芥路混合
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //胶状芥子气
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //胶状芥路
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //烟雾弹
    {100, 100, 100, 100, 100, 100, 100, 100, 100},

    //测试弹
    {0, 0, 0, 0, 0, 0, 0, 0, 0},

    //测距
    {0, 0, 0, 0, 0, 0, 0, 0, 0},

    //裁决
    {0, 0, 0, 0, 0, 0, 0, 0, 0},

    //保留--
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
    //1腹、2左胸、3左胸、4背部、5背部、6背部、7背部、8右胸、9右胸、10腹部
//  PARTITION_NONE    = 0,
//	PARTITION_HEAD    = 1	,	//	头
//	PARTITION_LCHEST  = 2	,	//  左胸
//	PARTITION_RCHEST  = 3	,	//  右胸
//	PARTITION_ABDOMEN = 4	,	//  腹部
//	PARTITION_BACK    = 5	,	//  背
//	PARTITION_LARM    = 6,	    //	左臂
//	PARTITION_RARM    = 7	,	//	右臂
//	PARTITION_LLEG    = 8	,	//	左腿
//	PARTITION_RLEG    = 9	,	//	右腿
//	PARTITION_MAX     = 10,
};

POWER_OFF_MAG 		g_sPowerOffMag;

//开关机初始化，完成开机逻辑，该函数会导致系统堵塞1s
void AppPowerOnOffInit(POWER_OFF_MAG *pMag)
{
    PwrKey_GpioInit();
    PwrEnable_GpioInit();
    pMag->m_bIsOff = FALSE;
    pMag->m_wTimer = 0;
    pMag->m_bPoweroff_Force = FALSE;
		pMag->m_bPoweroffPre = FALSE;
}

//开关机定时任务，完成关机检测 100ms定时调度
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
    //检测是否有关机动作且在训练模式
    if((pMag->m_bIsOff == TRUE) || (pMag->m_bPoweroff_Force == TRUE))
    {
				if(g_sUserParam.m_byEnableChangeBat == FALSE)
				{
					if(g_sUserParam.m_byMode == GAME_GOING && pMag->m_bPoweroff_Force == FALSE)
					{
							//检测有关机动作，且在演习模式
							Wtn6_Play(WTN6_Mode_Mano); 		//语音提示“演习模式”
							Wtn6_Play(WTN6_Death_Illegal_Stop);  	//语音提示“非法关机”
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
        Wtn6_Sound(WTN6_SystemStop);  // 语音提示“系统关机”
				Delay_Ms(500);
        g_sRunledMag.m_sHwOps.led_off();//关灯
        Motor_On(&g_sMotorMag, OPT_MODE_BLOCK, 0);
        Delay_Ms(500);
        PwrEnable_Off(); //关机

        do
        {
            Delay_Ms(100);
            PwrEnable_Off(); //关机
            Motor_On(&g_sMotorMag, OPT_MODE_BLOCK, 0);
            IWDG_Feed();//防止看门狗咬狗导致系统复位，关机键一直按着系统重启
        }
        while(1);
    }

}

void DrillStateParamHandle(void)
{
    static uint8_t Cnt = 0;

    if(g_sUserParam.m_byMode != TRAIN)
    {
        if(Cnt == 5) //5s定时到
        {
            Cnt = 0;
            SynchronizeState(GAME_GOING, g_sUserParam.m_byEnableChangeBat);

            //参数信息有变化，需要保存
            if(g_sResComTask_Mag.BackupArea[0].XOR != g_sResComTask_Mag.BackupArea[1].XOR)
            {
								BackupDrillState();
            }
        }
				Cnt++;
    }
}

//非法关机处理，涉及到死亡操作，10s后再判断
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
        //关机作弊事件
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
										//1分钟
										if(g_sResComTask_Mag.FumeInstallStartTime >= 3)
										{
												g_sResComTask_Mag.Vios.FumeUnInstall = TRUE;
												g_sResComTask_Mag.FumeInstallStartTime = 0;
												g_sResComTask_Mag.FumeInstallEndTime = 0;
												//作弊开始事件
												FumeCheatStartUploadHandle();
												Wtn6_Play(WTN6_EXPLOSIVE_DEVICE_NOT_INSTALL);
												g_sResComTask_Mag.FumeInstallTimeStamp = g_sUserParam.m_sUtcTime;
												g_sUserParam.m_sTsChar1.m_bFailureState = TRUE;
												g_sUserParam.m_sTsChar1.m_bLocked = TRUE;
												UserParam_SetTsChar1(g_sUserParam.m_sTsChar1);
												//锁定发射机
												InterConn_ProtDeal_Slaver_Weapon_State(TRUE);		//武器状态同步
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
												//作弊结束事件
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
								//上报开始事件
								DetectorLossEvtStartUpload();
								//记录上报开始时间
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
								//上报结束事件
								DetectorLossEvtStopUpload();
						}
				}
		}
}

//非法移动探测
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
						//本地经度
						LocalLon = g_sResComTask_Mag.m_sNmeaMag.m_byJD_Du + (g_sResComTask_Mag.m_sNmeaMag.m_byJD_Fen + g_sResComTask_Mag.m_sNmeaMag.m_dwJD_Fen_XiaoShu / 100000.0) / 60.0; //计算出整体经度
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
						//本地纬度	
						LocalLat = g_sResComTask_Mag.m_sNmeaMag.m_byWD_Du + (g_sResComTask_Mag.m_sNmeaMag.m_byWD_Fen + g_sResComTask_Mag.m_sNmeaMag.m_dwWD_Fen_XiaoShu / 100000.0) / 60.0; //计算出整体的纬度
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
						//阵亡时经度
						RemoteLon = g_sUserParam.m_sLastDeathMag.m_fJD; 

						//阵亡时纬度	
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
										//上报开始事件
										IllegalMoveStartUploadHandle();
										Wtn6_Play(WTN6_CHEAT_EVENT);
										Wtn6_Play(WTN6_ILLEGAL_MOVE);
										//记录上报开始时间
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
										//上报结束事件
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
									//上报结束事件
									IllegalMoveEndUploadHandle();
							}
					}
			}
}

//演习模式下头盔离线处理
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
										//作弊开始事件
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
								//作弊结束事件
								HelmetCheatEndUploadHandle();
						}
				}
		}
}

void IllegalEventHandle(void)
{
		if(g_sResComTask_Mag.DrillParam.DisableCheat == TRUE)
			return;
		
		FumeInstallHandle();		//发烟冒未安装作弊
		DetectorLossHandle();		//探头未连接作弊
		IllegalMoveMonitor();		//死亡后非法移动作弊
		IllegalHelmetDisconHandle();		//头盔未开机
		IllegalPowerOffHandle();	//非法关机
}

LED_MAG g_sRunledMag;

/****************************************************************
@FUNCTION：运行灯初始化
@INPUT：LED管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 运行灯硬件操作需提前定义
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
@FUNCTION：状态指示灯初始化
@INPUT：LED管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 运行灯硬件操作需提前定义
			@@ RLed_GpioInit
			@@ RLed_On
			@@ RLed_Off
闪烁指示：

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
@FUNCTION：非阻塞模式下无源蜂鸣器控制功能初始化
@INPUT：BEEP管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
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
@FUNCTION：阻塞模式下马达控制功能初始化
@INPUT：MOTOR管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
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
@FUNCTION：阻塞模式下发烟罐控制功能初始化
@INPUT：FUME管理结构体指针
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
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

    //对应串口配置
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
@FUNCTION：终端运行时参数设置默认值
@INPUT：无
@OUTPUT：g_sUserParam结构体赋值
@RETURN: 无
@AUTHOR：xfw
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
@FUNCTION：终端运行时参数初始化
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
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
				if(ParamTmp.m_byMode == GAME_GOING) //不是训练模式
				{
						memcpy((void *)&g_sUserParam,(void *)&ParamTmp,sizeof(USER_PARAM));

						//演习模式下阵亡,低电量自动关机,重启应该保持原来状态
						if(g_sUserParam.m_byEnableChangeBat != TRUE)
						{
								g_sUserParam.m_IsIllegalPowerOff = TRUE;
						}
				}
				else
				{
						//恢复数据
						memcpy((void *)&g_sUserParam, (void *)&ParamTmp, sizeof(USER_PARAM) - 4);
				}
				Wtn6_Play(WTN6_SystemStart);
				DrillStateRecovery();
		}
}

/****************************************************************
@FUNCTION：终端运行时参数保存
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 阻塞较长时间
****************************************************************/
void UserParam_Save(void)
{
    g_sUserParam.m_dwChk = GetIFlashCheckSum_U32((uint8_t *)&g_sUserParam, sizeof(USER_PARAM) - 4);
    IFlash_Write(&g_sIFlashMag, FLASH_USER_ADDR, (uint8_t *)&g_sUserParam, sizeof(USER_PARAM));
}

/****************************************************************
@FUNCTION：获取用户参数中的子设备数
@INPUT：无
@OUTPUT：无
@RETURN:  子设备数
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint16_t UserParam_GetSlaverCnt(void)
{
    return g_sUserParam.m_wSingleCnt;
}


/****************************************************************
@FUNCTION：设置用户参数中的子设备数
@INPUT：新值
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
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
@FUNCTION：获取用户参数中的子设备状态获取间隔
@INPUT：无
@OUTPUT：无
@RETURN:  子设备数
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint16_t UserParam_GetSlaverStatusInterval(void)
{
    return g_sUserParam.m_wSlaverStaGetInt;
}


/****************************************************************
@FUNCTION：设置用户参数中的子设备状态获取间隔
@INPUT：间隔
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
void UserParam_SetSlaverStatusInterval(uint16_t Val)
{
    g_sUserParam.m_wSlaverStaGetInt = Val;
}

/****************************************************************
@FUNCTION：获取用户参数中的管理结构体
@INPUT：无
@OUTPUT：无
@RETURN: 操作句柄
@AUTHOR：xfw
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
@FUNCTION：设置用户参数中的子设备管理结构体
@INPUT：子设备管理结构体，子设备序号
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
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
@FUNCTION：获取当前血量
@INPUT：无
@OUTPUT：无
@RETURN: 血量
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint8_t UserParam_GetBlood(void)
{
    return g_sUserParam.m_byBlood;
}

/****************************************************************
@FUNCTION：设置当前血量
@INPUT：新值
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
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
@FUNCTION：获取当前密钥
@INPUT：无
@OUTPUT：无
@RETURN: 密钥
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint8_t UserParam_GetKey(void)
{
    return g_sUserParam.m_byKey;
}

/****************************************************************
@FUNCTION：设置当前密钥
@INPUT：新值
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetKey(uint8_t Val)
{
    g_sUserParam.m_byKey = Val;
}

/****************************************************************
@FUNCTION：获取当前在线探头数
@INPUT：无
@OUTPUT：无
@RETURN: 在线探头数
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint8_t UserParam_GetActLaserCnt(void)
{
    return g_sUserParam.m_byActLaserCnt;
}

/****************************************************************
@FUNCTION：设置当前在线探头数
@INPUT：新值
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetActLaserCnt(uint8_t Val)
{
    g_sUserParam.m_byActLaserCnt = Val;
}



/****************************************************************
@FUNCTION：获取当前身份标识
@INPUT：无
@OUTPUT：无
@RETURN: 身份标识
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
ENUM_BRCHAR UserParam_GetBRChar(void)
{
    return g_sUserParam.m_eBRChar;
}

/****************************************************************
@FUNCTION：设置当前身份标识
@INPUT：新值
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetBRChar(ENUM_BRCHAR eVal)
{
    g_sUserParam.m_eBRChar = eVal;
		g_sUserParam.m_sTsChar1.m_bIdentity = eVal;
}

/****************************************************************
@FUNCTION：获取综合伤势
@INPUT：无
@OUTPUT：无
@RETURN: 综合伤势
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
ENUM_CI_INDIVIDUAL UserParam_GetCIdegree(void)
{
    return g_sUserParam.m_eCIdegree;
}

/****************************************************************
@FUNCTION：设置综合伤势
@INPUT：新值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetCIdegree(ENUM_CI_INDIVIDUAL eCI)
{
    g_sUserParam.m_eCIdegree = eCI;
}

/****************************************************************
@FUNCTION：获取实体状态字1
@INPUT：无
@OUTPUT：无
@RETURN: 实体状态字1
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
STRUCT_TS_Character1 UserParam_GetTsChar1(void)
{
    return g_sUserParam.m_sTsChar1;
}

/****************************************************************
@FUNCTION：设置实体状态字1
@INPUT：新值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetTsChar1(STRUCT_TS_Character1 Val)
{
    g_sUserParam.m_sTsChar1 = Val;
}

/****************************************************************
@FUNCTION：获取实体状态字2
@INPUT：无
@OUTPUT：无
@RETURN: 实体状态字2
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
STRUCT_TS_Character2 UserParam_GetTsChar2(void)
{
    return g_sUserParam.m_sTsChar2;
}

/****************************************************************
@FUNCTION：设置实体状态字2
@INPUT：新值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetTsChar2(STRUCT_TS_Character2 Val)
{
    g_sUserParam.m_sTsChar2 = Val;
}

/****************************************************************
@FUNCTION：获取无线模组Mac地址
@INPUT：无
@OUTPUT：无
@RETURN: Zigbee模组Mac地址
@AUTHOR：xfw
@SPECIAL: 
****************************************************************/
uint8_t *UserParam_GetWMac(void)
{
    return g_sUserParam.m_aWMac;
}

/****************************************************************
@FUNCTION：设置无线模组Mac地址
@INPUT： 设置值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 不影响实际MAC
****************************************************************/
void UserParam_SetWMac(uint8_t * pVal, uint16_t ValLen)
{

    memcpy((void *)g_sUserParam.m_aWMac, pVal, MIN(ValLen,8));
  
}

/****************************************************************
@FUNCTION：获取当前无线模块发射功率
@INPUT：无
@OUTPUT：无
@RETURN: 密钥
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint8_t UserParam_GetWTxPwr(void)
{
    return g_sUserParam.m_byWTxPwr;
}

/****************************************************************
@FUNCTION：设置当前无线模块发射功率
@INPUT：新值
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL: 只是值传递，不涉及硬件操作
****************************************************************/
void UserParam_SetWTxPwr(uint8_t Val)
{
    g_sUserParam.m_byWTxPwr = Val;
}

/****************************************************************
@FUNCTION：获取经纬度坐标
@INPUT：无
@OUTPUT：无
@RETURN: 经纬度坐标
@AUTHOR：xfw
@SPECIAL: 
****************************************************************/
STRUCT_LPOS *UserParam_GetLPos(void)
{
    return &g_sUserParam.m_sLPosition;
}

/****************************************************************
@FUNCTION：设置经纬度坐标
@INPUT： 设置值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
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
@FUNCTION：获取UTC时间
@INPUT：无
@OUTPUT：无
@RETURN: UTC时间
@AUTHOR：xfw
@SPECIAL: 
****************************************************************/
STRUCT_TIME1 *UserParam_GetTime1(void)
{
    return &g_sUserParam.m_sUtcTime;
}

/****************************************************************
@FUNCTION：设置UTC时间
@INPUT： 设置值
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
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
@FUNCTION：获取定位状态
@INPUT：无
@OUTPUT：无
@RETURN: 密钥
@AUTHOR：xfw
@SPECIAL: 
****************************************************************/
uint8_t UserParam_GetPosType(void)
{
    return g_sUserParam.m_byPosType;
}

/****************************************************************
@FUNCTION：设置定位状态
@INPUT：新值
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL: 
****************************************************************/
void UserParam_SetPosType(uint8_t Val)
{
    g_sUserParam.m_byPosType = Val;
}

/****************************************************************
@FUNCTION：获取卫星数量
@INPUT：无
@OUTPUT：无
@RETURN: 密钥
@AUTHOR：xfw
@SPECIAL: 
****************************************************************/
uint8_t UserParam_GetSatNum(void)
{
    return g_sUserParam.m_bySatNum;
}

/****************************************************************
@FUNCTION：设置卫星数量
@INPUT：新值
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
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
@FUNCTION：获取火工品安装标志
@INPUT：无
@OUTPUT：无
@RETURN: 密钥
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint8_t UserParam_GetFireInstallFlg(void)
{
    return g_sUserParam.m_bFireInstallFlg;
}

/****************************************************************
@FUNCTION：设置火工品安装标志
@INPUT：新值
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL: 
****************************************************************/
void UserParam_SetFireInstallFlg(uint8_t Val)
{
    g_sUserParam.m_bFireInstallFlg = Val;
}

/****************************************************************
@FUNCTION：获取分体组件实际在线探头数(针对单兵，为头盔)
@INPUT：无
@OUTPUT：无
@RETURN: 密钥
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
uint8_t UserParam_GetActLaserCnt1(void)
{
    return g_sUserParam.m_byActLaserCnt1;
}

/****************************************************************
@FUNCTION：设置分体组件实际在线探头数(针对单兵，为头盔)
@INPUT：新值
@OUTPUT：无
@RETURN:  无
@AUTHOR：xfw
@SPECIAL: g_sUserParam
****************************************************************/
void UserParam_SetActLaserCnt1(uint8_t Val)
{
    g_sUserParam.m_byActLaserCnt1 = Val;
}

//获取子设备是否都在线
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
@FUNCTION：测试模式处理
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 无
****************************************************************/
void TesModeHandle(void)
{
    #if (BOARDID == 1)
    //测试模式
    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 1);				//马达震动
    Beep_On(&g_sBeepMag, OPT_MODE_NONBLOCK, 400);				//蜂鸣器响
    Led_SetRunMode(LED_MODE_BLINK_1, &g_sRunledMag);			//灯亮
    Led_SetRunMode(LED_MODE_BLINK_2, &g_sStaledMag);			//灯亮
    LaserCom_Prot_CtrlMode(&g_sLaserComTask_Mag, LASER_MODE_TEST);		//探头带进入测试模式
    Fume_On(&g_sFumeMag, 3);									//发烟帽点亮v

    Wtn6_Play(WTN6_YX_ZDHX1);

    #elif  (BOARDID == 2)
    //测试模式
    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 1);				//马达震动
    Beep_On(&g_sBeepMag, OPT_MODE_NONBLOCK, 400);				//蜂鸣器响
    Led_SetRunMode(LED_MODE_BLINK_1, &g_sRunledMag);			//灯亮
    Led_SetRunMode(LED_MODE_BLINK_2, &g_sStaledMag);			//灯亮
    LaserCom_Prot_CtrlMode(LaserComTask_HandleGet(), LASER_MODE_TEST);		//探头带进入测试模式
    //Fume_On(&g_sFumeMag, 3);									//发烟帽点亮v

    Wtn6_Play(WTN6_YX_ZDHX1);

    #endif
}

/****************************************************************
@FUNCTION：获取板级自定义硬件状态1
@INPUT：无
@OUTPUT：无
@RETURN:  自定义硬件状态1
@AUTHOR：xfw
@SPECIAL: 无
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
@FUNCTION：获取板级自定义硬件状态2
@INPUT：无
@OUTPUT：无
@RETURN:  自定义硬件状态2
@AUTHOR：xfw
@SPECIAL: 无
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
@FUNCTION：获取板级自定义硬件状态3
@INPUT：无
@OUTPUT：无
@RETURN:  自定义硬件状态3
@AUTHOR：xfw
@SPECIAL: 无
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
@FUNCTION：获取板级自定义硬件状态4
@INPUT：无
@OUTPUT：无
@RETURN:  自定义硬件状态4
@AUTHOR：xfw
@SPECIAL: 无
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
@FUNCTION：获取电台信号强度
@INPUT：无
@OUTPUT：无
@RETURN:  信号强度（0-4）
@AUTHOR：xfw
@SPECIAL: 无
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
@FUNCTION：内部互联互通通道发送数据
@INPUT：pChanParam - 相关参数
@OUTPUT：
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 帧ID取值规则:IR38通道-功能码<<24; ZIG通道-(功能码 << 24) | 目标地址前三字节
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
            //不能是广播地址
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
                ID = 0;    //注意该值

                //内部硬件控制指令，规定其回包为原样返回，其ID为该帧和目的地址的累加和
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
        sdram_free(0, (void *)pFrame);//防止内存泄漏
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
@FUNCTION：子设备硬件控制
@INPUT：pDstAddr - 子设备地址，DstAddrLen-子设备地址，pCtrlDat - 协议数据，CtrlDatLen - 数据长度
@OUTPUT：
@RETURN: TRUE - 成功/ FALSE - 失败
@AUTHOR：xfw
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

    //数据载荷
    DatBufLen = sizeof(DatBuf);
    if(TRUE == Protocal_ProtocolMakeShort55AA(pCtrlDat,CtrlDatLen,DatBuf,&DatBufLen))
    {
        ChanParam.m_eChan = eChan;
        ChanParam.m_pDstAddr = pDstAddr;
        ChanParam.m_byDstAddrLen = DstAddrLen;
        ChanParam.m_pDatBuf = DatBuf;
        ChanParam.m_wDatBufLen = DatBufLen;
        ChanParam.m_dwRepeatCnt = 5;  //
        ChanParam.m_dwScheTime = 200;  //定时5s
       
        return InterChanSend(&ChanParam);
    }

    return FALSE;
}

/****************************************************************
@FUNCTION：子设备建链
@INPUT：pDstAddr - 子设备地址，DstAddrLen-子设备地址，pHandle - 操作句柄
@OUTPUT：
@RETURN: TRUE - 成功/ FALSE - 失败
@AUTHOR：xfw
@SPECIAL: 注意该帧除非被删除，否则一直会发送
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
		
    //数据载荷
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
    ChanParam.m_dwRepeatCnt = 0xFFFFFFFF;  //无限次重发
    ChanParam.m_dwScheTime = 500;  //定时5s
    return InterChanSend(&ChanParam);
}

/****************************************************************
@FUNCTION：子设备状态获取
@INPUT：pDstAddr - 子设备地址，DstAddrLen-子设备地址，pHandle - 操作句柄
@OUTPUT：
@RETURN: TRUE - 成功/ FALSE - 失败
@AUTHOR：xfw
@SPECIAL: 
****************************************************************/
uint32_t  InterConn_SlaverStaGet(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen)
{
    uint8_t DatBuf[50];
    uint8_t DatBufLen;
    S_CONN_CHAN_PARAM ChanParam;

    //数据载荷
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
    ChanParam.m_dwScheTime = 100;  //定时5s
   
    return InterChanSend(&ChanParam);
}

/****************************************************************
@FUNCTION：子设备状态获取
@INPUT：pDstAddr - 子设备地址，DstAddrLen-子设备地址，pHandle - 操作句柄
@OUTPUT：
@RETURN: TRUE - 成功/ FALSE - 失败
@AUTHOR：xfw
@SPECIAL: 
****************************************************************/
uint32_t  InterConn_HelmetStaGet(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen)
{
    uint8_t DatBuf[50];
    uint8_t DatBufLen;
    S_CONN_CHAN_PARAM ChanParam;
    
    //数据载荷
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
    ChanParam.m_dwScheTime = 100;  //定时5s
   
    return InterChanSend(&ChanParam);
}


uint32_t InterConn_ProtDeal_MacGet(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
    uint8_t DatBuf[50];
    uint8_t DatBufLen;
    S_CONN_CHAN_PARAM ChanParam;

    uint8_t Offset, TType, TSubtype, RID;
    uint16_t PID;
    
    //输入参数校验
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

    //是否是发给自己
    if(TRUE != IDParam_IsIDValid(TType, TSubtype, RID, PID))
    {
        //return;
    }


    //红外短距回复
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
    ChanParam.m_dwRepeatCnt = 0x1;  //只发一次
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
	
		//输入参数校验
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
    // 电池电量（扩大1000倍）
    DatBuf[DatBufLen++] = BatTask_GetExBatVol() >> 8;
    DatBuf[DatBufLen++] = BatTask_GetExBatVol() & 0xFF;
    // 自身MAC地址
    memcpy(&DatBuf[DatBufLen],UserParam_GetWMac(),8);
    DatBufLen += 8;
    
    // 电台信号强度（0-4）
    DatBuf[DatBufLen++] = GetDTRssi();
    // 配置子设备数量n(若为0，则无后面字段)
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
    ChanParam.m_dwRepeatCnt = 0x1;  //只发一次
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

    //输入参数校验
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

    //红外短距回复
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
    ChanParam.m_dwRepeatCnt = 0x1;  //只发一次
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

    //输入参数校验
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

    //是否是发给自己
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
    //红外短距回复

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
    ChanParam.m_dwRepeatCnt = 0x1;  //只发一次
    ChanParam.m_dwScheTime = 100;  
    
    return InterChanSend(&ChanParam);
}

uint32_t InterConn_ProtDeal_IR_DB_Mac(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
		S_CONN_CHAN_PARAM ChanParam;
		
		uint8_t DatBuf[50];
    uint8_t DatBufLen, Offset;
		
		//输入参数校验
    if(pBuf == NULL || BufLen < 8)
    {
        return FALSE;
    }
		//红外短距回复

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
    ChanParam.m_dwRepeatCnt = 0x1;  //只发一次
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
		
		//输入参数校验
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
		
		//红外短距回复

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
		
		//红外短距回复
		Character1.m_bLocked = g_sUserParam.m_sTsChar1.m_bLocked;
		Character1.m_bConnectionState = g_sUserParam.m_sTsChar1.m_bConnectionState;		//各部件连接状态0：正常	1：异常
    Character1.m_bFailureState    = g_sUserParam.m_sTsChar1.m_bFailureState;		//0：无故障	1：有故障
    Character1.m_bUnitmode		  	= g_sUserParam.m_sTsChar1.m_bUnitmode;		//0：训练模式	1：演习模式
    Character1.m_bBatteryState	  = g_sUserParam.m_sTsChar1.m_bBatteryState;		//0：电量正常	1：欠电
    Character1.m_bIdentity     	  = g_sUserParam.m_sTsChar1.m_bIdentity;		//身份 ENUM_BRCHAR
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
    ChanParam.m_dwRepeatCnt = 0x1;  //只发一次
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
	
		//输入参数校验
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
		g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_byCM_Characte = pBuf[8];  //救治方式
		g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_byCE_Characte = pBuf[9];  //救治结果

		//BSN_RescureEvtHandle(&g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued);  //事件上传
		
		g_sUserParam.m_byAidEndFlag2 = TRUE;
		if(g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_byCE_Characte == 1)//救治得当，回复战斗
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
    else if(g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_byCE_Characte == 2)	//救治成功，不再恶化
    {
        Wtn6_Play(WTN6_SAVE_SUCCESS);
        g_sUserParam.m_IsAid = TRUE;
    }
    else if(g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_byCE_Characte == 3)	//救治失败，伤情恶化
    {
        Wtn6_Play(WTN6_SAVE_FALI);
        g_sUserParam.m_IsAid = FALSE;
    }
    else if(g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_byCE_Characte == 4)	//救治失败，死亡
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
    ChanParam.m_dwRepeatCnt = 0x1;  //只发一次
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
	
		//输入参数校验
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
		Character1.m_bConnectionState = g_sUserParam.m_sTsChar1.m_bConnectionState;		//各部件连接状态0：正常	1：异常
    Character1.m_bFailureState    = g_sUserParam.m_sTsChar1.m_bFailureState;		//0：无故障	1：有故障
    Character1.m_bUnitmode		  	= g_sUserParam.m_sTsChar1.m_bUnitmode;		//0：训练模式	1：演习模式
    Character1.m_bBatteryState	  = g_sUserParam.m_sTsChar1.m_bBatteryState;		//0：电量正常	1：欠电
    Character1.m_bIdentity     	  = g_sUserParam.m_sTsChar1.m_bIdentity;		//身份 ENUM_BRCHAR
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
    ChanParam.m_dwRepeatCnt = 0x1;  //只发一次
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
	
		//输入参数校验
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
	
		//红外短距回复
		Character1.m_bLocked = g_sUserParam.m_sTsChar1.m_bLocked;
		Character1.m_bConnectionState = g_sUserParam.m_sTsChar1.m_bConnectionState;		//各部件连接状态0：正常	1：异常
    Character1.m_bFailureState    = g_sUserParam.m_sTsChar1.m_bFailureState;		//0：无故障	1：有故障
    Character1.m_bUnitmode		  	= g_sUserParam.m_sTsChar1.m_bUnitmode;		//0：训练模式	1：演习模式
    Character1.m_bBatteryState	  = g_sUserParam.m_sTsChar1.m_bBatteryState;		//0：电量正常	1：欠电
    Character1.m_bIdentity     	  = g_sUserParam.m_sTsChar1.m_bIdentity;		//身份 ENUM_BRCHAR
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
    ChanParam.m_dwRepeatCnt = 0x1;  //只发一次
    ChanParam.m_dwScheTime = 100;  
    
    return InterChanSend(&ChanParam);
}

//单兵演习参数设置指令
void InterConn_ProtDeal_ManoParam_Set(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
    uint8_t i, len;
    uint8_t offset;
		uint32_t LastTimeMs, NowTimeMs;
    Struct_CtrlManoParam_IndividualSoldierDownload ManoParam;
		
    memset(&ManoParam, 0, sizeof(Struct_CtrlManoParam_IndividualSoldierDownload));

		//输入参数校验
    if(pBuf == NULL || BufLen < 34)
    {
        return;
    }
		
		offset = 3;
    //协议数据接收
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

    //防止数组越界，目前最多三个武器
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

		//RID和PID校验
    if(CheckRID(ManoParam.m_byRID) && CheckPID((ManoParam.m_aPID[0] << 8 | ManoParam.m_aPID[1])))
    {
        //避免多基站应用场景下连续收到相同指令
				LastTimeMs = g_sResComTask_Mag.m_sCmdTime.m_ulCtrlManoeuvre;
        NowTimeMs = GetSysRunMilliSecond();

        if((NowTimeMs < LastTimeMs) || ((NowTimeMs - LastTimeMs) < 1000))
        {
            return;
        }

        g_sResComTask_Mag.m_sCmdTime.m_ulCtrlManoeuvre = NowTimeMs;

        //本次接收到报文的utc时间
				RecordRcvBackStageProtocolTime(&g_sResComTask_Mag.m_sCmdTime.m_BSNulCtrlManoeuvre);
        
        //数据处理
        if(TRUE == BNSoldierManoParamSetHandle(ManoParam))
        {
						Wtn6_Play(WTN6_MANO_PARAM_CFG_SUCCESS);
            //事件上传
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

    //输入参数校验
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

		//红外短距回复
	
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
    ChanParam.m_dwRepeatCnt = 0x1;  //只发一次
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

    //输入参数校验
    if(pBuf == NULL || BufLen < 11 )
    {
        return FALSE;
    }

    Offset = 3;
		
    Supplement.m_byChangeAmmuNum = pBuf[Offset++];
    Supplement.m_byChangeSuppleNum = pBuf[Offset++];

		for(i = 0; i < Supplement.m_byChangeAmmuNum; i++) //变化的弹药种类数
    {
        Supplement.m_sChangeAmmu[i].m_byA_Type 			= pBuf[Offset++];
        Supplement.m_sChangeAmmu[i].m_byA_Subtype 		= pBuf[Offset++];
        Supplement.m_sChangeAmmu[i].m_aChangeAmmuNum[0] = pBuf[Offset++];
        Supplement.m_sChangeAmmu[i].m_aChangeAmmuNum[1] = pBuf[Offset++];
        Supplement.m_sChangeAmmu[i].m_aChangeAmmuNum[2] = pBuf[Offset++];
    }

    for(i = 0; i < Supplement.m_byChangeSuppleNum; i++) //变化的物资种类数
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
	
    //输入参数校验
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
    //是否是发给自己
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
		
    //保存配置参数
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

    //红外短距回复

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
    ChanParam.m_dwRepeatCnt = 0x1;  //只发一次
    ChanParam.m_dwScheTime = 100;  
    
    return InterChanSend(&ChanParam);
}

void ResCom_FourGParam_Set()
{
	RESCOMTASK_MAG *pMag = ResComTask_HandleGet();
		
	FRAME_BUF *pFrame = NULL;								
	
	/*设置ip地址 端口号 等*/
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

	//组包
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
		//红外短距回复
		uint8_t DatBuf[50];
    uint8_t DatBufLen;
    S_CONN_CHAN_PARAM ChanParam;
	
		//输入参数校验
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
    ChanParam.m_dwRepeatCnt = 0x1;  //只发一次
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

    //输入参数校验
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
    //是否是发给自己
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

    //红外短距回复

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
    ChanParam.m_dwRepeatCnt = 0x1;  //只发一次
    ChanParam.m_dwScheTime = 100;  
    
    return InterChanSend(&ChanParam);
}

// A3 A5 子设备建链确认
uint32_t InterConn_ProtDeal_SlaverConnectRet(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
    uint8_t Offset, TType, TSubtype, RID;
    uint16_t PID;
    uint32_t MagicID;
    SINGLE_SLAVER_MAG *pSlaverMag;

    SCOMTASK_MAG *pScomTaskMag;
    
    //输入参数校验
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

// A3 A7  无线子设备状态输出指令
uint32_t InterConn_ProtDeal_SlaverStatusRet(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
		uint8_t Offset, TType, TSubtype, WType;

    SINGLE_SLAVER_MAG *pSlaverMag;

    //输入参数校验
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

    //输入参数校验
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
				//同步
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
				
				//同步
				memcpy(&g_sUserParam.m_saAmmuInfo, &g_sUserParam.m_saWeaponSelfAmmuInfo,
					 sizeof(SOLDIERAMMU_T) * g_sUserParam.m_bySelfWeaponNum);  //保留上一次的武器设置	
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

    //输入参数校验
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
				//同步
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

				//同步
				memcpy(&g_sUserParam.m_saAmmuInfo, &g_sUserParam.m_saWeaponSelfAmmuInfo,
					 sizeof(SOLDIERAMMU_T) * g_sUserParam.m_bySelfWeaponNum);  //保留上一次的武器设置	
				
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

    //输入参数校验
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
@FUNCTION：新后台非数字化火炮发射事件数据组包
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：MTY
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
@FUNCTION：新后台非数字化火炮发射事件上传
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：MTY
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

		//重新计算

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
@FUNCTION：新后台数字化火炮发射事件数据组包
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：LW
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
@FUNCTION：新后台绑定人员（装备）的数字化火炮发射事件上传
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：LW
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

		//重新计算

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

		if(g_sVarious_Elements.m_byGS_or_ZY == 1) //非数字化协议
    {
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_byT_Type                = Ammu->m_byWeapon_Type;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_byT_Subtype             = Ammu->m_byWeapon_SubType;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_byRID                   = IDParam_GetRid();
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_aPID[0]                 = IDParam_GetPid() >> 8;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_aPID[1]                 = IDParam_GetPid() & 0xff;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_byC_Idegree             = GetInjuryDegree(g_sUserParam.m_byBlood);
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_sL_Position             = g_sUserParam.m_sLPosition;    //位置和高程
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_sTime                   = g_sUserParam.m_sUtcTime;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_byA_Type                = Ammu->m_sBulletInfo[0].m_byBullet_Type;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_byA_Subtype             = Ammu->m_sBulletInfo[0].m_byBullet_SubType;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNonDigitIndirectLanuch.m_byG_Num                 = g_sVarious_Elements.m_byG_Num;    //装药号
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
        //和单兵绑定，迫击炮模拟弹没连接前，按照单兵装具状态上传（A8，92），一旦迫击炮模拟弹连接上，状态上传用A8，94，事件上传用A8，95。
        //迫击炮数字化火炮改用绑定事件上传：数字化火炮发射事件，发射炮弹的实体类型为0x14，实体子类型为具体炮击炮，取值范围1-5.
				g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_byT_Type    = Ammu->m_byWeapon_Type;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_byT_Subtype = Ammu->m_byWeapon_SubType;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_byRID   = IDParam_GetRid();
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_aPID[0] = IDParam_GetPid() >> 8;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_aPID[1] = IDParam_GetPid() & 0xff;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_byC_Idegree = GetInjuryDegree(g_sUserParam.m_byBlood);
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_sL_Position = g_sUserParam.m_sLPosition;    //位置和高程
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_sTime       = g_sUserParam.m_sUtcTime;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_byA_Type    = Ammu->m_sBulletInfo[0].m_byBullet_Type;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_byA_Subtype = Ammu->m_sBulletInfo[0].m_byBullet_SubType;
        g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sDigitIndirectLanuch.m_byG_Num                 = g_sVarious_Elements.m_byG_Num;    //装药号
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
@FUNCTION：新后台直瞄重武器发射事件数据组包
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：MTY
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
@FUNCTION：新后台直瞄重武器发射事件上传
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：MTY
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

		//重新计算

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
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sHeavyWpnLaunch.m_sL_Position 	= g_sUserParam.m_sLPosition;    //位置和高程
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
	
    //输入参数校验
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
			
			//后台数据上传
			if(pSlaverMag->m_byTType == 0x40)
			{
					HeavyWpnLaunchEventUploadHandle(&g_sUserParam.m_saAmmuInfo[w], b); //只有单发的重武器才进行射击事件上传
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
		
    //输入参数校验
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

//合成营获取诸元信息
uint32_t InterConn_ProtDeal_SetUpVariousElements(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{  
		uint8_t TType, TSubtype, RID;
		uint8_t DatBuf[64];
    uint8_t DatBufLen, Offset;
		uint16_t PID;
	
	  S_CONN_CHAN_PARAM ChanParam;
	
		STRUCT_Various_Elements *Various_Elements = &g_sVarious_Elements; 
	
    //输入参数校验
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

    if(0 == Various_Elements->m_byGS_or_ZY)		//数字化（高斯）
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
    else //非数字化（诸元）
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
		//回包  
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
	
		//输入参数校验
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
		//是否是发给自己
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

    //输入参数校验
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

    //输入参数校验
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

//定时上传自身状态
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
				
				DatBuf[DatBufLen++] = GetInjuryDegree(g_sUserParam.m_byBlood);		//综合伤势
								
				Char1.m_bLocked          = g_sUserParam.m_sTsChar1.m_bLocked;
				Char1.m_bConnectionState = g_sUserParam.m_sTsChar1.m_bConnectionState;		//各部件连接状态0：正常	1：异常
				Char1.m_bFailureState    = g_sUserParam.m_sTsChar1.m_bFailureState;		//0：无故障	1：有故障
				Char1.m_bUnitmode		  	 = g_sUserParam.m_sTsChar1.m_bUnitmode;		//0：训练模式	1：演习模式
				Char1.m_bBatteryState	   = g_sUserParam.m_sTsChar1.m_bBatteryState;		//0：电量正常	1：欠电
				Char1.m_bIdentity     	 = g_sUserParam.m_sTsChar1.m_bIdentity;		//身份 ENUM_BRCHAR
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
						if(g_sUserParam.m_saAmmuInfo[w].m_byCfgBulletKind != 0)//判断是否有配置弹药
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

// A3 C0  头盔状态输出指令
uint32_t InterConn_ProtDeal_HelmetStatusRet(ENUM_CONN_CHAN eChan,uint8_t *pDstAddr, uint8_t DstAddrLen,uint8_t *pBuf, uint16_t BufLen)
{
    uint8_t Offset;
    SINGLE_SLAVER_MAG *pSlaverMag;
    SCOMTASK_MAG *pScomTaskMag;
    STRUCT_LPOS Pos;
    STRUCT_TIME1 Tim1;
    STRUCT_TS_Character1 Char1;
    uint16_t BatVol;
    
    //输入参数校验
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
		
    //输入参数校验
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

//根据爆炸部位计算爆炸血量
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
            case PARTITION_HEAD:	//头部
                InjuryBlood = 100;
                break;

            case PARTITION_LCHEST:	//左胸
                InjuryBlood = 100;
                break;

            case PARTITION_RCHEST:	//右胸
                InjuryBlood = 60 + rand() % 40;	//危重伤
                break;

            case PARTITION_ABDOMEN:	//腹部
                InjuryBlood = 27 + rand() % 32;	//重度伤
                break;

            case PARTITION_BACK:	//后背
                InjuryBlood = 11 + rand() % 15;	//中度伤
                break;

            case PARTITION_LARM:	//左臂
                InjuryBlood = 0 + rand() % 8;	//轻伤
                break;

            case PARTITION_RARM:	//右臂
                InjuryBlood = 0 + rand() % 8;	//轻伤
                break;

            case PARTITION_LLEG:	//左腿
                InjuryBlood = 0 + rand() % 8;	//轻伤
                break;

            case PARTITION_RLEG:	//右腿
                InjuryBlood = 0 + rand() % 8;	//轻伤
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

//zigbee爆炸内没有雷的子类型报文，收到雷后爆炸伤害后，在这里填充
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
		
		if(HurtInfo.m_HPart == PARTITION_NONE) 	//=PARTITION_NONE时屏幕无位置想显示
		{
				HurtInfo.m_HPart = PARTITION_HEAD;
		}
		
		HurtInfo.m_HType      = (ENUM_H_Type_INDIVIDUAL)H_Type_I_Wounded;
		GetBulletUniID(HurtInfo.m_A_Type, HurtInfo.m_A_Subtype, &HurtInfo.m_byUniID);   //获取武器统一编码
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
				Wtn6_Play(WTN6_YX_SL);				//手雷语音
		}
		else
		{
				Wtn6_Play(WTN6_YX_MineExplosion);	//地雷语音
		}

		//屏幕显示
		HMI_GetInjury(&g_sHmiComTask_Mag, HurtInfo.m_HPart, HurtInfo.m_RT_Type, HurtInfo.m_RT_Subtype);
		//毁伤重映射
		RemapHurtPart(&HurtInfo, 1);
		//毁伤结算
		UpdateDamage(HurtInfo);
		//事件上传
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
			
    //输入参数校验
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
		Offset += 4;		//保留
		GetOnDistance = pBuf[30];

		g_sUserParam.m_sBroadcastInfo.m_wRcvZigbeeBeaconTimeOutCnt = 0;
		
		if(g_sUserParam.m_sLPosition.m_byLposState == TRUE)
			distance = GetDistance(Position);
		else		//人员无位置，不能上车
		{
			g_sUserParam.m_sBroadcastInfo.m_byIsGetOn = FALSE;
						
			g_sUserParam.m_sBroadcastInfo.m_byVehicleTType = 0;
			g_sUserParam.m_sBroadcastInfo.m_byVehicleTSubtype	= 0;
			g_sUserParam.m_sBroadcastInfo.m_byVehicleRID = 0;
			g_sUserParam.m_sBroadcastInfo.m_wVehiclePID = 0;
			return FALSE;
		}

		//人员上下车
		if((uint8_t)distance < GetOnDistance)
		{				
				g_sUserParam.m_sBroadcastInfo.m_byRecBeacon = TRUE;
					
				g_sUserParam.m_sBroadcastInfo.m_byTargetVehicleTType = TType;
				g_sUserParam.m_sBroadcastInfo.m_byTargetVehicleTSubtype = TSubtype;
				g_sUserParam.m_sBroadcastInfo.m_byTargetVehicleRID = RID;
				g_sUserParam.m_sBroadcastInfo.m_wTargetVehiclePID = PID;
				
				memcpy(g_sUserParam.m_sBroadcastInfo.m_byTargetVehicleMac, pDstAddr, 8);
		}
		else if((uint8_t)distance > (GetOnDistance + 5)) 	//如果距离超过阈值5
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
    DatBufLen += sizeof(g_sUserParam.m_sLPosition);		//位置和高程
		
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
		
    //输入参数校验
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
	
		//是否是发给自己
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

    //输入参数校验
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
	
		//是否是发给自己
		/*
    if(TRUE != IDParam_IsIDValid(TType, TSubtype, RID, PID))
    {
        return FALSE;
    }
		*/
		InOut_Ctrl = pBuf[Offset];
		
		if(InOut_Ctrl == 1)
		{
				g_sUserParam.m_sBroadcastInfo.m_byIsGetOn = TRUE;		//上车
				
				Offset = 9;
				g_sUserParam.m_sBroadcastInfo.m_byVehicleTType	= pBuf[Offset++];
				g_sUserParam.m_sBroadcastInfo.m_byVehicleTSubtype	= pBuf[Offset++];
				g_sUserParam.m_sBroadcastInfo.m_byVehicleRID = pBuf[Offset++];
				g_sUserParam.m_sBroadcastInfo.m_wVehiclePID = pBuf[Offset] << 8 | pBuf[Offset + 1];
		}
		else if(InOut_Ctrl == 2)
		{
				g_sUserParam.m_sBroadcastInfo.m_byIsGetOn = FALSE;		//下车
			
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

		if(HurtInfo.m_HPart == PARTITION_NONE) 	//=PARTITION_NONE时屏幕无位置显示
		{
				HurtInfo.m_HPart = PARTITION_HEAD;
		}

		HurtInfo.m_HurtBlood  = GetInjuryBloodByExplodePart(HurtInfo.m_HPart, (int)distance, g_sMpu6050Mag.Pose);

		GetExplodeMineAtypeASubtype(&HurtInfo);

		Wtn6_Play(WTN6_YX_MineExplosion);	//地雷语音
		
		if(IsAlive(g_sUserParam.m_eStatus))// && Key == g_sUserParam.m_byKey)
    {
				GetBulletUniID(HurtInfo.m_A_Type, HurtInfo.m_A_Subtype, &HurtInfo.m_byUniID);   //获取武器统一编码
				g_sInjuryMag.m_Ttype    = HurtInfo.m_RT_Type;
				g_sInjuryMag.m_Tsubtype = HurtInfo.m_RT_Subtype;

				if(HurtInfo.m_HurtBlood > 100 )
				{
						HurtInfo.m_HurtBlood = 100;
				}

				//屏幕显示
				HMI_GetInjury(&g_sHmiComTask_Mag, HurtInfo.m_HPart, HurtInfo.m_RT_Type, HurtInfo.m_RT_Subtype);
				//毁伤重映射
				RemapHurtPart(&HurtInfo, 1);
				//毁伤结算
				UpdateDamage(HurtInfo);
				//事件上传
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
										sizeof(SOLDIERAMMU_T));  //保留上一次的武器设置	
								}
							}
					}

					pSlaverMag->m_byIsPowerOnFlg = TRUE;
				}
		}
}

//排雷处理
void MineClearanceHandle(u8 RID_F, u16 PID_F, u8 M_A_TYPE, u8 M_A_SUBTYPE, u8 M_T_TYPE, u8 M_T_SUBTYPE)
{
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_byT_Type    	= IDParam_GetTType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_byT_Subtype 	= IDParam_GetTSubType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_byRID 			  = IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_aPID[0] 		  = IDParam_GetPid() >>8 ;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_aPID[1] 		  = IDParam_GetPid() &0xff;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_byC_Idegree 	= GetInjuryDegree(g_sUserParam.m_byBlood);
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_sL_Position 	= g_sUserParam.m_sLPosition;    //位置和高程
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

//步雷处理
void MineSetHandle(u8 RID_F, u16 PID_F, u8 M_A_TYPE, u8 M_A_SUBTYPE, u8 M_T_TYPE, u8 M_T_SUBTYPE)
{
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_byT_Type    	= IDParam_GetTType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_byT_Subtype 	= IDParam_GetTSubType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_byRID 			  = IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_aPID[0] 		  = IDParam_GetPid() >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_aPID[1] 		  = IDParam_GetPid() &0xff;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_byC_Idegree 	= GetInjuryDegree(g_sUserParam.m_byBlood);
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndCleanMine.m_sL_Position 	= g_sUserParam.m_sLPosition;    //位置和高程
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
//内部互联互通
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

//内部互联互通协议解析
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
            if(g_sUserParam.LayMines_Or_MineClearance == 0x01)		//布雷指令
            {
								g_sUserParam.LayMines_Or_MineClearance = 0x00;
                Wtn6_Play(WTN6_MINE_SET_SUCCESS);
                MineSetHandle(pBuf[5], pBuf[6] << 8 | pBuf[7], 0xFF, 0xFF, pBuf[3], pBuf[4]);
            }
            else if(g_sUserParam.LayMines_Or_MineClearance == 0x02)		//排雷指令
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
				
        case 0xA2A0:	// 4.2 无线设备MAC地址获取指令
            InterConn_ProtDeal_MacGet(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
            break;

        case 0xA2A1:	// 4.4 身份信息设置指令（0xA2，0xA1）
            InterConn_ProtDeal_IDSet(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
            break;

        case 0xA2A3:	// 4.8 清空无线子设备指令
            InterConn_ProtDeal_SlaverClear(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
            SlaverMag_ListInit();
            break;

        case 0xA2A4:	// 4.10 配置无线子设备指令（0xA2，0xA4）
            InterConn_ProtDeal_SlaverConfig(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
            SlaverMag_ListInit();
            break;

        case 0xA2AD:	// 4.28 获取工况信息指令（0xA2，0xAD）
            InterConn_ProtDeal_InfoSend(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
            break;

        case 0xA2AE:	// 4.29 实体参数获取指令（0xA2，0xAE） 删除
						InterConn_ProtDeal_ParamSend(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
            break;

        case 0xA2AF:	// 4.31 实体参数设置指令（0xA2，0xAF）
            InterConn_ProtDeal_ParamSet(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
            break;

        case 0xA2FF:	// 板级测试模式（0xA2，0xFF） 自定义
            InterConn_ProtDeal_InfoSend(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
            TesModeHandle();
            break;
				
        case 0xA3A5:
            InterConn_ProtDeal_SlaverConnectRet(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
            break;
				
				case 0xA3A6:
						InterConn_ProtDeal_Slaver_WeaponLockRet(eChan,ZigComTask_LastCommMacGet(),8,pBuf, BufLen);
						break;
				
        case 0xA3A7:		//无线子设备状态回复
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
//红外接收处理
//红外短距弹药物资补给指令处理
void IRAmmuGetHandle(Struct_CtrlSupplementDownload *Supplement)
{
		uint8_t b, i, k;
    uint32_t BulletNum = 0, BulletTotalNum = 0, BulletAddNum = 0, BulletRemain = 0;
    SOLDIERAMMU_T *pAmmuInfo = NULL;

    //将弹药的remain和补弹相加再放入remain
    for(b = 0; b < Supplement->m_byChangeAmmuNum; b++) //弹药变化种类
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

                    //Zigbee发送
                    InterConn_ProtDeal_Slaver_AmmuSupplyNum(pAmmuInfo, k, BulletNum);

										//事件上传
                    Supplement->m_byMasterT_Type 	  = 0x48;	//调理设备
                    Supplement->m_byMasterT_Subtype = 0x02;	//模拟装弹器
                    Supplement->m_byMasterRID 		= 0xff;
                    Supplement->m_aMasterPID[0] 	= 0xff;
                    Supplement->m_aMasterPID[1] 	= 0xff;
                    RSP_CtrlSupplementEventUploadHandle(Supplement);
                }
            }
        }
    }

    //补弹提示
    Wtn6_Play(WTN6_ReplenishAmmunition);
}

//连带毁伤
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

//广播
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

//硬件控制协议解析
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
        case 0x06://48k探头接收
            if(DevCtrl == 0x01 && pBuf[8] == 0x10 && DatLen > 6)
            {
                //pBuf[9] |= 0x80; //探头编号最高位变为1
								/*
								if((pBuf[7] < 16) && (pBuf[11] == 0x7A))
								{
									if(pBuf[19] == 0x01)
										CtrlHelmetLaserOnOff(pBuf[9], 1);
									else
										CtrlHelmetLaserOnOff(pBuf[9], 0);
								}
								*/
								LaserProtocalDeal(pMag, &pBuf[6], pBuf[7], TRUE);		//支持五面探头和小探头
								ZigComTask_SendToHelmet(ZigComTask_LastCommMacGet(), 8 ,&pBuf[6], BufLen-8, 0x06, CTRL_RECV);
                //ResComTask_SendToUpcom(&pBuf[6], BufLen-8, 0x06, CTRL_RECV);
            }
            else if(DevCtrl == 0x01 && pBuf[8] == 0xAF)
            {
                //pBuf[9] |= 0x80; //探头编号最高位变为1
								//LaserProtocalDeal(pMag, &pBuf[8], DatLen);		//支持五面探头和小探头
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
        //头盔
        case 0xFE08:
            InterConn_HelmetStaGet(eChan, pMag->m_aWMac, sizeof(pMag->m_aWMac));
            break;

        default:
            InterConn_SlaverStaGet(eChan, pMag->m_aWMac, sizeof(pMag->m_aWMac));
            break;
    }
}

/****************************************************************
@FUNCTION：子设备状态初始列表初始化
@INPUT：无
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
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

        //系统参数中的MAC有效
        if(TRUE == IsParamValid(pParam->m_aWMac, sizeof(pParam->m_aWMac)))
        {
            //往运行时参数中新增该条记录
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

            //子设备建链	
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
@FUNCTION：从子设备状态列表获取指定MAC地址的子设备管理结构体指针
@INPUT：MAC地址
@OUTPUT：无
@RETURN: 子设备管理结构体指针
@AUTHOR：xfw
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
    
    //先搜索当前的列表
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
@FUNCTION：从子设备状态列表获取指定实体类型的子设备管理结构体指针
@INPUT：实体类型+ 实体子类那个
@OUTPUT：无
@RETURN: 子设备管理结构体指针
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
SINGLE_SLAVER_MAG* SlaverMag_GetHandleByType(uint8_t TType,uint8_t TSubType)
{
    uint8_t i;
    SINGLE_SLAVER_MAG *pMag = NULL;
    
    //先搜索当前的列表
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
//子设备维护
void SlaverMag_TimedProcess(void)
{
    SINGLE_SLAVER_MAG *pMag = NULL;
		SCOMTASK_MAG *pScomTaskMag;
	  uint8_t i;
    uint32_t NowTimeMs = GetSysRunMilliSecond();
		uint32_t MagicID;
    
    //遍历当前的列表,建链不在该定时任务内
    for(i = 0; i < UserParam_GetSlaverCnt(); i++)
    {
        pMag = UserParam_GetSlaverMag(i);
        if(NULL == pMag || pMag->m_byIsOnline == FALSE || 0xFF == pMag->m_byTType || pMag->m_byTType == 0)
        {
            continue;
        }
				
        //建链成功，在线，判断离线
				if(pMag->m_byIsPowerOnFlg == TRUE)
				{
						//子设备建链
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
				
				//定时获取状态
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

    //先转给本身
    ComTask_RetransmissionData(LaserComTask_TaskHandleGet(), pBuf, BufLen);
    
    DevIdx = pBuf[0];
    DevCtrl = pBuf[1];
    
    //择情转给别的终端
    Len = BufLen-3;
    Offset = 3;
    
    //背甲灯控
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
    //模式控制
    else if((Len >= 5) && (0xB1 == pBuf[Offset+2]))
    {
        HardCtrl_HelmetBase(pBuf,BufLen);
    }
#if 0
    //遮挡探测
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
        Off = UserParam_GetFireInstallFlg(); // 0-未安装，1-已安装
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

//开机通知
void HardCtrl_PowerOn_Process(void)
{
    uint8_t OnSta = 1;

    
    ResComTask_SendToUpcom(&OnSta, 1, 0x19, CTRL_DI);
}

//位置通报
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
    //开机通知
    ResComTask_SendToUpcom(Buf, Off, 0xF1, CTRL_DAT);
}

//头盔状态通报
void HardCtrl_HelmetNoti_Process(void)
{
    uint8_t Buf[30];
    uint8_t Off = 0;

    SINGLE_SLAVER_MAG *pSingleMag;

    pSingleMag = SlaverMag_GetHandleByType(0xFE,0x08);//找到头盔
    Buf[Off++] = pSingleMag->m_byIsOnline;
    Buf[Off++] = UserParam_GetActLaserCnt1();
    Buf[Off++] = GetBatRemainingCapacity_18650(pSingleMag->m_wBatVol, 1);
   
    //开机通知
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
				
				//组包
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
	
		if(cnt++ < 300)		//5分钟检测一次
			return;
		
		cnt = 0;
		
		if(GetBatRemainingCapacity_18650(BatTask_GetExBatVol(), 1) < 3)
		{
			Wtn6_Play(WTN6_LowBattery_Cloth);
			Wtn6_Play(WTN6_PLS_CHANGE_BATTERY);
		}
		
    //子设备
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

//硬件控制协议处理（55AA协议0功能码）
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
                    
                    //涉及头盔探头数据
                    ProtDeal_HardCtrl_48k(&pBuf[0], BufLen);
                    break;

                case 0x07:
                    ComTask_RetransmissionData(IR38ComTask_TaskHandleGet(), &pBuf[3], BufLen - 3);
                    break;

                case 0x13:
                    SComTask_RetransmissionData(HmiComTask_TaskHandleGet(), &pBuf[3], BufLen - 3);
                    break;

                //电源
                case 0x19:
                    ProtDeal_HardCtrl_Power(&pBuf[1], BufLen - 1);
                    break;

                //运行灯
                case 0x01:
                    ProtDeal_HardCtrl_RunLed(&pBuf[1], BufLen - 1);
                    break;

                //语音芯片
                case 0x0D:
                case 0x0E:
                case 0x0F:
                    ProtDeal_HardCtrl_Voice(&pBuf[1], BufLen - 1);
                    break;

                //马达
                case 0x04:
                    ProtDeal_HardCtrl_Motor(&pBuf[1], BufLen - 1);
                    break;

                //发烟罐1
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

//头盔硬件控制基础函数
void HardCtrl_HelmetBase(uint8_t *pBuf, uint16_t BufLen)
{
    SINGLE_SLAVER_MAG *pSingleMag;

    pSingleMag = SlaverMag_GetHandleByType(0xFE,0x08);//找到头盔
    if(NULL != pSingleMag && FALSE == pSingleMag->m_byIsSleepFlg)
    {
        InterConn_SlaverHardCtrl(CHAN_ZIGBEE,pSingleMag->m_aWMac,sizeof(pSingleMag->m_aWMac),pBuf,BufLen);
    }
}


//控制头盔发烟罐发烟
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

//控制头盔探头灯
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
    Buf[Off++] = 0x00;		// 1-亮，0-灭
    Buf[Off++] = 0xFF;		// 探头编号，从1开始
		//Buf[Off++] = 0;
		//Buf[Off++] = 0;
		//Buf[Off++] = 0;
		//Buf[Off++] = 0;
		//Buf[Off++] = 0;
    Buf[Off++] = 0xAA;
    
		HardCtrl_HelmetBase(Buf,Off);
}

//控制头盔探头灯
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
    Buf[Off++] = 0x01;		// 1-亮，0-灭
    Buf[Off++] = 0xFF;		// 探头编号，从1开始
		//Buf[Off++] = 1;
		//Buf[Off++] = 1;
		//Buf[Off++] = 1;
		//Buf[Off++] = 1;
		//Buf[Off++] = 1;
    Buf[Off++] = 0xAA;
    
		HardCtrl_HelmetBase(Buf,Off);
}

//控制单个头盔探头灯
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
    Buf[Off++] = IsOn?1:0;// 1-亮，0-灭
    Buf[Off++] = LedIdx; // 探头编号，从1开始
    Buf[Off++] = 0xAA;
    
		HardCtrl_HelmetBase(Buf,Off);
}

//控制头盔扬声器
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

  pBuf[num++] = BNEventType_RSP_CtrlManoParam_IndividualSoldier;		//事件类型
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
@FUNCTION：新后台演习参数设置成功事件上传
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：LW
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

  //帧头赋值
	pMag->m_aData_Frame[0] = 0xA8;
	pMag->m_aData_Frame[1] = BNCmd_Terminal_EventUpload;

	pFrame->m_wBufLen = 0;
	pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[0];
	pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aData_Frame[1];
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0;

  //事件数据组包
	memset(BackStage_dataBuf,0,sizeof(BackStage_dataBuf));						
	CtrlManoParamIndividualSoldier_EventData_Spell(&pMag->Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier,BackStage_dataBuf,&Len);
	memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen], BackStage_dataBuf, Len);
	pFrame->m_wBufLen += Len;
	
  //事件帧序列号
	pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[0];
	pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[1];

  //计算异或和
	pMag->m_byXor = 0;
	for(cut = 3; cut < pFrame->m_wBufLen - 2; cut++)		//事件序列号不参与校验
	{
		pMag->m_byXor ^= pFrame->m_byBuf[cut];
	}
	pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_byXor;

  //数据帧内容长度  
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
    //变量赋值
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_byT_Type = IDParam_GetTType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_byT_Subtype = IDParam_GetTSubType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_byRID = IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_aPID[0] = IDParam_GetPid() >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_aPID[1] = IDParam_GetPid() & 0xff;
    memcpy(&g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_sRecCmdTime, &g_sResComTask_Mag.m_sCmdTime.m_BSNulCtrlManoeuvre, sizeof(STRUCT_TIME1)); //接收指令时间
    memcpy(&g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_sStartTime, &ManoParam->m_sStartTime, sizeof(STRUCT_TIME1)); //演习开始时间内
		memcpy(&g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlManoParam_IndividualSoldier.m_sEndTime, &ManoParam->m_sEndTime, sizeof(STRUCT_TIME1));   	 //演习结束时间内

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
@FUNCTION：新后台上传周期设置成功事件数据组包
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：LW
@SPECIAL:
****************************************************************/
void RSP_CtrlIntervalEventData_Spell(StructEvent_RSP_CtrlInterval *pMag,u8 *pBuf,u8 *Len)
{
	uint8_t num = 0;

  //组包
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
@FUNCTION：新后台周期设置成功事件上传
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：LW
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

//计算未上传事件编号
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

    //未上传事件响应标记
		memcpy(g_sResComTask_Mag.BackupArea[0].EvtRspFlgBak, g_sResComTask_Mag.EvtRspFlg, sizeof(g_sResComTask_Mag.EvtRspFlg));

    //开始结束时间
    memcpy(&g_sResComTask_Mag.BackupArea[0].m_sManoStart, &g_sUserParam.m_sManoStart, sizeof(STRUCT_TIME1));
    //演习结束时间
		memcpy(&g_sResComTask_Mag.BackupArea[0].m_sManoEnd, &g_sUserParam.m_sManoEnd, sizeof(STRUCT_TIME1));
    //切换到白天的起始时间
    memcpy(&g_sResComTask_Mag.BackupArea[0].m_sDayTime, &g_sUserParam.m_sDayStartTime, sizeof(STRUCT_TIME2));
    //切换到黑夜的起始时间
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
				//探头灯带恢复
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

    //计算校验和
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
		//人员上下车
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
		
		//伤情采集
		g_sInjuryMag.m_byRcdCnt = 0;
		
		g_sUserParam.Thickness = 0;
    g_sUserParam.m_IsIllegalPowerOff = FALSE;

    //演习开始时间
    memset(&g_sUserParam.m_sManoStart, 0, sizeof(STRUCT_TIME1));
    //演习结束时间
    memset(&g_sUserParam.m_sManoEnd, 0, sizeof(STRUCT_TIME1));

    memset(&g_sUserParam.EP_Num, 0, 3);

    g_sResComTask_Mag.UnUpLoadEvtCnt  = 0;
    g_sResComTask_Mag.m_dwCtrlEvtNum  = 0;

    //清除未响应事件标记
    memset(g_sResComTask_Mag.EvtRspFlg, 0, sizeof(g_sResComTask_Mag.EvtRspFlg));
		memset(g_sResComTask_Mag.EvtRspFlgBak, 0, sizeof(g_sResComTask_Mag.EvtRspFlg));

    g_sResComTask_Mag.Vios.DetectorLoss = FALSE;
    g_sResComTask_Mag.Vios.FumeUnInstall = FALSE;
    g_sResComTask_Mag.Vios.HelmetLoss = FALSE;
    g_sResComTask_Mag.Vios.IllegalMove = FALSE;
}

void AssignStateFromBuffer(PARAMATER_BACKUP *BK)
{
    //两个区域的值同步
    if(g_sResComTask_Mag.BackupArea[0].HeadFlg[0] == 0x5A && g_sResComTask_Mag.BackupArea[0].HeadFlg[1] == 0xA5)
    {
        memcpy(&g_sResComTask_Mag.BackupArea[1], &g_sResComTask_Mag.BackupArea[0], sizeof(PARAMATER_BACKUP));
    }
    else if(g_sResComTask_Mag.BackupArea[1].HeadFlg[0] == 0x5A && g_sResComTask_Mag.BackupArea[1].HeadFlg[1] == 0xA5)
    {
        memcpy(&g_sResComTask_Mag.BackupArea[0], &g_sResComTask_Mag.BackupArea[1], sizeof(PARAMATER_BACKUP));
    }

		/*
    //判断演习参数是不是0
    if(BK->EP_Num[0] == 0 && BK->EP_Num[1] == 0 && BK->EP_Num[2] == 0)
    {
        TrainModeInit();
        return;
    }
		*/
    //为训练模式下关机
    if(BK->IsDrill == TRAIN)
    {
        g_sUserParam.m_byMode = TRAIN;
				Wtn6_Play(WTN6_Mode_Train);
				g_sUserParam.m_sTsChar1.m_bUnitmode = FALSE;
				g_sUserParam.m_sTsChar1.m_bLocked = FALSE;
				g_sUserParam.m_sTsChar1.m_bFailureState = FALSE;
        
				g_sUserParam.m_sLPosition.m_byLposState = FALSE;
				//人员上下车
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
				//事件上传
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

        //演习开始时间
        memcpy(&g_sUserParam.m_sManoStart, &g_sResComTask_Mag.BackupArea[0].m_sManoStart, sizeof(STRUCT_TIME1));
        //演习结束时间
        memcpy(&g_sUserParam.m_sManoEnd, &g_sResComTask_Mag.BackupArea[0].m_sManoEnd, sizeof(STRUCT_TIME1));

        //切换到白天的起始时间
        memcpy(&g_sUserParam.m_sDayStartTime, &BK->m_sDayTime, sizeof(Struct_Time2));
        //切换到黑夜的起始时间
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

        //清除未响应事件标记
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
    else 	//演习模式
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
        //昼夜模式需要恢复
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
        
				//演习开始时间
				memcpy(&g_sUserParam.m_sManoStart, &g_sResComTask_Mag.BackupArea[0].m_sManoStart, sizeof(STRUCT_TIME1));
				//演习结束时间
        memcpy(&g_sUserParam.m_sManoEnd, &g_sResComTask_Mag.BackupArea[0].m_sManoEnd, sizeof(STRUCT_TIME1));

        //切换到白天的起始时间
        memcpy(&g_sUserParam.m_sDayStartTime, &BK->m_sDayTime, sizeof(STRUCT_TIME2));
        //切换到黑夜的起始时间
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
        
            g_sHmiComTask_Mag.m_byHurtCountFlg = BK->InjuryCnt;	//利用头盔蓝牙断开恢复会恢复之前的伤部图片

            for(u8 i = 0; i < g_sHmiComTask_Mag.m_byHurtCountFlg; i++)
            {
                g_sHmiComTask_Mag.m_sInjury[i].m_byPart = BK->m_byInjuryPart[i];
            }

            //探头灯带恢复
            for(u8 i = 0; i < sizeof(BK->Decter); i++)
            {
                if(BK->Decter[i] == TRUE)
                {
                    Laser_CtrlLedOn(i, &g_sLaserComTask_Mag); //点亮灯
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

//参数恢复
u8 DrillStateReadFromOutFlash(uint8_t *pBuf, uint32_t Len, uint8_t Area)
{
    uint32_t WriteAddr = 0;
    uint8_t Xor = 0;

    if(Area == 1)
    {
				WriteAddr = g_sIFlashMag.m_dwCapacity * 1024 - g_sIFlashMag.m_wPageSize * 4;		//数据存储的地址
    }
    else if(Area == 2)
    {
        WriteAddr = g_sIFlashMag.m_dwCapacity * 1024 - g_sIFlashMag.m_wPageSize * 3;		//数据存储的地址
    }
		else
			return FALSE;

		IFlash_Read(&g_sIFlashMag, FLASH_BOOT_START + WriteAddr, pBuf, Len);
}

//演习参数恢复
uint8_t DrillStateRecovery(void)
{
    uint8_t XorA = 0, XorB = 0;

    //从flash A区恢复参数
    DrillStateReadFromOutFlash((u8*)(&g_sResComTask_Mag.BackupArea[0]), sizeof(PARAMATER_BACKUP), 1);
    //从flash B区恢复参数
    DrillStateReadFromOutFlash((u8*)(&g_sResComTask_Mag.BackupArea[1]), sizeof(PARAMATER_BACKUP), 2);

    //计算校验和
    for(uint32_t i = 0; i < sizeof(PARAMATER_BACKUP) - 1; i++)
    {
        XorA ^= ((u8*)(&g_sResComTask_Mag.BackupArea[0]))[i];
    }

    //计算校验和
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
        //没有参数，转为训练模式开机
        TrainModeInit();
    }
}

//参数保存
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

	//计算校验和
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
	//A信息拷贝到B信息
	memcpy(&g_sResComTask_Mag.BackupArea[1],&g_sResComTask_Mag.BackupArea[0],sizeof(PARAMATER_BACKUP));
	
	//保存参数到外部flash
	IWDG_Feed();
	DrillStateWriteToOutFlash((uint8_t*)(&g_sResComTask_Mag.BackupArea[0]),sizeof(PARAMATER_BACKUP),1);
	IWDG_Feed();
	DrillStateWriteToOutFlash((uint8_t*)(&g_sResComTask_Mag.BackupArea[1]),sizeof(PARAMATER_BACKUP),2);
	IWDG_Feed();
}

void FullConditionRecovery()
{
    PARAMATER_BACKUP *BK = NULL;

    //两个区域的值同步
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

        //判断演习参数是不是0
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

        //切换到白天的起始时间
        memcpy(&g_sUserParam.m_sDayStartTime, &BK->m_sDayTime, sizeof(Struct_Time2));
        //切换到黑夜的起始时间
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

//记录本次接收后台协议的UTC时间
void RecordRcvBackStageProtocolTime(StructUTC * Time)
{
    Time->byYear   = g_sUserParam.m_sUtcTime.m_byYear;
    Time->byMonth  = g_sUserParam.m_sUtcTime.m_byMonth;
    Time->byDay    = g_sUserParam.m_sUtcTime.m_byDate;
    Time->byHour   = g_sUserParam.m_sUtcTime.m_byHour;
    Time->byMin    = g_sUserParam.m_sUtcTime.m_byMinute;
    Time->bySecond = g_sUserParam.m_sUtcTime.m_bySecond;
}

//后台接收参数设置数据处理
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

    //开始时间
    sprintf((char*)StartBuf, "%d%02d%02d%02d%02d",
            ManoParam.m_sStartTime.m_byYear,
            ManoParam.m_sStartTime.m_byMonth,
            ManoParam.m_sStartTime.m_byDate,
            ManoParam.m_sStartTime.m_byHour,
            ManoParam.m_sStartTime.m_byMinute);

    //结束时间
    sprintf((char*)EndBuf, "%d%02d%02d%02d%02d",
            ManoParam.m_sEndTime.m_byYear,
            ManoParam.m_sEndTime.m_byMonth,
            ManoParam.m_sEndTime.m_byDate,
            ManoParam.m_sEndTime.m_byHour,
            ManoParam.m_sEndTime.m_byMinute);

    //演习结束时间小于演习开始时间，不符合实际
    if(0 <= memcmp(StartBuf, EndBuf, sizeof(StartBuf)))
    {
        return FALSE;
    }
   
		//武器信息,同步武器状态
		//g_sUserParam.m_bySelfWeaponNum = UserParam_GetWeaponNum();
		//将上一次的后台武器配置保存
		//memcpy(&g_sUserParam.m_saWeaponSelfAmmuInfo, &g_sUserParam.m_saAmmuInfo, sizeof(SOLDIERAMMU_T) * g_sUserParam.m_bySelfWeaponNum); //保留上一次的武器设置
		/*
		for(uint8_t w = g_sUserParam.m_bySelfWeaponNum; w < WEAPONKIND_MAX; w++) //清除数组后不需要的信息,防止上次遗留
		{
				memset(&g_sUserParam.m_saWeaponSelfAmmuInfo[w], 0, sizeof(SOLDIERAMMU_T));
		}
		*/
		//演习开始时间
		memcpy(&g_sUserParam.m_sManoStart, &ManoParam.m_sStartTime, sizeof(STRUCT_TIME1));

		//演习结束时间
		memcpy(&g_sUserParam.m_sManoEnd, &ManoParam.m_sEndTime, sizeof(STRUCT_TIME1));

		//切换到白天的起始时间
		memcpy(&g_sUserParam.m_sDayStartTime, &ManoParam.m_sDayStartTime, sizeof(STRUCT_TIME2));

		//切换到黑夜的起始时间
		memcpy(&g_sUserParam.m_sNightStartTime, &ManoParam.m_sNightStartTime, sizeof(STRUCT_TIME2));

		//秘钥
		UserParam_SetKey(ManoParam.m_byKey);
		//红蓝属性
		UserParam_SetBRChar(ManoParam.m_byBR_Attribute);

		//同步演习参数的子类型
		IDParam_SetTSubType(ManoParam.m_byT_Subtype);

		//赋值新配置，最多3把武器，防止数组超限
		g_sUserParam.m_byBackstageCfgWeaponNum = ManoParam.m_byAmmuNum;

		if(g_sUserParam.m_byBackstageCfgWeaponNum > WEAPONKIND_MAX)
				g_sUserParam.m_byBackstageCfgWeaponNum = WEAPONKIND_MAX;

		memcpy(&g_sUserParam.m_saBackstageCfgAmmuInfo, &ManoParam.m_sAmmu, sizeof(SOLDIERAMMU_T) * g_sUserParam.m_byBackstageCfgWeaponNum); //将武器设置更新到武器信息当中

		//将其中弹药的remain也填充
		for(uint8_t w = 0; w < g_sUserParam.m_byBackstageCfgWeaponNum; w++)
		{
				for(uint8_t b = 0; b < g_sUserParam.m_saBackstageCfgAmmuInfo[w].m_byCfgBulletKind; b++)
				{
						memcpy(g_sUserParam.m_saBackstageCfgAmmuInfo[w].m_sBulletInfo[b].m_aBulletRemainNum, g_sUserParam.m_saBackstageCfgAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum, 3);
						memcpy(g_sUserParam.m_saBackstageCfgAmmuInfo[w].m_sBulletInfo[b].m_aBulletTotalNum, g_sUserParam.m_saBackstageCfgAmmuInfo[w].m_sBulletInfo[b].m_aBulletCfgNum, 3);
				}
		}
		
		for(uint8_t w = g_sUserParam.m_byBackstageCfgWeaponNum; w < WEAPONKIND_MAX; w++) //清除数组后不需要的信息,防止上次遗留
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

		//配置方案编号
		memcpy(&g_sUserParam.EP_Num, &ManoParam.EP_Num, 3);
		//实体防护能力
		g_sUserParam.Thickness = ManoParam.Thickness;
		InitShell(g_sUserParam.Thickness);

		g_sResComTask_Mag.m_ManoParamFlg = TRUE;  //被配置过演习参数的标记

		//保存演习参数到flash
		SaveDrillState(g_sUserParam.m_byMode, g_sUserParam.m_byEnableChangeBat);

		InterConn_ProtDeal_SelfStatue();
		return TRUE;
}

//上传周期设置指令处理
void BNIntervalSetHandle(Struct_CtrlIntervalDownload Interval)
{
    uint8_t Xor = 0;
	
    //实体自身状态数据上传周期
    g_sUserParam.m_byUploadInterval = Interval.m_bySelfStatusDataUploadCycle;//秒
    g_sResComTask_Mag.m_dwRptInterval = g_sUserParam.m_byUploadInterval * 1000; //转为毫秒
    
		//实体绑定炸点显示器状态数据上传周期
    g_sUserParam.m_byDynamometerDisplayStatusDataUploadCycle = Interval.m_byDynamometerDisplayStatusDataUploadCycle;
    //绑定人员（装备）数量和身份数据上传周期
    g_sUserParam.m_byEquipmentAndIdentityDataUploadCycle = Interval.m_byEquipmentAndIdentityDataUploadCycle;

    //保存状态
    g_sResComTask_Mag.BackupArea[0].RptInterval = g_sResComTask_Mag.m_dwRptInterval;

    //计算校验和
    for(uint32_t i = 0; i < sizeof(PARAMATER_BACKUP) - 1; i++)
    {
        Xor ^= ((u8*)(&g_sResComTask_Mag.BackupArea[0]))[i];
    }

    g_sResComTask_Mag.BackupArea[0].XOR = Xor;
}

void RSP_CtrlIntervalEventUploadHandle(Struct_CtrlIntervalDownload *Interval)
{
    //参数赋值
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_byT_Type =	IDParam_GetTType();		//被设置实体类型
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_byT_Subtype = IDParam_GetTSubType();		//被设置实体子类型
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_byRID = IDParam_GetRid();		//被设置实体单位代字
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_aPID[0] = IDParam_GetPid() >> 8;		//被设置实体设备编号
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_aPID[1] = IDParam_GetPid() & 0xff;	
    memcpy(&g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_sRecCmdTime, &g_sResComTask_Mag.m_sCmdTime.m_sBSNulCtrlInterval, sizeof(STRUCT_TIME1));		//接收指令时间
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_byST_Type = Interval->m_byMasterT_Type;		//实施设置方实体类型
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_byST_Subtype = Interval->m_byMasterT_Subtype;		//实施设置方实体子类型
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_byS_RID = Interval->m_byMasterRID;		//实施设置方实体单位代字
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_aS_PID[0] = Interval->m_aMasterPID[0];		//实施设置方实体设备编号
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_aS_PID[1] = Interval->m_aMasterPID[1];

    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_bySelfStatusDataUploadCycle = Interval->m_bySelfStatusDataUploadCycle;	//实体自身状态数据上传周期 	秒
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_byDynamometerDisplayStatusDataUploadCycle = Interval->m_byDynamometerDisplayStatusDataUploadCycle;		//实体绑定炸点显示器状态数据上传周期  分
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlInterval.m_byEquipmentAndIdentityDataUploadCycle = Interval->m_byEquipmentAndIdentityDataUploadCycle;			//绑定人员（装备）数量和身份数据上传周期  分

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

//后台弹药物资补给指令处理
void BNSupplementHandle(Struct_CtrlSupplementDownload *Supplement)
{
		SOLDIERAMMU_T *pAmmuInfo = NULL;
    uint32_t BulletNum = 0, BulletTotalNum = 0, BulletAddNum = 0, BulletRemain = 0;
		
		g_sUserParam.m_byWeaponNum = UserParam_GetWeaponNum();
    //将弹药的remain和补弹相加再放入remain
    for(u8 b = 0; b < Supplement->m_byChangeAmmuNum; b++) //弹药变化种类
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

							//发送给武器子设备
							InterConn_ProtDeal_Slaver_AmmuSupplyNum(pAmmuInfo, k, BulletNum);
					}
				}
			}
    }
		InterConn_ProtDeal_SelfStatue();
    //Wtn6_Play(WTN6_ReplenishAmmunition);
}

/****************************************************************
@FUNCTION：新后台弹药（物资）分发/补给事件数据组包
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：LW
@SPECIAL:
****************************************************************/
void RSP_CtrlSupplementEventData_Spell(StructEvent_RSP_CtrlSupplement *pMag,u8 *pBuf,u8 *Len)
{
	u8 num = 0,cut = 0;
	 
  //组包
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
@FUNCTION：新后台弹药（物资）分发/补给事件上传
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：LW
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
		
		//组包
		RSP_CtrlSupplementEventData_Spell(&pMag->Event_Pack.m_sRSP_CtrlSupplement,BackStage_dataBuf,&Len);
		memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen], BackStage_dataBuf, Len);
		pFrame->m_wBufLen += Len;
		
		//事件帧序列号
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[1];
		
		//重新计算
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
    //变量赋值
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_byT_Type =	IDParam_GetTType();		//被设置实体类型
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_byT_Subtype = IDParam_GetTSubType();		//被设置实体子类型
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_byRID = IDParam_GetRid();		//被设置实体单位代字
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_aPID[0] = IDParam_GetPid() >> 8;		//被设置实体设备编号
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_aPID[1] = IDParam_GetPid() & 0xff;
		g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_byC_Idegree = GetInjuryDegree(g_sUserParam.m_byBlood);  	//综合伤势 ENUM_CI_ARMOR  ENUM_CI_INDIVIDUAL  ENUM_CI_OTHER
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_sL_Position = g_sUserParam.m_sLPosition; //经纬度 高度 定位状态
    memcpy(&g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_sRecCmdTime, &g_sResComTask_Mag.m_sCmdTime.m_BSNulCtrlManoeuvre, sizeof(STRUCT_TIME1));  //接收指令时间
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_byST_Type = Supplement->m_byMasterT_Type;		//引起弹药/物资变化的实体类型
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_byST_Subtype = Supplement->m_byMasterT_Subtype;		//引起弹药/物资变化的实体子类型
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_byS_RID = Supplement->m_byMasterRID;		//引起弹药/物资变化的实体单位代字
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_aS_PID[0] = Supplement->m_aMasterPID[0];		//引起弹药/物资变化的实体设备编号
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_aS_PID[1] = Supplement->m_aMasterPID[1];
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_byIO_Idegree = IOCHAR_IN;		//弹药 (物资)补给标识字	ENUM_IOCHAR
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_byChangeAmmuNum = Supplement->m_byChangeAmmuNum;		//发生变化弹药种类数（m，m=0 时，弹药数据区无内容）
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_byChangeSuppleNum = Supplement->m_byChangeSuppleNum;		//发生变化物资种类数（n，n=0 时，物资数据区无内容）

    for(u8 w = 0; w < g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_byChangeAmmuNum; w++)
    {
        memcpy(&g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_sChangeAmmu[w], &Supplement->m_sChangeAmmu[w], sizeof(STRUCT_AMMUSUPPLY_t));    //弹药信息
    }

    for(u8 s = 0; s < g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_byChangeSuppleNum; s++)
    {
        memcpy(&g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sRSP_CtrlSupplement.m_sChangeSupply[s], &Supplement->m_sChangeSupply, sizeof(Struct_Supply));  //物资信息
    }

    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;

    RSP_CtrlSupplementEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}

/****************************************************************
@FUNCTION：新后台演习模式转换事件数据组包
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：MTY
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
@FUNCTION：新后台演习模式转换事件上传
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：MTY
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
		
		//组包
		ChangeToManoEventData_Spell(&pMag->Event_Pack.m_sChangeToMano, BackStage_dataBuf, &Len);
    memcpy(&pFrame->m_byBuf[pFrame->m_wBufLen], BackStage_dataBuf, Len);
		pFrame->m_wBufLen += Len;
		
		//事件帧序列号
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[0];
		pFrame->m_byBuf[pFrame->m_wBufLen++] = pMag->m_aES_Num[1];
		
		pMag->m_byXor = 0;
		
		//重新计算

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

//判死
void BSN_Judge_DeathHandle(RCVHURTINFO_T Info)
{
		if(UserParam_GetBlood() == 0)
			return;
    Info.m_HurtBlood = 0;
    UpdateDamage(Info);
}

//复活
void BSN_Judge_ReviveHandle(RCVHURTINFO_T Info)
{
		if(UserParam_GetBlood() == 100)
			return;
    Info.m_HurtBlood = 100;
    UpdateDamage(Info);
}

//复位
void BSN_Judge_RestartHandle(RCVHURTINFO_T Info)
{
    Info.m_HurtBlood = 100;
    UpdateDamage(Info);
    InterConn_ProtDeal_Slaver_Weapon_State(FALSE);
		InterConn_ProtDeal_SelfStatue();
    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 2);
}

//近弹处理
void ShotAroundHandle(u8 Part)
{
    switch(Part)
    {
        case 0:		// 左侧近弹
						Wtn6_Play(WTN6_YX_ZDHX1);
            //Wtn6_StartLSound(WTN6_YX_ZDHX1);
            break;

        case 1:		// 右侧近弹
						Wtn6_Play(WTN6_YX_ZDHX1);
            //Wtn6_StartRSound(WTN6_YX_ZDHX1);
            break;

        case 2:		//头顶近弹
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
    //弹药恢复设置值
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
						//被后台设置过武器弹药的情况
						
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
						//将武器弹药补满
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

    //当前时间距距上次调理时间大于10s
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
            //未上传事件响应标记清零
            memset(g_sResComTask_Mag.EvtRspFlg, 0, sizeof(g_sResComTask_Mag.EvtRspFlg));
					
            //演习模式转换事件
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_byT_Type    = IDParam_GetTType();
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_byT_Subtype = IDParam_GetTSubType();
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_byRID       = IDParam_GetRid();
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_aPID[0]     = IDParam_GetPid() >>8 ;
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_aPID[1]     = IDParam_GetPid() &0xff;
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_sL_Position = g_sUserParam.m_sLPosition;    //位置和高程
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_sChangeToManoTime = g_sUserParam.m_sUtcTime;
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_byCT_Type   	= 0xFF;
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_byCT_Subtype = 0xFF;
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_byC_RID 		  = 0x01;		//临时
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_aC_PID[0] 		= 0xFF;
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_aC_PID[1] 		= 0xFF;

						g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_sTS_Character1.m_bLocked = g_sUserParam.m_sTsChar1.m_bLocked;
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_sTS_Character1.m_bConnectionState = GetAllSlavesConnectStatus();		//各部件连接状态0：正常	1：异常
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_sTS_Character1.m_bFailureState = g_sUserParam.m_sTsChar1.m_bFailureState;		//0：无故障	1：有故障
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_sTS_Character1.m_bUnitmode     = g_sUserParam.m_sTsChar1.m_bUnitmode;		//0：训练模式	1：演习模式
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_sTS_Character1.m_bBatteryState = g_sUserParam.m_sTsChar1.m_bBatteryState;		//0：电量正常	1：欠电
            g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sChangeToMano.m_sTS_Character1.m_bIdentity     = g_sUserParam.m_sTsChar1.m_bIdentity;		//身份 ENUM_BRCHAR
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
		//销毁演习时间和方案编号
		memset(&g_sUserParam.m_sManoStart, 0, sizeof(STRUCT_TIME1));
		memset(&g_sUserParam.EP_Num, 0, 3);
		g_sResComTask_Mag.m_dwCtrlEvtNum = 0;
		g_sResComTask_Mag.UnUpLoadEvtCnt = 0;
		//未上传事件响应标记清零
		memset(g_sResComTask_Mag.EvtRspFlg, 0, sizeof(g_sResComTask_Mag.EvtRspFlg));
		memset(g_sResComTask_Mag.EvtRspFlgBak, 0, sizeof(g_sResComTask_Mag.EvtRspFlg));
		UserParam_Save();

        Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 1);
}

//白天模式处理
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

//夜晚模式处理
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

//轻伤
void BSN_Judge_SlightHandle(RCVHURTINFO_T Info)
{
    srand(9);
    Info.m_HurtBlood = 91 + rand() % 9;
    //对应血量，死亡：0，危重伤：1-40，重度伤：41-73，中度伤：74-90，轻伤：91-99，正常：100
    UpdateDamage(Info);
    Wtn6_Play(WTN6_Slight_Injury);
}

//中度伤
void BSN_Judge_ModerateHandle(RCVHURTINFO_T Info)
{
    srand(16);
    Info.m_HurtBlood = 74 + rand() % 16;
    UpdateDamage(Info);
    Wtn6_Play(WTN6_Moderate_Injury);
}

//重度伤
void BSN_Judge_SevereHandle(RCVHURTINFO_T Info)
{
    srand(32);
    Info.m_HurtBlood = 41 + rand() % 32;
    UpdateDamage(Info);
    Wtn6_Play(WTN6_Severe_Injury);
}

//危重伤
void BSN_Judge_CriticalHandle(RCVHURTINFO_T Info)
{
    srand(39);
    Info.m_HurtBlood = 1 + rand() % 39;
    UpdateDamage(Info);
		Wtn6_Play(WTN6_Injury_Worse);
}

//伤势等级加重
void BSN_Judge_WorseHandle(RCVHURTINFO_T Info)
{
    Wtn6_Play(WTN6_Injury_Worse);
    Info.m_HurtBlood = GetWorseLevelBlood(g_sUserParam.m_byBlood);
    UpdateDamage(Info);
}

uint8_t GetWorseLevelBlood(uint8_t Blood)
{
    uint8_t InjuryBlood;
	
    //对应血量，死亡：0，危重伤：1-40，重度伤：41-73，中度伤：74-90，轻伤：91-99，正常：100
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

//伤势等级减轻
void BSN_Judge_ReliefHandle(RCVHURTINFO_T Info)
{
    Info.m_HurtBlood = GetReliefBlood(g_sUserParam.m_byBlood);
    UpdateDamage(Info);
}

uint8_t GetReliefBlood(uint8_t Blood)
{
    uint8_t InjuryBlood;
    SetRand();

    //对应血量，死亡：0，危重伤：1-40，重度伤：41-73，中度伤：74-90，轻伤：91-99，正常：100
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

//后台裁决处理
uint8_t BackStageJudgeHandle(uint8_t JudgeKeyWord, RCVHURTINFO_T Info)
{
		RCVHURTINFO_T HurtInfo;
	
    switch(JudgeKeyWord)
    {
        case ACHAR_TEST:    //测试
            break;

        case ACHAR_DEATH:   //人员判死，装备击毁
            BSN_Judge_DeathHandle(Info);
            break;

        case ACHAR_REVIVE:  //复活、恢复（武器弹药保持死亡前状态）
            BSN_Judge_ReviveHandle(Info);
            break;

        case ACHAR_RESTART: //人员/装备复位
            BSN_Judge_RestartHandle(Info);
            break;

        case ACHAR_INVALID: //武器装备失效
            BSN_Judge_InvalidHandle();
            break;

        case ACHAR_TOLAST:  //恢复弹药 恢复到上一次设置弹药数
            BSN_Judge_TolastHandle();
            break;

        case ACHAR_MANO:    //演习模式
            if(g_sUserParam.m_byMode == TRAIN)
            {
                g_sUserParam.m_byJudge = TRUE;
								ManoStartHandle();
								//FullConditionRecovery();
								SaveDrillState(g_sUserParam.m_byMode, g_sUserParam.m_byEnableChangeBat);
            }
            break;

        case ACHAR_TRAIN:   //训练模式
            if(g_sUserParam.m_byMode == GAME_GOING)
            {
                g_sUserParam.m_byJudge = TRUE;
								ManoEndHandle();
								//保存状态
								SaveDrillState(g_sUserParam.m_byMode, FALSE);
            }
            break;

        case ACHAR_DAY:     //白天模式
            DayModeHandle(TRUE);
            break;

        case ACHAR_NIGHT:   //夜间模式
            NightModeHandle(TRUE);
            break;

        case ACHAR_PERSON_SLIGHT:   //人员轻伤
            BSN_Judge_SlightHandle(Info);
            break;

        case ACHAR_PERSON_MODERATE: //人员中度伤
            BSN_Judge_ModerateHandle(Info);
            break;

        case ACHAR_PERSON_SEVERE:   //人员重度伤
            BSN_Judge_SevereHandle(Info);
            break;

        case ACHAR_PERSON_CRITICAL: //人员危重伤
            BSN_Judge_CriticalHandle(Info);
            break;

        case ACHAR_ARMOUR_LOCK_WEAPON:  //锁定激光发射/作业操作功能
            BSN_Judge_InvalidHandle();
            break;

        case ACHAR_RESTOE:  //人员救护/装备维修成功，恢复战斗
            BSN_Judge_RestartHandle(Info);//等同于人员装备复位
            break;

        case ACHAR_NEAR:    //近弹
            //ShotAroundHandle(0x02);
            break;

        case ACHAR_NUCLEAR: //核威胁
            BSN_Judge_NCBHandle(NCBTTYPE_N, Info);
            break;

        case ACHAR_BIOLOGY: //化威胁
            BSN_Judge_NCBHandle(NCBTTYPE_C, Info);
            break;

        case ACHAR_CHEMISTRY: //生威胁
            BSN_Judge_NCBHandle(NCBTTYPE_B, Info);
            break;

        case ACHAR_PHOTOELE:  //光电威胁
            break;

        case ACHAR_WORSE:   //伤势等级加重
            BSN_Judge_WorseHandle(Info);
            break;

        case ACHAR_RELIEF:  //伤势等级减轻
            BSN_Judge_ReliefHandle(Info);
            break;

        case ACHAR_TO_RED:  //转为红军
            BSN_Judge_ToRedHandle();
            break;

        case ACHAR_TO_BLUE: //转为蓝军
            BSN_Judge_ToBlueHandle();
            break;

        case ACHAR_DETAIN:  //扣押
            break;

        case ACHAR_SUSPEND_DRILL:	//演习暂停
            break;

        case ACHAR_CONTINUE_DRILL:	//演习开始
            break;

        case ACHAR_RELOAD_OIL:		//重置油量
            break;

        case ACHAR_LOW_VOICE:		//微声模式
            Wtn6_NightSound();
            //BleWtn6Send(0xE2);
            break;

        case ACHAR_NORMAL_VOICE:	//正常声音模式
            Wtn6_DaySound();
            //BleWtn6Send(0xEF);
            break;

        case ACHAR_POWER_OFF:		//关机
            g_sPowerOffMag.m_bPoweroff_Force = TRUE;
            break;

        default:
            break;
    }

    return 0;
}

/****************************************************************
@FUNCTION：新后台导控裁决事件数据组包
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：LW
@SPECIAL:
****************************************************************/
void BNJudgeEventData_Spell(StructEvent_BNJudge *pMag,u8 *pBuf,u8 *Len)
{
	u8 num = 0;
	
  //组包
	pBuf[num++] = BNEventType_Judge;		//导控裁决事件
	pBuf[num++] = pMag->m_byT_Type;
	pBuf[num++] = pMag->m_byT_Subtype;
	pBuf[num++] = pMag->m_byRID;
	pBuf[num++] = pMag->m_aPID[0];
	pBuf[num++] = pMag->m_aPID[1];
	pBuf[num++] = pMag->m_byC_Idegree; //综合伤势

	memcpy(&pBuf[num],&pMag->m_sL_Position,sizeof(STRUCT_LPOS)); //经纬度高度定位
	num += sizeof(STRUCT_LPOS);
	memcpy(&pBuf[num],&pMag->m_sRecCmdTime,sizeof(STRUCT_TIME1)); //时间
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
@FUNCTION：新后台导控裁决事件上传
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：LW
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
	pFrame->m_byBuf[pFrame->m_wBufLen++] = 0; //长度
	
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
    //变量赋值
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
@FUNCTION：根据A_Type和A_Subtype获取武器弹药统一编码
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：MTY
@SPECIAL:
****************************************************************/
//获取弹药类型和子类型的统一编码
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

//获取武器类型和武器子类型的统一编码
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


//根据直瞄武器和弹药编号和受伤部位获得减少血量
uint8_t GetDirectWeaponDamage(uint16_t UniID, uint8_t Part)
{
    //判断是否为直瞄武器
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
        case 1:		//头
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
        case 18:	//胸部
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
        case 32:	//腹部
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

        case 44:	//上肢
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
        case 56:	//背部
        case 57:
        case 58:
            RePart = PARTITION_BACK;
            break;

        case 61:	//腿部
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

        default:	//腹部
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

	g_sUserParam.m_sLastDeathMag.m_fJD = 	g_sResComTask_Mag.m_sNmeaMag.m_byJD_Du + (g_sResComTask_Mag.m_sNmeaMag.m_byJD_Fen + g_sResComTask_Mag.m_sNmeaMag.m_dwJD_Fen_XiaoShu / 100000.0) / 60.0; //计算出整体经度
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
	//计算出整体纬度
	g_sUserParam.m_sLastDeathMag.m_fWD = g_sResComTask_Mag.m_sNmeaMag.m_byWD_Du + (g_sResComTask_Mag.m_sNmeaMag.m_byWD_Fen + g_sResComTask_Mag.m_sNmeaMag.m_dwWD_Fen_XiaoShu / 100000.0) / 60.0; //计算出整体纬度
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

//复活处理:1发声 2发烟 3同步状态
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

//阵亡处理:1发声 2发烟 3同步状态 4 判死事件上传
void DeathHandle(void)
{
		g_sUserParam.m_byBlood = 0;
		g_sUserParam.m_sTsChar1.m_bLocked = TRUE;
		UserParam_SetTsChar1(g_sUserParam.m_sTsChar1);
		play_Status(g_sUserParam.m_eStatus);

		//人员在车上不能点烟雾罐
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
 *函数名称: EventNoRspRestore
 *功能描述: 读取内存中未上传事件并提交
 *输入描述: 无
 *输出描述: 无
 *返回描述: 无
 *作者日期: XFW
 *全局声明: 无
 *特殊说明: 
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

//事件存储
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
		
		//写入数据
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
		
		//写入校验
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
 *函数名称: TerminalInjury_Save
 *功能描述: 保存毁伤记录
 *输入描述: 毁伤记录管理结构体，包含伤类伤部伤势等
 *输出描述: 若保存成功，毁伤记录计数自增1
 *返回描述: TRUE-成功 FALSE-失败
 *作者日期: XFW 20200429
 *全局声明:
 *特殊说明: 自动填充当前UTC时间
 ************************************************************************************************************************************************************************
*/

#define	RECORD_INJURY_MAX		20

uint32_t TerminalInjury_Save(INJURY_MAG *a_psMag)
{
    uint16_t TempLen;
    uint8_t *p = (uint8_t *)a_psMag;
    uint32_t WriteAddr;

    //填充UTC时间
    memcpy((char *)&a_psMag->m_sUtc, &g_sUserParam.m_sUtcTime, sizeof(STRUCT_TIME1));

		a_psMag->m_byChkSum = 0;
    for(TempLen = 0; TempLen < (sizeof(INJURY_MAG) - 1); TempLen++)
    {
        a_psMag->m_byChkSum += *p++;
    }

    //提交给存储设备
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
@FUNCTION：毁伤管理
@INPUT：打击方RID/PID,伤情种类，打击部位，打击血量，打击武器弹药
@OUTPUT：无
@RETURN: 无
@AUTHOR：xfw
@SPECIAL: 日志存取，支持复活操作(a_Blood=-100),不支持恢复部分血量操作
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

    //调理枪或者后台操作
    if(Info.m_HType == H_Type_I_BACKSTAGECTRL || Info.m_HType == H_Type_I_TLQCTRL)
    {
        if(Info.m_HurtBlood == 100) 	//复活
        {
            g_sUserParam.m_eStatus = (Info.m_HType == H_Type_I_BACKSTAGECTRL ? Sta_BackstageRevive :
                                     (Info.m_HType == H_Type_I_TLQCTRL ? Sta_JudgeRevive : Sta_Normal));
            ReviveHandle();
        }
        else if(Info.m_HurtBlood == 0) 	//死亡
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
						TerminalInjury_Save(&g_sInjuryMag); //毁伤记录保存
        }
        else 	//受伤
        {
            g_sUserParam.m_eStatus = Sta_Normal;
            Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 2);
            g_sUserParam.m_byBlood = Info.m_HurtBlood;
            Laser_CtrlLedToggle(0, &g_sLaserComTask_Mag);
            g_sUserParam.m_IsAid = FALSE;
            g_sHmiComTask_Mag.m_byHurtKind = HMI_HURTKIND_DT;
						g_sInjuryMag.m_byResidueBlood = g_sUserParam.m_byBlood;
						TerminalInjury_Save(&g_sInjuryMag); //毁伤记录保存
        }
    }
    else if(IsAlive(g_sUserParam.m_eStatus)) 	//还活着
    {
        switch(Info.m_HType)
        {
            case H_Type_I_NuclearChemicalBiological:          //核化生
                //减伤
                g_sUserParam.m_byBlood -= MIN(g_sUserParam.m_byBlood, Info.m_HurtBlood);

                //减伤后死亡
                if(g_sUserParam.m_byBlood == 0)
                {
                    g_sUserParam.m_eStatus = (Info.m_HType == KIND_BACKSTAGE_CTRL ? Sta_BackstageDie :
                                             (Info.m_HType == KIND_TLQ_CTRL ? Sta_JudgeDie : Sta_Die));
                    DeathHandle();
                }
                else if(g_sUserParam.m_byBlood < 100) //减伤后受伤
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

                break;														 //间瞄伤类

            case H_Type_I_ShockInjury:	   //震荡伤
                //减伤
                g_sUserParam.m_byBlood -= MIN(g_sUserParam.m_byBlood, Info.m_HurtBlood);

                //减伤后死亡
                if(g_sUserParam.m_byBlood == 0)
                {
                    g_sUserParam.m_eStatus = Sta_Die;
                    DeathHandle();
                }
                else if(g_sUserParam.m_byBlood < 100) //减伤后受伤
                {
                    Wtn6_Play(WTN6_Injury);
                    Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 2);
                    InterConn_ProtDeal_SelfStatue();
                }

                Laser_CtrlLedToggle(0, &g_sLaserComTask_Mag);
                g_sHmiComTask_Mag.m_byHurtKind = HMI_HURTKIND_ZDS;
                break;

            case H_Type_I_BulletWounded:		//枪弹伤
                //减伤
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

            case H_Type_I_Wounded:												  //炸伤
                //减伤
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

            case H_Type_I_Burn:													  //烧伤
                //减伤
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
                //减伤
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

            case H_Type_I_Dislocated:				//脱臼
                //减伤
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

            case H_Type_I_LimbSevered:			    //肢体离断
                //减伤
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

            case H_Type_I_Fracture:					// 骨折
                //减伤
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

            case H_Type_I_CollisionInjury:	        //撞击伤
                //减伤
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

            case H_Type_I_Sprain_Strain:		//扭伤/劳损
                //减伤
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

            case H_Type_I_Dislocation_Dislocation:	//错位/脱位
                //减伤
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
        //保存毁伤
        g_sInjuryMag.m_byResidueBlood = g_sUserParam.m_byBlood;
        TerminalInjury_Save(&g_sInjuryMag);
    }
    else
    {

    }
		
    //毁伤保存
		if(g_sUserParam.m_byMode == GAME_GOING)
		{
				UserParam_Save();
		}

    //演习模式下并且在配置锁定伤情情况下
    if((0 < g_sUserParam.m_byBlood) &&
            (g_sUserParam.m_byBlood <= (100 - GetInjuryBloodByHurtDegree(g_sResComTask_Mag.DrillParam.CfgFSHHurtDegree))))
    {
        //锁定发射机
        InterConn_ProtDeal_Slaver_Weapon_State(TRUE);		//武器状态同步
    }
}

/****************************************************************
@FUNCTION：新后台火力毁伤事件数据组包
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：LW
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
@FUNCTION：新后台火力毁伤事件上传
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：LW
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
		
  //重新计算
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
    //NCBTTYPE_N==核 NCBTTYPE_C==化 NCBTTYPE_B==生
    if(NCBT->m_byNCBT_Type == NCBTTYPE_N) //核
    {
        g_sResComTask_Mag.m_byNuclearFlg = (NCBT->m_byELR_Character = 1 ? 1 : 0); //1=进入，2=离开
    }
    else if(NCBT->m_byNCBT_Type == NCBTTYPE_C)  //化
    {
        g_sResComTask_Mag.m_byBiologyFlg = (NCBT->m_byELR_Character = 1 ? 1 : 0); //1=进入，2=离开
    }
    else if(NCBT->m_byNCBT_Type == NCBTTYPE_B)  //生
    {
        g_sResComTask_Mag.m_byChemistryFlg = (NCBT->m_byELR_Character = 1 ? 1 : 0); //1=进入，2=离开
    }

		g_sResComTask_Mag.NBC_Strength = NCBT->m_byStrength;
}

/****************************************************************
@FUNCTION：新后台进入/离开核化生沾染区事件数据组包
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：LW
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
@FUNCTION：新后台进入/离开核化生沾染区事件上传
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：LW
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
					
		//重新计算
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
    //TRUE = 进入，FALSE = 离开
    if(1 == HVC.ELR_Character)
    {
        if(HVC.m_byFT_Type == 0x60/*直升机*/)
        {
            g_sResComTask_Mag.SoundSimulation.HelicopterVoiceFlg = TRUE;
        }
        else if(HVC.m_byFT_Type == 0x61/*无人机*/ || HVC.m_byFT_Type == 0x62/*战斗机*/)
        {
            g_sResComTask_Mag.SoundSimulation.AeroplaneVoiceFlg = TRUE;
        }
        else  /*间苗*/
        {
            g_sResComTask_Mag.SoundSimulation.IndirectGunVoiceFlg = TRUE;
        }
    }
    else if(2 == HVC.ELR_Character)
    {
        if(HVC.m_byFT_Type == 0x60/*直升机*/)
        {
            g_sResComTask_Mag.SoundSimulation.HelicopterVoiceFlg = FALSE;
        }
        else if(HVC.m_byFT_Type == 0x61/*无人机*/ || HVC.m_byFT_Type == 0x62/*战斗机*/)
        {
            g_sResComTask_Mag.SoundSimulation.AeroplaneVoiceFlg = FALSE;
        }
        else  /*间瞄*/
        {
            g_sResComTask_Mag.SoundSimulation.IndirectGunVoiceFlg = FALSE;
        }
    }
}

/****************************************************************
@FUNCTION：新后台违规开始事件数据组包
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：MTY
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
@FUNCTION：新后台违规结束事件数据组包
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：MTY
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
@FUNCTION：新后台违规开始事件上传
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：MTY
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
			
		//重新计算
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
@FUNCTION：新后台违规结束事件上传
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：MTY
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

		//重新计算

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

    //组包
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

		//重新计算

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

    //组包
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

//伤势恶化
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

		//重新计算
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
@FUNCTION：新后台核化生毁伤事件数据组包
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：LW
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
@FUNCTION：新后台核化生毁伤事件上传
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：LW
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

		//重新计算
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
@FUNCTION：新后台演习参数设置(单兵)
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：MTY
@SPECIAL:
****************************************************************/
void BNSoldierManoParamSet(uint8_t *pBuf)
{
		Struct_CtrlManoParam_IndividualSoldierDownload ManoParam;
    uint8_t i;
    uint8_t offset = 3;
    
    uint32_t LastTimeMs, NowTimeMs;

    memset(&ManoParam, 0, sizeof(Struct_CtrlManoParam_IndividualSoldierDownload));

    //协议数据接收
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

    //防止数组越界，目前最多三个武器
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

    //RID和PID校验
    if(CheckRID(ManoParam.m_byRID) && CheckPID((ManoParam.m_aPID[0] << 8 | ManoParam.m_aPID[1])))
    {
        //避免多基站应用场景下连续收到相同指令
				LastTimeMs = g_sResComTask_Mag.m_sCmdTime.m_ulCtrlManoeuvre;
        NowTimeMs = GetSysRunMilliSecond();

        if((NowTimeMs < LastTimeMs) || ((NowTimeMs - LastTimeMs) < 2000))
        {
            return;
        }

        g_sResComTask_Mag.m_sCmdTime.m_ulCtrlManoeuvre = NowTimeMs;

        //本次接收到报文的utc时间
        RecordRcvBackStageProtocolTime(&g_sResComTask_Mag.m_sCmdTime.m_BSNulCtrlManoeuvre);
        
        //数据处理
        if(TRUE == BNSoldierManoParamSetHandle(ManoParam))
        {
						Wtn6_Play(WTN6_TERMINAL_ONLINE);
						Wtn6_Play(WTN6_MANO_PARAM_CFG_SUCCESS);
						g_sResComTask_Mag.m_IsAccess = 	TRUE;
            //事件上传
            CtrlManoParamEventUploadHandle(&ManoParam);
        }
    }
}

/****************************************************************
@FUNCTION：新后台上传周期设置
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：MTY
@SPECIAL:
****************************************************************/
void BNIntervalSet(u8 *pBuf)
{
		Struct_CtrlIntervalDownload Interval;
    uint32_t LastTimeMs, NowTimeMs;
    uint8_t offset = 3;

    //数据接收
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

    //RID和PID校验
    if(CheckRID(Interval.m_byRID) && CheckPID((Interval.m_aPID[0] << 8 | Interval.m_aPID[1])))
    {
        //避免多基站应用场景下连续收到相同指令
        LastTimeMs = g_sResComTask_Mag.m_sCmdTime.m_ulCtrlInterval;
        NowTimeMs = GetSysRunMilliSecond();

        if((NowTimeMs < LastTimeMs) || ((NowTimeMs - LastTimeMs) < 1000))
        {
            return;
        }

        g_sResComTask_Mag.m_sCmdTime.m_ulCtrlInterval = NowTimeMs;
        //本次接收到报文的utc时间
        RecordRcvBackStageProtocolTime(&g_sResComTask_Mag.m_sCmdTime.m_sBSNulCtrlInterval);

        //数据处理
        BNIntervalSetHandle(Interval);

				//数据保存,上传周期设置指令下发时，保存操作；20220722
				SaveDrillState(g_sUserParam.m_byMode, g_sUserParam.m_byEnableChangeBat);

        //事件上传
        RSP_CtrlIntervalEventUploadHandle(&Interval);
    }
}

/****************************************************************
@FUNCTION：新后台弹药物资补给
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：MTY
@SPECIAL:
****************************************************************/
void BNSupplement(u8 *pBuf)
{
		Struct_CtrlSupplementDownload Supplement;
    u8 i, offset = 3;
    uint32_t LastTimeMs, NowTimeMs;

    memset(&Supplement, 0, sizeof(Struct_CtrlSupplementDownload));

    //数据接收
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

    for(i = 0; i < Supplement.m_byChangeAmmuNum; i++) //变化的弹药种类数
    {
        Supplement.m_sChangeAmmu[i].m_byA_Type = pBuf[offset++];
        Supplement.m_sChangeAmmu[i].m_byA_Subtype = pBuf[offset++];
        Supplement.m_sChangeAmmu[i].m_aChangeAmmuNum[0] = pBuf[offset++];
        Supplement.m_sChangeAmmu[i].m_aChangeAmmuNum[1] = pBuf[offset++];
        Supplement.m_sChangeAmmu[i].m_aChangeAmmuNum[2] = pBuf[offset++];
    }

    for(i = 0; i < Supplement.m_byChangeSuppleNum; i++) //变化的物资种类数
    {
        Supplement.m_sChangeSupply[i].m_byM_Type = pBuf[offset++];
        Supplement.m_sChangeSupply[i].m_byM_Subtype = pBuf[offset++];
        Supplement.m_sChangeSupply[i].m_aRemainSupplyNum[0] = pBuf[offset++];
        Supplement.m_sChangeSupply[i].m_aRemainSupplyNum[1] = pBuf[offset++];
        Supplement.m_sChangeSupply[i].m_aRemainSupplyNum[2] = pBuf[offset++];
    }

    //RID和PID校验
    if(CheckRID(Supplement.m_byRID) && CheckPID((Supplement.m_aPID[0] << 8 | Supplement.m_aPID[1])))
    {
        //避免多基站应用场景下连续收到相同指令
        LastTimeMs = g_sResComTask_Mag.m_sCmdTime.m_ulCtrlSupplement;
        NowTimeMs = GetSysRunMilliSecond();

        if((NowTimeMs < LastTimeMs) || ((NowTimeMs - LastTimeMs) < 1000))
        {
            return;
        }

        g_sResComTask_Mag.m_sCmdTime.m_ulCtrlSupplement = NowTimeMs;

        //本次接收到报文的utc时间
        RecordRcvBackStageProtocolTime(&g_sResComTask_Mag.m_sCmdTime.m_sBSNulCtrlSupplement);

        //数据处理
        BNSupplementHandle(&Supplement);

        //事件上传
        RSP_CtrlSupplementEventUploadHandle(&Supplement);
    }
}

/****************************************************************
@FUNCTION：新后台系统裁决
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：MTY
@SPECIAL:
****************************************************************/
void BNJudge(uint8_t *pBuf)
{
	  Struct_JudgeDownload Judge;
    RCVHURTINFO_T Info = {0};
    uint32_t LastTimeMs, NowTimeMs;
    uint8_t offset = 3;

    //数据接收
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

    //RID和PID校验
    if(CheckRID(Judge.m_byRID) && CheckPID((Judge.m_aPID[0] << 8 | Judge.m_aPID[1])))
    {
        //避免多基站应用场景下连续收到相同指令
        LastTimeMs = g_sResComTask_Mag.m_sCmdTime.m_ulCtrlBloodVolume;
        NowTimeMs = GetSysRunMilliSecond();

        if((NowTimeMs < LastTimeMs) || ((NowTimeMs - LastTimeMs) < 1000))
        {
            return;
        }

        g_sResComTask_Mag.m_sCmdTime.m_ulCtrlBloodVolume = NowTimeMs;

        //本次接收到报文的utc时间
        RecordRcvBackStageProtocolTime(&g_sResComTask_Mag.m_sCmdTime.m_sBSNJudge);
        memcpy(&Judge.m_judgeTime, &g_sResComTask_Mag.m_sCmdTime.m_sBSNJudge, sizeof(STRUCT_TIME1));
        //数据处理
        Info.m_A_Type 		= 0xff;
        Info.m_A_Subtype 	= 0x01;
        Info.m_HType 		  = H_Type_I_BACKSTAGECTRL;
        Info.m_RT_Subtype = Judge.m_byMasterT_Type;
        Info.m_RT_Subtype = Judge.m_byMasterT_Subtype;
        Info.m_RRID 		= Judge.m_byMasterRID;
        Info.m_wRPID 		= Judge.m_aMasterPID[0] | Judge.m_aMasterPID[1];
        BackStageJudgeHandle(Judge.m_byAC_Character, Info);
        
				//事件上传
        BNJudgeEventUploadHandle(&Judge);
    }
}

/****************************************************************
@FUNCTION：新后台仿真模拟打击指令
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：MTY
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
	
    //数据接收
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
    IndirectAim.m_byFT_Type = pBuf[offset++];     //攻击方实体类型
    IndirectAim.m_byFT_Subtype = pBuf[offset++];
    IndirectAim.m_byF_RID = pBuf[offset++];
    IndirectAim.m_aF_PID[0] = pBuf[offset++];
    IndirectAim.m_aF_PID[1] = pBuf[offset++];
    IndirectAim.m_aFA_Type = pBuf[offset++];      //攻击方弹药类型
    IndirectAim.m_aFA_Subtype = pBuf[offset++];

    memcpy(&IndirectAim.m_sL_Position, &pBuf[offset++], sizeof(STRUCT_LPOS)) ;

    //RID和PID校验
    if(CheckRID(IndirectAim.m_byRID) && CheckPID((IndirectAim.m_aPID[0] << 8 | IndirectAim.m_aPID[1])))
    {
        if(g_sUserParam.m_byBlood == 0)
        {
            return;
        }

        //避免多基站应用场景下连续收到相同指令
        LastTimeMs = g_sResComTask_Mag.m_sCmdTime.m_ulCtrlSimulateAim;
        NowTimeMs = GetSysRunMilliSecond();

        if((NowTimeMs < LastTimeMs) || ((NowTimeMs - LastTimeMs) < 2000))
        {
            return;
        }

        g_sResComTask_Mag.m_sCmdTime.m_ulCtrlInterval = NowTimeMs;
        //本次接收到报文的utc时间
        RecordRcvBackStageProtocolTime(&g_sResComTask_Mag.m_sCmdTime.m_sBSNulCtrlInterval);

        //数据处理
        //血量
        HurtBlood = GetInjuryBloodByHurtDegree((ENUM_CI_INDIVIDUAL)IndirectAim.m_byH_Idegree);
        //统一武器ID
        GetBulletUniID(IndirectAim.m_aFA_Type, IndirectAim.m_aFA_Subtype, &UniID);		//获取武器统一编码

        //事件上传
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

        //受伤
        IndirectAim.m_byH_Part = ReverseMapHurtPart(IndirectAim.m_byH_Part);
        HMI_GetInjury(&g_sHmiComTask_Mag, IndirectAim.m_byH_Part, IndirectAim.m_byFT_Type, IndirectAim.m_byFT_Subtype);
				Wtn6_Play(WTN6_ShellHit);
        UpdateDamage(HurtInfo);
        DamageReportEventUploadHandle(&HurtInfo);
    }
}

/****************************************************************
@FUNCTION：新后台核生化威胁状态
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：MTY
@SPECIAL:
****************************************************************/
void BNNCBT(uint8_t *pBuf)
{
		Struct_NCBTDownload NCBT;
    uint32_t LastTimeMs, NowTimeMs;
    uint8_t offset = 3;
	
    //数据接收
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
    NCBT.m_byNCBT_Type = pBuf[offset++];      //核生化标识字
    NCBT.m_byStrength = pBuf[offset++];       //沾染区威胁强度
    NCBT.m_byELR_Character = pBuf[offset++];  //进入、离开核化生沾染地域标识字

    //RID和PID校验
    if(CheckRID(NCBT.m_byRID) && CheckPID((NCBT.m_aPID[0] << 8 | NCBT.m_aPID[1])))
    {
        //避免多基站应用场景下连续收到相同指令
        LastTimeMs = g_sResComTask_Mag.m_sCmdTime.m_ulCtrlInOutNBCArea;
        NowTimeMs = GetSysRunMilliSecond();

        if((NowTimeMs < LastTimeMs) || ((NowTimeMs - LastTimeMs) < 1000))
        {
            return;
        }

        g_sResComTask_Mag.m_sCmdTime.m_ulCtrlInOutNBCArea = NowTimeMs;

        //数据处理
        NCBTEvent_Handle(&NCBT);

        //事件上传
        NCBTEventUpload_Handle(&NCBT);
    }
}

void BNHelicopterVoiceCtrl(uint8_t *pBuf)
{
		Struct_HelicopterVoiceCtr HVC;
    uint32_t LastTimeMs, NowTimeMs;
    uint8_t offset = 3;

    //数据接收
    HVC.m_byFT_Type = pBuf[offset++];
    HVC.m_byFT_Subtype = pBuf[offset++];
    HVC.m_byF_RID = pBuf[offset++];
    HVC.m_aF_PID[0] = pBuf[offset++];
    HVC.m_aF_PID[1] = pBuf[offset++];
    HVC.m_byT_Type = pBuf[offset++];	//附近实体类型
    HVC.m_byT_Subtype = pBuf[offset++];	//附近实体子类型
    HVC.m_byRID = pBuf[offset++];
    HVC.m_aPID[0] = pBuf[offset++];
    HVC.m_aPID[1] = pBuf[offset++];
    memcpy(&HVC.m_sL_Position, pBuf, sizeof(STRUCT_LPOS));
    offset += sizeof(STRUCT_LPOS);
    HVC.ELR_Character = pBuf[offset++];
    HVC.m_byXor 			= pBuf[offset++];

    if(CheckRID(HVC.m_byRID) && CheckPID((HVC.m_aPID[0] << 8 | HVC.m_aPID[1] )))
    {
        //避免多基站应用场景下连续收到相同指令
        LastTimeMs = g_sResComTask_Mag.m_sCmdTime.m_ulCtrlHelicopterVoice;
        NowTimeMs = GetSysRunMilliSecond();

        if((NowTimeMs < LastTimeMs) || ((NowTimeMs - LastTimeMs) < 1000))
        {
            return;
        }

        g_sResComTask_Mag.m_sCmdTime.m_ulCtrlHelicopterVoice = NowTimeMs;
        //本次接收到报文的utc时间
        RecordRcvBackStageProtocolTime(&g_sResComTask_Mag.m_sCmdTime.m_sBSNulCtrlInterval);
        //数据处理
        BNHelicopterVoiceCtrlHandle(HVC);
        
        Wtn6_Play(WTN6_AirAttack);
        //事件上传
				//RSP_CtrVoiceEventUploadHandle(&HVC);
    }
}

/****************************************************************
@FUNCTION：新后台事件接受应答
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：MTY
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
	
  //RID和PID校验
	if(CheckRID(Responce.m_byRID) && CheckPID((Responce.m_aPID[0] << 8 | Responce.m_aPID[1])))
	{
		//如果是第一个应答。esnum=0；说明后台响应了入网申请
		if((Responce.m_byES_Num[0] << 8 | Responce.m_byES_Num[1]) == 0)
		{
			//入网成功
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

//终端参数设置
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
        g_sResComTask_Mag.DrillParam.DisableSuicide = tmnpara & 0x01;		//是否允许自杀
        g_sResComTask_Mag.DrillParam.DisableKillTeammate = (tmnpara >> 1) & 0x01;		//是否允许杀伤队友
        g_sResComTask_Mag.DrillParam.DisableCheat = (tmnpara >> 2) & 0x01;		//是否允许作弊
        g_sResComTask_Mag.DrillParam.DisableInstallFume = (tmnpara >> 3) & 0x01;		//是否安装发烟冒
        g_sResComTask_Mag.DrillParam.EnableDiffRid = (tmnpara >> 4) & 0x01;		//是否允许不同rid交战
        g_sResComTask_Mag.DrillParam.DisableAssistLaser = (tmnpara >> 5) & 0x01;		//是否允许辅助激光
        g_sResComTask_Mag.DrillParam.LocationShowType = (tmnpara >> 6) & 0x01;		//显示坐标的方式经纬度还是高斯
        g_sResComTask_Mag.DrillParam.DisableDrillStartVoice = (tmnpara >> 7) & 0x01;		//变化提示音是否关闭
        //g_sResComTask_Mag.DrillParam.CfgFSHHurtDegree = C_Lock1;		//单兵锁定发射机控制

				if(g_sResComTask_Mag.DrillParam.DisableDrillStartVoice == TRUE)
					Wtn6_NightSound();
				else
					Wtn6_DaySound();
				
				/*
				if(g_sResComTask_Mag.DrillParam.CfgFSHHurtDegree == TRUE)
					InterConn_ProtDeal_Slaver_Weapon_State(TRUE);		//武器状态同步
				else
					InterConn_ProtDeal_Slaver_Weapon_State(FALSE);		//武器状态同步
        */
				
				//保存备份区
        memcpy(&g_sResComTask_Mag.BackupArea[0].DrillParam, &g_sResComTask_Mag.DrillParam, sizeof(DRILL_PARAM));

        //计算校验和
        for(uint32_t i = 0; i < sizeof(PARAMATER_BACKUP) - 1; i++)
        {
            Xor ^= ((u8*)(&g_sResComTask_Mag.BackupArea[0]))[i];
        }
        g_sResComTask_Mag.BackupArea[0].XOR = Xor;
				
				SaveDrillState(g_sUserParam.m_byMode, g_sUserParam.m_byEnableChangeBat);      //BackupDrillState();
    }
}

//终端类型设置
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

//导控心跳下发
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

//单兵布雷事件组包
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


//单兵布雷事件上传
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
			
		//重新计算
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

//单兵排雷事件组包
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


//单兵排雷事件上传
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
			
		//重新计算
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

    if(NBCtype == NCBTTYPE_N)		//核
    {
        Info.m_HType = (ENUM_H_Type_INDIVIDUAL)NCBTTYPE_N;
        HurtBlood = 56 + rand() % 8;
    }
    else if(NBCtype == NCBTTYPE_B)		//生
    {
        Info.m_HType = (ENUM_H_Type_INDIVIDUAL)NCBTTYPE_B;
        HurtBlood = 56 + rand() % 8;
    }
    else if(NBCtype == NCBTTYPE_C)		//化
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
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sNCB_Injury.m_sL_Position     = g_sUserParam.m_sLPosition;    //位置和高程
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


//伤势恶化事件上传
void BNInjuryWorseHandle(void)
{
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sInjuryWorse.m_byT_Type = IDParam_GetTType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sInjuryWorse.m_byT_Subtype = IDParam_GetTSubType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sInjuryWorse.m_byRID = IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sInjuryWorse.m_aPID[0] = IDParam_GetPid() >>8 ;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sInjuryWorse.m_aPID[1] = IDParam_GetPid() &0xff;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sInjuryWorse.m_byC_Idegree = GetInjuryDegree(g_sUserParam.m_byBlood);
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sInjuryWorse.L_m_sPosition = g_sUserParam.m_sLPosition;    //位置和高程
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sInjuryWorse.m_sTime = g_sUserParam.m_sUtcTime;

    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;

    RSP_InjuryWorseEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}

//发烟帽未安装作弊事件上传
void FumeCheatStartUploadHandle(void)
{
		g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byT_Type    = IDParam_GetTType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byT_Subtype = IDParam_GetTSubType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byRID       = IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_aPID[0]     = IDParam_GetPid() >>8 ;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_aPID[1]     = IDParam_GetPid() &0xff;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byC_Idegree = GetInjuryDegree(g_sUserParam.m_byBlood);
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_sL_Position = g_sUserParam.m_sLPosition;    //位置和高程
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_sTime       = g_sUserParam.m_sUtcTime;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byIE_Type   = IE_SMOKEPOT_DISCONN;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;
    AgaistRuleStartEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}

//发烟帽未安装作弊事件上传
void FumeCheatEndUploadHandle(void)
{
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byT_Type    = IDParam_GetTType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byT_Subtype = IDParam_GetTSubType();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byRID       = IDParam_GetRid();
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_aPID[0]     = IDParam_GetPid() >>8 ;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_aPID[1]     = IDParam_GetPid() &0xff;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_byC_Idegree = GetInjuryDegree(g_sUserParam.m_byBlood);
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_sL_Position = g_sUserParam.m_sLPosition;    //位置和高程
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
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_sL_Position = g_sUserParam.m_sLPosition;    //位置和高程
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
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_sL_Position = g_sUserParam.m_sLPosition;    //位置和高程
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
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_sL_Position = g_sUserParam.m_sLPosition;    //位置和高程
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
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_sL_Position = g_sUserParam.m_sLPosition;    //位置和高程
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
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_sL_Position = g_sUserParam.m_sLPosition;    //位置和高程
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
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_sL_Position = g_sUserParam.m_sLPosition;    //位置和高程
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
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_sL_Position = g_sUserParam.m_sLPosition;    //位置和高程
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
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStop.m_sL_Position = g_sUserParam.m_sLPosition;    //位置和高程
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
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sIndRescued.m_sL_Position = g_sUserParam.m_sLPosition;    //位置和高程
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
@FUNCTION：后台协议解析
@INPUT：pBuf pLen - 报文
@OUTPUT：pIsCutted - 报文是否完整
@RETURN: TRUE - 成功FALSE - 失败
@AUTHOR：xfw
@SPECIAL:
****************************************************************/
uint32_t Backstage_ProtocalParse(const uint8_t *pBuf, uint16_t *pLen, uint32_t *pIsCutted)
{
    return Protocol_FindC1XORFrame(pBuf, pLen, pIsCutted, 2052 - 4);
}

//内部互联互通
uint32_t BackStage_Frame_Parse(uint8_t *pBuf, uint16_t *pLen, uint32_t *pIsCutted)
{
	  uint8_t len;

    DDL_ASSERT((pBuf != NULL) && (pLen != NULL));

    if(pBuf[0] == 0x5A)
    {
        //数据长度太短
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
@FUNCTION：新后台解析
@INPUT：
@OUTPUT：
@RETURN:
@AUTHOR：ZEG
@SPECIAL:
****************************************************************/
uint8_t BackStageNewAnalysis(uint8_t *pBuf,uint8_t len)
{
	uint8_t cmd = pBuf[1];	
	
	switch(cmd)
	{
			case BNCmd_CtrlManoParam_NotIndividualSoldier:		//演习参数设置(@非单兵)
					//BNManoParamSet(pBuf);
					break;

			case BNCmd_CtrlManoParam_IndividualSoldier:		//演习参数设置(@单兵)
					BNSoldierManoParamSet(pBuf);
					break;

			case BNCmd_CtrlInterval:		//上传周期设置指令
					BNIntervalSet(pBuf);
					break;

			case BNCmd_CtrlSupplement:		//弹药物资补给指令
					BNSupplement(pBuf);
					break;

			case BNCmd_CtrlJudge:		//主控系统裁决指令
					BNJudge(pBuf);
					break;

			case BNCmd_CtrlIndirectAim:		//仿真模拟打击指令
					BNIndirectAim(pBuf);
					break;

			case BNCmd_CtrlNCBT:		//进入/离开核化生沾染区通报指令
					BNNCBT(pBuf);
					break;

			case BNCmd_CtrlHelicopter:		//进入/离开直升机、飞机等附近通报指令
					BNHelicopterVoiceCtrl(pBuf);
					break;

			case BNCmd_BSResponce:		//事件接收应答
					BNResponce(pBuf);
					break;

			case BNCmd_CtrlTerminalParam:		//终端配置指令
					BNTerminalParamHandle(pBuf);
					break;

			case BNCmd_CtrlSetTerminalType:		//设置终端类型
					BNSetTerminalTypeHandle(pBuf);
					break;

			case BNCmd_CtrlHeartBeat:		//心跳
					HeartBeatHandle(pBuf);
					break;

			default:
					break;
	}
	return 0;
}

//裁决
void Laser_JudgeHandle(LASERCOMTASK_MAG *pMag,uint8_t IsMaster,uint8_t Sta)
{ 
	Motor_On(&g_sMotorMag, OPT_MODE_NONBLOCK, 2);
	
	if(IsMaster)
	{
		switch(Sta)
		{
			case ACHAR_TEST:		//测试
					pMag->m_TestMode = TRUE;
					LaserCom_Prot_CtrlMode(LaserComTask_HandleGet(), LASER_MODE_TEST);		//探头带进入测试模式
          Wtn6_Play(WTN6_TEST_MODEL);
					break;
			
			case ACHAR_DEATH:		//判死
					BSN_Judge_DeathHandle(pMag->m_sRcvHurtInfo);
					break;
			
			case ACHAR_REVIVE:		//判活
					BSN_Judge_ReviveHandle(pMag->m_sRcvHurtInfo);
					break;
			
			case ACHAR_RESTART:		//复位
					BSN_Judge_RestartHandle(pMag->m_sRcvHurtInfo);
					break;
			
			case ACHAR_INVALID:		//武器失效
					BSN_Judge_InvalidHandle();
					break;
			
			case ACHAR_TOLAST:  //恢复弹药 恢复到上一次设置弹药数
					BSN_Judge_TolastHandle();
					break;
			
			case ACHAR_MANO:    //演习模式
					//if(g_sUserParam.m_byMode == TRAIN)
					{
							g_sUserParam.m_byJudge = TRUE;
							ManoStartHandle();
							//保存状态
							FullConditionRecovery();
							SaveDrillState(g_sUserParam.m_byMode, g_sUserParam.m_byEnableChangeBat);
					}
					break;
					
			case ACHAR_TRAIN:		//训练模式
					//if(g_sUserParam.m_byMode == GAME_GOING)
					{
							g_sUserParam.m_byJudge = TRUE;

							ManoEndHandle();

							//保存状态
							SaveDrillState(g_sUserParam.m_byMode, FALSE);
					}
					break;
			
			case ACHAR_DAY:		//白天模式 
					DayModeHandle(TRUE);
					break;
			
			case ACHAR_NIGHT:		//夜间模式
					NightModeHandle(TRUE);
					break;    
			
			case ACHAR_PERSON_SLIGHT:		//人员轻伤
					BSN_Judge_SlightHandle(pMag->m_sRcvHurtInfo);
					break;
			
			case ACHAR_PERSON_MODERATE:		//人员中度伤
					BSN_Judge_ModerateHandle(pMag->m_sRcvHurtInfo);
					break;
			
			case ACHAR_PERSON_SEVERE:		//人员重度伤
					BSN_Judge_SevereHandle(pMag->m_sRcvHurtInfo);
					break;
			
			case ACHAR_PERSON_CRITICAL:		//人员危重伤
					BSN_Judge_CriticalHandle(pMag->m_sRcvHurtInfo);
					break;
			
			case ACHAR_ARMOUR_LOCK_WEAPON:		//锁定激光发射/作业操作功能
					BSN_Judge_InvalidHandle();
					break;
			
			case ACHAR_RESTOE:		//人员救护/装备维修成功，恢复战斗
					BSN_Judge_RestartHandle(pMag->m_sRcvHurtInfo);//等同于人员装备复位
					break;
			
			case ACHAR_NEAR:		//近弹
					ShotAroundHandle(0x02);
					break;
			
			case ACHAR_NUCLEAR:		//核威胁
					BSN_Judge_NCBHandle(NCBTTYPE_N, pMag->m_sRcvHurtInfo);
					break;
			
			case ACHAR_BIOLOGY:		//化威胁
					BSN_Judge_NCBHandle(NCBTTYPE_C, pMag->m_sRcvHurtInfo);
					break;
			
			case ACHAR_CHEMISTRY:		//生威胁
					BSN_Judge_NCBHandle(NCBTTYPE_B, pMag->m_sRcvHurtInfo);
					break;
			
			case ACHAR_PHOTOELE:		//光电威胁 
					break;
			
			case ACHAR_WORSE:		//伤势等级加重
					BSN_Judge_WorseHandle(pMag->m_sRcvHurtInfo);
					break;
			
			case ACHAR_RELIEF:		//伤势等级减轻
					BSN_Judge_ReliefHandle(pMag->m_sRcvHurtInfo);
					break;
			
			case ACHAR_TO_RED:		//转为红军
					BSN_Judge_ToRedHandle();
					break;
			
			case ACHAR_TO_BLUE:		//转为蓝军
					BSN_Judge_ToBlueHandle();
					break;
			
			case ACHAR_DETAIN:		//扣押
					break;

			case ACHAR_SUSPEND_DRILL:	//演习暂停
					break;

			case ACHAR_CONTINUE_DRILL:	//演习开始
					break;

			case ACHAR_RELOAD_OIL:		//重置油量
					break;

			case ACHAR_LOW_VOICE:		//微声模式
					Wtn6_NightSound();
					break;

			case ACHAR_NORMAL_VOICE:	//正常声音模式
					Wtn6_DaySound();
					break;
				
			case ACHAR_POWER_OFF:		//关机//55 01 01 02 02 AA
					HardCtrl_Slaver_PowerOff();
					g_sPowerOffMag.m_bPoweroffPre = TRUE;
          break;
				
			default:
				  break;			
		}			
	}
}

/****************************************************************
@FUNCTION：根据红外接收模块ID获取当前接收模块所在分区
@INPUT：
	> IsMaster: 1-主控模块(头盔) 0-背甲
	> byID: 红外接收模块编号
@OUTPUT：
	< pPartition:所在分区
@RETURN:无
@AUTHOR：
@SPECIAL:
****************************************************************/
void Laser_GetPartitionByID(u8 IsMaster, u8 byID, ENUM_PARTITION *pPartition)
{
    ENUM_PARTITION PartTemp = PARTITION_NONE;

    if(IsMaster)
    {
        PartTemp = PARTITION_HEAD;

    }
    else  //等实际情况添加分区
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

// 求弧度
double radian(double d)
{
    return d * PI / 180.0;   //角度1? = π / 180
}

//计算距离
double get_distance(double lat1/*纬度1*/, double lng1/*经度1*/, double lat2/*纬度2*/, double lng2/*经度2*/)
{
    double radLat1 = radian(lat1);
    double radLat2 = radian(lat2);
    double a = radLat1 - radLat2;
    double b = radian(lng1) - radian(lng2);

    double dst = 2 * asin((sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2) )));

    dst = dst * EARTH_RADIUS;

    dst = round(dst * 10000);	//分米
    return dst;
}

//敌方经度换算
double GetPositionLon(unsigned char* pdata)
{
    unsigned short du    = (pdata[0] & 0x0f) * 100 + ((pdata[1] >> 4) & 0x0f) * 10 + (pdata[1] & 0x0f);
    unsigned short fen   = ((pdata[2] >> 4) & 0x0f) * 10 + (pdata[2] & 0x0f);
    double fen_decimals1 = ((pdata[3] >> 4) & 0x0f) * 1.0 / 10 + (pdata[3] & 0x0f) * 1.0 / 100; //分的十分位 百分位
    double fen_decimals2 = ((pdata[4] >> 4) & 0x0f) * 1.0 / 1000 + (pdata[4] & 0x0f) * 1.0 / 10000; //分的千分位 万分位
    double lon = (du + (fen + fen_decimals1 + fen_decimals2) / 60);
    return lon;
}

//敌方纬度换算
double GetPositionLat(unsigned char* pdata)
{
    unsigned short du    = ((pdata[0] >> 4) & 0x0f) * 10 + (pdata[0] & 0x0f);
    unsigned short fen   = ((pdata[1] >> 4) & 0x0f) * 10 + (pdata[1] & 0x0f);
    double fen_decimals1 = ((pdata[2] >> 4) & 0x0f) * 1.0 / 10 + (pdata[2] & 0x0f) * 1.0 / 100; //分的十分位 百分位
    double fen_decimals2 = ((pdata[3] >> 4) & 0x0f) * 1.0 / 1000 + (pdata[3] & 0x0f) * 1.0 / 10000; //分的千分位 万分位
    double lat = (du + (fen + fen_decimals1 + fen_decimals2) / 60);
    return lat;
}


//敌方经纬度高程
double GetPositionHeight(unsigned char* pdata)
{
    double thousand = (pdata[0] & 0x0f) * 1000 + ((pdata[1] >> 4) & 0x0f) * 100 + (pdata[1] & 0x0f) * 10 \
                      + pdata[2] + pdata[2] * 1.0 / 10;
    return thousand;
}

/*武器有效射程,单位是50米*/
uint8_t WeaponDistanceJudge(uint8_t Range, STRUCT_LPOS Position)
{
		double RemoteLon, RemoteLat, LocalLon, LocalLat, distance;

    //本地经度
    LocalLon = g_sResComTask_Mag.m_sNmeaMag.m_byJD_Du + (g_sResComTask_Mag.m_sNmeaMag.m_byJD_Fen + g_sResComTask_Mag.m_sNmeaMag.m_dwJD_Fen_XiaoShu / 100000.0) / 60.0; //计算出整体经度
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
    //本地纬度
    LocalLat = g_sResComTask_Mag.m_sNmeaMag.m_byWD_Du + (g_sResComTask_Mag.m_sNmeaMag.m_byWD_Fen + g_sResComTask_Mag.m_sNmeaMag.m_dwWD_Fen_XiaoShu / 100000.0) / 60.0; //计算出整体的纬度
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
									
    //远端经度
    RemoteLon = GetPositionLon((unsigned char*)&Position);
    //远端纬度
    RemoteLat = GetPositionLat(&(((unsigned char*)&Position)[5]));
    //计算距离
    distance = get_distance(LocalLat, LocalLon, RemoteLat, RemoteLon);

    distance = distance / 10.0;	//米为单位

    if(RemoteLon < 1.0f || RemoteLat < 1.0f)	//如果敌方没有经纬度，可以打击
    {
        return TRUE;
    }

		if(fabs(distance - (float)(50 * Range)) < 5.0f)	//误差小于10米
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

    //本地经度
    LocalLon = g_sResComTask_Mag.m_sNmeaMag.m_byJD_Du + (g_sResComTask_Mag.m_sNmeaMag.m_byJD_Fen + g_sResComTask_Mag.m_sNmeaMag.m_dwJD_Fen_XiaoShu / 100000.0) / 60.0; //计算出整体经度
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
    //本地纬度
    LocalLat = g_sResComTask_Mag.m_sNmeaMag.m_byWD_Du + (g_sResComTask_Mag.m_sNmeaMag.m_byWD_Fen + g_sResComTask_Mag.m_sNmeaMag.m_dwWD_Fen_XiaoShu / 100000.0) / 60.0; //计算出整体的纬度
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
									
    //远端经度
    RemoteLon = GetPositionLon((uint8_t *)&Position);
    //远端纬度
    RemoteLat = GetPositionLat(&(((uint8_t *)&Position)[5]));
    //计算距离
    distance = get_distance(LocalLat, LocalLon, RemoteLat, RemoteLon);

    distance = distance / 10.0;	//米为单位

    return distance;
}

//判断是否为主控设备，因为单兵程序用作红缨6 /红箭73  /迫击炮的主控
uint32_t IsMaster(uint8_t TType)
{
    uint8_t ret;

    switch(TType)
    {
        case TType_PersonalEquipment:
        case TType_HJ73G_FTKDD:
        case TType_HY6_DD:
        case TType_89_82PaiJiPao: //89 式 82mm 迫击炮
        case TType_89_100PaiJiPao://89 式 100mm 迫击炮
        case TType_120PaiJiPao:   //120mm 迫击炮
        case TType_89_60PaiJiPao: //89 式 60mm 迫击炮
        case TType_82SSPaiJiPao:  //82mm 速射迫击炮
            ret = TRUE;
            break;

        default:
            ret = FALSE;
            break;
    }
    return ret;
}

//探头协议
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
	uint8_t Range, Velocity3 = 0, Thickness1 = 0, Thickness2 = 0,Belong = 0, TsCharacter2 = 0, Key = 0;		//武器有效距离
	uint16_t wFPid, Thickness16 = 0;
	
	if(len < 8)
	{
		return;  
	}
	
	//模拟装弹器补弹
	if(len == 0x0B)
	{
		//避免连续收到相同指令
		SupplyBulletLastTimeMs = pMag->m_dwSupplyBulletTimes;
		SupplyBulletNowTimeMs = GetSysRunMilliSecond();
		
		if((SupplyBulletNowTimeMs < SupplyBulletLastTimeMs) || ((SupplyBulletNowTimeMs-SupplyBulletLastTimeMs)<5000))   //裁决间隔2s
		{
				return;
		}
		Id         = pBuf[3];		//探头ID
		FA_Type    = pBuf[5];		//弹药类型
		FA_Subtype = pBuf[6];		//弹药子类型
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
					
					Wtn6_Play(WTN6_ReplenishAmmunition); //补弹成功

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

					for(u8 i=0; i<Supplement.m_byChangeAmmuNum; i++)   //变化的弹药种类数
					{
						Supplement.m_sChangeAmmu[i].m_byA_Type = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_Type;
						Supplement.m_sChangeAmmu[i].m_byA_Subtype = g_sUserParam.m_saAmmuInfo[w].m_sBulletInfo[b].m_byBullet_SubType;
						Supplement.m_sChangeAmmu[i].m_aChangeAmmuNum[0] = BulletSupply;
						Supplement.m_sChangeAmmu[i].m_aChangeAmmuNum[1] = BulletSupply>>8;
						Supplement.m_sChangeAmmu[i].m_aChangeAmmuNum[2] = BulletSupply>>16;
					}

					for(u8 i=0; i<Supplement.m_byChangeSuppleNum; i++)     //变化的物资种类数
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
	
	Id = pBuf[3];		//探头ID
	
	if(pBuf[4] != 3)		//无效
		return;
	
	FA_Type		   = pBuf[5];		//弹药类型
	FA_Subtype   = pBuf[6];		//弹药子类型
	FT_Type 		 = pBuf[7];		//武器类型
	FT_Subtype   = pBuf[8];		//武器子类型
	FRid 				 = pBuf[9];		//RID
	wFPid 			 = pBuf[10] << 8 | pBuf[11];		//PID

	TsCharacter2 = pBuf[12]; 
	memcpy(&Position, &pBuf[13], sizeof(STRUCT_LPOS) - 3);		//打击方经纬度
	
	Velocity3  = pBuf[len - 6];	
	Thickness1 = pBuf[len - 4];
	Range      = pBuf[len - 2];	
	GameMode   = (TsCharacter2 & 0x80) == 0 ? TRAIN: GAME_GOING;
	Belong     = (TsCharacter2 >> 5) & 0x03;
	Key			 	 = TsCharacter2 & 0x1F;

	//调理激光码内容
	if(len < 16)		
	{
		//避免连续收到相同指令
		LastTimeMs = pMag->m_dwLaserCtrlTimes;
		NowTimeMs = GetSysRunMilliSecond();
		
		if((NowTimeMs < LastTimeMs) || ((NowTimeMs - LastTimeMs) < 2000))   //裁决间隔2s
		{
			return;
		}

		switch(FA_Type) 
		{ 				
			//测试弹
			case 0x78:
				break;
			
			//测距
			case 0x79:		
				break;
			
			//裁决	
			case 0x7A:
				AcChar = pBuf[13];
				
				pMag->m_sRcvHurtInfo.m_RT_Type = FT_Type;
				pMag->m_sRcvHurtInfo.m_RT_Subtype = FT_Subtype;
				pMag->m_sRcvHurtInfo.m_RRID = FRid;
				pMag->m_sRcvHurtInfo.m_wRPID = wFPid;
				pMag->m_sRcvHurtInfo.m_HType = H_Type_I_TLQCTRL;
				Laser_JudgeHandle(pMag, 1, AcChar);  

				//事件上传            
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
					
		pMag->m_dwLaserCtrlTimes = GetSysRunMilliSecond();    //更新上一次收到裁决的时间
	}
	else
	{
			if(FA_Type == 0x79) 	//测距码不处理
			{
					return;
			}
		
			if((Key != (g_sUserParam.m_byKey & 0x1F)) && (g_sUserParam.m_byMode == GAME_GOING))
				return;
		
			//中弹
			if(IsAlive(g_sUserParam.m_eStatus))
			{
					pMag->m_byLaserTimer = 0;
					pMag->m_sRcvHurtInfo.m_RT_Type    = FT_Type;		//攻击方实体类型
					pMag->m_sRcvHurtInfo.m_RT_Subtype = FT_Subtype;		//攻击方实体子类型
					pMag->m_sRcvHurtInfo.m_RRID       = FRid;		//攻击方单位代字
					pMag->m_sRcvHurtInfo.m_wRPID      = wFPid;		//攻击方设备编号
					pMag->m_sRcvHurtInfo.m_A_Type     = FA_Type;		//攻击方弹药类型
					pMag->m_sRcvHurtInfo.m_A_Subtype  = FA_Subtype;		//攻击方弹药子类型
					memcpy(&pMag->m_sRcvHurtInfo.m_sFPosition, &Position, sizeof(STRUCT_LPOS) - 3);		//攻击方地理位置
					GetBulletUniID(FA_Type, FA_Subtype, &pMag->m_byUniID);		//获取武器统一编码
					Laser_GetPartitionByID(IsMaster, Id, &Part);		//获取中弹分区
					Laser_CtrlLedToggle(Id - 1, pMag);
					pMag->m_sRcvHurtInfo.m_HPart = Part;
					
					//如果是训练模式，不做任何判断，可以被打击
					if(g_sUserParam.m_byMode == TRAIN)
					{
							pMag->m_bIsShoted = TRUE;
							Laser_InsertPartitionAndID(Id, (u8)Part, pMag);		//[被击中的部位][击中该部位的激光次数]=该部位的激光ID
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
									//红箭73打击范围400-3000米
									if(0x42 == FT_Type && 0x01 == FT_Subtype)
									{
											//计算距离
											pMag->HJ73Distance = GetDistance(Position);

											//计算弹丸飞行时间
											if(Velocity3)
											{
													pMag->ButtletTime = pMag->HJ73Distance / (Velocity3 * 10) * 10; //以100ms为单位
											}

											//防御击穿
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
															pMag->DamaCalcLater = TRUE;	//延迟结算伤害
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
	
									//防御击穿
									Thickness16 = Thickness1 < 20 ? Thickness1 : (Thickness1 - 18) * 10;

									if(FT_Type == 0x43 && FT_Subtype == 0x04)
									{
											pMag->Shell[Part] = 0;
											pMag->m_bIsShoted = TRUE;
											Laser_InsertPartitionAndID(Id, (u8)Part, pMag);	//[被击中的部位][击中该部位的激光次数]=该部位的激光ID
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
											Laser_InsertPartitionAndID(Id, (u8)Part, pMag);	//[被击中的部位][击中该部位的激光次数]=该部位的激光ID
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
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_sL_Position = g_sUserParam.m_sLPosition;    //位置和高程
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_sTime       = g_sUserParam.m_sUtcTime;
    g_sResComTask_Mag.m_sEvtUploadMag.Event_Pack.m_sAgaistRuleStart.m_byIE_Type   = IE_DT_COVER;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[0] = g_sResComTask_Mag.m_dwCtrlEvtNum >> 8;
    g_sResComTask_Mag.m_sEvtUploadMag.m_aES_Num[1] = g_sResComTask_Mag.m_dwCtrlEvtNum & 0xff;
    g_sResComTask_Mag.m_dwCtrlEvtNum++;
    AgaistRuleStartEventUpload_Send(&g_sResComTask_Mag.m_sEvtUploadMag);
}

//伤势恶化，获取扣血频率
uint8_t GetBloodLossFrequency(void)
{
    ENUM_CI_INDIVIDUAL HurtDegree = GetInjuryDegree(g_sUserParam.m_byBlood);
    uint8_t frequency = 0;

    switch(HurtDegree)
    {
        case CI_SOLDIER_N13:
            frequency = 0;	//满血情况
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
            frequency = 0;	//血量为0的情况
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
//医疗救护 1min定时检查，若未被救护，伤情加重
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
        //未被救治，伤情加重
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
 *函数名称: BatteryReplacementProcess
 *功能描述: 更换电池判断和处理,启用备电
 *输入描述: 外部电池电压
 *输出描述: change bat flag
 *返回描述: TRUE提交成功/FALSE提交失败
 *作者日期: XFW
 *全局声明: 无
 *特殊说明:
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
            VeryLowFlag = 0;		//电压恢复
        }
    }
    else if(ExtBatVol > (BAT_LOW + BAT_OFFSET))
    {
        LowFlag = 0;		//电压恢复
        VeryLowFlag = 0;
        LowCnt = 0;
    }

    if((LowFlag == 0) && (VeryLowFlag == 0)) //电池电压正常
    {
        if(pEnableChangeBat != NULL)
        {
            *pEnableChangeBat = FALSE;
        }

        Led_SetRunMode(LED_MODE_BLINK_ALWAYS_OFF, &g_sStaledMag);

        g_sUserParam.m_sTsChar1.m_bBatteryState = FALSE;
    }
    else if((LowFlag == 1) && (VeryLowFlag == 0)) //电池电压降到更换电池电压
    {
        //低电量时红灯翻转
        Led_SetRunMode(LED_MODE_BLINK_1, &g_sStaledMag);

        if(pEnableChangeBat != NULL)
        {
            *pEnableChangeBat = TRUE;
        }
				g_sUserParam.m_sTsChar1.m_bBatteryState = TRUE;
    }
    else  //电池电压降到关机电压,不关机，关闭电台和探头电源
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

//返回分米
float DistanceCalcu()
{
    double NowLon = 0;
    double NowLat = 0;
    static double LastLon = 0;
    static double LastLat = 0;
    double distance = 0;


    //本地经度
		NowLon = g_sResComTask_Mag.m_sNmeaMag.m_byJD_Du + (g_sResComTask_Mag.m_sNmeaMag.m_byJD_Fen + g_sResComTask_Mag.m_sNmeaMag.m_dwJD_Fen_XiaoShu / 100000.0) / 60.0; //计算出整体经度
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
	//本地纬度	
	NowLat = g_sResComTask_Mag.m_sNmeaMag.m_byWD_Du + (g_sResComTask_Mag.m_sNmeaMag.m_byWD_Fen + g_sResComTask_Mag.m_sNmeaMag.m_dwWD_Fen_XiaoShu / 100000.0) / 60.0; //计算出整体的纬度
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
        //计算距离
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

    //要识别的姿态：站，走，跑，卧，匍匐
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
        //可能是站、走、跑
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
    g_sUserParam.m_sLPosition.m_byJD_DuHundred = pMag->m_byJD_Du/100%10;		//经度百位
    g_sUserParam.m_sLPosition.m_byJD_DuTen = pMag->m_byJD_Du/10%10;		//经度十位
    g_sUserParam.m_sLPosition.m_byJD_DuUnit = pMag->m_byJD_Du%10;		//经度个位
    g_sUserParam.m_sLPosition.m_byJD_FenTen = pMag->m_byJD_Fen/10%10;		//经度分的十位
    g_sUserParam.m_sLPosition.m_byJD_FenUnit = pMag->m_byJD_Fen%10;		//经度分的个位
    g_sUserParam.m_sLPosition.m_byJD_FenTenth = pMag->m_dwJD_Fen_XiaoShu/10000%10;		//经度分的十分位
    g_sUserParam.m_sLPosition.m_byJD_FenPercent = pMag->m_dwJD_Fen_XiaoShu/1000%10;		//经度分的百分位
    g_sUserParam.m_sLPosition.m_byJD_FenThousandth = pMag->m_dwJD_Fen_XiaoShu/100%10;		//经度分的千分位
    g_sUserParam.m_sLPosition.m_byJD_FenTenThousandth = pMag->m_dwJD_Fen_XiaoShu/10%10;		//经度分的万分位
    g_sUserParam.m_sLPosition.m_byWD_DuTen = pMag->m_byWD_Du/10%10;		//纬度十位
    g_sUserParam.m_sLPosition.m_byWD_DuUnit = pMag->m_byWD_Du%10;		//纬度个位
    g_sUserParam.m_sLPosition.m_byWD_FenTen = pMag->m_byWD_Fen/10%10;		//纬度分的十位
    g_sUserParam.m_sLPosition.m_byWD_FenUnit = pMag->m_byWD_Fen%10;		//纬度分的个位
    g_sUserParam.m_sLPosition.m_byWD_FenTenth = pMag->m_dwWD_Fen_XiaoShu/10000%10;		//纬度分的十分位
    g_sUserParam.m_sLPosition.m_byWD_FenPercent = pMag->m_dwWD_Fen_XiaoShu/1000%10;		//纬度分的百分位
    g_sUserParam.m_sLPosition.m_byWD_FenThousandth = pMag->m_dwWD_Fen_XiaoShu/100%10;		//纬度分的千分位
    g_sUserParam.m_sLPosition.m_byWD_FenTenThousandth = pMag->m_dwWD_Fen_XiaoShu/10%10;		//纬度分的万分位
	
    g_sUserParam.m_sLPosition.m_byLenthSign     = pMag->m_dwAltitude>0.0?0:1;
    g_sUserParam.m_sLPosition.m_byLenthThousand = pMag->m_dwAltitude/10000%10;		//高度千位
    g_sUserParam.m_sLPosition.m_byLenthHundred  = pMag->m_dwAltitude/1000%10;		//高度百位
    g_sUserParam.m_sLPosition.m_byLenthTen      = pMag->m_dwAltitude/100%10;		//高度十位
    g_sUserParam.m_sLPosition.m_byLenthUnit     = pMag->m_dwAltitude/10%10;		//高度个位
    g_sUserParam.m_sLPosition.m_byLenthTenth    = pMag->m_dwAltitude%10;		//高度十分位 
}

/******************************************************************
*函数名称:NMEA_GNRMC_Analysis
*功能描述:分析GNRMC信息
*输入参数:gpsx:nmea信息结构体，buf:接收到的GPS数据缓冲区首地址
*输出参数:
*返回值:                  
*其它说明:
*修改日期    版本号   修改人    修改内容
*---------------------------------------------------
*                              查飞

GPRMC

推荐最小数据量的GPS信息（RecommendedMinimumSpecificGPS/TRANSITData）

样例：

$GPRMC,161229.487,A,3723.2475,N,12158.3416,W,0.13,309.62,120598,,,A*10

格式：

$GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>*<13>

解析：

$--RMC消息ID，$GPRMC，RMC协议头。

<1>定位点的UTC时间，161229.487，格式：hhmmss.sss

<2>定位状态，A，A=定位，V=导航

<3>纬度，3723.2475，格式：ddmm.mmmm

<4>纬度方向，N，N=北纬；S=南纬

<5>经度，12158.3416，格式：dddmm.mmmm

<6>经度方向，W，W=西经；E=东经

<7>对地航速，0.13，Knots，范围：000.0--999.9，1Knot（节）=1852米/时

<8>对地航向，309.62，度，范围：000.0--359.9，以真北为参考基准。

<9>定位点的UTC日期，120598，格式：ddmmyy（日月年）

<10>磁偏角，（000.0--180.0度，前面的0也将被传输）

<11>磁偏角方向，E（东）或W（西）

<12>模式指示，A（仅NMEA-0183 3.0版本以上输出，A=自主定位，D=差分，E=估算，N=无效数据）

<13>校验和，*10

回车换行将消息结束。
******************************************************************/		
void NMEA_GNRMC_Analysis(NMEA_MAG *pMag, uint8_t *buf, uint16_t len)
{		 
	uint32_t temp;	 
	uint8_t StrLen[15], StrTemp[15][12]={0}; 
	uint8_t i, j, StrIdx = 0;

	if(0 == strncmp((char *)buf, "$GNRMC", 6))
	{
		//按照","分割字符串，字符串长度存入StrLen[]中
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

//解析传入的字符串，是否满足NMEA0183协议，若满足，返回协议字符串长度，支持判断字符串被截断的情况
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

        case Cmd_ProgramVersion: //获取版本号（新）
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

