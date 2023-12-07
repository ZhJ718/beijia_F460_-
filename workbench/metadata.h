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
*                           弹药类型（A_Type�?                   *
******************************************************************/



/******************************************************************
*                           �þ������֣�AC_Character��            *
���������ػ�����ն˶�ʵ��Ĳþ����
���ȣ�1BYTE
��ʽ��HEX

	0-	����
	1-	����
	2-	����
	3-	��Ա/װ����λ��״̬�ָ���������ҩ�ָ��趨ֵ��
	4-	����װ��ʧЧ
	5-	�ָ���ҩ���ָ�����һ�����õ�ҩ��
	6-	��ϰģʽ
	7-	ѵ��ģʽ
	8-	����ģʽ
	9-	ҹ��ģʽ
	10-	��Ա����
	11-	��Ա�ж���
	12-	��Ա�ض���
	13-	��ԱΣ����
	14-	װ���������
	15-	װ���ж�����
	16-	װ���ض�����
	17-	װ�׳���ɥʧ��������
	18-	װ�׳���ɥʧ��������
	19-	װ�׳���ɥʧ�����ͻ�������
	20-	�������ⷢ��/��ҵ��������
	21-	��Ա�Ȼ�/װ��ά�޳ɹ����ָ�ս��
	22-	����
	23-	����в
	24-	����в
	25-	����в
	26-	�����в
	27-	���Ƶȼ�����
	28-	���Ƶȼ�����
	29-	תΪ���
	30-	תΪ����
	31-	��Ѻ
	0xFF -���ӳ�������
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
*                          ��Ա��̬״̬�֣�BP_Character��		  *
��������λ��ʾ��������̬��
���ȣ�1BYTE
��ʽ��HEX

	0	-	����
	1	-	����
	2	-	����
	3	-	����
	4-255	����
******************************************************************/

typedef enum
{
    BPCHAR_RES = 0,
    BPCHAR_STAND,
    BPCHAR_LIE,
    BPCHAR_KNEEL,
} ENUM_BPCHAR;

/******************************************************************
*                          ��Ա��ݱ�ʶ�֣�BR_Character��		  *
�������������뽻ս��ʵ����ݡ�
���ȣ�2BIT
��ʽ��HEX

ȡֵ 	˵�� 	��ע
00 	ģ������ 	��
01 	��ѵ���� 	��
10 	Ⱥ�� 	��
11 	������Ա���ʮ��ҽ����Ա 	��

******************************************************************/

typedef enum
{
    BRCHAR_BLUE = 0,
    BRCHAR_RED,
    BRCHAR_YELLOW,
    BRCHAR_WHITE,
} ENUM_BRCHAR;

/******************************************************************
*						   �ۺ����ƣ�C_Idegree��				  *
��������ʵ������ T_Type ���ʹ�ã��������������豸�ĵ�ǰ�ۺϻ��˵ȼ���
���ȣ�1BYTE
��ʽ��HEX
******************************************************************/


/***********************************************************
ʵ������		C_Idegree	���Ƶȼ�			��ע

װ����			0			����				K ��
��0x01-0x12�� 1			�����ͻ���������ɥʧ M_F ��
				2			ɥʧ��������		M ��
				3			ɥʧ��������		F ��
				4			����װ���ض�����
				5			����װ���ж�����
				6			����װ���������
				7			����
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
ʵ������ 		C_Idegree 	���Ƶȼ� 	��ע

�����ࣨ0x13�� 	0 			���� 		death
				1-5 		Σ����      critical
				6-9 		�ض���
				10-11 		�ж���
				12 			����
				13 			����

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
    HI_SOLDIER_D0 = 0,   //��
    HI_SOLDIER_C1,   //Σ
    HI_SOLDIER_C2,   //Σ
    HI_SOLDIER_C3,   //Σ
    HI_SOLDIER_C4,   //Σ
    HI_SOLDIER_C5,   //Σ
    HI_SOLDIER_S6,   //��
    HI_SOLDIER_S7,   //��
    HI_SOLDIER_S8,   //��
    HI_SOLDIER_S9,   //��
    HI_SOLDIER_ME10, //��
    HI_SOLDIER_ME11, //��
    HI_SOLDIER_L12,  //��
    HI_SOLDIER_N13,  //��
} ENUM_HI_INDIVIDUAL_T;

/***********************************************************
ʵ������ 		C_Idegree 	���Ƶȼ� 	��ע

������ 			0 			����
				1 			����
				2 			����
************************************************************/

typedef enum
{
    CI_OTHER_D = 0,
    CI_OTHER_I,
    CI_OTHER_N,
} ENUM_CI_OTHER;


/******************************************************************
*                          ��Ա���ν����ʶ�֣�CE_Character��	  *
��������ʾ��Ա�����κ��Ч��
���ȣ�1BYTE
��ʽ��HEX

ȡֵ 	˵�� 				��ע
0 		����
1 		���εõ����ָ�ս�� 	 Recovery
2 		���γɹ������ٶ� 	 Hold
3 		����ʧ�ܣ������ 	 Worse
4 		����ʧ�ܣ����� 	 	 Death
5-255 	����
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
*                          ���η�ʽ��ʶ�֣�CM_Character��		  *
������ҽ����Ա���η�ʽ��
���ȣ�1BYTE
��ʽ��HEX


ȡֵ 	˵�� 			��ע
0 		����
1 		�Ծ�
2 		����
3 		�Ȼ���Ա�ֳ�����
4 		�Ȼ���Ա���Ⱥ���
5-255 	����

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
*                          ��λ��Direction��					  *
��������ʾǰ�������ҡ��ϡ��� 6 ����λ��
���ȣ�1BYTE
��ʽ��HEX


ȡֵ 	˵�� 	��ע
0 		����
1 		ǰ
2 		��
3 		��
4 		��
5 		��
6 		��
7-255 	����

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
*                       ����/�뿪Σ�յ����ʶ�֣�ELR_Character��  *
������ʶ�������뿪ĳһΣ�յ����׳����˻���մȾ���ȣ���
���ȣ�1BYTE
��ʽ��HEX

ȡֵ 	˵�� 	��ע
0 		����
1 		����
2 		�뿪
3-255 	����
******************************************************************/

typedef enum
{
    ELRCHAR_RES = 0,
    ELRCHAR_ENTER,
    ELRCHAR_LEAVE,
} ENUM_ELRCHAR;


/******************************************************************
*                      �������ͣ�Fuse_Type��						 *
��������ʾ��ҩ���������͡�
���ȣ�1BYTE
��ʽ��HEX

ȡֵ 	˵�� 		��ע
0 		����
1 		����������
2 		��������
3 		˲������
4 		ʱ������
5 		��ը����
6-255 	����
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
*                      װҩ�ţ�G_Num��							 *
��������ʾ���鵯ҩ��װҩ�š�
���ȣ�1BYTE
��ʽ��HEX

ȡֵ 	˵�� 		��ע
0 		0��װҩ
1 		1��װҩ
2 		2��װҩ
3 		3��װҩ
4 		4��װҩ
5 		5��װҩ
6 		6��װҩ
7-255 	����

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
*                      ��˹���꣨G_Position��					  *
�������豸��˹���� X����˹���� Y �͸�˹����̡߳�
���ȣ�9BYTE
��ʽ��HEX�����룩

�ֽ���� 	���� 	��ע
1-3 		��˹���� X
4-7 		��˹���� Y
8-9 		����߳�
******************************************************************/

//__packed typedef struct
//{
//	int32_t m_dwX;
//	int32_t m_dwY;
//	int16_t m_wH;
//}STRUCT_GPOS;

/******************************************************************
*                      Υ���¼������ͣ�IE_Type��				  *
��������ʾΥ����������͡�
���ȣ�1BYTE
��ʽ��HEX

ȡֵ 	˵�� 			��ע
0 		����
1 		�ڵ�̽ͷ����
2 		̽ͷδ����
3 		����δ����
4 		���λ���װ��δ����
5 		���̹�δ��װ
6 		���ⵯδ��װ
7 		����/�������˶�
8 		ɥʧ�����������˶�
9 		����ͷ��δ����
10 		Υ��ػ�
11 		�������ƶ�
12-255 	����
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
*                      ��ҩ (����)������ʶ�֣�IO_Character��	  *
��������ʶ����(��ҩ)������ȡ��������
���ȣ�1BYTE
��ʽ��HEX


ȡֵ 	˵�� 						��ע
0 		����
1 		��ҩ/���ʱ�ȡ�����������٣�
2 		��ҩ/���ʵõ��������������ӣ�
3-255 	����

******************************************************************/

typedef enum
{
    IOCHAR_RES = 0,
    IOCHAR_OUT,
    IOCHAR_IN,
} ENUM_IOCHAR;


/******************************************************************
*                     ��γ�����꣨L_Position��					  *
�������豸���ȡ�γ�ȡ��߶��Լ���λ״̬��
���ȣ�12BYTE
��ʽ��BCD


�ֽ���� 	���� 	λ�� 	��ע
1 			���� 	bit7 	1���Ѷ�λ��0��δ��λ
					bit6-bit4 	����
					bit3-bit0 	�ȵİ�λ
2 					bit7-bit4 	�ȵ�ʮλ
					bit3-bit0 	�ȵĸ�λ
3 					bit7-bit4 	�ֵ�ʮλ
					bit3-bit0 	�ֵĸ�λ
4 					bit7-bit4 	�ֵ�ʮ��λ
					bit3-bit0 	�ֵİٷ�λ
5 					bit7-bit4 	�ֵ�ǧ��λ
					bit3-bit0 	�ֵ����λ

6 			γ��  	bit7-bit4 	�ȵ�ʮλ
					bit3-bit0 	�ȵĸ�λ
7 					bit7-bit4 	�ֵ�ʮλ
					bit3-bit0 	�ֵĸ�λ
8 					bit7-bit4 	�ֵ�ʮ��λ
					bit3-bit0 	�ֵİٷ�λ
9 					bit7-bit4 	�ֵ�ǧ��λ
					bit3-bit0 	�ֵ����λ

10 			�߶�  	bit7 		0��+��1��-
		����λ:�ף�	bit3-bit0 	ǧλ
11 					bit7-bit4 	��λ
					bit3-bit0 	ʮλ
12 					bit7-bit4 	��λ
					bit3-bit0 	ʮ��λ

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
*                      ���ӱ�ʶ�֣�L_Character��				  *
��������ʾ����״̬��
���ȣ�1BYTE
��ʽ��HEX

ȡֵ 	˵�� 		��ע
0 		����
1 		���ӣ�Ƿ��
2 		���ӣ���Ƿ��
3 		����
4-255 	����

******************************************************************/

typedef enum
{
    LCHAR_RES = 0,
    LCHAR_LINK_LACK,
    LCHAR_LINK_UNLACK,
    LCHAR_DISLINK,
} ENUM_LCHAR;



/******************************************************************
*                      �����в���ͣ�LT_Type��					  *
��������ʾʵ���ܵ��Ĺ����в���͡�
���ȣ�1BYTE
��ʽ��HEX

ȡֵ 	˵�� 	��ע
0 	����
1 	��༤����в
2 	���伤����в
3-255 	����
******************************************************************/

typedef enum
{
    LTTYPE_RES = 0,
    LTTYPE_RANGE,
    LTTYPE_SHINE,
} ENUM_LTTYPE;

/******************************************************************
*                      �������� 			       				  *
******************************************************************/

/******************************************************************
*                      ȼ���������� 			       			*

M_Subtype ȡֵ 	���������� 	��ע
1 				����
2 				����
3 				����ȼ��
******************************************************************/

typedef enum
{
    M_OIL_DIESEL = 1,
    M_OIL_GASOLINE,
    M_OIL_JET,
} ENUM_M_OIL;

/******************************************************************
*                     ��������			       			*

M_Subtype ȡֵ 	���������� 	��ע
1 				����
2 				����
3 				ɴ��
******************************************************************/

typedef enum
{
    M_QUARTERMASTER_QUILT = 1,
    M_QUARTERMASTER_TENT,
    M_QUARTERMASTER_GAUZE,
} ENUM_M_QUARTERMASTER;



/******************************************************************
*                     ɨ�׷�ʽ��ʶ�֣�MCM_Character�� 			*

ȡֵ 	˵�� 	��ע
0 		����
1 		��ɨ
2 		��ɨ
3 		ѹ��ɨ��
4-255 	����

******************************************************************/

typedef enum
{
    MCMCHAR_RES = 0,
    MCMCHAR_PLOUGH,
    MCMCHAR_MAGNET,
    MCMCHAR_PRESS,
} ENUM_MCMCHAR;


/******************************************************************
*                   �˻��������¼������ͣ�NCBP_Type�� 			*

��������ʾʵ��ʵʩ�ĺ˻����������͡�
���ȣ�1BYTE
��ʽ��HEX

ȡֵ 	˵�� 		��ע
0 		����
1 		������װ��
2 		�ر�����װ��
3 		����������
4 		ժ�·������
5-255 	����

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
*                   �˻�����ʶ�֣�NCBT_Type��			 			*

��������ʾ�˻�����в�������ľ�������
���ȣ�1BYTE
��ʽ��HEX


ȡֵ 	˵�� 	��ע
0 		����
1 		��
2 		��
3 		��
4-255 	����

******************************************************************/

typedef enum
{
    NCBTTYPE_RES = 0,
    NCBTTYPE_N,
    NCBTTYPE_C,
    NCBTTYPE_B,
} ENUM_NCBTTYPE;

/******************************************************************
*                 ��/�ػ�������ʶ�֣�PO_Character��		 			*

��������ʾ������ػ�������
���ȣ�1BYTE
��ʽ��HEX

ȡֵ 	˵�� 	��ע
0 		����
1 		��Ƿ��ػ�
2 		Ƿ��ػ�
3 		����
4-255 	����
******************************************************************/

typedef enum
{
    POCHAR_RES = 0,
    POCHAR_POWEROFF_UNLACK,
    POCHAR_POWEROFF_LACK,
    POCHAR_POWERON,
} ENUM_POCHAR;


/******************************************************************
*                 ��λ���֣�RID��						 			*

�������ն��豸ʹ�õ�λ��š�
���ȣ�1BYTE
��ʽ��HEX


ȡֵ 	˵�� 		��ע
0 		����
1 		���պͻ���
2 		�������
3 		XXX ��λ
4 		XXX ��λ
5 		XXX ��λ
6 		XXX ��λ
�� 		��
128-255 ����

******************************************************************/

typedef enum
{
    RID_RES = 0,
    RID_ZRH,
    RID_SJ,
} ENUM_RID;

/******************************************************************
*                �������ģʽ��ʶ�֣�RM_Character��	 			*

��������ʾ���ڵ����ģʽ��
���ȣ�1BYTE
��ʽ��HEX

ȡֵ 	˵�� 		��ע
0 		����
1 		��������
2 		��������
3-255 	����

******************************************************************/

typedef enum
{
    RMCHAR_RES = 0,
    RMCHAR_H,
    RMCHAR_L,
} ENUM_RMCHAR;

/******************************************************************
*                ���/У��ģʽ��ʶ�֣�SAM_Character��	 			*

����������ʶ�����/У���״�ĵ�ǰʵ��״̬�� 1��
���ȣ�1BYTE
��ʽ��HEX

ȡֵ 	˵�� 	��ע
0 		����
1 		���
2 		У��
3-255 	����

******************************************************************/

typedef enum
{
    SAMCHAR_RES = 0,
    SAMCHAR_SCOUT,
    SAMCHAR_AMENDING,
} ENUM_SAMCHAR;

/******************************************************************
*                �������¼������ͣ�SIP_Type��						*

��������ʾʵ��ʵʩ����/�յ������¼��ľ������͡�
���ȣ�1BYTE
��ʽ��HEX

ȡֵ 	˵�� 		��ע
0 		����
1 		��������
2 		������ƫ����
3-255 	����
******************************************************************/

typedef enum
{
    SIPTYPE_RES = 0,
    SIPTYPE_SMOG,
    SIPTYPE_LASER,
} ENUM_SIPTYPE;

/******************************************************************
*                ��/ͣ������ʶ�֣�SS_Character��				*

����������ʶ��ʵ�忪ʼ������ֹͣ��������������
���ȣ�1BYTE
��ʽ��HEX

ȡֵ 	˵�� 	��ע
0 		����
1 		��ʼ
2 		ֹͣ
3-255 	����

******************************************************************/

typedef enum
{
    SSCHAR_RES = 0,
    SSCHAR_START,
    SSCHAR_STOP,
} ENUM_SSCHAR;

/******************************************************************
*                ���ý����ʶ�֣�SR_Character��				*

��������ʾ���ý����
���ȣ�1BYTE
��ʽ��HEX

ȡֵ 	˵�� 	��ע
0 		ʧ��
1 		�ɹ�
2-255 	����


******************************************************************/

typedef enum
{
    SRCHAR_FAILURE = 0,
    SRCHAR_SUCCESS,
} ENUM_SRCHAR;


/******************************************************************
*            ʵ������				*

���������豸���
���ȣ�1BYTE
��ʽ��HEX


******************************************************************/
typedef enum
{
    T_TYPE_Tank = 0X01,								//̹�ˣ��ǻ��ģ�⣩
    T_TYPE_TankE,									//̹�ˣ�Ƕ��ʽģ�⣩
    T_TYPE_InfantryFightingVehicle,					//��ս��
    T_TYPE_ArmouredCarrier,							//װ�����ͳ�
    T_TYPE_RaiderBuggy, 							//ͻ����
    T_TYPE_AssaultGun, 								//ͻ����
    T_TYPE_AntiTankMissileSystem,					//��̹�˵���ϵͳ
    //0x08-0x0F 	����

    T_TYPE_TowCannon = 0X10, 						//ǣ������
    T_TYPE_NonDigitalSelfPropelledArtillery, 		//���л��ڣ������ֻ���
    T_TYPE_DigitalSelfPropelledArtillery,			//���л��ڣ����ֻ�)
    T_TYPE_Rocket,									//�����
    T_TYPE_Mortar,									//�Ȼ���
    T_TYPE_ArtilleryReconnaissance,					//�ڱ����
    //0x16-0x1F  	����

    T_TYPE_AntiAircraftGun = 0X20,					//���ڣ��л�أ�
    T_TYPE_NoFireControlAntiAircraftGun,			//���ڣ��޻�أ�
    T_TYPE_AntiAircraftMissileSystem,				//���յ���ϵͳ
    T_TYPE_AntiAircraftRadarSystem,					//�����״�ϵͳ
    T_TYPE_AntiAircraftCommandSystem,				//����ָ��ϵͳ
    T_TYPE_ManAntiAircraftMissile,					//�������յ���
    //0x26-0x2F 	����

    T_TYPE_Personnel = 0X30,						//��Ա
    T_TYPE_AutomaticRifle,							//�Զ���ǹ
    T_TYPE_MachineGun,								//���û�ǹ
    T_TYPE_SniperRifle,								//�ѻ���ǹ
    T_TYPE_SubmachineGun,							//΢�����ǹ
    T_TYPE_ShotGun,									//����ǹ
    T_TYPE_Pistol,									//��ǹ
    T_TYPE_PrivateGrenadeLauncher,					//�����񵯷�����
    T_TYPE_HeavyMachineRifle,							//�ػ�ǹ
    T_TYPE_AntiAircraftRifle,							//�����ǹ
    T_TYPE_OpticalReconnaissanceEquipment,			//��ѧ����豸
    //0x3B-0x3F 	����

    T_TYPE_IndividualRocket = 0X40,					//�������
    T_TYPE_ManAntiTankRocket,						//������̹�˻��
    T_TYPE_ManAntiTankmissile,						//������̹�˵���
    T_TYPE_Mine,									//����
    T_TYPE_PrivateMineDeminer,						//����������
    T_TYPE_HandGrenade,								//����
    T_TYPE_ImprovisedExplosive,						//���ױ�ը��
    T_TYPE_MedicalServiceAndAmbulanceEquipment,					//���ھȻ�װ��
    T_TYPE_ConditioningEquipment,					//�����豸
    //0x49-0x4F  	����

    T_TYPE_GmVehicle = 0X50, 						//ͨ�ó���
    T_TYPE_CommandVehicle,							//ָ�ӳ���
    T_TYPE_CommunicationVehicle,					//ͨ�ų���
    T_TYPE_AmmunitionCarrier,						//��ҩ���ͳ�
    T_TYPE_OilVehicle,								//�������䳵
    T_TYPE_RocketMineVehicle,						//������׳�
    T_TYPE_EngineeringReconnaissanceVehicle,		//������쳵
    T_TYPE_LightObstructorVehicle,					//�������ϳ�
    T_TYPE_IntegratedMinesweeperVehicle,			//�ۺ�ɨ�׳�
    T_TYPE_HeavyMechanizedBridgeVehicle,			//���ͻ�е����
    T_TYPE_EmergencyVehicle,						//���޳���
    T_TYPE_EngineeringVehicle,						//������ҵ����
    //0x5C-0x5F  	����

    T_TYPE_Helicopter = 0X60,						//ֱ����
    T_TYPE_Drone,									//���˻�
    T_TYPE_Bomber,									//ս����/��ը��
    T_TYPE_ElectronicCountermeasure,				//���ӶԿ�
    T_TYPE_FortificationsFacilities,				//������ʩ
    T_TYPE_BattlefieldObstacle,						//ս���ϰ�
    //0x66-0xFE 	����

    T_TYPE_MasterStation = 0XFF,					//����̨

} ENUM_T_TYPE;




typedef enum
{
    T_Subtype_Tank96 = 0X01,						//96 ʽ̹��
    T_Subtype_Tank88B,								//88B ʽ̹��
    T_Subtype_Tank62,								//62 ʽ̹��
    T_Subtype_Tank59,								//59 ʽ̹��
    T_Subtype_TankM60A3, 							//M60A3 ̹��
    T_Subtype_Tank63A, 								//63A ʽ̹��
    T_Subtype_Tank59G,								//59 ��ʽ̹��
    T_Subtype_TankM1A1,								//M1A1 ̹��
    T_Subtype_Tank99,								//99 ʽ̹��
    T_Subtype_Tank96A,								//96A ʽ̹��
} ENUM_T_Subtype_Tank;

typedef enum
{
    T_Subtype_TankE96A   = 0X01,					//96A Ƕ��ʽ̹��
} ENUM_T_Subtype_TankE;

typedef enum
{
    T_Subtype_InfantryFightingVehicle86 = 0X01,							//86 ʽ��ս��
    T_Subtype_InfantryFightingVehicle92,								//92 ��ʽ��ս��
    T_Subtype_InfantryFightingVehicle04,								//04 ʽ��ս��
    T_Subtype_InfantryFightingVehicle05,								//05 ʽ��ս��
    T_Subtype_InfantryFightingVehicle05A, 								//05 ʽ����װ�ײ���ս��
    T_Subtype_InfantryFightingVehicle08, 								//08 ʽ��ʽ��ս��

} ENUM_T_Subtype_InfantryFightingVehicle;

typedef enum
{
    T_Subtype_ArmouredCarrier63C = 0X01,								//63C ʽװ�����ͳ�
    T_Subtype_ArmouredCarrier001,										//001 ������װ�����ͳ�
    T_Subtype_ArmouredCarrier89,										//89 ʽװ�����ͳ�
    T_Subtype_ArmouredCarrier10,										//10 ʽװ�����ͳ�
} ENUM_T_Subtype_ArmouredCarrier;

typedef enum
{
    T_Subtype_RaiderBuggy05 = 0X01,										//05 ʽͻ����
    T_Subtype_RaiderBuggy11,											//11 ʽͻ����
    T_Subtype_RaiderBuggy03,											//03 ʽͻ����
    T_Subtype_RaiderBuggy05A,											//05 ʽ����װ��ͻ����
} ENUM_T_Subtype_RaiderBuggy;

typedef enum
{
    T_Subtype_AssaultGun100 = 0X01,										//100mm ��ʽͻ����
    T_Subtype_AssaultGun120,											//120mm ��̹�˻�����
} ENUM_T_Subtype_AssaultGun;

typedef enum
{
    T_Subtype_AntiTankMissileSystemRedArrow9 = 0X01,					//��� 9 ��̹�˵���ϵͳ
    T_Subtype_AntiTankMissileSystemRedArrow10,							//��� 10 ��̹�˵���ϵͳ
} ENUM_T_Subtype_AntiTankMissileSystem;

typedef enum
{
    T_Subtype_TowCannon122 = 0X01,										//PL83/96 ʽ 122 ����
    T_Subtype_TowCannon130,												//PL59-1 ʽ 130 ��ũ��
    T_Subtype_TowCannon152,												//PL66 ʽ 152 ������
} ENUM_T_Subtype_TowCannon;

typedef enum
{
    T_Subtype_NonDigitalSelfPropelledArtillery122 = 0X01,				//PLZ89 ʽ 122 ����
    T_Subtype_NonDigitalSelfPropelledArtillery152,						//PLZ83 ʽ 152 ������
} ENUM_T_Subtype_NonDigitalSelfPropelledArtillery;

typedef enum
{
    T_Subtype_DigitalSelfPropelledArtillery155 = 0X01,					//PLZ05 ʽ 155 ������
    T_Subtype_DigitalSelfPropelledArtilleryZ122,						//PLZ07 ʽ 122 ����
    T_Subtype_DigitalSelfPropelledArtilleryC122,						//PLC09 ʽ 122 ����
    T_Subtype_DigitalSelfPropelledArtillery120,							//PLL05 ʽ 120mm ����������
    T_Subtype_DigitalSelfPropelledArtilleryA, 							//������������

} ENUM_T_Subtype_DigitalSelfPropelledArtillery;

typedef enum
{
    T_Subtype_RocketL122 = 0X01,										//PHL81 ʽ 122 �����
    T_Subtype_RocketZ122,												//PHZ89 ʽ 122 �����
    T_Subtype_Rocket300,												//PHZ05 ʽ 300mm Զ�̻����
} ENUM_T_Subtype_Rocket;



typedef enum
{
    T_Subtype_Mortar82 = 0X01,											//89 ʽ 82mm �Ȼ���
    T_Subtype_Mortar100,												//89 ʽ 100mm �Ȼ���
    T_Subtype_Mortar120,												//120mm �Ȼ���
    T_Subtype_Mortar60, 												//89 ʽ 60mm �Ȼ���
    T_Subtype_Mortar82S,												//82mm �����Ȼ���

} ENUM_T_Subtype_Mortar;


typedef enum
{
    T_Subtype_ArtilleryReconnaissanceRadar = 0X01,						//��λ����״�
    T_Subtype_ArtilleryReconnaissanceVehicle,							//�ڱ���쳵
    T_Subtype_ArtilleryReconnaissanceMobileTargetRadar,				//�ڱ��Ŀ������״�
    T_Subtype_ArtilleryReconnaissanceBlastPointIndicator, 				//�ڱ������ģ��ը��ָʾ��
} ENUM_T_Subtype_ArtilleryReconnaissance;


typedef enum
{
    T_Subtype_AntiAircraftGunPZK091A35 = 0X01,							//PZK091A35 ���׸���ս��
    T_Subtype_AntiAircraftGunPG99,										//PG99 ʽ 35 ���׸������ݲɼ��豸
    T_Subtype_AntiAircraftGunPGZ04,									//PGZ04 �͵��ڽ������ϵͳս��
    T_Subtype_AntiAircraftGunPLB625, 									//PLB625 ���ڽ������ս��
} ENUM_T_Subtype_AntiAircraftGun;

typedef enum
{
    T_Subtype_NoFireControlAntiAircraftGun37 = 0X01,					//˫ 37 ����
    T_Subtype_NoFireControlAntiAircraftGun25,							//˫ 25 ����
    T_Subtype_NoFireControlAntiAircraftGun25S,							//�ģ�25 ����
    T_Subtype_NoFireControlAntiAircraftGun57,							//57mm ����

} ENUM_T_Subtype_NoFireControlAntiAircraftGun;

typedef enum
{
    T_Subtype_AntiAircraftMissileSystemRedFlag17S = 0X01,				//���� 17 ��������ϵͳ S ��
    T_Subtype_AntiAircraftMissileSystemRedFlag17F,						//���� 17 ��������ϵͳ F ��
    T_Subtype_AntiAircraftMissileSystemRedFlag16AD,					//���� 16A/D ��������ϵͳ���ݲɼ��豸
    T_Subtype_AntiAircraftMissileSystemRedFlag7BS,						//���� 7B �������� S ��ϵͳ���ݲɼ��豸
    T_Subtype_AntiAircraftMissileSystemRedFlag7BF, 						//���� 7B �������� F ��ϵͳ���ݲɼ��豸

} ENUM_T_Subtype_AntiAircraftMissileSystem;



typedef enum
{
    T_Subtype_AntiAircraftRadarSystemLLP12A = 0X01,						//LLP12A ����״����ݲɼ��豸
    T_Subtype_AntiAircraftRadarSystem,									//�鱨�״����ݲɼ��豸����׼�ͣ�

} ENUM_T_Subtype_AntiAircraftRadarSystem;

typedef enum
{
    T_Subtype_AntiAircraftCommandSystemBrigadeStation = 0X01,			//ָ����Ϣϵͳ�ã�Ӫ��վ���ݲɼ��豸
    T_Subtype_AntiAircraftCommandSystemBatteryStation,					//ָ����Ϣϵͳ������վ���ݲɼ��豸

} ENUM_T_Subtype_AntiAircraftCommandSystem;

typedef enum
{
    T_Subtype_ManAntiAircraftMissileRedTassel5 = 0X01,					//��ӧ 5
    T_Subtype_ManAntiAircraftMissileRedTassel6,							//��ӧ 6 ��Яʽ���յ���
    T_Subtype_ManAntiAircraftMissileAvantGarde2,						//ǰ�� 2 ��Яʽ���յ���
    T_Subtype_ManAntiAircraftMissileRedTassel6B,						//��ӧ 6 ��Яʽ����
    T_Subtype_ManAntiAircraftMissileAvantGarde2B, 						//ǰ�� 2 ��Яʽ���յ���

} ENUM_T_Subtype_ManAntiAircraftMissile;

typedef enum
{
    T_Subtype_PersonnelIndividual    = 0X01,							//����װ��
    T_Subtype_PersonnelIndividualSpecial,								//����װ�ߣ���ս�ͣ�
    T_Subtype_PersonnelIndividualAmphibious,							//����װ�ߣ������ͣ�
} ENUM_T_Subtype_Personnel;

typedef enum
{
    T_Subtype_AutomaticRifle95    = 0X01,								//95 ʽ 5.8mm �Զ���ǹ
    T_Subtype_AutomaticRifleAK47,										//AK47 �Զ���ǹ
    T_Subtype_AutomaticRifle95_1,										//95-1 ʽ 5.8mm �Զ���ǹ
    T_Subtype_AutomaticRifle03,											//03 ʽ 5.8mm �Զ���ǹ
    T_Subtype_AutomaticRiflePrecision,									//5.8mm ��׼��ǹ
    T_Subtype_AutomaticRifle95B,										//95B ʽ 5.8mm �̲�ǹ
    T_Subtype_AutomaticRifle95S,										//95 ʽ 5.8mm �Զ���ǹ����ս�ͣ�
    T_Subtype_AutomaticRifle95_1S,										//95-1 ʽ 5.8mm �Զ���ǹ����ս�ͣ�
    T_Subtype_AutomaticRifle95BS,										//95B ʽ 5.8mm �̲�ǹ����ս�ͣ�
    T_Subtype_AutomaticRifle95_1BS,										//95-1 ʽ 5.8mm �̲�ǹ����ս�ͣ�
    T_Subtype_AutomaticRifle95A,										//95 ʽ 5.8mm �Զ���ǹ�������ͣ�
    T_Subtype_AutomaticRifle95_1A,										//95-1 ʽ 5.8mm �Զ���ǹ�������ͣ�
    T_Subtype_AutomaticRifle95BA,										//95B ʽ 5.8mm �̲�ǹ�������ͣ�
    T_Subtype_AutomaticRifle95_1BA,										//95-1 ʽ 5.8mm �̲�ǹ�������ͣ�

} ENUM_T_Subtype_AutomaticRifle;

typedef enum
{
    T_Subtype_MachineGun95    = 0X01,									//95 ʽ 5.8mm ���û�ǹ
    T_Subtype_MachineGun95_1,											//95-1 ʽ 5.8mm ���û�ǹ
    T_Subtype_MachineGun95_1S,											//95-1 ʽ 5.8mm ���û�ǹ����ս�ͣ�
    T_Subtype_MachineGun95A,											//95 ʽ 5.8mm ���û�ǹ�������ͣ�
    T_Subtype_MachineGun95_1A,											//95-1 ʽ 5.8mm ���û�ǹ�������ͣ�

} ENUM_T_Subtype_MachineGun;

typedef enum
{
    T_Subtype_SniperRifle88_58mm    = 0X01,								//88 ʽ 5.8mm �ѻ�ǹ
    T_Subtype_SniperRifle09,											//09 �ѻ���ǹ
    T_Subtype_SniperRifleHighPrecision,									//7.62mm �߾��Ⱦѻ�ǹ
    T_Subtype_SniperRifle10_127mm,										//10 ʽ 12.7mm �ѻ�ǹ
    T_Subtype_SniperRifle58mmS,											//5.8mm �ѻ�ǹ����ս�ͣ�
    T_Subtype_SniperRifle86mmS,											//8.6mm �ѻ�ǹ����ս�ͣ�
    T_Subtype_SniperRifle127mmS,										//12.7mm �ѻ�ǹ����ս�ͣ�
    T_Subtype_SniperRifle58mmA,											//5.8mm �ѻ�ǹ�������ͣ�
    T_Subtype_SniperRifle762mmA,										//7.62mm �ѻ�ǹ�������ͣ�
    T_Subtype_SniperRifle10_127mmA,										//10 ʽ 12.7mm �ѻ�ǹ�������ͣ�

} ENUM_T_Subtype_SniperRifle;

typedef enum
{
    T_Subtype_SubmachineGun05_58mm     = 0X01,							//05 ʽ 5.8mm ΢�����ǹ
    T_Subtype_SubmachineGun05_58mmS,									//05 ʽ 5.8mm ΢�����ǹ����ս�ͣ�
    T_Subtype_SubmachineGun05_58mmA,									//05 ʽ 5.8mm ΢�����ǹ�������ͣ�

} ENUM_T_Subtype_SubmachineGun;

typedef enum
{
    T_Subtype_ShotGun09_184mm   = 0X01,									//09 ʽ 18.4mm ����ǹ
    T_Subtype_ShotGun09_184mmS,											//09 ʽ 18.4mm ����ǹ����ս�ͣ�
    T_Subtype_ShotGun09_184mmA,										//09 ʽ 18.4mm ����ǹ�������ͣ�

} ENUM_T_Subtype_ShotGun;


typedef enum
{
    T_Subtype_Pistol = 0X01,												//��ǹ
    T_Subtype_PistolS,													//��ǹ

} ENUM_T_Subtype_Pistol;

typedef enum
{
    T_Subtype_PrivateGrenadeLauncher87_35mm      = 0X01,				//87 ʽ 35mm �Զ��񵯷�����
    T_Subtype_PrivateGrenadeLauncher04_35mm,							//04 ʽ 35mm �Զ��񵯷�����
    T_Subtype_PrivateGrenadeLauncher131_35mm,							//131 �� 35mm �ѻ��񵯷�����
    T_Subtype_PrivateGrenadeLauncher10_35mm,							//10 ʽ 35mm ǹ���񵯷�����
    T_Subtype_PrivateGrenadeLauncher87_35mmS,						//87 ʽ 35mm �Զ��񵯷���������ս�ͣ�
    T_Subtype_PrivateGrenadeLauncher04_35mmS,							//04 ʽ 35mm �Զ��񵯷���������ս�ͣ�
    T_Subtype_PrivateGrenadeLauncher131_35mmS,							//131 �� 35mm �ѻ��񵯷���������ս�ͣ�
    T_Subtype_PrivateGrenadeLauncher87_35mmA,						//87 ʽ 35mm �Զ��񵯷������������ͣ�
    T_Subtype_PrivateGrenadeLauncher04_35mmA,							//04 ʽ 35mm �Զ��񵯷������������ͣ�
    T_Subtype_PrivateGrenadeLauncher131_35mmA,							//131 �� 35mm �ѻ��񵯷������������ͣ�
    T_Subtype_PrivateGrenadeLauncher10_35mmA,							//10 ʽ 35mm ǹ���񵯷������������ͣ�

} ENUM_T_Subtype_PrivateGrenadeLauncher;


typedef enum
{
    T_Subtype_HeavyMachineRifle89_127mm    = 0X01,						//89 ʽ 12.7mm �ػ�ǹ
    T_Subtype_HeavyMachineRifleQBZ171_127mm,							//QBZ171 ɽ���� 12.7mm �ػ�ǹ
    T_Subtype_HeavyMachineRifle89_127mmS,								//89 ʽ 12.7mm �ػ�ǹ����ս�ͣ�
    T_Subtype_HeavyMachineRifle89_127mmA,								//89 ʽ 12.7mm �ػ�ǹ�������ͣ�

} ENUM_T_Subtype_HeavyMachineRifle;

typedef enum
{
    T_Subtype_AntiAircraftRifle85_127mm     = 0X01,						//85 ʽ 12.7mm �����ǹ
    T_Subtype_AntiAircraftRifle85_127mmA,								//85 ʽ 12.7mm �����ǹ�������ͣ�

} ENUM_T_Subtype_AntiAircraftRifle;

typedef enum
{
    T_Subtype_OpticalReconnaissanceEquipmentBaz308       = 0X01,		//Baz-308 ս��������
    T_Subtype_OpticalReconnaissanceEquipmentBad902C,					//Bad-902C Զ����������

} ENUM_T_Subtype_OpticalReconnaissanceEquipment;

typedef enum
{
    T_Subtype_IndividualRocket80mm    = 0X01,							//80 ��
    T_Subtype_IndividualRocket11_93mm,									//11 ʽ 93mm �������Ͳ
    T_Subtype_IndividualRocket89_80mm,									//PF89 ʽ 80mm �������Ͳ
    T_Subtype_IndividualRocket11_93mmG,									//11 ʽ 93mm �������Ͳ?
    T_Subtype_IndividualRocket98_93mm,									//PF98 ʽ 93mm �������Ͳ

} ENUM_T_Subtype_IndividualRocket;

typedef enum
{
    T_Subtype_ManAntiTankRocket98B_120mm = 0X01,						//PF98 ʽӪ�� 120mm ��̹�˻��Ͳ
    T_Subtype_ManAntiTankRocket98C_120mm,								//PF98 ʽ���� 120mm ��̹�˻��Ͳ
    T_Subtype_ManAntiTankRocket98B_120mmA,								//PF98 ʽӪ�� 120mm ��̹�˻��Ͳ�������ͣ�
    T_Subtype_ManAntiTankRocket98C_120mmA,								//PF98 ʽ���� 120mm ��̹�˻��Ͳ�������ͣ�
} ENUM_T_Subtype_ManAntiTankRocket;

typedef enum
{
    T_Subtype_ManAntiTankRocketRedArrow73 = 0X01,						//��� 73 �ķ�̹�˵���
    T_Subtype_ManAntiTankRocketRedArrow8,								//��� 8 ��̹�˵���
    T_Subtype_ManAntiTankRocketRedArrow12,								//��� 12 ��̹�˵���
    T_Subtype_ManAntiTankRocketRedArrow8G,								//��� 8 ��̹�˵��� ?
} ENUM_T_Subtype_ManAntiTankmissile;


typedef enum
{
    T_Subtype_MineAntiInfantry = 0X01,									//ѹ��ʽ����������
    T_Subtype_MineAntiTank,												//��̹�˵���
    T_Subtype_MineTrippingHairAntiInfantry,								//��ʽ����������
    T_Subtype_MineDirectional,											//������
} ENUM_T_Subtype_Mine;

typedef enum
{
    T_Subtype_PrivateMineDeminer = 0X01,								//����������
    T_Subtype_PrivateMineDeminerSapper,									//����ģ����
} ENUM_T_Subtype_PrivateMineDeminer;

typedef enum
{
    T_Subtype_HandGrenade82 = 0X01,										//82-2 ʽ����
    T_Subtype_HandGrenade77,											//77-1 ʽ����
} ENUM_T_Subtype_HandGrenade;

typedef enum
{
    T_Subtype_ImprovisedExplosive = 0X01,								//��ը��
} ENUM_T_Subtype_ImprovisedExplosive;


typedef enum
{
    T_Subtype_MedicalServiceAndAmbulanceEquipment = 0X01,				//���ھȻ�
} ENUM_T_Subtype_MedicalServiceAndAmbulanceEquipment;

typedef enum
{
    T_Subtype_ConditioningEquipmentGun = 0X01,							//����ǹ
    T_Subtype_ConditioningEquipmentLoadAmmo,							//ģ��װ����
    T_Subtype_ConditioningEquipmentTerminal,							//�����ն�

} ENUM_T_Subtype_ConditioningEquipment;

typedef enum
{
    T_Subtype_GmVehicle = 0X01,											//ͨ�ó���
    T_Subtype_GmVehicleA,												//����ͨ�÷ǻ�������
} ENUM_T_Subtype_GmVehicle;

typedef enum
{
    T_Subtype_CommandVehicle    = 0X01,									//ָ�ӳ�
    T_Subtype_CommandVehicleArtillery,									//�ڱ�ָ�ӳ�
    T_Subtype_CommandVehiclePZK091A35,									//PZK091A35 ���׸���ָ�ӳ�
    T_Subtype_CommandVehiclePGZ04,										//PGZ04 �͵��ڽ������ϵͳָ�ӳ�
    T_Subtype_CommandVehiclePLB625,										//PLB625 ���ڽ������ָ�ӳ�

} ENUM_T_Subtype_CommandVehicle;

typedef enum
{
    T_Subtype_CommunicationVehicle = 0X01,								//ͨ�ų�
    T_Subtype_CommunicationVehicleArtillery,							//�ڱ�ͨ�Žڵ㳵
} ENUM_T_Subtype_CommunicationVehicle;

typedef enum
{
    T_Subtype_AmmunitionCarrier = 0X01,									//��ҩ���ͳ�
} ENUM_T_Subtype_AmmunitionCarrier;

typedef enum
{
    T_Subtype_OilVehicle = 0X01,										//�������䳵
} ENUM_T_Subtype_OilVehicle;

typedef enum
{
    T_Subtype_RocketMineVehicle = 0X01,									//���׳�
} ENUM_T_Subtype_RocketMineVehicle;

typedef enum
{
    T_Subtype_EngineeringReconnaissanceVehicle = 0X01,					//������쳵
    T_Subtype_ReconnaissanceVehicle,									//��쳵
} ENUM_T_Subtype_EngineeringReconnaissanceVehicle;

typedef enum
{
    T_Subtype_LightObstructorVehicle = 0X01,							//���ϳ�
    T_Subtype_LightObstructorVehicleCSL132A,							//CSL132 ������װ�����ϳ�
} ENUM_T_Subtype_LightObstructorVehicle;

typedef enum
{
    T_Subtype_IntegratedMinesweeperVehicle = 0X01,						//�ۺ�ɨ�׳�
} ENUM_T_Subtype_IntegratedMinesweeperVehicle;

typedef enum
{
    T_Subtype_HeavyMechanizedBridgeVehicle84A = 0X01,					//84A ʽ���ͻ�е����
} ENUM_T_Subtype_HeavyMechanizedBridgeVehicle;

typedef enum
{
    T_Subtype_EmergencyVehicle = 0X01,									//װ�����޳�
} ENUM_T_Subtype_EmergencyVehicle;

typedef enum
{
    T_Subtype_EngineeringVehicleCCZ111 = 0X01,							//CCZ111 �����ܹ�����ҵ��
} ENUM_T_Subtype_EngineeringVehicle;

typedef enum
{
    T_Subtype_Helicopter10    = 0X01,				//ֱ 10 ��װֱ����
    T_Subtype_Helicopter19,							//ֱ 19 ��װֱ����
    T_Subtype_Helicopter20,							//ֱ 20 ϵ��ֱ����
    T_Subtype_Helicopter17,							//�� 17 ϵ��ֱ����
    T_Subtype_Helicopter8,						//ֱ 8 ϵ��ֱ����
    T_Subtype_Helicopter9,							//ֱ 9 ��װֱ����
    T_Subtype_HelicopterArm,						//��װֱ����
    T_Subtype_HelicopterTransport,			//����ֱ����

} ENUM_T_Subtype_Helicopter;

typedef enum
{
    T_Subtype_DroneRainbow3    = 0X01,				//�ʺ�-3
    T_Subtype_DroneRainbow4,						//�ʺ�-4
    T_Subtype_DronePterosaur1,						//����-1
    T_Subtype_DroneMQ9,								//MQ-9
    T_Subtype_DroneRQ1,							//RQ-1
    T_Subtype_DroneRQ4,								//RQ-4

} ENUM_T_Subtype_Drone;

typedef enum
{
    T_Subtype_BomberH6    = 0X01,					//�� 6
    T_Subtype_BomberH7,								//�ߺ� 7
    T_Subtype_BomberJ10,							//�� 10
    T_Subtype_BomberJ11,							//�� 11
    T_Subtype_BomberJ15,							//�� 15
    T_Subtype_BomberJ16,							//�� 16
    T_Subtype_BomberJ20,							//�� 20
    T_Subtype_BomberS27,							//�� 27
    T_Subtype_BomberS30,							//�� 30
    T_Subtype_BomberS35,							//�� 35
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
    T_Subtype_BattlefieldElectromagneticEnvironmentDataAcquisitionEquipment = 0X01,					//ս����Ż������ݲɼ��豸
} ENUM_T_Subtype_ElectronicCountermeasure;


typedef enum
{
    T_Subtype_Shelter = 0X01,						//������
    T_Subtype_Bunker,								//�ر�
    T_Subtype_SimpleFortificationsFacilities,		//���׹���
    T_Subtype_StrongFortificationsFacilities,		//��̹���
    T_Subtype_Building,							//������
    T_Subtype_CommandPost,							//ָ����
    T_Subtype_Ammunition,							//��ҩ��
    T_Subtype_FuelTank,							//���Ͽ�
    T_Subtype_MaterialWarehouse,					//���ʿ�
    T_Subtype_RadarStation,							//�״�վ
    T_Subtype_Bridge,							//����
    T_Subtype_RailwayStation,						//��վ
    T_Subtype_Runway,								//�ܵ�
    T_Subtype_Hangar,								//����
    T_Subtype_AimingPointBetween,					//�������
    T_Subtype_RemoteSmokeGenerator,					//ң����������

} ENUM_T_Subtype_FortificationsFacilities;

typedef enum
{
    T_Subtype_BarbedWire = 0X01,														//��˿��
    T_Subtype_ThreePyramid,																//����׶
    T_Subtype_Moat,																		//����
    T_Subtype_AntiTankTrench,															//��̹��ǵ��
    T_Subtype_VirtualInfantryMinefield,												//����������׳�
    T_Subtype_VirtualAntiTankMinefield,													//���̹ⷴ���׳�
    T_Subtype_VirtualMixedMinefield,													//�������׳�
    T_Subtype_PhysicalInfantryMinefield1,												//ʵ��������׳�1
    T_Subtype_PhysicalInfantryMinefield2,												//ʵ��������׳�2
    T_Subtype_PhysicalInfantryMinefield3,												//ʵ��������׳�3
    T_Subtype_VirtualNuclearAndBiologicalContaminationArea,							//����˻���մȾ��

} ENUM_T_Subtype_BattlefieldObstacle;


/***********************************************************
 		H_Part 	���в�λ �����ࣨ0x13��
************************************************************/

typedef enum
{
    H_Part_I_Head = 1,											//ͷ��
    H_Part_I_Skull,												//­��
    H_Part_I_Neck,												//ͷ�澱
    H_Part_I_Neck_Eye,											//ͷ�澱����
    H_Part_I_Head_Chest,										//ͷ/��
    H_Part_I_Head_Abdomen_Kidney,								//ͷ/��/��
    H_Part_I_Head_Abdomen_Spleen,								//ͷ/��/Ƣ
    H_Part_I_Head_Abdomen_Liver,								//ͷ/��/����
    H_Part_I_Head_Abdomen_Colon,								//ͷ/��/�᳦
    H_Part_I_Head_Abdomen_Bladder,								//ͷ/��/����		10
    H_Part_I_Head_LowerLimb,									//ͷ/��֫
    H_Part_I_Face,												//�沿
    H_Part_I_Face_Neck,											//��/��
    H_Part_I_Eye,												//��
    H_Part_I_Ear,												//��
    H_Part_I_Jaw,												//���
    H_Part_I_Shoulder,											//��
    H_Part_I_Chest,												//�ز�
    H_Part_I_Chest_Abdomen_Kidney,								//��/��/����
    H_Part_I_Chest_Abdomen_Bladder,								//��/��/���� 		20
    H_Part_I_Chest_Abdomen_Spleen,								//��/��/Ƣ
    H_Part_I_Chest_Abdomen_Liver,								//��/��/����
    H_Part_I_Chest_Abdomen_Colon,								//��/��/�᳦
    H_Part_I_Chest_Abdomen_Kidney_Bladder,						//��/��/�᳦/����
    H_Part_I_Chest_Limb,										//��/��֫
    H_Part_I_Chest_UpperLimb_Abdomen,							//��/��֫/��
    H_Part_I_Chest_Abdomen,										//��/��
    H_Part_I_Liver,												//��
    H_Part_I_Spleen,											//Ƣ
    H_Part_I_Kidney,											//�� 	30
    H_Part_I_Colon,												//С��
    H_Part_I_Abdomen,											//����
    H_Part_I_Abdomen_Colon_Spleen,								//��/�᳦/Ƣ
    H_Part_I_Abdomen_Colon_Liver,								//��/�᳦/����
    H_Part_I_Abdomen_Colon_Bladder,								//��/�᳦/����
    H_Part_I_Abdomen_Pelvic_Liver_Kidney,						//��/��ǻ/��/����
    H_Part_I_Abdomen_Pelvic_Spleen_Bladder,						//��/��ǻ/Ƣ/����
    H_Part_I_Abdomen_Colon_LowerLimb,							//��/�᳦/��֫
    H_Part_I_Abdomen_Pelvic_LowerLimb,							//��/��ǻ/��֫
    H_Part_I_Abdomen_LowerLimb,									//��/��֫ 			40
    H_Part_I_Abdomen_Pelvic_Limb,										//��/��ǻ/��֫
    H_Part_I_Abdomen_Limb,						//��/��֫
    H_Part_I_Spine,												//����
    H_Part_I_UpperLimb,											//��֫
    H_Part_I_Humerus,											//�Ź�
    H_Part_I_Hand,												//��
    H_Part_I_Finger,											//��ָ
    H_Part_I_Hand_Finger,										//��/��ָ
    H_Part_I_Elbow,												//��
    H_Part_I_Wrist,												//�� 		50
    H_Part_I_CarpalBone,										//��
    H_Part_I_Forearm,											//ǰ��
    H_Part_I_Ulna_Radial,										//�߹�/���
    H_Part_I_Trunk,												//����
    H_Part_I_Rib,												//�߹�
    H_Part_I_Back,												//����
    H_Part_I_Hip,												//�β�
    H_Part_I_Pelvis,											//����
    H_Part_I_Femoral,											//�ɹ�
    H_Part_I_Genital,											//��ֳ�� 60
    H_Part_I_Thigh,												//����
    H_Part_I_Knee,												//ϥ
    H_Part_I_PrayerBone,										//ϥ��
    H_Part_I_Foot,												//��
    H_Part_I_Ankle,												//��
    H_Part_I_Foot_Ankle,										//��/��
    H_Part_I_Foot_Ankle_Heel,									//��/��/��
    H_Part_I_AboveKnee,											//ϥ������
    H_Part_I_BelowKnee,											//ϥ������
    H_Part_I_LowerLimb_Genital,									//��֫����ֳ�� 70
    H_Part_I_LowerLimb,											//��֫
    H_Part_I_WholeBody,											//ȫ��

} ENUM_H_Part_INDIVIDUAL ;

/***********************************************************
 		H_Part 	���в�λ ̹�ˣ�0x01�� ͻ������0x04��
************************************************************/
typedef enum
{
    H_Part_T_WholeVehicle = 0,											//����
    H_Part_T_FrontHigherArmorRight,												//����װ����
    H_Part_T_FrontHigherArmorLeft,											//����װ����
    H_Part_T_FrontLowerArmorRight,											//����װ����
    H_Part_T_FrontLowerArmorLeft,											//����װ����
    H_Part_T_RightSideTrackFront,											//�Ҳ��Ĵ�����
    H_Part_T_LefttSideTrackFront,											//����Ĵ�����
    H_Part_T_RightFrontOfHull,											//�����Ҳ���ǰ��
    H_Part_T_RightMiddleOfHull,											//�����Ҳ����в�
    H_Part_T_RightRearOfHull,											//�����Ҳ����
    H_Part_T_LeftFrontOfHull,											//���������ǰ��
    H_Part_T_LeftMiddleOfHull,											//����������в�
    H_Part_T_LeftRearOfHull,											//����������
    H_Part_T_RightBehindTrack,											//�Ҳ��Ĵ�����
    H_Part_T_LeftBehindTrack,											//����Ĵ�����
    H_Part_T_CannonAndShield,											//���ڼ�����
    H_Part_T_TurretFrontLeft,											//�����������
    H_Part_T_TurretFrontRight,											//���������Ҳ�
    H_Part_T_AntiAircraftGun,											//�����ǹ
    H_Part_T_leftSideOfTurret,											//�������
    H_Part_T_RightSideOfTurret,											//�����Ҳ�
    H_Part_T_RearTurretLeft,											//���������
    H_Part_T_RearTurretRight,											//�������Ҳ�

} ENUM_H_Part_TANK_RaiderBuggy ;

/***********************************************************
 		H_Part 	���в�λ ����װ��������(0x02-0x03; 0x05-0x12)
************************************************************/
typedef enum
{
    H_Part_A_WholeVehicle = 0,											//����
    H_Part_A_FrontLeftOfVehicle,										//������ǰ��
    H_Part_A_FrontOfVehicle,											//����ǰ��
    H_Part_A_FrontRightOfVehicle,										//������ǰ��
    H_Part_A_LeftSideOfVehicle,											//������ಿ
    H_Part_A_RightSideOfVehicle,										//�����Ҳಿ
    H_Part_A_RearLeftOfVehicle,											//�������
    H_Part_A_RearOfVehicle,												//������
    H_Part_A_RearRightOfVehicle,										//�����Һ�
    H_Part_A_VehicleChassis,											//��������

} ENUM_H_Part_Armor ;

/***********************************************************
 		H_Type 	����     �����ࣨ0x13��
************************************************************/
typedef enum
{
    H_Type_I_NuclearChemicalBiological = 0,		//�˷������� ��ѧ���� ��������
    H_Type_I_Burn,		//����
    H_Type_I_Dislocated,		//�Ѿ�
    H_Type_I_LimbSevered,		//֫�����
    H_Type_I_Fracture,		// ����
    H_Type_I_BulletWounded,		//ǹ����
    H_Type_I_ShockInjury,		//����
    H_Type_I_CollisionInjury,		//ײ����
    H_Type_I_Wounded,		//ը��
    H_Type_I_MultipleTrauma,		//�෢��
    H_Type_I_Sprain_Strain,		//Ť��/����
    H_Type_I_Dislocation_Dislocation,		//��λ/��λ

    //�Զ���
    H_Type_I_WORSE,
    H_Type_I_TLQCTRL,				//����ǹ
    H_Type_I_BACKSTAGECTRL,	//��̨����
} ENUM_H_Type_INDIVIDUAL ;

//��������
typedef enum
{
    KIND_WORSE,		//�������
    KIND_DIRECT,	//ֱ���������(ǹ����)
    KIND_INDIRECT,	//�����������(��ը��)
    KIND_EXPLODE,	//��ը��
    KIND_NUCLEAR,	//������
    KIND_BACKSTAGE_CTRL,		//��̨����
    KIND_TLQ_CTRL,		//����Ա����
    KIND_BURN,		//����
    KIND_ENCLOSED,//��տռ�
} ENUM_DAMAGE_KIND;

/***********************************************************
 		H_Type 	����     ̹�ˣ�0x01�� ͻ������0x04��
************************************************************/
typedef enum
{
    H_Type_T_Destroy = 1,		//����
    H_Type_T_SevereDamage,		//�ض�����
    H_Type_T_ArmorDamageLevel1,		//װ������Level1
    H_Type_T_ArmorDamageLevel2,		//װ������Level2
    H_Type_T_ArmorDamageLevel3,		//װ������Level3
    H_Type_T_Nuclear,		//������
    H_Type_T_Chemical,		//��ѧմȾ����
    H_Type_T_Biological,		//����մȾ����
    H_Type_T_Res9,		//
    H_Type_T_Res10,		//
    H_Type_T_Res11,		//
    H_Type_T_Res12,		//
    H_Type_T_Res13,		//
    H_Type_T_Res14,		//
    H_Type_T_Res15,		//
    H_Type_T_Res16,		//
    H_Type_T_DrivingGearDamage,		// ��ʻװ����
    H_Type_T_Res18,		//
    H_Type_T_Res19,		//
    H_Type_T_TrackOrInducerDamage,		// �Ĵ�/�յ�����
    H_Type_T_Res21,		//
    H_Type_T_Res22,		//
    H_Type_T_Res23,		//
    H_Type_T_GearBoxDamage,		// ���ִ�������
    H_Type_T_Res25,		//
    H_Type_T_LoadWheelDamage,		// ��������
    H_Type_T_EngineFailure,		// ��������
    H_Type_T_RadiatorEngineDamage,		// ɢ����/��������
    H_Type_T_FanOrGearboxDamage,		// ����/��������
    H_Type_T_Res30,		//
    H_Type_T_CaterpillarDrivingWheelWamaged,		// �Ĵ�/��������
    H_Type_T_ArtilleryFailure,		// ����ʧЧ 0x20
    H_Type_T_FireControlSystemFailure,		// ���ϵͳʧЧ
    H_Type_T_Res34,		//
    H_Type_T_AntiAircraftMachineGunFailure,		// �����ǹʧЧ
    H_Type_T_Res36,		//

} ENUM_H_Type_TANK_RaiderBuggy ;


/***********************************************************
 		H_Type 	����     ����װ��������(0x02-0x03; 0x05-0x12)
************************************************************/
typedef enum
{
    H_Type_A_Destroy = 1,											//����
    H_Type_A_PowerAndFireSystemsWereDamage,							//����ϵͳ�ͻ���ϵͳ����
    H_Type_A_PowerSystemsWereDamage,								//����ϵͳ��
    H_Type_A_FireSystemsWereDamage,									//����ϵͳ��
    H_Type_A_Res5,													//
    H_Type_A_Nuclear,												// ������
    H_Type_A_Chemical,												// ��ѧմȾ����
    H_Type_A_Biological,											// ����մȾ����

} ENUM_H_Type_Armor;


__packed typedef struct
{
    uint8_t m_bLocked: 1;		//0-������1-����
    uint8_t m_bConnectionState: 1;		//����������״̬	0������	1���쳣
    uint8_t m_bFailureState: 1;		//0���޹���	1���й���
    uint8_t m_bUnitmode: 1;		//0��ѵ��ģʽ	1����ϰģʽ
    uint8_t m_bBatteryState: 1;		//0����������	1��Ƿ��
    uint8_t m_bIdentity: 2;		//��� ENUM_BRCHAR
    uint8_t m_bIsNight: 1;		//������־
} STRUCT_TS_Character1;		//ʵ��״̬��1

__packed typedef struct
{
		uint8_t m_bKey: 5;						//��Կ
    uint8_t m_bIdentity: 2;				//��� ENUM_BRCHAR
		uint8_t m_bUnitmode: 1;				//0��ѵ��ģʽ	1����ϰģʽ

} STRUCT_TS_Character2;	//ʵ��״̬��2

__packed typedef struct
{
    u8 m_byHour;
    u8 m_byMinute;
    u8 m_bySecond;
} Struct_Time2;			//ʱ��2

__packed typedef struct
{
    u8 m_byYear;
    u8 m_byMonth;
    u8 m_byDate;
    u8 m_byHour;
    u8 m_byMinute;
    u8 m_bySecond;
} STRUCT_TIME1;			//ʱ��1

__packed typedef struct
{
    u8 m_byHour;
    u8 m_byMinute;
    u8 m_bySecond;
} STRUCT_TIME2;			//ʱ��2

__packed typedef struct
{
    u8 m_byGS_X[3];
    u8 m_byGS_Y[4];
    u8 m_byGS_Lenth[2];
} STRUCT_GPOS;

__packed typedef struct
{
//	u8 m_byLposState:1;				//1 �Ѷ�λ	��0δ��λ
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
    u8 m_byLposState: 1;				//1 �Ѷ�λ	��0δ��λ

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

//����
typedef enum
{
    PARTITION_NONE = 0,
    PARTITION_HEAD 	,	//	ͷ
    PARTITION_LCHEST	,	//����
    PARTITION_RCHEST	,	//����
    PARTITION_ABDOMEN	,	//����
    PARTITION_BACK	,	//��
    PARTITION_LARM	,	//	���
    PARTITION_RARM	,	//	�ұ�
    PARTITION_LLEG	,	//	����
    PARTITION_RLEG	,	//	����
    PARTITION_LBACK	,	//	��
    PARTITION_RBACK	,	//	�ұ�
    PARTITION_LABDOMEN	,	//	��
    PARTITION_RABDOMEN	,	//	�Ҹ�
    PARTITION_LWAIST	,	//	����
    PARTITION_RWAIST	,	//	����
    PARTITION_MAX,

} ENUM_PARTITION;

typedef struct
{
    uint8_t m_RT_Type;           //������ʵ������
    uint8_t m_RT_Subtype;        //������ʵ��������
    uint8_t m_RRID;              //��������λ����
    uint16_t m_wRPID;            //�������豸���
    uint8_t m_byUniID;
    uint8_t m_A_Type;            //��������ҩ����
    uint8_t m_A_Subtype;         //��������ҩ������
    uint8_t m_HurtBlood;					//����ҩ������˺�Ѫ��
    ENUM_PARTITION m_HPart;           //�����ʵ�����в�λ
    ENUM_H_Type_INDIVIDUAL m_HType;   //�����ʵ������
    //ENUM_HI_INDIVIDUAL_T   m_H_IDegree;         //�����ʵ����������
    STRUCT_LPOS m_sFPosition;    //����������λ��

} RCVHURTINFO_T;

typedef struct
{
    u8 T_Type;      //ʵ������
    u8 T_Subtype;   //ʵ��������
    u32 BulletNum;   //��ʼ����
} WEAPONUNID_T;

__packed typedef struct
{
    uint8_t m_byBullet_Type;            //����n�ŵ�ҩ����
    uint8_t m_byBullet_SubType;         //����n�ŵ�ҩ������
    uint8_t m_aBulletCfgNum[3];         //n�ŵ�ҩ��ʼ��������
    uint8_t m_aBulletRemainNum[3];      //ʣ�൯��
    uint8_t m_aBulletShotNum[3];        //��������
    uint8_t m_aBulletTotalNum[3];       //ʣ�൯ҩ������
		uint8_t m_byAmmuHitRate;						//��ҩ���и���	
		uint8_t m_byAmmuAngle;							//��ҩ�ֳ���
		uint8_t m_byAmmuSpeed;							//ҩ��������ٶ�	
    uint8_t m_byAmmuWearDepth;          //��ҩ�������
    uint8_t m_byAmmuConcreteDepth;      //��ҩ��͸���������
    uint8_t m_byAmmuRange;              //��ҩ��Ч��̣���������Ϊֱ��ʱ��Ч��̣�
		
		//uint8_t m_sChangeBulletNum[3];		//�ӵ��仯������
} BULLETINFO_T;

__packed typedef struct
{

    uint8_t byYear;		//��
    uint8_t byMonth;		//��
    uint8_t byDay;		//��
    uint8_t byHour;		//ʱ
    uint8_t byMin;		//��
    uint8_t bySecond;	//��
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
    uint8_t year;	//���(+2000)
    uint8_t month;	//�·�
    uint8_t date;	//����
    uint8_t hour; 	//Сʱ
    uint8_t min; 	//����
    uint8_t sec; 	//����
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
    //̹�ˣ��ǻ��ģ�⣩
    TType_96Tank = 1, //96̹��
    TType_88BTank	, //88B̹��
    TType_62Tank	, //62̹��
    TType_59Tank	, //59̹��
    TType_M60A3Tank	, //M60A3̹��
    TType_63ATank	, //63A̹��
    TType_59GTank	, //59��̹��
    TType_M1A1Tank	, //M1A1̹��
    TType_99Tank	, //99̹��
    TType_96ATank,    //96Aʽ̹��

    //̹�ˣ�Ƕ��ʽģ�⣩
    TType_E96Tank,    //96AǶ��ʽ̹��

    //��ս��
    TType_86Hariot 	, //86��ʽ��ս��
    TType_92Hariot 	, //92��ʽ��ս��
    TType_04Hariot	, //04��ս��
    TType_05Hariot,   //05ʽ��ս��
    TType_05AmphibiousHariot, //05ʽ����װ�ײ���ս��
    TType_08Hariot,   //ʽ��ʽ��ս��

    //װ�����ͳ�
    TType_63CArmoredCarrier,    //63Cװ�����ͳ�
    TType_001AmphibiousCarrier, //001 ������װ�����ͳ�
    TType_89Carrier,            //89 ʽװ�����ͳ�
    TType_10Carrier,            //10 ʽװ�����ͳ�

    //ͻ����
    TType_05RaiderBuggy,  //05ͻ����
    TType_11RaiderBuggy,  //11 ʽͻ����
    TType_03RaiderBuggy,  //03 ʽͻ����
    TType_05AmphibiousRaiderBuggy,//05 ʽ����װ��ͻ����

    //ͻ����
    TType_100mmAssaultGun,           //100mm��ʽͻ����
    TType_120mmAntiTankSmoothBoreGun,//120mm ��̹�˻�����

    //��̹�˵���ϵͳ
    TType_RedArrow9,  //��� 9 ��̹�˵���ϵͳ
    TType_RedArrow10, //��� 10 ��̹�˵���ϵͳ

    //ǣ������
    TType_83_96_122LiuDanPao, //PL83/96 ʽ 122 ����
    TType_59_1_130JiaNongPao, //PL59-1 ʽ 130 ��ũ��
    TType_66_152JiaLiuPao,    //PL66 ʽ 152 ������

    //���л��ڣ������ֻ���
    TType_89_122LiuDanPao, //PLZ89 ʽ 122 ����
    TType_83_152JiaLiuPao, //PLZ83 ʽ 152 ������

    //���л��ڣ����ֻ���
    TType_05_155JiaLiuPao,           //PLZ05 ʽ 155 ������
    TType_07_122_LiuDanPao,          //PLZ07 ʽ 122 ����
    TType_09_122_LiuDanPao,          //PLC09 ʽ 122 ����
    TType_05_122_ZiXingPaiLiuPao,    //PLL05 ʽ 120mm ����������
    TType_AmphibiousZiXingLiuDanPao, //������������

    //�����
    TType_81_122HuoJianPao,     //PHL81 ʽ 122 �����
    TType_89_122HuoJianPao,     //PHZ89 ʽ 122 �����
    TType_05_300YCHuoJianPao,   //PHZ05 ʽ 300mm Զ�̻����

    //�Ȼ���
    TType_89_82PaiJiPao, //89 ʽ 82mm �Ȼ���
    TType_89_100PaiJiPao,//89 ʽ 100mm �Ȼ���
    TType_120PaiJiPao,   //120mm �Ȼ���
    TType_89_60PaiJiPao, //89 ʽ 60mm �Ȼ���
    TType_82SSPaiJiPao,  //82mm �����Ȼ���

    //�ڱ����
    TType_ArtilleryPositionRadar,//��λ����״�
    TType_ArtilleryVehicle,      //�ڱ���쳵
    TType_ArtilleryMovingTargetReconnaissanceRadar,//�ڱ��Ŀ������״�
    TType_AnalogBurstPointIndicator,//�ڱ������ģ��ը��ָʾ��

    //���ڣ��л�أ�
    TType_091A35GPZC,    //PZK091A35 ���׸���ս��
    TType_99_35_GP_DataAcquisitionUnit,    //PG99 ʽ 35 ���׸������ݲɼ��豸
    TType_04_DPJHWQXTZC,    //PGZ04 �͵��ڽ������ϵͳս��
    TType_625_DPJHWQZC,    //PLB625 ���ڽ������ս��

    //���ڣ��޻�أ�
    TType_AntiaircraftGun_Double37,    //˫ 37 ����
    TType_AntiaircraftGun_Double25,    //˫25���� 20
    TType_AntiaircraftGun_4_25,        //�ģ�25����
    TType_AntiaircraftGun_57mm,        //57mm����

    //���ص���ϵͳ
    TType_HQ17_SC,     //���� 17 ��������ϵͳ S ��
    TType_HQ17_FC,     //���� 17 ��������ϵͳ F ��
    TType_HQ16_DataAcquisitionUnit,        //���� 16A/D ��������ϵͳ���ݲɼ��豸
    TType_HQ7B_SC_DataAcquisitionUnit,     //���� 7B �������� S ��ϵͳ���ݲɼ��豸
    TType_HQ7B_FC_DataAcquisitionUnit,     //���� 7B �������� F ��ϵͳ���ݲɼ��豸

    //�����״�ϵͳ
    TType_LLP12A_RadarDataAcquisitionUnit,    //LLP12A ����״����ݲɼ��豸
    TType_STD_RadarDataAcquisitionUnit,   //�鱨�״����ݲɼ��豸����׼�ͣ�

    //����ָ��ϵͳ
    TType_CICS_LZ_DataAcquisitionUnit,    //ָ����Ϣϵͳ�ã�Ӫ��վ���ݲɼ��豸
    TType_CICS_GPLZ_DataAcquisitionUnit,    //ָ����Ϣϵͳ������վ���ݲɼ��豸

    //�������յ���
    TType_HY5,      //��ӧ 5
    TType_HY6_FKDD, //��ӧ 6 ��Яʽ���յ���
    TType_QW2_FKDD, //ǰ�� 2 ��Яʽ���յ���
    TType_HY6_DD,   //��ӧ 6 ��Яʽ����
    TType_QW2_FFDD2,//ǰ�� 2 ��Яʽ���յ��� TODO:�������ظ���

    //��Ա
    TType_PersonalEquipment,       //����װ��
    TType_PersonalEquipment_TZ,    //����װ�ߣ���ս�ͣ�
    TType_PersonalEquipment_LQ,    //����װ�ߣ������ͣ�

    //�Զ���ǹ
    TType_95_58ZDBQ,       //95 ʽ 5.8mm �Զ���ǹ
    TType_AK47ZDBQ,        //AK47 �Զ���ǹ
    TType_95_1_58ZDBQ,     //95-1 ʽ 5.8mm �Զ���ǹ
    TType_03_58ZDBQ,      //03 ʽ 5.8mm �Զ���ǹ
    TType_58JZBQ,          //5.8mm ��׼��ǹ
    TType_95B_58DBQ,       //95B ʽ 5.8mm �̲�ǹ
    TType_95_1_58DBQ,      //95-1 ʽ 5.8mm �̲�ǹ
    TType_95_58ZDBQ_TZ,    //95 ʽ 5.8mm �Զ���ǹ����ս�ͣ�
    TType_95_1_58ZDBQ_TZ,  //95-1 ʽ 5.8mm �Զ���ǹ����ս�ͣ�
    TType_95B_58DBQ_TZ,    //95B ʽ 5.8mm �̲�ǹ����ս�ͣ�
    TType_95_1_58DBQ_TZ,   //95-1 ʽ 5.8mm �̲�ǹ����ս�ͣ�
    TType_95_58ZDBQ_LQ,    //95 ʽ 5.8mm �Զ���ǹ�������ͣ�
    TType_95_1_58ZDBQ_LQ,  //95-1 ʽ 5.8mm �Զ���ǹ�������ͣ�
    TType_95B_58DBQ_LQ,    //95B ʽ 5.8mm �̲�ǹ�������ͣ�
    TType_95_1_58DBQ_LQ,   //95-1 ʽ 5.8mm �̲�ǹ�������ͣ�

    //���û�ǹ
    TType_95_58BYJQ,       //95 ʽ 5.8mm ���û�ǹ
    TType_95_1_58BYJQ,     //95-1 ʽ 5.8mm ���û�ǹ
    TType_95_1_58BYJQ_TZ,  //95-1 ʽ 5.8mm ���û�ǹ����ս�ͣ�
    TType_95_58BYJQ_LQ,    //95 ʽ 5.8mm ���û�ǹ�������ͣ�
    TType_95_1_58BYJQ_LQ,  //95-1 ʽ 5.8mm ���û�ǹ�������ͣ�

    //�ѻ���ǹ
    TType_88_58JJQ,    //88 ʽ 5.8mm �ѻ�ǹ
    TType_09JJBQ,      //09 �ѻ���ǹ
    TType_762GJDJJQ,   //7.62mm �߾��Ⱦѻ�ǹ
    TType_10_127JJQ,   //10 ʽ 12.7mm �ѻ�ǹ
    TType_58JJQ_TZ,    //5.8mm �ѻ�ǹ����ս�ͣ�
    TType_86JJQ_TZ,    //8.6mm �ѻ�ǹ����ս�ͣ�
    TType_127JJQ_TZ,   //12.7mm �ѻ�ǹ����ս�ͣ�
    TType_58JJQ_LQ,    //5.8mm �ѻ�ǹ�������ͣ�
    TType_762JJQ_LQ,   //7.62mm �ѻ�ǹ�������ͣ�
    TType_10_127JJQ_LQ,//10 ʽ 12.7mm �ѻ�ǹ�������ͣ�

    //΢�����ǹ
    TType_05_58WSCFQ,       //05 ʽ 5.8mm ΢�����ǹ
    TType_05_58WSCFQ_TZ,    //05 ʽ 5.8mm ΢�����ǹ����ս�ͣ�
    TType_05_58WSCFQ_LQ,    //05 ʽ 5.8mm ΢�����ǹ�������ͣ�

    //����ǹ
    TType_09_184XDQ,    //09 ʽ 18.4mm ����ǹ
    TType_09_184XDQ_TZ,    //09 ʽ 18.4mm ����ǹ����ս�ͣ�
    TType_09_184XDQ_LQ,    //09 ʽ 18.4mm ����ǹ�������ͣ�

    //��ǹ
    TType_SQ,    //��ǹ
    TType_SQ_TZ, //��ǹ���������ս�ͣ�

    //�����񵯷�����
    TType_87_35ZDLD,       //87 ʽ 35mm �Զ��񵯷�����
    TType_04_35ZDLD,       //04 ʽ 35mm �Զ��񵯷�����
    TType_131_35JJLD,      //131 �� 35mm �ѻ��񵯷�����
    TType_10_35QGLD,       //10 ʽ 35mm ǹ���񵯷�����
    TType_87_35ZDLD_TZ,    //87 ʽ 35mm �Զ��񵯷���������ս�ͣ�
    TType_04_35ZDLD_TZ,    //04 ʽ 35mm �Զ��񵯷���������ս�ͣ�
    TType_131_35JJLD_TZ,   //131 �� 35mm �ѻ��񵯷���������ս�ͣ�
    TType_87_35ZDLD_LQ,    //87 ʽ 35mm �Զ��񵯷������������ͣ�
    TType_04_35ZDLD_LQ,    //04 ʽ 35mm �Զ��񵯷������������ͣ�
    TType_131_35JJLD_LQ,   //131 �� 35mm �ѻ��񵯷������������ͣ�
    TType_10_35QGLD_LQ,    //10 ʽ 35mm ǹ���񵯷������������ͣ�

    //�ػ�ǹ
    TType_89_127ZJQ,       //89 ʽ 12.7mm �ػ�ǹ
    TType_QBZ171_127ZJQ,   //QBZ171 ɽ���� 12.7mm �ػ�ǹ
    TType_89_127ZJQ_TZ,    //89 ʽ 12.7mm �ػ�ǹ����ս�ͣ�
    TType_89_127ZJQ_LQ,    //89 ʽ 12.7mm �ػ�ǹ�������ͣ�

    //�����ǹ
    TType_85_127GSJQ,      //85 ʽ 12.7mm �����ǹ
    TType_85_127GSJQ_LQ,   //85 ʽ 12.7mm �����ǹ�������ͣ�

    //��ѧ����豸
    TType_BAZ_308,     //Baz-308 ս��������
    TType_BAD_902C,    //Bad-902C Զ����������

    //�������
    TType_80DBHJT,       //80 ��
    TType_11_93DBHJT,    //11 ʽ 93mm �������Ͳ
    TType_PF89_80DBHJT,  //PF89 ʽ 80mm �������Ͳ
    TType_11_93DBHJT2,   //11 ʽ 93mm �������Ͳ
    TType_PF89_80DBHJT2, //PF98 ʽ 93mm �������Ͳ  TODO:�������ظ�

    //������̹�˻��
    TType_PF98_YY120_FTKHJT,       //PF98 ʽӪ�� 120mm ��̹�˻��Ͳ
    TType_PF98_LY120_FTKHJT,       //PF98 ʽ���� 120mm ��̹�˻��Ͳ
    TType_PF98_YY120_FTKHJT_LQ,    //PF98 ʽӪ�� 120mm ��̹�˻��Ͳ�������ͣ�
    TType_PF98_LY120_FTKHJT_LQ,    //PF98 ʽ���� 120mm ��̹�˻��Ͳ�������ͣ�

    //������̹�˵���
    TType_HJ73G_FTKDD,   //��� 73 �ķ�̹�˵���
    TType_HJ8_FTKDD,     //��� 8 ��̹�˵���
    TType_HJ12_FTKDD,    //��� 12 ��̹�˵���
    TType_HJ8_FTKDD2,    //��� 8 ��̹�˵��� TODO:�ظ�

    //����
    TType_YFSFBBmine,    //ѹ��ʽ����������
    TType_FTKmine,       //��̹�˵���
    TType_BFSFBBmine,    //��ʽ����������
    TType_DXmine,        //������

    //����������
    TType_DBPLQ,    //����������
    TType_GBMNQ,    //����ģ����

    //����
    TType_82_2SLD,    //82-2 ʽ����
    TType_77_1SLD,    //77-1 ʽ����

    //���ױ�ը��
    TType_BZW,        //��ը��

    //���ھȻ�װ��
    TType_WQJH,       //���ھȻ�

    //�����豸
    TType_TLQ,      //����ǹ
    TType_MNZDQ,    //ģ��װ����
    TType_TLZD,     //�����ն�

    //ͨ�ó���
    TType_TYCL,      //ͨ�ó���
    TType_LQTYFHLCL, //����ͨ�÷ǻ�������

    //ָ�ӳ���
    TType_ZHC,              //ָ�ӳ�
    TType_PBZHC,            //�ڱ�ָ�ӳ�
    TType_PZK091A35_ZHC,    //PZK091A35 ���׸���ָ�ӳ�
    TType_PGZ04_ZHC,        //PGZ04 �͵��ڽ������ϵͳָ�ӳ�
    TType_PLB625_ZHC,       //PLB625 ���ڽ������ָ�ӳ�

    //ͨ�ų���
    TType_TXC,       //ͨ�ų�
    TType_PBTXJDC,   //�ڱ�ͨ�Žڵ㳵

    //��ҩ���ͳ�
    TType_DYSSC,    //��ҩ���ͳ�

    //�������䳵
    TType_YLYSC,    //�������䳵

    //������׳�
    TType_BLC,      //���׳�

    //������쳵
    TType_GCZZC,    //������쳵
    TType_ZZC,      //��쳵

    //�������ϳ�
    TType_PZC,      //���ϳ�
    TType_CSL132,   //CSL132 ������װ�����ϳ�

    //�ۺ�ɨ�׳�
    TType_ZHSLC,    //�ۺ�ɨ�׳�

    //���ͻ�е����
    TType_84A,      //84A ʽ���ͻ�е����

    //���޳���
    TType_ZJQXCL,   //װ�����޳�

    //������ҵ����
    TType_CCZ111,   //CCZ111 �����ܹ�����ҵ��

    //ֱ����
    TType_Z10WZZSJ,  //ֱ 10 ��װֱ����
    TType_Z19WZZSJ,  //ֱ 19 ��װֱ����
    TType_Z20ZSJ,    //ֱ 20 ϵ��ֱ����
    TType_M17ZSJ,    //�� 17 ϵ��ֱ����
    TType_Z8ZSJ,     //ֱ 8 ϵ��ֱ����
    TType_Z9WZZSJ,   //ֱ 9 ��װֱ����
    TType_WZZSJ,     //��װֱ����
    TType_YSZSJ,     //����ֱ����

    //���˻�
    TType_CH3,    //�ʺ�-3
    TType_CH4,    //�ʺ�-4
    TType_YL1,    //����-1
    TType_MQ9,    //MQ-9
    TType_RQ1,    //RQ-1
    TType_RQ4,    //RQ-4

    //ս����/��ը��
    TType_H6,     //�� 6
    TType_JH7,    //�ߺ� 7
    TType_J10,    //�� 10
    TType_J11,    //�� 11
    TType_J15,    //�� 15
    TType_J16,    //�� 16
    TType_J20,    //�� 20
    TType_S27,    //�� 27
    TType_S30,    //�� 30
    TType_S35,    //�� 35
    TType_F15,    //F15
    TType_F16,    //F16
    TType_F18,    //F18
    TType_F22,    //F22
    TType_F35,    //F35
    TType_B52,    //B52
    TType_B1B,    //B1B
    TType_B2,     //B2

    //���ӶԿ�
    TType_DCHJ_DataGetEquip,    //ս����Ż������ݲɼ��豸

    //������ʩ
    TType_FirePoint, //������
    TType_DB,        //�ر�
    TType_JYGS,      //���׹���
    TType_JGGS,      //��̹���
    TType_JZW,       //������
    TType_ZhiHuiSuo, //ָ����
    TType_DanYaoKu,  //��ҩ��
    TType_YouLiaoKu, //���Ͽ�
    TType_WuZiKu,    //���ʿ�
    TType_LDZ,       //�״�վ
    TType_QiangLiang,//����
    TType_HCZ,       //��վ
    TType_PD,        //�ܵ�
    TType_JK,        //����
    TType_JMLD,      //�������
    TType_YKYWFSQ,   //ң����������

    //ս���ϰ�
    TType_TieSiWang,      //��˿��
    TType_SanJiaoZhui,    //����׶
    TType_HG,             //����
    TType_FTKQH,          //��̹��ǵ��
    TType_XNFBBLC,        //����������׳�
    TType_XNFTKLC,        //���̹ⷴ���׳�
    TType_FNHHLC,         //�������׳�
    TType_STFBBLC,        //ʵ��������׳�
    TType_STFBBLC2,       //ʵ��������׳�
    TType_STFBBLC3,       //ʵ��������׳� TODO:�ظ�
    TType_XNHHSZRQ,       //����˻���մȾ��

    //����̨
    TType_ZKT,      //����̨

} EnumTerminalType;

typedef enum
{
    WTYPE_WIRE = 0,//����
    WTYPE_BLE = 1,//����
    WTYPE_XYZ66X = 2,//��һ�±�׼zigbee
    WTYPE_LORA = 3,//lora�豸
} E_WIRELESS_TYPE;

#ifdef __cplusplus
}
#endif

#endif




