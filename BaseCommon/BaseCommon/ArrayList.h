#ifndef _INCLUDE_ARRAYLIST_H_
#define _INCLUDE_ARRAYLIST_H_

#include "Array.h"
//-------------------------------------------------------------------------*
// 数组方式实现的列表，使用于频繁增加删除的列表，但不要求元素顺序的列表
// 快速删除和增加，删除时直接将最后元素移到到删除的位置，增加只增加到最后
//-------------------------------------------------------------------------*
template<typename T, int INIT_COUNT = 8>
class ArrayList : public Array<T, INIT_COUNT>
{
	typedef Array<T, INIT_COUNT> Base;
public:
	DSIZE size() const { return Base::size(); }

public:
	void add(const T &value)
	{
		Base::push_back(value);
	}

	bool removeAt(size_t pos)
	{
		if (pos>=0 && pos< size()-1)
		{
			Base::mpArrayPtr[pos] = Base::mpArrayPtr[size()-1];
			Base::mpArrayPtr[size()-1] = T();
			--Base::mCount;
			return true;
		}
		else if (pos==size()-1)
		{
			Base::mpArrayPtr[size()-1] = T();
			--Base::mCount;
			return true;
		}
		return false;
	}

	bool remove(const T &value)
	{
		for (size_t i=0; i<size(); ++i)
		{
			if (Base::mpArrayPtr[i]==value)
			{
				return removeAt(i);
			}
		}
		return false;
	}

	bool removeByPos(size_t pos)
	{
		return Base::remove(pos);
	}

	int find(const T &value)
	{
		for (size_t i = 0; i < size(); ++i)
		{
			if (Base::mpArrayPtr[i] == value)
			{
				return i;
			}
		}

		return -1;
	}

	T& find(size_t pos)
	{
		if (pos < size())
			return (*this)[pos];
		thread_local T msValue;
		return msValue;
	}

	T* findPtr(size_t pos)
	{
		if (pos < size())
			return &(*this)[pos];
		return NULL;
	}
	 
	T& push()
	{
		return (*this)[size()];
	}
};
//-------------------------------------------------------------------------*

#endif //_INCLUDE_ARRAYLIST_H_