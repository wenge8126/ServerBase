#ifndef _INCLUDE_ARRAYINDEX_H_
#define _INCLUDE_ARRAYINDEX_H_

#include "Array.h"
#include "Common.h"

#define ARRAY_INDEX_MAX_ID		0XFFFF
//-------------------------------------------------------------------------*/
// �����������
// NOTE: ���븲��Array::remove����ΪArray::remove���ƶ��±��Ӧ��ֵ
//-------------------------------------------------------------------------*/
template<typename T>
class ArrayIndex : public Array<T*>
{
	typedef Array<T*> Base;
public:
	// NOTE: Alway delete already val
	void Append(int id, T *pVal)
	{
		if (id>ARRAY_INDEX_MAX_ID)
		{
			ERROR_LOG("�������������������ID [%d]", ARRAY_INDEX_MAX_ID);
			return;
		}

		if (id >= (int)Base::mSize)
		{
			Base::resize(id + 8);
			Base::mCount = id + 1;
		}
		else
		{
			if (id >= (int)Base::mCount)
				Base::mCount = id + 1;
			if (Base::mpArrayPtr[id] != NULL && Base::mpArrayPtr[id] != pVal)
			{
				delete Base::mpArrayPtr[id];
			}
		}
		Base::mpArrayPtr[id] = pVal;
	}

	int Append(T *pVal)
	{
		Base::push_back(pVal);
		return Base::size()-1;
	}

	T* Find(int id)
	{
		if (id>=0 && id< Base::size())
			return Base::mpArrayPtr[id];

		return NULL;
	}

	// NOTE: ���븲�Ǵ˹��ܣ�Array::remove ���ƶ��±��Ӧ��ֵ
	bool remove(size_t pos)
	{
		if (pos>=0 && pos< Base::size())
		{
			bool b = Base::mpArrayPtr[pos]!=NULL;
			if (b)
				delete Base::mpArrayPtr[pos];
			Base::mpArrayPtr[pos] = NULL;
			return b;
		}
		return false;
	}

public:
	~ArrayIndex()
	{
		clear(true);
	}

	void clear(bool bFreeBuffer = true)
	{
		for (size_t i=0; i< Base::size(); ++i)
		{
			if (Base::mpArrayPtr[i]!=NULL)
				delete Base::mpArrayPtr[i];
		}
		Base::clear(bFreeBuffer);
	}
};

template <typename T>
class ArrayMap : public Array<T>
{
	typedef Array<T> Base;
public:
	T& operator [] (int id) { return Find(id); }
	const T& operator [] (int id) const
	{
		if (id>=0 && id< Base::size())
			return Base::mpArrayPtr[id];

		return Base::_getStatic();
	}

public:
	void Append(int id, const T &val)
	{
		AssertEx(id>=0, "ArrayMap must id>=0 ");
		if (id >= (int)Base::mSize)
		{
			Base::resize(id + 8);
			Base::mCount = id + 1;
		}
		else	if (id >= (int)Base::mCount)
				Base::mCount = id + 1;
		Base::mpArrayPtr[id] = val;
	}

	int Append(const T &val)
	{
		Base::push_back(val);
		return Base::size() - 1;
	}

	T& Find(int id)
	{
		if (id>=0 && id< Base::size())
			return Base::mpArrayPtr[id];

		return Base::_getStatic();
	}

    T* FindPtr(int id)
    {
        if (id>=0 && id< Base::size())
            return &(Base::mpArrayPtr[id]);

        return NULL;
    }

	// NOTE: ���븲�Ǵ˹��ܣ�Array::remove ���ƶ��±��Ӧ��ֵ
	bool remove(size_t pos)
	{
		if (pos>=0 && pos< Base::size())
		{
			Base::mpArrayPtr[pos] = T();
			return true;
		}
		return false;
	}
};


#endif //_INCLUDE_ARRAYINDEX_H_