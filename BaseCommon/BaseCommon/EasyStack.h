#ifndef _INCLUDE_EASYSTACK_H_
#define _INCLUDE_EASYSTACK_H_

#include "Array.h"

#define  STACK_INIT_COUNT	16

template<typename T, typename _SIZE_TYPE = unsigned int>
class EasyStack : MemBase
{
public:
	EasyStack()
		//: mCount(0)
	{
		//msStatic = T();
		mData.resize(STACK_INIT_COUNT);
		mData.clear(false);
	}
	~EasyStack()
	{
		clear();
	}

public:
	void push(const T& val)
	{
		mData.push_back(val);
		//if (mCount>=mData.size())
		//	mData.resize(mData.size()*2);
		//mData[mCount++] = val;
	}

	T pop(void)
	{
		if (!mData.empty())
		{
			T temp = mData.back();
			mData.pop_back();
			return temp;
		}
		return T();
	}
	void clear()
	{
		mData.clear(false);
		//for (size_t i=0; i<mCount; ++i)
		//{
		//	mData[i] = T();
		//}
		//mData.resize(STACK_INIT_COUNT);
		//mCount = 0;
	}

	bool empty(){ return mData.empty(); }
	DSIZE size(){ return mData.size(); }

	//T& operator [] (size_t pos)
	//{
	//	if (pos<mCount)
	//		return mData[pos];

	//	//static T s;
	//	return msStatic;
	//}

	void swap(EasyStack &other)
	{
		mData.swap(other.mData);
		//size_t t = mCount;
		//mCount = other.mCount;
		//other.mCount = t;
	}

	Array<T, 8, _SIZE_TYPE>& _getList(){ return mData; }

protected:
	Array<T, 8, _SIZE_TYPE>		mData;
	//size_t		mCount;

	//T			msStatic;
};


#endif //_INCLUDE_EASYSTACK_H_