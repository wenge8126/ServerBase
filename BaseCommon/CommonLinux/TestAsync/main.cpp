#include <cstdio>
#include "CoroutineTool.h"
#include "TimeManager.h"
#include "TableManager.h"
#include "WorkThread.h"
#include <thread>
#include "coroutine.h"
#include <iostream>

class MM : public AutoBase
{
public:
	void AsyncTest()
	{
		NOTE_LOG("== start test * %u", CORO);
		YIELD;
		//Auto<TestTask> t = gTaskMgr.StartTask(1);
		//t->x = 0;
		//t->AWait(CORO);
		//int ccc = t->x;
		NOTE_LOG("** finish test * %u", CORO);
	}

public:
	MM()
	{

	}

	~MM()
	{

	}
};

void routine_func1()
{
	NOTE_LOG( "xx");
	coroutine::yield();

	NOTE_LOG( "finish" );

	//int i = channel.pop();
	//std::cout << i << std::endl;

	//i = channel.pop();
	//std::cout << i << std::endl;
}

class TestWork : public AutoBase
{
public:
	void backWorkThread() 
	{
		//coroutine::routine_t rt1 = coroutine::create(routine_func1);

		//std::cout << "00" << std::endl;
		//coroutine::resume(rt1);

		//std::cout << "01" << std::endl;

		//coroutine::resume(rt1);

		Auto<MM> m = MEM_NEW MM();
		auto co = ASYNCAUTO(&MM::AsyncTest, m.getPtr());
		NOTE_LOG("sleep 3 sec ...");
		m.setNull();
		TimeManager::Sleep(6000);
		RESUME(co);
		NOTE_LOG(" end ====== ");
	}
};


int main()
{
	Allot::setLogFile("TestNetNode");
	TableTool::SetLog(new TableLog("TestCommon.txt"));
	TableTool::SetErrorLog(new TableLog("TestError.txt"));
	 
	new TableManager();

	//std::thread t1(TestWork::backWorkThread);

	//t1.join();
	Auto<TestWork>  w = MEM_NEW TestWork();
	//w.InitThread();
	auto co = ASYNCAUTO(&TestWork::backWorkThread, w.getPtr());

	while (true)
	{
		TimeManager::Sleep(100);
		CoroutineTool::CheckFinish();
	}

    printf("hello from TestAsync!\n");
    return 0;
}