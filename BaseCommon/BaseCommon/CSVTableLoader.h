#ifndef _INCLUDE_CSVTABLELOADER_H_
#define _INCLUDE_CSVTABLELOADER_H_

#include "BaseTable.h"

//-------------------------------------------------------------------------*/
// CSV ����д
// NOTE: Ŀǰ������Window, ��д���� ANIS > UTF8 > ANIS �ַ���ת��
//-------------------------------------------------------------------------*/

class BaseCommon_Export CSVTableLoader : public TableLoader
{
public:
	static bool SaveCSV(tBaseTable *pTable, const char *szCSVFileName, const char *szIndexField = NULL);

public:
	virtual bool SaveTable(tBaseTable *pTable, DataStream *resultData, const char *szIndexField = NULL);

	virtual bool LoadTable(tBaseTable *pTable, DataStream *scrData);
	 
	virtual bool LoadFromData(tBaseTable *pTable, DataStream *scrStream);
};

#define TXT_TABLE_DELIMITED_STRING      "\t"

class BaseCommon_Export TxtTableLoader : public CSVTableLoader
{
public:
    virtual bool SaveTable(tBaseTable *pTable, DataStream *resultData, const char *szIndexField = NULL);
    virtual bool LoadFromData(tBaseTable *pTable, DataStream *scrStream);
};

#endif //_INCLUDE_CSVTABLELOADER_H_