/**
 * @file        delay.h
 * @author      Carlos Fernandes
 * @version     1.0
 * @date        28 March, 2021
 * @brief       Delay functionalities Header File
*/

#ifndef _DELAY_H_
#define _DELAY_H_

#ifdef __cplusplus
    extern "C" {
#endif


/* Includes ----------------------------------------------- */
#include <types.h>

/* Exported types ----------------------------------------- */


/* Exported constants ------------------------------------- */


/* Exported macros ---------------------------------------- */


/* Exported functions ------------------------------------- */

void delay_us(uint32_t us);

#ifdef __cplusplus
    }
#endif

#endif /* _DELAY_H_ */