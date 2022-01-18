/**
 * @file        parser.h
 * @author      Carlos Fernandes
 * @version     1.0
 * @date        13 July, 2018
 * @brief       Command line Parser Definition Header File
*/

#ifndef PARSER_H
#define PARSER_H

#ifdef __cplusplus
    extern "C" {
#endif


/* Includes ----------------------------------------------- */
#include <types.h>
#include <cmd.h>

/* Exported types ----------------------------------------- */


/* Exported constants ------------------------------------- */


/* Exported macros ---------------------------------------- */


/* Exported functions ------------------------------------- */

cmd_t CmdParserGetAction(char **str, int32_t *state);

CmdCommand_t CmdParserGetCommand(char **str, int32_t *state);

CmdAddr_t CmdParserGetAddress(char **str, int32_t *state);

CmdSize_t CmdParserGetSize(char **str, int32_t *state);

CmdData_t CmdParserGetData(char **str, int32_t *state);

void CmdParserGetStr(char **str, int32_t *state, char* dst);

cmdLoadType_t CmdParserGetLoadType(char **str, int32_t *state);

#ifdef __cplusplus
    }
#endif

#endif // PARSER_H
