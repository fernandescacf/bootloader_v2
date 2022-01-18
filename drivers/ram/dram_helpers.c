#include <misc.h>
#include <dram.h>
#include <misc.h>

#define CONFIG_SYS_SDRAM_BASE		0x40000000

/*
 * Wait up to 1s for value to be set in given part of reg.
 */
void mctl_await_completion(uint32_t *reg, uint32_t mask, uint32_t val)
{
	while ((readl(reg) & mask) != val) {}
}

/*
 * Test if memory at offset offset matches memory at begin of DRAM
 */
bool_t mctl_mem_matches(uint32_t offset)
{
	/* Try to write different values to RAM at two addresses */
	writel(0, CONFIG_SYS_SDRAM_BASE);
	writel(0xaa55aa55, (ulong_t)CONFIG_SYS_SDRAM_BASE + offset);
	dsb();
	/* Check if the same value is actually observed when reading back */
	return readl(CONFIG_SYS_SDRAM_BASE) ==
	       readl((ulong_t)CONFIG_SYS_SDRAM_BASE + offset);
}
