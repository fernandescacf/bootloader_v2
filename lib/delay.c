/**
 * @file        delay.c
 * @author      Carlos Fernandes
 * @version     1.0
 * @date        28 March, 2021
 * @brief       Delay functionalities implementation
*/



/* Includes ----------------------------------------------- */
#include <delay.h>
#include <pmu.h>


/* Private types ------------------------------------------ */



/* Private constants -------------------------------------- */



/* Private macros ----------------------------------------- */



/* Private variables -------------------------------------- */



/* Private function prototypes ---------------------------- */



/* Private functions -------------------------------------- */

void delay_us(uint32_t us)
{
    uint32_t us_cnt = 0;
    uint32_t cur = 0;
    uint32_t target = 0;

    us_cnt = pmu_us2ticks(us);
    cur = pmu_get_cyclecount();
    target = cur - pmu_overheadCall() + us_cnt;

    while(!(((int32_t)(cur) - (int32_t)(target) >= 0)))
    {
        cur = pmu_get_cyclecount();
    }
}
