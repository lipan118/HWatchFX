#ifndef _SYSTEM_H
#define _SYSTEM_H

#include <stdint.h>
#include <stdio.h>

#define DEV_DEBUG 1
#if DEV_DEBUG
	#define Debug(__info,...) printf("Debug : " __info,##__VA_ARGS__)
#else
	#define DEBUG(__info,...)
#endif

typedef uint8_t UBYTE;
typedef uint16_t UWORD;
typedef uint32_t UDOUBLE;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define FLIPW(h) ((((uint16_t)h << 8)&0xFF00) | ((uint16_t)h >> 8))
#define HLIPW(h) ( (((uint32_t)h << 24)&0xFF000000) | (((uint32_t)h << 8)&0x00FF0000) | (((uint32_t)h >> 8)&0x0000FF00) | (((uint32_t)h >> 24)&0x000000FF) )
#define MERAGE2(h,l) ((((uint16_t)h) << 8) | l)
#define MERAGE4(one,two,three,four)	(((uint32_t)one << 24) | ((uint32_t)two << 16) | ((uint16_t)three << 8) | four)

extern uint16_t gSystickCount;

#endif
