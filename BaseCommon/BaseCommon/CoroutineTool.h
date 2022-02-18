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

// NOTE: YIELD��RESUME����ɶԳ���, RESUME ֮ǰ����ִ�� YIELD , ����, �����δ��֪�Ĵ���ִ������
//(����ʾ��: ��������δYIELD, ִ��AWaitTime YIELD, ���ӳɹ���, RESUME, ִ��ʱ�����, ʱ�䵽ʱ��, ���ٴ�RESUME, ��ʱ, ��ʹ�µ�Task:Await ����ִ��)
#define YIELD		CoroutineTool::yield();
#define RESUME(c)	CoroutineTool::Resume(c);
#define ASYNC(fun, obj) CoroutineTool::Async(AsyncFunction(fun, obj));
#define ASYNCAUTO(fun, obj) CoroutineTool::AsyncAuto(AsyncAutoFunction(fun, obj));
//-------------------------------------------------------------------------*/
// ����Э�̴���, �Լ�Э�̺���ִ����ɺ�, �Զ������ͷ�
// 1 AsyncDo �д��������뵽�б�
// 2 coFunction ����ɺ�, �������б��Ƴ����뵽������б�
// 3 BaseThread::Run�� ���� CheckFinish, ��մ�ɾ���б�������
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

	// ÿ֡�����ͷ��Ѿ���ɵ�Э��, �ﵽ�Զ��ͷ����Э�̵�Ŀ��
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
