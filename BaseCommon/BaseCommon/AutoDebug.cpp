/********************************************************************
	created:	2011/05/27
	created:	27:5:2011   1:55
	filename: 	e:\������\EventSystem_5.1\AutoDebug.cpp
	file path:	e:\������\EventSystem_5.1
	file base:	AutoDebug
	file ext:	cpp
	author:		���ĸ�
	
	purpose:	
*********************************************************************/

#include "AutoDebug.h"
#include "AutoPtr.h"

#define	DEBUG_CHAR_STRING		1

//UseInfoEx* UseInfoEx::New(){ NEW_USE_LOG return MEM_NEW UseInfoEx; }
//void UseInfoEx::Release(){ FREE_USE_LOG delete this; }

AutoDebugLog::~AutoDebugLog()
{
	for (PtrInfoMap::iterator it = mInfoMap.begin(); it!=mInfoMap.end(); ++it)
	{
		//if (it->second>0)
			//gAutoDebugLog.log("�д��ڶ���:[%d]-->[%s]",it->first, it->second);
		//else if (it->second<0)
		//	gAutoDebugLog.log("���ش���-->����:[%d]-->[%s]",it->first.c_str(),it->second.c_str());
	}
#if DEBUG_CHAR_STRING
	for (STRINGMAP::iterator it = gDebugAutoString.begin(); it!=gDebugAutoString.end(); ++it)
	{
		#pragma warning(push)
		#pragma warning(disable:4312)
		if (it->second>0)
			gAutoDebugLog.log("=======�д����ַ�������:[%s]-->[%d]",(char*)(it->first), it->second);
		else if (it->second<0)
			gAutoDebugLog.log("���ش���-->�ַ�������:[%s]-->[%d]",(char*)(it->first), it->second);
		#pragma warning(pop)
	}
#endif
	gAutoDebugLog.log("����Use����[%u]���ͷ�Use����[%u]", gNewUseCount, gFreeUseCount);
}

AutoDebugLog::AutoDebugLog() 
	: gNewCount(0)
	, gFreeCount(0)
	, gNewUseCount(0)
	, gFreeUseCount(0)
{
	gAutoDebugLog.setFile( "Auto_DebugInfo.txt","wt" );
	
}

void AutoDebugLog::NewLog( const type_info &classinfo, void *ptr )
{
	CsLockTool l(mLock);
	++gNewCount; 
	//gAutoDebugLog.log("����[%s] ��������Ϊ:[%u] �ͷŸ���[%u] ��ǰ����[%u]\n", 
	//	classinfo.name(), gNewCount, gFreeCount, gNewCount-gFreeCount);

	PtrInfoMap::iterator it = mInfoMap.find(ptr);
	if (it!=mInfoMap.end())
		gAutoDebugLog.log("���ش���-->��¼�������");
		//it->second ++;
	else
		mInfoMap[ptr] = classinfo.name();

#if DEBUG_CHAR_STRING
	if (typeid(char)==classinfo)
	{
		#pragma warning(push)
		#pragma warning(disable:4311)
		STRINGMAP::iterator it = gDebugAutoString.find((size_t)ptr);
		if (it!=gDebugAutoString.end())
		{
			it->second ++;
			gAutoDebugLog.log("���ش���-->�ַ�������:[%s]-->�ظ�����AUTO[%d]",(char*)ptr,it->second);
		}
		else
		{
			gDebugAutoString[(size_t)ptr] = 1;
			gAutoDebugLog.log("=====�ַ�������:[%s]\n",(char*)ptr);

		}
		#pragma warning(pop)
	}
#endif
}

void AutoDebugLog::FreeLog( const type_info &classinfo, void *ptr )
{
	CsLockTool l(mLock);
	++gFreeCount; 
	//gAutoDebugLog.log("�ͷ�[%s] ��������Ϊ:[%u] �ͷŸ���[%u] ��ǰ����[%u]\n", 
	//	classinfo.name(), gNewCount, gFreeCount, gNewCount-gFreeCount);
	if (!mInfoMap.empty())
	{
		PtrInfoMap::iterator it = mInfoMap.find(ptr);
		if (it!=mInfoMap.end())
			mInfoMap.erase(it);
			//it->second--;
		else
			gAutoDebugLog.log("����:û���������Ĵ���[%s]",classinfo.name());
	}
#if DEBUG_CHAR_STRING
	if (typeid(char)==classinfo)
	{
		#pragma warning(push)
#pragma warning(disable:4311)
		if (!gDebugAutoString.empty())
		{
			STRINGMAP::iterator it2 = gDebugAutoString.find((size_t)ptr);
			if (it2!=gDebugAutoString.end())
			{
				--it2->second;
				if (0==it2->second)
					gDebugAutoString.erase((size_t)ptr);
			}
			else
				gAutoDebugLog.log("����:û������ַ��Ĵ���[%llu]",(size_t)ptr);
		}
#pragma warning(pop)
	}
#endif
}