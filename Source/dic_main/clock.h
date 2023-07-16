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
typedef std::function<void()> ClockHandler;

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
class Clock
{
private:
    /* 构造函数 */
    Clock();
public:
    /* 析构函数 */
    ~Clock();

    /* 设置时钟周期 */
    void SetPeriod(U32 second, U64 microsecond);
    /* 设置时钟处理函数 */
    void SetClockHandler(ClockHandler handler);
    /* 启动时钟 */
    bool Start();
    /* 停止时钟 */
    void Stop();

    /* 获取程序运行时间(单位为定时周期) */
    static U64 AppTime();
    /* 获取UTC时间(单位为秒) */
    static I64 UtcTime();
    /* 获取时间戳 */
    static U8* Timestamp();

    /* 获取时钟单例 */
    static Clock* Instance();
private:
    /* HEX-BCD码转换 */
    static U8 HexToBcd(U8 value);
    static U8 BcdToHex(U8 value);
    /* 更新时间戳 */
    static void UpdateTimestamp();
#ifdef WINDOWS
    /* 时钟中断处理函数 */
    static void CALLBACK TimerHandler(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2);
#else
    /* 时钟中断处理函数 */
    static void TimerHandler(I32 param);
#endif
private:
    /* 退出标志 */
    bool _exitFlag;
    /* 用户时钟处理函数指针 */
    ClockHandler _clockHandler;
    /* 时钟周期 */
    struct itimerval _period;
#ifdef WINDOWS
    /* 定时器句柄 */
    MMRESULT _timerId;
#endif
    /* 应用程序运行时间(周期累加) */
    U64 _appTime;
    /* 应用程序运行时utc时间 */
    I64 _utcTimeBegin;

    /* 类单实例指针 */
    static Clock* _clock;
    /* 每月天数 */
    static U8 _days[12];
    /* 时间戳 */
    static U8 _timestampBcd[8];
    static U8 _timestampHex[8];
private:
    /* 垃圾回收类 */
    class GC
    {
    public:
        /* 构造函数 */
        GC()
        {
            PASS;
        }
        ~GC()
        {
            /* 释放单例内存 */
            FREE_OBJECT(_clock);
        }
    };
    /* 垃圾回收类 */
    static GC _gc;
};

#endif /* CLOCK_H */
