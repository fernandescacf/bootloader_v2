/**
 * @file        loader.c
 * @author      Carlos Fernandes
 * @version     1.0
 * @date        13 July, 2018
 * @brief       Loader implementation
*/

/* Includes ----------------------------------------------- */
#include <loader.h>
#include <misc.h>
#include <helper.h>
#include <serial.h>
#include <fat32.h>


/* Private types ------------------------------------------ */



/* Private constants -------------------------------------- */
#ifndef CPU_CORE_COUNT
#define CPU_CORE_COUNT 0x1
#endif


/* Private macros ----------------------------------------- */



/* Private variables -------------------------------------- */



/* Private function prototypes ---------------------------- */
#if (CPU_CORE_COUNT > 1)
    extern void _SetBoot(void* _boot_addr, void* _boot_arg0, void* _boot_arg1);
    extern void boot_sec(void);
    extern int32_t StartCore(uint32_t core, void* startup);
#endif

uint32_t GetWord(void)
{
    uint32_t shift = 0;
    unsigned char c = 0;
    uint32_t word = 0;

    while(shift < 32)
    {
        c = getc();
        word |= ((uint32_t)(c) << shift);
        shift += 8;
    }
    return word;
}


/* Private functions -------------------------------------- */

void LoaderGo(ptr_t addr, uint32_t core, uint32_t arg0, uint32_t arg1)
{
#if (CPU_CORE_COUNT == 1)
    void (*entry)(uint32_t, uint32_t);
    entry = (void*)addr;
    entry(arg0, arg1);
#else
    // Set boot parameters
    _SetBoot((void*)addr, (void*)arg0, (void*)arg1);

    // Check if we want to boot all cores
    if(core == (uint32_t)-1)
    {
        for(core = 1; core < CPU_CORE_COUNT; ++core)
        {
            StartCore(core, boot_sec);
        }

        // Trigger core 0 boot
        core = 0;
    }

    if(core == 0)
    {
        // Boot core 0 (current core)
        void (*entry)(uint32_t, uint32_t);
        entry = (void*)addr;
        entry(arg0, arg1);
    }

    // Boot specific core other then core 0
    StartCore(core, boot_sec);

#endif
}

int32_t LoaderWrite(ptr_t addr, uint32_t data)
{
    writel(data, addr);

    return E_OK;
}

int32_t LoaderRead(ptr_t addr, uint32_t *data)
{
    char buffer[11];
    *data = readl(addr);

    itoa((uint32_t)addr, buffer, 16);
    puts("@");
    puts(buffer);
    puts(": ");
    itoa(*data, buffer, 16);
    puts(buffer);
    puts("\n");

    return E_OK;
}

int32_t LoaderSerialLoad(ptr_t addr, uint32_t size)
{
    uint32_t *dst = (uint32_t *)addr;
    uint32_t count = 0;
    uint32_t per = (size >> 4);

    (void)putc('0');
    (void)putc('%');
    (void)putc('[');
    for(;count < 16; count++)
    {
        (void)putc(' ');
    }
    (void)putc(']');
    (void)putc('1');
    (void)putc('0');
    (void)putc('0');
    (void)putc('%');
    for(count=0; count < 21; count++)
    {
        (void)putc(ASCII_BS);
    }

    uint32_t inc = 0;
    count = 0;

    while(size > count)
    {
        *dst = GetWord();
        dst++;
        count += 4;
        inc += 4;

        if(per < inc )
        {
            inc = 0;
            (void)putc('=');
        }
    }
    (void)putc('=');
    (void)putc('\r');
    (void)putc('\n');

    return E_OK;
}

int32_t LoaderSdLoad(ptr_t addr, char* file)
{
    uint32_t size = Fat32ReadFile(file, addr, 0, (uint32_t)-1);
    if(size == 0)
    {
        puts("Failed to read file: ");
        puts(file);
        puts("\n");
        return E_ERROR;
    }
    else
    {
        char temp[11];
        puts("File: ");
        puts(file);
        puts(" loaded at 0x");
        puts(itoa((uint32_t)addr, temp, 16));
        puts(" with size: ");
        puts(itoa(size, temp, 10));
        puts("\n");
        return E_OK;
    }
}