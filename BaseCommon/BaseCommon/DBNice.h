#pragma once
#ifndef _INCLUDE_DBDATA_H_
#define _INCLUDE_DBDATA_H_

#include "NiceData.h"
#include "AData.h"
//-------------------------------------------------------------------------*
class tNiceData;
struct BaseCommon_Export DBNice
{
    friend class BaseRecord;
protected:
    mutable tNiceData   *mpNice;
    mutable BaseRecord *mpRecord;
    mutable int mFieldCol;

    static AData msAData;

public:
    ~DBNice();

    DBNice()
        : mpRecord(NULL)
        , mpNice(NULL)
        , mFieldCol(0)
    {}

    DBNice(const DBNice  &other);

    DBNice & operator =(const DBNice  &other)
    {
        mpNice = other.mpNice;
        mFieldCol = other.mFieldCol;
        mpRecord = other.mpRecord;
        other.mpNice = NULL;
        mpRecord = NULL;
        mFieldCol = 0;
        return *this;
    }

    // 保存更新到DB
    void Save();

    tNiceData* GetNice() { return mpNice; }

    AData& operator [](const AString szKey)
    {
        if (mpNice!=NULL)
            return mpNice->getOrCreate(szKey.c_str());
        return msAData;
    }

    AData& operator [](const char *szKey);

    AData& operator [](int nkey)
    {
        if (mpNice != NULL)
            return mpNice->getOrCreate(nkey);
        return msAData;
    }
};


//-------------------------------------------------------------------------*

#endif //_INCLUDE_DBDATA_H_
