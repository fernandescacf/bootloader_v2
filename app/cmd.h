/**
 * @file        cmd.h
 * @author      Carlos Fernandes
 * @version     1.0
 * @date        13 July, 2018
 * @brief       Command line Definition Header File
*/

#ifndef CMD_H
#define CMD_H

#ifdef __cplusplus
    extern "C" {
#endif


/* Includes ----------------------------------------------- */
#include <types.h>


/* Exported types ----------------------------------------- */
typedef enum
{
    cmdHelp,
    cmdLoad,
    cmdGo,
    cmdRead,
    cmdWrite,
    cmdInvalid,
}cmd_t;

typedef enum
{
    cmdSerial,
    cmdSd
}cmdLoadType_t;

typedef cmd_t       CmdCommand_t;
typedef ptr_t       CmdAddr_t;
typedef uint32_t    CmdSize_t;
typedef uint32_t    CmdData_t;

/* Exported constants ------------------------------------- */
#define CMD_VALID       (0)
#define CMD_INVALID     ((int32_t)(-1))


/* Exported macros ---------------------------------------- */


/* Exported functions ------------------------------------- */

int32_t CmdInterpretCommand(void);

#ifdef __cplusplus
    }
#endif

#endif // CMD_H
