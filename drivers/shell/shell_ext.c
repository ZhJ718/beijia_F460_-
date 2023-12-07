/**
 * @file shell_ext.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief shell extensions
 * @version 3.0.0
 * @date 2019-12-31
 *
 * @Copyright (c) 2019 Letter
 *
 */

#include "shell_cfg.h"
#include "shell.h"
#include "shell_ext.h"


extern ShellCommand* shellSeekCommand(Shell *shell,
                                      const char *cmd,
                                      ShellCommand *base,
                                      unsigned short compareLength);
extern int shellGetVarValue(Shell *shell, ShellCommand *command);

/**
 * @brief 判断数字进制
 *
 * @param string 参数字符�? * @return NUM_Type 进制
 */
static NUM_Type shellExtNumType(char *string)
{
    char *p = string;
    NUM_Type type = NUM_TYPE_INT;

    if ((*p == '0') && ((*(p + 1) == 'x') || (*(p + 1) == 'X')))
    {
        type = NUM_TYPE_HEX;
    }
    else if ((*p == '0') && ((*(p + 1) == 'b') || (*(p + 1) == 'B')))
    {
        type = NUM_TYPE_BIN;
    }
    else if (*p == '0')
    {
        type = NUM_TYPE_OCT;
    }

    while (*p++)
    {
        if (*p == '.' && *(p + 1) != 0)
        {
            type = NUM_TYPE_FLOAT;
            break;
        }
    }

    return type;
}


/**
 * @brief 字符转数�? *
 * @param code 字符
 * @return char 数字
 */
static char shellExtToNum(char code)
{
    if ((code >= '0') && (code <= '9'))
    {
        return code - '0';
    }
    else if ((code >= 'a') && (code <= 'f'))
    {
        return code - 'a' + 10;
    }
    else if ((code >= 'A') && (code <= 'F'))
    {
        return code - 'A' + 10;
    }
    else
    {
        return 0;
    }
}


/**
 * @brief 解析字符参数
 *
 * @param string 字符串参�? * @return char 解析出的字符
 */
static char shellExtParseChar(char *string)
{
    char *p = string + 1;
    char value = 0;

    if (*p == '\\')
    {
        switch (*(p + 1))
        {
            case 'b':
                value = '\b';
                break;

            case 'r':
                value = '\r';
                break;

            case 'n':
                value = '\n';
                break;

            case 't':
                value = '\t';
                break;

            case '0':
                value = 0;
                break;

            default:
                value = *(p + 1);
                break;
        }
    }
    else
    {
        value = *p;
    }

    return value;
}


/**
 * @brief 解析字符串参�? *
 * @param string 字符串参�? * @return char* 解析出的字符�? */
static char* shellExtParseString(char *string)
{
    char *p = string;
    unsigned short index = 0;

    if (*string == '\"')
    {
        p = ++string;
    }

    while (*p)
    {
        if (*p == '\\')
        {
            *(string + index) = shellExtParseChar(p - 1);
            p++;
        }
        else if (*p == '\"')
        {
            *(string + index) = 0;
        }
        else
        {
            *(string + index) = *p;
        }

        p++;
        index ++;
    }

    *(string + index) = 0;
    return string;
}


/**
 * @brief 解析数字参数
 *
 * @param string 字符串参�? * @return unsigned int 解析出的数字
 */
static unsigned int shellExtParseNumber(char *string)
{
    NUM_Type type = NUM_TYPE_INT;
    char radix = 10;
    char *p = string;
    char offset = 0;
    signed char sign = 1;
    unsigned int valueInt = 0;
    float valueFloat = 0.0;
    unsigned int devide = 0;

    if (*string == '-')
    {
        sign = -1;
    }

    type = shellExtNumType(string + ((sign == -1) ? 1 : 0));

    switch ((char)type)
    {
        case NUM_TYPE_HEX:
            radix = 16;
            offset = 2;
            break;

        case NUM_TYPE_OCT:
            radix = 8;
            offset = 1;
            break;

        case NUM_TYPE_BIN:
            radix = 2;
            offset = 2;
            break;

        default:
            break;
    }

    p = string + offset + ((sign == -1) ? 1 : 0);

    while (*p)
    {
        if (*p == '.')
        {
            devide = 1;
            p++;
            continue;
        }

        valueInt = valueInt * radix + shellExtToNum(*p);
        devide *= 10;
        p++;
    }

    if (type == NUM_TYPE_FLOAT && devide != 0)
    {
        valueFloat = (float)valueInt / devide * sign;
        return *(unsigned int *)(&valueFloat);
    }
    else
    {
        return valueInt * sign;
    }
}


/**
 * @brief 解析变量参数
 *
 * @param shell shell对象
 * @param var 变量
 * @return unsigned int 变量�? */
static unsigned int shellExtParseVar(Shell *shell, char *var)
{
    ShellCommand *command = shellSeekCommand(shell,
                            var + 1,
                            shell->commandList.base,
                            0);

    if (command)
    {
        return shellGetVarValue(shell, command);
    }
    else
    {
        return 0;
    }
}


/**
 * @brief 解析参数
 *
 * @param shell shell对象
 * @param string 参数
 * @return unsigned int 解析结果
 */
unsigned int shellExtParsePara(Shell *shell, char *string)
{
    if (*string == '\'' && *(string + 1))
    {
        return (unsigned int)shellExtParseChar(string);
    }
    else if (*string == '-' || (*string >= '0' && *string <= '9'))
    {
        return (unsigned int)shellExtParseNumber(string);
    }
    else if (*string == '$' && *(string + 1))
    {
        return shellExtParseVar(shell, string);
    }
    else if (*string)
    {
        return (unsigned int)shellExtParseString(string);
    }

    return 0;
}


/**
 * @brief 执行命令
 *
 * @param shell shell对象
 * @param function 执行命令的函�? * @param argc 参数个数
 * @param argv 参数
 * @return int 返回�? */
int shellExtRun(Shell *shell, int (*function)(), int argc, char *argv[])
{
    switch (argc)
    {
        case 1:
            return function();

        // break;
        case 2:
            return function(shellExtParsePara(shell, argv[1]));

        // break;
        case 3:
            return function(shellExtParsePara(shell, argv[1]), shellExtParsePara(shell, argv[2]));

        // break;
        case 4:
            return function(shellExtParsePara(shell, argv[1]), shellExtParsePara(shell, argv[2]),
                            shellExtParsePara(shell, argv[3]));

        // break;
        case 5:
            return function(shellExtParsePara(shell, argv[1]), shellExtParsePara(shell, argv[2]),
                            shellExtParsePara(shell, argv[3]), shellExtParsePara(shell, argv[4]));

        // break;
        case 6:
            return function(shellExtParsePara(shell, argv[1]), shellExtParsePara(shell, argv[2]),
                            shellExtParsePara(shell, argv[3]), shellExtParsePara(shell, argv[4]),
                            shellExtParsePara(shell, argv[5]));

        // break;
        case 7:
            return function(shellExtParsePara(shell, argv[1]), shellExtParsePara(shell, argv[2]),
                            shellExtParsePara(shell, argv[3]), shellExtParsePara(shell, argv[4]),
                            shellExtParsePara(shell, argv[5]), shellExtParsePara(shell, argv[6]));

        // break;
        case 8:
            return function(shellExtParsePara(shell, argv[1]), shellExtParsePara(shell, argv[2]),
                            shellExtParsePara(shell, argv[3]), shellExtParsePara(shell, argv[4]),
                            shellExtParsePara(shell, argv[5]), shellExtParsePara(shell, argv[6]),
                            shellExtParsePara(shell, argv[7]));

        // break;
        default:
            return -1;
            // break;
    }
}

