#ifndef _INCLUDE_RECORDTOOL_H_
#define _INCLUDE_RECORDTOOL_H_

#include "ArrayList.h"

template <typename T>
class DBArray : public ArrayList<T>
{
public:
    virtual ~DBArray(){}
};

template <typename T>
class SaveDBArray : public DBArray<T>
{
	typedef DBArray<T> Base;
public:
	Auto<BaseRecord> re;
	int nCol;

	SaveDBArray()
		: nCol(-1) {}

	~SaveDBArray()
	{
		if (re && nCol >= 0)
		{
			AutoData data = re->getDataBuffer(nCol);
			if (!data)
				data = MEM_NEW DataBuffer(64);
			data->seek(0);
			data->writeArray(*this);
			re->set(nCol, &data, typeid(data));
		}
	}

    void moveLastToFrist()
    {
        if (Base::mCount>1)
        {
            T v = Base::mpArrayPtr[Base::mCount - 1];
            __Free(Base::mpArrayPtr[Base::mCount - 1]);
            memmove(Base::mpArrayPtr + 1, Base::mpArrayPtr, (Base::mCount - 1) * sizeof(T));
            __Set(Base::mpArrayPtr[0]);
			Base::mpArrayPtr[0] = v;
        }
    }
};

#endif //_INCLUDE_RECORDTOOL_H_

