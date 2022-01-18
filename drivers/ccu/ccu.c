/**
 * @file        ccu.c
 * @author      Carlos Fernandes
 * @version     1.0
 * @date        18 March, 2018
 * @brief       Sunxi Allwiner H3 clock Driver file
*/



/* Includes ----------------------------------------------- */
#include <ccu.h>
#include <clock_sun6i.h>
#include <delay.h>
#include <misc.h>


/* Private types ------------------------------------------ */



/* Private constants -------------------------------------- */



/* Private macros ----------------------------------------- */



/* Private variables -------------------------------------- */



/* Private function prototypes ---------------------------- */



/* Private functions -------------------------------------- */

uint32_t CpuFreq(void)
{
    uint32_t pll_cpux_ctrl = *(volatile uint32_t*)(CCU_BASE + 0x00);
    uint32_t cpux_axi_cfg  = *(volatile uint32_t*)(CCU_BASE + 0x50);
    
    if(cpux_axi_cfg == 0)
    {
        return 32;
    }

    if(cpux_axi_cfg == 1)
    {
        return 24000;
    }

    uint32_t factor_p = 0x3 & (pll_cpux_ctrl >> 16);
    factor_p = 1 << factor_p;                               // 1/2/4/8
    uint32_t factor_n = 0x1f & (pll_cpux_ctrl >> 8);        //the range is 0-31
    uint32_t factor_k = (0x3 & (pll_cpux_ctrl >> 4)) + 1;   //the range is 1-4
    uint32_t factor_m = (0x3 & (pll_cpux_ctrl >> 0)) + 1;   //the range is 1-4

    return ((24000 * factor_n * factor_k) / (factor_p * factor_m));
}

void clock_set_pll1(unsigned int clk)
{
    const int p = 0;
    int k = 1;
    int m = 1;

    if (clk > 1152000000) {
        k = 2;
    } else if (clk > 768000000) {
        k = 3;
        m = 2;
    }

    // Switch to 24MHz clock while changing PLL1
    writel(AXI_DIV_3 << AXI_DIV_SHIFT |
           ATB_DIV_2 << ATB_DIV_SHIFT |
           CPU_CLK_SRC_OSC24M << CPU_CLK_SRC_SHIFT,
           (CPUX_AXI_CFG_REG));

    // PLL1 rate = ((24000000 * n * k) >> p) / m
    writel(CCM_PLL1_CTRL_EN | CCM_PLL1_CTRL_P(p) |
           CCM_PLL1_CTRL_N(clk / (24000000 * k / m)) |
           CCM_PLL1_CTRL_K(k) | CCM_PLL1_CTRL_M(m), PLL_CPUX_CTRL_REG);

    volatile uint32_t i;
    for(i=0;i<200;i++);

    // Switch CPU to PLL1
    writel(AXI_DIV_3 << AXI_DIV_SHIFT |
           ATB_DIV_2 << ATB_DIV_SHIFT |
           CPU_CLK_SRC_PLL1 << CPU_CLK_SRC_SHIFT,
           (CPUX_AXI_CFG_REG));
}

void clock_set_pll5(unsigned int clk, bool_t sigma_delta_enable)
{
    struct sunxi_ccm_reg * const ccm =
        (struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
    const int max_n = 32;
    int k = 1, m = 2;

    clrsetbits(&ccm->pll5_tuning_cfg, CCM_PLL5_TUN_LOCK_TIME_MASK |
            CCM_PLL5_TUN_INIT_FREQ_MASK,
            CCM_PLL5_TUN_LOCK_TIME(2) | CCM_PLL5_TUN_INIT_FREQ(16));

    if (sigma_delta_enable)
        writel(CCM_PLL5_PATTERN, &ccm->pll5_pattern_cfg);

    /* PLL5 rate = 24000000 * n * k / m */
    if (clk > 24000000 * k * max_n / m) {
        m = 1;
        if (clk > 24000000 * k * max_n / m)
            k = 2;
    }
    writel(CCM_PLL5_CTRL_EN |
           (sigma_delta_enable ? CCM_PLL5_CTRL_SIGMA_DELTA_EN : 0) |
           CCM_PLL5_CTRL_UPD |
           CCM_PLL5_CTRL_N(clk / (24000000 * k / m)) |
           CCM_PLL5_CTRL_K(k) | CCM_PLL5_CTRL_M(m), &ccm->pll5_cfg);

    delay_us(5500);
}

uint32_t clock_get_pll6(void)
{
    uint32_t rval = readl(PLL_PERIPH0_CTRL_REG);

    #define CCM_PLL6_CTRL_N_SHIFT   8
    #define CCM_PLL6_CTRL_N_MASK    (0x1f << CCM_PLL6_CTRL_N_SHIFT)
    int n = ((rval & CCM_PLL6_CTRL_N_MASK) >> CCM_PLL6_CTRL_N_SHIFT) + 1;
    #define CCM_PLL6_CTRL_K_SHIFT   4
    #define CCM_PLL6_CTRL_K_MASK    (0x3 << CCM_PLL6_CTRL_K_SHIFT)
    int k = ((rval & CCM_PLL6_CTRL_K_MASK) >> CCM_PLL6_CTRL_K_SHIFT) + 1;

    return 24000000 * n * k / 2;
}