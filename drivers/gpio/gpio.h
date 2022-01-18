/**
 * @file        gpio.h
 * @author      Carlos Fernandes
 * @version     1.0
 * @date        18 March, 2018
 * @brief       Sunxi Allwiner H3 GPIO Driver header file
*/

#ifndef _GPIO_H_
#define _GPIO_H_

#ifdef __cplusplus
    extern "C" {
#endif


/* Includes ----------------------------------------------- */
#include <types.h>


/* Exported types ----------------------------------------- */



/* Exported constants ------------------------------------- */

#define GPIO_A    (0)
#define GPIO_B    (1)
#define GPIO_C    (2)
#define GPIO_D    (3)
#define GPIO_E    (4)
#define GPIO_F    (5)
#define GPIO_G    (6)
#define GPIO_H    (7)
#define GPIO_I    (8)
#define GPIO_L    (11)

#define GPIO_SIZE (32)

/* GPIO pin function config */
#define GPIO_INPUT        (0)
#define GPIO_OUTPUT       (1)
#define GPIO_DISABLE      (7)

/* GPIO pin pull-up/down config */
#define GPIO_PULL_DISABLE (0)
#define GPIO_PULL_UP      (1)
#define GPIO_PULL_DOWN    (2)

#define GPIO_VALUE_LOW	  (0)
#define GPIO_VALUE_HIGH	  (1)

/* Exported macros ---------------------------------------- */



/* SUNXI GPIO number definitions */
#define GPA(_nr)          ((GPIO_A * GPIO_SIZE) + (_nr))
#define GPB(_nr)          ((GPIO_B * GPIO_SIZE) + (_nr))
#define GPC(_nr)          ((GPIO_C * GPIO_SIZE) + (_nr))
#define GPD(_nr)          ((GPIO_D * GPIO_SIZE) + (_nr))
#define GPE(_nr)          ((GPIO_E * GPIO_SIZE) + (_nr))
#define GPF(_nr)          ((GPIO_F * GPIO_SIZE) + (_nr))
#define GPG(_nr)          ((GPIO_G * GPIO_SIZE) + (_nr))
#define GPH(_nr)          ((GPIO_H * GPIO_SIZE) + (_nr))
#define GPI(_nr)          ((GPIO_I * GPIO_SIZE) + (_nr))
#define GPL(_nr)		  ((GPIO_L * GPIO_SIZE) + (_nr))


/* Exported functions ------------------------------------- */

int32_t GpioInit(void);

/**
 * @brief	Initialize the select uart
 * @param	uartId - ID uart
 * 			baudrate - baudrate speed of the uart
 * 			fifoConfig - uart data fifo configuration
 * @retval	Purpose of return value
 */
int32_t GpioSetCfgpin(uint32_t pin, uint32_t val);


int32_t GpioSetDrv(uint32_t pin, uint32_t val);


int32_t GpioSetPull(uint32_t pin, uint32_t val);


int32_t GpioOutput(uint32_t pin, uint32_t val);


int32_t GpioInput(uint32_t pin);


int32_t GpioDirectionInput(unsigned gpio);


int32_t GpioDirectionOutput(unsigned gpio, int32_t value);


#ifdef __cplusplus
    }
#endif

#endif
