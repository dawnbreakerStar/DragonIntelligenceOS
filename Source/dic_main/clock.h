/*-----------------------------------------------------------------------------------------
**
** 版权(Copyright): Inory, 2023~
**
** 文件名(Filename): clock.h
**
** 描述(Description): 本文件定义Clock类,该类实现定时功能
**
** 设计注记(Design Annotation):
**
** 作者(Author):
**		Inory , 2023年7月4日开始编写本文件
**
** 更改历史(Modification History):
**		Inory , 2023年7月4日 创建本文件
**		$Revision$
**		$Date$
**		$Author$
**		$Log$
**
**-----------------------------------------------------------------------------------------
*/
#ifndef CLOCK_H
#define CLOCK_H

/*-----------------------------------------------------------------------------------------
**										   Include
**-----------------------------------------------------------------------------------------
*/
#include "global.h"
#ifdef WINDOWS
#define NOMINMAX					/* 禁用C++中的min/max宏定义 */
#define WIN32_LEAN_AND_MEAN			/* 不定义此宏,windows.h有可能包含winsock.h */
#include <windows.h>
#include <mmsystem.h>				/* windows多媒体定时器头文件和库文件 */
#pragma comment(lib, "Winmm.lib")

/*-----------------------------------------------------------------------------------------
**									   Type Definition
**-----------------------------------------------------------------------------------------
*/
/* 定时参数结构体 */
struct timeval
{
	U32 tv_sec;		/* 秒 */
	U64 tv_usec;	/* 微秒 */
};
struct itimerval
{
	struct timeval it_interval;
	struct timeval it_value;
};
#else
/*-----------------------------------------------------------------------------------------
**										   Include
**-----------------------------------------------------------------------------------------
*/
#include <sys/time.h>
#endif

/*-----------------------------------------------------------------------------------------
**									   Type Definition
**-----------------------------------------------------------------------------------------
*/
/* 用户时钟处理函数指针类型 */
typedef void (*ClockHandler)(void);

/*-----------------------------------------------------------------------------------------
**									   Type Definition
**-----------------------------------------------------------------------------------------
*/
/* 定时周期默认值(秒+微秒) */
#define PERIOD_SECOND	0
#define PERIOD_USECOND	10000
/* 定时计数 */
#define INTERVAL_10MS	1
#define INTERVAL_20MS	2
#define INTERVAL_30MS	3
#define INTERVAL_40MS	4
#define INTERVAL_50MS	5
#define INTERVAL_100MS	10
#define INTERVAL_200MS	20
#define INTERVAL_500MS	50
#define INTERVAL_1S		100

/*-----------------------------------------------------------------------------------------
**									   Class Definition
**-----------------------------------------------------------------------------------------
*/
/* <类描述> */
/* 1)产生周期定时信号,作为应用程序的时钟源 */
/* 2)单实例 */
typedef struct SClock
{
	/* 设置时钟周期 */
	void (*setPeriod)(void* this, U32 second, U64 microsecond);
	/* 设置时钟处理函数 */
	void (*setClockHandler)(void* this, ClockHandler handler);
	/* 启动时钟 */
	bool (*start)(void* this);
	/* 停止时钟 */
	void (*stop)(void* this);
} Clock;

/*-----------------------------------------------------------------------------------------
**									 Function Declaration
**-----------------------------------------------------------------------------------------
*/
/* 获取时钟单例 */
extern Clock* CreateClockInstance(void);
/* 析构函数 */
extern void DestroyClock(Clock* this);
/* 获取程序运行时间(单位为定时周期) */
extern U64 AppTime(void);
/* 获取UTC时间(单位为秒) */
extern I64 UtcTime(void);
/* 获取时间戳 */
extern U8* Timestamp(void);

#endif /* CLOCK_H */
