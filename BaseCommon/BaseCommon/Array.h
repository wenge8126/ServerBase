/********************************************************************
	created:	2011/05/24
	created:	24:5:2011   23:13
	filename: 	h:\2011_02_27\Code\common\DataBase\Array.h
	file path:	h:\2011_02_27\Code\common\DataBase
	file base:	Array
	file ext:	h
	author:		杨文鸽

	purpose:
	note:		分配内存时不会初始为0, 分配空间后进行memset 初始为0, 有效处理指针类型,int,float等基本类型的初始 2019-9-10
*********************************************************************/

#ifndef _INCLUDE_ARRAY_H_
#define _INCLUDE_ARRAY_H_

#include "Assertx.h"

#include "MemBase.h"
#include <new>
#include "BaseCommon.h"

#define	ARRAY_AUTO_SIZE		1

#pragma pack(push)
#pragma pack(1)

template<typename T, int INIT_COUNT = 8, typename _SIZE_TYPE = unsigned int>
class tArray : public MemBase
{
protected:
	//-------------------------------------------------------------------------
	// 内存中的对象处理, 填充, 移除
	// 填充对象初始值, 保证对象初始内存空间不为零时的正确性
	// 移除时, 可以保证正确调用对象的析构, 保证对象正确释放
	//-------------------------------------------------------------------------
	void __Set(T &o) { new (&o) T; }
	void __Free(T &o) { (&o)->~T(); }
	//-------------------------------------------------------------------------

public:
	tArray()
		:mCount(0)
		, mpArrayPtr(NULL)
		, mSize(0)
	{
        AssertEx(INIT_COUNT > 0, "Array INIT_COUNT must >0");
	}

	tArray(DSIZE initSize)
		: mCount(0)
		, mpArrayPtr(NULL)
		, mSize(0)
	{
		AssertEx(INIT_COUNT > 0, "Array INIT_COUNT must >0");
		resize(initSize);
	}

	tArray(DSIZE initSize, T def)
		: mCount(0)
		, mpArrayPtr(NULL)
		, mSize(0)
	{
		AssertEx(INIT_COUNT > 0, "Array INIT_COUNT must >0");
		resize(initSize, def);
	}

	~tArray()
	{
		clear(true);
	}

	DSIZE size() const { return mCount; }
	bool empty()const { return 0 == mCount; }

protected:
	void _resize(DSIZE num)
	{
		AssertEx(num>=0 && num * sizeof(T) < (_SIZE_TYPE)0xFFFFFFFF, "More max size %u", (_SIZE_TYPE)0xFFFFFFFF);
		T *p = mpArrayPtr;
		if (num == mCount)
			return;
		else if (num < (DSIZE)mCount)
		{
			size_t size = sizeof(T)*num;
			mpArrayPtr = (T*)ALLOCT_NEW(size);
			//memset(mpArrayPtr, 0, size);
			memcpy(mpArrayPtr, p, size);

			for (size_t i = num; i < mCount; ++i)
			{
				__Free(p[i]);
			}
		}
		else if (num == 0)
		{
			clear();
		}
		else if (mCount == 0)
		{
			size_t size = sizeof(T)*num;
			mpArrayPtr = (T*)ALLOCT_NEW(size);
			memset(mpArrayPtr, 0, size);
			for (size_t i = 0; i < num; ++i)
			{
				__Set(mpArrayPtr[i]);
				//mpArrayPtr[i] = def;
			}
		}
		else
		{
			size_t size = sizeof(T)*num;
			mpArrayPtr = (T*)ALLOCT_NEW(size);

			size_t existSize = sizeof(T)*mCount;
			memcpy(mpArrayPtr, p, existSize);
			// 把新分配的空间进行初始
			memset(((char*)mpArrayPtr) + existSize, 0, size - existSize);
			for (size_t i = mCount; i < num; ++i)
			{
				__Set(mpArrayPtr[i]);
				//mpArrayPtr[i] = def;
			}
		}

		Allot::freePtr(p, mCount * sizeof(T));
		mCount = num;
	}


public:
	void push_back(const T &val, T def)
	{
		size_t nowCount = mCount;
		if (nowCount >= mSize)
		{
			size_t i = mSize;
			if (mSize <= 0)
				mSize = INIT_COUNT;
			else
				mSize *= 2;
			this->_resize(mSize);
			for (; i < mSize; ++i)
			{
				mpArrayPtr[i] = def;
			}
		}
		mpArrayPtr[nowCount] = val;
		mCount = nowCount + 1;
	}

	void push_back(const T &val)
	{
		size_t nowCount = mCount;
		if (nowCount >= mSize)
		{
			if (mSize <= 0)
				mSize = INIT_COUNT;
			else
				mSize *= 2;
			this->_resize(mSize);
		}
		mpArrayPtr[nowCount] = val;
		mCount = nowCount + 1;
	}

	void pop_back()
	{
		if (!empty())
		{
			__Free(mpArrayPtr[--mCount]);
			memset(mpArrayPtr + mCount, 0, sizeof(T));
		}
	}

	bool remove(size_t pos)
	{
		if (pos < size())
		{
			__Free(mpArrayPtr[pos]);

			memmove(mpArrayPtr + pos, mpArrayPtr + pos + 1, (size() - (pos + 1)) * sizeof(T));
			--mCount;
			// 将空出的最后一个位置初始为0
			memset(mpArrayPtr+mCount, 0, sizeof(T));
			__Set(mpArrayPtr[mCount]);

			return true;
		}
		return false;
	}

	bool remove(size_t pos, T def)
	{
		if (pos < size())
		{
			__Free(mpArrayPtr[pos]);

			memmove(mpArrayPtr + pos, mpArrayPtr + pos + 1, (size() - (pos + 1)) * sizeof(T));
			--mCount;
			// 将空出的最后一个位置初始为0
			memset(mpArrayPtr + mCount, 0, sizeof(T));
			__Set(mpArrayPtr[mCount]);
			mpArrayPtr[mCount] = def;
			return true;
		}
		return false;
	}

	void clear(bool bFreeBuffer = true)
	{
		if (mpArrayPtr)
		{
			if (bFreeBuffer)
			{
				for (size_t i = 0; i < mSize; ++i)
				{
					__Free(mpArrayPtr[i]);
				}
				Allot::freePtr(mpArrayPtr, sizeof(T)*mSize);
				mpArrayPtr = NULL;
				mSize = 0;
			}
			else
			{
				for (size_t i = 0; i < mSize; ++i)
				{
					mpArrayPtr[i] = T();
				}
			}
		}
		mCount = 0;
	}

	void resize(size_t num)
	{
		if (mCount == num)
			return;

		size_t nowCount = mCount;

		if (mSize < num)
		{
			_resize(num);
			mSize = num;
		}
		else if (nowCount > num)
		{
			T def = T();
			for (size_t i = num; i < nowCount; ++i)
			{
				mpArrayPtr[i] = def;
			}
		}
		// nowCount小于num且mSize>num, 此时,理论上已经在分配时进行了初始, 所以不再初始
		//else 
		//{
		//	T def = T();
		//	for (size_t i=nowCount; i<num; ++i)
		//	{
		//		mpArrayPtr[i] = def;
		//	}
		//}		
		mCount = num;
	}

	void resize(size_t num, T def)
	{
		if (mCount == num)
			return;

		size_t nowCount = mCount;

		if (mSize < num)
		{
			size_t i = mSize;
			_resize(num);
			mSize = num;
			for (; i < mSize; ++i)
			{
				mpArrayPtr[i] = def;
			}
		}
		else if (nowCount > num)
		{
			for (size_t i = num; i < nowCount; ++i)
			{
				mpArrayPtr[i] = def;
			}
		}
		else
		{
			for (size_t i = nowCount; i < num; ++i)
			{
				mpArrayPtr[i] = def;
			}
		}
		mCount = num;
	}

	void swap(tArray &other)
	{
		size_t nTemp = mCount;
		T *p = mpArrayPtr;

		mCount = other.mCount;
		mpArrayPtr = other.mpArrayPtr;

		other.mCount = nTemp;
		other.mpArrayPtr = p;

		size_t tempSize = mSize;
		mSize = other.mSize;
		other.mSize = tempSize;
	}

	size_t _useSpaceSize() const
	{
		return sizeof(tArray) + sizeof(T)*mSize;
	}

protected:
	_SIZE_TYPE		mSize;
	_SIZE_TYPE		mCount;
	T						*mpArrayPtr;
};
//-------------------------------------------------------------------------*/
template<typename T, int INIT_COUNT = 8, typename _SIZE_TYPE = unsigned int>
class Array : public tArray<T, INIT_COUNT, _SIZE_TYPE>
{
	typedef  tArray<T, INIT_COUNT, _SIZE_TYPE> Base;
public:
	Array()
	{
		
	}

	Array(size_t initSize)
		: Base(initSize)
	{
		
		Base::resize(initSize, _getStatic());
	}

	Array(const Array &other)
	{		
		*this = other;
	}

	~Array()
	{
		Base::clear(true);
	}
	
	bool empty() const { return Base::empty(); }
	DSIZE size() const { return Base::size(); }

public:
	T& back()
	{
		if (empty())
		{
			AssertEx(0, "数组为空,取最后元素失败");
			return _getStatic();
		}
		return Base::mpArrayPtr[Base::mCount - 1];
	}
	const T& back()const
	{
		if (!empty())
			return Base::mpArrayPtr[Base::mCount - 1];
		AssertNote(0, "数组为空,取最后元素失败");
		return T();
	}

	T & operator [] (size_t pos)
	{		
#if ARRAY_AUTO_SIZE
		if (pos >= Base::mSize)
		{
			size_t size = pos + 1;
			size = size > 8 ? GET_BLOCK_SIZE(size) : 8;
			Base::resize(size);
			Base::mCount = pos + 1;
		}
		else if (pos >= Base::mCount)
			Base::mCount = pos + 1;
#else
		if (pos >= Base::mCount)
		{
			AssertNote(0, "错误:使用数组越界");
			return _getStatic();
		}
#endif			

		return Base::mpArrayPtr[pos];
	}

	const T & operator [] (size_t pos) const
	{
		if (pos >= Base::mCount)
		{
			AssertEx(0, "错误:使用数组越界");
			return _getStatic();
		}
		return Base::mpArrayPtr[pos];
	}

	Array& operator = (const Array &other)
	{
		if (&other == this)
			return *this;
		Base::clear(false);
		Base::resize(other.size());
		for (size_t i = 0; i < other.size(); ++i)
		{
			Base::mpArrayPtr[i] = other.mpArrayPtr[i];
		}
		return *this;
	}

	T& _getStatic() const
	{
		_THREAD_LOCAL_ static T ms = T();
		return ms;
		//return mStatic;
	}
};



//-------------------------------------------------------------------------*/
#pragma pack(pop)

#endif //_INCLUDE_ARRAY_H_