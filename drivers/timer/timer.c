#include <misc.h>
#include <ccu.h>
#include <timer.h>

typedef struct {
    volatile uint32_t irqen;				// 0x00 - Timer IRQ Enable Register
    volatile uint32_t irqsta;				// 0x04 - Timer Status Register
    volatile uint32_t reserved[2];
    struct{
        volatile uint32_t ctrl;				// Timer Control Register
        volatile uint32_t intv;				// Timer Interval Value Register
        volatile uint32_t cur;				// Timer 0 Current Value Register
        volatile uint32_t reserved;
    }timer[2];								// Timer 0 & Timer 1
}h3_timer_t;

typedef struct
{
    volatile uint32_t ctrl;                 // 0x00 - AVS control register 
    volatile uint32_t cnt0;                 // 0x04 - AVS counter 0
    volatile uint32_t cnt1;                 // 0x08 - AVS counter 1
    volatile uint32_t div;                  // 0x0C - AVS clock divisor
}h3_avs_t;

// Timer defines
#define	CTRL_ENABLE			(0x1 << 0)
#define	CTRL_RELOAD			(0x1 << 1)
#define	CTRL_SRC_32K		(0x0 << 2)
#define	CTRL_SRC_24M		(0x1 << 2)
#define	CTRL_PRE_1			(0x0 << 4)
#define	CTRL_PRE_2			(0x1 << 4)
#define	CTRL_PRE_4			(0x2 << 4)
#define	CTRL_PRE_8			(0x3 << 4)
#define	CTRL_PRE_16			(0x4 << 4)
#define	CTRL_PRE_32			(0x5 << 4)
#define	CTRL_PRE_64			(0x6 << 4)
#define	CTRL_PRE_128		(0x7 << 4)
#define	CTRL_AUTO			(0x0 << 7)
#define	CTRL_SINGLE			(0x1 << 7)

// AVS defines
#define AVS_CNT0_EN         (1 << 0)
#define AVS_CNT1_EN         (1 << 1)
#define AVS_CNT0_D_SHIFT    (0)
#define AVS_CNT1_D_SHIFT    (16)
#define AVS_DIV_N_CNT0      (0x2EE0)        // 24Mhz / 2 / 12000 = 1KHz
#define AVS_DIV_N_CNT1      (0x000C)        // 24Mhz / 2 / 12 = 1MHz

#define CLOCK_24M				24000000
#define TIMER_HZ_VALUE(hz)		(CLOCK_24M/hz)
#define TIMER_USEC_VALUE(usec)	(24 * (usec))
#define TIMER_AUTO_MODE			(0)
#define TIMER_SINGLESHOT_MODE	(1)

#define TIMER_LOAD_VAL		0xffffffff

#define SUNXI_TIMER_BASE	( (h3_timer_t*) 0x01c20c00)
#define SUNXI_AVS_BASE      ( (h3_avs_t*) 0x01c20c80)


/*
 * Timer 0 & 1 Driver
*/

int32_t TimerInit(uint32_t timer, uint32_t usec, uint32_t mode)
{
    if(timer > TIMER_1)
    {
        return E_INVAL;
    }

    h3_timer_t* h3Timers = SUNXI_TIMER_BASE;

    if(mode >= TIMER_DISABLED)
    {
        h3Timers->timer[timer].ctrl = 0x0;

        return E_OK;
    }

    uint32_t loadValue = TIMER_USEC_VALUE(usec);

    h3Timers->timer[timer].intv = loadValue;
    h3Timers->timer[timer].ctrl = 0x0;

    // By default disable the interrupt
    h3Timers->irqen &= (~(1 << timer));

    if(TIMER_SINGLESHOT_MODE == mode)
    {
        h3Timers->timer[timer].ctrl |= CTRL_SINGLE;
    }

    h3Timers->timer[timer].ctrl |= CTRL_SRC_24M;
    h3Timers->timer[timer].ctrl |= CTRL_RELOAD;

    while (h3Timers->timer[timer].ctrl & CTRL_RELOAD){}

    h3Timers->timer[timer].ctrl |= CTRL_ENABLE;

    return E_OK;
}

void TimerInterruptEnable(uint32_t timer)
{
	SUNXI_TIMER_BASE->irqen |= (1 << timer);
}

void TimerInterruptAck(uint32_t timer)
{
	SUNXI_TIMER_BASE->irqsta |= (1 << timer);
}

/*
 * AVS Timer Driver
*/

void AvsTimerInit(void)
{
    // TODO: Change this!!! Or use a CCU driver or the bootloader should set the clock gate
    // setbits((volatile void *)(0x01C20144), (1 << 31));
    ENABLE_AVS_CLOCK();

    h3_avs_t *h3_avs = SUNXI_AVS_BASE;
        
    // Enable counter 0 and counter 1
    h3_avs->ctrl = (AVS_CNT0_EN | AVS_CNT1_EN);
    // Set clock divisors for counter 0 & 1
    h3_avs->div = ((AVS_DIV_N_CNT1 << AVS_CNT1_D_SHIFT) | (AVS_DIV_N_CNT0 << AVS_CNT0_D_SHIFT));
    // Reset counter values
    h3_avs->cnt0 = 0;
    h3_avs->cnt1 = 0; 
}

void __msdelay(uint32_t ms)
{
    h3_avs_t *h3_avs = SUNXI_AVS_BASE;

    uint32_t t1, t2;
    
    t1 = h3_avs->cnt0;
    t2 = t1 + ms;

    do
    {
        t1 = h3_avs->cnt0;
    } while (t2 >= t1);
}

void __usdelay(uint32_t us)
{
    
    h3_avs_t *h3_avs = SUNXI_AVS_BASE;
    
    uint32_t t1, t2;

    t1 =  h3_avs->cnt1;
    t2 = t1 + us;
    
    do
    {
        t1 =  h3_avs->cnt1;
    } while (t2 >= t1);
}
