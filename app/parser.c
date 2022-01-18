/**
 * @file        parser.c
 * @author      Carlos Fernandes
 * @version     1.0
 * @date        13 July, 2018
 * @brief       Command line implementation
*/

/* Includes ----------------------------------------------- */
#include <parser.h>
#include <helper.h>
#include <string.h>


/* Private types ------------------------------------------ */


/* Private constants -------------------------------------- */

#define MAXCOMMANDS     cmdInvalid
#define MAXLOADTYPES    2

/* Private macros ----------------------------------------- */

#define GETCMDSTRING(counter, src, dst)                             \
    do{                                                             \
        counter = 0;                                                \
        while(src[0][counter] != ' ' && src[0][counter] != '\0')    \
        {                                                           \
            dst[counter] = src[0][counter];                         \
            counter++;                                              \
        }                                                           \
        dst[counter] = '\0';                                        \
    }while(0)

/* Private variables -------------------------------------- */

static struct
{
    char    str[8];
    cmd_t   cmd;
}commandEntries[MAXCOMMANDS] =
{
    {"help",cmdHelp}, {"load",cmdLoad}, {"go",cmdGo},
    {"read",cmdRead}, {"write",cmdWrite}
};

static struct
{
    char            str[8];
    cmdLoadType_t   cmd;
}loadTypesEntries[MAXLOADTYPES] =
{
    {"serial",cmdSerial}, {"sd",cmdSd}
};


/* Private function prototypes ---------------------------- */

static cmd_t CommandEntriesLookup(char *str)
{
    int i;
    for (i = 0; i < MAXCOMMANDS; i++)
    {
        if (!strcmp(str,commandEntries[i].str))return commandEntries[i].cmd;
    }
    return (cmd_t)CMD_INVALID;
}

static cmdLoadType_t LoadTypesEntriesLookup(char *str)
{
    int i;
    for (i = 0; i < MAXLOADTYPES; i++)
    {
        if (!strcmp(str,loadTypesEntries[i].str))return loadTypesEntries[i].cmd;
    }
    return (cmdLoadType_t)CMD_INVALID;
}

uint32_t Str2Uint(char *str, char **end, int32_t *state){

    if('\0' == *str){
        *state = CMD_INVALID;
        return 0;
    }

    uint32_t value = (uint32_t)strtoul((const char*)str, end, 0);

    if(' ' != *end[0] && '\0' != *end[0]){
        *state = CMD_INVALID;
    }
    else {
        *state = CMD_VALID;
    }
    return value;
}

/* Private functions -------------------------------------- */

cmd_t CmdParserGetAction(char **str, int32_t *state)
{

    uint32_t index = 0;
    cmd_t cmd;
    char cmdStr[8];

    GETCMDSTRING(index, str, cmdStr);

    cmd = CommandEntriesLookup (cmdStr);

    if((cmd_t)CMD_INVALID != cmd)
    {
        *state = CMD_VALID;
        *str += index;
    }
    else{
        *state = CMD_INVALID;
    }
    return cmd;
}

CmdCommand_t CmdParserGetCommand(char **str, int32_t *state)
{
    return (CmdCommand_t)CmdParserGetAction(str, state);
}

CmdAddr_t CmdParserGetAddress(char **str, int32_t *state){

    char *end = NULL;
    CmdAddr_t addr = (CmdAddr_t)Str2Uint(*str, &end, state);

    if(CMD_VALID == *state)
    {
        *str = end;
    }

    return addr;
}

CmdSize_t CmdParserGetSize(char **str, int32_t *state)
{
    char *end = NULL;
    CmdSize_t size = (CmdSize_t)Str2Uint(*str, &end, state);

    if(CMD_VALID == *state)
    {
        *str = end;
    }

    return size;
}

void CmdParserGetStr(char **str, int32_t *state, char* dst)
{
    uint32_t len;

    GETCMDSTRING(len, str, dst);

    if(len == 0)
    {
        *state = CMD_INVALID;
    }
    else
    {
        *state = CMD_VALID;
        *str += len;
    }
}

CmdData_t CmdParserGetData(char **str, int32_t *state)
{
    char *end = NULL;
    CmdData_t data = (CmdData_t)Str2Uint(*str, &end, state);

    if(CMD_VALID == *state){
        *str = end;
    }

    return data;
}

cmdLoadType_t CmdParserGetLoadType(char **str, int32_t *state)
{
    uint32_t index = 0;
    cmdLoadType_t loadType;
    char cmdStr[8];

    GETCMDSTRING(index, str, cmdStr);

    loadType = LoadTypesEntriesLookup(cmdStr);

    if((cmdLoadType_t)CMD_INVALID != loadType)
    {
        *state = CMD_VALID;
        *str += index;
    }
    else
    {
        *state = CMD_INVALID;
    }
    return loadType;
}