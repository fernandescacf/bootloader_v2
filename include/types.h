/**
 * @file        types.h
 * @author      Carlos Fernandes
 * @version     1.0
 * @date        29 October, 2015
 * @brief       Types Definition Header File
*/

#ifndef _TYPES_H_
#define _TYPES_H_

#ifdef __cplusplus
    extern "C" {
#endif


/* Includes ----------------------------------------------- */


/* Exported types ----------------------------------------- */

/*< Data types >*/
typedef unsigned int		bool_t;
typedef signed char			int8_t;
typedef signed short		int16_t;
typedef signed int			int32_t;
typedef signed long			long_t;
typedef signed long long	int64_t;
typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int		uint32_t;
typedef unsigned long long	uint64_t;
typedef unsigned long		ulong_t;
typedef unsigned long       size_t;

typedef void*               ptr_t;
typedef void*               paddr_t;
typedef void*               vaddr_t;

/* Exported constants ------------------------------------- */

/*< Boolean >*/
#define FALSE        0
#define TRUE   		 1

/*< Null >*/
#define NULL  		 0

/*< Error Codes >*/
#define E_OK			0        // No error
#define E_INVAL			1        // Invalid argument
#define E_BUSY			2        // Busy
#define E_NO_INIT		3        // Not initialised
#define E_SRCH			4        // Cannot find specified parameter
#define E_NO_RES		5        // Not enought resources
#define E_FAULT			6        // Invalid pointer
#define E_AGAIN			7        // Try again
#define E_NO_MEMORY		8        // Not enough memory
#define E_ERROR			10       // Generic error


/* Exported macros ---------------------------------------- */


/* Exported functions ------------------------------------- */


#ifdef __cplusplus
    }
#endif

#endif /* _TYPES_H_ */
