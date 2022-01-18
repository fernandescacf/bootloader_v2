#include <cpucfg.h>
#include <ccu.h>
#include <prcm.h>
#include <delay.h>

#define CPUCFG_BASE     0x01F01C00
#define POWER_OFF       ((uint32_t*) (PRCM_BASE + 0x100))

typedef struct 
{
/*0x00*/ uint32_t rst_ctrl;             // CPU0 reset control
/*0x04*/ uint32_t ctrl;                 // CPU0 control register
/*0x08*/ uint32_t status;               // CPU0 status register
/*0x0C*/ uint32_t padding[13];          // Padding
}cpu_ctrl_t;

typedef struct
{
/*0x000*/ uint32_t cpus_rst_ctrl;       // CPUS reset control register
/*0x004*/ uint32_t reserved_0[15];      // Resserved 0
/*0x040*/ cpu_ctrl_t cpu_ctrl[4];       // CPU0 -> CPU3 control registers
/*0x140*/ uint32_t cpu_sys_rst;         // CPU system reset register
/*0x144*/ uint32_t cpu_clk_gating;      // CPU clock gating register
/*0x148*/ uint32_t reserved_1[15];      // Resserved 1
/*0x184*/ uint32_t gener_ctrl;          // General control register
/*0x188*/ uint32_t reserved_2[6];       // Resserved 2
/*0x1A0*/ uint32_t sup_stan_flag;       // Super standby flag register
/*0x1A4*/ uint32_t start_addr;          // CPU start address
/*0x1A8*/ uint32_t reserved_3[54];      // Resserved 3
/*0x280*/ uint32_t cnt64_ctrl;          // 64-bit counter control register
/*0x284*/ uint32_t cnt64_low;           // 64-bit counter low register
/*0x288*/ uint32_t cnt64_high;          // 64-bit counter high register
}cpucfg_t;

int32_t StartCore(uint32_t core, void* startup)
{
    static cpucfg_t* cpucfg = (cpucfg_t*)CPUCFG_BASE;

    if(core < 1 || core > 3)
    {
        return E_INVAL;
    }

    // Set core start address
    cpucfg->start_addr = (uint32_t)startup;

    // Put core into reset
    cpucfg->cpu_ctrl[core].rst_ctrl = 0x00;

    // Reset core L1 cache
    cpucfg->gener_ctrl &= ~(1 << core);

    // Clear power-off gating
    cpucfg->cpu_clk_gating |= (1 << core);

    // Power on core
    *POWER_OFF &= ~(1 << core);

    // Delay 1000 us
    delay_us(1000);

    // Free core from reset
    cpucfg->cpu_ctrl[core].rst_ctrl = 0x03;

    return E_OK;
}