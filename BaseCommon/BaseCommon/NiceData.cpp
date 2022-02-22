

#include "NiceData.h"
#include "Common.h"
#include "DataBuffer.h"

#include <stdarg.h>
#include "BaseTable.h"
#include "IDNiceData.h"

#if USE_JSON_LIB
#	include "json.h"
#endif
//----------------------------------------------------------------
#if __LINUX__
#define _atoi64(val)     strtoll(val, NULL, 10)
#endif

//----------------------------------------------------------------
const char spaceString[] = "        ";

	const AString& tNiceData::iterator::key()
	{
		if (IS_ARRAY)
		{
			thread_local static AString msKey;
			if (mCurrentPos < mpDataList->size())
				msKey = STRING(mCurrentPos);
			else
				msKey = STRING(-1);
			return msKey;
		}
		else if (IS_NICE)
			return mHashIt.key();
		else
		{
			thread_local static AString msKey;
			msKey = TableManager::getSingleton().mStringKeyIndex.KeyString(mIDMapIt.key());
			if (msKey.empty())
			{
				WARN_LOG("IDNiceData unkwon key string, now return temp key ID string");
				msKey = (STRING(mIDMapIt.key()));
			}
			return msKey;
		}
	}

//----------------------------------------------------------------
tNiceData::iterator::iterator(NiceData *mpNice) : mCurrentPos(0)
	, mpDataList(NULL)
	, mNiceType(mpNice->getType())
{

	mHashIt = mpNice->mDataMap.begin();
}

tNiceData::iterator::iterator(ArrayNiceData *mpNice) : mCurrentPos(0)
	, mpDataList(&mpNice->mDataList)
	, mNiceType(mpNice->getType())
{

}

tNiceData::iterator::iterator(IDNiceData *pIDNice) 
	: mNiceType(pIDNice->getType())
	, mCurrentPos(0)
{
	mIDMapIt = pIDNice->mDataMap.begin();
}


//----------------------------------------------------------------
//----------------------------------------------------------------



Data tNiceData::get(const char* key) const
{
	AData &d = getAData(key);	

    Data temp(&d);

    return temp;
}

bool tNiceData::get( const char* key, void *obj, const type_info &typeInfo ) const
{
	AData &d = getAData(key);
	return d.get(obj, typeInfo);
}

//-------------------------------------------------------------------------*/
void tNiceData::initData()
{
	//mDataMap.clear(false);
	//int sum = count();
	//for (size_t i=0; i<sum; ++i)
	//{
	//	get(i).free();
	//}

	for (auto it=begin(); it.have(); it.next())
	{
		it.get().free();
	}
}


AString tNiceData::dump( int sub, int code ) const
{	
	AString temp = "\r\n";
    if (sub==0)
    {
        for (int n=0; n<sub; ++n)
        {
            temp += spaceString;
        }
        temp += "_____________________________________________\r\n";		
    }
	if (msKey.length() > 0)
	{
		temp += msKey;
		temp += "\r\n";
	}
	int i=0;
	for (auto it=begin(); it.have(); it.next(), ++i)
	{
		AData &d = it.get();
		AString key = it.key();
		for (int n=0; n<sub; ++n)
		{
			temp += spaceString;
		}
		//temp += "[";
		temp += key;
		//temp += "]";
		temp += " : ";
		temp += d.empty() ? "" : d._getField()->getTypeString();
		temp += " = ";
		if (d.empty())
			temp += "NULL(WARN)";
		else
		{		
			FieldInfo f = d._getField();
			if (f->getType()==FIELD_NICEDATA || f->getType()==FIELD_ARRAYDATA)
			{
				AutoNice subData;
                if (f->getType()==FIELD_NICEDATA)
				    d.get(&subData, typeid(subData));
                else
                {
                    AutoArrayData dV;
                    d.get(&dV, typeid(dV));
                    subData = dV;
                }
				if (subData)
				{
                    if (subData->getType()== NICEDATA)
					    temp += "Nice\r\n";
                    else
                        temp += "Array\r\n";
                    for (int n=0; n<sub; ++n)
                    {
                        temp += spaceString;
                    }
                    temp +="   {";
					temp += subData->dump(sub+1, (int)i);
                    for (int n=0; n<sub; ++n)
                    {
                        temp += spaceString;
                    }
					temp += "   }";
                    temp += key;
				}
				else
					temp += "NULL";
			}
			else
			{
				AString str;
				f->get(d._dataPtr(), str);
				if (f->getType()==FIELD_STRING)
#if __LINUX__
					temp += str;
#else
					temp += AString::getANIS(str.c_str());
#endif
				else
					temp += str;
				//if (d.getTypeInfo()->getType()==FIELD_TABLE)
				//{
				//	AutoTable hTable;
				//	d.get(&hTable, typeid(AutoTable));
				//	if (hTable)
				//	{
				//		AString t = "TableLog/CSV/";
				//		t += mName.c_str();
				//		t += "_";
				//		t += szName;
				//		t += ".csv";
				//		if (hTable->SaveCSV(t.c_str()))
				//		{
				//			temp += " This is a table, then saved to [";
				//			temp += t;
				//			temp += "]";
				//		}
				//		else
				//		{
				//			temp += " Warning: This is a table, but save fail.";
				//		}
				//	}
				//	else
				//	{
				//		temp += " Warning: This is a table type, but get table fail.";
				//	}
				//}
			}
		}
		temp += "\r\n";
	}
    for (int n=0; n<sub; ++n)
    {
        temp += spaceString;
    }
    if (sub<=0)
    {
        temp += "_____________________________________________________\r\n";
    }
    else
        temp +="\r\n";
	return temp;
}

bool tNiceData::existData( const char *keyList, AString *resultInfo )
{
	Array<AString>	stringList;
	AString::Split(keyList, stringList, " ", 0xFFFF);

	bool bAllExist = true;
	for (size_t i=0; i<stringList.size(); ++i)
	{
		if (!exist(stringList[i].c_str()))
		{
			if (resultInfo!=NULL)
			{
				if (resultInfo->length()>0)
					(*resultInfo) += ", ";

				(*resultInfo) += stringList[i];
			}
			bAllExist = false;
		}
	}
	return bAllExist;
}

bool tNiceData::FullJSON(AString scrJsonString)
{
	clear(false);

#if USE_JSON_LIB
	Json::Value jv;
	Json::Reader jsonReader;

	if (jsonReader.parse(scrJsonString.c_str(), scrJsonString.c_str() + scrJsonString.length(), jv))
	{
		return FullJSON("", &jv);
	}
	else
		ERROR_LOG("JSON parse error > %s", scrJsonString.c_str());

	return false;
#else

	if (scrJsonString=="{}")
		return true;
	AString dataString = scrJsonString.SplitBlock('{', '}');
	if (dataString.empty())
		return false;

	if (dataString=="")
		return true;

	while (true)
	{
		if (dataString.empty() || dataString=="")
			break;

		//if (*dataString.c_str()!='\"')
		//	return false;

		AString key = dataString.SplitBlock('\"', '\"');
		if (key.empty())
			return false;

		const char *sz = dataString.c_str();
		if (*sz!=':')
			return false;

		sz += 1;

		if (*sz == '[')
		{
			AString strValue = dataString.SplitBlock('[', ']', false);

			AutoNice niceData = MEM_NEW ArrayNiceData();
			niceData->FullJSON(strValue);
			if (!set(key.c_str(), niceData))
			{
				AssertEx(0, "设置错误");
				return false;
			}

			// 去掉后面的","
			dataString.SplitLeft(",");
			continue;
		}
		if (*sz=='{')
		{
			AString tempValue = dataString.SplitBlock('{', '}', true);
			//if (tempValue.empty())
			//	return false;

			AutoNice niceData = MEM_NEW NiceData();

			if (!niceData->FullJSON(tempValue))
				return false;

			if (!set(key.c_str(), niceData))
			{
				AssertEx(0, "设置错误");
				return false;
			}
			// 去掉后面的","
			dataString.SplitLeft(",");		
			continue;
		}	

		AString tempValue = dataString.SplitLeft(",");
		if (tempValue.empty())
		{
			tempValue = dataString;
			dataString.setNull();
			if (tempValue.empty())
				return false;
		}

		sz = tempValue.c_str();
		sz += 1;

		if (*sz=='\"')
		{
			AString v = tempValue.SplitBlock('\"', '\"');
			set(key.c_str(), v.c_str());
		}
		else
		{
			if (strstr(tempValue.c_str(), ".")!=NULL)
				set(key.c_str(), (float)atof(sz));
			else if (strlen(tempValue.c_str())>9)
				set(key.c_str(), (UInt64)_atoi64(sz));
			else
				set(key.c_str(), atoi(sz));
		}
	}

	return true;
#endif
}


bool tNiceData::FullByStringArray( AString scrStringArray )
{
	AString key;
	while (true)
	{
		if (key.empty())
		{
			key = scrStringArray.SplitBlock('"', '"');
			if (key.empty())
				return true;
		}
		else
		{
			AString val = scrStringArray.SplitBlock('"', '"');
			if (val.empty())
				return false;
			set(key.c_str(), val.c_str());
			key.setNull();
		}
	}

	return true;
}

bool tNiceData::SetStringArray( const char *szKey, StringArray &scrArray )
{
	AutoData d = MEM_NEW DataBuffer(128);
	if (d->writeStringArray(scrArray))
		return set(szKey, d);

	return false;
}

bool tNiceData::GetStringArray( const char *szKey, StringArray &resultArray )
{
	AutoData d;
	if (get(szKey, &d, typeid(d)) && d)
	{
		d->seek(0);
		return d->readStringArray(resultArray);
	}
	return false;
}

tBaseTable* tNiceData::getTable( const char *key )
{
	AutoTable t;
	get(key, &t, typeid(t));
	return t.getPtr();
}

//-------------------------------------------------------------------------*/

//-------------------------------------------------------------------------*/
#if !USE_IDNICEDATA

AData& NiceData::getOrCreate(const char* key)
{
	AData *p = (AData*)mDataMap.findPtr(key);
	if (p==NULL)
	{
		mDataMap.insert(key, AData());
		return *(AData*)mDataMap.findPtr(key);
	}
	return *p;
}
//----------------------------------------------------------------
#define AUTODATA_COUNT_TYPE		short
#define AUTODATA_COUNT_MAX		(0xEFFF)

bool NiceData::serialize(DataStream *destData) const
{
	if (destData->IsLoopBuffer())
	{
		int saveCount = 0;
		for (auto it = mDataMap.begin(); it.have(); it.next())
		{
			auto &val = it.Value();
			FieldInfo f = val.mVal._getField();
			if (!f || !f->needSaveToBuffer() || val.mVal.empty())
			{
				continue;
			}
			++saveCount;
		}

		if (saveCount > AUTODATA_COUNT_MAX)
		{
			ERROR_LOG("NiceData 数量[%llu]超过了 %d\r\n%s"
				, saveCount
				, (int)AUTODATA_COUNT_MAX
				, dump().c_str()
			);
			AssertEx(0, "NiceData %d count too most then %d", saveCount, AUTODATA_COUNT_MAX);
			return false;
		}

		// 避免使用tell, LoopStream 不支持 tell
		//size_t sizePos = destData->tell();

		destData->write((AUTODATA_COUNT_TYPE)saveCount);


		for (auto it = mDataMap.begin(); it.have(); it.next())
		{
			auto &val = it.Value();
			FieldInfo f = val.mVal._getField();
			if (!f || !f->needSaveToBuffer() || val.mVal.empty())
			{
				continue;
			}
			destData->writeString(val.mKey);
			if (!val.mVal.serialize(destData))
				return false;
		}
		return true;
	}

	if (count()>AUTODATA_COUNT_MAX)
	{
		ERROR_LOG("NiceData 数量[%llu]超过了 %d\r\n%s"
			, count()
			, (int)AUTODATA_COUNT_MAX
			, dump().c_str() 
			);
		AssertEx(0, "NiceData %d count too most then %d", (int)count(), AUTODATA_COUNT_MAX);
		return false;
	}

	AUTODATA_COUNT_TYPE s = (AUTODATA_COUNT_TYPE)count();

	size_t sizePos = destData->tell();
	destData->write(s);

	AUTODATA_COUNT_TYPE saveCount = 0;

	for (auto it = mDataMap.begin(); it.have(); it.next())
	{
		auto &val = it.Value();

		FieldInfo f = val.mVal._getField();
		if (!f || !f->needSaveToBuffer() || val.mVal.empty())
		{			
			continue;
		}
		++saveCount;		
		destData->writeString(val.mKey); 
		if (!val.mVal.serialize(destData))
			return false;
	}

	if (saveCount<s)
	{
		size_t nowPos = destData->tell();
		destData->seek(sizePos);
		destData->write(saveCount);
		destData->seek(nowPos);
	}

	return true;
}
//----------------------------------------------------------------

bool NiceData::restore(DataStream *scrData)
{
	mDataMap.clear(false);
	if (scrData->lastDataSize()<=0)
		return false;
	AUTODATA_COUNT_TYPE s = 0;

	if (!scrData->read(s))
		return false;

	for (size_t i=0; i<s; ++i)
	{
		AString str;
		if (!scrData->readString(str))
			return false;
		mDataMap.insert(str, AData());
		AData *p = (AData*)mDataMap.findPtr(str);
		if (!p->restore(scrData))
			return false;
	}

	return true;
}

size_t NiceData::append(const tNiceData &scrData, bool bReplace )
{
	if (dynamic_cast<const NiceData*>(&scrData)==NULL)
	{
		ERROR_LOG("NiceData append必须追加的源数据为 NiceData, now [%d]", scrData.getType());
		return 0;
	}
	size_t resultCount = 0;
	for (auto it=scrData.begin(); it.have(); it.next())
	{		
		const AString &key = it.key();
		const AData &d = it.get();

		// 如果替换, 先直接销毁元素, 不替换但如果存在,则直接处理下一个
		if (bReplace)
			remove(key.c_str());
		else if (!getAData(key.c_str()).empty())
			continue;

		mDataMap.insert(key, d);
		++resultCount;
	}

	return resultCount;
}
//-------------------------------------------------------------------------*/
bool NiceData::append( int index, const AData &data, bool bReplace /*= false*/ )
{
	ERROR_LOG(0, "XXX ERROR: NiceData Can not use append int");
	return false;
}

bool NiceData::append( const AString &index, const AData &data, bool bReplace /*= false*/ )
{
	if (mDataMap.exist(index))
	{
		if (bReplace)
		{
#if !USE_HASH_NICEDATA
            mDataMap.erase(index);
#endif
			mDataMap.insert(index, data);
			return true;
		}		
	}
	else
	{
		mDataMap.insert(index, data);
		return true;
	}
	return false;
}



void NiceData::initData()
{
	for (auto it=mDataMap.begin(); it.have(); it.next())
	{
		auto &kV = it.Value();
		kV.mVal.free();
	}
}

#if USE_JSON_LIB
AString NiceData::ToJSONEx(tNiceData *pMainCommentInfo, tNiceData *pNowCommentInfo)
{
	Json::Value	tempJson(Json::ValueType::objectValue);
	if (pNowCommentInfo != NULL)
	{ 
		return  ToJSON(&tempJson, pNowCommentInfo, pMainCommentInfo, 0);
	}
	
	return  ToJSON(&tempJson, pMainCommentInfo, pMainCommentInfo, 0);

	//Json::StyledWriter jsonWrier;

	//return jsonWrier.write(tempJson).c_str();
}

using namespace Json;
AString NiceData::ToJSON(Json::Value *destJson, tNiceData *pCommentInfo, tNiceData *pMainComment, int space)
{

	AString spaceStr;
	for (int i=0; i<space; ++i)
	{
		spaceStr += "    ";
	}	
	AString spaceString = spaceStr + "    ";

	AString resultString;
	resultString.Format("\r\n%s{\r\n", spaceStr.c_str());

	AutoNice orderList;
	// 获取字段顺序
	if (pMainComment != NULL)
		orderList = (tNiceData*)pMainComment->get(msKey + "__list__");
	if (!orderList)
	{
		orderList = MEM_NEW ArrayNiceData();
		for (auto it = mDataMap.begin(); it.have(); it.next())
		{
			orderList[orderList->count()] = it.key();
		}
	}
	for (int i=0; i<orderList->count(); ++i)
	{
		AString keyName = orderList[i];
		AData *pData = (AData*)mDataMap.findPtr(keyName);
		if (pData == NULL)
		{
			NOTE_LOG("No exist member %s", keyName.c_str());
			continue;
		}

		AString commentString;
		if (pCommentInfo != NULL)
		{
			AString comment = pCommentInfo->get(keyName);
			if (comment.length() > 0)
			{
				commentString = "//";
				commentString += comment;
			}
		}
		AutoNice  d;
		if (pData->get(&d, typeid(AutoNice)))
		{
			if (d)
			{
				AString strValue;
				tNiceData *pChildComment = NULL;
				if (pMainComment !=NULL)
				{
					if (d->msKey.length() > 0)
					{
						AutoNice comm = (tNiceData*)pMainComment->get(d->msKey);
						pChildComment = comm.getPtr();
					}
					else
					{
						AString typeName = pCommentInfo->get(keyName + "__type__");
						AutoNice comm = (tNiceData*)pMainComment->get(typeName);
						pChildComment = comm.getPtr();
					}
				}

				if (d->getType() == ARRAY_NICEDATA)
				{
					Value &jv = (*destJson)[keyName.c_str()];
					//if (commentString.length() > 0)
					//	jv.setComment(commentString.c_str(), commentAfterOnSameLine);
					Json::Value(Json::ValueType::arrayValue).swap(jv);
					strValue = d->ToJSON(&jv, pChildComment, pMainComment, space+1);
				}
				else
				{
					Value &jv = (*destJson)[keyName.c_str()];
					//if (commentString.length() > 0)
					//	jv.setComment(commentString.c_str(), commentAfterOnSameLine);
					Json::Value(Json::ValueType::objectValue).swap(jv);

					strValue = d->ToJSON(&jv, pChildComment, pMainComment, space+1);
				}

				AString info;
				if (strValue.c_str()[strValue.length() - 2] == '\r' && strValue.c_str()[strValue.length() - 1] == '\n')
					info.set(strValue.c_str(), strValue.length() - 2);
				else if (strValue.c_str()[strValue.length() - 1] == '\n')
					info.set(strValue.c_str(), strValue.length() - 1);
				else
					info = strValue.c_str();
				
				AString temp;
				if (d->getType() == ARRAY_NICEDATA)
					temp.Format("%s\"%s\" : %s%s    %s\r\n", spaceString.c_str(), keyName.c_str(), info.c_str(), i==orderList->count()-1 ? "":",", commentString.c_str());
				else
					temp.Format("%s\r\n%s%s\r\n%s\"%s\" : %s%s\r\n"
						, space==0 ? "    //___________________________________________________________________":""
						,  spaceString.c_str()
						, commentString.c_str()
						, spaceString.c_str()
						, keyName.c_str()
						, info.c_str(), i == orderList->count() - 1 ? "" : ","
					);
				resultString += temp;
			}
		}
		else
		{
			Value &jv = (*destJson)[keyName.c_str()];
			//if (commentString.length() > 0)
			//	jv.setComment(commentString.c_str(), commentAfterOnSameLine);
			AData &value = *pData;
			switch (value.getType())
			{
			case FIELD_INT:
				Json::Value((int)value).swap(jv);
				break;

			case FIELD_FLOAT:
				Json::Value((float)value).swap(jv);
				break;

			case FIELD_BOOL:
				Json::Value((bool)value).swap(jv);
				break;

			case  FIELD_UINT64:
				Json::Value(STRING((UInt64)value)).swap(jv);
				break;
			case FIELD_INT64:
				Json::Value(STRING((Int64)value)).swap(jv);
				break;

			default:
			{
				AString str = value;
				Json::Value(str.c_str()).swap(jv);
				break;
			}
			break;
			}
			Json::StyledWriter  write;
			std::string strValue = write.write(jv);
			AString info;
			if (strValue.c_str()[strValue.length() - 2] == '\r' && strValue.c_str()[strValue.length() - 1] == '\n')
				info.set(strValue.c_str(), strValue.length() - 2);
			else if (strValue.c_str()[strValue.length() - 1] == '\n')
				info.set(strValue.c_str(), strValue.length() - 1);
			else
				info = strValue.c_str();
				
			AString temp;
			temp.Format("%s\"%s\" : %s%s    %s\r\n", spaceString.c_str(), keyName.c_str(), info.c_str(), (info.c_str(), i == orderList->count() - 1 ? "":","), commentString.c_str());
			resultString += temp;
		}
	}
	resultString += spaceStr;
	resultString += "}\r\n";
	return resultString;
}


bool tNiceData::FullJSON(const char *szKey, Json::Value *pJV)
{
	if (pJV->isObject())
	{
		auto memberList = pJV->getMemberNames();
		for (int i = 0; i < memberList.size(); ++i)
		{
			auto key = memberList[i];

			AData &targetAData = getOrCreate(key.c_str());

			Value &value = (*pJV)[key.c_str()];
			if (!FullToAData(key.c_str(), &value, targetAData))
				return false;
		} 
		return true;
	}
	else
		ERROR_LOG("%s is not nicedata > %d", szKey, pJV->type());
	return false;
}

bool tNiceData::FullToAData(const char *szKey, Json::Value *pScrJSON, AData &targetAData)
{
	Value &value = *pScrJSON;
	if (value.isArray())
	{
		AutoNice d = MEM_NEW ArrayNiceData();
		targetAData.set(&d, typeid(AutoNice));
		if (!d->FullJSON(szKey, &value))
			return false;
	}
	else if (value.isObject())
	{
		AutoNice d = MEM_NEW NiceData();
		targetAData.set(&d, typeid(AutoNice));
		if (!d->FullJSON(szKey, &value))
			return false;
	}
	else
	{
		switch (value.type())
		{
		case ValueType::intValue:
			targetAData = value.asInt();
			break;
		case ValueType::uintValue:
			targetAData = value.asUInt();
			break;

		case ValueType::realValue:
			targetAData = (float)value.asDouble();
			break;

		case ValueType::stringValue:
			targetAData = value.asString().c_str();
			break;

		case ValueType::booleanValue:
			targetAData = value.asBool();
			break;

		default:
			ERROR_LOG("%s Can not use json type > %d", szKey, value.type());
			return false;
		}
	}
	return true;
}



#endif
//-------------------------------------------------------------------------
//{"key":"char","key2":10}
AString NiceData::ToJSON()
{
#if USE_JSON_LIB
	Json::Value	tempJson(Json::ValueType::objectValue);
	ToJSON(&tempJson, NULL, NULL, 0);

	Json::StyledWriter jsonWrier;

	return jsonWrier.write(tempJson).c_str();
#else
	AString result = "{";
	bool bFrist = true;
	for (auto it=mDataMap.begin(); it.have(); it.next())
	{
		const auto &kV = it.Value();
		const char *szKey = kV.mKey.c_str();
		const AData &d = kV.mVal;

		if (!d.empty())
		{
            Data val(&d);
            
			AString valueString;
			FIELD_TYPE type = (FIELD_TYPE)d.getType();
			if (type==FIELD_INT
				|| type==FIELD_FLOAT
				|| type==FIELD_UINT64
				|| type==FIELD_SHORT
				)
			{
				if (bFrist)
					valueString.Format("\"%s\":%s", szKey, val.string().c_str());
				else
					valueString.Format(",\"%s\":%s", szKey, val.string().c_str());
			}
			else if (type==FIELD_NICEDATA)
			{
				AutoNice nice;
				d.get(&nice, typeid(AutoNice));
				AutoNice niceValue = nice;
				if (niceValue)
				{
					AString strVal = niceValue->ToJSON();
					if (bFrist)
						valueString.Format("\"%s\":%s", szKey, strVal.c_str());
					else
						valueString.Format(",\"%s\":%s", szKey, strVal.c_str());
				}
				else
					valueString.Format(",\"%s\":{}", szKey);
			}
			else
			{
				if (bFrist)
					valueString.Format("\"%s\":\"%s\"", szKey, val.string().c_str());
				else
					valueString.Format(",\"%s\":\"%s\"", szKey, val.string().c_str());
			}
			bFrist = false;

			result += valueString;
		}						
	}
	result += "}";

	return result;
#endif //USE_JSON_LIB
}

AString NiceData::ToFastJSON()
{
#if USE_JSON_LIB
	Json::Value	tempJson(Json::ValueType::objectValue);
	ToJSON(&tempJson, NULL, NULL, 0);

	Json::FastWriter jsonWrier;

	return jsonWrier.write(tempJson).c_str();
#else
	ERROR_LOG("No code");
	return AString();
#endif
}

#endif //USE_IDNICEDATA

//-------------------------------------------------------------------------

//----------------------------------------------------------------

AString ArrayNiceData::dump( int sub, int code ) const
{
	bool bHave = false;
	AString info = ""; //tNiceData::dump(sub, code);
	for (size_t i=0; i<mDataList.size(); ++i)
	{
		//if (!mDataList[i].empty())
		{
			bHave = true;
			info += "\r\n";
			
			if (sub>0)
			{
				for (int n = 0; n<sub; ++n)
				{
					info += spaceString;
				}				
			}
			if (code>0)
			{
				info += code;
				info += "_";
			}
			info += (int)i;
			if (mDataList[i].empty())
			{
				info += ">[NULL]";
				continue;
			}
			info += ">[";
			info += mDataList[i]._getField()->getTypeString();
			info += "] = ";
			if (mDataList[i].getType()==FIELD_NICEDATA ||mDataList[i].getType()==FIELD_ARRAYDATA)
			{
				AutoNice nice;
                if (mDataList[i].getType()==FIELD_NICEDATA)
				    mDataList[i].get(&nice, typeid(AutoNice));
                else
                {
                    AutoArrayData dV;
                    mDataList[i].get(&dV, typeid(dV));
                    nice = dV;
                }
				if (nice)
                {
                     if (mDataList[i].getType()==FIELD_NICEDATA)
                        info += "SUB Nice\r\n";
                     else
                         info += "SUB Array\r\n";
                    for (int n=0; n<sub; ++n)
                    {
                        info += spaceString;
                    }
                    info +="   {";
					info += nice->dump(sub+1, i);		
                    for (int n=0; n<sub; ++n)
                    {
                        info += spaceString;
                    }
                    info += "   }";
                    info += (int)i;
				}
				else
					info += "NULL";
			}
			else
			{
				AString valueString = mDataList[i];
				info += valueString;
			}
		}
		//else
		//	info += "[NULL]:NULL";
	}
	//if (bHave && sub>0 )
	//	info += "\r\n}\r\n";
    for (int n=0; n<sub; ++n)
    {
        info += spaceString;
    }
    if (sub==0)
        info +="-------------------------------------------------------------------------\r\n";
    else
        info +="\r\n";
	return info;
}

bool ArrayNiceData::append( int index, const AData &data, bool bReplace /*= false*/ )
{
	if (index>0xFFFF)
	{
		AString valueString = data;
		ERROR_LOG("数据下标过大, 超过 65535，[%s] 追加失败", valueString.c_str());
		return false;
	}
	if (index>=mDataList.size())
	{
		mDataList.resize(index+1);
	}
	else if (!bReplace)
	{
		if (!mDataList[index].empty())
			return false;
	}

	mDataList[index] = data;
	return true;
}

size_t ArrayNiceData::append(const tNiceData &scrData, bool bReplace)
{
	ArrayNiceData *p = (ArrayNiceData*)dynamic_cast<const ArrayNiceData*>(&scrData);
	if (p == NULL)
	{
		NOTE_LOG("ERROR: XXX ArrayNiceData append only ArrayNiceData param type");
		return 0;
	}
	int scrCount = scrData.count();
	if (scrCount > 0xFFFF)
	{
		ERROR_LOG("原数据下标超过 65535");
		return 0;
	}

	for (auto it = p->begin(); it.have(); it.next())
	{
		if (bReplace)
		{
			int k = TOINT(it.key().c_str());
			_get(k) = it.get();
		}
		else
			if (!append(count(), it.get(), false))
				ERROR_LOG("ArrayNiceData append data %d fail", count());
	}
	return scrCount;
}

bool ArrayNiceData::FullJSON(AString scrJsonString)
{
#if USE_JSON_LIB
	Json::Value jv;
	Json::Reader jsonReader;

	if (jsonReader.parse(scrJsonString.c_str(), scrJsonString.c_str() + scrJsonString.length(), jv))
	{
		return FullJSON("", &jv);
	}
	else
		ERROR_LOG("JSON parse error > %s", scrJsonString.c_str());
	return false;
#endif
	// 为了支持逗号, 使用  | 符号隔开
	clear(true);
	if (scrJsonString.c_str()[0] == '{')
	{
		int count = 0;
		mDataList.clear();
		do
		{
			bool bHave = false;
			AString data = scrJsonString.SplitBlock('{', '}', true, &bHave);
			if (bHave)
			{
				++count;
				AutoNice d = MEM_NEW NiceData();
				if (!d->FullJSON(data))
					ERROR_LOG("FullJSON to NiceData error > %s", data.c_str());
				if (mDataList.size() < count)
					mDataList.resize(count);
				if (!mDataList[count - 1].set(&d, typeid(d)))
					ERROR_LOG("FullJSON to NiceData AData set error > %s%s", d->dump().c_str(), data.c_str());

				if (scrJsonString.c_str()[0] == ',')
					scrJsonString.SplitLeft(",");
			}
			else
			{
				WARN_LOG("Empty array %s", scrJsonString.c_str());
				break;
			}
		} while (scrJsonString.length() > 0 && scrJsonString.c_str()[0] == '{');
	}
	else
	{
		AString strData = scrJsonString.SplitBlock('[', ']');
		StringArray strList;
		AString::Split(strData.c_str(), strList, ",", 65535);
		mDataList.resize(strList.size());
		for (int i=0; i<strList.size(); ++i)
		{
			mDataList[i] = strList[i];
		}
	}
	return true;
}

AString ArrayNiceData::ToJSON()
{
#if USE_JSON_LIB
	Json::Value	tempJson(Json::ValueType::arrayValue);
	ToJSON(&tempJson, NULL, NULL, 0);

	Json::FastWriter jsonWrier;

	return jsonWrier.write(tempJson).c_str();
#else
	AString result = "[";	
	for (size_t i=0; i<mDataList.size(); ++i)
	{
		if (i>0)
			result += ",";
        Data    d(&mDataList[i]);
		result += d.string();
	}
	result += "]";
	return result;
#endif
}
#if USE_JSON_LIB
AString ArrayNiceData::ToJSON(Json::Value *targetJS, tNiceData *pComment, tNiceData *pMainComment, int space)
{
	for (DSIZE i = 0; i < mDataList.size(); ++i)
	{
		AData &valueAData = mDataList[i];

		Value &jv = (*targetJS)[i];

		AutoNice  d;
		if (valueAData.get(&d, typeid(AutoNice)))
		{
			d->ToJSON(&jv, pComment, pMainComment, space+1);
		}
		else
		{
			switch (valueAData.getType())
			{
			case FIELD_INT:
				Json::Value((int)valueAData).swap(jv);
				break;

			case FIELD_FLOAT:
				Json::Value((float)valueAData).swap(jv);
				break;

			case FIELD_BOOL:
				Json::Value((bool)valueAData).swap(jv);
				break;

			case  FIELD_UINT64:
				Json::Value(STRING((UInt64)valueAData)).swap(jv);
				break;
			case FIELD_INT64:
				Json::Value(STRING((Int64)valueAData)).swap(jv);
				break;

			default:
			{
				AString str = valueAData;
				Json::Value(str.c_str()).swap(jv);
				break;
			}
			break;
			}
		}
	}

	Json::StyledWriter writer;
	return writer.write(*targetJS).c_str();
}

bool ArrayNiceData::FullJSON(const char *szKey, Json::Value *pJV)
{
	if (pJV->isArray())
	{
		AString key;
	
		for (DSIZE i = 0; i < (DSIZE)pJV->size(); ++i)
		{
			AData &targetAData = getOrCreate(i);

			Value &value = (*pJV)[i];
			key.Format("%s_%d", szKey, i);
			if (!FullToAData(key.c_str(), &value, targetAData))
				return false;
		}
		return true;
	}
	else
		ERROR_LOG("%s is not array nicedata format : '[...]'> %d", szKey, pJV->type());

	return false;
}

#endif

AutoNice ArrayNiceData::NewNice()
{
	return MEM_NEW ArrayNiceData();
}

bool ArrayNiceData::serialize( DataStream *destData ) const
{
	if (mDataList.size()>0xFFFF)
	{
		ERROR_LOG("数组下标[%d]超过 65535 >\r\n%s", (int)mDataList.size(), dump().c_str());
		AssertEx(0, "数组下标[%d]超过 65535 >\r\n%s", (int)mDataList.size(), dump().c_str());
		return false;
	}
	destData->write((ushort)mDataList.size());
	for (size_t i=0; i<mDataList.size(); ++i)
	{			
		if (!mDataList[i].serialize(destData))
		{
			ERROR_LOG("数据 [%u] 序列保存失败", i);
			return false;
		}
	}
	return true;
}

bool ArrayNiceData::restore( DataStream *scrData )
{
	ushort num = 0;
	if (!scrData->read(num))
		return false;

	if (mDataList.size()<num)
		mDataList.resize(num);

	for (size_t i=0; i<num; ++i)
	{
		if (!mDataList[i].restore(scrData))
		{
			ERROR_LOG("数据 [%u] type >[%s] 恢复失败", i, mDataList[i]._getField()!=NULL ? mDataList[i]._getField()->getTypeString():"NULL");
			return false;
		}
	}
	return true;
}
