/********************************************************************
	created:	2012/04/16
	created:	16:4:2012   21:32
	filename: 	d:\New\Common\DataBase\EasyMap.h
	file path:	d:\New\Common\DataBase
	file base:	EasyMap
	file ext:	h
	author:		杨
	
	purpose:	映射索引容器
				精巧，快速
				二分法检索算法
*********************************************************************/
#ifndef _INCLUDE_EASYMAP_H_
#define _INCLUDE_EASYMAP_H_
#include "Assertx.h"
#include <utility>

#include <stdlib.h>
#include "MemBase.h"
#include <new>
#include "BaseCommon.h"
#include "Common.h"
 
#define NULL_POS	(-1)
//-------------------------------------------------------------------------
// 用在Map内部1字节对齐, 节省空间
#pragma pack(push)
#pragma pack(1)

template<typename K, typename T>
struct EasyMapValue
{
	K			mKey;
	mutable T	mVal;

public:
	EasyMapValue()
		: mKey()
		, mVal()
	{

	}
};

//-------------------------------------------------------------------------
//用于索引映射查寻容器(如map)
template<typename K, typename T, int INIT_COUNT = 8, bool bMIN_SIZE = false, bool bUSE_POOL = true, typename COUNT_TYPE = ushort>
class EasyMap  : public MemBase
{
public:
	typedef  EasyMapValue<K, T>	Value;

protected:
	//-------------------------------------------------------------------------
	// 内存中的对象处理, 填充, 移除
	// 填充对象初始值, 保证对象初始内存空间不为零时的正确性
	// 移除时, 可以保证正确调用对象的析构, 保证对象正确释放
	//-------------------------------------------------------------------------
	void __Set(Value &o) { new (&(o)) Value; }
	void __Free(Value &o) { (&o)->~Value();  }
	//-------------------------------------------------------------------------

protected:
    COUNT_TYPE	mAllotSize;
    COUNT_TYPE	mCount;
	Value	*mpPtrArray;

public:
	class Iterator
	{
		friend class EasyMap;

	public:
		T& begin()
		{
			mCurrent = 0;
			if (mMap!=NULL)
				return mMap->get(mCurrent);

			return mMap->_getStaticValue().mVal;
		}
		bool have()
		{
			if (mMap!=NULL)
				return mCurrent<mMap->size();

			return false;
		}

		bool end()
		{
			if (mMap!=NULL)
				return mCurrent >= mMap->size();

			return true;
		}

		const K& key()
		{
			if (mMap!=NULL)
				return mMap->getKey(mCurrent);
			static const K s = K();
			return s;
		}

		T& operator * ()
		{
			if (mMap!=NULL)
				return mMap->get(mCurrent);

			return mMap->_getStaticValue().mVal;
		}

		T& get() 
		{
			if (mMap!=NULL)
				return mMap->get(mCurrent);
			AssertNote(0, "逻辑上不会出现这种情况");
			return mMap->_getStaticValue().mVal;
		}

		const EasyMap::Value& getValue() const
		{
			if (mMap!=NULL)
				return mMap->getValue(mCurrent);
			AssertNote(0, "逻辑上不会出现这种情况");
			return mMap->_getStaticValue();
		}
        const EasyMap::Value& Value() const
        {
            if (mMap!=NULL)
                return mMap->getValue(mCurrent);
            AssertNote(0, "逻辑上不会出现这种情况");
            return mMap->_getStaticValue();
        }

		bool next()
		{
			++mCurrent;
			return have();
		}
		T& nextValue()
		{
			++mCurrent;
			return get();
		}

		bool previou()
		{
			if (mCurrent>0 && have())
			{
				--mCurrent;
				return true;
			}

			mCurrent = NULL_POS;
			return false;
		}

		bool erase()
		{
			if (mMap!=NULL)
				return mMap->_remove(mCurrent);

			return false;
		}

        void reset() 
        {
            mCurrent = 0;
        }

		operator bool (){ return have(); }
        bool operator ++(){ return next(); }
        bool operator ++(int){ return next(); }

	public:
		Iterator()
		{
			mMap = NULL;
			mCurrent = NULL_POS;
		}
		Iterator(EasyMap *pMap)
		{
			mMap = pMap;			
			mCurrent = 0;
		}

		size_t getCurrentPos() const { return mCurrent; }

	protected:
		EasyMap		*mMap;
		size_t		mCurrent;
	};

public:
	Iterator begin()
	{
		return Iterator(this);
	}

	Iterator begin( const K &key )
	{
		Iterator it;

		size_t pos = NULL_POS;
		findFirst(key, &pos);
		if (pos!=NULL_POS)
		{
			it.mMap = this;
			it.mCurrent = pos;
		}
		return it;
	}

	Iterator GetLastIterator()
	{
		Iterator it;

		it.mMap = this;
		if (empty())
			it.mCurrent = NULL_POS;
		else
		{
			it.mCurrent = size()-1;
		}
		return it;
	}

protected:	
	void _allot(size_t num)
	{
		clear(true);
		if (num==0)		
			return;
		
		if (bUSE_POOL)
			mpPtrArray = (Value*)ALLOCT_NEW(num*_esize());
		else
			mpPtrArray = (Value*)::malloc(num*_esize());

		if (mpPtrArray != NULL)
		{
			memset(mpPtrArray, 0, num*_esize());
			mAllotSize = num;
		}
	}

public:
	// Warn : Use For fast serialization
	void _resize(size_t num)
	{
		if (num==mAllotSize)
			return;

		if (num==0)
		{
			clear(false);
			return;
		}		

		Value* pOld = mpPtrArray;	

		if (bUSE_POOL)
			mpPtrArray = (Value*)ALLOCT_NEW(num*_esize());
		else
			mpPtrArray = (Value*)::malloc(num*_esize());

		//only init new more space
		if (num>mCount)
		{
			if (pOld!=NULL && mpPtrArray!=NULL)
				memcpy(mpPtrArray, pOld, mCount*_esize());

			if (mpPtrArray!=NULL)
				memset(mpPtrArray+mCount, 0, (num-mCount)*_esize());
		}
		else 
		{
			if (pOld != NULL)
			{
				if (mpPtrArray!=NULL)
					memcpy(mpPtrArray, pOld, num*_esize());
				if (mCount < num)
				{
					WARN_LOG("EasyMap _resize() changed count, because now resize count %llu < now count %llu", num, mCount);
					for (size_t i = num; i < mCount; ++i)
					{
						__Free(pOld[i]);
					}
					mCount = num;
				}
			}
		}

		if (pOld != NULL)
		{
			if (bUSE_POOL)
				Allot::freePtr((void*)pOld, mAllotSize*_esize());
			else
				::free(pOld);
		}

		mAllotSize = num;
	}

public:
	void* _data(){ return mpPtrArray; }
	void _setCount(size_t uCount, size_t uAllotSize){ mCount = uCount; mAllotSize = uAllotSize; }

	size_t _esize() const { return sizeof(Value); }

	size_t _findInsertPos(const K &key) const
	{
		if (mCount==0)
		{
			return 0;
		}
		size_t end = mCount;
		size_t begin = 0;
		while (true)
		{
			if (end-begin==0)
				return begin;
			else if (end-begin==1)
			{
				if (mpPtrArray[begin].mKey < (K)key)
					return begin + 1;
				return begin;
			}
			size_t m = (end-begin)/2;
			if (mpPtrArray[begin+m].mKey > (K)key)
			{
				//left
				end = begin+m;				
			}
			else
			{
				//right
				begin += m; 				
			}
		}
	}

public:
	bool _remove(size_t pos)
	{
		if (pos<mCount)
		{
			///order to call destructor function.
			__Free(mpPtrArray[pos]);

			memmove(mpPtrArray+pos, mpPtrArray+(pos+1), (mCount-pos-1)*_esize());
			memset(&(mpPtrArray[mCount-1]), 0, _esize());

			--mCount;

			if (bMIN_SIZE)
				_resize(mCount);

			return true;
		}
		return false;
	}
	size_t _find(const K &key) const
	{
		if (mCount==0)
		{
			return NULL_POS;
		}
		size_t end = mCount;
		size_t begin = 0;
		while (true)
		{
			if (end-begin==0)
				return NULL_POS;
			else if (end-begin==1)
			{
				if (mpPtrArray[begin].mKey!=(K)key)
					return NULL_POS;

				return begin;
			}
			size_t m = (end-begin)/2;
			if (mpPtrArray[begin+m].mKey > (K)key)
			{
				//left
				end = begin+m;				
			}
			else
			{
				//right
				begin += m; 				
			}
		}
	}

	template<typename KT>
	size_t _findBy(const KT &key) const
	{
		if (mCount==0)
		{
			return NULL_POS;
		}
		size_t end = mCount;
		size_t begin = 0;
		while (true)
		{
			if (end-begin==0)
				return NULL_POS;
			else if (end-begin==1)
			{
				if (mpPtrArray[begin].mKey!=key)
					return NULL_POS;

				return begin;
			}
			size_t m = (end-begin)/2;
			if (mpPtrArray[begin+m].mKey > key)
			{
				//left
				end = begin+m;				
			}
			else
			{
				//right
				begin += m; 				
			}
		}
	}

public:
	EasyMap()
		: mAllotSize(0)
		, mCount(0)
		, mpPtrArray(NULL)
	{
	}

	EasyMap(const EasyMap &other)
		: mAllotSize(0)
		, mCount(0)
		, mpPtrArray(NULL)
	{
		*this = other;
	}

	~EasyMap()
	{
		clear(true);
	}

public:
	DSIZE size() const { return (DSIZE)mCount; }

	bool empty()const{ return 0==mCount; }

	// 因为原来为方便下标取值, 但与stl map下标找到KEY值相混, 所以暂时不开放
	const T& operator [] (const K &key) const
	{
		return find(key);
	}

	T& operator [] (const K &key)
	{
		return find(key);
	}

	const T& get(size_t pos) const
	{
		if (pos<mCount)
			return mpPtrArray[pos].mVal;
		//AssertEx(0, "错误: 给定位置下标越界");
		return _getStaticValue().mVal;
	}

	T& get(size_t pos)
	{
		if (pos<mCount)
			return mpPtrArray[pos].mVal;
		//AssertEx(0, "错误: 给定位置下标越界");
		return _getStaticValue().mVal;
	}

	const K& getKey(size_t pos) const
	{
		if (pos<mCount)
			return mpPtrArray[pos].mKey;
		//AssertEx(0, "错误: 给定位置下标越界");
		return _getStaticValue().mKey;
	}

	const Value& getValue(size_t pos) const
	{
		if (pos<mCount)
			return mpPtrArray[pos];
		//AssertEx(0, "错误: 给定位置下标越界");
		return _getStaticValue();
	}

	T* findPtr(const K &key) const
	{
		size_t pos = _find(key);
		if (pos<mCount)
			return &(mpPtrArray[pos].mVal);
		return NULL;
	}

	bool findEx(const K &key, T &result) const
	{
		T *p = findPtr(key);
		if (p)
		{
			result = *p;
			return true;
		}
		return false;
	}

	template<typename KT>
	bool findExBy(const KT &key, T &result) const
	{
		size_t pos = NULL_POS;
		result = findBy(key, &pos);
		return (pos!=NULL_POS);
	}
	template<typename KT>
	T* findPtrBy(const KT &key) const
	{
		size_t pos = NULL_POS;
		T& result = findBy(key, &pos);
		if (pos!=NULL_POS)
			return &result;
		return NULL;
	}

	void clear(bool bFreeBuffer = true)
	{
		if (mpPtrArray!=NULL)
		{		
			for (size_t i=0; i<mCount; ++i)
			{
				__Free(mpPtrArray[i]);
			}

			if (bFreeBuffer || bMIN_SIZE)
			{
				if (bUSE_POOL)
					Allot::freePtr(mpPtrArray, mAllotSize*_esize());
				else
					::free(mpPtrArray);

				mpPtrArray = NULL;
				mAllotSize = 0;
			}
			else
				memset(mpPtrArray, 0, _esize()*mAllotSize);
		}
		mCount = 0;
	}

	size_t insert(const K &key, const T &val )
	{
		Value v;
		v.mKey = key;
		v.mVal = val;
		return insert(v);
	}

	size_t insert(const Value &val)
	{
#pragma warning(push)
#pragma warning(disable:6387)
		size_t insertPos = _findInsertPos(val.mKey);

		if (mAllotSize<mCount+1)
		{			
			if (bMIN_SIZE)
				_resize(mCount + 1);
			else
			{
				size_t newSize = INIT_COUNT;
				if (mAllotSize > 0)
					newSize = mAllotSize * 2;
				_resize(newSize);
			}
		}
		
		memmove(mpPtrArray+insertPos+1, mpPtrArray+insertPos, _esize()*(mCount-insertPos));
		__Set(mpPtrArray[insertPos]);
		mpPtrArray[insertPos] = val;

		++mCount;
		return insertPos;
#pragma warning(pop)
	}

	T& find(const K &key, size_t *resultPos = NULL) const
	{
		size_t pos = _find(key);
		if (pos==NULL_POS)
		{
			if (resultPos)
				*resultPos = NULL_POS;
			return _getStaticValue().mVal;
		}
		
		if (resultPos!=NULL)
			*resultPos = pos;

		return mpPtrArray[pos].mVal;
	}

	template<typename KT>
	T& findBy(const KT &key, size_t *resultPos = NULL) const
	{
		size_t pos = _findBy(key);
		if (pos==NULL_POS)
		{
			if (resultPos)
				*resultPos = NULL_POS;
			return _getStaticValue().mVal;
		}

		if (resultPos!=NULL)
			*resultPos = pos;

		return mpPtrArray[pos].mVal;
	}

	const T& findFirst(const K &key, size_t *resultPos = NULL) const
	{
		size_t pos;
		T &result = find(key, &pos);

		if (pos==NULL_POS)
			return _getStaticValue().mVal;

		while (pos>0)
		{
			if (getKey(pos-1)!=key)
				break;
			--pos;
		}
		
		if (resultPos!=NULL)
			*resultPos = pos;

		return get(pos);
	}

	size_t erase(const K &key)
	{
		size_t num = 0;
		while ( true )
		{
			size_t pos = _find(key);
			if (pos==NULL_POS)
				break;
			_remove(pos);
			++num;
		}
		return num;
	}

	// delete value from multiple key map
	size_t erase(const K &key, const T &delValue)
	{
		size_t pos = _find(key);

		if (pos!=NULL_POS)
		{
			if (get(pos)==delValue)
			{
				_remove(pos);
				return 1;
			}
			size_t right = pos;
			while (true)
			{				
				if (++right>=size())
					break;

				if (getKey(right)!=key)
					break;

				if (get(right)==delValue)
				{
					_remove(right);
					return 1;
				}
			}
			//left
			while (pos>0)
			{				
				--pos;
				if (getKey(pos)!=key)
					return false;

				if (get(pos)==delValue)
				{
					_remove(pos);
					return 1;
				}
			}
		}

		return 0;
	}

	bool exist(const K &key)const
	{
		return _find(key)!=NULL_POS;
	}

	EasyMap& operator = (const EasyMap &other)
	{
		clear();
		if (!other.empty())
		{
			_allot(other.size());
			for ( size_t i=0; i<other.size(); ++i)
			{
				__Set(mpPtrArray[i]);
				mpPtrArray[i] = other.mpPtrArray[i];
			}
			mCount = mAllotSize;
		}
		return *this;
	}

	void swap(EasyMap &other)
	{
		size_t s = other.mAllotSize;
		other.mAllotSize = mAllotSize;
		mAllotSize = s;
		s = other.mCount;
		other.mCount = mCount;
		mCount = s;
		Value *p = other.mpPtrArray;
		other.mpPtrArray = mpPtrArray;
		mpPtrArray = p;
	}

	const Value* first()
	{
		if (!empty())
			return &(getValue(0));

		return NULL;
	}

	const Value* last()
	{
		if (!empty())
			return &(getValue(size()-1));

		return NULL;
	}

	size_t _useSpaceSize() const
	{
		return sizeof(EasyMap) + sizeof(Value) * mAllotSize;
	}

	static Value& _getStaticValue()
	{
		_THREAD_LOCAL_ static Value msValue;
		return msValue; 
	}
};

#pragma pack(pop)
//-------------------------------------------------------------------------


#endif