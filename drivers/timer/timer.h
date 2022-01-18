#ifndef _SUNXI_TIMER_H_
#define _SUNXI_TIMER_H_

#include <types.h>

#define TIMER_0				0
#define TIMER_1				1

#define TIMER0_IRQ			50
#define TIMER1_IRQ			51

#define TIMER_AUTO_RELOAD       (0)
#define TIMER_SINGLESHOT        (1)
#define TIMER_DISABLED          (2)


int32_t TimerInit(uint32_t timer, uint32_t usec, uint32_t mode);

void TimerInterruptEnable(uint32_t timer);

void TimerInterruptAck(uint32_t timer);

void AvsTimerInit(void);

void __msdelay(uint32_t ms);

void __usdelay(uint32_t us);

#endif
