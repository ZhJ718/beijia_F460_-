#ifndef __DRV_CONFIG_H_
#define __DRV_CONFIG_H_

#if defined(AT32F4xx)

    //没用到哪些串口，注释掉相应宏，节省空间
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
