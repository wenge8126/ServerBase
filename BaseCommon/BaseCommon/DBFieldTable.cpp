#include "DBFieldTable.h"
#include "TableUpdateLoader.h"
//#include "MemoryDBRecord.h"
#include "IntEasyMapIndex.h"

AutoTable ExDBTableFieldInfo::CreateFieldTable()
{
	if (!mDBTable)
		return AutoTable();
	AutoTable t = MEM_NEW DBFieldTable(this);
	t->InitField(mDBTable->GetField());
	return t;
}

bool ExDBTableFieldInfo::SaveDataToDB(tBaseTable *pScrTable, DataStream *pDBData)
{
    DSIZE pos = pDBData->tell();
    int count = 0;
    pDBData->write(count);
    for (auto it = pScrTable->GetRecordIt(); *it; ++(*it))
    {
        AString key;
        if( it->GetKey(key) )
        {
			if (mbKeyIsInt64)
				pDBData->write(TOINT64(key.c_str()));
			else
				pDBData->write(TOINT(key.c_str()) );
            ++count;
        }
        else
            ERROR_LOG("[%s] record get key [%s] error", getName().c_str(), it->Record()[0].string().c_str());
    }
    DSIZE lastPos = pDBData->tell();
    pDBData->seek(pos);
    pDBData->write(count);
    pDBData->seek(lastPos);

    return true;
}

bool ExDBTableFieldInfo::RestoreFromDB(DataStream *pScrDBData, tBaseTable *destTable)
{
    if (!mDBTable)
    {
        ERROR_LOG("Sub DB %s no exit", mDBTableName.c_str());
        return false;
    }
    
    pScrDBData->seek(0);
    int count =0; 
    if (!pScrDBData->read(count))
        return false;
    if (count>0)
    {
        if (!destTable)
        {
            destTable = MEM_NEW DBFieldTable(this);
            destTable->InitField(mDBTable->GetField());               
        }
        else
            destTable->ClearAll();
        for (int i=0; i<count; ++i)
        {
            Int64 key = 0;
			if (mbKeyIsInt64)
			{
				if (!pScrDBData->read(key))
					return false;
			}
			else
			{
				int k = 0;
				if (!pScrDBData->read(k))
					return false;
				key = k;
			}

            ARecord r = mDBTable->GetMainIndex()->GetRecord(key);
            if (r.getPtr()!=NULL)
			{
				// 直接加载未完全加载的冷记录
				if (mbKeyIsInt64)
				{
					Auto<Int64MapIndex> index = destTable->GetMainIndex();
					index->_insertRecord((int)key, r);
				}
				else
				{
					Auto<IntEasyMapIndex> index = destTable->GetMainIndex();
					index->_insertRecord((int)key, r);
				}				
			}
        }
    }
    return true;
}

bool DBFieldTable::AppendRecord(ARecord scrRecord, bool bReplace, bool bRestore /*= false*/)
{
    if (bRestore)
    {
        if (mpDBTableFieldInfo->mDBTable)
        {
            ARecord r = mpDBTableFieldInfo->mDBTable->GetRecord(scrRecord[0].string());                

            if (mpDBTableFieldInfo->mDBTable->AppendRecord(scrRecord, true, false))
            {
                //Auto<MemoryDBRecord> newRe = scrRecord;
                // 只要是不存在就设置为新插入
                scrRecord->SetNewInsert(!r);
                mbChanged = true;
                return NormalBaseTable::AppendRecord(scrRecord, true, false);
            }
        }
        ERROR_LOG("DB field DB table is NULL");
    }
    else
        return NormalBaseTable::AppendRecord(scrRecord, bReplace, false);

    return false;
}
