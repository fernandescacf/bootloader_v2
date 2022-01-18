/**
 * @file        mmc_bsp.c
 * @author      Carlos Fernandes
 * @version     1.0
 * @date        01 March, 2021
 * @brief       Sunxi MMC Driver file
*/



/* Includes ----------------------------------------------- */
#include <mmc.h>
#include <mmc_bsp.h>
#include <ccu.h>
#include <gpio.h>
#include <delay.h>
#include <string.h>
#include <misc.h>


/* Private types ------------------------------------------ */

struct sunxi_mmc_priv
{
    uint32_t  mmc_no;
    uint32_t* mclkreg;
    uint32_t  fatal_err;
    uint32_t  mod_clk;
    struct sunxi_mmc* reg;
    struct mmc_config cfg;
};

/* Private constants -------------------------------------- */

// GPIO Configurations
#define SUNXI_GPC_SDC2          3
#define SUNXI_GPF_SDC0          2

// MMC Configurations
#define SUNXI_MMC_STATUS_FIFO_EMPTY     (0x1 << 2)
#define SUNXI_MMC_STATUS_FIFO_FULL      (0x1 << 3)

#define SUNXI_MMC_CLK_ENABLE        (0x1 << 16)
#define SUNXI_MMC_CLK_DIVIDER_MASK  (0xff)

#define SUNXI_MMC_CMD_WAIT_PRE_OVER (0x1 << 13)
#define SUNXI_MMC_CMD_UPCLK_ONLY    (0x1 << 21)
#define SUNXI_MMC_CMD_START         (0x1 << 31)

#define SUNXI_MMC_GCTRL_SOFT_RESET	(0x1 << 0)
#define SUNXI_MMC_GCTRL_FIFO_RESET	(0x1 << 1)
#define SUNXI_MMC_GCTRL_DMA_RESET	(0x1 << 2)
#define SUNXI_MMC_GCTRL_RESET       (SUNXI_MMC_GCTRL_SOFT_RESET | \
                                    SUNXI_MMC_GCTRL_FIFO_RESET  | \
                                    SUNXI_MMC_GCTRL_DMA_RESET)
#define SUNXI_MMC_GCTRL_DMA_ENABLE      (0x1 << 5)
#define SUNXI_MMC_GCTRL_ACCESS_BY_AHB   (0x1 << 31)

#define SUNXI_MMC_RINT_RESP_ERROR           (0x1 << 1)
#define SUNXI_MMC_RINT_COMMAND_DONE         (0x1 << 2)
#define SUNXI_MMC_RINT_DATA_OVER            (0x1 << 3)
#define SUNXI_MMC_RINT_TX_DATA_REQUEST      (0x1 << 4)
#define SUNXI_MMC_RINT_RX_DATA_REQUEST      (0x1 << 5)
#define SUNXI_MMC_RINT_RESP_CRC_ERROR       (0x1 << 6)
#define SUNXI_MMC_RINT_DATA_CRC_ERROR       (0x1 << 7)
#define SUNXI_MMC_RINT_RESP_TIMEOUT         (0x1 << 8)
#define SUNXI_MMC_RINT_DATA_TIMEOUT         (0x1 << 9)
#define SUNXI_MMC_RINT_VOLTAGE_CHANGE_DONE  (0x1 << 10)
#define SUNXI_MMC_RINT_FIFO_RUN_ERROR       (0x1 << 11)
#define SUNXI_MMC_RINT_HARD_WARE_LOCKED     (0x1 << 12)
#define SUNXI_MMC_RINT_START_BIT_ERROR      (0x1 << 13)
#define SUNXI_MMC_RINT_AUTO_COMMAND_DONE    (0x1 << 14)
#define SUNXI_MMC_RINT_END_BIT_ERROR        (0x1 << 15)
#define SUNXI_MMC_RINT_SDIO_INTERRUPT       (0x1 << 16)
#define SUNXI_MMC_RINT_CARD_INSERT          (0x1 << 30)
#define SUNXI_MMC_RINT_CARD_REMOVE          (0x1 << 31)
#define SUNXI_MMC_RINT_INTERRUPT_ERROR_BIT      \
    (SUNXI_MMC_RINT_RESP_ERROR |                \
     SUNXI_MMC_RINT_RESP_CRC_ERROR |            \
     SUNXI_MMC_RINT_DATA_CRC_ERROR |            \
     SUNXI_MMC_RINT_RESP_TIMEOUT |              \
     SUNXI_MMC_RINT_DATA_TIMEOUT |              \
     SUNXI_MMC_RINT_VOLTAGE_CHANGE_DONE |       \
     SUNXI_MMC_RINT_FIFO_RUN_ERROR |            \
     SUNXI_MMC_RINT_HARD_WARE_LOCKED |          \
     SUNXI_MMC_RINT_START_BIT_ERROR |           \
     SUNXI_MMC_RINT_END_BIT_ERROR)              /* 0xbfc2 */
#define SUNXI_MMC_RINT_INTERRUPT_DONE_BIT       \
    (SUNXI_MMC_RINT_AUTO_COMMAND_DONE |         \
     SUNXI_MMC_RINT_DATA_OVER |                 \
     SUNXI_MMC_RINT_COMMAND_DONE |              \
     SUNXI_MMC_RINT_VOLTAGE_CHANGE_DONE)

#define SUNXI_MMC_CMD_RESP_EXPIRE           (0x1 << 6)
#define SUNXI_MMC_CMD_LONG_RESPONSE         (0x1 << 7)
#define SUNXI_MMC_CMD_CHK_RESPONSE_CRC      (0x1 << 8)
#define SUNXI_MMC_CMD_DATA_EXPIRE           (0x1 << 9)
#define SUNXI_MMC_CMD_WRITE                 (0x1 << 10)
#define SUNXI_MMC_CMD_AUTO_STOP             (0x1 << 12)
#define SUNXI_MMC_CMD_WAIT_PRE_OVER         (0x1 << 13)
#define SUNXI_MMC_CMD_SEND_INIT_SEQ         (0x1 << 15)
#define SUNXI_MMC_CMD_UPCLK_ONLY            (0x1 << 21)
#define SUNXI_MMC_CMD_START                 (0x1 << 31)

#define SUNXI_MMC_STATUS_RXWL_FLAG          (0x1 << 0)
#define SUNXI_MMC_STATUS_TXWL_FLAG          (0x1 << 1)
#define SUNXI_MMC_STATUS_FIFO_EMPTY         (0x1 << 2)
#define SUNXI_MMC_STATUS_FIFO_FULL          (0x1 << 3)
#define SUNXI_MMC_STATUS_CARD_PRESENT       (0x1 << 8)
#define SUNXI_MMC_STATUS_CARD_DATA_BUSY     (0x1 << 9)
#define SUNXI_MMC_STATUS_DATA_FSM_BUSY      (0x1 << 10)

/* Private macros ----------------------------------------- */


/* Private variables -------------------------------------- */

static struct mmc mmc_dev[MAX_MMC_NUM];
static struct sunxi_mmc_priv mmc_host[MAX_MMC_NUM];

/* Private function prototypes ---------------------------- */

static int32_t mmc_resource_init(int32_t sdc_no)
{
    struct sunxi_mmc_priv* priv = &mmc_host[sdc_no];

    switch (sdc_no)
    {
    case 0:
        priv->reg = (struct sunxi_mmc*)SDMMC0_BASE;
        priv->mclkreg = (uint32_t*)CCMU_MMC0_CLK_BASE;
        // CD Pin
        GpioSetPull(GPF(6), GPIO_PULL_UP);
        GpioSetCfgpin(GPF(6), GPIO_INPUT);
        break;
    case 2:
        priv->reg = (struct sunxi_mmc *)SDMMC2_BASE;
        priv->mclkreg = (uint32_t*)CCMU_MMC2_CLK_BASE;
        break;
    default:
        return -1;
    }

    priv->mmc_no = sdc_no;

    return E_OK;
}

static void mmc_pinmux_setup(int32_t sdc)
{
    uint32_t pin;

    switch (sdc)
    {
    case 0:
        /* SDC0: PF0-PF5 */
        for (pin = GPF(0); pin <= GPF(5); pin++)
        {
            GpioSetCfgpin(pin, SUNXI_GPF_SDC0);
            GpioSetPull(pin, GPIO_PULL_UP);
            GpioSetDrv(pin, 2);
        }
        break;
    case 2:
        /* SDC2: PC5-PC6, PC8-PC16 */
        for (pin = GPC(5); pin <= GPC(6); pin++)
        {
            GpioSetCfgpin(pin, SUNXI_GPC_SDC2);
            GpioSetPull(pin, GPIO_PULL_UP);
            GpioSetDrv(pin, 2);
        }

        for (pin = GPC(8); pin <= GPC(16); pin++)
        {
            GpioSetCfgpin(pin, SUNXI_GPC_SDC2);
            GpioSetPull(pin, GPIO_PULL_UP);
            GpioSetDrv(pin, 2);
        }
        break;
    default:
        break;
    }
}

static int mmc_set_mod_clk(int32_t sdc_no, uint32_t hz)
{
    struct sunxi_mmc_priv* priv = &mmc_host[sdc_no];
    uint32_t pll, pll_hz, div, n, oclk_dly, sclk_dly;

    if (hz <= 24000000)
    {
        pll = CCM_MMC_CTRL_OSCM24;
        pll_hz = 24000000;
    }
    else
    {
        pll = CCM_MMC_CTRL_PLL6;
        pll_hz = clock_get_pll6();
    }

    div = pll_hz / hz;
    if(pll_hz % hz) div++;

    n = 0;
    while (div > 16)
    {
        n++;
        div = (div + 1) / 2;
    }

    if (n > 3)
    {
        return -1;
    }

    // determine delays
    if (hz <= 400000)
    {
        oclk_dly = 0;
        sclk_dly = 0;
    }
    else if (hz <= 25000000)
    {
        oclk_dly = 0;
        sclk_dly = 5;
    }
    else if (hz <= 52000000)
    {
        oclk_dly = 3;
        sclk_dly = 4;
    }
    else
    {
        // hz > 52000000
        oclk_dly = 1;
        sclk_dly = 4;
    }

    uint32_t val = CCM_MMC_CTRL_OCLK_DLY(oclk_dly) | CCM_MMC_CTRL_SCLK_DLY(sclk_dly);

    writel(CCM_MMC_CTRL_ENABLE| pll | CCM_MMC_CTRL_N(n) | CCM_MMC_CTRL_M(div) | val, priv->mclkreg);

    return E_OK;
}

static int32_t mmc_clk_io_on(int32_t sdc_no)
{
    struct sunxi_mmc_priv* priv = &mmc_host[sdc_no];

    // Config gpio
    mmc_pinmux_setup(sdc_no);

    // Config ahb clock
    // #define AHB_GATE_OFFSET_MMC0     8
    // #define AHB_GATE_OFFSET_MMC(n)   (AHB_GATE_OFFSET_MMC0 + (n))
    set_wbit(CCMU_HCLKGATE0_BASE, (1 << (8 + sdc_no)));

    // Unassert reset
    // #define AHB_RESET_OFFSET_MMC0        8
    // #define AHB_RESET_OFFSET_MMC(n)      (AHB_RESET_OFFSET_MMC0 + (n))
    set_wbit(CCMU_HCLKRST0_BASE, (1 << (8 + sdc_no)));

    // Config mod clock
    priv->mod_clk = 24000000;
    return mmc_set_mod_clk(sdc_no, 24000000);
}

static int32_t mmc_update_clk(struct mmc* mmc)
{
    struct sunxi_mmc_priv* priv = (struct sunxi_mmc_priv*)mmc->priv;
    uint32_t cmd;
    uint32_t timeout = 0xfffff;

    cmd = SUNXI_MMC_CMD_START | SUNXI_MMC_CMD_UPCLK_ONLY | SUNXI_MMC_CMD_WAIT_PRE_OVER;
    writel(cmd, &priv->reg->cmd);

    while(readl(&priv->reg->cmd) & SUNXI_MMC_CMD_START)
    {
        if (--timeout == 0)
        {
            return -1;
        }
    }

    // Clock update sets various irq status bits, clear these
    writel(readl(&priv->reg->rint), &priv->reg->rint);

    return E_OK;
}

static int32_t mmc_config_clock(struct mmc* mmc, uint32_t clk)
{
    struct sunxi_mmc_priv* priv = (struct sunxi_mmc_priv*)mmc->priv;
    uint32_t rval = readl(&priv->reg->clkcr);

    // Disable Clock
    rval &= ~SUNXI_MMC_CLK_ENABLE;
    writel(rval, &priv->reg->clkcr);
    if (mmc_update_clk(mmc))
    {
        return -1;
    }

    // Set mod_clk to new rate
    if (mmc_set_mod_clk(priv->mmc_no, clk))
    {
        return -1;
    }

    // Clear internal divider
    rval &= ~SUNXI_MMC_CLK_DIVIDER_MASK;
    writel(rval, &priv->reg->clkcr);

    // Re-enable Clock
    rval |= SUNXI_MMC_CLK_ENABLE;
    writel(rval, &priv->reg->clkcr);
    if (mmc_update_clk(mmc))
    {
        return -1;
    }

    return E_OK;
}

static int32_t mmc_set_ios(struct mmc* mmc)
{
    struct sunxi_mmc_priv* priv = (struct sunxi_mmc_priv*)mmc->priv;

    // Change clock first
    if (mmc->clock && mmc_config_clock(mmc, mmc->clock))
    {
        priv->fatal_err = 1;
        return -E_INVAL;
    }

    // Change bus width
    if (mmc->bus_width == 8) writel(2, &priv->reg->width);
    else if (mmc->bus_width == 4) writel(1, &priv->reg->width);
    else writel(0, &priv->reg->width);

    return E_OK;
}

static int32_t mmc_core_init(struct mmc* mmc)
{
    struct sunxi_mmc_priv* priv = (struct sunxi_mmc_priv*)mmc->priv;

    // Reset controller
    writel(SUNXI_MMC_GCTRL_RESET, &priv->reg->gctrl);
    
    delay_us(1000);

    return E_OK;
}

static int32_t mmc_trans_data_by_cpu(struct mmc *mmc, struct mmc_data* data)
{
    const int32_t  reading = !!(data->flags & MMC_DATA_READ);
    const uint32_t status_bit = ((reading) ? (SUNXI_MMC_STATUS_FIFO_EMPTY) : (SUNXI_MMC_STATUS_FIFO_FULL));

    struct sunxi_mmc_priv* priv = (struct sunxi_mmc_priv*)mmc->priv;
    uint32_t  i;
    uint32_t  byte_cnt = data->blocksize * data->blocks;
    uint32_t* buff = (uint32_t*)(((reading) ? (data->dest) : (data->src)));
    uint32_t  timeout = 2000;

    // Always read / write data through the CPU
    set_wbit(&priv->reg->gctrl, SUNXI_MMC_GCTRL_ACCESS_BY_AHB);

    for (i = 0; i < (byte_cnt >> 2); i++)
    {
        // Wait for Data on the FIFO / Wait for FIFO empty
        while(readl(&priv->reg->status) & status_bit)
        {
            if(!timeout--)
            {
                return -1;
            }
            delay_us(1000);
        }

        if(reading)
        {
            buff[i] = readl(&priv->reg->fifo);
        }
        else
        {
            writel(buff[i], &priv->reg->fifo);
        }
    }

    return E_OK;
}

static int32_t mmc_rint_wait(struct mmc *mmc, uint32_t timeout_msecs, uint32_t done_bit, const char* what)
{
    struct sunxi_mmc_priv* priv = (struct sunxi_mmc_priv*)mmc->priv;
    uint32_t status;

    do
    {
        status = readl(&priv->reg->rint);
        if (!timeout_msecs-- || (status & SUNXI_MMC_RINT_INTERRUPT_ERROR_BIT))
        {   
            return TIMEOUT;
        }
        delay_us(1000);
    } while (!(status & done_bit));

    return 0;
}

static int32_t mmc_send_cmd(struct mmc* mmc, struct mmc_cmd* cmd, struct mmc_data* data)
{
    struct sunxi_mmc_priv* priv = (struct sunxi_mmc_priv*)mmc->priv;
    uint32_t cmdval = SUNXI_MMC_CMD_START;
    int32_t  timeout = 0;
    int32_t  error = 0;
    uint32_t status = 0;

    if (priv->fatal_err)
    {
        return -1;
    }
    if (cmd->resp_type & MMC_RSP_BUSY)
    {

    }
    if (cmd->cmdidx == 12)
    {
        return E_OK;
    }

    if (!cmd->cmdidx)
        cmdval |= SUNXI_MMC_CMD_SEND_INIT_SEQ;
    if (cmd->resp_type & MMC_RSP_PRESENT)
        cmdval |= SUNXI_MMC_CMD_RESP_EXPIRE;
    if (cmd->resp_type & MMC_RSP_136)
        cmdval |= SUNXI_MMC_CMD_LONG_RESPONSE;
    if (cmd->resp_type & MMC_RSP_CRC)
        cmdval |= SUNXI_MMC_CMD_CHK_RESPONSE_CRC;

    if (data)
    {
        if ((uint32_t)data->dest & 0x3)
        {
            error = -1;
            goto out;
        }

        cmdval |= SUNXI_MMC_CMD_DATA_EXPIRE | SUNXI_MMC_CMD_WAIT_PRE_OVER;
        if (data->flags & MMC_DATA_WRITE)
            cmdval |= SUNXI_MMC_CMD_WRITE;
        if (data->blocks > 1)
            cmdval |= SUNXI_MMC_CMD_AUTO_STOP;
        writel(data->blocksize, &priv->reg->blksz);
        writel(data->blocks * data->blocksize, &priv->reg->bytecnt);
    }

    writel(cmd->cmdarg, &priv->reg->arg);

    if (!data)
        writel(cmdval | cmd->cmdidx, &priv->reg->cmd);

    /*
     * transfer data and check status
     * STATREG[2] : FIFO empty
     * STATREG[3] : FIFO full
     */
    if (data)
    {
        int32_t ret = 0;

        writel(cmdval | cmd->cmdidx, &priv->reg->cmd);
        ret = mmc_trans_data_by_cpu(mmc, data);
        
        if (ret)
        {
            error = readl(&priv->reg->rint) & SUNXI_MMC_RINT_INTERRUPT_ERROR_BIT;
            goto out;
        }
    }

    error = mmc_rint_wait(mmc, 1000, SUNXI_MMC_RINT_COMMAND_DONE, "cmd");

    if (error)
        goto out;

    if (data)
    {
        error = mmc_rint_wait(mmc, 120, data->blocks > 1 ?
				      SUNXI_MMC_RINT_AUTO_COMMAND_DONE :
				      SUNXI_MMC_RINT_DATA_OVER,
				      "data");
		if (error)
			goto out;
    }

    if (cmd->resp_type & MMC_RSP_BUSY)
    {
        timeout = 2000;
        do
        {
            status = readl(&priv->reg->status);
            if (!timeout--)
            {
                error = -1;
                goto out;
            }
            delay_us(1000);
        } while (status & SUNXI_MMC_STATUS_CARD_DATA_BUSY);
    }
    if (cmd->resp_type & MMC_RSP_136)
    {
        cmd->response[0] = readl(&priv->reg->resp3);
        cmd->response[1] = readl(&priv->reg->resp2);
        cmd->response[2] = readl(&priv->reg->resp1);
        cmd->response[3] = readl(&priv->reg->resp0);
    }
    else
    {
        cmd->response[0] = readl(&priv->reg->resp0);
    }

out:
    if (error)
    {
        writel(SUNXI_MMC_GCTRL_RESET, &priv->reg->gctrl);
        mmc_update_clk(mmc);
    }

    writel(0xffffffff, &priv->reg->rint);
    writel(readl(&priv->reg->gctrl) | SUNXI_MMC_GCTRL_FIFO_RESET, &priv->reg->gctrl);

    return error;
}

/* Private functions -------------------------------------- */

int32_t sunxi_mmc_init(int32_t sdc_no)
{
    memset(&mmc_dev[sdc_no], 0, sizeof(struct mmc));
    memset(&mmc_host[sdc_no], 0, sizeof(struct sunxi_mmc_priv));

    struct mmc* mmc = &mmc_dev[sdc_no];
    struct mmc_config* cfg = &mmc_host[sdc_no].cfg;
    int32_t ret;

    cfg->name = "SUNXI SD/MMC";
    mmc->priv = &mmc_host[sdc_no];
    mmc->cfg = &mmc_host[sdc_no].cfg;
    mmc->send_cmd = mmc_send_cmd;
    mmc->set_ios = mmc_set_ios;
    mmc->init = mmc_core_init;

    cfg->voltages = MMC_VDD_32_33 | MMC_VDD_33_34;
    cfg->host_caps = MMC_MODE_4BIT | MMC_MODE_HS_52MHz | MMC_MODE_HS;
    cfg->b_max = CONFIG_SYS_MMC_MAX_BLK_COUNT;
	cfg->f_min = 400000;
	cfg->f_max = 52000000;

    if (mmc_resource_init(sdc_no))
    {
        return -1;
    }

    mmc_clk_io_on(sdc_no);
    ret = mmc_register(sdc_no, mmc);
    if (ret < 0)
        return -1;

    return mmc->lba;
}
