/*-----------------------------------------------------------------------------------------
**
** 版权(Copyright): Inory, 2023~
**
** 文件名(Filename): clock.c
**
** 描述(Description): 本文件定义Clock类成员函数实现
**
** 包含的函数(Included Function):
**		CreateClock
**		DestroyClock
**		SetPeriod
**		SetClockHandler
**		Start
**		Stop
**		AppTime
**		UtcTime
**		Timestamp
**		CreateClockInstance
**		HexToBcd
**		BcdToHex
**		UpdateTimestamp
**		TimerHandler
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
/*-----------------------------------------------------------------------------------------
**										   Include
**-----------------------------------------------------------------------------------------
*/
#include "clock.h"
#ifdef WINDOWS
#include <time.h>
#else
#include <signal.h>
#include <time.h>
#endif

/*-----------------------------------------------------------------------------------------
**									 Variable Definition
**-----------------------------------------------------------------------------------------
*/
/* 单例对象指针 */
static Clock* _clock = nullptr;
/* 每月天数 */
static U8 _days[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
/* 时间戳 */
static U8 _timestampBcd[8] = { 0 };
static U8 _timestampHex[8] = { 0 };

/*-----------------------------------------------------------------------------------------
**									 Function Declaration
**-----------------------------------------------------------------------------------------
*/
/* 设置时钟周期 */
static void SetPeriod(void* this, U32 second, U64 microsecond);
/* 设置时钟处理函数 */
static void SetClockHandler(void* this, ClockHandler handler);
/* 启动时钟 */
static bool Start(void* this);
/* 停止时钟 */
static void Stop(void* this);
/* HEX-BCD码转换 */
static U8 HexToBcd(U8 value);
/* BCD-HEX码转换 */
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

/*-----------------------------------------------------------------------------------------
**									   Class Definition
**-----------------------------------------------------------------------------------------
*/
/* <类描述> */
/* 1)时钟内部定义 */
/* 2) */
typedef struct SClockImp
{
	/* 设置时钟周期 */
	void (*setPeriod)(void* this, U32 second, U64 microsecond);
	/* 设置时钟处理函数 */
	void (*setClockHandler)(void* this, ClockHandler handler);
	/* 启动时钟 */
	bool (*start)(void* this);
	/* 停止时钟 */
	void (*stop)(void* this);

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
} ClockImp;

/*-----------------------------------------------------------------------------------------
**									 Function Definition
**-----------------------------------------------------------------------------------------
*/
/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): CreateClock
**
** 描述(Description): 本函数实现Clock类构造函数
**
** 输入参数(Input Parameter):
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**		Inory , 2023年7月16日 创建本函数
**		$Log$
**
**.EH--------------------------------------------------------------------------------------
*/
static Clock* CreateClock(void)
{
	ClockImp* this = __new(ClockImp);

	if (this)
	{
		int year;
		struct tm* localTime;

		/* 成员变量初始化 */
		this->_exitFlag = false;
		this->_clockHandler = nullptr;
		this->_period.it_value.tv_sec = PERIOD_SECOND;
		this->_period.it_value.tv_usec = PERIOD_USECOND;
		this->_period.it_interval = this->_period.it_value;
#ifdef WINDOWS
		/* 定时器句柄 */
		this->_timerId = 0;
#endif

		this->_appTime = 0;
		this->_utcTimeBegin = time((time_t*)nullptr);
		localTime = localtime((time_t*)&this->_utcTimeBegin);
		year = 1900 + localTime->tm_year;

		if ((year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0)))
		{
			_days[1] = 29;
		}

		_timestampBcd[0] = HexToBcd((U8)(year / 100));
		_timestampBcd[1] = HexToBcd((U8)(year % 100));
		_timestampBcd[2] = HexToBcd((U8)(1 + localTime->tm_mon));
		_timestampBcd[3] = HexToBcd((U8)localTime->tm_mday);
		_timestampBcd[4] = HexToBcd((U8)localTime->tm_hour);
		_timestampBcd[5] = HexToBcd((U8)localTime->tm_min);
		_timestampBcd[6] = HexToBcd((U8)localTime->tm_sec);
		_timestampBcd[7] = 0x00;

		_timestampHex[0] = (U8)((1900 + localTime->tm_yday) / 100);
		_timestampHex[1] = (U8)((1900 + localTime->tm_year) % 100);
		_timestampHex[2] = (U8)(1 + localTime->tm_mon);
		_timestampHex[3] = (U8)localTime->tm_mday;
		_timestampHex[4] = (U8)localTime->tm_hour;
		_timestampHex[5] = (U8)localTime->tm_min;
		_timestampHex[6] = (U8)localTime->tm_sec;
		_timestampBcd[7] = 0x00;

		/* 成员函数初始化 */
		this->setPeriod = SetPeriod;
		this->setClockHandler = SetClockHandler;
		this->start = Start;
		this->stop = Stop;
	}
	else
	{
		printf("Clock::Clock()--create Clock error.\n");
		return nullptr;
	}

	return (Clock*)this;
}
/* END of CreateClock */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): DestroyClock
**
** 描述(Description): 本函数实现Clock类析构函数
**
** 输入参数(Input Parameter):
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**		Inory , 2023年7月16日 创建本函数
**		$Log$
**
**.EH--------------------------------------------------------------------------------------
*/
void DestroyClock(Clock* this)
{
#ifdef DEBUG
	printf("Clock::~Clock()\n");
#endif
#ifdef WINDOWS
	/* 删除定时器句柄 */
	if (0 != (((ClockImp*)this)->_timerId))
	{
		timeKillEvent(((ClockImp*)this)->_timerId);
	}
#endif
}
/* END of DestroyClock */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): SetPeriod
**
** 描述(Description): 本函数实现设置时钟周期
**
** 输入参数(Input Parameter):
**		second , U32 定时周期秒
**		microsecond , U64 定时周期微秒
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**		Inory , 2023年7月16日 创建本函数
**		$Log$
**
**.EH--------------------------------------------------------------------------------------
*/
static void SetPeriod(void* this, U32 second, U64 microsecond)
{
	((ClockImp*)this)->_period.it_value.tv_sec = second;
	((ClockImp*)this)->_period.it_value.tv_usec = microsecond;
	((ClockImp*)this)->_period.it_interval = ((ClockImp*)this)->_period.it_value;
}
/* END of SetPeriod */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): SetClockHandler
**
** 描述(Description): 本函数实现设置用户时钟处理函数
**
** 输入参数(Input Parameter):
**		handler , ClockHandler 用户时钟处理函数
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**		Inory , 2023年7月16日 创建本函数
**		$Log$
**
**.EH--------------------------------------------------------------------------------------
*/
static void SetClockHandler(void* this, ClockHandler handler)
{
	((ClockImp*)this)->_clockHandler = handler;
}
/* END of SetClockHandler */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): Start
**
** 描述(Description): 本函数实现启动时钟
**
** 输入参数(Input Parameter):
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**		bool true-启动成功 false-启动失败
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**		Inory , 2023年7月16日 创建本函数
**		$Log$
**
**.EH--------------------------------------------------------------------------------------
*/
static bool Start(void* this)
{
#ifdef WINDOWS
	((ClockImp*)this)->_timerId = timeSetEvent((UINT)(((ClockImp*)this)->_period.it_value.tv_sec * 1000 + ((ClockImp*)this)->_period.it_value.tv_usec / 1000), 1, (LPTIMECALLBACK)TimerHandler, 0, (UINT)TIME_PERIODIC);
	if (0 == ((ClockImp*)this)->_timerId)
	{
		return false;
	}
	else
	{
		return true;
	}
#else
	/* 将定时器信号同处理函数进行绑定 */
	signal(SIGNALRM, timerHandler);

	/* 启动定时器 */
	if (-1 == setitimer(ITIMER_REAL, &_period, NULL))
	{
		return false;
	}
	else
	{
		return true;
	}
#endif
}
/* END of Start */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): Stop
**
** 描述(Description): 本函数实现关闭时钟
**
** 输入参数(Input Parameter):
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**		Inory , 2023年7月16日 创建本函数
**		$Log$
**
**.EH--------------------------------------------------------------------------------------
*/
static void Stop(void* this)
{
	((ClockImp*)this)->_exitFlag = true;
}
/* END of Stop */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): AppTime
**
** 描述(Description): 本函数实现返回程序运行时间
**
** 输入参数(Input Parameter):
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**		U64 程序运行时间
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**		Inory , 2023年7月16日 创建本函数
**		$Log$
**
**.EH--------------------------------------------------------------------------------------
*/
U64 AppTime(void)
{
	if (_clock)
	{
		return ((ClockImp*)_clock)->_appTime;
	}
	else
	{
		return 0;
	}
}
/* END of AppTime */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): UtcTime
**
** 描述(Description): 本函数实现获取UTC时间(单位为S)
**
** 输入参数(Input Parameter):
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**		I64 UTC时间
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**		Inory , 2023年7月16日 创建本函数
**		$Log$
**
**.EH--------------------------------------------------------------------------------------
*/
I64 UtcTime(void)
{
	if (_clock)
	{
		return ((ClockImp*)_clock)->_utcTimeBegin + (((ClockImp*)_clock)->_appTime / INTERVAL_1S);
	}
	else
	{
		return time((time_t*)NULL);
	}
}
/* END of UtcTime */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): Timestamp
**
** 描述(Description): 本函数实现获取时间戳
**
** 输入参数(Input Parameter):
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**		U8* 时间戳
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**		Inory , 2023年7月16日 创建本函数
**		$Log$
**
**.EH--------------------------------------------------------------------------------------
*/
U8* Timestamp(void)
{
	return _timestampBcd;
}
/* END of Timestamp */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): CreateClockInstance
**
** 描述(Description): 本函数实现返回时钟单例指针
**
** 输入参数(Input Parameter):
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**		Clock* 时钟单例指针
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**		Inory , 2023年7月16日 创建本函数
**		$Log$
**
**.EH--------------------------------------------------------------------------------------
*/
Clock* CreateClockInstance(void)
{
	/* 创建并返回时钟对象指针 */
	if (!_clock)
	{
		_clock = CreateClock();

		if (!_clock)
		{
			printf("Clock::Instance()--create Clock error.\n");
		}
	}

	return _clock;
}
/* END of CreateClockInstance */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): HexToBcd
**
** 描述(Description): 本函数实现HEX-BCD码转换
**
** 输入参数(Input Parameter):
**		U8 HEX码
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**		U8 BCD码
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**		Inory , 2023年7月15日 创建本函数
**		$Log$
**
**.EH--------------------------------------------------------------------------------------
*/
static U8 HexToBcd(U8 value)
{
	value = (value > 200 ? value - 200 : (value >= 100 ? value - 100 : value));
	return (value / 10) << 4 | (value % 10);
}
/* END of HexToBcd */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): BcdToHex
**
** 描述(Description): 本函数实现BCD-HEX码转换
**
** 输入参数(Input Parameter):
**		U8 BCD码
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**		U8 HEX码
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**		Inory , 2023年7月15日 创建本函数
**		$Log$
**
**.EH--------------------------------------------------------------------------------------
*/
static U8 BcdToHex(U8 value)
{
	return (value >> 4) * 10 + (value & 0x0F);
}
/* END of BcdToHex */

/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): UpdateTimestamp
**
** 描述(Description): 本函数实现更新时间戳
**
** 输入参数(Input Parameter):
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**		Inory , 2023年7月15日 创建本函数
**		$Log$
**
**.EH--------------------------------------------------------------------------------------
*/
static void UpdateTimestamp(void)
{
	if (++_timestampHex[7] >= 100)
	{
		_timestampHex[7] = 0;
		if (++_timestampHex[6] >= 60)
		{
			_timestampHex[6] = 0;
			if (++_timestampHex[5] >= 60)
			{
				_timestampHex[5] = 0;
				if (++_timestampHex[4] >= 24)
				{
					_timestampHex[4] = 0;
					if (_timestampHex[3] >= _days[_timestampHex[2] - 1])
					{
						_timestampHex[3] = 0;
						if (++_timestampHex[2] >= 13)
						{
							_timestampHex[2] = 1;
							if (++_timestampHex[1] >= 100)
							{
								_timestampHex[1] = 0;
								if (++_timestampHex[0] >= 100)
								{
									_timestampHex[0] = 0;
								}
								_timestampBcd[0] = HexToBcd(_timestampHex[0]);
							}
							_timestampBcd[1] = HexToBcd(_timestampHex[1]);
						}
						_timestampBcd[2] = HexToBcd(_timestampHex[2]);
					}
					_timestampBcd[3] = HexToBcd(_timestampHex[3]);
				}
				_timestampBcd[4] = HexToBcd(_timestampHex[4]);
			}
			_timestampBcd[5] = HexToBcd(_timestampHex[5]);
		}
		_timestampBcd[6] = HexToBcd(_timestampHex[6]);
	}
	_timestampBcd[7] = HexToBcd(_timestampHex[7]);
}
/* END of UpdateTimestamp */

#ifdef WINDOWS
/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): TimerHandler
**
** 描述(Description): 本函数实现时钟中断处理
**
** 输入参数(Input Parameter):
**		wTimerID , UINT 事件ID
**		msg , UINT 消息
**		DWORD , dwUser 存放用户提供的回调数据
**		DWORD , dw1 参数一
**		DWORD , dw2 参数二
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**		Inory , 2023年7月15日 创建本函数
**		$Log$
**
**.EH--------------------------------------------------------------------------------------
*/
static void TimerHandler(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	UNUSED(wTimerID);
	UNUSED(msg);
	UNUSED(dwUser);
	UNUSED(dw1);
	UNUSED(dw2);

	/* 判断时钟是否启动 */
	if ((!_clock) || ((ClockImp*)_clock)->_exitFlag)
	{
		return;
	}

	/* 更新应用程序运行时间 */
	((ClockImp*)_clock)->_appTime++;
	/* 更新时间戳 */
	UpdateTimestamp();

	/* 调用用户时钟处理函数 */
	if (nullptr != ((ClockImp*)_clock)->_clockHandler)
	{
		((ClockImp*)_clock)->_clockHandler();
	}
}
/* END of TimerHandler */
#else
/*.BH--------------------------------------------------------------------------------------
**
** 函数名(Function Name): TimerHandler
**
** 描述(Description): 本函数实现时钟中断处理
**
** 输入参数(Input Parameter):
**		param , I32 中断标志
**
** 输出参数(Output Parameter):
**
** 返回值(Return Value):
**
** 设计注记(Design Annotation):
**
** 更改历史(Modification History):
**		Inory , 2023年7月15日 创建本函数
**		$Log$
**
**.EH--------------------------------------------------------------------------------------
*/
static void TimerHandler(I32 param)
{
	UNUSED(param);

	/* 判断时钟是否启动 */
	if ((!_clock) || ((ClockImp*)_clock)->_exitFlag)
	{
		return;
	}

	/* 更新应用程序运行时间 */
	((ClockImp*)_clock)->_appTime++;
	/* 更新时间戳 */
	UpdateTimestamp();

	/* 调用用户时钟处理函数 */
	if (nullptr != ((ClockImp*)_clock)->_clockHandler)
	{
		((ClockImp*)_clock)->_clockHandler();
	}
}
/* END of TimerHandler */
#endif