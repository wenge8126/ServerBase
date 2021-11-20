/********************************************************************
	created:	2011/05/24
	created:	24:5:2011   3:11
	filename: 	h:\2011_02_27\Code\common\EventCore\EasyList.h
	file path:	h:\2011_02_27\Code\common\EventCore
	file base:	EasyList
	file ext:	h
	author:		���ĸ�
	
	purpose:	��������
	NOTE:		����λ�õ�����ͷ
				size() ���б���������Ч�ʽϵ�
*********************************************************************/
#ifndef _INCLUDE_EASYLIST_H_
#define _INCLUDE_EASYLIST_H_

#include "MemBase.h"
#include "BaseCommon.h"

template<typename V>	
class EasyListNode : public MemBase
{
public:
	EasyListNode()
		: next(NULL)
	{

	}
	~EasyListNode()
	{
		//if (next)
		//	delete next;
	}

	EasyListNode	*next;
	V				mVal;
};


template<typename T>
class EasyList : public MemBase
{
public:
	EasyList<T>()
		: mRootNode(NULL)
	{
		
	}
	~EasyList()
	{
		clear();
	}
public:
	//virtual void onRaversal(T &noodVal) = 0;

	class iterator
	{
		friend class EasyList<T>;
	public:
		iterator()
			: mNode(NULL)
		{

		}
		iterator(EasyListNode<T> *node)
			: mNode(node)
		{
			 
		}

		operator bool ()
		{
			return mNode!=NULL;
		}

		T& operator  *()
		{
			if (mNode)
				return mNode->mVal;
			return EasyList<T>::_getStatic();
		}
		iterator& operator ++ ()
		{
			if (mNode)
				mNode = mNode->next;
			else
				mNode = NULL;
			return *this;
		}

		iterator& operator ++ (int)
		{
			if (mNode)
				mNode = mNode->next;
			else
				mNode = NULL;
			return *this;
		}

		//bool operator != (const iterator &other) const
		//{
		//	return mNode != other.mNode; 
		//}

	private:
		EasyListNode<T> *mNode; 

		
	};

public:
	iterator begin()
	{
		return iterator(mRootNode);
	}

	static iterator& staticIterator()
	{
		_THREAD_LOCAL_ static  iterator msIt;
		return msIt;
	}

	iterator insert(const iterator &whereIt, const T &val)
	{
		if (mRootNode==NULL || mRootNode==whereIt.mNode)
		{
			insert(val);
			return begin();
		}
		else
		{
			EasyListNode<T> *pNood = mRootNode;
			while (pNood)
			{
				if (pNood->next==whereIt.mNode)
				{
					pNood->next = MEM_NEW EasyListNode<T>();
					pNood->next->mVal = val;
					pNood->next->next = whereIt.mNode;
					return iterator(pNood->next);
				}
				pNood = pNood->next;
			}
		}
		return staticIterator();
	}

	iterator erase(const iterator &whereIt)
	{
		if (NULL == mRootNode || NULL==whereIt.mNode)
			return staticIterator();
		if (mRootNode==whereIt.mNode)
		{
			remove(NULL, mRootNode);
			return iterator(mRootNode);
		}
		EasyListNode<T> *pNood = mRootNode;
		while (pNood)
		{
			if (pNood->next && pNood->next==whereIt.mNode)
			{
				remove(pNood, pNood->next);
				return iterator(pNood->next);
			}
			pNood = pNood->next;
		}
		//AssertEx(0, "������������Ԫ��")
		return staticIterator();
	}

public:
	bool empty()const{ return mRootNode==NULL; }
	DSIZE size() const
	{
		DSIZE count = 0;
		EasyListNode<T> *pNood = mRootNode;
		while (pNood)
		{
			++count;
			pNood = pNood->next;
		}
		return count;
	}
	void clear()
	{
		while (mRootNode)
		{
			remove(NULL, mRootNode);
		}
		//if (mRootNode)
		//{
		//	delete mRootNode;
		//	mRootNode = NULL;
		//}
	}

public:
	// NOTE: insert at begin
	void insert(const T &val)
	{
		EasyListNode<T> *pNood = MEM_NEW EasyListNode<T>();
		pNood->mVal = val;
		pNood->next = mRootNode;
		mRootNode = pNood;
	}

	void push_back(const T &val)
	{
		if (NULL==mRootNode)
		{
			insert(val);
			return;
		}

		EasyListNode<T> *pNode = mRootNode;
		while (pNode)
		{
			if (NULL==pNode->next)
				break;
			pNode = pNode->next;
		}
		EasyListNode<T> *newNode = MEM_NEW EasyListNode<T>();
		newNode->mVal = val;
		newNode->next = NULL;
		pNode->next = newNode;
	}

	bool pop_back(T &result)
	{
		if (NULL==mRootNode)
		{			
			return false;
		}

		EasyListNode<T> *pNode = mRootNode;
		EasyListNode<T> *up = NULL;
		while (pNode)
		{
			if (NULL==pNode->next)
			{
				result = pNode->mVal;
				remove(up, pNode);
				return true;
			}
			up = pNode;
			pNode = pNode->next;			
		}
		return false;
	}

	void push_front(const T &val)
	{
		insert(val);
	}

	bool pop_front(T &result)
	{
		if (mRootNode)
		{
			result = mRootNode->mVal;
			remove(NULL, mRootNode);
			return true;
		}
		return false;
	}

	bool remove(const T &val)
	{
		if (NULL == mRootNode)
			return false;
		if (mRootNode->mVal==val)
			return remove(NULL, mRootNode);
		EasyListNode<T> *pNood = mRootNode;
		while (pNood)
		{
			if (pNood->next && pNood->next->mVal==val)
			{
				return remove(pNood,pNood->next);
			}
			pNood = pNood->next;
		}
		return false;
	}

	void append(EasyList &other)
	{
		EasyListNode<T> *pNood = other.mRootNode;
		while(pNood)
		{
			push_back(pNood->mVal);
			pNood = pNood->next;
		}
	}

	bool exist(const T &element)
	{
		EasyListNode<T> *pNood = mRootNode;
		while (pNood)
		{
			if (pNood->mVal==element)
			{
				return true;
			}
			pNood = pNood->next;
		}
		return false;
	}

	const T& back() const
	{
		if (NULL==mRootNode)
		{			
			return _getStatic();
		}

		EasyListNode<T> *pNode = mRootNode;
		EasyListNode<T> *up = NULL;
		while (pNode)
		{
			if (NULL==pNode->next)
			{				
				return pNode->mVal;
			}
			up = pNode;
			pNode = pNode->next;			
		}
		return _getStatic();
	}

	bool back(T &result)
	{
		if (NULL==mRootNode)
		{			
			return false;
		}

		EasyListNode<T> *pNode = mRootNode;
		EasyListNode<T> *up = NULL;
		while (pNode)
		{
			if (NULL==pNode->next)
			{				
				result = pNode->mVal;
				return true;
			}
			up = pNode;
			pNode = pNode->next;			
		}
		return false;
	}

public:
	bool remove(EasyListNode<T> *up, EasyListNode<T> *delNood)
	{
		if (NULL==up) //root
			mRootNode = delNood->next;
		else
			up->next = delNood->next;
		
		delNood->next = NULL;
		delete delNood;
		return true;
	}


	EasyList& operator = (const EasyList &other)
	{
		//AssertEx(0, "Not can use this function");
		clear();
		const EasyListNode<T> *pNood = other.mRootNode;
		while (pNood)
		{
			push_back(pNood->mVal); 
			pNood = pNood->next;
		}
		return *this;
	}

	EasyList(const EasyList &other)
		: mRootNode(NULL)
	{
		//AssertEx(0, "Not can use this function");
		*this = other;
	}

	void swap(EasyList &other)
	{
		EasyListNode<T> *temp = mRootNode;
		mRootNode = other.mRootNode;
		other.mRootNode = temp;
	}

	static T& _getStatic() 
	{
		_THREAD_LOCAL_ static T			msVal;
		return msVal;
	}

protected:
	EasyListNode<T> *mRootNode;
};



#endif //_INCLUDE_EASYLIST_H_
