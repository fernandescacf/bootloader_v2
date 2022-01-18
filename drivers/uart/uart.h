/**
 * @file        uart.h
 * @author      Carlos Fernandes
 * @version     1.0
 * @date        18 March, 2018
 * @brief       Sunxi Allwiner H3 Uart Driver header file
*/

#ifndef _UART_H_
#define _UART_H_

#ifdef __cplusplus
    extern "C" {
#endif


/* Includes ----------------------------------------------- */
#include <types.h>


/* Exported types ----------------------------------------- */

typedef struct
{
	volatile uint32_t data;	/* 00 - Rx/Tx data */
	volatile uint32_t ier;	/* 04 - interrupt enables */
	volatile uint32_t iir;	/* 08 - interrupt ID / FIFO control */
	volatile uint32_t lcr;	/* 0c - line control */
	volatile uint32_t mcr;	/* 10 - modem control */
	volatile uint32_t lsr;	/* 14 - line status */
	volatile uint32_t msr;	/* 18 - modem status */
}h3_uart_t;


/* Exported constants ------------------------------------- */

/* Uart Ids*/
#define UART0	(0)
#define UART1	(1)
#define UART2	(2)
#define UART3	(3)

/* User Macros */
#define BAUD_115200    (0xD) /* 24 * 1000 * 1000 / 16 / 115200 = 13 */
#define NO_PARITY      (0)
#define ONE_STOP_BIT   (0)
#define DAT_LEN_8_BITS (3)
#define LC_8_N_1       (NO_PARITY << 3 | ONE_STOP_BIT << 2 | DAT_LEN_8_BITS)

/* Exported macros ---------------------------------------- */


/* Exported functions ------------------------------------- */

/**
 * @brief	Initialize the select uart
 * @param	uartId - ID uart
 * 			baudrate - baudrate speed of the uart
 * 			fifoConfig - uart data fifo configuration
 * @retval	Purpose of return value
 */
uint32_t UartInit(uint32_t uartId, uint32_t baudrate, uint32_t fifoConfig);


/**
 * @brief    Get a character from the uart communication channel
 * @param    uart_id - ID uart
 * @retval   Return the character that has been received from the uart
 */
char UartGetc(uint32_t uartId);

/**
 * @brief    A simple public function example with 2 arguments and returning
 * @param    uart_id - ID uart
             c - Character to be sent by the uart
 * @retval   No return value
 */
void UartPutc(uint32_t uartId, char c);

/**
 * @brief    A simple public function example with 2 arguments and returning
 * @param    uart_id - ID uart
             s - String to be sent by the uart
 * @retval   No return value
 */
void UartPuts(uint32_t uartId, const char *s);

#ifdef __cplusplus
    }
#endif

#endif
