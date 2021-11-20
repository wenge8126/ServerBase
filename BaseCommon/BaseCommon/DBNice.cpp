
#include "DBNice.h"
#include "BaseRecord.h"
#include "IDNiceData.h"

AData DBNice::msAData;

DBNice::~DBNice()
{
    
}

DBNice::DBNice(const DBNice &other)
{
    mpNice = other.mpNice;
    mpRecord = other.mpRecord;
    mFieldCol = other.mFieldCol;
    other.mpNice = NULL;
    other.mFieldCol = 0;
    other.mpRecord = NULL;
}


void DBNice::Save()
{
    if (mpNice && mpRecord != NULL)
    {
        mpRecord->OnChangedNiceData(mFieldCol);
    }
}

AData& DBNice::operator [](const char *szKey)
{
    if (mpNice != NULL)
        return ((IDNiceData*)mpNice)->getOrCreate(szKey);
    return msAData;
}