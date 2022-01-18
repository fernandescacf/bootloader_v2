/**
 * @file        misc.h
 * @author      Carlos Fernandes
 * @version     1.0
 * @date        18 March, 2018
 * @brief       Misc Definition Header File
*/

#ifndef _MISC_H_
#define _MISC_H_

#ifdef __cplusplus
    extern "C" {
#endif


/* Includes ----------------------------------------------- */
#include <types.h>

/* Exported types ----------------------------------------- */


/* Exported constants ------------------------------------- */


/* Exported macros ---------------------------------------- */

#define ROUND_UP(m,a)			(((m) + ((a) - 1)) & (~((a) - 1)))
#define ROUND_DOWN(m,a)			((m) & (~((a) - 1)))

#define set_wbit(addr, v)	(*((volatile ulong_t *)(addr)) |= (ulong_t)(v))
#define readl(addr)			(*((volatile ulong_t *)(addr)))
#define writel(v, addr)		(*((volatile ulong_t *)(addr)) = (ulong_t)(v))

#define dsb()				asm volatile("dsb")
#define dmb()				asm volatile("dmb")
#define isb()				asm volatile("isb")

/* Exported functions ------------------------------------- */

static inline void clrsetbits(volatile void  *mem, uint32_t clr, uint32_t set)
{
	uint32_t __val = readl(mem);
	__val &= ~clr;
	__val |= set;
	writel(__val, mem);
}

static inline void clrbits(volatile void *mem, uint32_t clr)
{
	uint32_t __val = readl(mem);
	__val &= ~clr;
	writel(__val, mem);
}

static inline void setbits(volatile void *mem, uint32_t set)
{
	uint32_t __val = readl(mem);
	__val |= set;
	writel(__val, mem);
}

#define min(x, y) ({				\
	typeof(x) _min1 = (x);			\
	typeof(y) _min2 = (y);			\
	(void) (&_min1 == &_min2);		\
	_min1 < _min2 ? _min1 : _min2; })

#define max(x, y) ({				\
	typeof(x) _max1 = (x);			\
	typeof(y) _max2 = (y);			\
	(void) (&_max1 == &_max2);		\
	_max1 > _max2 ? _max1 : _max2; })

#define clamp(val, lo, hi) min((typeof(val))max(val, lo), hi)

#define fls generic_fls

static inline int generic_fls(int x)
{
	int r = 32;

	if (!x)
		return 0;
	if (!(x & 0xffff0000u)) {
		x <<= 16;
		r -= 16;
	}
	if (!(x & 0xff000000u)) {
		x <<= 8;
		r -= 8;
	}
	if (!(x & 0xf0000000u)) {
		x <<= 4;
		r -= 4;
	}
	if (!(x & 0xc0000000u)) {
		x <<= 2;
		r -= 2;
	}
	if (!(x & 0x80000000u)) {
		x <<= 1;
		r -= 1;
	}
	return r;
}

#ifdef __cplusplus
    }
#endif

#endif /* _MISC_H_ */
