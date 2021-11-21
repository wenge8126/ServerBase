#pragma once


#ifndef _INCLUDE_POOLLOOP_H_
#define _INCLUDE_POOLLOOP_H_

#include "PoolList.h"

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
template <typename T>
class PoolLoop : public LoopList<T>
{
	typedef LoopList<T> Base;

public:
	// ���α�������: ��ȡ��ͷ��һ��ֵ, �ҽ���һ���ڵ��ƶ�Ϊ��ͷ, �����б�ר�����칦��
	// NOTE: ���ݸ��ӱ�ǩ�ڵ�, ���ж��Ƿ��ٴα�ѭ����
	struct LoopIterator
	{
		typename LoopList<T>::Node	*mpFlagNode;
		PoolLoop<T>								*mpLoopList;

		typedef typename LoopList<T>::Node		LoopNode;
	public:
		LoopIterator(PoolLoop<T>  *pLoopList)
			: mpLoopList(pLoopList)
		{
			mpFlagNode = pLoopList->_NewNode();
			pLoopList->_insert(mpFlagNode);
		}
		~LoopIterator()
		{
			mpLoopList->_remove(mpFlagNode);
			mpFlagNode = NULL;
		}

		operator bool() const
		{
			LoopNode *pRoot = mpLoopList->__getRoot();
			return (pRoot != NULL && pRoot != mpFlagNode);
		}

		T& value()
		{
			LoopNode	*pRoot = mpLoopList->__getRoot();
			if (pRoot != NULL && pRoot != mpFlagNode)
				return pRoot->mValue;
			static thread_local T sVal;
			return sVal;
		}

		T* valuePtr()
		{
			LoopNode	*pRoot = mpLoopList->__getRoot();
			if (pRoot != NULL && pRoot != mpFlagNode)
				return &pRoot->mValue;
			return NULL;
		}

		bool erase()
		{
			LoopNode	*pRoot = mpLoopList->__getRoot();
			if (pRoot != NULL && pRoot != mpFlagNode)
			{
				mpLoopList->_remove(pRoot);
				return true;
			}
			return false;
		}

		bool loop()
		{
			mpLoopList->loop();
			LoopNode	*pRoot = mpLoopList->__getRoot();
			return (pRoot != NULL && pRoot != mpFlagNode);
		}

		// �����ڵ��ƶ�����һ���ڵ�, ��������һ���ڵ�ֵ��ָ��
		T* loopNextValue()
		{
			if (mpLoopList->loop())
			{
				LoopNode *pRoot = mpLoopList->__getRoot();
				if (pRoot != NULL && pRoot != mpFlagNode)
				{
					return &pRoot->mValue;
				}
			}
			return NULL;
		}

		void reset()
		{
			mpLoopList->_remove(mpFlagNode);
			mpFlagNode = mpLoopList->_NewNode();
			mpLoopList->_insert(mpFlagNode);
		}
	};

public:
	PoolLoop(int freeMaxCount)
		: mFreeMaxCount(freeMaxCount)
		, mFreeNodeRoot(NULL)
	{

	}

	~PoolLoop()
	{
		while (mFreeNodeRoot != NULL)
		{
			typename Base::Node *p = mFreeNodeRoot;
			mFreeNodeRoot = mFreeNodeRoot->mpNext;
			delete p;
		}
	}

	PoolLoop(const PoolLoop &other)
	{
		AssertNote(0, "��֧�������ֵ");
	}

	PoolLoop& operator = (const PoolLoop &other)
	{
		AssertNote(0, "��֧�������ֵ");
		return *this;
	}

public:
	virtual typename Base::Node*  _NewNode()
	{
		if (mFreeNodeRoot == NULL)
			return MEM_NEW typename  Base::Node();
		else
		{
			typename Base::Node *p = mFreeNodeRoot;
			mFreeNodeRoot = mFreeNodeRoot->mpNext;
			return p;
		}
	}

protected:
	virtual void _FreeNode(typename Base::Node *pNode)
	{
		//if (mFreeMaxCount>0 && mFreeNodeList.size()>=mFreeMaxCount)
		//	delete pNode; 
		//else
		{
			pNode->mpPreviou = NULL;
			pNode->mValue = T();
			if (mFreeNodeRoot == NULL)
			{
				pNode->mpNext = NULL;
				mFreeNodeRoot = pNode;
			}
			else
			{
				pNode->mpNext = mFreeNodeRoot;
				mFreeNodeRoot = pNode;
			}
		}
	}

protected:
	typename Base::Node	*mFreeNodeRoot;
	int	mFreeMaxCount;
};

#endif