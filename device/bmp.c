#include "bmp.h"
#include "iic.h"


void IIC_Init(void)
{
    stc_port_init_t stcPortInit = {0};

//	PORT_SetFunc(BMP_SCL_PORT, BMP_SCL_PIN,Func_Gpio,Disable);
    PORT_SetFunc(BMP_SDA_PORT, BMP_SDA_PIN, Func_Gpio, Disable);

    stcPortInit.enPinMode = Pin_Mode_Out;
    stcPortInit.enPinOType = Pin_OType_Cmos;
    stcPortInit.enPullUp = Enable;
    stcPortInit.enLatch = Disable;
    stcPortInit.enInvert = Disable;
//	stcPortInit.enPinDrv = Pin_Drv_H;
//	stcPortInit.enPinOType = Pin_OType_Od;

    PORT_Init(BMP_SCL_PORT, BMP_SCL_PIN, &stcPortInit);
    PORT_Init(BMP_SDA_PORT, BMP_SDA_PIN, &stcPortInit);

    PORT_SetBits(BMP_SCL_PORT, BMP_SCL_PIN);
    PORT_SetBits(BMP_SDA_PORT, BMP_SDA_PIN);
}


void IIC_Start(void)
{
    SDA_OUT();
    SDA_SET_HIGH();
    SCL_SET_HIGH();
    Ddl_Delay1us(4);
    //START:when CLK is high,DATA change form high to low
    SDA_SET_LOW();
    Ddl_Delay1us(4);
    SCL_SET_LOW();
}

/*
 @ describetion: IIC stop signal
 @ param: none
 @ return: none
 @ author: Xuewei Zhou
 @ date : 2019-4-17
 @ note:
 @ function void IIC_Stop(void)
*/
void IIC_Stop(void)
{
    SDA_OUT();
    SCL_SET_LOW();
    //STOP:when CLK is high DATA change form low to high
    SDA_SET_LOW();
    Ddl_Delay1us(4);
    SCL_SET_HIGH();
    SDA_SET_HIGH();
    Ddl_Delay1us(4);
}
/*
 @ describetion: IIC wait ack signal function
 @ param: none
 @ return:  1-> Ack failed
 @			0-> Ack succeed
 @ author: Xuewei Zhou
 @ date : 2019-4-17
 @ note:
 @ function: unsigned char IIC_Wait_Ack(void)
*/
unsigned char IIC_Wait_Ack(void)
{
    u8 ucErrTime = 0;

    SDA_IN();
    SDA_SET_HIGH();
    Ddl_Delay1us(1);
    SCL_SET_HIGH();
    Ddl_Delay1us(1);

    while(READ_SDA())
    {
        ucErrTime++;

        if(ucErrTime > 50)
        {
            IIC_Stop();
            return 1;
        }

        Ddl_Delay1us(4);
    }

    SCL_SET_LOW();
    return 0;
}
/*
 @ describetion: produce ack signal function
 @ param: none
 @ return:  none
 @ author: Xuewei Zhou
 @ date : 2019-4-17
 @ note:
 @ function: void IIC_Ack(void)
*/
void IIC_Ack(void)
{
    SCL_SET_LOW();
    SDA_OUT();
    SDA_SET_LOW();
    Ddl_Delay1us(4);
    SCL_SET_HIGH();
    Ddl_Delay1us(4);
    SCL_SET_LOW();
}
/*
 @ describetion: not produce ack signal function
 @ param: none
 @ return:  none
 @ author: Xuewei Zhou
 @ date : 2019-4-17
 @ note:
 @ function: void IIC_NAck(void)
*/
void IIC_NAck(void)
{
    SCL_SET_LOW();
    SDA_OUT();
    SDA_SET_HIGH();
    Ddl_Delay1us(4);
    SCL_SET_HIGH();
    Ddl_Delay1us(4);
    SCL_SET_LOW();
}

/*
 @ describetion: IIC send char data
 @ param: none
 @ return:  none
 @ author: Xuewei Zhou
 @ date : 2019-4-17
 @ note:
 @ function: void IIC_NAck(void)
*/
void IIC_Send_Byte(u8 txd)
{
    u8 t;

    SDA_OUT();
    SCL_SET_LOW();

    for(t = 0; t < 8; t++)
    {
        if(txd & 0x80)
            SDA_SET_HIGH();
        else
            SDA_SET_LOW();

        txd <<= 1;
        Ddl_Delay1us(4);
        SCL_SET_HIGH();
        Ddl_Delay1us(4);
        SCL_SET_LOW();
        Ddl_Delay1us(4);
    }
}
/*
 @ describetion: IIC read char data
 @ param: ack 1-> send ACK
 @			  0->send not ACK
 @ return:  none
 @ author: Xuewei Zhou
 @ date : 2019-4-17
 @ note:
 @ function: unsigned char IIC_Read_Byte(unsigned char ack)
*/
unsigned char IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, receive = 0;

    SDA_IN();

    for(i = 0; i < 8; i++ )
    {
        SCL_SET_LOW();
        Ddl_Delay1us(4);
        SCL_SET_HIGH();
        receive <<= 1;

        if(READ_SDA())
            receive |= 1;

        Ddl_Delay1us(4);
    }

    if (!ack)
        IIC_NAck();
    else
        IIC_Ack();

    return receive;
}




unsigned char Bmp_Write_Byte(u8 reg, u8 data)
{
    IIC_Start();
    IIC_Send_Byte((BMP_ADDR << 1) | 0); //发送器件地址+写命令

    if(IIC_Wait_Ack())	//等待应答
    {
        IIC_Stop();
        return 1;
    }

    IIC_Send_Byte(reg);	//写寄存器地址
    IIC_Wait_Ack();		//等待应答
    IIC_Send_Byte(data);//发送数据

    if(IIC_Wait_Ack())	//等待ACK
    {
        IIC_Stop();
        return 1;
    }

    IIC_Stop();
    return 0;
}
//IIC读一个字节
//reg:寄存器地址
//返回值:读到的数据
unsigned char Bmp_Read_Byte(u8 reg)
{
    u8 res;

    IIC_Start();
    IIC_Send_Byte((BMP_ADDR << 1) | 0); //发送器件地址+写命令
    IIC_Wait_Ack();		//等待应答
    IIC_Send_Byte(reg);	//写寄存器地址
    IIC_Wait_Ack();		//等待应答
    IIC_Start();
    IIC_Send_Byte((BMP_ADDR << 1) | 1); //发送器件地址+读命令
    IIC_Wait_Ack();		//等待应答
    res = IIC_Read_Byte(0); //读取数据,发送nACK
    IIC_Stop();			//产生一个停止条件

    return res;
}


/******
****************************BMP280 参数配置*********************************
*******/
void Bmp280_Reset(void)
{
    Bmp_Write_Byte(BMP280_RESET_REG, BMP280_RESET_VALUE);
}

void bmp280_set_standby_time(BMP_MANAGER *pMag, BMP280_T_SB t_standby)
{
    uint8_t config_reg;

    pMag->t_sb = t_standby;
    config_reg = pMag->t_sb << 5 | pMag->filter_coefficient << 2;

    Bmp_Write_Byte(BMP280_CONFIG_REG, config_reg);
}


void bmp280_set_work_mode(BMP_MANAGER *pMag, BMP280_WORK_MODE mode)
{
    uint8_t ctrlmeas_reg;

    pMag->mode = mode;
    ctrlmeas_reg = pMag->t_oversampling << 5 | pMag->p_oversampling << 2 | pMag->mode;

    Bmp_Write_Byte(BMP280_CTRLMEAS_REG, ctrlmeas_reg);
}

void bmp280_set_temperature_oversampling_mode(BMP_MANAGER *pMag, BMP280_T_OVERSAMPLING t_osl)
{
    uint8_t ctrlmeas_reg;

    pMag->t_oversampling = t_osl;
    ctrlmeas_reg = pMag->t_oversampling << 5 | pMag->p_oversampling << 2 | pMag->mode;

    Bmp_Write_Byte(BMP280_CTRLMEAS_REG, ctrlmeas_reg);
}

void bmp280_set_pressure_oversampling_mode(BMP_MANAGER *pMag, BMP280_P_OVERSAMPLING p_osl)
{
    uint8_t ctrlmeas_reg;

    pMag->t_oversampling = p_osl;
    ctrlmeas_reg = pMag->t_oversampling << 5 | pMag->p_oversampling << 2 | pMag->mode;

    Bmp_Write_Byte( BMP280_CTRLMEAS_REG, ctrlmeas_reg);
}

void bmp280_set_filter_mode(BMP_MANAGER *pMag, BMP280_FILTER_COEFFICIENT f_coefficient)
{
    uint8_t config_reg;

    pMag->filter_coefficient = f_coefficient;
    config_reg = pMag->t_sb << 5 | pMag->filter_coefficient << 2;

    Bmp_Write_Byte(BMP280_CONFIG_REG, config_reg);
}


/*  Returns  temperature  in  DegC,  double  precision.  Output  value  of  “51.23”  equals  51.23  DegC.  */
static  double  bmp280_compensate_temperature_double(BMP_MANAGER *pMag,  int32_t  adc_T)
{
    double  var1,  var2,  temperature;

    var1  =  (((double)  adc_T)  /  16384.0  -  ((double)  pMag->T1)  /  1024.0)
             *  ((double)  pMag->T2);
    var2  =  ((((double)  adc_T)  /  131072.0  -  ((double)  pMag->T1)  /  8192.0)
              *  (((double)  adc_T)  /  131072.0  -  ((double)  pMag->T1)  /  8192.0))
             *  ((double)  pMag->T3);
    pMag->t_fine  =  (int32_t)  (var1  +  var2);
    temperature  =  (var1  +  var2)  /  5120.0;

    return  temperature;
}


/*  Returns  pressure  in  Pa  as  double.  Output  value  of  “96386.2”  equals  96386.2  Pa  =  963.862  hPa  */
static  double  bmp280_compensate_pressure_double(BMP_MANAGER *pMag,  int32_t  adc_P)
{
    double  var1,  var2,  pressure;

    var1  =  ((double)  pMag->t_fine  /  2.0)  -  64000.0;
    var2  =  var1  *  var1  *  ((double)  pMag->P6)  /  32768.0;
    var2  =  var2  +  var1  *  ((double)  pMag->P5)  *  2.0;
    var2  =  (var2  /  4.0)  +  (((double)  pMag->P4)  *  65536.0);
    var1  =  (((double)  pMag->P3)  *  var1  *  var1  /  524288.0
              +  ((double)  pMag->P2)  *  var1)  /  524288.0;
    var1  =  (1.0  +  var1  /  32768.0)  *  ((double)  pMag->P1);

    if  (var1  ==  0.0)
    {
        return  0;  //  avoid  exception  caused  by  division  by  zero
    }

    pressure  =  1048576.0  -  (double)  adc_P;
    pressure  =  (pressure  -  (var2  /  4096.0))  *  6250.0  /  var1;
    var1  =  ((double)  pMag->P9)  *  pressure  *  pressure  /  2147483648.0;
    var2  =  pressure  *  ((double)  pMag->P8)  /  32768.0;
    pressure  =  pressure  +  (var1  +  var2  +  ((double)  pMag->P7))  /  16.0;

    return  pressure;
}


/*  Returns  temperature  in  DegC,  double  precision.
  Output  value  of  “51.23”  equals  51.23  DegC.  */
double  bmp280_get_temperature(BMP_MANAGER *pMag)
{
    uint8_t  lsb,  msb,  xlsb;
    int32_t  adc_T;
    double  temperature;

    xlsb  =  Bmp_Read_Byte( BMP280_TEMPERATURE_XLSB_REG);
    lsb  =  Bmp_Read_Byte( BMP280_TEMPERATURE_LSB_REG);
    msb  =  Bmp_Read_Byte( BMP280_TEMPERATURE_MSB_REG);

    adc_T  =  (msb  <<  12)  |  (lsb  <<  4)  |  (xlsb  >>  4);
    Ddl_Delay1us(5);
    temperature  =  bmp280_compensate_temperature_double(pMag,  adc_T);

    return  temperature;
}

/*  Returns  pressure  in  Pa  as  double.  Output  value  of  “96386.2”  equals  96386.2  Pa  =  963.862  hPa  */
double  bmp280_get_pressure(BMP_MANAGER *pMag)
{
    uint8_t  lsb,  msb,  xlsb;
    int32_t  adc_P;
    double  pressure;


    xlsb  =  Bmp_Read_Byte( BMP280_PRESSURE_XLSB_REG);
    lsb  =  Bmp_Read_Byte( BMP280_PRESSURE_LSB_REG);
    msb  =  Bmp_Read_Byte( BMP280_PRESSURE_MSB_REG);

    adc_P  =  (msb  <<  12)  |  (lsb  <<  4)  |  (xlsb  >>  4);
    pressure  =  bmp280_compensate_pressure_double(pMag,  adc_P);

    return  pressure;
}

/**
  *  仅在BMP280被设置为normal  mode后，
  *  可使用该接口直接读取温度和气压。
  */
void  bmp280_get_temperature_and_pressure(BMP_MANAGER *pMag,  double  *temperature,  double  *pressure)
{
    *temperature  =  bmp280_get_temperature(pMag);
    *pressure  =  bmp280_get_pressure(pMag);
}

/**
  *  当BMP280被设置为forced  mode后，
  *  可使用该接口直接读取温度和气压。
  */
void  bmp280_forced_mode_get_temperature_and_pressure(BMP_MANAGER *pMag,  double  *temperature,  double  *pressure)
{
    bmp280_set_work_mode(pMag,  BMP280_FORCED_MODE);

    Ddl_Delay1ms(100);
    IWDG_Feed();
    bmp280_get_temperature_and_pressure(pMag,  temperature,  pressure);
}

u8 BmpConfig(void)
{
    BMP_MANAGER *pMag = &g_sBmpMag;
    uint8_t lsb, msb;
    uint8_t ctrlmeas_reg, config_reg;

    IIC_Init();
    u8 Id = Bmp_Read_Byte(0xD0);

    if(Id == 0x58)
    {
        pMag->mode = BMP280_SLEEP_MODE;
        pMag->t_sb = BMP280_T_SB1;
        pMag->p_oversampling = BMP280_P_MODE_1;
        pMag->t_oversampling = BMP280_T_MODE_1;
        pMag->filter_coefficient = BMP280_FILTER_MODE_1;
    }
    else
    {
        return 0;
    }

    /*  read  the  temperature  calibration  parameters  */
    lsb  =  Bmp_Read_Byte( BMP280_DIG_T1_LSB_REG);
    msb  =  Bmp_Read_Byte( BMP280_DIG_T1_MSB_REG);
    pMag->T1  =  msb  <<  8  |  lsb;
    lsb  =  Bmp_Read_Byte( BMP280_DIG_T2_LSB_REG);
    msb  =  Bmp_Read_Byte( BMP280_DIG_T2_MSB_REG);
    pMag->T2  =  msb  <<  8  |  lsb;
    lsb  =  Bmp_Read_Byte( BMP280_DIG_T3_LSB_REG);
    msb  =  Bmp_Read_Byte( BMP280_DIG_T3_MSB_REG);
    pMag->T3  =  msb  <<  8  |  lsb;

    /* read the pressure calibration parameters */
    lsb = Bmp_Read_Byte(BMP280_DIG_P1_LSB_REG);
    msb = Bmp_Read_Byte(BMP280_DIG_P1_MSB_REG);
    pMag->P1 = msb << 8 | lsb;
    lsb = Bmp_Read_Byte(BMP280_DIG_P2_LSB_REG);
    msb = Bmp_Read_Byte(BMP280_DIG_P2_MSB_REG);
    pMag->P2 = msb << 8 | lsb;
    lsb = Bmp_Read_Byte(BMP280_DIG_P3_LSB_REG);
    msb = Bmp_Read_Byte(BMP280_DIG_P3_MSB_REG);
    pMag->P3 = msb << 8 | lsb;
    lsb = Bmp_Read_Byte(BMP280_DIG_P4_LSB_REG);
    msb = Bmp_Read_Byte(BMP280_DIG_P4_MSB_REG);
    pMag->P4 = msb << 8 | lsb;
    lsb = Bmp_Read_Byte(BMP280_DIG_P5_LSB_REG);
    msb = Bmp_Read_Byte(BMP280_DIG_P5_MSB_REG);
    pMag->P5 = msb << 8 | lsb;
    lsb = Bmp_Read_Byte(BMP280_DIG_P6_LSB_REG);
    msb = Bmp_Read_Byte(BMP280_DIG_P6_MSB_REG);
    pMag->P6 = msb << 8 | lsb;
    lsb = Bmp_Read_Byte(BMP280_DIG_P7_LSB_REG);
    msb = Bmp_Read_Byte(BMP280_DIG_P7_MSB_REG);
    pMag->P7 = msb << 8 | lsb;
    lsb = Bmp_Read_Byte(BMP280_DIG_P8_LSB_REG);
    msb = Bmp_Read_Byte(BMP280_DIG_P8_MSB_REG);
    pMag->P8 = msb << 8 | lsb;
    lsb = Bmp_Read_Byte(BMP280_DIG_P9_LSB_REG);
    msb = Bmp_Read_Byte(BMP280_DIG_P9_MSB_REG);
    pMag->P9 = msb << 8 | lsb;

    Bmp280_Reset();
//	 Id = Bmp_Read_Byte(0xD0);
//	 IWDG_Feed();
    ctrlmeas_reg = pMag->t_oversampling << 5 |  \
                   pMag->p_oversampling << 2 | pMag->mode;

    config_reg  =  pMag->t_sb  <<  5  |  pMag->filter_coefficient  <<  2;

    Bmp_Write_Byte(BMP280_CTRLMEAS_REG,  ctrlmeas_reg);
    Bmp_Write_Byte(BMP280_CONFIG_REG,  config_reg);
//	 IWDG_Feed();
    Ddl_Delay1ms(100);
//	 IWDG_Feed();
    volatile u8 myId = Bmp_Read_Byte(BMP280_CHIPID_REG);

}


void Bmp280TimedPorcess(BMP_MANAGER *pMag)
{
    pMag->pressure = bmp280_get_pressure(pMag);
    Ddl_Delay1us(5);
    pMag->temperature = bmp280_get_temperature(pMag);
//   bmp280_forced_mode_get_temperature_and_pressure(pMag,&pMag->temperature, &pMag->pressure);

}






