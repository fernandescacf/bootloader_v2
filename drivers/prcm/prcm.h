/**
 * @file        prcm.h
 * @author      Carlos Fernandes
 * @version     1.0
 * @date        18 March, 2018
 * @brief       Sunxi Allwiner H3 Power, Reset & Clock Management Driver header file
*/

#ifndef _PRCM_H_
#define _PRCM_H_

#ifdef __cplusplus
    extern "C" {
#endif


/* Includes ----------------------------------------------- */
#include <types.h>


/* Exported types ----------------------------------------- */



/* Exported constants ------------------------------------- */
#define PRCM_BASE			(0x01f01400)		// Power, Reset & Clock Management
#define APB0_GATING_REG		(PRCM_BASE + 0x28)	// APB0 clock gating control
#define APB0_MODULE_RST_REG	(PRCM_BASE + 0xb0)	// APB0 module software reset control

#define R_PIO_GATING		(1 << 0)			// Gating APB clock for R_PIO
#define R_PIO_RST			(1 << 0)			// R_PIO reset control


/* Exported macros ---------------------------------------- */

// R_GPIO clock
#define DISABLE_RGPIO_CLK_GATING()  set_wbit(APB0_GATING_REG, 1 << 0);
#define DEASSERT_RGPIO_CLK_GATING() set_wbit(APB0_MODULE_RST_REG, 1 << 0);


/* Exported functions ------------------------------------- */



#ifdef __cplusplus
    }
#endif

#endif
