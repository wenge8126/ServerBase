#ifndef _WORKTHREAD_H_
#define _WORKTHREAD_H_

#include "BaseCommon.h"
#include "MemBase.h"
#include "AutoString.h"

class BaseCommon_Export WorkThread : public MemBase
{
public:
	WorkThread();
	virtual ~WorkThread()
	{
		Close();	
	}

	virtual void InitThread(void);
	 
	virtual void Close(void);

	virtual void Suspend();

	virtual bool Resume();
    
    virtual bool IsThreadQuit();
    
	bool _ForceClose(size_t reVal);

	bool IsActive(){ return !mClose; }
	void WaitTheadFinish();
	UInt64 GetThreadID(){ return mThreadID; }

public:
	virtual void backWorkThread(void) = 0;
	virtual void onBeforeClose(void) {};
	virtual void onExceptionExist(){}

protected:
#if !__LINUX__
	UInt64			mBackTread;
#endif

	bool				mClose;                 //  «∑Ò÷’÷π

#if __WINDOWS__
	UINT				mThreadID;
#elif __LINUX__
	UInt64				mThreadID;
#endif
	AString				mDebugName;
};


#endif //_WORKTHREAD_H_