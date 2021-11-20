#ifndef _INCLUDE_AI_H_
#define _INCLUDE_AI_H_

#include "Hand.h"
#include "AnyData.h"
#include "CEvent.h"
#include "EventCenter.h"
/*/-------------------------------------------------------------------------/
AIԭ�� ����ͼԭ��
1 ÿ���ڵ㶼����ִ�У���ִֻ��һ���¼�
2 �ڵ���м���˳�����������
3 �ڵ㼤��ǰ�������ɵ�ǰ��Ľڵ㰴˳��ݹ��˻��ƶ�����Ҫ����Ľڵ�
4 ����󣬼�ִ��
5 ִ�к󣬽������
6 ���󱣴�ļ��ǵ�ǰ��Ľڵ�
7 ÿ���ڵ��ڶ����ڹ����ȫ�ֵ�������, �Ҿ�������˽�е�����
//-------------------------------------------------------------------------*/

class tObject;

namespace AI
{

	//-------------------------------------------------------------------------*/
	// AI�����ڱ����������ָ�룬�ڵ��ڱ��������壬���Ե����嶪���ڵ�ʱ�������ȶ����������е�ָ������Ϊ��
	class tNode;
	class tAIData : public NiceData
	{
		friend class tNode;
		friend class tAI;
	public:
		tObject* GetOwner() { return mpOwner; }

	public:
		tAIData()
			: mpOwner(NULL)
		{
			 
		}

	protected:
		tObject	*mpOwner;
	};
	typedef Auto<tAIData> AIData;

	//-------------------------------------------------------------------------*/
	class tNode : public Logic::CEvent
	{
		friend class tAI;
		typedef Hand<tNode> ANode;
	public:
		virtual void start() = 0;
		virtual bool needStart() = 0;
		virtual bool OnTrigger(int eventType, NiceData *pData) { return false; }

		virtual void OnDead() {}
		virtual void OnHit(int attacker) {}
		virtual void OnHpChanged(int oldHp) {}

		virtual void think()
		{
			ANode needStartAI = thinkStart();
			if (needStartAI)
				needStartAI->DoEvent(false);
			else
			{
				needStartAI = thinkUpStart();
				if (needStartAI)
					needStartAI->DoEvent(false);
			}
		}

		virtual void Trigger(int eventType, NiceData *pData)
		{
			if (!_Trigger(eventType, pData, true))
				_Trigger(eventType, pData, false);			
		}

		virtual bool _Trigger(int eventType, NiceData *pData, bool up)
		{
			if (OnTrigger(eventType, pData))
				return true;

			if (up)
			{
				if (mParent)
				{				
					return mParent->_Trigger(eventType, pData, up);
				}
			}
			else
			{
				for (auto it=mChildrenList.begin(); it; ++it)
				{
					ANode n = *it;
					if (n->_Trigger(eventType, pData, up))
						return true;
				}					
			}
			return false;
		}

		virtual bool enable(){ return mAIData->GetOwner()!=NULL; }

		virtual bool _DoEvent() override
		{
			if (GetOwner()==NULL)
			{
				Free();
				return false;
			}
			start();
			return true;
		}

		virtual void _OnFinish() override
		{
			think();
		}

	public:
		// ����׼���õĽڵ�, �����ڵ�ǰ�ܹ��ݹ���ȥ�����ӽڵ��е�
		ANode StartUpNode(const char *szNodeName)
		{
			if (mParent)
			{
				if (strcmp(mParent->GetEventName(), szNodeName)==0)
				{
					mParent->DoEvent(false);
					return mParent;
				}
				return mParent->StartUpNode(szNodeName);
			}

			return ANode();
		}

		ANode StartNode(const char *szNodeName)
		{
			for (auto it=mChildrenList.begin(); it; ++it)
			{
				ANode n = *it;
				if (strcmp(n->GetEventName(), szNodeName)==0)
				{
					n->DoEvent(false);
					return n;
				}
				ANode re = n->StartNode(szNodeName);
				if (re)
					return re;
			}					
			return ANode();
		}

		// ����������Ҫִ�е�AI
		ANode thinkStart()
		{
			for (auto it=mChildrenList.begin(); it; ++it)
			{
				ANode n = *it;
				if (n->needStart())
				{					
					return n;
				}
				ANode re = n->thinkStart();
				if (re)
					return re;
			}					
			return ANode();
		}

		// ����������Ҫִ�е�AI
		ANode thinkUpStart()
		{
			if (mParent)
			{
				if (mParent->needStart())
				{ 
					return mParent;
				}
				return mParent->thinkUpStart();
			}

			return ANode();
		}

		tObject* GetOwner() { return mAIData->GetOwner(); }

		// �����еĵ�ǰ�ڵ��ڶ����ͷ�ʱ����ִ�д˹���, ��֤�������ָ����ȷ
		void Destory()
		{
			mAIData->mpOwner = NULL;
		}

	public:
		void Init(AIData data)
		{
			mAIData = data;
		}
		bool AppendChildren(const char *szNodeName, bool bRelpace)
		{
			if (!bRelpace && mChildrenList.exist(szNodeName))
				return false;
			Hand<tNode> node = StartEvent(szNodeName);
			if (node)
			{
				AppendChildren(node);
				return true;
			}
			return false;
		}

		void AppendChildren(Hand<tNode> children)
		{
			children->mAIData = mAIData;
			mChildrenList.insert(children->GetEventName(), children);
		}

	protected:
		Hand<tNode>	mParent;

		EasyHash<AString, Hand<tNode>>	mChildrenList;

		AIData		mAIData;
	};

	typedef Hand<tNode> ANode;
	//-------------------------------------------------------------------------*/
	// AI Ӧ��
	//-------------------------------------------------------------------------*/
	class tAI : public MemBase
	{
	public:
		void Start(const char *szAIName, Logic::tEventCenter *pCenter)
		{
			Stop();
			mAINode = pCenter->StartEvent(szAIName);
			if (mAINode)
			{
				mAINode->mAIData = mAIData;
				mAINode->DoEvent(false);
			}
		}

		void Stop()
		{
			if (mAINode)
			{
				mAINode->Finish();
				mAINode->Free();
				mAINode.setNull();
			}
		}

	public:
		tAI(tObject *pOwner)
		{
			mAIData = MEM_NEW tAIData();
			mAIData->mpOwner = pOwner;
		}

		tAI(AIData aiData)
		{
			mAIData = aiData;
		}

		~tAI()
		{
			Stop();
		}

	public:
		AIData			mAIData;
		ANode			mAINode;
	};
	//-------------------------------------------------------------------------*/

}


#endif //_INCLUDE_AI_H_