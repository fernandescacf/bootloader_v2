/**
 * @file        ccu.h
 * @author      Carlos Fernandes
 * @version     1.0
 * @date        18 March, 2018
 * @brief       Sunxi Allwiner H3 clock Driver header file
*/

#ifndef _CLOCK_H_
#define _CLOCK_H_

#ifdef __cplusplus
    extern "C" {
#endif


/* Includes ----------------------------------------------- */
#include <types.h>
#include <clock_sun6i.h>


/* Exported types ----------------------------------------- */



/* Exported constants ------------------------------------- */

#define CCU_BASE			    (0x01C20000)		// Clock Control Unit
#define SUNXI_CCM_BASE		    (0x01C20000)

#define PLL_CPUX_CTRL_REG       (CCU_BASE + 0x000)  // PLL_CPUX Control Register
#define PLL_PERIPH0_CTRL_REG    (CCU_BASE + 0x028)  // PLL_PERIPH0 (PLL6) control Register
#define CPUX_AXI_CFG_REG        (CCU_BASE + 0x050)  // CPUX/AXI Configuration Register
#define AHB1_APB1_CFG_REG       (CCU_BASE + 0x054)  // AHB1/APB1 Configuration Register
#define APB2_CFG_REG            (CCU_BASE + 0x058)  // APB2_CFG_REG
#define BUS_CLK_GATING_REG0     (CCU_BASE + 0x060)  // BUS Clock Gating 0
#define BUS_CLK_GATING_REG1     (CCU_BASE + 0x064)  // BUS Clock Gating 1
#define BUS_CLK_GATING_REG2     (CCU_BASE + 0x068)  // BUS Clock Gating 2
#define BUS_CLK_GATING_REG3     (CCU_BASE + 0x06C)  // BUS Clock Gating 3
#define BUS_CLK_GATING_REG4     (CCU_BASE + 0x070)  // BUS Clock Gating 4
#define SDMMC0_CLK_REG          (CCU_BASE + 0x088)  // SD/MMC0 Clock Register
#define SDMMC1_CLK_REG          (CCU_BASE + 0x08C)  // SD/MMC1 Clock Register
#define SDMMC2_CLK_REG          (CCU_BASE + 0x090)  // SD/MMC2 Clock Register
#define MBUS_RESET_REG          (CCU_BASE + 0x0FC)  // MBUS reset register
#define AVS_CLK_REG             (CCU_BASE + 0x144)  // AVS clock
#define BUS_SOFT_RST_REG0       (CCU_BASE + 0x2C0)  // BUS Software reset register 0
#define BUS_SOFT_RST_REG1       (CCU_BASE + 0x2C4)  // BUS Software reset register 1
#define BUS_SOFT_RST_REG2       (CCU_BASE + 0x2C8)  // BUS Software reset register 2
#define BUS_SOFT_RST_REG3       (CCU_BASE + 0x2D0)  // BUS Software reset register 3
#define BUS_SOFT_RST_REG4       (CCU_BASE + 0x2D8)  // BUS Software reset register 4

// SD/MMC
#define CCMU_HCLKGATE0_BASE     (CCU_BASE + 0x060)
#define CCMU_HCLKRST0_BASE      (CCU_BASE + 0x2c0)
#define CCMU_MMC0_CLK_BASE      (CCU_BASE + 0x088)
#define CCMU_MMC2_CLK_BASE      (CCU_BASE + 0x090)
#define CCMU_PLL5_CLK_BASE      (CCU_BASE + 0x090)
#define CCMU_PLL6_CLK_BASE      (CCU_BASE + 0x028)

#define BUS_UART0_GATING        (16)                // Gating Clock For UART0
#define BUS_UART0_RST           (16)                // UART0 Reset
#define BUS_PIO_GATING          (5)                 // Gating Clock For PIO

#define CCM_MMC_CTRL_M(x)           ((x) - 1)
#define CCM_MMC_CTRL_OCLK_DLY(x)    ((x) << 8)
#define CCM_MMC_CTRL_N(x)           ((x) << 16)
#define CCM_MMC_CTRL_SCLK_DLY(x)    ((x) << 20)
#define CCM_MMC_CTRL_OSCM24         (0x0 << 24)
#define CCM_MMC_CTRL_PLL6           (0x1 << 24)
#define CCM_MMC_CTRL_ENABLE         (0x1 << 31)

/* Exported macros ---------------------------------------- */

// Uart clocks
#define ENABLE_UART0_CLOCK()        set_wbit(BUS_CLK_GATING_REG3, 1 << 16)
#define ENABLE_UART1_CLOCK()        set_wbit(BUS_CLK_GATING_REG3, 1 << 17)
#define ENABLE_UART2_CLOCK()        set_wbit(BUS_CLK_GATING_REG3, 1 << 18)
#define ENABLE_UART3_CLOCK()        set_wbit(BUS_CLK_GATING_REG3, 1 << 19)
#define DISABLE_UART0_CLOCK()       clear_wbit(BUS_CLK_GATING_REG3, 1 << 16)
#define DISABLE_UART1_CLOCK()       clear_wbit(BUS_CLK_GATING_REG3, 1 << 17)
#define DISABLE_UART2_CLOCK()       clear_wbit(BUS_CLK_GATING_REG3, 1 << 18)
#define DISABLE_UART3_CLOCK()       clear_wbit(BUS_CLK_GATING_REG3, 1 << 19)
#define DEASSERT_UART0_CLK_GATING() set_wbit(BUS_SOFT_RST_REG4, 1 << 16)
#define DEASSERT_UART1_CLK_GATING() set_wbit(BUS_SOFT_RST_REG4, 1 << 17)
#define DEASSERT_UART2_CLK_GATING() set_wbit(BUS_SOFT_RST_REG4, 1 << 18)
#define DEASSERT_UART3_CLK_GATING() set_wbit(BUS_SOFT_RST_REG4, 1 << 19)


// TWI clocks
#define ENABLE_TWI0_CLOCK()         set_wbit(BUS_CLK_GATING_REG3, 1 << 2)
#define ENABLE_TWI1_CLOCK()         set_wbit(BUS_CLK_GATING_REG3, 1 << 1)
#define ENABLE_TWI2_CLOCK()         set_wbit(BUS_CLK_GATING_REG3, 1 << 0)
#define DISABLE_TWI0_CLOCK()        clear_wbit(BUS_CLK_GATING_REG3, 1 << 2)
#define DISABLE_TWI1_CLOCK()        clear_wbit(BUS_CLK_GATING_REG3, 1 << 1)
#define DISABLE_TWI2_CLOCK()        clear_wbit(BUS_CLK_GATING_REG3, 1 << 0)
#define DEASSERT_TWI0_CLK_GATING()  set_wbit(BUS_SOFT_RST_REG4, 1 << 2)
#define DEASSERT_TWI1_CLK_GATING()  set_wbit(BUS_SOFT_RST_REG4, 1 << 1)
#define DEASSERT_TWI2_CLK_GATING()  set_wbit(BUS_SOFT_RST_REG4, 1 << 0)

// AVS Timer clock
#define ENABLE_AVS_CLOCK()     set_wbit(AVS_CLK_REG, 1 << 31)
#define DISABLE_AVS_CLOCK()    clear_wbit(AVS_CLK_REG, 1 << 31)

// GPIO clock
#define ENABLE_GPIO_CLOCK()    set_wbit(BUS_CLK_GATING_REG2, 1 << 5)
#define DISABLE_GPIO_CLOCK()   clear_wbit(BUS_CLK_GATING_REG2, 1 << 5)

// Reset Mbus
#define RESET_MBUS()           clear_wbit(MBUS_RESET_REG, 1 << 31)

/* Exported functions ------------------------------------- */

uint32_t CpuFreq(void);

void clock_set_pll1(unsigned int clk);

void clock_set_pll5(unsigned int clk, bool_t sigma_delta_enable);

uint32_t clock_get_pll6(void);


#ifdef __cplusplus
    }
#endif

#endif
