/**
 * @file        loader.h
 * @author      Carlos Fernandes
 * @version     1.0
 * @date        13 July, 2018
 * @brief       Loader Definition Header File
*/

#ifndef LOADER_H
#define LOADER_H

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


void LoaderGo(ptr_t addr, uint32_t core, uint32_t arg0, uint32_t arg1);

int32_t LoaderWrite(ptr_t addr, uint32_t data);

int32_t LoaderRead(ptr_t addr, uint32_t *data);

int32_t LoaderSerialLoad(ptr_t addr, uint32_t size);

int32_t LoaderSdLoad(ptr_t addr, char* file);

#ifdef __cplusplus
    }
#endif

#endif // LOADER_H
