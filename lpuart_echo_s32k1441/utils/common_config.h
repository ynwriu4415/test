/****************************************************************************
* @file
* @author		yangmaolin
* @date			2019.09.03
* @version		1.3
* @brief		This file is used to be a common and necessary include file in other common module.
* @copyright	
****************************************************************************/

/****************************************************************************
* Version & History
* 
* 1.0
* First edition.
****************************************************************************/
#ifndef COMMON_CONFIG_H
#define COMMON_CONFIG_H
/**********************************INCLUDE**********************************/
#include <stdint.h>
#include <string.h>
#include "s32_core_cm4.h"
/***************************************************************************/

/**************************Preprocesser Conditions**************************/
/***************************************************************************/

/*****************************Macro Definitions*****************************/
#ifndef TRUE
#define TRUE	1U
#endif

#ifndef FALSE
#define FALSE	0U
#endif

#ifndef BOOL
#define BOOL	uint8_t
#endif

#ifndef NULL
#define NULL	((void *)0)
#endif

#define UTIL_U16_SPLIT_HIGH(data)						((uint8_t)(((uint16_t)(data)) >> 8U))
#define UTIL_U16_SPLIT_LOW(data)						((uint8_t)(data))

#define UTIL_U32_SPLIT_HIGH(data)						((uint8_t)(((uint32_t)(data)) >> 24U))
#define UTIL_U32_SPLIT_MHIGH(data)						((uint8_t)(((uint32_t)(data)) >> 16U))
#define UTIL_U32_SPLIT_MLOW(data)						((uint8_t)(((uint32_t)(data)) >> 8U))
#define UTIL_U32_SPLIT_LOW(data)						((uint8_t)(data))

#define UTIL_U32_COMBINE(l, ml, mh, h)					((uint32_t)(((uint8_t)(l)) | (((uint8_t)(ml)) << 8U) | (((uint8_t)(mh)) << 16U) | (((uint8_t)(h)) << 24U)))
#define UTIL_U16_COMBINE(l, h)							((uint16_t)(((uint8_t)(l)) | (((uint8_t)(h)) << 8U)))

/**<This macro is used to set bit, used in compile stage.*/
#define UTIL_BIT_SET(data, bit_idx)						((data) = ((uint32_t)(data)) | ((uint32_t)(1U << (bit_idx))))
/**<This macro is used to clear bit, used in compile stage.*/
#define UTIL_BIT_CLR(data, bit_idx)						((data) = ((uint32_t)(data)) & (~((uint32_t)(1U << (bit_idx)))))
/**<This macro is used to check if a bit is true, used in compile stage.*/
#define UTIL_BIT_IS_TRUE(data, bit_idx)					((((uint32_t)(data)) & ((uint32_t)(1U << (bit_idx)))) != 0U)
/**<This macro is used to check if a bit is false, used in compile stage.*/
#define UTIL_BIT_IS_FALSE(data, bit_idx)				((((uint32_t)(data)) & ((uint32_t)(1U << (bit_idx)))) == 0U)

/**<This macro is used to set bit, used in preprocesser(before compile).*/
#define UTIL_BIT_SET_PRE(data, bit_idx)					((data) = (data) | (1U << (bit_idx)))
/**<This macro is used to clear bit, used in preprocesser(before compile).*/
#define UTIL_BIT_CLR_PRE(data, bit_idx)					((data) = (data) & (~(1U << (bit_idx))))
/**<This macro is used to check if a bit is true, used in preprocesser(before compile).*/
#define UTIL_BIT_IS_TRUE_PRE(data, bit_idx)				(((data) & (1U << (bit_idx))) != 0U)
/**<This macro is used to check if a bit is false, used in preprocesser(before compile).*/
#define UTIL_BIT_IS_FALSE_PRE(data, bit_idx)			(((data) & (1U << (bit_idx))) == 0U)

#ifndef INT8_MIN
#define INT8_MIN										(-128)
#endif
#ifndef INT8_MAX
#define INT8_MAX										(127)
#endif
#ifndef INT16_MIN
#define INT16_MIN										(-32768)
#endif
#ifndef INT16_MAX
#define INT16_MAX										(32767)
#endif
#ifndef INT32_MIN
#define INT32_MIN										(~0x7fffffffl)
#endif
#ifndef INT32_MAX
#define INT32_MAX										(2147483647l)
#endif
#ifndef INT64_MIN
#define INT64_MIN										(~0x7fffffffffffffffLL)
#endif
#ifndef INT64_MAX
#define INT64_MAX										(9223372036854775807LL)
#endif
#ifndef UINT8_MIN
#define UINT8_MIN										(0U)
#endif
#ifndef UINT8_MAX
#define UINT8_MAX										(255U)
#endif
#ifndef UINT16_MIN
#define UINT16_MIN										(0U)
#endif
#ifndef UINT16_MAX
#define UINT16_MAX										(65535U)
#endif
#ifndef UINT32_MIN
#define UINT32_MIN										(0U)
#endif
#ifndef UINT32_MAX
#define UINT32_MAX										(4294967295U)
#endif
#ifndef UINT64_MIN
#define UINT64_MIN										(0U)
#endif
#ifndef UINT64_MAX
#define UINT64_MAX										(18446744073709551615LLU)
#endif

#ifndef STR
#define STR(s)   										#s				/**<Make string.*/
#endif
#ifndef UNUSED
#define UNUSED(a)										((void)(a))		/**<Unused a variable for avoid compiler warning.*/
#endif
/***************************************************************************/

/******************************Common Config********************************/
#define DEBUG_ENABLE									TRUE								/**<UserConfig: Enable or disable debug..*/
#if (DEBUG_ENABLE == TRUE)
#define DEBUG_ASSERT(condition)							if((condition) == FALSE) {}else{while(condition);}		/**<UserConfig: Enable or disable assert.*/
#else
#define DEBUG_ASSERT(condition)							/**<UserConfig: Enable or disable assert.*/
#endif
#define CONFIG_ENTER_CRITICAL()							DISABLE_INTERRUPTS()				/**<UserConfig: Critical enter function.*/
#define CONFIG_EXIT_CRITICAL()							ENABLE_INTERRUPTS()					/**<UserConfig: Critical exit function.*/
#define UTIL_REV_32(data)								__builtin_bswap32((uint32_t)(data))	/**<UserConfig: Reverse word(32-bit).*/
#define __WEAK_DEFINE									__attribute__((weak))				/**<UserConfig: Define weak define.*/
#define __ALIGNMENT(n) 									_Pragma(STR(pack(push, n)))			/**<UserConfig: Define alignment.*/
#define __ALIGNMENT_POP() 								_Pragma(STR(pack(pop)))				/**<UserConfig: Define alignment pop.*/
/***************************************************************************/

/****************************User Owner Config******************************/
#define DEFINE_CONFIG_RUN_TIME_ENVIROMENT_BOOT			0U
#define DEFINE_CONFIG_RUN_TIME_ENVIROMENT_APP			1U
#define CONFIG_RUN_TIME_ENVIROMENT						DEFINE_CONFIG_RUN_TIME_ENVIROMENT_APP
/***************************************************************************/

/******************************Type Definitions*****************************/
typedef float 	fp32_t;										/**<UserConfig: Define 32-bit float point data type.*/
typedef double 	fp64_t;										/**<UserConfig: Define 64-bit float point data type.*/
#if 0	/*If do not include the file:stdint.h, this preprocessor must be opened.*/
typedef unsigned char 		uint8_t;
typedef unsigned short	int uint16_t;
typedef unsigned long		uint32_t;
typedef unsigned long long	uint64_t;

typedef char 				int8_t;
typedef short int			int16_t;
typedef long				int32_t;
typedef long long			int64_t;
#endif
/***************************************************************************/

/***************************Variable Declarations***************************/
/***************************************************************************/

/****************************Function Declarations**************************/
#if (DEBUG_ENABLE == TRUE)
#include "shell.h"
//#include "defs.h"
#define myprintf(...)		color_printk(green, __VA_ARGS__)
#else
#define myprintf(fmt, ...)
#endif
/***************************************************************************/

#ifndef DEBUG_ENABLE
#error User must define DEBUG_ENABLE in common_config.h.
#else
#if ((DEBUG_ENABLE != TRUE) && (DEBUG_ENABLE != FALSE))
#error DEBUG_ENABLE must be equal to TRUE or FALSE  in common_config.h.
#endif
#endif

#endif
