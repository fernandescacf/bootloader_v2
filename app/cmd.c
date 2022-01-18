/**
 * @file        cmd.c
 * @author      Carlos Fernandes
 * @version     1.0
 * @date        13 July, 2018
 * @brief       Command line implementation
*/

/* Includes ----------------------------------------------- */
#include <cmd.h>
#include <parser.h>
#include <loader.h>
#include <serial.h>


/* Private types ------------------------------------------ */



/* Private constants -------------------------------------- */
#ifndef CPU_CORE_COUNT
#define CPU_CORE_COUNT 0x1
#endif


/* Private macros ----------------------------------------- */

#define SKIPWHITESPACES(str)            \
    do{                                 \
        while(' ' == *str)              \
        {                               \
            str++;                      \
        }                               \
    }while(0)

#define CMDASSERT(cmd,state)            \
    do{                                 \
        if(CMD_INVALID == state)        \
        {                               \
            CmdPrintUsage(TRUE, cmd);   \
            return CMD_INVALID;         \
        }                               \
    }while(0)

#define CMDCHECKEND(cmd,str)            \
    do{                                 \
        if('\0' != *str)                \
        {                               \
            CmdPrintUsage(TRUE, cmd);   \
            return CMD_INVALID;         \
        }                               \
    }while(0)

#define SETNULLTERMINATOR(str)                                          \
    do{                                                                 \
        uint32_t __i = 0;                                               \
        while('\n' != str[__i] && '\r' != str[__i] && '\0' != str[__i]) \
        {                                                               \
            __i++;                                                      \
        }                                                               \
        str[__i] = '\0';                                                \
    }while(0)

/* Private variables -------------------------------------- */

static char commandList[5][100] =
{
    "help 'command' - print help for the specified 'command'",
    "load 'type' 'addr' 'size/file' - Load file. Types: sd or serial",
    "go '<core>' 'addr' 'arg0' 'arg1' - start application using core '<core>' at address 'addr'",
    "read 'addr'",
    "write 'addr' 'value'",
};

/* Private function prototypes ---------------------------- */

static void CmdPrintUsage(uint32_t error, cmd_t cmd){

    if(TRUE == error)
    {
        puts("Error Usage: ");
    }

    puts(commandList[cmd]);

    puts("\n");
}

/* Private functions -------------------------------------- */

int32_t CmdInterpretCommand(void)
{
    char Cmdstr[64];
    int32_t state = CMD_INVALID;
    char *ptr = Cmdstr;

    puts("boot>");
    gets(Cmdstr);

    SETNULLTERMINATOR(Cmdstr);

    if('\0' == Cmdstr[0])
    {
        return E_OK;
    }

    cmd_t cmd = CmdParserGetAction(&ptr, &state);

    SKIPWHITESPACES(ptr);

    switch(cmd)
    {
    case cmdHelp:
    {
        if('\0' != *ptr)
        {
            CmdCommand_t helpcmd = CmdParserGetCommand(&ptr, &state);
            SKIPWHITESPACES(ptr);
            CMDCHECKEND(cmdHelp,ptr);
            CmdPrintUsage(FALSE, helpcmd);
        }
        else
        {
            uint32_t i = 0;
            puts("------------ Help ------------\n");
            while(i < (uint32_t)cmdInvalid)
            {
                CmdPrintUsage(FALSE, i);
                i++;
            }
            puts("------------------------------\n");
        }
        break;
    }
    case cmdLoad:
    {
        cmdLoadType_t type = CmdParserGetLoadType(&ptr, &state);
        CMDASSERT(cmdLoad, state);
        SKIPWHITESPACES(ptr);

        CmdAddr_t addr = CmdParserGetAddress(&ptr, &state);
        CMDASSERT(cmdLoad, state);
        SKIPWHITESPACES(ptr);

        if(type == cmdSerial)
        {
            CmdSize_t size = CmdParserGetSize(&ptr, &state);
            CMDASSERT(cmdLoad, state);
            SKIPWHITESPACES(ptr);
            CMDCHECKEND(cmdLoad,ptr);

            LoaderSerialLoad((ptr_t)addr, (uint32_t)size);
        }
        else
        {
            char file[32];
            CmdParserGetStr(&ptr, &state, file);
            CMDASSERT(cmdLoad, state);
            SKIPWHITESPACES(ptr);
            CMDCHECKEND(cmdLoad,ptr);
            
            LoaderSdLoad((ptr_t)addr, file);
        }

        break;
    }
    case cmdGo:
    {
        CmdData_t arg0 = 0x0;
        CmdData_t arg1 = 0x0;
        uint32_t core = 0x0;

        // Check for core to boot
        if('<' == *ptr)
        {
            ptr++;
            SKIPWHITESPACES(ptr);
            if('*' == *ptr)
            {
                core = (uint32_t)-1;
                ptr++;
            }
            else
            {
                core = CmdParserGetData(&ptr, &state);
                CMDASSERT(cmdGo, state);
            }
            SKIPWHITESPACES(ptr);

            if('>' != *ptr)
            {
                CMDASSERT(cmdGo, CMD_INVALID);
            }

            ptr++;
            SKIPWHITESPACES(ptr);

            // Check if specified core is valid
            if(core >= CPU_CORE_COUNT && core != (uint32_t)-1)
            {
                puts("Invalid core\n");
                CMDASSERT(cmdGo, CMD_INVALID);
            }
        }

        CmdAddr_t addr = CmdParserGetAddress(&ptr, &state);
        CMDASSERT(cmdGo, state);
        SKIPWHITESPACES(ptr);

        // Check if we have arg0
        if('\0' != *ptr)
        {
            arg0 = CmdParserGetData(&ptr, &state);
            CMDASSERT(cmdGo, state);
            SKIPWHITESPACES(ptr);
        }

        // Check if we have arg1
        if('\0' != *ptr)
        {
            arg1 = CmdParserGetData(&ptr, &state);
            CMDASSERT(cmdGo, state);
            SKIPWHITESPACES(ptr);
        }

        CMDCHECKEND(cmdGo,ptr);

        LoaderGo((ptr_t)addr, core, arg0, arg1);
        // Will not return

        break;
    }
    case cmdRead:
    {
        CmdAddr_t addr = CmdParserGetAddress(&ptr, &state);
        CMDASSERT(cmdRead, state);
        SKIPWHITESPACES(ptr);
        CMDCHECKEND(cmdRead,ptr);

        CmdData_t data = 0;

        LoaderRead((ptr_t)addr, &data);

        break;
    }
    case cmdWrite:
    {
        CmdAddr_t addr = CmdParserGetAddress(&ptr, &state);
        CMDASSERT(cmdWrite, state);
        SKIPWHITESPACES(ptr);

        CmdData_t data = CmdParserGetData(&ptr, &state);
        CMDASSERT(cmdWrite, state);
        SKIPWHITESPACES(ptr);
        CMDCHECKEND(cmdWrite,ptr);

        LoaderWrite((ptr_t)addr, (uint32_t)data);

        break;
    }
    default:
    {
        puts("Unknown command! Type help to see available commands\n");
        return E_ERROR;
        break;
    }
    }

    return E_OK;
}
