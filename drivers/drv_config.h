#ifndef __DRV_CONFIG_H_
#define __DRV_CONFIG_H_

#if defined(AT32F4xx)

    //û�õ���Щ���ڣ�ע�͵���Ӧ�꣬��ʡ�ռ�
    #define ENABLE_COM1
    #define ENABLE_COM2
    #define ENABLE_COM3
    #define ENABLE_COM4
    #define ENABLE_COM5
    //#define ENABLE_COM6
    //#define ENABLE_COM7
    //#define ENABLE_COM8

#elif defined(HC32F46x)

    #define ENABLE_COM1
    #define ENABLE_COM2
    #define ENABLE_COM3
    #define ENABLE_COM4

#endif




#endif
