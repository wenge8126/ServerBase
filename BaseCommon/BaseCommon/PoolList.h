
#ifndef _INCLUDE_POOLLIST_H_
#define _INCLUDE_POOLLIST_H_

#include "AutoPtr.h"

// 具有自身节点分配池功能链表
// 提高重复增删效率
// NOTE: 目前不支持多线

class NodePool : public MemBase
{
	friend class Allot;

	struct BlockInfo
	{
		unsigned char*		mPoint;
		size_t				mSize;
	};

private:
	unsigned char				*m_NewPointer; 
	BlockInfo*					mAllocPtrList;
	size_t						mBlockCount;
	size_t						mObjectSize;
	int							mInitCount;

#if MEMORY_DEBUT_TEST
	size_t			mNewCount;
	size_t			mFreeCount;
	size_t			mTotalSize;
#endif

public:
	NodePool(int nInitCount = BLOCK_INIT_NUM)
	{
		mInitCount = nInitCount;
		m_NewPointer = 0;
		mAllocPtrList = NULL;
		mBlockCount = 0;
		mObjectSize = 4;
#if MEMORY_DEBUT_TEST
		mNewCount = 0;
		mFreeCount = 0;
		mTotalSize = 0;
#endif
	}

	~NodePool()
	{
		if (mAllocPtrList != NULL)
		{
			for (size_t i = 0; i < mBlockCount; ++i)
			{
				free(mAllocPtrList[i].mPoint);
			}
			delete[] mAllocPtrList;
			mAllocPtrList = NULL;
		}
		mAllocPtrList = 0;
		mBlockCount = 0;
	}

	void setBlockSize(size_t size){ mObjectSize = size; }
	size_t getBlockSize(void){ return mObjectSize; }
	size_t getCount(){ return mBlockCount; }

	size_t _useSpaceSize()
	{
		size_t s = 0;
		for( size_t i=0; i<mBlockCount; ++i )
		{
			s += mAllocPtrList[i].mSize;
		}
		return s;
	}

	void *newPtr()
	{

		if(NULL==m_NewPointer) 
			MyAlloc(); 
		unsigned char *rp = m_NewPointer; 
		//由于头4个字节被“强行”解释为指向下一内存块的指针，这里m_NewPointer就指向了下一个内存块，以备下次分配使用。 
		m_NewPointer = *reinterpret_cast<unsigned char**>(rp);

#if MEMORY_DEBUT_TEST
		mNewCount++;
#endif

		return rp; 
	}

	bool deletePtr(void *ptr)
	{
		if (!HasPoint(ptr))
		{
			return false;
		}

		*reinterpret_cast<unsigned char**>(ptr) = m_NewPointer; 
		m_NewPointer = static_cast<unsigned char*>(ptr); 
#if MEMORY_DEBUT_TEST
		mFreeCount++;
#endif

		return true;
	}

protected:
	//判断指针是否在分配范围内
	bool HasPoint(void *ptr)
	{
		for( size_t i=0; i<mBlockCount; ++i )
		{
			if (ptr>=mAllocPtrList[i].mPoint && ptr<mAllocPtrList[i].mPoint+mAllocPtrList[i].mSize)
				return true;
		}
		return false;
	}

	void appendPtr(unsigned char* pointer, size_t size)
	{
		if (mBlockCount>0)
		{
			BlockInfo* temp = mAllocPtrList;
			mAllocPtrList = new BlockInfo[mBlockCount+1];
			memcpy(mAllocPtrList, temp, sizeof(BlockInfo)*mBlockCount);
			delete[] temp;
		}
		else
		{
			mAllocPtrList = new BlockInfo[1];
		}
		mAllocPtrList[mBlockCount].mPoint = pointer;
		mAllocPtrList[mBlockCount].mSize = size;
		++mBlockCount;
	}

	void MyAlloc() 
	{ 
		size_t allocCount = mInitCount;
		if (getCount()>0)
			allocCount = mInitCount * getCount() * 2;
		size_t newSize = mObjectSize * allocCount;
		m_NewPointer = (unsigned char*)malloc(newSize); 
#if MEMORY_DEBUT_TEST
		mTotalSize += newSize;
#endif
		appendPtr(m_NewPointer, newSize);
		//强制转型为双指针，这将改变每个内存块头4个字节的含义。 
		unsigned char **cur = reinterpret_cast<unsigned char**>(m_NewPointer); 
		unsigned char *next = m_NewPointer; 
		for(size_t i = 0; i < allocCount-1; i++) 
		{ 
			next += mObjectSize; 
			*cur = next; 
			 //这样，所分配的每个内存块的头4个字节就被“强行“解释为指向下一个内存块的指针， 即形成了内存块的链表结构。 
			cur = reinterpret_cast<unsigned char**>(next);
		} 
		*cur = 0; 
	} 
};
typedef AutoPtr<NodePool>	AutoNodePool;
//----------------------------------------------------------------------------------

template <typename T, bool bInitPool = true>
class PoolList : public MemBase
{
public:
	class ListNode
	{
	public:
		ListNode()
			: next(NULL)
            , previous(NULL)
		{

		}
		~ListNode()
		{
			
		}

        ListNode *previous;
		ListNode	*next;
		T			mVal;
	};

	ListNode* __newNode(void)
	{
		void *p = mNodePool->newPtr();
		//printf( "\n new  [%p]", p);
		//memset(p, 0 , sizeof(ListNode));
		return	new (p) ListNode();
	}

	void __freeNode( ListNode *p )
	{
		//printf( "\n free [%p]", p);
		p->next = NULL;
        p->previous = NULL;
		p->mVal = T();
		
		if (mNodePool)
			mNodePool->deletePtr( p );
	}

public:
	PoolList()
		: mRootNode(NULL)
        , mLastNode(NULL)
        , mCount(0)
	{
		msVal = T();
		if (bInitPool)
		{
			mNodePool = MEM_NEW NodePool();
			mNodePool->setBlockSize(sizeof(ListNode));
		}
	}
	~PoolList()
	{
		clear();
	}

	void ReadyNodePool(AutoPtr<NodePool> hNodePool)
	{
		if (mNodePool && hNodePool->getBlockSize()!=mNodePool->getBlockSize())
		{
			AssertNote(0, "Pool block size is not same.");
			return;
		}
		AssertNote(empty(), "Warn: now list is not empty.");
		clear();
		mNodePool = hNodePool;
		AssertEx(mNodePool, "Error : ready node pool is null.");
	}

	AutoNodePool GetNodePool(void){ return mNodePool; }

public:
	//virtual void onRaversal(T &noodVal) = 0;

	class iterator
	{
		friend class PoolList<T, bInitPool>;
	public:
		iterator()
			: mNode(NULL)
		{

		}
		iterator(ListNode *node)
			: mNode(node)
		{

		}

		operator bool ()
		{
			return mNode!=NULL;
		}

		void* getNode(){ return mNode; }

		T& operator  *()
		{
			if (mNode)
				return mNode->mVal;
			return msVal;
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
		ListNode *mNode; 
		T		msVal;
	};

public:
	iterator begin()
	{
		return iterator(mRootNode);
	}

	// 插入到指定位置的前面, 如果指定位置不存在, 则插入到最前面
	iterator insert(const iterator &whereIt, const T &val)
	{
		if (mRootNode==NULL || mRootNode==whereIt.mNode || whereIt.mNode==NULL)
		{
			insert(val);
			return begin();
		}		
		
        ListNode *pNood  = whereIt.mNode;
        ListNode *pre = pNood->previous;
        ListNode *pNew = __newNode();

        pNew->mVal = val;

        pre->next = pNew;
        pNew->previous = pre;

        pNew->next = pNood;
        pNood->previous = pNew;

        ++mCount;
        return iterator(pNew);
    }

	// 插入到指定位置后面, 如果指定位置不存在, 则插入到最后面
	iterator insert_back(const iterator &whereIt, const T &val)
	{
		if (mRootNode==NULL)
		{
			AssertNote(mLastNode==NULL, "Frist is Null, then last must is Null");
			insert(val);
			return begin();
		}		

		if (whereIt.mNode==NULL || mLastNode==whereIt.mNode)
		{
			push_back(val);
			return iterator(mLastNode);
		}

		ListNode *pNood  = whereIt.mNode;
		ListNode *pNext = pNood->next;
		ListNode *pNew = __newNode();

		pNew->mVal = val;

		pNood->next = pNew;
		pNew->previous = pNood;

		pNext->previous = pNew;
		pNew->next = pNext;

		++mCount;
		return iterator(pNew);
	}

	void erase(iterator &whereIt)
	{
		if (NULL == mRootNode || NULL==whereIt.mNode)
			return;

		if (mRootNode==whereIt.mNode)
		{
			remove(NULL, mRootNode);			
			whereIt.mNode =mRootNode;
		}
		else
		{
			ListNode *pNext = whereIt.mNode->next;
			remove(whereIt.mNode->previous, whereIt.mNode);
			whereIt.mNode = pNext;		
		}
	}

public:
	bool empty()const{ return mRootNode==NULL; }
	DSIZE size() const
	{
        return mCount;		
	}
	void clear()
	{		
		while (mRootNode!=NULL)
		{
			ListNode *pNext = mRootNode->next;
			__freeNode(mRootNode);
			mRootNode = pNext;
		}
		mLastNode = NULL;
        mCount = 0;
	}

public:
	void insert(const T &val)
	{
		ListNode *pNood = __newNode();
		pNood->mVal = val;
		pNood->next = mRootNode;
        
        if (mRootNode==NULL)
            mLastNode = pNood;
        else
            mRootNode->previous = pNood;

		mRootNode = pNood;
        ++mCount;
	}

	void* push_back(const T &val)
	{
		if (NULL==mRootNode)
		{
			insert(val);
			return mRootNode;
		}

        AssertNote(mLastNode!=NULL, "Last node must exist");
        
        ListNode *pNew = __newNode();
        pNew->mVal = val;

        pNew->previous = mLastNode;
        mLastNode->next = pNew;

        mLastNode = pNew;

        ++mCount;

        return pNew;
	}

	bool pop_back(T &result)
	{
		if (NULL==mLastNode)
		{			
			return false;
		}
		result = mLastNode->mVal;
        return remove(mLastNode->previous, mLastNode);
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
		ListNode *pNood = mRootNode;
		while (pNood)
		{
			if (pNood->next && pNood->next->mVal==val)
			{
				return remove(pNood, pNood->next);
			}
			pNood = pNood->next;
		}
		return false;
	}

	void append(PoolList &other)
	{
		ListNode *pNood = other.mRootNode;
		while(pNood)
		{
			push_back(pNood->mVal);
			pNood = pNood->next;
		}
	}

	bool exist(T &element)
	{
		ListNode *pNood = mRootNode;
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


public:
	bool remove(ListNode *up, ListNode *delNood)
	{
		if (NULL==up) //root
        {
            if (delNood==mLastNode)
                mLastNode = NULL;
            delNood->previous = NULL;
			mRootNode = delNood->next;                   
        }
		else
        {
			up->next = delNood->next;
            if (delNood->next!=NULL)
                delNood->next->previous = up;
            else
            {
                AssertNote(delNood==mLastNode, "Must is last");
                mLastNode = up;
            }
        }
		
		delNood->next = NULL;
        delNood->previous = NULL;
		__freeNode(delNood);
        --mCount;
		return true;
	}


	PoolList& operator = (const PoolList &other)
	{
		//AssertEx(0, "Not can use this function");
		clear();
		const ListNode *pNood = other.mRootNode;
		while (pNood)
		{
			push_back(pNood->mVal); 
			pNood = pNood->next;
		}
        mCount = other.mCount;
		return *this;
	}

	PoolList(const PoolList &other)
		: mRootNode(NULL)
		, mLastNode(NULL)
		, mCount(0)
	{
		//AssertEx(0, "Not can use this function");
		*this = other;
	}

	void swap(PoolList &other)
	{
		ListNode *pNood = mRootNode;
		mRootNode = other.mRootNode;
		other.mRootNode = pNood;

		pNood = mLastNode;
		mLastNode = other.mLastNode;
		other.mLastNode = pNood;

		AutoPtr<NodePool> t = mNodePool;
		mNodePool = other.mNodePool; 
		other.mNodePool = t;
        size_t count = mCount;
        mCount = other.mCount;
        other.mCount = count;
	}

	// 不包括池的空间
	size_t _useSpaceSize(bool bAddPoolSize)
	{		
		if (bAddPoolSize)
			return mNodePool->_useSpaceSize() + sizeof(PoolList);
		return sizeof(PoolList);
	}

protected:
	DSIZE                     mCount;
	ListNode				*mRootNode;
    ListNode                *mLastNode;	

	AutoPtr<NodePool>			mNodePool;

	T						msVal;		// 支持多线，不可再定义为静态
	iterator				msIt;
};

//template<typename T>
//T PoolList<T>::msVal;


#endif //_INCLUDE_POOLLIST_H_