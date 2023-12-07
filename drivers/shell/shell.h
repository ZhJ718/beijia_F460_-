/**
 * @file shell.h
 * @author Letter (NevermindZZT@gmail.com)
 * @brief letter shell
 * @version 3.0.0
 * @date 2019-12-30
 *
 * @Copyright (c) 2020 Letter
 *
 */

#ifndef     __SHELL_H__
#define     __SHELL_H__

#include "shell_cfg.h"

#define     SHELL_VERSION               "3.0.0-beta2"                 /**< 版本�?*/


/**
 * @brief shell 断言
 *
 * @param expr 表达�? * @param action 断言失败操作
 */
#define     SHELL_ASSERT(expr, action) \
    if (!(expr)) { \
        action; \
    }

/**
 * @brief shell 命令权限
 *
 * @param permission 权限级别
 */
#define     SHELL_CMD_PERMISSION(permission) \
    (permission & 0x000000FF)

/**
 * @brief shell 命令类型
 *
 * @param type 类型
 */
#define     SHELL_CMD_TYPE(type) \
    ((type & 0x0000000F) << 8)

#define     SHELL_CMD_ENABLE_UNCHECKED \
    (1 << 12)

#define     SHELL_CMD_DISABLE_RETURN \
    (1 << 13)


#if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && __ARMCC_VERSION >= 6000000)
    #define SECTION(x)                  __attribute__((section(x)))
#elif defined(__ICCARM__)
    #define SECTION(x)                  @ x
#elif defined(__GNUC__)
    #define SECTION(x)                  __attribute__((section(x)))
#else
    #define SECTION(x)
#endif

#if SHELL_USING_CMD_EXPORT == 1

/**
 * @brief shell 命令定义
 *
 * @param _attr 命令属�?     * @param _name 命令�?     * @param _func 命令函数
 * @param _desc 命令描述
 */
#define SHELL_EXPORT_CMD(_attr, _name, _func, _desc) \
    const char shellCmd##_name[] = #_name; \
    const char shellDesc##_name[] = #_desc; \
    const ShellCommand \
    shellCommand##_name SECTION("shellCommand") =  \
            { \
              .attr.value = _attr, \
              .data.cmd.name = shellCmd##_name, \
              .data.cmd.function = (int (*)())_func, \
              .data.cmd.desc = shellDesc##_name \
            }

/**
 * @brief shell 变量定义
 *
 * @param _attr 变量属�?     * @param _name 变量�?     * @param _value 变量�?     * @param _desc 变量描述
 */
#define SHELL_EXPORT_VAR(_attr, _name, _value, _desc) \
    const char shellCmd##_name[] = #_name; \
    const char shellDesc##_name[] = #_desc; \
    const ShellCommand \
    shellVar##_name SECTION("shellCommand") =  \
            { \
              .attr.value = _attr, \
              .data.var.name = shellCmd##_name, \
              .data.var.value = (void *)_value, \
              .data.var.desc = shellDesc##_name \
            }

/**
 * @brief shell 用户定义
 *
 * @param _attr 用户属�?     * @param _name 用户�?     * @param _password 用户密码
 * @param _desc 用户描述
 */
#define SHELL_EXPORT_USER(_attr, _name, _password, _desc) \
    const char shellCmd##_name[] = #_name; \
    const char shellPassword##_name[] = #_password; \
    const char shellDesc##_name[] = #_desc; \
    const ShellCommand \
    shellUser##_name SECTION("shellCommand") =  \
            { \
              .attr.value = _attr|SHELL_CMD_TYPE(SHELL_TYPE_USER), \
              .data.user.name = shellCmd##_name, \
              .data.user.password = shellPassword##_name, \
              .data.user.desc = shellDesc##_name \
            }

/**
 * @brief shell 按键定义
 *
 * @param _attr 按键属�?     * @param _value 按键键�?     * @param _func 按键函数
 * @param _desc 按键描述
 */
#define SHELL_EXPORT_KEY(_attr, _value, _func, _desc) \
    const char shellDesc##_value[] = #_desc; \
    const ShellCommand \
    shellKey##_value SECTION("shellCommand") =  \
            { \
              .attr.value = _attr|SHELL_CMD_TYPE(SHELL_TYPE_KEY), \
              .data.key.value = _value, \
              .data.key.function = (void (*)(Shell *))_func, \
              .data.key.desc = shellDesc##_value \
            }
#else
/**
 * @brief shell 命令item定义
 *
 * @param _attr 命令属�?     * @param _name 命令�?     * @param _func 命令函数
 * @param _desc 命令描述
 */
#define SHELL_CMD_ITEM(_attr, _name, _func, _desc) \
    { \
        .attr.value = _attr, \
                      .data.cmd.name = #_name, \
                                       .data.cmd.function = (int (*)())_func, \
                                               .data.cmd.desc = #_desc \
    }

/**
 * @brief shell 变量item定义
 *
 * @param _attr 变量属�?     * @param _name 变量�?     * @param _value 变量�?     * @param _desc 变量描述
 */
#define SHELL_VAR_ITEM(_attr, _name, _value, _desc) \
    { \
        .attr.value = _attr, \
                      .data.var.name = #_name, \
                                       .data.var.value = (void *)_value, \
                                               .data.var.desc = #_desc \
    }

/**
 * @brief shell 用户item定义
 *
 * @param _attr 用户属�?     * @param _name 用户�?     * @param _password 用户密码
 * @param _desc 用户描述
 */
#define SHELL_USER_ITEM(_attr, _name, _password, _desc) \
    { \
        .attr.value = _attr|SHELL_CMD_TYPE(SHELL_TYPE_USER), \
                      .data.user.name = #_name, \
                                        .data.user.password = #_password, \
                                                .data.user.desc = #_desc \
    }

/**
 * @brief shell 按键item定义
 *
 * @param _attr 按键属�?     * @param _value 按键键�?     * @param _func 按键函数
 * @param _desc 按键描述
 */
#define SHELL_KEY_ITEM(_attr, _value, _func, _desc) \
    { \
        .attr.value = _attr|SHELL_CMD_TYPE(SHELL_TYPE_KEY), \
                      .data.key.value = _value, \
                                        .data.key.function = (void (*)(Shell *))_func, \
                                                .data.key.desc = #_desc \
    }

#define SHELL_EXPORT_CMD(_attr, _name, _func, _desc)
#define SHELL_EXPORT_VAR(_attr, _name, _value, _desc)
#define SHELL_EXPORT_USER(_attr, _name, _password, _desc)
#define SHELL_EXPORT_KEY(_attr, _value, _func, _desc)
#endif /** SHELL_USING_CMD_EXPORT == 1 */

/**
 * @brief shell command类型
 */
typedef enum
{
    SHELL_TYPE_CMD_MAIN = 0,                                    /**< main形式命令 */
    SHELL_TYPE_CMD_FUNC,                                        /**< C函数形式命令 */
    SHELL_TYPE_VAR_INT,                                         /**< int型变�?*/
    SHELL_TYPE_VAR_SHORT,                                       /**< short型变�?*/
    SHELL_TYPE_VAR_CHAR,                                        /**< char型变�?*/
    SHELL_TYPE_VAR_POINT,                                       /**< 指针型变�?*/
    SHELL_TYPE_VAL,                                             /**< 常量 */
    SHELL_TYPE_USER,                                            /**< 用户 */
    SHELL_TYPE_KEY,                                             /**< 按键 */
} ShellCommandType;


/**
 * @brief Shell定义
 */
typedef struct
{
    struct
    {
        const struct shell_command *user;                       /**< 当前用户 */
        int activeTime;                                         /**< shell激活时�?*/
    } info;
    struct
    {
        unsigned short length;                                  /**< 输入数据长度 */
        unsigned short cursor;                                  /**< 当前光标位置 */
        char *buffer;                                           /**< 输入缓冲 */
        unsigned short bufferSize;                              /**< 输入缓冲大小 */
        char *param[SHELL_PARAMETER_MAX_NUMBER];                /**< 参数 */
        unsigned short paramCount;                              /**< 参数数量 */
        int keyValue;                                           /**< 输入按键键�?*/
    } parser;
    struct
    {
        char *item[SHELL_HISTORY_MAX_NUMBER];                   /**< 历史记录 */
        unsigned short number;                                  /**< 历史记录�?*/
        unsigned short record;                                  /**< 当前记录位置 */
        signed short offset;                                    /**< 当前历史记录偏移 */
    } history;
    struct
    {
        void *base;                                             /**< 命令表基址 */
        unsigned short count;                                   /**< 命令数量 */
    } commandList;
    struct
    {
        unsigned char isChecked : 1;                            /**< 密码校验通过 */
        unsigned char isActive : 1;                             /**< 当前活动Shell */
        unsigned char tabFlag : 1;                              /**< tab标志 */
    } status;
    signed char (*read)(char *, uint16_t *);                               /**< shell读函�?*/
    void (*write)(const char);                                  /**< shell写函�?*/
} Shell;


/**
 * @brief shell command定义
 */
typedef struct shell_command
{
    union
    {
        struct
        {
            unsigned char permission : 8;                       /**< command权限 */
            ShellCommandType type : 4;                          /**< command类型 */
            unsigned char enableUnchecked : 1;                  /**< 在未校验密码的情况下可用 */
            unsigned char disableReturn : 1;                    /**< 禁用返回值输�?*/
        } attrs;
        int value;
    } attr;                                                     /**< 属�?*/
    union
    {
        struct
        {
            const char *name;                                   /**< 命令�?*/
            int (*function)();                                  /**< 命令执行函数 */
            const char *desc;                                   /**< 命令描述 */
        } cmd;                                                  /**< 命令定义 */
        struct
        {
            const char *name;                                   /**< 变量�?*/
            void *value;                                        /**< 变量�?*/
            const char *desc;                                   /**< 变量描述 */
        } var;                                                  /**< 变量定义 */
        struct
        {
            const char *name;                                   /**< 用户�?*/
            const char *password;                               /**< 用户密码 */
            const char *desc;                                   /**< 用户描述 */
        } user;                                                 /**< 用户定义 */
        struct
        {
            int value;                                          /**< 按键键�?*/
            void (*function)(Shell *);                          /**< 按键执行函数 */
            const char *desc;                                   /**< 按键描述 */
        } key;                                                  /**< 按键定义 */
    } data;
} ShellCommand;


void shellInit(Shell *shell, char *buffer, unsigned short size);
unsigned short shellWriteString(Shell *shell, const char *string);
void shellPrint(Shell *shell, char *fmt, ...);
Shell* shellGetCurrent(void);
void shellHandler(Shell *shell, char data);
void shellTask(void *param);

#endif

