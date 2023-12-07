
/*******************************************************************************
 * Include files
 ******************************************************************************/

#include "main_task.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "iic.h"
#include "iwdt.h"
#include "timer.h"

#define IIC_SDA(x)		((x==1)?PORT_SetBits(I2C3_SDA_PORT, I2C3_SDA_PIN):PORT_ResetBits(I2C3_SDA_PORT, I2C3_SDA_PIN))
#define Read_SDA()		(PORT_GetBit(I2C3_SDA_PORT, I2C3_SDA_PIN))
#define IIC_SCL(x)		((x==1)?PORT_SetBits(I2C3_SCL_PORT, I2C3_SCL_PIN):PORT_ResetBits(I2C3_SCL_PORT, I2C3_SCL_PIN))
#define delay_us(x)		(Ddl_Delay1us(x))

//mpu6050 地址初始化
void MPU6050_GpioInit(void)
{
	stc_port_init_t stcPortInit;    
	
	PORT_SetFunc(MPU_AD0_PORT, MPU_AD0_PIN, Func_Gpio, Disable);	//设置输出模式，禁止周围模式
	
	MEM_ZERO_STRUCT(stcPortInit);
	stcPortInit.enPinMode = Pin_Mode_Out;	//输出
	stcPortInit.enPinOType = Pin_OType_Cmos;	//推挽模式
	stcPortInit.enPinDrv = Pin_Drv_H;	//高驱动能力
	stcPortInit.enExInt = Disable;		//无外部中断
	stcPortInit.enPullUp = Disable;		//不需要内部上拉（做输入时起作用）

	PORT_Init(MPU_AD0_PORT, MPU_AD0_PIN, &stcPortInit);	
}

//mpu6050 地址设置低
void MPU6050_AD0_SetLow(void)
{
	PORT_ResetBits(MPU_AD0_PORT, MPU_AD0_PIN);	
}

/**
 ******************************************************************************
 ** \brief  Initialize the I2C peripheral for master
 **
 ** \param  None
 **
 ** \retval Process result
 **         - I2C_RET_ERROR  Initialize failed
 **         - I2C_RET_OK     Initialize success
 ******************************************************************************/
uint8_t Master_Initialize(void)
{	
	stc_port_init_t stcPortInit;    
	MEM_ZERO_STRUCT(stcPortInit);
	stcPortInit.enPinMode = Pin_Mode_Out;
	stcPortInit.enExInt = Enable;
	stcPortInit.enPullUp = Enable;
	PORT_Init(I2C3_SCL_PORT, I2C3_SCL_PIN, &stcPortInit);
	PORT_Init(I2C3_SDA_PORT, I2C3_SDA_PIN, &stcPortInit);	

	IIC_SDA(1);
	IIC_SCL(1);		
}

void SDA_OUT_GpioInit(void)
{
	stc_port_init_t stcPortInit;    
	MEM_ZERO_STRUCT(stcPortInit);
	stcPortInit.enPinMode = Pin_Mode_Out;
	stcPortInit.enExInt = Enable;
	stcPortInit.enPullUp = Enable;
	PORT_Init(I2C3_SDA_PORT, I2C3_SDA_PIN, &stcPortInit);
}

void SDA_IN_GpioInit(void)
{
	stc_port_init_t stcPortInit;
	MEM_ZERO_STRUCT(stcPortInit);	
	PORT_Init(I2C3_SDA_PORT, I2C3_SDA_PIN, &stcPortInit);
}

void IIC_Start(void)
{
	IIC_SDA(1);
	IIC_SCL(1);
	IIC_SDA(0);
	IIC_SCL(0);
}

void IIC_Stop(void)
{
	IIC_SDA(0);	
	IIC_SCL(1);
	IIC_SDA(1);
}

void IIC_Ack(void)
{
	IIC_SCL(0);
	SDA_OUT_GpioInit();
	IIC_SDA(0);
	IIC_SCL(1);
	IIC_SCL(0);
}

void IIC_NAck(void)
{
	IIC_SCL(0);
	SDA_OUT_GpioInit();
	IIC_SDA(1);
	IIC_SCL(1);
	IIC_SCL(0);	
}


uint8_t IIC_Wait_Ack(void)
{
	uint8_t pTime = 0;
		
	IIC_SDA(1);
	SDA_IN_GpioInit();
	IIC_SCL(1);
	pTime = Read_SDA();
	IIC_SCL(0);
	SDA_OUT_GpioInit();
	return pTime;
}

void IIC_Send_Byte(uint8_t data)
{
	uint8_t i,j;
	
	for(i=0;i<8;i++)
	{
		if((data << i) & 0x80)
		{
			IIC_SDA(1);
		}
		else
		{
			IIC_SDA(0);
		}
		IIC_SCL(1);
		IIC_SCL(0);
	}
}

uint8_t IIC_Rcv_Byte(uint8_t ack)
{
	uint8_t i,rcvdata = 0;
	
	SDA_IN_GpioInit();
	for(i=0;i<8;i++)
	{
		IIC_SCL(0);
		IIC_SCL(1);
		rcvdata<<=1;
		if(Read_SDA())
		{
			rcvdata++;
		}
	}
	
	if(!ack)
	{
		IIC_NAck();
	}
	else
	{
		IIC_Ack();
	}
	return rcvdata;
}

/**
 ******************************************************************************
 ** \brief  Judge the result. LED0 toggle when result is error status.
 **
 ** \param  u8Result
 **
 ** \retval None
 ******************************************************************************/
extern void JudgeResult(uint8_t u8Result)
{
  	IWDG_Feed();
    if(I2C_RET_ERROR == u8Result)
    {
        //while(1)
        {					
            //LED0_TOGGLE();
            //Ddl_Delay1ms(5ul);
	//					g_MpuMag.InitFail = 1;
        //    return;
        }
    }
}


//IIC写一个字节 
//reg:寄存器地址
//data:数据
//返回值:0,正常
//    其他,错误代码
uint8_t MPU_Write_Byte(uint8_t  reg,uint8_t data) 				 
{ 
	IIC_Start();
	IIC_Send_Byte((DEVICE_ADDRESS<<1) | 0);
	if(IIC_Wait_Ack())
	{
		IIC_Stop();
		return 1;
	}
	IIC_Send_Byte(reg);
	if(IIC_Wait_Ack())
	{
		IIC_Stop();
		return 1;
	}
	IIC_Send_Byte(data);
	if(IIC_Wait_Ack())
	{
		IIC_Stop();
		return 1;
	}
	IIC_Stop();
	return 0;	
}


//设置MPU6050陀螺仪传感器满量程范围
//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
//返回值:0,设置成功
//    其他,设置失败 
uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr)
{
	return MPU_Write_Byte(MPU_GYRO_CFG_REG,fsr<<3);//设置陀螺仪满量程范围  
}

//设置MPU6050加速度传感器满量程范围
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
//返回值:0,设置成功
//    其他,设置失败 
uint8_t MPU_Set_Accel_Fsr(uint8_t fsr)
{
	return MPU_Write_Byte(MPU_ACCEL_CFG_REG,fsr<<3);//设置加速度传感器满量程范围  
}

//设置MPU6050的采样率(假定Fs=1KHz)
//rate:4~1000(Hz)
//返回值:0,设置成功
//    其他,设置失败 
uint8_t MPU_Set_Rate(uint16_t rate)
{
	uint8_t data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=MPU_Write_Byte(MPU_SAMPLE_RATE_REG,data);	//设置数字低通滤波器
 	return MPU_Set_LPF(rate/2);	//自动设置LPF为采样率的一半
}

//设置MPU6050的数字低通滤波器
//lpf:数字低通滤波频率(Hz)
//返回值:0,设置成功
//    其他,设置失败 
uint8_t MPU_Set_LPF(uint16_t lpf)
{
	uint8_t data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return MPU_Write_Byte(MPU_CFG_REG,data);//设置数字低通滤波器  
}


//IIC读一个字节 
//reg:寄存器地址 
//返回值:读到的数据
uint8_t MPU_Read_Byte(uint8_t reg)
{
	uint8_t ret;
	
	IIC_Start();
	IIC_Send_Byte((DEVICE_ADDRESS<<1) | 0);
	IIC_Wait_Ack();
	IIC_Send_Byte(reg);
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte((DEVICE_ADDRESS<<1) | 1);
	IIC_Wait_Ack();
	ret = IIC_Rcv_Byte(0);
	IIC_Stop();
	return ret;	

}


//初始化mpu
uint8_t MPU_HwInit(void)
{
    uint8_t res; 
	
void MPU6050_GpioInit(void);
void MPU6050_AD0_SetLow(void);	
    MPU6050_GpioInit();
    MPU6050_AD0_SetLow();	//设置MPU6050地址
	
    /* Initialize I2C peripheral and enable function*/
    Master_Initialize();            //初始化i2c
		IWDG_Feed();
    Delay_Ms(50ul);
		IWDG_Feed();
    Delay_Ms(50ul);
		IWDG_Feed();
	
    MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X80);	//复位MPU6050
		IWDG_Feed();
		Delay_Ms(50ul);
		IWDG_Feed();
    Delay_Ms(50ul);
		IWDG_Feed();

	MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X00);	//唤醒MPU6050 
	MPU_Set_Gyro_Fsr(3);					//陀螺仪传感器,±2000dps
	MPU_Set_Accel_Fsr(0);					//加速度传感器,±2g
	MPU_Set_Rate(50);						//设置采样率50Hz
	MPU_Write_Byte(MPU_INT_EN_REG,0X00);	//关闭所有中断
	MPU_Write_Byte(MPU_USER_CTRL_REG,0X00);	//I2C主模式关闭
	MPU_Write_Byte(MPU_FIFO_EN_REG,0X00);	//关闭FIFO
	MPU_Write_Byte(MPU_INTBP_CFG_REG,0X80);	//INT引脚低电平有效
	res=MPU_Read_Byte(MPU_DEVICE_ID_REG);
	if(res==MPU_ADDR)//器件ID正确
	{
		MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X01);	//设置CLKSEL,PLL X轴为参考
		MPU_Write_Byte(MPU_PWR_MGMT2_REG,0X00);	//加速度与陀螺仪都工作
		MPU_Set_Rate(50);						//设置采样率为50Hz
 	}else return 1;
	return 0;
}

uint8_t MPU_Init(void)
{
	return MPU_HwInit();
}



//得到加速度值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
uint8_t MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
    uint8_t buf[6],res;  
	res=MPU_Read_Len(MPU_ADDR,MPU_ACCEL_XOUTH_REG,6,buf);
	if(res==0)
	{
		*ax=((uint16_t)buf[0]<<8)|buf[1];  
		*ay=((uint16_t)buf[2]<<8)|buf[3];  
		*az=((uint16_t)buf[4]<<8)|buf[5];
	} 	
    return res;
}

//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
uint8_t MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
    uint8_t buf[6],res;  
	res=MPU_Read_Len(MPU_ADDR,MPU_GYRO_XOUTH_REG,6,buf);
	if(res==0)
	{
		*gx=((uint16_t)buf[0]<<8)|buf[1];  
		*gy=((uint16_t)buf[2]<<8)|buf[3];  
		*gz=((uint16_t)buf[4]<<8)|buf[5];
	} 	
    return res;;
}

//得到温度值
//返回值:温度值
uint8_t MPU_Get_Temperature(float *t)
{
	uint8_t buf[6],res;  

	res=MPU_Read_Len(MPU_ADDR,MPU_TEMP_OUTH_REG,2,buf);
	if(res==0)
	{
		*t = ((uint16_t)buf[0]<<8)|buf[1];  
		*t = 36.53+(*t)/340;  
	} 	
	
	return res;
}


//IIC连续读
//addr:器件地址
//reg:要读取的寄存器地址
//len:要读取的长度
//buf:读取到的数据存储区
//返回值:0,正常
//    其他,错误代码
uint8_t MPU_Read_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{ 
    
	IIC_Start();
	IIC_Send_Byte((addr<<1) | 0);
	if(IIC_Wait_Ack())
	{
		IIC_Stop();
		return 1;
	}
	IIC_Send_Byte(reg);
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte((addr<<1) | 1);
	IIC_Wait_Ack();
	while(len)
	{
		if(len == 1)
		{
			*buf = IIC_Rcv_Byte(0);
		}
		else
		{
			*buf = IIC_Rcv_Byte(1);
		}
		len--;
		buf++;
	}
	IIC_Stop();
	return 0;
    
}


//IIC连续写
//addr:器件地址 
//reg:寄存器地址
//len:写入长度
//buf:数据区
//返回值:0,正常
//    其他,错误代码
uint8_t MPU_Write_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf)
{
	uint8_t i;
	
	IIC_Start();
	IIC_Send_Byte((addr<<1) | 0);
	if(IIC_Wait_Ack())
	{
		IIC_Stop();
		return 1;
	}
	IIC_Send_Byte(reg);
	IIC_Wait_Ack();
	for(i=0;i<len;i++)
	{
		IIC_Send_Byte(buf[i]);
		if(IIC_Wait_Ack())
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_Stop();
	return 0;
}



/******************************
*     Mpu_Dmp Init
******************************/
MPU6050_MAG g_sMpu6050Mag;

void Mpu6050_Init(MPU6050_MAG *pMag)
{
    uint8_t i, ret = 0;
  
		g_sMpu6050Mag.Flag = 0;
	
    for(i = 0; i < 3; i++)
    {
        ret = mpu_dmp_init();

        if(ret == 0)
        {
            break;
        }

        Delay_Ms(50ul);//200
    }
}


void Mpu6050_GetAccel(float *gx, float *gy, float *gz)
{
    short G[3];

    if(0 == mpu_get_accel_reg(G, NULL))
    {
        if(gx != NULL)
        {
            *gx = (float)G[0] / 16384.0 * 9.80665; //量程2g
        }

        if(gy != NULL)
        {
            *gy = (float)G[1] / 16384.0 * 9.80665; //量程2g
        }

        if(gz != NULL)
        {
            *gz = (float)G[2] / 16384.0 * 9.80665; //量程2g
        }
         if(gx != NULL || gy != NULL || gz != NULL)
        {
            g_sMpu6050Mag.Flag = TRUE;
        }
    }

}

void Mpu6050_TimedProcess(MPU6050_MAG *pMag)
{
    static u8 cnt = 1;

    //if(GetSysRunMilliSecond() % 1000 == 0)	//每隔1秒检测一次
    {
        cnt = mpu_dmp_get_data(&pMag->m_fPitch, &pMag->m_fRoll, &pMag->m_fYaw);
        Mpu6050_GetAccel(&pMag->m_fGx, &pMag->m_fGy, &pMag->m_fGz);
				pMag->Pose = PoseAnalyse(pMag);
				
				/*
				if(cnt == 0)
				{
					pMag->Flag = 1;
				}
				*/
        //LOG("MPU6050:(%.2f,%.2f,%.2f)",pMag->m_fPitch,pMag->m_fRoll,pMag->m_fYaw);
    }

    //MPU_Get_Temperature(&pMag->m_fTempe);
    //LOG(" T:%.2f",pMag->m_fTempe);
    //Mpu6050_GetAccel(&pMag->m_fGx,&pMag->m_fGy,&pMag->m_fGz);
    //LOG(" G:%.2f,%.2f,%.2f\n\r",pMag->m_fGx,pMag->m_fGy,pMag->m_fGz);
}



MPU6050_MAG *m_byg_sMpu6050Mag(void)
{
    return &g_sMpu6050Mag;
}



