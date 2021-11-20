#include "EventManager.h"



void EventManager::appendListen(const char *szEventName, ListenFunction listen, bool bCheckExist /*= true*/, bool bOnlyOne)
{
	AListenList list = mListenHash.find(szEventName);
	if (list)
	{
		if (bOnlyOne)
			list->clear(false);
		else if (bCheckExist)
		{
			for (int i = 0; i < list->size(); ++i)
			{
				if ((*list)[i] == listen)
					return;
			}
		}
	}
	else
	{
		list = MEM_NEW ListenFunList();
		mListenHash.insert(szEventName, list);
	}
	list->push_back(listen);
}

bool EventManager::removeListen(const char *szEventName, ListenFunction listen)
{
	AListenList list = mListenHash.find(szEventName);
	if (list)
	{
		for (int i = 0; i < list->size(); ++i)
		{
			if ((*list)[i] == listen)
			{
				list->removeAt(i);
				return true;
			}
		}
	}
	return false;
}

bool EventManager::dispatchEvent(const char *szEventName, tNiceData *pData)
{
	AListenList list = mListenHash.find(szEventName);
	if (list)
	{
		for (int i = 0; i < list->size(); )
		{
			ListenFunction &fun = (*list)[i];
			if (fun.empty())
				list->removeAt(i);
			else
			{
				fun.run(pData);
				++i;
			}
		}
		return true;
	}
	return false;
}

EventManager& EventManager::GetMe()
{
	thread_local static EventManager msMgr;
	return msMgr;
}
////////////////////////////////////////////////////////////////////////// Ã¶¾Ùid×÷Îªkey

void PEventManager::appendListen(int16_t eventId, ListenFunction listen, bool bCheckExist /*= true*/)
{
	AListenList list = mListenHash.find(eventId);
	if (list)
	{
		if (bCheckExist)
		{
			for (int i = 0; i < list->size(); ++i)
			{
				if ((*list)[i] == listen)
					return;
			}
		}
	}
	else
	{
		list = MEM_NEW ListenFunList();
		mListenHash.insert(eventId, list);
	}
	list->push_back(listen);
}

bool PEventManager::removeListen(int16_t eventId, ListenFunction listen)
{
	AListenList list = mListenHash.find(eventId);
	if (list)
	{
		for (int i = 0; i < list->size(); ++i)
		{
			if ((*list)[i] == listen)
			{
				list->removeAt(i);
				return true;
			}
		}
	}
	return false;
}

bool PEventManager::dispatchEvent(int16_t eventId, tNiceData *pData)
{
	AListenList list = mListenHash.find(eventId);
	if (list)
	{
		for (int i = 0; i < list->size(); )
		{
			ListenFunction &fun = (*list)[i];
			if (fun.empty())
				list->removeAt(i);
			else
			{
				fun.run(pData);
				++i;
			}
		}
		return true;
	}
	return false;
}

PEventManager& PEventManager::GetMe()
{
	thread_local static PEventManager msMgr; return msMgr;
}