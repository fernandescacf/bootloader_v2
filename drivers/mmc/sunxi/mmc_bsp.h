/**
 * @file        mmc_bsp.h
 * @author      Carlos Fernandes
 * @version     1.0
 * @date        01 March, 2021
 * @brief       Sunxi MMC Driver header file
*/

#ifndef _MMC_BSP_H_
#define _MMC_BSP_H_

#ifdef __cplusplus
    extern "C" {
#endif


/* Includes ----------------------------------------------- */
#include <types.h>


/* Exported types ----------------------------------------- */

struct sunxi_mmc
{
    uint32_t gctrl;         /* (0x00) SMC Global Control Register */
    uint32_t clkcr;         /* (0x04) SMC Clock Control Register */
    uint32_t timeout;       /* (0x08) SMC Time Out Register */
    uint32_t width;         /* (0x0C) SMC Bus Width Register */
    uint32_t blksz;         /* (0x10) SMC Block Size Register */
    uint32_t bytecnt;       /* (0x14) SMC Byte Count Register */
    uint32_t cmd;           /* (0x18) SMC Command Register */
    uint32_t arg;           /* (0x1C) SMC Argument Register */
    uint32_t resp0;         /* (0x20) SMC Response Register 0 */
    uint32_t resp1;         /* (0x24) SMC Response Register 1 */
    uint32_t resp2;         /* (0x28) SMC Response Register 2 */
    uint32_t resp3;         /* (0x2C) SMC Response Register 3 */
    uint32_t imask;         /* (0x30) SMC Interrupt Mask Register */
    uint32_t mint;          /* (0x34) SMC Masked Interrupt Status Register */
    uint32_t rint;          /* (0x38) SMC Raw Interrupt Status Register */
    uint32_t status;        /* (0x3C) SMC Status Register */
    uint32_t ftrglevel;     /* (0x40) SMC FIFO Threshold Watermark Register */
    uint32_t funcsel;       /* (0x44) SMC Function Select Register */
    uint32_t cbcr;          /* (0x48) SMC CIU Byte Count Register */
    uint32_t bbcr;          /* (0x4C) SMC BIU Byte Count Register */
    uint32_t dbgc;          /* (0x50) SMC Debug Enable Register */
    uint32_t res0[9];       /* (0x54~0x74) */
    uint32_t hwrst;         /* (0x78) SMC eMMC Hardware Reset Register */
    uint32_t res1;          /* (0x7c) */
    uint32_t dmac;          /* (0x80) SMC IDMAC Control Register */
    uint32_t dlba;          /* (0x84) SMC IDMAC Descriptor List Base Address Register */
    uint32_t idst;          /* (0x88) SMC IDMAC Status Register */
    uint32_t idie;          /* (0x8C) SMC IDMAC Interrupt Enable Register */
    uint32_t chda;          /* (0x90) */
    uint32_t cbda;          /* (0x94) */
    uint32_t res2[90];      /* (0x98~0x1ff) */
    uint32_t fifo;          /* (0x200) SMC FIFO Access Address */
};

/* Exported constants ------------------------------------- */

#define MAX_MMC_NUM         3
#define SDMMC_BASE          0x01C0F000
#define SDMMC0_BASE         0x01C0F000
#define SDMMC1_BASE         0x01C10000
#define SDMMC2_BASE         0x01C11000

/* Exported macros ---------------------------------------- */


/* Exported functions ------------------------------------- */

int32_t sunxi_mmc_init(int32_t sdc_no);

#ifdef __cplusplus
    }
#endif

#endif
