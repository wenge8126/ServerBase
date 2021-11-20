
#include "TableFieldInfo.h"
#include "Common.h"
#include "TableManager.h"
#include "IndexBaseTable.h"
#include "TableUpdateLoader.h"
#include "MemoryTableFieldIndex.h"
//bool TableFieldInfo::saveData( void *data, DataStream *destData ) const
//{
//	AutoTable temp;
//	if (base::get(data, &temp, typeid(AutoTable)))
//    {
//        if (temp)
//	    {
//            destData->write(true);
//		    AString type = temp->GetTypeName();
//		    destData->writeString(type.c_str());
//		    //DataBuffer tempData;
//		    size_t size = temp->_SerializeStream(destData);
//		    return size>0; 
//        }
//        else
//            destData->write(false);
//
//        return true;
//	}
//	return false;
//}
//
//bool TableFieldInfo::restoreData( void *data, DataStream *scrData )
//{
//    bool bHave = false;
//    if (scrData->read(bHave))
//    {
//        if (bHave)
//        {
//            AString type;
//            if (scrData->readString(type))
//            {
//                AutoTable temp = mConfigManager.CreateNewObject(type.c_str()) ; 
//                if (temp)		
//                    if (temp->_RestoreFromeStream(scrData))
//                        return base::set(data, &temp, typeid(AutoTable));		
//            }
//        }
//        AutoTable  t;
//        base::set (data, &t, typeid(AutoTable));
//        return true;
//    }
//    return false;
//}
//
//bool TableFieldInfo::set( void *data, const AString &strVal )
//{ 
//	return set(data, strVal.c_str()); 
//}
//
//bool TableFieldInfo::set( void *data, const char* szVal )
//{
//	AutoTable temp;
//	if (!base::get(data, &temp, typeid(AutoTable)) || !temp)
//	{
//		if (!mSubTableField)
//		{
//			DEBUG_LOG("ERROR: No exist field when restore form string");
//			return false;
//		}
//		temp = MEM_NEW NiceTable(mSubTableField);
//		base::set(data, &temp, typeid(AutoTable));
//	}
//
//	AString tempScr = szVal;
//
//	while (true)
//	{
//		AString recordData = tempScr.SplitBlock();
//		if (recordData.empty() || recordData=="")
//			break;
//
//		AString key = recordData.SplitBlock('[', ']');
//
//		if (key.empty())
//			return true;
//
//		ARecord r = temp->CreateRecord(key);
//
//		if (!r->FullFromString(recordData, false))
//		{
//			DEBUG_LOG("WARN: full data from string FAIL, key = [%s]", key.c_str());
//		}
//	}
//	return true;
//}
//
//bool TableFieldInfo::get( void *data, AString &resultString ) const
//{
//	AutoTable temp;
//	if (base::get(data, &temp, typeid(AutoTable)) && temp)
//	{
//		if (mSubTableField && temp->GetField()!=mSubTableField)
//		{
//			DEBUG_LOG("WARN: FIELD [%s] table field is not same of now value table", getName().c_str());
//		}
//		for (TableIt hIt(temp); hIt._Have(); hIt._Next())
//		{
//			ARecord r = hIt.getCurrRecord();
//			if (r)
//			{
//				AString recordData;
//				if (r->ToString(recordData, true))
//				{
//					resultString += "{";
//					resultString += recordData;
//					resultString += "}";
//				}
//				else
//				{
//					DEBUG_LOG("WARN: Record to string fail");
//				}
//			}
//		}
//	}
//	return true;
//}
//
//AString TableFieldInfo::getTypeParam() const
//{
//	if (mSubTableField)
//	{
//		return mSubTableField->ToString();
//	}
//	return AString();
//}
//
//void TableFieldInfo::setTypeParam( const AString &typeParam )
//{
//	if (typeParam.empty() || typeParam=="")
//		return;
//
//	mSubTableField = MEM_NEW FieldIndex(NULL, 0);
//	if (!mSubTableField->FullFromString(typeParam))
//	{
//		DEBUG_LOG("ERROR: [%s] Restore field from string fail", getName().c_str());
//	}
//}
//
//void TableFieldInfo::CopyData( const tFieldInfo *pScrFieldInfo )
//{
//	base::CopyData(pScrFieldInfo);
//	const TableFieldInfo *p = dynamic_cast<const TableFieldInfo*>(pScrFieldInfo);
//	if (p!=NULL)
//	{
//		mSubTableField = p->mSubTableField;
//	}
//}


//-------------------------------------------------------------------------
// 用于内存 DB
// 字段信息只保存校验码
//-------------------------------------------------------------------------

AString DBTableFieldInfo::getTypeParam() const
{
	AString str = STRING((int)mMaxLength);		
	if (mSubTableField)
	{
		str += " ";
		str += mSubTableField->ToString();
	}
	return str;
}

void DBTableFieldInfo::setTypeParam( const AString &typeParam )
{
	if (typeParam.empty() || typeParam=="")
	{
		return;
	}
	AString str  = typeParam;
	AString lenString = str.SplitLeft(" ");
	if (lenString.empty())
		lenString.swap(str);
	int len = TOINT(lenString.c_str());
	{
		if (len>0)
			mMaxLength = len;
	}

	if (str.length()>0)
	{	
		mSubTableField = MEM_NEW MemoryTableFieldIndex(NULL);
		if (!mSubTableField->FullFromString(str))
		{
			ERROR_LOG("ERROR: [%s] Restore field from string fail", getName().c_str());
		}
	}
}

void DBTableFieldInfo::CopyData( const tFieldInfo *pScrFieldInfo )
{
	base::CopyData(pScrFieldInfo);
	const DBTableFieldInfo *p = dynamic_cast<const DBTableFieldInfo*>(pScrFieldInfo);
	if (p!=NULL)
	{
		mSubTableField = p->mSubTableField;
	}
}

bool DBTableFieldInfo::saveData( void *data, DataStream *destData ) const
{
	AutoTable temp;
	if (base::get(data, &temp, typeid(AutoTable)))
	{
		if (temp)
		{
			destData->write((byte)_SubTableDefault);
			destData->write(temp->GetField()->GetCheckCode());
			TableDataLoader loader;
			if ( !temp->Save(&loader, destData) )
				return false;			
		}
		else
			destData->write((byte)_SubTableNull);

		return true;
	}
	return false;
}

bool DBTableFieldInfo::restoreData( void *data, DataStream *scrData )
{
	byte bHave = 0;
	if (scrData->read(bHave))
	{
		if (bHave==_SubTableDefault)
		{
			int checkCode = 0;
			if (!scrData->read(checkCode))
			{
				ERROR_LOG("读取字段验证失败");
				return false;
			}
			if (mSubTableField && !mSubTableField->CheckSame(checkCode))
			{
				DEBUG_LOG("WARN: 字段校验失败 now [%d] of [%d]", checkCode, mSubTableField->GetCheckCode());
				//return false;
			}

			AutoTable temp;
			if (!base::get(data, &temp, typeid(AutoTable)))
				return false;

			if (!temp)
			{
				temp = CreateFieldTable(); //tBaseTable::NewFieldTable(false); 
				//temp->InitField(mSubTableField);

				if (!base::set(data, &temp, typeid(AutoTable) ))
					return false;
			}
			TableDataLoader loader;
			if (!temp->Load(&loader, scrData))
				return false;			
			//else if (!mSubTableField)
			//{
			//	mSubTableField = temp->GetField();
			//	mSubTableField->_updateInfo();
			//}
		}
		else if (bHave!=_SubTableNull)
		{
			ERROR_LOG("Restore type error >%d", (int)bHave);
			return false;
		}
		return true;
	}
	return false;
}

bool DBTableFieldInfo::set( void *data, const char* szVal )
{
	AutoTable temp;
	if (!base::get(data, &temp, typeid(AutoTable)) || !temp)
	{
		if (!mSubTableField)
		{
			DEBUG_LOG("ERROR: No exist field when restore form string");
			return false;
		}
		temp = tBaseTable::NewBaseTable();
		temp->InitField(mSubTableField);
		base::set(data, &temp, typeid(AutoTable));
	}

	AString tempScr = szVal;

	while (true)
	{
		AString recordData = tempScr.SplitBlock();
		if (recordData.empty() || recordData=="")
			break;

		AString key = recordData.SplitBlock('[', ']');

		if (key.empty())
			return true;

		ARecord r = temp->CreateRecord(key.c_str(), true);

		if (!r->FullFromString(recordData.c_str(), false))
		{
			DEBUG_LOG("WARN: full data from string FAIL, key = [%s]", key.c_str());
		}
	}
	return true;
}

bool DBTableFieldInfo::get( void *data, AString &resultString ) const
{
	AutoTable temp;
	if (base::get(data, &temp, typeid(AutoTable)) && temp)
	{
		if (mSubTableField && temp->GetField()!=mSubTableField)
		{
			DEBUG_LOG("WARN: FIELD [%s] table field is not same of now value table", getName().c_str());
		}
		for (ARecordIt hIt = temp->GetRecordIt(); *hIt; ++(*hIt))
		{
			ARecord r = *hIt;
			if (r)
			{
				AString recordData;
				if (r->ToString(recordData, true))
				{
					resultString += "{";
					resultString += recordData;
					resultString += "}";
				}
				else
				{
					DEBUG_LOG("WARN: Record to string fail");
				}
			}
		}
	}
	return true;
}

bool DBTableFieldInfo::saveField( DataStream *destData ) const
{
	base::saveField(destData);
	destData->write(getMaxLength());
	if (mSubTableField)
	{
		destData->write(true);
		return mSubTableField->saveToData(destData);
	}
	else
		return destData->write(false);

	return true;
}

bool DBTableFieldInfo::restoreFromData( DataStream *scrData )
{
	if (base::restoreFromData(scrData))
	{
		if (!scrData->read(mMaxLength))
		{
			DEBUG_LOG("读取表格字段长度失败");
			mMaxLength = DB_BLOB_FIELD_DEFAULT_LENGTH;
			return false;
		}
		bool bHave = false;
		if (!scrData->read(bHave))
			return false;

		if (bHave)
		{
			AutoField newField = MEM_NEW BaseFieldIndex();
			if (newField->restoreFromData(scrData))
			{
				//if (!mSubTableField || !(*mSubTableField==*newField))
				mSubTableField = newField;
				newField->_updateInfo();
				return true;
			}
			ERROR_LOG("ERROR: restore sub field fail");
			return false;
		}
		return true;
	}
	return false;
}

AutoTable DBTableFieldInfo::CreateFieldTable()
{
	AutoTable t = tBaseTable::NewFieldTable(false);
	t->InitField(mSubTableField);
	return t;
}
