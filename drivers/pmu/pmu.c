/**
 * @file        pmu.c
 * @author      Carlos Fernandes
 * @version     1.0
 * @date        14 February, 2021
 * @brief       ARMv7-A Performance Monitor Unit Functions
*/

/* Includes ----------------------------------------------- */
#include <pmu.h>
#include <ccu.h>
#include <misc.h>


/* Private constants -------------------------------------- */


/* Private types ------------------------------------------ */


/* Private macros ----------------------------------------- */



/* Private variables -------------------------------------- */
static uint32_t overhead;
static uint32_t cpuFrep;


/* Private function prototypes ---------------------------- */



/* Private functions -------------------------------------- */

void pmu_ini(void)
{
    // Initialize PMU counters
    pmu_int_perfcounters(1, 0);
    // Measure the counting overhead:
    uint32_t start = pmu_get_cyclecount();
    overhead = pmu_get_cyclecount() - start;
    // Get Cpu frequency
    cpuFrep = CpuFreq();
}

uint32_t pmu_ticks2us(uint32_t ticks)
{
    return ( (ticks - overhead) / ((cpuFrep / 1000) + 1) );
}

uint32_t pmu_us2ticks(uint32_t us)
{
    return ((cpuFrep / 1000) + 1) * us;
}

uint32_t pmu_overheadCall()
{
    return overhead;
}

