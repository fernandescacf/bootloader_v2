/**
 * @file        serial.h
 * @author      Carlos Fernandes
 * @version     1.0
 * @date        13 July, 2018
 * @brief       Serial Definition Header File
*/

#ifndef SERIAL_H
#define SERIAL_H

#ifdef __cplusplus
    extern "C" {
#endif


/* Includes ----------------------------------------------- */
#include <types.h>

/* Exported types ----------------------------------------- */


/* Exported constants ------------------------------------- */

#define  ASCII_BS   0x08     /* Backspace */
#define  ASCII_SP   0x20     /* Space */
#define  ASCII_DEL  0x7F     /* Delete */

/* Exported macros ---------------------------------------- */


/* Exported functions ------------------------------------- */

int32_t putc(char c);

int32_t puts(const char *str);

char getc(void);

char *gets(char *str);


#ifdef __cplusplus
    }
#endif

#endif // SERIAL_H
