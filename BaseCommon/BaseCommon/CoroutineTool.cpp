#include "CoroutineTool.h"
#include "coroutine.h"
#include "TimeManager.h"


void CoroutineTool::yield()
{
	if (CORO == 0)
	{
		ERROR_LOG("Must yield in coroutine function");
		return;
	}
	coroutine::yield();
}

bool CoroutineTool::Resume(unsigned int coro)
{
	// 统一到主循环内进行恢复运行
	if (CORO != 0)
	{
		// 放到恢复列表, 在主线进行恢复
		ResumeList().push_back(coro);
		return true;
	}
	return coroutine::resume(coro)==0;
}

unsigned int CoroutineTool::Create(std::function<void()> f)
{
	CoroID id = coroutine::create(f);
#if DEVELOP_MODE
	++DebugCount();
#endif
	//if (coroutine::current() == 0)
	//{
	//	Resume(id);
	//}
	//else
		StartList().push_back(id);
	return id;
}

void CoroutineTool::Destroy(unsigned int coro)
{
	coroutine::destroy(coro);
}

void CoroutineTool::_Remove(CoroID id)
{
		DeleteList().push_front(id);
}

bool CoroutineTool::empty()
{
	if (!StartList().empty())
		return false;
	return coroutine::empty();	
}

int CoroutineTool::getCount()
{
	return coroutine::getCount();
}

int CoroutineTool::getWaitStartCount()
{
	return (int)StartList().size();
}

void CoroutineTool::CheckFinish()
{
	CoroToolList &startList = StartList();
	if (!startList.empty())
	{
		UInt64 now = TimeManager::NowTick();
		while (true)
		{
			// 一次只启动一个
			auto it = startList.begin();
			if (it)
			{
				CoroID id = *it;
				startList.erase(it);
				int err = coroutine::resume(id);
				if (err != 0)
					ERROR_LOG("Start resume coro fail %d, error : %d", id, err);
			}
			else
				break;
			if (TimeManager::NowTick() - now > 1)
				break;
		}
		//for (auto it = startList.begin(); it; )
		//{
		//	CoroID id = *it;
		//	startList.erase(it);
		//	Resume(id);
		//	break;
		//}
		//startList.clear();
	}

	CoroToolList &resumeList = ResumeList();
	if (!resumeList.empty())
	{
		UInt64 now = TimeManager::NowTick();
		while (true)
		{
			// 一次只启动一个
			auto it = resumeList.begin();
			if (it)
			{
				CoroID id = *it;
				resumeList.erase(it);
				int err = coroutine::resume(id);
				if (err != 0)
					ERROR_LOG("Resume coro fail %d, error : %d", id, err);
			}
			else
				break;
			if (TimeManager::NowTick() - now > 1)
				break;
		}
	}

	CoroToolList &list = DeleteList();
	if (!list.empty())
	{
		for (auto it = list.begin(); it; ++it)
		{
			coroutine::destroy(*it);
		}
#if DEVELOP_MODE
		DeleteCount() += list.size();
#endif
		list.clear();
	}
}

CoroutineTool::CoroToolList& CoroutineTool::DeleteList()
{
	thread_local static CoroToolList msDeleteList;
	return msDeleteList;
}

CoroutineTool::CoroToolList& CoroutineTool::StartList()
{
	thread_local static CoroToolList msStartList;
	return msStartList;
}

CoroutineTool::CoroToolList& CoroutineTool::ResumeList()
{
	thread_local static CoroToolList msResumeList;
	return msResumeList;
}

void CoroutineTool::handCoFunction(AsyncFunction fun)
{
	fun();
	_Remove(CORO);
}

void CoroutineTool::autoCoFunction(AsyncAutoFunction fun)
{
	fun();
	_Remove(CORO);
}

void CoroutineTool::runCoFunction(std::function<void()> f)
{
	f();
	_Remove(CORO);
}

#if DEVELOP_MODE
int& CoroutineTool::DebugCount()
{
	thread_local static int count = 0;
	return count;
}

int& CoroutineTool::DeleteCount()
{
	thread_local static int count = 0;
	return count;
}
#endif
CoroID CoroutineTool::Async(AsyncFunction fun)
{
	CoroID id = coroutine::create(std::bind(handCoFunction, fun));	
#if DEVELOP_MODE
	++DebugCount();
#endif
	if (coroutine::current() == 0)
	{
		Resume(id);
	}
	else
		StartList().push_back(id);
	return id;
}

CoroID CoroutineTool::AsyncAuto(AsyncAutoFunction fun)
{
	CoroID id = coroutine::create(std::bind(autoCoFunction, fun));
#if DEVELOP_MODE
	++DebugCount();
#endif
	if (coroutine::current() == 0)
	{
		Resume(id);
	}
	else
		StartList().push_back(id);
	return id;
}

CoroID CoroutineTool::current()
{
	return coroutine::current();
}

