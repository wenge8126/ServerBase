#ifndef _INCLUDE_EVENTDLLFACE_H_
#define _INCLUDE_EVENTDLLFACE_H_

#include "EventFactory.h"
#include "EventCenter.h"

namespace Logic
{

	class tEventDllFace
	{
	public:
		virtual ~tEventDllFace()
		{
			// �������ǿ���ͷ�,���������,������֮��,ʹ�õ���ģ�鴴���Ĺ����������
			// ��ģ���ڴ����Ķ���,����ģ��رպ�ȫ���ջ�
			for ( size_t i=0; i<mFactoryList.size(); ++i )
			{
				mFactoryList[i]._free();
			}
		}

		void AppendFacotry( tEventCenter *eventCenter, const char* eventName, tEventFactory *pFactory )
		{
			AutoEventFactory hFact = pFactory;
			pFactory->__SetSelfHandle(hFact);
			mFactoryList.push_back(hFact);
			eventCenter->RegisterEvent(eventName, hFact);
		}

	protected:
		Array<AutoEventFactory>	mFactoryList;
	};


}

#endif //_INCLUDE_EVENTDLLFACE_H_