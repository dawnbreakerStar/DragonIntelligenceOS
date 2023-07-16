/*-----------------------------------------------------------------------------------------
**
** 版权(Copyright): Inory, 2023~
**
** 文件名(Filename): global.h
**
** 描述(Description): 本文件定义基本信息,便于在不同机器上进行代码移植。定义常用的特殊宏,基本数据类型以及数值范围
**
** 设计注记(Design Annotation):
**
** 作者(Author):
**		Inory , 2023年6月26日开始编写本文件
**
** 更改历史(Modification History):
**		Inory , 2023年6月26日 创建本文件
**		$Revision$
**		$Date$
**		$Author$
**		$Log$
**
**-----------------------------------------------------------------------------------------
*/
#ifndef GLOBAL_H
#define GLOBAL_H

/*-----------------------------------------------------------------------------------------
**									   Macro Definition
**-----------------------------------------------------------------------------------------
*/
/* 操作系统信息宏定义 */
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#define WINDOWS
#else
#define LINUX
#endif

/* 调试信息编译控制宏 */
#define DEBUG

/* DLL导入导出定义 */
#ifdef WINDOWS
#ifdef DLL_EXPORT
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimort)
#endif
#else
#include <QtCore/qglobal.h>
#ifdef DLL_EXPORT
#define DLL_API Q_DECL_EXPORT
#else
#define DLL_API Q_DECL_IMPORT
#endif
#endif

/* 编译器控制选项 */
#ifdef WINDOWS
#pragma warning (disable:4290)	/* 忽略C++异常规范警告 */
#pragma warning (disable:4275)	/* 忽略非dll接口用作dll接口的基类警告 */
#define _CRT_SECURE_NO_WARNINGS	/* 忽略函数安全警告 */
#endif

/*-----------------------------------------------------------------------------------------
**										   Include
**-----------------------------------------------------------------------------------------
*/
#include <functional>
#include <iostream>

/*-----------------------------------------------------------------------------------------
**									   Type Definition
**-----------------------------------------------------------------------------------------
*/
/* 基本数据类型及布尔值定义 */
typedef char			   I8;
typedef unsigned char	   U8;
typedef short			   I16;
typedef unsigned short	   U16;
typedef int				   I32;
typedef unsigned int	   U32;
typedef long long		   I64;
typedef unsigned long long U64;
typedef float			   F32;
typedef double			   F64;

/*-----------------------------------------------------------------------------------------
**									   Macro Definition
**-----------------------------------------------------------------------------------------
*/
/* 基本数据类型数值范围 */
#define I8_MIN		(-127-1)
#define I16_MIN		(-32767-1)
#define I32_MIN		(-2147483647-1)
#define I64_MIN		(-I64(9223372036854775807)-1)

#define I8_MAX		(127)
#define I16_MAX		(32767)
#define I32_MAX		(2147483647)
#define I64_MAX		(I64(9223372036854775807))

#define U8_MAX		(255)
#define U16_MAX		(65535)
#define U32_MAX		(4294967295U)
#define U64_MAX		(U64(18446744073709551615))

/* 特殊宏定义 */
#define FD_INVALID	(-1)

/* 宏定义 */
#define UNUSED(x)		(void)x												/* 消除未使用变量引起的警告 */
#define PASS																/* 消除未操作引起的警告 */
#define LOOP(x, len)	(x >= len ? (x - len) : x)							/* 游标循环操作 */
#define FREE_OBJECT(x)	if (NULL != x) {delete x; x = NULL;} else {PASS}	/* 释放对象内存 */
#define FREE_ARRAY(x)	if (NULL != x) {delete[] x; x = NULL;} else {PASS}	/* 释放数组内存 */

/* 大小端转换 */
/* Swap 2 bytes, 16 bit values */
#define SWAP2BYTES(val)		((((val) >> 8) & 0x00FF) | (((val) << 8) & 0xFF00))
/* Swap 4 bytes, 32 bit values */
#define SWAP4BYTES(val)		((((val) >> 24) & 0x000000FF) | (((val) >> 8) & 0x0000FF00) | (((val) << 8) & 0x00FF0000) | (((val) << 24) & 0xFF000000))
/* Swap 8 bytes, 64 bit values */
#define SWAP8BYTES(val)		((((val) >> 56) & 0x00000000000000FF) | (((val) >> 40) & 0x000000000000FF00) | (((val) >> 24) & 0x0000000000FF0000) | (((val) >> 8) & 0x00000000FF000000)	\
							(((val) << 8) & 0x000000FF00000000) | (((val) << 24) & 0x0000FF0000000000) | (((val) << 40) & 0x00FF000000000000) | (((val) << 56) & 0xFF00000000000000))

#endif /* GLOBAL_H */
