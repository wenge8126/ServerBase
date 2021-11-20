#pragma once

#ifndef _INCLUDE_EVENTMANAGER_H_
#define _INCLUDE_EVENTMANAGER_H_

#include "MemBase.h"
#include "ListenFunction.h"
#include "ArrayList.h"
#include "EventCore.h"
#include "EasyHash.h"

class EventCoreDll_Export  EventManager : public MemBase
{
public:
	static void AppendListen(const char *szEventName, ListenFunction  listen, bool bCheckExist = true, bool bOnlyOne = false)
	{
		GetMe().appendListen(szEventName, listen, bCheckExist, bOnlyOne);
	}

	static bool RemoveListen(const char *szEventName, ListenFunction  listen)
	{
		return GetMe().removeListen(szEventName, listen);
	}

	static bool RemoveListen(const char *szEventName)
	{
		return GetMe().removeListen(szEventName);
	}

	static bool DispatchEvent(const char *szEventName, tNiceData *pData)
	{
		return GetMe().dispatchEvent(szEventName, pData);
	}

public:
	void appendListen(const char *szEventName, ListenFunction  listen, bool bCheckExist = true, bool bOnlyOne = false);

	bool removeListen(const char *szEventName, ListenFunction  listen);

	bool removeListen(const char *szEventName)
	{
		return mListenHash.erase(szEventName);
	}

	bool dispatchEvent(const char *szEventName, tNiceData *pData);

	void removeAll()
	{
		mListenHash.clear(true);
	}

public:
	static EventManager& GetMe();

protected:
	typedef ArrayList<ListenFunction>	ListenFunList;
	typedef AutoPtr<ListenFunList>	AListenList;
	EasyHash<AString, AListenList>		mListenHash;
};

////////////////////////////////////////////////////////////////////////// Ã¶¾Ùid×÷Îªkey
class EventCoreDll_Export  PEventManager : public MemBase
{
public:
	static void AppendListen(int16_t eventId, ListenFunction  listen, bool bCheckExist = true)
	{
		GetMe().appendListen(eventId, listen, bCheckExist);
	}

	static bool RemoveListen(int16_t eventId, ListenFunction  listen)
	{
		return GetMe().removeListen(eventId, listen);
	}

	static bool RemoveListen(int16_t eventId)
	{
		return GetMe().removeListen(eventId);
	}

	static bool DispatchEvent(int16_t eventId, tNiceData *pData)
	{
		return GetMe().dispatchEvent(eventId, pData);
	}

public:
	void appendListen(int16_t eventId, ListenFunction  listen, bool bCheckExist = true);

	bool removeListen(int16_t eventId, ListenFunction  listen);

	bool removeListen(int16_t eventId)
	{
		return mListenHash.erase(eventId);
	}

	bool dispatchEvent(int16_t eventId, tNiceData *pData);

	void removeAll()
	{
		mListenHash.clear(true);
	}

public:
	static PEventManager& GetMe();

protected:
	typedef ArrayList<ListenFunction>	ListenFunList;
	typedef AutoPtr<ListenFunList>	AListenList;
	EasyHash<int16_t, AListenList>		mListenHash;
};


#endif //_INCLUDE_EVENTMANAGER_H_