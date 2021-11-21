

#include "ObjectManager.h"
#include <stdarg.h>
#include "Common.h"

#define USE_CHANGE_ID		0

//EasyMap<int, AutoObjectFactory>				ObjectManager::mObjectFactory;

ObjectManager::ObjectManager()
: mCount(OBJECT_FIRST_ID)
{
	mObjectList.resize(INIT_POOL_COUNT);
}

ObjectManager::~ObjectManager()
{

}

void ObjectManager::Log( const char *szInfo, ... )
{
#if DEVELOP_MODE 
	va_list argptr;
	va_start(argptr, szInfo);

	TableTool::DebugLog(argptr,szInfo);
#endif
}

AObject ObjectManager::CreateObject( const char *szType, bool bUseFreeID, ObjectManager *pFactMgr )
{
	AutoObjectFactory hF;
	if (pFactMgr!=NULL)
		hF = pFactMgr->GetObjectFactory(szType);
	else
		hF = GetObjectFactory(szType);

	if (hF)
	{
		AObject hObj = hF->NewObject();
		if (hObj)
		{
			AppendObject(hObj, bUseFreeID);
		}
		return hObj;
	}
	return AObject();
}

AObject ObjectManager::CreateObject( int nType, bool bUseFreeID, ObjectManager *pFactMgr )
{
	AutoObjectFactory hF;
	if (pFactMgr!=NULL)
		hF = pFactMgr->GetObjectFactory(nType);
	else
		hF = GetObjectFactory(nType);
	if (hF)
	{
		AObject hObj = hF->NewObject();
		if (hObj)
		{
			AppendObject(hObj, bUseFreeID);
		}
		return hObj;
	}
	return AObject();
}

AObject ObjectManager::CreateObjectSpecifiedID( int nType, ID specifiedID, ObjectManager *pFactMgr )
{
	if ( FindObject(specifiedID) )
	{
		Log("XXX Error: ��Ӧ��ID�����Ѿ�����, ����ָ��ID����ʧ��>>> [%d]", specifiedID);
		return AObject();
	}
	if (specifiedID>OBJECT_MAX_COUNT)
	{
		Log("XXX Error: ���ش���: ���ڶ����������������� [30000]");
		AssertEx(0, "���ش���: ���ڶ����������������� [30000]");		
	}
	AutoObjectFactory hF;
	if (pFactMgr!=NULL)
		hF = pFactMgr->GetObjectFactory(nType);
	else
		hF = GetObjectFactory(nType);
	if (hF)
	{
		AObject hObj = hF->NewObject();
		if (hObj)
		{	
			hObj->SetID(specifiedID);
			if (mCount<specifiedID+1)
			{
				mObjectList.resize(specifiedID+1);
				mCount = specifiedID+1;
			}		

			mObjectList[specifiedID] = hObj;
			return hObj;
		}
	}
	
	return AObject();
	
}


AObject ObjectManager::FindObject( ID iIndex )
{
	if (iIndex>=OBJECT_FIRST_ID && iIndex<mCount)
		return mObjectList[iIndex];
	return AObject();
}

bool ObjectManager::RemoveObject( ID iIndex, bool bFree )
{
	if (iIndex>=OBJECT_FIRST_ID && iIndex<mCount)
	{
		if (mObjectList[iIndex] && bFree)
			mObjectList[iIndex]->OnNodifyRemove();
#if USE_CHANGE_ID
		// Ϊ��ɾ���󲻸ı����������id��������������������ʱ������Ϊ�յĶ���λ��
		mObjectList[iIndex] = mObjectList[mCount-1];
		mObjectList[mCount-1] = AObject();
		--mCount;
#else
		mObjectList[iIndex].setNull();
#endif
		return true;
	}
	return false;
}

bool ObjectManager::RemoveObject( AObject hObj, bool bFree )
{
	if (hObj)
		return RemoveObject(hObj->GetID(), bFree);

	return false;
}

int ObjectManager::Process( void *param, bool bInitCount )
{
	int count = 0;
	//!!! ��������ʼʱ,һ��Ҫ��֤mObjectList����������
	//int maxCount = mCount;
	//if (maxCount>mObjectList.size())
	//	maxCount = mObjectList.size();
	for (size_t i=OBJECT_FIRST_ID; i<mCount; ++i)
	{
		AObject &obj = mObjectList[i];
		if (obj)
		{
			if (obj->NeedRemove())
			{
				obj->OnNodifyRemove();
				obj._free();
				RemoveObject(i);
			}
			else
			{
				try{
					++count;
					obj->Process(param);
				}
				catch (std::exception &e)
				{
					ERROR_LOG("[%d] process error >%s", i, e.what());
				}
				catch (...)
				{
					ERROR_LOG("[%d] process error", i);
				}
			}
		}
	}
	if (bInitCount && mCount>OBJECT_FIRST_ID && count<=0)
		mCount = OBJECT_FIRST_ID;
	return count;
}

void ObjectManager::RegisterObject( const char *szType, AutoObjectFactory hFact )
{
	hFact->SetTypeName(szType);
	if (mObjectFactoryByName.erase(szType))
	{		
		ERROR_LOG("Warn: already object factory, when add [%s] factory", szType);
		//AssertEx(0, "xxx Error: �Ѿ����ڶ��󹤳�");
	}
	mObjectFactoryByName.insert(szType, hFact);
}

void ObjectManager::RegisterObject( int nType, AutoObjectFactory hFact )
{
	hFact->SetType(nType);
	if (mObjectFactory.remove(nType))
	{
		WARN_LOG("Warn: already [%d] object factory", nType);
		//AssertEx(0, "xxx Error: �Ѿ����ڶ��󹤳�");
	}
	mObjectFactory.Append(nType, hFact);
}

AutoObjectFactory ObjectManager::GetObjectFactory( const char *szType )
{
	return mObjectFactoryByName.find(szType);
}

AutoObjectFactory ObjectManager::GetObjectFactory( int nType )
{
	return mObjectFactory.Find(nType);
}

ID ObjectManager::AppendObject( AObject hObj, bool bUseFreeID )
{
	if (mCount>OBJECT_MAX_COUNT)
	{
		Log("XXX Error: ���ش���: ���ڶ����������������� [30000]");
		AssertEx(0, "���ش���: ���ڶ����������������� [30000]");		
	}

#if USE_CHANGE_ID
	if (mCount>=mObjectList.size())
		mObjectList.resize(mObjectList.size()*2);
	hObj->SetID(mCount);
	mObjectList[mCount++] = hObj;
	return hObj->GetID();
#else
	//NOTE: ʹ������IDʱ�����׳��ֿͻ���ɾ����ͬ��ID��BUG
	if (bUseFreeID)
	{
		for (size_t i=OBJECT_FIRST_ID; i<mCount; ++i)
		{
			if (!mObjectList[i])
			{
				mObjectList[i] = hObj;
				hObj->SetID(i);
				return i;
			}
		}
	}

	if (mCount>=mObjectList.size())
		mObjectList.resize(mObjectList.size()*2);
	hObj->SetID(mCount);
	mObjectList[mCount++] = hObj;
	return hObj->GetID();
#endif
}

void ObjectManager::RemoveAll( bool bFreeObject )
{
	for (size_t i=OBJECT_FIRST_ID; i<mCount; ++i)
	{
		if (mObjectList[i])
		{
			mObjectList[i]->OnNodifyRemove();
			if (bFreeObject)
				mObjectList[i]._free();
		}
	}	
	mObjectList.clear();
	mObjectList.resize(OBJECT_FIRST_ID);
	mCount = OBJECT_FIRST_ID;
}

void ObjectManager::RemoveAllFactory( void )
{
	//for (auto it=mObjectFactory.begin(); it.have(); it.next())
	for (int i=0; i<mObjectFactory.size(); ++i)
	{
		mObjectFactory[i]._free();
	}
	mObjectFactory.clear();
	for (auto it=mObjectFactoryByName.begin(); it; ++it)
	{
		(*it)._free();
	}
	mObjectFactoryByName.clear();
}

bool ObjectManager::AllotCount( int needCount )
{
	if (needCount>OBJECT_MAX_COUNT)
	{
		Log("XXX Error: ���ش���: ���ڶ����������������� [30000]");
		AssertEx(0, "���ش���: ���ڶ����������������� [30000]");		
		return false;
	}
	if ((int)mCount<needCount)
	{
		mObjectList.resize(needCount);
		mCount = needCount;
		return true;
	}
	return false;
}


