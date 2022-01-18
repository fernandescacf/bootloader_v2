#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <types.h>
#include <helper.h>
#include <serial.h>

//#define DEBUG

#ifdef DEBUG
    #define DEBUG_DUMP_STR(s)       puts(s)
    #define DEBUG_DUMP_HEX(s,v)     {char _d_str_[10]; DEBUG_DUMP_STR(s); DEBUG_DUMP_STR(itoa((int32_t)v, _d_str_, 16));}
    #define DEBUG_DUMP_DEC(s,v)     {char _d_str_[10]; DEBUG_DUMP_STR(s); DEBUG_DUMP_STR(itoa((int32_t)v, _d_str_, 10));}
#else
    #define DEBUG_DUMP_STR(s)
    #define DEBUG_DUMP_HEX(s,v)
    #define DEBUG_DUMP_DEC(s,v)
#endif

#endif