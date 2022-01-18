#include <types.h>
// Drivers
#include <gpio.h>
#include <uart.h>
#include <dram.h>
#include <ccu.h>
#include <pmu.h>
#include <mmc_bsp.h>
#include <mmc.h>
#include <fat32.h>

// bootLoader processes
#include <cmd.h>

#include <serial.h>
#include <misc.h>
#include <string.h>
#include <debug.h>

typedef struct
{
    uint8_t boot_indicator;         // Boot indicator; 0x80 = Active partition / 0x00 Inactive partition
    struct
    {
        uint8_t head;               // partition start: head
        uint8_t sect;               // partition start: sector
        uint8_t cyl;                // partition start: cylinder
    }chs_start;
    uint8_t system_indicator;       // Partition ID (example ID=1 for FAT12)
    struct
    {
        uint8_t head;               // partition end: head
        uint8_t sect;               // partition end: sector
        uint8_t cyl;                // partition end: cylinder
    }chs_end;
    uint32_t sectors_before;        // Number of sectors before the beginning of this partition
    uint32_t number_of_sectors;     // Number of sectos in this partition
} __attribute__ ((__packed__)) part_table_t;

#define SD                      (0)
#define PARTITION_TABLE_OFFSET  (0x01BE)

int32_t FileSystemInit(void)
{
    // For now!!!
    uint8_t* buffer = (uint8_t*)0x50000000;

    puts("Initialize SD Card...\n");
    if(sunxi_mmc_init(SD) < 0 || mmc_bread(SD, 0, 1, buffer) == 0)
    {
        puts("ERROR: Failed to init SD Card!");
        return E_ERROR;
    }
    
//    part_table_t* part_table = (part_table_t*)(&buffer[PARTITION_TABLE_OFFSET]);

    part_table_t part_table;

    DEBUG_DUMP_STR("DEBUG: Try copy...\n");
    memcpy(&part_table, &buffer[PARTITION_TABLE_OFFSET], 16);
    DEBUG_DUMP_HEX("Copy worked... 0x", part_table.sectors_before);
    DEBUG_DUMP_STR("\n");

    puts("Mount FAT32 filesystem...\n");
    if(Fat32Init(SD, part_table.sectors_before, buffer) != E_OK)
    {
        puts("ERROR: Failed to initialize Fat32 file system!");
        return E_ERROR;
    }

    puts("FAT32 filesystem mounted\n");

    return E_OK;
}

// This will be moved to another place since is board dependent
int32_t BoardInit(void)
{
    /* Initialize PMU */
    pmu_ini();

    /* Initialize GPIO */
    GpioInit();
    
    /* Set CPU clock speed */
    clock_set_pll1(1000000000);

    writel(PLL6_CFG_DEFAULT, PLL_PERIPH0_CTRL_REG);
    while (!(readl(PLL_PERIPH0_CTRL_REG) & CCM_PLL6_CTRL_LOCK))
        ;

    writel(AHB1_ABP1_DIV_DEFAULT, (AHB1_APB1_CFG_REG));

    // Configure pin GPA_15 as output
    GpioSetCfgpin(GPA(15) , GPIO_OUTPUT);
    // Configure pin GPL_10 as output
    GpioSetCfgpin(GPL(10) , GPIO_OUTPUT);

    // Set GPA_15 value as High Red
    GpioDirectionOutput(GPA(15), 1);
    // Set GPL_10 value as High Green
    //GpioDirectionOutput(GPL(10), 1);

    /* Initialize Uart 0*/
    UartInit(UART0, BAUD_115200, LC_8_N_1);
    /* Initialize Dram*/
    DramInit();

    return E_OK;
}

int32_t main(void)
{
    /* Initialize the Board Processor and Peripherals*/
    BoardInit();

    puts("\nBootLoader 1.0\n");

    /* Initialize FileSystem */
    if(FileSystemInit() != E_OK)
    {
        puts("No filesystem support will be enabled\n");
    }

    while(1)
    {
        (void)CmdInterpretCommand();
    }

    return E_OK;
}
