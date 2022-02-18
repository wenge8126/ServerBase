#pragma once
#ifndef _INCLUDE_COROUTINETOOL_H_
#define _INCLUDE_COROUTINETOOL_H_

#include "BaseCommon.h"
#include "Hand.h"
#include <functional>
#include "Auto.h"
#include "PoolList.h"
#include "AsyncFunction.h"
#include "AsyncAutoFunction.h"

typedef unsigned int CoroID;
#define CORO		CoroutineTool::current()
#define IN_CORO	(CoroutineTool::current()!=0)

// NOTE: YIELD与RESUME必须成对出现, RESUME 之前必须执行 YIELD , 否则, 会出现未可知的错误执行流程
//(错误示例: 正在连接未YIELD, 执行AWaitTime YIELD, 连接成功后, RESUME, 执行时间结束, 时间到时后, 又再次RESUME, 此时, 则使新的Task:Await 继续执行)
#define YIELD		CoroutineTool::yield();
#define RESUME(c)	CoroutineTool::Resume(c);
#define ASYNC(fun, obj) CoroutineTool::Async(AsyncFunction(fun, obj));
#define ASYNCAUTO(fun, obj) CoroutineTool::AsyncAuto(AsyncAutoFunction(fun, obj));
//-------------------------------------------------------------------------*/
// 用于协程创建, 以及协程函数执行完成后, 自动清理释放
// 1 AsyncDo 中创建并加入到列表
// 2 coFunction 中完成后, 从所有列表移除加入到待清空列表
// 3 BaseThread::Run内 调用 CheckFinish, 清空待删除列表进行清空
//-------------------------------------------------------------------------*/
class BaseCommon_Export  CoroutineTool 
{
	typedef PoolList<CoroID>  CoroToolList;

	CoroutineTool() {}

public:
	static CoroID Async(AsyncFunction  fun);
	static CoroID AsyncAuto(AsyncAutoFunction  fun);

	template<typename T>
	static CoroID AsyncCall(void(*function)(T), T param)
	{
		std::function<void()> f = std::bind(function, param);
		return Create(std::bind(runCoFunction, f));
	}

	static CoroID Async(std::function<void()> f)
	{
		return Create(std::bind(runCoFunction, f));
	}
	
	template<typename FUN>
	static void _runCoFunction(FUN f)
	{
		f();
		_Remove(CORO);
	}

	template<typename FUN>
	static CoroID AsyncCall(FUN f)
	{
		return Create(std::bind(_runCoFunction<FUN>, f));
	}

public:
	static CoroID current();
	static void yield();
	static bool Resume(CoroID coro);
	static unsigned int Create(std::function<void()> f);
	static void Destroy(CoroID coro);

	static void _Remove(CoroID id);

	static bool empty();

	static int getCount();

	static int getWaitStartCount();

	// 每帧进行释放已经完成的协程, 达到自动释放完成协程的目标
	static void CheckFinish();

	static CoroToolList& DeleteList();

	static CoroToolList& StartList();

	static CoroToolList& ResumeList();

	static void handCoFunction(AsyncFunction  fun);
	static void autoCoFunction(AsyncAutoFunction  fun);

	static void runCoFunction(std::function<void()> f);

#if DEVELOP_MODE
	static int& DebugCount();
	static int& DeleteCount();

	static void DumpDebug()
	{
		NOTE_LOG("All  coro count %d, delete %d, now = %d", DebugCount(), DeleteCount(), DebugCount()-DeleteCount());
	}

#endif
};



#endif //_INCLUDE_COROUTINETOOL_H_
