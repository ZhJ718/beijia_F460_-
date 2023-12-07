#include "w25qxx.h"

//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK��ӢSTM32������
//W25Q64 ����
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/9
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////




u16 W25QXX_TYPE = W25Q32;	//Ĭ����W25Q128









//��ȡW25QXX��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
u8 W25QXX_ReadSR(W25QXX_HW_OPERATIONS a_sHwOps)
{
    u8 byte = 0;

    a_sHwOps.reset_cs();	 //ʹ������
    a_sHwOps.rwbyte(W25X_ReadStatusReg); //���Ͷ�ȡ״̬�Ĵ�������
    byte = a_sHwOps.rwbyte(0Xff);        //��ȡһ���ֽ�
    a_sHwOps.set_cs();	   //ȡ��Ƭѡ
    return byte;
}
//дW25QXX״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
void W25QXX_Write_SR(W25QXX_HW_OPERATIONS a_sHwOps, u8 sr)
{
    a_sHwOps.reset_cs();	 //ʹ������
    a_sHwOps.rwbyte(W25X_WriteStatusReg);//����дȡ״̬�Ĵ�������
    a_sHwOps.rwbyte(sr);               	//д��һ���ֽ�
    a_sHwOps.set_cs();	   //ȡ��Ƭѡ
}
//W25QXXдʹ��
//��WEL��λ
void W25QXX_Write_Enable(W25QXX_HW_OPERATIONS a_sHwOps)
{
    a_sHwOps.reset_cs();	 //ʹ������
    a_sHwOps.rwbyte(W25X_WriteEnable); 	//����дʹ��
    a_sHwOps.set_cs();	   //ȡ��Ƭѡ
}
//W25QXXд��ֹ
//��WEL����
void W25QXX_Write_Disable(W25QXX_HW_OPERATIONS a_sHwOps)
{
    a_sHwOps.reset_cs();	 //ʹ������
    a_sHwOps.rwbyte(W25X_WriteDisable);  //����д��ָֹ��
    a_sHwOps.set_cs();	   //ȡ��Ƭѡ
}
//��ȡоƬID
//����ֵ����:
//0XEF13,��ʾоƬ�ͺ�ΪW25Q80
//0XEF14,��ʾоƬ�ͺ�ΪW25Q16
//0X8515,��ʾоƬ�ͺ�ΪW25Q32
//0XEF16,��ʾоƬ�ͺ�ΪW25Q64
//0XEF17,��ʾоƬ�ͺ�ΪW25Q128
u16 W25QXX_ReadID(W25QXX_HW_OPERATIONS a_sHwOps)
{
    u16 Temp = 0;

    a_sHwOps.reset_cs();

    a_sHwOps.rwbyte(0x90);//���Ͷ�ȡID����
    a_sHwOps.rwbyte(0x00);
    a_sHwOps.rwbyte(0x00);
    a_sHwOps.rwbyte(0x00);
    Temp |= a_sHwOps.rwbyte(0xFF) << 8;
    Temp |= a_sHwOps.rwbyte(0xFF);

    a_sHwOps.set_cs();
    return Temp;
}
//��ȡSPI FLASH
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void W25QXX_Read(W25QXX_HW_OPERATIONS a_sHwOps, u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
    u16 i;

    a_sHwOps.reset_cs();	 //ʹ������
    a_sHwOps.rwbyte(W25X_ReadData);         	//���Ͷ�ȡ����
    a_sHwOps.rwbyte((u8)((ReadAddr) >> 16));  	//����24bit��ַ
    a_sHwOps.rwbyte((u8)((ReadAddr) >> 8));
    a_sHwOps.rwbyte((u8)ReadAddr);

    for(i = 0; i < NumByteToRead; i++)
    {
        pBuffer[i] = a_sHwOps.rwbyte(0XFF);   	//ѭ������
    }

    a_sHwOps.set_cs();	   //ȡ��Ƭѡ
}
//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
void W25QXX_Write_Page(W25QXX_HW_OPERATIONS a_sHwOps, u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    u16 i;

    W25QXX_Write_Enable(a_sHwOps);                  	//SET WEL
    a_sHwOps.reset_cs();	 //ʹ������
    a_sHwOps.rwbyte(W25X_PageProgram);      	//����дҳ����
    a_sHwOps.rwbyte((u8)((WriteAddr) >> 16)); 	//����24bit��ַ
    a_sHwOps.rwbyte((u8)((WriteAddr) >> 8));
    a_sHwOps.rwbyte((u8)WriteAddr);

    for(i = 0; i < NumByteToWrite; i++)
        a_sHwOps.rwbyte(pBuffer[i]);//ѭ��д��

    a_sHwOps.set_cs();	   //ȡ��Ƭѡ
    W25QXX_Wait_Busy(a_sHwOps);					   		//�ȴ�д�����
}
//�޼���дSPI FLASH
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ����
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void W25QXX_Write_NoCheck(W25QXX_HW_OPERATIONS a_sHwOps, u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    u16 pageremain;
    pageremain = 256 - WriteAddr % 256; //��ҳʣ����ֽ���

    if(NumByteToWrite <= pageremain)pageremain = NumByteToWrite; //������256���ֽ�

    while(1)
    {
        W25QXX_Write_Page(a_sHwOps, pBuffer, WriteAddr, pageremain);

        if(NumByteToWrite == pageremain)break; //д�������
        else //NumByteToWrite>pageremain
        {
            pBuffer += pageremain;
            WriteAddr += pageremain;

            NumByteToWrite -= pageremain;			 //��ȥ�Ѿ�д���˵��ֽ���

            if(NumByteToWrite > 256)pageremain = 256; //һ�ο���д��256���ֽ�
            else pageremain = NumByteToWrite; 	 //����256���ֽ���
        }
    };
}
//дSPI FLASH
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)

/*void W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
	u32 secpos;
	u16 secoff;
	u16 secremain;
 	u16 i;
	u8 * W25QXX_BUF;
   	W25QXX_BUF=W25QXX_BUFFER;
 	secpos=WriteAddr/4096;//������ַ
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
	while(1)
	{
		W25QXX_Read(W25QXX_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//��Ҫ����
		}
		if(i<secremain)//��Ҫ����
		{
			W25QXX_Erase_Sector(secpos);		//�����������
			for(i=0;i<secremain;i++)	   		//����
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];
			}
			W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//д����������

		}else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������.
		if(NumByteToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0

		   	pBuffer+=secremain;  				//ָ��ƫ��
			WriteAddr+=secremain;				//д��ַƫ��
		   	NumByteToWrite-=secremain;			//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;//��һ����������д����
			else secremain=NumByteToWrite;		//��һ����������д����
		}
	};
}*/

//��������оƬ
//�ȴ�ʱ�䳬��...
void W25QXX_Erase_Chip(W25QXX_HW_OPERATIONS a_sHwOps)
{
    W25QXX_Write_Enable(a_sHwOps);                 	 	//SET WEL
    W25QXX_Wait_Busy(a_sHwOps);
    a_sHwOps.reset_cs();	 //ʹ������
    a_sHwOps.rwbyte(W25X_ChipErase);        	//����Ƭ��������
    a_sHwOps.set_cs();	   //ȡ��Ƭѡ
    W25QXX_Wait_Busy(a_sHwOps);   				   		//�ȴ�оƬ��������
}

//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ��ɽ��������ʱ��:150ms
void W25QXX_Erase_Sector(W25QXX_HW_OPERATIONS a_sHwOps, u32 Dst_Addr)
{
    Dst_Addr *= 4096;
    W25QXX_Write_Enable(a_sHwOps);                  	//SET WEL
    W25QXX_Wait_Busy(a_sHwOps);
    a_sHwOps.reset_cs();	 //ʹ������   a_sHwOps.reset_cs();	 //ʹ������
    a_sHwOps.rwbyte(W25X_SectorErase);      	//������������ָ��
    a_sHwOps.rwbyte((u8)((Dst_Addr) >> 16));  	//����24bit��ַ
    a_sHwOps.rwbyte((u8)((Dst_Addr) >> 8));
    a_sHwOps.rwbyte((u8)Dst_Addr);
    a_sHwOps.set_cs();	   //ȡ��Ƭѡ
    W25QXX_Wait_Busy(a_sHwOps);   				   		//�ȴ��������
}

//����һ��ҳ
//Dst_Addr:ҳ��ַ ����ʵ����������
void W25QXX_Erase_Page(W25QXX_HW_OPERATIONS a_sHwOps, u32 Dst_Addr)
{

    W25QXX_Write_Enable(a_sHwOps);                  	//SET WEL
    W25QXX_Wait_Busy(a_sHwOps);
    a_sHwOps.reset_cs();	 //ʹ������   a_sHwOps.reset_cs();	 //ʹ������
    a_sHwOps.rwbyte(W25X_PageErase);      	//������������ָ��
    a_sHwOps.rwbyte((u8)((Dst_Addr) >> 16));  	//����24bit��ַ
    a_sHwOps.rwbyte((u8)((Dst_Addr) >> 8));
    a_sHwOps.rwbyte((u8)Dst_Addr);
    a_sHwOps.set_cs();	   //ȡ��Ƭѡ
    W25QXX_Wait_Busy(a_sHwOps);   				   		//�ȴ��������
}

//�ȴ�����
void W25QXX_Wait_Busy(W25QXX_HW_OPERATIONS a_sHwOps)
{
    while((W25QXX_ReadSR(a_sHwOps) & 0x01) == 0x01);  		// �ȴ�BUSYλ���
}
//�ȴ�����
u32 W25QXX_Is_Busy(W25QXX_HW_OPERATIONS a_sHwOps)
{
    return W25QXX_ReadSR(a_sHwOps) & 0x01;  		// �ȴ�BUSYλ���
}

//�������ģʽ
void W25QXX_PowerDown(W25QXX_HW_OPERATIONS a_sHwOps)
{
    a_sHwOps.reset_cs();	 //ʹ������
    a_sHwOps.rwbyte(W25X_PowerDown);        //���͵�������
    a_sHwOps.set_cs();	   //ȡ��Ƭѡ
    //delay_us(3);                               //�ȴ�TPD
}
//����
void W25QXX_WAKEUP(W25QXX_HW_OPERATIONS a_sHwOps)
{
    a_sHwOps.reset_cs();	 //ʹ������
    a_sHwOps.rwbyte(W25X_ReleasePowerDown);	//  send W25X_PowerDown command 0xAB
    a_sHwOps.set_cs();	   //ȡ��Ƭѡ
    //delay_us(3);                            	//�ȴ�TRES1
}



















