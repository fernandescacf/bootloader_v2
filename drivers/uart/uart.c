/**
 * @file        uart.c
 * @author      Carlos Fernandes
 * @version     1.0
 * @date        18 March, 2018
 * @brief       Sunxi Allwiner H3 Uart Driver file
*/



/* Includes ----------------------------------------------- */
#include <uart.h>
#include <gpio.h>
#include <misc.h>
#include <ccu.h>
#include <serial.h>

/* Private types ------------------------------------------ */



/* Private constants -------------------------------------- */

#define SUNXI_UART0_BASE	0x01C28000
#define SUNXI_UART1_BASE	0x01C28400
#define SUNXI_UART2_BASE	0x01C28800
#define SUNXI_UART3_BASE	0x01C28C00

// Pin configurations
#define SUN8I_H3_GPA_UART0	(2)
#define SUN8I_H3_GPA_UART1	(2)
#define SUN8I_H3_GPA_UART2	(2)
#define SUN8I_H3_GPA_UART3	(2)

/* bits in the lsr */
#define RX_READY	0x01
#define TX_READY	0x40
#define TX_EMPTY	0x80

/* bits in the ier */
#define IE_RDA		0x01	/* Rx data available */
#define IE_TXE		0x02	/* Tx register empty */
#define IE_LS		0x04	/* Line status */
#define IE_MS		0x08	/* Modem status */

#define LCR_DATA_5	0x00	/* 5 data bits */
#define LCR_DATA_6	0x01	/* 6 data bits */
#define LCR_DATA_7	0x02	/* 7 data bits */
#define LCR_DATA_8	0x03	/* 8 data bits */

#define LCR_STOP	0x04	/* extra (2) stop bits, else: 1 */
#define LCR_PEN		0x08	/* parity enable */

#define LCR_EVEN	0x10	/* even parity */
#define LCR_STICK	0x20	/* stick parity */
#define LCR_BREAK	0x40

#define LCR_DLAB	0x80	/* divisor latch access bit */

#define FCR_EFIFO	0x01	/* Enable in and out hardware FIFOs */
#define FCR_RRESET  0x02	/* Reset receiver FIFO */

/* Private macros ----------------------------------------- */



/* Private variables -------------------------------------- */

static h3_uart_t *h3Uarts[] =
{
    (h3_uart_t *)SUNXI_UART0_BASE,
    (h3_uart_t *)SUNXI_UART1_BASE,
    (h3_uart_t *)SUNXI_UART2_BASE,
    (h3_uart_t *)SUNXI_UART3_BASE
};

static struct
{
    uint16_t	TxPin;
    uint16_t	TxCfg;
    uint16_t	RxPin;
    uint16_t	RxCfg;
} UartPinConfig[] =
{
    { GPA(4),  SUN8I_H3_GPA_UART0, GPA(5),  SUN8I_H3_GPA_UART0 },
    { GPG(6),  SUN8I_H3_GPA_UART1, GPG(7),  SUN8I_H3_GPA_UART1 },
    { GPA(0),  SUN8I_H3_GPA_UART2, GPA(1),  SUN8I_H3_GPA_UART2 },
    { GPA(13), SUN8I_H3_GPA_UART3, GPA(14), SUN8I_H3_GPA_UART3 },
};

/* Private function prototypes ---------------------------- */

static inline void UartInitGPIO(uint32_t uartId)
{
    GpioSetCfgpin(UartPinConfig[uartId].TxPin, UartPinConfig[uartId].TxCfg);
    GpioSetCfgpin(UartPinConfig[uartId].RxPin, UartPinConfig[uartId].RxCfg);
    GpioSetPull(UartPinConfig[uartId].RxPin, GPIO_PULL_UP);
}

static inline void UartInitClock(uint32_t uartId)
{
    /* Open the clock gate for UART0 */
    ENABLE_UART0_CLOCK();
    /* Deassert UART0 reset (only needed on A31/A64/H3) */
    DEASSERT_UART0_CLK_GATING();
}


/* Private functions -------------------------------------- */

/**
 * UartInit Implementation (See header file for description)
*/
uint32_t UartInit(uint32_t uartId, uint32_t baudrate, uint32_t fifoConfig)
{
    volatile h3_uart_t *uart = h3Uarts[uartId];

    UartInitGPIO(uartId);

    UartInitClock(uartId);

    /* Disable uart interrupts*/
    uart->ier = 0;
    /* select dll dlh */
    uart->lcr = LCR_DLAB;
    /* set baudrate */
    uart->ier = 0;			// DLH
    uart->data = baudrate;	// LSB
    /* set line control */
    uart->lcr = fifoConfig;
    /* enable fifos */
    uart->iir = (FCR_EFIFO | FCR_RRESET);
    /* Interrupts configuration */
    // {...}

    return 0;
}

/**
 * UartGetc Implementation (See header file for description)
*/
char UartGetc(uint32_t uartId)
{
    h3_uart_t *uart = h3Uarts[uartId];

    while(!(uart->lsr & RX_READY)){}

    return (char)uart->data;
}

/**
 * UartPutc Implementation (See header file for description)
*/
void UartPutc(uint32_t uartId, char c)
{
    h3_uart_t *uart = h3Uarts[uartId];

    while (!(uart->lsr & TX_READY)) {}
    uart->data = c;
}

/**
 * uart_puts Implementation (See header file for description)
*/
void UartPuts(uint32_t uartId, const char *s)
{
    while (*s)
    {
        if (*s == '\n')
            UartPutc(uartId, '\r');
        UartPutc(uartId, *s++);
    }
}

int32_t putc(char c)
{
    UartPutc(UART0, c);
    return E_OK;
}

int32_t puts(const char *str)
{
    UartPuts(UART0, str);
    return E_OK;
}

char getc(void)
{
    return UartGetc(UART0);
}

char *gets(char *str)
{
    if(NULL == str)
    {
        return NULL;
    }

    uint32_t index = 0;
    char c;
    do
    {
        c = UartGetc(UART0);
        if(ASCII_BS == c)
        {
            if(index > 0)
            {
                (void)putc(ASCII_BS);
                (void)putc(ASCII_SP);
                (void)putc(ASCII_BS);
                index--;
            }
        }
        else if(ASCII_SP <= c && ASCII_DEL > c)
        {
            (void)putc(c);
            str[index] = c;
            index++;
        }
    }while('\n' != c && '\r' != c);

    str[index] = '\0';

    (void)putc('\n');
    (void)putc('\r');

    return str;
}
