/**
  ******************************************************************************
  * @file    metadata_ljb.h
  * @author  xfw
  * @brief   app
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 JiangSu HuaRu Defense Tech Co.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of HuaRu nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __METADATA_LJB_H
#define __METADATA_LJB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

/******************************************************************
*                           寮硅嵂绫诲瀷锛圓_Type锛?                   *
******************************************************************/



/******************************************************************
*                           裁决命令字（AC_Character）            *
描述：主控或调理终端对实体的裁决命令。
长度：1BYTE
格式：HEX

	0-	测试
	1-	判死
	2-	复活
	3-	人员/装备复位（状态恢复正常，弹药恢复设定值）
	4-	武器装备失效
	5-	恢复弹药，恢复到上一次设置弹药数
	6-	演习模式
	7-	训练模式
	8-	白天模式
	9-	夜间模式
	10-	人员轻伤
	11-	人员中度伤
	12-	人员重度伤
	13-	人员危重伤
	14-	装甲轻度受损
	15-	装甲中度受损
	16-	装甲重度受损
	17-	装甲车辆丧失火力能力
	18-	装甲车辆丧失机动能力
	19-	装甲车辆丧失火力和机动能力
	20-	锁定激光发射/作业操作功能
	21-	人员救护/装备维修成功，恢复战斗
	22-	近弹
	23-	核威胁
	24-	化威胁
	25-	生威胁
	26-	光电威胁
	27-	伤势等级加重
	28-	伤势等级减轻
	29-	转为红军
	30-	转为蓝军
	31-	扣押
	0xFF -连接超级蓝牙
******************************************************************/

typedef enum
{
		ACHAR_TEST = 0,
    ACHAR_DEATH,
    ACHAR_REVIVE,
    ACHAR_RESTART,
    ACHAR_INVALID,
    ACHAR_TOLAST = 5,
    ACHAR_MANO,
    ACHAR_TRAIN,
    ACHAR_DAY,
    ACHAR_NIGHT,
    ACHAR_PERSON_SLIGHT = 10,
    ACHAR_PERSON_MODERATE,
    ACHAR_PERSON_SEVERE,
    ACHAR_PERSON_CRITICAL,
    ACHAR_ARMOUR_SLIGHT,
    ACHAR_ARMOUR_MODERATE = 15,
    ACHAR_ARMOUR_SEVERE,
    ACHAR_ARMOUR_NOFIRE,
    ACHAR_ARMOUR_NOMOVE,
    ACHAR_ARMOUR_NO_FIRE_MOVE,
    ACHAR_ARMOUR_LOCK_WEAPON = 20,
    ACHAR_RESTOE,
    ACHAR_NEAR,
    ACHAR_NUCLEAR,
    ACHAR_BIOLOGY,
    ACHAR_CHEMISTRY = 25,
    ACHAR_PHOTOELE,
    ACHAR_WORSE,
    ACHAR_RELIEF,
    ACHAR_TO_RED,
    ACHAR_TO_BLUE = 30,
    ACHAR_DETAIN,
    ACHAR_SUSPEND_DRILL,
    ACHAR_CONTINUE_DRILL,
    ACHAR_RELOAD_OIL,
    ACHAR_LOW_VOICE,
    ACHAR_NORMAL_VOICE,
    ACHAR_POWER_OFF,
    ACHAR_ONLINETEST,
    ACHAR_SUPERBLE = 0xff,
} ENUM_ACCHAR;

/******************************************************************
*                          人员姿态状态字（BP_Character）		  *
描述：用位表示单兵的姿态。
长度：1BYTE
格式：HEX

	0	-	保留
	1	-	立姿
	2	-	卧姿
	3	-	跪姿
	4-255	保留
******************************************************************/

typedef enum
{
    BPCHAR_RES = 0,
    BPCHAR_STAND,
    BPCHAR_LIE,
    BPCHAR_KNEEL,
} ENUM_BPCHAR;

/******************************************************************
*                          人员身份标识字（BR_Character）		  *
描述：描述参与交战的实体身份。
长度：2BIT
格式：HEX

取值 	说明 	备注
00 	模拟蓝军 	蓝
01 	受训部队 	红
10 	群众 	黄
11 	调理人员或红十字医务人员 	白

******************************************************************/

typedef enum
{
    BRCHAR_BLUE = 0,
    BRCHAR_RED,
    BRCHAR_YELLOW,
    BRCHAR_WHITE,
} ENUM_BRCHAR;

/******************************************************************
*						   综合伤势（C_Idegree）				  *
描述：与实体类型 T_Type 组合使用，用于描述该类设备的当前综合毁伤等级。
长度：1BYTE
格式：HEX
******************************************************************/


/***********************************************************
实体类型		C_Idegree	伤势等级			备注

装甲类			0			击毁				K 级
（0x01-0x12） 1			机动和火力能力均丧失 M_F 级
				2			丧失机动能力		M 级
				3			丧失火力能力		F 级
				4			防护装甲重度受损
				5			防护装甲中度受损
				6			防护装甲轻度受损
				7			正常
************************************************************/
typedef enum
{
    CI_ARMOR_K = 0,
    CI_ARMOR_M_F,
    CI_ARMOR_M,
    CI_ARMOR_F,
    CI_ARMOR_S,
    CI_ARMOR_ME,
    CI_ARMOR_L,
    CI_ARMOR_N,
} ENUM_CI_ARMOR;




/***********************************************************
实体类型 		C_Idegree 	伤势等级 	备注

单兵类（0x13） 	0 			死亡 		death
				1-5 		危重伤      critical
				6-9 		重度伤
				10-11 		中度伤
				12 			轻伤
				13 			正常

************************************************************/

typedef enum
{
    CI_SOLDIER_D0 = 0,
    CI_SOLDIER_C1,
    CI_SOLDIER_C2,
    CI_SOLDIER_C3,
    CI_SOLDIER_C4,
    CI_SOLDIER_C5,
    CI_SOLDIER_S6,
    CI_SOLDIER_S7,
    CI_SOLDIER_S8,
    CI_SOLDIER_S9,
    CI_SOLDIER_ME10,
    CI_SOLDIER_ME11,
    CI_SOLDIER_L12,
    CI_SOLDIER_N13,
} ENUM_CI_INDIVIDUAL ;


//H_Idegree
typedef enum
{
    HI_SOLDIER_D0 = 0,   //死
    HI_SOLDIER_C1,   //危
    HI_SOLDIER_C2,   //危
    HI_SOLDIER_C3,   //危
    HI_SOLDIER_C4,   //危
    HI_SOLDIER_C5,   //危
    HI_SOLDIER_S6,   //重
    HI_SOLDIER_S7,   //重
    HI_SOLDIER_S8,   //重
    HI_SOLDIER_S9,   //重
    HI_SOLDIER_ME10, //中
    HI_SOLDIER_ME11, //中
    HI_SOLDIER_L12,  //轻
    HI_SOLDIER_N13,  //无
} ENUM_HI_INDIVIDUAL_T;

/***********************************************************
实体类型 		C_Idegree 	伤势等级 	备注

其他类 			0 			击毁
				1 			击伤
				2 			正常
************************************************************/

typedef enum
{
    CI_OTHER_D = 0,
    CI_OTHER_I,
    CI_OTHER_N,
} ENUM_CI_OTHER;


/******************************************************************
*                          人员救治结果标识字（CE_Character）	  *
描述：表示人员被救治后的效果
长度：1BYTE
格式：HEX

取值 	说明 				备注
0 		保留
1 		救治得当，恢复战斗 	 Recovery
2 		救治成功，不再恶化 	 Hold
3 		救治失败，伤情恶化 	 Worse
4 		救治失败，死亡 	 	 Death
5-255 	保留
******************************************************************/

typedef enum
{
    CECHAR_RES = 0,
    CECHAR_R,
    CECHAR_H,
    CECHAR_W,
    CECHAR_D,
} ENUM_CECHAR;

/******************************************************************
*                          救治方式标识字（CM_Character）		  *
描述：医护人员救治方式。
长度：1BYTE
格式：HEX


取值 	说明 			备注
0 		保留
1 		自救
2 		互救
3 		救护人员现场救治
4 		救护人员急救后送
5-255 	保留

******************************************************************/

typedef enum
{
    CMCHAR_RES = 0,
    CMCHAR_SAVE_ONESELF,
    CMCHAR_MUTUAL_HELP,
    CMCHAR_SCENE,
    CMCHAR_EMERGENCY,
} ENUM_CMCHAR;


/******************************************************************
*                          方位（Direction）					  *
描述：表示前、后、左、右、上、下 6 个方位。
长度：1BYTE
格式：HEX


取值 	说明 	备注
0 		保留
1 		前
2 		后
3 		左
4 		右
5 		上
6 		下
7-255 	保留

******************************************************************/

typedef enum
{
    DIR_RES = 0,
    DIR_FRONT,
    DIR_BACK,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_UP,
    DIR_DOWN,
} ENUM_DIR;


/******************************************************************
*                       进入/离开危险地域标识字（ELR_Character）  *
描述：识别进入或离开某一危险地域（雷场、核化生沾染区等）。
长度：1BYTE
格式：HEX

取值 	说明 	备注
0 		保留
1 		进入
2 		离开
3-255 	保留
******************************************************************/

typedef enum
{
    ELRCHAR_RES = 0,
    ELRCHAR_ENTER,
    ELRCHAR_LEAVE,
} ENUM_ELRCHAR;


/******************************************************************
*                      引信类型（Fuse_Type）						 *
描述：表示弹药的引信类型。
长度：1BYTE
格式：HEX

取值 	说明 		备注
0 		保留
1 		短延期引信
2 		延期引信
3 		瞬发引信
4 		时间引信
5 		空炸引信
6-255 	保留
******************************************************************/

typedef enum
{
    FUSE_TYPE_RES = 0,
    FUSE_TYPE_SHORT_DELAY,
    FUSE_TYPE_DELAY,
    FUSE_TYPE_NONDELAY,
    FUSE_TYPE_TIME,
    FUSE_TYPE_AIRBLAST,
} ENUM_FUSE_TYPE;

/******************************************************************
*                      装药号（G_Num）							 *
描述：表示间瞄弹药的装药号。
长度：1BYTE
格式：HEX

取值 	说明 		备注
0 		0号装药
1 		1号装药
2 		2号装药
3 		3号装药
4 		4号装药
5 		5号装药
6 		6号装药
7-255 	保留

******************************************************************/

typedef enum
{
    GNUM_0 = 0,
    GNUM_1,
    GNUM_2,
    GNUM_3,
    GNUM_4,
    GNUM_5,
    GNUM_6,
} ENUM_GNUM;

/******************************************************************
*                      高斯坐标（G_Position）					  *
描述：设备高斯坐标 X、高斯坐标 Y 和高斯坐标高程。
长度：9BYTE
格式：HEX（补码）

字节序号 	内容 	备注
1-3 		高斯坐标 X
4-7 		高斯坐标 Y
8-9 		坐标高程
******************************************************************/

//__packed typedef struct
//{
//	int32_t m_dwX;
//	int32_t m_dwY;
//	int16_t m_wH;
//}STRUCT_GPOS;

/******************************************************************
*                      违规事件子类型（IE_Type）				  *
描述：表示违规操作的类型。
长度：1BYTE
格式：HEX

取值 	说明 			备注
0 		保留
1 		遮挡探头作弊
2 		探头未连接
3 		警灯未连接
4 		二次毁伤装置未连接
5 		发烟罐未安装
6 		声光弹未安装
7 		击毁/死亡后运动
8 		丧失机动能力后运动
9 		单兵头盔未开机
10 		违规关机
11 		火力点移动
12-255 	保留
******************************************************************/

typedef enum
{
    IE_RES = 0,
    IE_DT_COVER,
    IE_DT_DISCONN,
    IE_ALMLED_DISCONN,
    IE_SECONDDAMAGE_DISCONN,
    IE_SMOKEPOT_DISCONN,
    IE_SGD_DISCONN,
    IE_MOVE_SOLDIER,
    IE_MOVE_ARMOR,
    IE_HELMET_DISCONN,
    IE_POWEROFF,
    IE_MOVE_FIREPOINT,
} ENUM_IE_TYPE;

/******************************************************************
*                      弹药 (物资)补给标识字（IO_Character）	  *
描述：标识物资(弹药)补给或取出操作。
长度：1BYTE
格式：HEX


取值 	说明 						备注
0 		保留
1 		弹药/物资被取出（数量减少）
2 		弹药/物资得到补给（数量增加）
3-255 	保留

******************************************************************/

typedef enum
{
    IOCHAR_RES = 0,
    IOCHAR_OUT,
    IOCHAR_IN,
} ENUM_IOCHAR;


/******************************************************************
*                     经纬度坐标（L_Position）					  *
描述：设备经度、纬度、高度以及定位状态。
长度：12BYTE
格式：BCD


字节序号 	内容 	位段 	备注
1 			经度 	bit7 	1：已定位；0：未定位
					bit6-bit4 	保留
					bit3-bit0 	度的百位
2 					bit7-bit4 	度的十位
					bit3-bit0 	度的个位
3 					bit7-bit4 	分的十位
					bit3-bit0 	分的个位
4 					bit7-bit4 	分的十分位
					bit3-bit0 	分的百分位
5 					bit7-bit4 	分的千分位
					bit3-bit0 	分的万分位

6 			纬度  	bit7-bit4 	度的十位
					bit3-bit0 	度的个位
7 					bit7-bit4 	分的十位
					bit3-bit0 	分的个位
8 					bit7-bit4 	分的十分位
					bit3-bit0 	分的百分位
9 					bit7-bit4 	分的千分位
					bit3-bit0 	分的万分位

10 			高度  	bit7 		0：+；1：-
		（单位:米）	bit3-bit0 	千位
11 					bit7-bit4 	百位
					bit3-bit0 	十位
12 					bit7-bit4 	个位
					bit3-bit0 	十分位

******************************************************************/

//__packed typedef struct
//{
//	uint8_t m_IsLocation:1;
//	uint8_t m_bRes1:3;
//	uint8_t m_bJDDu_Bai:4;
//	uint8_t m_bJDDu_Shi:4;
//	uint8_t m_bJDDu_Ge:4;
//	uint8_t m_bJDFen_Shi:4;
//	uint8_t m_bJDFen_Ge:4;
//	uint8_t m_bJDFen_SF:4;
//	uint8_t m_bJDFen_BF:4;
//	uint8_t m_bJDFen_QF:4;
//	uint8_t m_bJDFen_WF:4;
//
//	uint8_t m_bWDDu_Shi:4;
//	uint8_t m_bWDDu_Ge:4;
//	uint8_t m_bWDFen_Shi:4;
//	uint8_t m_bWDFen_Ge:4;
//	uint8_t m_bWDFen_SF:4;
//	uint8_t m_bWDFen_BF:4;
//	uint8_t m_bWDFen_QF:4;
//	uint8_t m_bWDFen_WF:4;
//
//	uint8_t m_IsNegative:1;
//	uint8_t m_bRes2:3;
//	uint8_t m_bH_Qian:4;
//	uint8_t m_bH_Bai:4;
//	uint8_t m_bH_Shi:4;
//	uint8_t m_bH_Ge:4;
//	uint8_t m_bH_SF:4;
//}STRUCT_LPOS;

/******************************************************************
*                      链接标识字（L_Character）				  *
描述：表示链接状态。
长度：1BYTE
格式：HEX

取值 	说明 		备注
0 		保留
1 		链接，欠电
2 		链接，不欠电
3 		断链
4-255 	保留

******************************************************************/

typedef enum
{
    LCHAR_RES = 0,
    LCHAR_LINK_LACK,
    LCHAR_LINK_UNLACK,
    LCHAR_DISLINK,
} ENUM_LCHAR;



/******************************************************************
*                      光电威胁类型（LT_Type）					  *
描述：表示实体受到的光电威胁类型。
长度：1BYTE
格式：HEX

取值 	说明 	备注
0 	保留
1 	测距激光威胁
2 	照射激光威胁
3-255 	保留
******************************************************************/

typedef enum
{
    LTTYPE_RES = 0,
    LTTYPE_RANGE,
    LTTYPE_SHINE,
} ENUM_LTTYPE;

/******************************************************************
*                      物资类型 			       				  *
******************************************************************/

/******************************************************************
*                      燃油物资类型 			       			*

M_Subtype 取值 	物资子类型 	备注
1 				柴油
2 				汽油
3 				航空燃油
******************************************************************/

typedef enum
{
    M_OIL_DIESEL = 1,
    M_OIL_GASOLINE,
    M_OIL_JET,
} ENUM_M_OIL;

/******************************************************************
*                     军需物资			       			*

M_Subtype 取值 	物资子类型 	备注
1 				军被
2 				帐篷
3 				纱布
******************************************************************/

typedef enum
{
    M_QUARTERMASTER_QUILT = 1,
    M_QUARTERMASTER_TENT,
    M_QUARTERMASTER_GAUZE,
} ENUM_M_QUARTERMASTER;



/******************************************************************
*                     扫雷方式标识字（MCM_Character） 			*

取值 	说明 	备注
0 		保留
1 		犁扫
2 		磁扫
3 		压发扫雷
4-255 	保留

******************************************************************/

typedef enum
{
    MCMCHAR_RES = 0,
    MCMCHAR_PLOUGH,
    MCMCHAR_MAGNET,
    MCMCHAR_PRESS,
} ENUM_MCMCHAR;


/******************************************************************
*                   核化生防护事件子类型（NCBP_Type） 			*

描述：表示实体实施的核化生防护类型。
长度：1BYTE
格式：HEX

取值 	说明 		备注
0 		保留
1 		打开三防装置
2 		关闭三防装置
3 		佩戴防毒面具
4 		摘下防毒面具
5-255 	保留

******************************************************************/

typedef enum
{
    NCBPTYPE_RES = 0,
    NCBPTYPE_OPEN_THREEPROOF,
    NCBPTYPE_CLOSE_THREEPROOF,
    NCBPTYPE_WEAR_MASK,
    NCBPTYPE_TAKEOFF_MASK,
} ENUM_NCBPTYPE;



/******************************************************************
*                   核化生标识字（NCBT_Type）			 			*

描述：表示核化生威胁、防护的具体类型
长度：1BYTE
格式：HEX


取值 	说明 	备注
0 		保留
1 		核
2 		化
3 		生
4-255 	保留

******************************************************************/

typedef enum
{
    NCBTTYPE_RES = 0,
    NCBTTYPE_N,
    NCBTTYPE_C,
    NCBTTYPE_B,
} ENUM_NCBTTYPE;

/******************************************************************
*                 开/关机操作标识字（PO_Character）		 			*

描述：表示开机或关机操作。
长度：1BYTE
格式：HEX

取值 	说明 	备注
0 		保留
1 		不欠电关机
2 		欠电关机
3 		开机
4-255 	保留
******************************************************************/

typedef enum
{
    POCHAR_RES = 0,
    POCHAR_POWEROFF_UNLACK,
    POCHAR_POWEROFF_LACK,
    POCHAR_POWERON,
} ENUM_POCHAR;


/******************************************************************
*                 单位代字（RID）						 			*

描述：终端设备使用单位编号。
长度：1BYTE
格式：HEX


取值 	说明 		备注
0 		保留
1 		朱日和基地
2 		三界基地
3 		XXX 单位
4 		XXX 单位
5 		XXX 单位
6 		XXX 单位
… 		…
128-255 保留

******************************************************************/

typedef enum
{
    RID_RES = 0,
    RID_ZRH,
    RID_SJ,
} ENUM_RID;

/******************************************************************
*                火炮射击模式标识字（RM_Character）	 			*

描述：表示火炮的射击模式。
长度：1BYTE
格式：HEX

取值 	说明 		备注
0 		保留
1 		高射界射击
2 		低射界射击
3-255 	保留

******************************************************************/

typedef enum
{
    RMCHAR_RES = 0,
    RMCHAR_H,
    RMCHAR_L,
} ENUM_RMCHAR;

/******************************************************************
*                侦察/校射模式标识字（SAM_Character）	 			*

描述：用于识别侦察/校射雷达的当前实体状态字 1。
长度：1BYTE
格式：HEX

取值 	说明 	备注
0 		保留
1 		侦察
2 		校射
3-255 	保留

******************************************************************/

typedef enum
{
    SAMCHAR_RES = 0,
    SAMCHAR_SCOUT,
    SAMCHAR_AMENDING,
} ENUM_SAMCHAR;

/******************************************************************
*                光电防护事件子类型（SIP_Type）						*

描述：表示实体实施烟雾/诱导防护事件的具体类型。
长度：1BYTE
格式：HEX

取值 	说明 		备注
0 		保留
1 		烟雾弹防护
2 		激光诱偏防护
3-255 	保留
******************************************************************/

typedef enum
{
    SIPTYPE_RES = 0,
    SIPTYPE_SMOG,
    SIPTYPE_LASER,
} ENUM_SIPTYPE;

/******************************************************************
*                开/停操作标识字（SS_Character）				*

描述：用于识别实体开始工作和停止工作操作动作。
长度：1BYTE
格式：HEX

取值 	说明 	备注
0 		保留
1 		开始
2 		停止
3-255 	保留

******************************************************************/

typedef enum
{
    SSCHAR_RES = 0,
    SSCHAR_START,
    SSCHAR_STOP,
} ENUM_SSCHAR;

/******************************************************************
*                设置结果标识字（SR_Character）				*

描述：表示设置结果。
长度：1BYTE
格式：HEX

取值 	说明 	备注
0 		失败
1 		成功
2-255 	保留


******************************************************************/

typedef enum
{
    SRCHAR_FAILURE = 0,
    SRCHAR_SUCCESS,
} ENUM_SRCHAR;


/******************************************************************
*            实体类型				*

描述：表设备类别。
长度：1BYTE
格式：HEX


******************************************************************/
typedef enum
{
    T_TYPE_Tank = 0X01,								//坦克（非火控模拟）
    T_TYPE_TankE,									//坦克（嵌入式模拟）
    T_TYPE_InfantryFightingVehicle,					//步战车
    T_TYPE_ArmouredCarrier,							//装甲输送车
    T_TYPE_RaiderBuggy, 							//突击车
    T_TYPE_AssaultGun, 								//突击炮
    T_TYPE_AntiTankMissileSystem,					//反坦克导弹系统
    //0x08-0x0F 	保留

    T_TYPE_TowCannon = 0X10, 						//牵引火炮
    T_TYPE_NonDigitalSelfPropelledArtillery, 		//自行火炮（非数字化）
    T_TYPE_DigitalSelfPropelledArtillery,			//自行火炮（数字化)
    T_TYPE_Rocket,									//火箭炮
    T_TYPE_Mortar,									//迫击炮
    T_TYPE_ArtilleryReconnaissance,					//炮兵侦察
    //0x16-0x1F  	保留

    T_TYPE_AntiAircraftGun = 0X20,					//高炮（有火控）
    T_TYPE_NoFireControlAntiAircraftGun,			//高炮（无火控）
    T_TYPE_AntiAircraftMissileSystem,				//防空导弹系统
    T_TYPE_AntiAircraftRadarSystem,					//防空雷达系统
    T_TYPE_AntiAircraftCommandSystem,				//防空指挥系统
    T_TYPE_ManAntiAircraftMissile,					//单兵防空导弹
    //0x26-0x2F 	保留

    T_TYPE_Personnel = 0X30,						//人员
    T_TYPE_AutomaticRifle,							//自动步枪
    T_TYPE_MachineGun,								//班用机枪
    T_TYPE_SniperRifle,								//狙击步枪
    T_TYPE_SubmachineGun,							//微声冲锋枪
    T_TYPE_ShotGun,									//霰弹枪
    T_TYPE_Pistol,									//手枪
    T_TYPE_PrivateGrenadeLauncher,					//单兵榴弹发射器
    T_TYPE_HeavyMachineRifle,							//重机枪
    T_TYPE_AntiAircraftRifle,							//高射机枪
    T_TYPE_OpticalReconnaissanceEquipment,			//光学侦察设备
    //0x3B-0x3F 	保留

    T_TYPE_IndividualRocket = 0X40,					//单兵火箭
    T_TYPE_ManAntiTankRocket,						//单兵反坦克火箭
    T_TYPE_ManAntiTankmissile,						//单兵反坦克导弹
    T_TYPE_Mine,									//地雷
    T_TYPE_PrivateMineDeminer,						//单兵排雷器
    T_TYPE_HandGrenade,								//手雷
    T_TYPE_ImprovisedExplosive,						//简易爆炸物
    T_TYPE_MedicalServiceAndAmbulanceEquipment,					//卫勤救护装备
    T_TYPE_ConditioningEquipment,					//调理设备
    //0x49-0x4F  	保留

    T_TYPE_GmVehicle = 0X50, 						//通用车辆
    T_TYPE_CommandVehicle,							//指挥车辆
    T_TYPE_CommunicationVehicle,					//通信车辆
    T_TYPE_AmmunitionCarrier,						//弹药输送车
    T_TYPE_OilVehicle,								//油料运输车
    T_TYPE_RocketMineVehicle,						//火箭布雷车
    T_TYPE_EngineeringReconnaissanceVehicle,		//工程侦察车
    T_TYPE_LightObstructorVehicle,					//轻型破障车
    T_TYPE_IntegratedMinesweeperVehicle,			//综合扫雷车
    T_TYPE_HeavyMechanizedBridgeVehicle,			//重型机械化桥
    T_TYPE_EmergencyVehicle,						//抢修车辆
    T_TYPE_EngineeringVehicle,						//工程作业车辆
    //0x5C-0x5F  	保留

    T_TYPE_Helicopter = 0X60,						//直升机
    T_TYPE_Drone,									//无人机
    T_TYPE_Bomber,									//战斗机/轰炸机
    T_TYPE_ElectronicCountermeasure,				//电子对抗
    T_TYPE_FortificationsFacilities,				//工事设施
    T_TYPE_BattlefieldObstacle,						//战场障碍
    //0x66-0xFE 	保留

    T_TYPE_MasterStation = 0XFF,					//主控台

} ENUM_T_TYPE;




typedef enum
{
    T_Subtype_Tank96 = 0X01,						//96 式坦克
    T_Subtype_Tank88B,								//88B 式坦克
    T_Subtype_Tank62,								//62 式坦克
    T_Subtype_Tank59,								//59 式坦克
    T_Subtype_TankM60A3, 							//M60A3 坦克
    T_Subtype_Tank63A, 								//63A 式坦克
    T_Subtype_Tank59G,								//59 改式坦克
    T_Subtype_TankM1A1,								//M1A1 坦克
    T_Subtype_Tank99,								//99 式坦克
    T_Subtype_Tank96A,								//96A 式坦克
} ENUM_T_Subtype_Tank;

typedef enum
{
    T_Subtype_TankE96A   = 0X01,					//96A 嵌入式坦克
} ENUM_T_Subtype_TankE;

typedef enum
{
    T_Subtype_InfantryFightingVehicle86 = 0X01,							//86 式步战车
    T_Subtype_InfantryFightingVehicle92,								//92 轮式步战车
    T_Subtype_InfantryFightingVehicle04,								//04 式步战车
    T_Subtype_InfantryFightingVehicle05,								//05 式步战车
    T_Subtype_InfantryFightingVehicle05A, 								//05 式两栖装甲步兵战车
    T_Subtype_InfantryFightingVehicle08, 								//08 式轮式步战车

} ENUM_T_Subtype_InfantryFightingVehicle;

typedef enum
{
    T_Subtype_ArmouredCarrier63C = 0X01,								//63C 式装甲输送车
    T_Subtype_ArmouredCarrier001,										//001 型两栖装甲输送车
    T_Subtype_ArmouredCarrier89,										//89 式装甲输送车
    T_Subtype_ArmouredCarrier10,										//10 式装甲输送车
} ENUM_T_Subtype_ArmouredCarrier;

typedef enum
{
    T_Subtype_RaiderBuggy05 = 0X01,										//05 式突击车
    T_Subtype_RaiderBuggy11,											//11 式突击车
    T_Subtype_RaiderBuggy03,											//03 式突击车
    T_Subtype_RaiderBuggy05A,											//05 式两栖装甲突击车
} ENUM_T_Subtype_RaiderBuggy;

typedef enum
{
    T_Subtype_AssaultGun100 = 0X01,										//100mm 轮式突击炮
    T_Subtype_AssaultGun120,											//120mm 反坦克滑膛炮
} ENUM_T_Subtype_AssaultGun;

typedef enum
{
    T_Subtype_AntiTankMissileSystemRedArrow9 = 0X01,					//红箭 9 反坦克导弹系统
    T_Subtype_AntiTankMissileSystemRedArrow10,							//红箭 10 反坦克导弹系统
} ENUM_T_Subtype_AntiTankMissileSystem;

typedef enum
{
    T_Subtype_TowCannon122 = 0X01,										//PL83/96 式 122 榴弹炮
    T_Subtype_TowCannon130,												//PL59-1 式 130 加农炮
    T_Subtype_TowCannon152,												//PL66 式 152 加榴炮
} ENUM_T_Subtype_TowCannon;

typedef enum
{
    T_Subtype_NonDigitalSelfPropelledArtillery122 = 0X01,				//PLZ89 式 122 榴弹炮
    T_Subtype_NonDigitalSelfPropelledArtillery152,						//PLZ83 式 152 加榴炮
} ENUM_T_Subtype_NonDigitalSelfPropelledArtillery;

typedef enum
{
    T_Subtype_DigitalSelfPropelledArtillery155 = 0X01,					//PLZ05 式 155 加榴炮
    T_Subtype_DigitalSelfPropelledArtilleryZ122,						//PLZ07 式 122 榴弹炮
    T_Subtype_DigitalSelfPropelledArtilleryC122,						//PLC09 式 122 榴弹炮
    T_Subtype_DigitalSelfPropelledArtillery120,							//PLL05 式 120mm 自行迫榴炮
    T_Subtype_DigitalSelfPropelledArtilleryA, 							//两栖自行榴弹炮

} ENUM_T_Subtype_DigitalSelfPropelledArtillery;

typedef enum
{
    T_Subtype_RocketL122 = 0X01,										//PHL81 式 122 火箭炮
    T_Subtype_RocketZ122,												//PHZ89 式 122 火箭炮
    T_Subtype_Rocket300,												//PHZ05 式 300mm 远程火箭炮
} ENUM_T_Subtype_Rocket;



typedef enum
{
    T_Subtype_Mortar82 = 0X01,											//89 式 82mm 迫击炮
    T_Subtype_Mortar100,												//89 式 100mm 迫击炮
    T_Subtype_Mortar120,												//120mm 迫击炮
    T_Subtype_Mortar60, 												//89 式 60mm 迫击炮
    T_Subtype_Mortar82S,												//82mm 速射迫击炮

} ENUM_T_Subtype_Mortar;


typedef enum
{
    T_Subtype_ArtilleryReconnaissanceRadar = 0X01,						//炮位侦察雷达
    T_Subtype_ArtilleryReconnaissanceVehicle,							//炮兵侦察车
    T_Subtype_ArtilleryReconnaissanceMobileTargetRadar,				//炮兵活动目标侦察雷达
    T_Subtype_ArtilleryReconnaissanceBlastPointIndicator, 				//炮兵侦察用模拟炸点指示器
} ENUM_T_Subtype_ArtilleryReconnaissance;


typedef enum
{
    T_Subtype_AntiAircraftGunPZK091A35 = 0X01,							//PZK091A35 毫米高炮战车
    T_Subtype_AntiAircraftGunPG99,										//PG99 式 35 毫米高炮数据采集设备
    T_Subtype_AntiAircraftGunPGZ04,									//PGZ04 型弹炮结合武器系统战车
    T_Subtype_AntiAircraftGunPLB625, 									//PLB625 弹炮结合武器战车
} ENUM_T_Subtype_AntiAircraftGun;

typedef enum
{
    T_Subtype_NoFireControlAntiAircraftGun37 = 0X01,					//双 37 高炮
    T_Subtype_NoFireControlAntiAircraftGun25,							//双 25 高炮
    T_Subtype_NoFireControlAntiAircraftGun25S,							//四－25 高炮
    T_Subtype_NoFireControlAntiAircraftGun57,							//57mm 高炮

} ENUM_T_Subtype_NoFireControlAntiAircraftGun;

typedef enum
{
    T_Subtype_AntiAircraftMissileSystemRedFlag17S = 0X01,				//红旗 17 导弹武器系统 S 车
    T_Subtype_AntiAircraftMissileSystemRedFlag17F,						//红旗 17 导弹武器系统 F 车
    T_Subtype_AntiAircraftMissileSystemRedFlag16AD,					//红旗 16A/D 导弹武器系统数据采集设备
    T_Subtype_AntiAircraftMissileSystemRedFlag7BS,						//红旗 7B 导弹武器 S 车系统数据采集设备
    T_Subtype_AntiAircraftMissileSystemRedFlag7BF, 						//红旗 7B 导弹武器 F 车系统数据采集设备

} ENUM_T_Subtype_AntiAircraftMissileSystem;



typedef enum
{
    T_Subtype_AntiAircraftRadarSystemLLP12A = 0X01,						//LLP12A 火控雷达数据采集设备
    T_Subtype_AntiAircraftRadarSystem,									//情报雷达数据采集设备（标准型）

} ENUM_T_Subtype_AntiAircraftRadarSystem;

typedef enum
{
    T_Subtype_AntiAircraftCommandSystemBrigadeStation = 0X01,			//指挥信息系统旅（营）站数据采集设备
    T_Subtype_AntiAircraftCommandSystemBatteryStation,					//指挥信息系统高炮连站数据采集设备

} ENUM_T_Subtype_AntiAircraftCommandSystem;

typedef enum
{
    T_Subtype_ManAntiAircraftMissileRedTassel5 = 0X01,					//红缨 5
    T_Subtype_ManAntiAircraftMissileRedTassel6,							//红缨 6 便携式防空导弹
    T_Subtype_ManAntiAircraftMissileAvantGarde2,						//前卫 2 便携式防空导弹
    T_Subtype_ManAntiAircraftMissileRedTassel6B,						//红缨 6 便携式导弹
    T_Subtype_ManAntiAircraftMissileAvantGarde2B, 						//前卫 2 便携式防空导弹

} ENUM_T_Subtype_ManAntiAircraftMissile;

typedef enum
{
    T_Subtype_PersonnelIndividual    = 0X01,							//单兵装具
    T_Subtype_PersonnelIndividualSpecial,								//单兵装具（特战型）
    T_Subtype_PersonnelIndividualAmphibious,							//单兵装具（两栖型）
} ENUM_T_Subtype_Personnel;

typedef enum
{
    T_Subtype_AutomaticRifle95    = 0X01,								//95 式 5.8mm 自动步枪
    T_Subtype_AutomaticRifleAK47,										//AK47 自动步枪
    T_Subtype_AutomaticRifle95_1,										//95-1 式 5.8mm 自动步枪
    T_Subtype_AutomaticRifle03,											//03 式 5.8mm 自动步枪
    T_Subtype_AutomaticRiflePrecision,									//5.8mm 精准步枪
    T_Subtype_AutomaticRifle95B,										//95B 式 5.8mm 短步枪
    T_Subtype_AutomaticRifle95S,										//95 式 5.8mm 自动步枪（特战型）
    T_Subtype_AutomaticRifle95_1S,										//95-1 式 5.8mm 自动步枪（特战型）
    T_Subtype_AutomaticRifle95BS,										//95B 式 5.8mm 短步枪（特战型）
    T_Subtype_AutomaticRifle95_1BS,										//95-1 式 5.8mm 短步枪（特战型）
    T_Subtype_AutomaticRifle95A,										//95 式 5.8mm 自动步枪（两栖型）
    T_Subtype_AutomaticRifle95_1A,										//95-1 式 5.8mm 自动步枪（两栖型）
    T_Subtype_AutomaticRifle95BA,										//95B 式 5.8mm 短步枪（两栖型）
    T_Subtype_AutomaticRifle95_1BA,										//95-1 式 5.8mm 短步枪（两栖型）

} ENUM_T_Subtype_AutomaticRifle;

typedef enum
{
    T_Subtype_MachineGun95    = 0X01,									//95 式 5.8mm 班用机枪
    T_Subtype_MachineGun95_1,											//95-1 式 5.8mm 班用机枪
    T_Subtype_MachineGun95_1S,											//95-1 式 5.8mm 班用机枪（特战型）
    T_Subtype_MachineGun95A,											//95 式 5.8mm 班用机枪（两栖型）
    T_Subtype_MachineGun95_1A,											//95-1 式 5.8mm 班用机枪（两栖型）

} ENUM_T_Subtype_MachineGun;

typedef enum
{
    T_Subtype_SniperRifle88_58mm    = 0X01,								//88 式 5.8mm 狙击枪
    T_Subtype_SniperRifle09,											//09 狙击步枪
    T_Subtype_SniperRifleHighPrecision,									//7.62mm 高精度狙击枪
    T_Subtype_SniperRifle10_127mm,										//10 式 12.7mm 狙击枪
    T_Subtype_SniperRifle58mmS,											//5.8mm 狙击枪（特战型）
    T_Subtype_SniperRifle86mmS,											//8.6mm 狙击枪（特战型）
    T_Subtype_SniperRifle127mmS,										//12.7mm 狙击枪（特战型）
    T_Subtype_SniperRifle58mmA,											//5.8mm 狙击枪（两栖型）
    T_Subtype_SniperRifle762mmA,										//7.62mm 狙击枪（两栖型）
    T_Subtype_SniperRifle10_127mmA,										//10 式 12.7mm 狙击枪（两栖型）

} ENUM_T_Subtype_SniperRifle;

typedef enum
{
    T_Subtype_SubmachineGun05_58mm     = 0X01,							//05 式 5.8mm 微声冲锋枪
    T_Subtype_SubmachineGun05_58mmS,									//05 式 5.8mm 微声冲锋枪（特战型）
    T_Subtype_SubmachineGun05_58mmA,									//05 式 5.8mm 微声冲锋枪（两栖型）

} ENUM_T_Subtype_SubmachineGun;

typedef enum
{
    T_Subtype_ShotGun09_184mm   = 0X01,									//09 式 18.4mm 霰弹枪
    T_Subtype_ShotGun09_184mmS,											//09 式 18.4mm 霰弹枪（特战型）
    T_Subtype_ShotGun09_184mmA,										//09 式 18.4mm 霰弹枪（两栖型）

} ENUM_T_Subtype_ShotGun;


typedef enum
{
    T_Subtype_Pistol = 0X01,												//手枪
    T_Subtype_PistolS,													//手枪

} ENUM_T_Subtype_Pistol;

typedef enum
{
    T_Subtype_PrivateGrenadeLauncher87_35mm      = 0X01,				//87 式 35mm 自动榴弹发射器
    T_Subtype_PrivateGrenadeLauncher04_35mm,							//04 式 35mm 自动榴弹发射器
    T_Subtype_PrivateGrenadeLauncher131_35mm,							//131 型 35mm 狙击榴弹发射器
    T_Subtype_PrivateGrenadeLauncher10_35mm,							//10 式 35mm 枪挂榴弹发射器
    T_Subtype_PrivateGrenadeLauncher87_35mmS,						//87 式 35mm 自动榴弹发射器（特战型）
    T_Subtype_PrivateGrenadeLauncher04_35mmS,							//04 式 35mm 自动榴弹发射器（特战型）
    T_Subtype_PrivateGrenadeLauncher131_35mmS,							//131 型 35mm 狙击榴弹发射器（特战型）
    T_Subtype_PrivateGrenadeLauncher87_35mmA,						//87 式 35mm 自动榴弹发射器（两栖型）
    T_Subtype_PrivateGrenadeLauncher04_35mmA,							//04 式 35mm 自动榴弹发射器（两栖型）
    T_Subtype_PrivateGrenadeLauncher131_35mmA,							//131 型 35mm 狙击榴弹发射器（两栖型）
    T_Subtype_PrivateGrenadeLauncher10_35mmA,							//10 式 35mm 枪挂榴弹发射器（两栖型）

} ENUM_T_Subtype_PrivateGrenadeLauncher;


typedef enum
{
    T_Subtype_HeavyMachineRifle89_127mm    = 0X01,						//89 式 12.7mm 重机枪
    T_Subtype_HeavyMachineRifleQBZ171_127mm,							//QBZ171 山地型 12.7mm 重机枪
    T_Subtype_HeavyMachineRifle89_127mmS,								//89 式 12.7mm 重机枪（特战型）
    T_Subtype_HeavyMachineRifle89_127mmA,								//89 式 12.7mm 重机枪（两栖型）

} ENUM_T_Subtype_HeavyMachineRifle;

typedef enum
{
    T_Subtype_AntiAircraftRifle85_127mm     = 0X01,						//85 式 12.7mm 高射机枪
    T_Subtype_AntiAircraftRifle85_127mmA,								//85 式 12.7mm 高射机枪（两栖型）

} ENUM_T_Subtype_AntiAircraftRifle;

typedef enum
{
    T_Subtype_OpticalReconnaissanceEquipmentBaz308       = 0X01,		//Baz-308 战场侦察电视
    T_Subtype_OpticalReconnaissanceEquipmentBad902C,					//Bad-902C 远程数字照相

} ENUM_T_Subtype_OpticalReconnaissanceEquipment;

typedef enum
{
    T_Subtype_IndividualRocket80mm    = 0X01,							//80 单
    T_Subtype_IndividualRocket11_93mm,									//11 式 93mm 单兵火箭筒
    T_Subtype_IndividualRocket89_80mm,									//PF89 式 80mm 单兵火箭筒
    T_Subtype_IndividualRocket11_93mmG,									//11 式 93mm 单兵火箭筒?
    T_Subtype_IndividualRocket98_93mm,									//PF98 式 93mm 单兵火箭筒

} ENUM_T_Subtype_IndividualRocket;

typedef enum
{
    T_Subtype_ManAntiTankRocket98B_120mm = 0X01,						//PF98 式营用 120mm 反坦克火箭筒
    T_Subtype_ManAntiTankRocket98C_120mm,								//PF98 式连用 120mm 反坦克火箭筒
    T_Subtype_ManAntiTankRocket98B_120mmA,								//PF98 式营用 120mm 反坦克火箭筒（两栖型）
    T_Subtype_ManAntiTankRocket98C_120mmA,								//PF98 式连用 120mm 反坦克火箭筒（两栖型）
} ENUM_T_Subtype_ManAntiTankRocket;

typedef enum
{
    T_Subtype_ManAntiTankRocketRedArrow73 = 0X01,						//红箭 73 改反坦克导弹
    T_Subtype_ManAntiTankRocketRedArrow8,								//红箭 8 反坦克导弹
    T_Subtype_ManAntiTankRocketRedArrow12,								//红箭 12 反坦克导弹
    T_Subtype_ManAntiTankRocketRedArrow8G,								//红箭 8 反坦克导弹 ?
} ENUM_T_Subtype_ManAntiTankmissile;


typedef enum
{
    T_Subtype_MineAntiInfantry = 0X01,									//压发式防步兵地雷
    T_Subtype_MineAntiTank,												//防坦克地雷
    T_Subtype_MineTrippingHairAntiInfantry,								//绊发式防步兵地雷
    T_Subtype_MineDirectional,											//定向雷
} ENUM_T_Subtype_Mine;

typedef enum
{
    T_Subtype_PrivateMineDeminer = 0X01,								//单兵排雷器
    T_Subtype_PrivateMineDeminerSapper,									//工兵模拟器
} ENUM_T_Subtype_PrivateMineDeminer;

typedef enum
{
    T_Subtype_HandGrenade82 = 0X01,										//82-2 式手榴弹
    T_Subtype_HandGrenade77,											//77-1 式手榴弹
} ENUM_T_Subtype_HandGrenade;

typedef enum
{
    T_Subtype_ImprovisedExplosive = 0X01,								//爆炸物
} ENUM_T_Subtype_ImprovisedExplosive;


typedef enum
{
    T_Subtype_MedicalServiceAndAmbulanceEquipment = 0X01,				//卫勤救护
} ENUM_T_Subtype_MedicalServiceAndAmbulanceEquipment;

typedef enum
{
    T_Subtype_ConditioningEquipmentGun = 0X01,							//调理枪
    T_Subtype_ConditioningEquipmentLoadAmmo,							//模拟装弹器
    T_Subtype_ConditioningEquipmentTerminal,							//调理终端

} ENUM_T_Subtype_ConditioningEquipment;

typedef enum
{
    T_Subtype_GmVehicle = 0X01,											//通用车辆
    T_Subtype_GmVehicleA,												//两栖通用非火力车辆
} ENUM_T_Subtype_GmVehicle;

typedef enum
{
    T_Subtype_CommandVehicle    = 0X01,									//指挥车
    T_Subtype_CommandVehicleArtillery,									//炮兵指挥车
    T_Subtype_CommandVehiclePZK091A35,									//PZK091A35 毫米高炮指挥车
    T_Subtype_CommandVehiclePGZ04,										//PGZ04 型弹炮结合武器系统指挥车
    T_Subtype_CommandVehiclePLB625,										//PLB625 弹炮结合武器指挥车

} ENUM_T_Subtype_CommandVehicle;

typedef enum
{
    T_Subtype_CommunicationVehicle = 0X01,								//通信车
    T_Subtype_CommunicationVehicleArtillery,							//炮兵通信节点车
} ENUM_T_Subtype_CommunicationVehicle;

typedef enum
{
    T_Subtype_AmmunitionCarrier = 0X01,									//弹药输送车
} ENUM_T_Subtype_AmmunitionCarrier;

typedef enum
{
    T_Subtype_OilVehicle = 0X01,										//油料运输车
} ENUM_T_Subtype_OilVehicle;

typedef enum
{
    T_Subtype_RocketMineVehicle = 0X01,									//布雷车
} ENUM_T_Subtype_RocketMineVehicle;

typedef enum
{
    T_Subtype_EngineeringReconnaissanceVehicle = 0X01,					//工程侦察车
    T_Subtype_ReconnaissanceVehicle,									//侦察车
} ENUM_T_Subtype_EngineeringReconnaissanceVehicle;

typedef enum
{
    T_Subtype_LightObstructorVehicle = 0X01,							//破障车
    T_Subtype_LightObstructorVehicleCSL132A,							//CSL132 型两栖装甲破障车
} ENUM_T_Subtype_LightObstructorVehicle;

typedef enum
{
    T_Subtype_IntegratedMinesweeperVehicle = 0X01,						//综合扫雷车
} ENUM_T_Subtype_IntegratedMinesweeperVehicle;

typedef enum
{
    T_Subtype_HeavyMechanizedBridgeVehicle84A = 0X01,					//84A 式重型机械化桥
} ENUM_T_Subtype_HeavyMechanizedBridgeVehicle;

typedef enum
{
    T_Subtype_EmergencyVehicle = 0X01,									//装甲抢修车
} ENUM_T_Subtype_EmergencyVehicle;

typedef enum
{
    T_Subtype_EngineeringVehicleCCZ111 = 0X01,							//CCZ111 型两栖工程作业车
} ENUM_T_Subtype_EngineeringVehicle;

typedef enum
{
    T_Subtype_Helicopter10    = 0X01,				//直 10 武装直升机
    T_Subtype_Helicopter19,							//直 19 武装直升机
    T_Subtype_Helicopter20,							//直 20 系列直升机
    T_Subtype_Helicopter17,							//米 17 系列直升机
    T_Subtype_Helicopter8,						//直 8 系列直升机
    T_Subtype_Helicopter9,							//直 9 武装直升机
    T_Subtype_HelicopterArm,						//武装直升机
    T_Subtype_HelicopterTransport,			//运输直升机

} ENUM_T_Subtype_Helicopter;

typedef enum
{
    T_Subtype_DroneRainbow3    = 0X01,				//彩虹-3
    T_Subtype_DroneRainbow4,						//彩虹-4
    T_Subtype_DronePterosaur1,						//翼龙-1
    T_Subtype_DroneMQ9,								//MQ-9
    T_Subtype_DroneRQ1,							//RQ-1
    T_Subtype_DroneRQ4,								//RQ-4

} ENUM_T_Subtype_Drone;

typedef enum
{
    T_Subtype_BomberH6    = 0X01,					//轰 6
    T_Subtype_BomberH7,								//歼轰 7
    T_Subtype_BomberJ10,							//歼 10
    T_Subtype_BomberJ11,							//歼 11
    T_Subtype_BomberJ15,							//歼 15
    T_Subtype_BomberJ16,							//歼 16
    T_Subtype_BomberJ20,							//歼 20
    T_Subtype_BomberS27,							//苏 27
    T_Subtype_BomberS30,							//苏 30
    T_Subtype_BomberS35,							//苏 35
    T_Subtype_BomberF15,							//F15
    T_Subtype_BomberF16,							//F16
    T_Subtype_BomberF18,							//F18
    T_Subtype_BomberF22,							//F22
    T_Subtype_BomberF35,							//F35
    T_Subtype_BomberB53,							//B52
    T_Subtype_BomberB1B,							//B1B
    T_Subtype_BomberB2,								//B2

} ENUM_T_Subtype_Bomber;

typedef enum
{
    T_Subtype_BattlefieldElectromagneticEnvironmentDataAcquisitionEquipment = 0X01,					//战场电磁环境数据采集设备
} ENUM_T_Subtype_ElectronicCountermeasure;


typedef enum
{
    T_Subtype_Shelter = 0X01,						//火力点
    T_Subtype_Bunker,								//地堡
    T_Subtype_SimpleFortificationsFacilities,		//简易工事
    T_Subtype_StrongFortificationsFacilities,		//坚固工事
    T_Subtype_Building,							//建筑物
    T_Subtype_CommandPost,							//指挥所
    T_Subtype_Ammunition,							//弹药库
    T_Subtype_FuelTank,							//油料库
    T_Subtype_MaterialWarehouse,					//物资库
    T_Subtype_RadarStation,							//雷达站
    T_Subtype_Bridge,							//桥梁
    T_Subtype_RailwayStation,						//火车站
    T_Subtype_Runway,								//跑道
    T_Subtype_Hangar,								//机库
    T_Subtype_AimingPointBetween,					//间瞄落点
    T_Subtype_RemoteSmokeGenerator,					//遥控烟雾发生器

} ENUM_T_Subtype_FortificationsFacilities;

typedef enum
{
    T_Subtype_BarbedWire = 0X01,														//铁丝网
    T_Subtype_ThreePyramid,																//三角锥
    T_Subtype_Moat,																		//壕沟
    T_Subtype_AntiTankTrench,															//反坦克堑壕
    T_Subtype_VirtualInfantryMinefield,												//虚拟防步兵雷场
    T_Subtype_VirtualAntiTankMinefield,													//虚拟反坦克雷场
    T_Subtype_VirtualMixedMinefield,													//虚拟混合雷场
    T_Subtype_PhysicalInfantryMinefield1,												//实体防步兵雷场1
    T_Subtype_PhysicalInfantryMinefield2,												//实体防步兵雷场2
    T_Subtype_PhysicalInfantryMinefield3,												//实体防步兵雷场3
    T_Subtype_VirtualNuclearAndBiologicalContaminationArea,							//虚拟核化生沾染区

} ENUM_T_Subtype_BattlefieldObstacle;


/***********************************************************
 		H_Part 	命中部位 单兵类（0x13）
************************************************************/

typedef enum
{
    H_Part_I_Head = 1,											//头部
    H_Part_I_Skull,												//颅骨
    H_Part_I_Neck,												//头面颈
    H_Part_I_Neck_Eye,											//头面颈，眼
    H_Part_I_Head_Chest,										//头/胸
    H_Part_I_Head_Abdomen_Kidney,								//头/腹/肾
    H_Part_I_Head_Abdomen_Spleen,								//头/腹/脾
    H_Part_I_Head_Abdomen_Liver,								//头/腹/肝脏
    H_Part_I_Head_Abdomen_Colon,								//头/腹/结肠
    H_Part_I_Head_Abdomen_Bladder,								//头/腹/膀胱		10
    H_Part_I_Head_LowerLimb,									//头/下肢
    H_Part_I_Face,												//面部
    H_Part_I_Face_Neck,											//面/颈
    H_Part_I_Eye,												//眼
    H_Part_I_Ear,												//耳
    H_Part_I_Jaw,												//下颌
    H_Part_I_Shoulder,											//肩
    H_Part_I_Chest,												//胸部
    H_Part_I_Chest_Abdomen_Kidney,								//胸/腹/肾脏
    H_Part_I_Chest_Abdomen_Bladder,								//胸/腹/膀胱 		20
    H_Part_I_Chest_Abdomen_Spleen,								//胸/腹/脾
    H_Part_I_Chest_Abdomen_Liver,								//胸/腹/肝脏
    H_Part_I_Chest_Abdomen_Colon,								//胸/腹/结肠
    H_Part_I_Chest_Abdomen_Kidney_Bladder,						//胸/腹/结肠/膀胱
    H_Part_I_Chest_Limb,										//胸/四肢
    H_Part_I_Chest_UpperLimb_Abdomen,							//胸/上肢/腹
    H_Part_I_Chest_Abdomen,										//胸/腹
    H_Part_I_Liver,												//肝
    H_Part_I_Spleen,											//脾
    H_Part_I_Kidney,											//肾 	30
    H_Part_I_Colon,												//小肠
    H_Part_I_Abdomen,											//腹部
    H_Part_I_Abdomen_Colon_Spleen,								//腹/结肠/脾
    H_Part_I_Abdomen_Colon_Liver,								//腹/结肠/肝脏
    H_Part_I_Abdomen_Colon_Bladder,								//腹/结肠/膀胱
    H_Part_I_Abdomen_Pelvic_Liver_Kidney,						//腹/盆腔/肝/肾脏
    H_Part_I_Abdomen_Pelvic_Spleen_Bladder,						//腹/盆腔/脾/膀胱
    H_Part_I_Abdomen_Colon_LowerLimb,							//腹/结肠/下肢
    H_Part_I_Abdomen_Pelvic_LowerLimb,							//腹/盆腔/下肢
    H_Part_I_Abdomen_LowerLimb,									//腹/下肢 			40
    H_Part_I_Abdomen_Pelvic_Limb,										//腹/盆腔/四肢
    H_Part_I_Abdomen_Limb,						//腹/四肢
    H_Part_I_Spine,												//脊柱
    H_Part_I_UpperLimb,											//上肢
    H_Part_I_Humerus,											//肱骨
    H_Part_I_Hand,												//手
    H_Part_I_Finger,											//手指
    H_Part_I_Hand_Finger,										//手/手指
    H_Part_I_Elbow,												//肘
    H_Part_I_Wrist,												//腕 		50
    H_Part_I_CarpalBone,										//腕部
    H_Part_I_Forearm,											//前臂
    H_Part_I_Ulna_Radial,										//尺骨/桡骨
    H_Part_I_Trunk,												//躯干
    H_Part_I_Rib,												//肋骨
    H_Part_I_Back,												//背部
    H_Part_I_Hip,												//臀部
    H_Part_I_Pelvis,											//骨盆
    H_Part_I_Femoral,											//股骨
    H_Part_I_Genital,											//生殖器 60
    H_Part_I_Thigh,												//大腿
    H_Part_I_Knee,												//膝
    H_Part_I_PrayerBone,										//膝盖
    H_Part_I_Foot,												//足
    H_Part_I_Ankle,												//踝
    H_Part_I_Foot_Ankle,										//足/踝
    H_Part_I_Foot_Ankle_Heel,									//足/踝/踵
    H_Part_I_AboveKnee,											//膝盖以上
    H_Part_I_BelowKnee,											//膝盖以下
    H_Part_I_LowerLimb_Genital,									//下肢和生殖器 70
    H_Part_I_LowerLimb,											//下肢
    H_Part_I_WholeBody,											//全身

} ENUM_H_Part_INDIVIDUAL ;

/***********************************************************
 		H_Part 	命中部位 坦克（0x01） 突击车（0x04）
************************************************************/
typedef enum
{
    H_Part_T_WholeVehicle = 0,											//整车
    H_Part_T_FrontHigherArmorRight,												//首上装甲右
    H_Part_T_FrontHigherArmorLeft,											//首上装甲左
    H_Part_T_FrontLowerArmorRight,											//首下装甲右
    H_Part_T_FrontLowerArmorLeft,											//首下装甲左
    H_Part_T_RightSideTrackFront,											//右侧履带正面
    H_Part_T_LefttSideTrackFront,											//左侧履带正面
    H_Part_T_RightFrontOfHull,											//车体右侧面前部
    H_Part_T_RightMiddleOfHull,											//车体右侧面中部
    H_Part_T_RightRearOfHull,											//车体右侧面后部
    H_Part_T_LeftFrontOfHull,											//车体左侧面前部
    H_Part_T_LeftMiddleOfHull,											//车体左侧面中部
    H_Part_T_LeftRearOfHull,											//车体左侧面后部
    H_Part_T_RightBehindTrack,											//右侧履带后面
    H_Part_T_LeftBehindTrack,											//左侧履带后面
    H_Part_T_CannonAndShield,											//火炮及防盾
    H_Part_T_TurretFrontLeft,											//炮塔正面左侧
    H_Part_T_TurretFrontRight,											//炮塔正面右侧
    H_Part_T_AntiAircraftGun,											//高射机枪
    H_Part_T_leftSideOfTurret,											//炮塔左侧
    H_Part_T_RightSideOfTurret,											//炮塔右侧
    H_Part_T_RearTurretLeft,											//炮塔后部左侧
    H_Part_T_RearTurretRight,											//炮塔后部右侧

} ENUM_H_Part_TANK_RaiderBuggy ;

/***********************************************************
 		H_Part 	命中部位 其他装甲武器类(0x02-0x03; 0x05-0x12)
************************************************************/
typedef enum
{
    H_Part_A_WholeVehicle = 0,											//整车
    H_Part_A_FrontLeftOfVehicle,										//车辆左前部
    H_Part_A_FrontOfVehicle,											//车辆前部
    H_Part_A_FrontRightOfVehicle,										//车辆右前部
    H_Part_A_LeftSideOfVehicle,											//车辆左侧部
    H_Part_A_RightSideOfVehicle,										//车辆右侧部
    H_Part_A_RearLeftOfVehicle,											//车辆左后部
    H_Part_A_RearOfVehicle,												//车辆后部
    H_Part_A_RearRightOfVehicle,										//车辆右后部
    H_Part_A_VehicleChassis,											//车辆底盘

} ENUM_H_Part_Armor ;

/***********************************************************
 		H_Type 	伤类     单兵类（0x13）
************************************************************/
typedef enum
{
    H_Type_I_NuclearChemicalBiological = 0,		//核辐射致伤 化学致伤 生物致伤
    H_Type_I_Burn,		//烧伤
    H_Type_I_Dislocated,		//脱臼
    H_Type_I_LimbSevered,		//肢体离断
    H_Type_I_Fracture,		// 骨折
    H_Type_I_BulletWounded,		//枪弹伤
    H_Type_I_ShockInjury,		//震荡伤
    H_Type_I_CollisionInjury,		//撞击伤
    H_Type_I_Wounded,		//炸伤
    H_Type_I_MultipleTrauma,		//多发伤
    H_Type_I_Sprain_Strain,		//扭伤/劳损
    H_Type_I_Dislocation_Dislocation,		//错位/脱位

    //自定义
    H_Type_I_WORSE,
    H_Type_I_TLQCTRL,				//调理枪
    H_Type_I_BACKSTAGECTRL,	//后台导控
} ENUM_H_Type_INDIVIDUAL ;

//毁伤类型
typedef enum
{
    KIND_WORSE,		//伤情加重
    KIND_DIRECT,	//直瞄武器打击(枪弹伤)
    KIND_INDIRECT,	//间瞄武器打击(爆炸伤)
    KIND_EXPLODE,	//爆炸物
    KIND_NUCLEAR,	//核生化
    KIND_BACKSTAGE_CTRL,		//后台导控
    KIND_TLQ_CTRL,		//调理员导控
    KIND_BURN,		//烧伤
    KIND_ENCLOSED,//封闭空间
} ENUM_DAMAGE_KIND;

/***********************************************************
 		H_Type 	伤类     坦克（0x01） 突击车（0x04）
************************************************************/
typedef enum
{
    H_Type_T_Destroy = 1,		//击毁
    H_Type_T_SevereDamage,		//重度损伤
    H_Type_T_ArmorDamageLevel1,		//装甲受损Level1
    H_Type_T_ArmorDamageLevel2,		//装甲受损Level2
    H_Type_T_ArmorDamageLevel3,		//装甲受损Level3
    H_Type_T_Nuclear,		//核损伤
    H_Type_T_Chemical,		//化学沾染损伤
    H_Type_T_Biological,		//生物沾染损伤
    H_Type_T_Res9,		//
    H_Type_T_Res10,		//
    H_Type_T_Res11,		//
    H_Type_T_Res12,		//
    H_Type_T_Res13,		//
    H_Type_T_Res14,		//
    H_Type_T_Res15,		//
    H_Type_T_Res16,		//
    H_Type_T_DrivingGearDamage,		// 驾驶装置损坏
    H_Type_T_Res18,		//
    H_Type_T_Res19,		//
    H_Type_T_TrackOrInducerDamage,		// 履带/诱导轮损坏
    H_Type_T_Res21,		//
    H_Type_T_Res22,		//
    H_Type_T_Res23,		//
    H_Type_T_GearBoxDamage,		// 齿轮传动箱损坏
    H_Type_T_Res25,		//
    H_Type_T_LoadWheelDamage,		// 负重轮损坏
    H_Type_T_EngineFailure,		// 发动机损坏
    H_Type_T_RadiatorEngineDamage,		// 散热器/发动机损坏
    H_Type_T_FanOrGearboxDamage,		// 风扇/变速箱损坏
    H_Type_T_Res30,		//
    H_Type_T_CaterpillarDrivingWheelWamaged,		// 履带/主动轮损坏
    H_Type_T_ArtilleryFailure,		// 火炮失效 0x20
    H_Type_T_FireControlSystemFailure,		// 火控系统失效
    H_Type_T_Res34,		//
    H_Type_T_AntiAircraftMachineGunFailure,		// 高射机枪失效
    H_Type_T_Res36,		//

} ENUM_H_Type_TANK_RaiderBuggy ;


/***********************************************************
 		H_Type 	伤类     其他装甲武器类(0x02-0x03; 0x05-0x12)
************************************************************/
typedef enum
{
    H_Type_A_Destroy = 1,											//击毁
    H_Type_A_PowerAndFireSystemsWereDamage,							//动力系统和火力系统均损坏
    H_Type_A_PowerSystemsWereDamage,								//动力系统损坏
    H_Type_A_FireSystemsWereDamage,									//火力系统损坏
    H_Type_A_Res5,													//
    H_Type_A_Nuclear,												// 核损伤
    H_Type_A_Chemical,												// 化学沾染损伤
    H_Type_A_Biological,											// 生物沾染损伤

} ENUM_H_Type_Armor;


__packed typedef struct
{
    uint8_t m_bLocked: 1;		//0-解锁，1-锁定
    uint8_t m_bConnectionState: 1;		//各部件连接状态	0：正常	1：异常
    uint8_t m_bFailureState: 1;		//0：无故障	1：有故障
    uint8_t m_bUnitmode: 1;		//0：训练模式	1：演习模式
    uint8_t m_bBatteryState: 1;		//0：电量正常	1：欠电
    uint8_t m_bIdentity: 2;		//身份 ENUM_BRCHAR
    uint8_t m_bIsNight: 1;		//建链标志
} STRUCT_TS_Character1;		//实体状态字1

__packed typedef struct
{
		uint8_t m_bKey: 5;						//密钥
    uint8_t m_bIdentity: 2;				//身份 ENUM_BRCHAR
		uint8_t m_bUnitmode: 1;				//0：训练模式	1：演习模式

} STRUCT_TS_Character2;	//实体状态字2

__packed typedef struct
{
    u8 m_byHour;
    u8 m_byMinute;
    u8 m_bySecond;
} Struct_Time2;			//时刻2

__packed typedef struct
{
    u8 m_byYear;
    u8 m_byMonth;
    u8 m_byDate;
    u8 m_byHour;
    u8 m_byMinute;
    u8 m_bySecond;
} STRUCT_TIME1;			//时刻1

__packed typedef struct
{
    u8 m_byHour;
    u8 m_byMinute;
    u8 m_bySecond;
} STRUCT_TIME2;			//时刻2

__packed typedef struct
{
    u8 m_byGS_X[3];
    u8 m_byGS_Y[4];
    u8 m_byGS_Lenth[2];
} STRUCT_GPOS;

__packed typedef struct
{
//	u8 m_byLposState:1;				//1 已定位	；0未定位
//	u8 m_byRes1:3;
//	u8 m_byJD_DuHundred:4;
//	u8 m_byJD_DuTen:4;
//	u8 m_byJD_DuUnit:4;
//	u8 m_byJD_FenTen:4;
//	u8 m_byJD_FenUnit:4;
//	u8 m_byJD_FenTenth:4;
//	u8 m_byJD_FenPercent:4;
//	u8 m_byJD_FenThousandth:4;
//	u8 m_byJD_FenTenThousandth:4;

//	u8 m_byWD_DuTen:4;
//	u8 m_byWD_DuUnit:4;
//	u8 m_byWD_FenTen:4;
//	u8 m_byWD_FenUnit:4;
//	u8 m_byWD_FenTenth:4;
//	u8 m_byWD_FenPercent:4;
//	u8 m_byWD_FenThousandth:4;
//	u8 m_byWD_FenTenThousandth:4;
//
//	u8 m_byLenthSign:1;//0+		1-
//	u8 m_byRes2:3;
//	u8 m_byLenthThousand:4;
//	u8 m_byLenthHundred:4;
//	u8 m_byLenthTen:4;
//	u8 m_byLenthUnit:4;
//	u8 m_byLenthTenth:4;

    u8 m_byJD_DuHundred: 4;
    u8 m_byRes1: 3;
    u8 m_byLposState: 1;				//1 已定位	；0未定位

    u8 m_byJD_DuUnit: 4;
    u8 m_byJD_DuTen: 4;

    u8 m_byJD_FenUnit: 4;
    u8 m_byJD_FenTen: 4;

    u8 m_byJD_FenPercent: 4;
    u8 m_byJD_FenTenth: 4;

    u8 m_byJD_FenTenThousandth: 4;
    u8 m_byJD_FenThousandth: 4;



    u8 m_byWD_DuUnit: 4;
    u8 m_byWD_DuTen: 4;

    u8 m_byWD_FenUnit: 4;
    u8 m_byWD_FenTen: 4;

    u8 m_byWD_FenPercent: 4;
    u8 m_byWD_FenTenth: 4;

    u8 m_byWD_FenTenThousandth: 4;
    u8 m_byWD_FenThousandth: 4;



    u8 m_byLenthThousand: 4;
    u8 m_byRes2: 3;
    u8 m_byLenthSign: 1; //0+		1-

    u8 m_byLenthTen: 4;
    u8 m_byLenthHundred: 4;

    u8 m_byLenthTenth: 4;
    u8 m_byLenthUnit: 4;


} STRUCT_LPOS;

//分区
typedef enum
{
    PARTITION_NONE = 0,
    PARTITION_HEAD 	,	//	头
    PARTITION_LCHEST	,	//左胸
    PARTITION_RCHEST	,	//右胸
    PARTITION_ABDOMEN	,	//腹部
    PARTITION_BACK	,	//背
    PARTITION_LARM	,	//	左臂
    PARTITION_RARM	,	//	右臂
    PARTITION_LLEG	,	//	左腿
    PARTITION_RLEG	,	//	右腿
    PARTITION_LBACK	,	//	左背
    PARTITION_RBACK	,	//	右背
    PARTITION_LABDOMEN	,	//	左腹
    PARTITION_RABDOMEN	,	//	右腹
    PARTITION_LWAIST	,	//	左腰
    PARTITION_RWAIST	,	//	右腰
    PARTITION_MAX,

} ENUM_PARTITION;

typedef struct
{
    uint8_t m_RT_Type;           //攻击方实体类型
    uint8_t m_RT_Subtype;        //攻击方实体子类型
    uint8_t m_RRID;              //攻击方单位代字
    uint16_t m_wRPID;            //攻击方设备编号
    uint8_t m_byUniID;
    uint8_t m_A_Type;            //攻击方弹药类型
    uint8_t m_A_Subtype;         //攻击方弹药子类型
    uint8_t m_HurtBlood;					//被弹药击打的伤害血量
    ENUM_PARTITION m_HPart;           //被打击实体命中部位
    ENUM_H_Type_INDIVIDUAL m_HType;   //被打击实体伤类
    //ENUM_HI_INDIVIDUAL_T   m_H_IDegree;         //被打击实体命中伤势
    STRUCT_LPOS m_sFPosition;    //攻击方地理位置

} RCVHURTINFO_T;

typedef struct
{
    u8 T_Type;      //实体类型
    u8 T_Subtype;   //实体子类型
    u32 BulletNum;   //初始弹量
} WEAPONUNID_T;

__packed typedef struct
{
    uint8_t m_byBullet_Type;            //武器n号弹药类型
    uint8_t m_byBullet_SubType;         //武器n号弹药子类型
    uint8_t m_aBulletCfgNum[3];         //n号弹药初始配置数量
    uint8_t m_aBulletRemainNum[3];      //剩余弹量
    uint8_t m_aBulletShotNum[3];        //发射数量
    uint8_t m_aBulletTotalNum[3];       //剩余弹药总数量
		uint8_t m_byAmmuHitRate;						//弹药命中概率	
		uint8_t m_byAmmuAngle;							//弹药侵彻角
		uint8_t m_byAmmuSpeed;							//药弹丸飞行速度	
    uint8_t m_byAmmuWearDepth;          //弹药穿甲深度
    uint8_t m_byAmmuConcreteDepth;      //弹药穿透混凝土厚度
    uint8_t m_byAmmuRange;              //弹药有效射程（间瞄武器为直射时有效射程）
		
		//uint8_t m_sChangeBulletNum[3];		//子弹变化的数量
} BULLETINFO_T;

__packed typedef struct
{

    uint8_t byYear;		//年
    uint8_t byMonth;		//月
    uint8_t byDay;		//日
    uint8_t byHour;		//时
    uint8_t byMin;		//分
    uint8_t bySecond;	//秒
} StructUTC;

typedef struct
{
    u8 A_TypeID;
    u8 Sub_A_TypeID;
} AmmuID;

typedef struct
{
    uint8_t WeaponID;
    uint8_t AmmuID;
} UnifiedID;


__packed typedef struct
{
    uint8_t year;	//年份(+2000)
    uint8_t month;	//月份
    uint8_t date;	//日期
    uint8_t hour; 	//小时
    uint8_t min; 	//分钟
    uint8_t sec; 	//秒钟
} UTC_TIME;

#if 0
typedef enum
{
    FLASH_BSY = 1,
    FLASH_PGRM_FLR,
    FLASH_WRPRT_FLR,
    FLASH_PRC_DONE,
    FLASH_TIMEOUT
} FLASH_Status;

#endif


typedef enum
{
    //坦克（非火控模拟）
    TType_96Tank = 1, //96坦克
    TType_88BTank	, //88B坦克
    TType_62Tank	, //62坦克
    TType_59Tank	, //59坦克
    TType_M60A3Tank	, //M60A3坦克
    TType_63ATank	, //63A坦克
    TType_59GTank	, //59改坦克
    TType_M1A1Tank	, //M1A1坦克
    TType_99Tank	, //99坦克
    TType_96ATank,    //96A式坦克

    //坦克（嵌入式模拟）
    TType_E96Tank,    //96A嵌入式坦克

    //步战车
    TType_86Hariot 	, //86轮式步战车
    TType_92Hariot 	, //92轮式步战车
    TType_04Hariot	, //04步战车
    TType_05Hariot,   //05式步战车
    TType_05AmphibiousHariot, //05式两栖装甲步兵战车
    TType_08Hariot,   //式轮式步战车

    //装甲输送车
    TType_63CArmoredCarrier,    //63C装甲输送车
    TType_001AmphibiousCarrier, //001 型两栖装甲输送车
    TType_89Carrier,            //89 式装甲输送车
    TType_10Carrier,            //10 式装甲输送车

    //突击车
    TType_05RaiderBuggy,  //05突击车
    TType_11RaiderBuggy,  //11 式突击车
    TType_03RaiderBuggy,  //03 式突击车
    TType_05AmphibiousRaiderBuggy,//05 式两栖装甲突击车

    //突击炮
    TType_100mmAssaultGun,           //100mm轮式突击炮
    TType_120mmAntiTankSmoothBoreGun,//120mm 反坦克滑膛炮

    //反坦克导弹系统
    TType_RedArrow9,  //红箭 9 反坦克导弹系统
    TType_RedArrow10, //红箭 10 反坦克导弹系统

    //牵引火炮
    TType_83_96_122LiuDanPao, //PL83/96 式 122 榴弹炮
    TType_59_1_130JiaNongPao, //PL59-1 式 130 加农炮
    TType_66_152JiaLiuPao,    //PL66 式 152 加榴炮

    //自行火炮（非数字化）
    TType_89_122LiuDanPao, //PLZ89 式 122 榴弹炮
    TType_83_152JiaLiuPao, //PLZ83 式 152 加榴炮

    //自行火炮（数字化）
    TType_05_155JiaLiuPao,           //PLZ05 式 155 加榴炮
    TType_07_122_LiuDanPao,          //PLZ07 式 122 榴弹炮
    TType_09_122_LiuDanPao,          //PLC09 式 122 榴弹炮
    TType_05_122_ZiXingPaiLiuPao,    //PLL05 式 120mm 自行迫榴炮
    TType_AmphibiousZiXingLiuDanPao, //两栖自行榴弹炮

    //火箭炮
    TType_81_122HuoJianPao,     //PHL81 式 122 火箭炮
    TType_89_122HuoJianPao,     //PHZ89 式 122 火箭炮
    TType_05_300YCHuoJianPao,   //PHZ05 式 300mm 远程火箭炮

    //迫击炮
    TType_89_82PaiJiPao, //89 式 82mm 迫击炮
    TType_89_100PaiJiPao,//89 式 100mm 迫击炮
    TType_120PaiJiPao,   //120mm 迫击炮
    TType_89_60PaiJiPao, //89 式 60mm 迫击炮
    TType_82SSPaiJiPao,  //82mm 速射迫击炮

    //炮兵侦察
    TType_ArtilleryPositionRadar,//炮位侦察雷达
    TType_ArtilleryVehicle,      //炮兵侦察车
    TType_ArtilleryMovingTargetReconnaissanceRadar,//炮兵活动目标侦察雷达
    TType_AnalogBurstPointIndicator,//炮兵侦察用模拟炸点指示器

    //高炮（有火控）
    TType_091A35GPZC,    //PZK091A35 毫米高炮战车
    TType_99_35_GP_DataAcquisitionUnit,    //PG99 式 35 毫米高炮数据采集设备
    TType_04_DPJHWQXTZC,    //PGZ04 型弹炮结合武器系统战车
    TType_625_DPJHWQZC,    //PLB625 弹炮结合武器战车

    //高炮（无火控）
    TType_AntiaircraftGun_Double37,    //双 37 高炮
    TType_AntiaircraftGun_Double25,    //双25高炮 20
    TType_AntiaircraftGun_4_25,        //四－25高炮
    TType_AntiaircraftGun_57mm,        //57mm高炮

    //防控导弹系统
    TType_HQ17_SC,     //红旗 17 导弹武器系统 S 车
    TType_HQ17_FC,     //红旗 17 导弹武器系统 F 车
    TType_HQ16_DataAcquisitionUnit,        //红旗 16A/D 导弹武器系统数据采集设备
    TType_HQ7B_SC_DataAcquisitionUnit,     //红旗 7B 导弹武器 S 车系统数据采集设备
    TType_HQ7B_FC_DataAcquisitionUnit,     //红旗 7B 导弹武器 F 车系统数据采集设备

    //防空雷达系统
    TType_LLP12A_RadarDataAcquisitionUnit,    //LLP12A 火控雷达数据采集设备
    TType_STD_RadarDataAcquisitionUnit,   //情报雷达数据采集设备（标准型）

    //防空指挥系统
    TType_CICS_LZ_DataAcquisitionUnit,    //指挥信息系统旅（营）站数据采集设备
    TType_CICS_GPLZ_DataAcquisitionUnit,    //指挥信息系统高炮连站数据采集设备

    //单兵防空导弹
    TType_HY5,      //红缨 5
    TType_HY6_FKDD, //红缨 6 便携式防空导弹
    TType_QW2_FKDD, //前卫 2 便携式防空导弹
    TType_HY6_DD,   //红缨 6 便携式导弹
    TType_QW2_FFDD2,//前卫 2 便携式防空导弹 TODO:和上面重复了

    //人员
    TType_PersonalEquipment,       //单兵装具
    TType_PersonalEquipment_TZ,    //单兵装具（特战型）
    TType_PersonalEquipment_LQ,    //单兵装具（两栖型）

    //自动步枪
    TType_95_58ZDBQ,       //95 式 5.8mm 自动步枪
    TType_AK47ZDBQ,        //AK47 自动步枪
    TType_95_1_58ZDBQ,     //95-1 式 5.8mm 自动步枪
    TType_03_58ZDBQ,      //03 式 5.8mm 自动步枪
    TType_58JZBQ,          //5.8mm 精准步枪
    TType_95B_58DBQ,       //95B 式 5.8mm 短步枪
    TType_95_1_58DBQ,      //95-1 式 5.8mm 短步枪
    TType_95_58ZDBQ_TZ,    //95 式 5.8mm 自动步枪（特战型）
    TType_95_1_58ZDBQ_TZ,  //95-1 式 5.8mm 自动步枪（特战型）
    TType_95B_58DBQ_TZ,    //95B 式 5.8mm 短步枪（特战型）
    TType_95_1_58DBQ_TZ,   //95-1 式 5.8mm 短步枪（特战型）
    TType_95_58ZDBQ_LQ,    //95 式 5.8mm 自动步枪（两栖型）
    TType_95_1_58ZDBQ_LQ,  //95-1 式 5.8mm 自动步枪（两栖型）
    TType_95B_58DBQ_LQ,    //95B 式 5.8mm 短步枪（两栖型）
    TType_95_1_58DBQ_LQ,   //95-1 式 5.8mm 短步枪（两栖型）

    //班用机枪
    TType_95_58BYJQ,       //95 式 5.8mm 班用机枪
    TType_95_1_58BYJQ,     //95-1 式 5.8mm 班用机枪
    TType_95_1_58BYJQ_TZ,  //95-1 式 5.8mm 班用机枪（特战型）
    TType_95_58BYJQ_LQ,    //95 式 5.8mm 班用机枪（两栖型）
    TType_95_1_58BYJQ_LQ,  //95-1 式 5.8mm 班用机枪（两栖型）

    //狙击步枪
    TType_88_58JJQ,    //88 式 5.8mm 狙击枪
    TType_09JJBQ,      //09 狙击步枪
    TType_762GJDJJQ,   //7.62mm 高精度狙击枪
    TType_10_127JJQ,   //10 式 12.7mm 狙击枪
    TType_58JJQ_TZ,    //5.8mm 狙击枪（特战型）
    TType_86JJQ_TZ,    //8.6mm 狙击枪（特战型）
    TType_127JJQ_TZ,   //12.7mm 狙击枪（特战型）
    TType_58JJQ_LQ,    //5.8mm 狙击枪（两栖型）
    TType_762JJQ_LQ,   //7.62mm 狙击枪（两栖型）
    TType_10_127JJQ_LQ,//10 式 12.7mm 狙击枪（两栖型）

    //微声冲锋枪
    TType_05_58WSCFQ,       //05 式 5.8mm 微声冲锋枪
    TType_05_58WSCFQ_TZ,    //05 式 5.8mm 微声冲锋枪（特战型）
    TType_05_58WSCFQ_LQ,    //05 式 5.8mm 微声冲锋枪（两栖型）

    //霰弹枪
    TType_09_184XDQ,    //09 式 18.4mm 霰弹枪
    TType_09_184XDQ_TZ,    //09 式 18.4mm 霰弹枪（特战型）
    TType_09_184XDQ_LQ,    //09 式 18.4mm 霰弹枪（两栖型）

    //手枪
    TType_SQ,    //手枪
    TType_SQ_TZ, //手枪发射机（特战型）

    //单兵榴弹发射器
    TType_87_35ZDLD,       //87 式 35mm 自动榴弹发射器
    TType_04_35ZDLD,       //04 式 35mm 自动榴弹发射器
    TType_131_35JJLD,      //131 型 35mm 狙击榴弹发射器
    TType_10_35QGLD,       //10 式 35mm 枪挂榴弹发射器
    TType_87_35ZDLD_TZ,    //87 式 35mm 自动榴弹发射器（特战型）
    TType_04_35ZDLD_TZ,    //04 式 35mm 自动榴弹发射器（特战型）
    TType_131_35JJLD_TZ,   //131 型 35mm 狙击榴弹发射器（特战型）
    TType_87_35ZDLD_LQ,    //87 式 35mm 自动榴弹发射器（两栖型）
    TType_04_35ZDLD_LQ,    //04 式 35mm 自动榴弹发射器（两栖型）
    TType_131_35JJLD_LQ,   //131 型 35mm 狙击榴弹发射器（两栖型）
    TType_10_35QGLD_LQ,    //10 式 35mm 枪挂榴弹发射器（两栖型）

    //重机枪
    TType_89_127ZJQ,       //89 式 12.7mm 重机枪
    TType_QBZ171_127ZJQ,   //QBZ171 山地型 12.7mm 重机枪
    TType_89_127ZJQ_TZ,    //89 式 12.7mm 重机枪（特战型）
    TType_89_127ZJQ_LQ,    //89 式 12.7mm 重机枪（两栖型）

    //高射机枪
    TType_85_127GSJQ,      //85 式 12.7mm 高射机枪
    TType_85_127GSJQ_LQ,   //85 式 12.7mm 高射机枪（两栖型）

    //光学侦察设备
    TType_BAZ_308,     //Baz-308 战场侦察电视
    TType_BAD_902C,    //Bad-902C 远程数字照相

    //单兵火箭
    TType_80DBHJT,       //80 单
    TType_11_93DBHJT,    //11 式 93mm 单兵火箭筒
    TType_PF89_80DBHJT,  //PF89 式 80mm 单兵火箭筒
    TType_11_93DBHJT2,   //11 式 93mm 单兵火箭筒
    TType_PF89_80DBHJT2, //PF98 式 93mm 单兵火箭筒  TODO:和上面重复

    //单兵反坦克火箭
    TType_PF98_YY120_FTKHJT,       //PF98 式营用 120mm 反坦克火箭筒
    TType_PF98_LY120_FTKHJT,       //PF98 式连用 120mm 反坦克火箭筒
    TType_PF98_YY120_FTKHJT_LQ,    //PF98 式营用 120mm 反坦克火箭筒（两栖型）
    TType_PF98_LY120_FTKHJT_LQ,    //PF98 式连用 120mm 反坦克火箭筒（两栖型）

    //单兵反坦克导弹
    TType_HJ73G_FTKDD,   //红箭 73 改反坦克导弹
    TType_HJ8_FTKDD,     //红箭 8 反坦克导弹
    TType_HJ12_FTKDD,    //红箭 12 反坦克导弹
    TType_HJ8_FTKDD2,    //红箭 8 反坦克导弹 TODO:重复

    //地雷
    TType_YFSFBBmine,    //压发式防步兵地雷
    TType_FTKmine,       //防坦克地雷
    TType_BFSFBBmine,    //绊发式防步兵地雷
    TType_DXmine,        //定向雷

    //单兵排雷器
    TType_DBPLQ,    //单兵排雷器
    TType_GBMNQ,    //工兵模拟器

    //手雷
    TType_82_2SLD,    //82-2 式手榴弹
    TType_77_1SLD,    //77-1 式手榴弹

    //简易爆炸物
    TType_BZW,        //爆炸物

    //卫勤救护装备
    TType_WQJH,       //卫勤救护

    //调理设备
    TType_TLQ,      //调理枪
    TType_MNZDQ,    //模拟装弹器
    TType_TLZD,     //调理终端

    //通用车辆
    TType_TYCL,      //通用车辆
    TType_LQTYFHLCL, //两栖通用非火力车辆

    //指挥车辆
    TType_ZHC,              //指挥车
    TType_PBZHC,            //炮兵指挥车
    TType_PZK091A35_ZHC,    //PZK091A35 毫米高炮指挥车
    TType_PGZ04_ZHC,        //PGZ04 型弹炮结合武器系统指挥车
    TType_PLB625_ZHC,       //PLB625 弹炮结合武器指挥车

    //通信车辆
    TType_TXC,       //通信车
    TType_PBTXJDC,   //炮兵通信节点车

    //弹药输送车
    TType_DYSSC,    //弹药输送车

    //油料运输车
    TType_YLYSC,    //油料运输车

    //火箭布雷车
    TType_BLC,      //布雷车

    //工程侦察车
    TType_GCZZC,    //工程侦察车
    TType_ZZC,      //侦察车

    //轻型破障车
    TType_PZC,      //破障车
    TType_CSL132,   //CSL132 型两栖装甲破障车

    //综合扫雷车
    TType_ZHSLC,    //综合扫雷车

    //重型机械化桥
    TType_84A,      //84A 式重型机械化桥

    //抢修车辆
    TType_ZJQXCL,   //装甲抢修车

    //工程作业车辆
    TType_CCZ111,   //CCZ111 型两栖工程作业车

    //直升机
    TType_Z10WZZSJ,  //直 10 武装直升机
    TType_Z19WZZSJ,  //直 19 武装直升机
    TType_Z20ZSJ,    //直 20 系列直升机
    TType_M17ZSJ,    //米 17 系列直升机
    TType_Z8ZSJ,     //直 8 系列直升机
    TType_Z9WZZSJ,   //直 9 武装直升机
    TType_WZZSJ,     //武装直升机
    TType_YSZSJ,     //运输直升机

    //无人机
    TType_CH3,    //彩虹-3
    TType_CH4,    //彩虹-4
    TType_YL1,    //翼龙-1
    TType_MQ9,    //MQ-9
    TType_RQ1,    //RQ-1
    TType_RQ4,    //RQ-4

    //战斗机/轰炸机
    TType_H6,     //轰 6
    TType_JH7,    //歼轰 7
    TType_J10,    //歼 10
    TType_J11,    //歼 11
    TType_J15,    //歼 15
    TType_J16,    //歼 16
    TType_J20,    //歼 20
    TType_S27,    //苏 27
    TType_S30,    //苏 30
    TType_S35,    //苏 35
    TType_F15,    //F15
    TType_F16,    //F16
    TType_F18,    //F18
    TType_F22,    //F22
    TType_F35,    //F35
    TType_B52,    //B52
    TType_B1B,    //B1B
    TType_B2,     //B2

    //电子对抗
    TType_DCHJ_DataGetEquip,    //战场电磁环境数据采集设备

    //工事设施
    TType_FirePoint, //火力点
    TType_DB,        //地堡
    TType_JYGS,      //简易工事
    TType_JGGS,      //坚固工事
    TType_JZW,       //建筑物
    TType_ZhiHuiSuo, //指挥所
    TType_DanYaoKu,  //弹药库
    TType_YouLiaoKu, //油料库
    TType_WuZiKu,    //物资库
    TType_LDZ,       //雷达站
    TType_QiangLiang,//桥梁
    TType_HCZ,       //火车站
    TType_PD,        //跑道
    TType_JK,        //机库
    TType_JMLD,      //间瞄落点
    TType_YKYWFSQ,   //遥控烟雾发生器

    //战场障碍
    TType_TieSiWang,      //铁丝网
    TType_SanJiaoZhui,    //三角锥
    TType_HG,             //壕沟
    TType_FTKQH,          //反坦克堑壕
    TType_XNFBBLC,        //虚拟防步兵雷场
    TType_XNFTKLC,        //虚拟反坦克雷场
    TType_FNHHLC,         //虚拟混合雷场
    TType_STFBBLC,        //实体防步兵雷场
    TType_STFBBLC2,       //实体防步兵雷场
    TType_STFBBLC3,       //实体防步兵雷场 TODO:重复
    TType_XNHHSZRQ,       //虚拟核化生沾染区

    //主控台
    TType_ZKT,      //主控台

} EnumTerminalType;

typedef enum
{
    WTYPE_WIRE = 0,//有线
    WTYPE_BLE = 1,//蓝牙
    WTYPE_XYZ66X = 2,//新一新标准zigbee
    WTYPE_LORA = 3,//lora设备
} E_WIRELESS_TYPE;

#ifdef __cplusplus
}
#endif

#endif




