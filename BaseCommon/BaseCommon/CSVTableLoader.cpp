
#include "CSVTableLoader.h"

#include "CSVReader.h"
#include "FileDataStream.h"
#include "Common.h"

//using namespace std;

bool _SplitCsvLine(std::string lineStr,  std::vector<std::string> &destList)
{
	size_t start = 0;

	for (int i = 0; i < destList.size(); ++i)
	{
		if (start >= lineStr.length())
		{
			if (i == destList.size() - 1)
			{
				destList[i] = "";
				break;
			}
			return false;
		}
		// ��"
		if (lineStr[start] == '"')
		{
			size_t pos2 = lineStr.find('"', start + 1);
			if (pos2 == std::string::npos)
				return false;
			start += 1;
			destList[i] = lineStr.substr(start, pos2 - start);
			if (i == destList.size() - 1)
			{
				break;
			}
			if (pos2 + 1 >= lineStr.length() || lineStr[pos2 + 1] != ',')
				return false;

			start = pos2 + 2;
		}
		else
		{
			size_t pos = lineStr.find(',', start);
			if (pos == std::string::npos)
			{
				if (i == destList.size() - 1)
				{
					destList[i] = lineStr.substr(start);
					break;
				}
				else
					return false;
			}
			destList[i] = lineStr.substr(start, pos - start);
			start = pos + 1;
		}
	}
	return true;
}


//int strstr_cnt(const char *string, const char *substring) {
//	int i,j,k,count = 0;
//	for (i = 0; string[i]; i++){
//		for (j = i, k = 0; (string[j] == substring[k] && (j < strlen(string))); j++,k++) {
//			if (! substring[k + 1]) {
//				count++;
//			}
//		}
//	}
//	return count;
//}
//
//int substring_index(const char *s1,const char *s2, int pos){
//	int i,j,k;
//	for( i = pos ; s1[i] ; i++ ) {
//		for( j = i, k = 0 ; s1[j] == s2[k]; j++,k++ ){
//			if (! s2[k + 1]) {
//				return i;
//			}
//		}
//	}
//	return -1;
//}


bool fgetcsvline(std::vector<std::string> &csv_databuf, DataStream *fhead) {
	//char  *ret_stat;
	std::string stringbuf;

	AString result;
	if (!fhead->readLine(result))
		return false;
	
	if (!_SplitCsvLine(result.c_str(), csv_databuf))
	{
		if (result.length()<=0)
			return false;
		ERROR_LOG("CSV ���ַ�����ʧ��>%s", result.c_str());
		return false;
	}
	return true;

	//ret_stat = fgets( data_buf, 1024, fhead );
	//const char  *data_buf = result.c_str();

	////if (ret_stat != NULL) 
	//{
	//	int len = strstr_cnt(data_buf,"\",\"");
	//	if (len > 0){
	//		int pos = substring_index(data_buf,"\",\"",0);
	//		int startpos = 1;
	//		string csv_buf;
	//		while (pos > 0) {
	//			stringbuf = (string)data_buf;
	//			csv_buf = stringbuf.substr(startpos,pos - startpos);
	//			csv_databuf.push_back(csv_buf);
	//			startpos = pos + 3;
	//			pos = substring_index(data_buf,"\",\"",pos + 2);
	//		}
	//		if ((substring_index(data_buf,"\n",0)) > 0){
	//			csv_buf = stringbuf.substr(startpos, stringbuf.length() - startpos - 2);
	//		} else {
	//			csv_buf = stringbuf.substr(startpos, stringbuf.length() - startpos - 1);
	//		}
	//		csv_databuf.push_back(csv_buf);
	//	}
	//}

	//return false;    
}

bool CSVTableLoader::SaveTable( tBaseTable *pTable, DataStream *resultData, const char *szIndexField )
{
	ARecordIt it;
	if (szIndexField!=NULL)
	{
		AutoIndex index = pTable->GetIndex(szIndexField);
		if (!index)
		{
			ERROR_LOG("����������[%s]", szIndexField);
			return false;
		}
		it = index->GetRecordIt();
	}
	else
		it = pTable->GetRecordIt();

	AutoField info = pTable->GetField();
	
	DataStream & f = *resultData;
	
	for (int i=0; i< info->getCount(); ++i)
	{
		if (i>0)
			f.writeText(",");
		f.writeText(info->getFieldInfo(i)->getTypeString());
		f.writeText( " {" );
		f.writeText( info->getFieldInfo(i)->getTypeParam().c_str() );
		f.writeText( "}" );
	}
	f.writeText("\r\n");

	for (int i=0; i< info->getCount(); ++i)
	{
		if (i>0)
			f.writeText(",");
		f.writeText(info->getFieldInfo(i)->getName().c_str());
	}
	f.writeText("\r\n");
	if (!it)
        return false;
	int fieldCount = info->getCount();
	for ( ; *it; ++(*it))
	{
		ARecord re = *it;
		if (!re)
			continue;
		for (int i=0; i< fieldCount; ++i)
		{
			if (i>0)
				f.writeText(",");

			AString val = AString::getANIS(re->get(i).string().c_str());
			//PRINT("CSV: %s : %s\r\n", info->getFieldInfo(i)->getName().c_str(), val.c_str());
			bool b = strstr(val.c_str(), ",")!=NULL;
			if (b)
				f.write('"');
			f.writeText(val.c_str());
			if (b)
				f.write('"');
		}
		f.writeText("\r\n");
	}

	return true;
}

bool CSVTableLoader::LoadTable( tBaseTable *pTable, DataStream *scrData )
{
	//InitReset();

	return LoadFromData(pTable, scrData);
}



bool CSVTableLoader::LoadFromData(tBaseTable *pTable, DataStream *scrStream )
{
	if (scrStream==NULL)
		return false;

	pTable->Init();
	
	Array<AString> typeInfo;
	Array<AString> fieldNameInfo;

	AString typeStr;
	AString nameStr;
	if (!scrStream->readLine(typeStr))
	//if (!fgetcsvline(typeInfo, scrStream))
	{
		WARN_LOG("������Ϊ��, �ձ� [%s]", pTable->GetTableName());
		return true;
	}
	//if (!fgetcsvline(fieldNameInfo, scrStream))
	if (!scrStream->readLine(nameStr))
	{
		WARN_LOG("������Ϊ��, �ձ� [%s]", pTable->GetTableName());
		return true;
	}
	
	int colNum = AString::Split(typeStr.c_str(), typeInfo, ",", 0);

	if (colNum<=0)
	{
		WARN_LOG("����ֶ�����Ϊ0 [%s]", pTable->GetTableName());
		return true;
	}
	AString::Split(nameStr.c_str(), fieldNameInfo, ",", 0);
	if (fieldNameInfo.size()!=colNum)
	{
		ERROR_LOG("������������������������� [%s]", pTable->GetTableName());
		return false;
	}

	int count = 0;
	std::vector<int> fieldCol;
	fieldCol.resize(colNum);
	for (int i=0; i<colNum; ++i)
	{
		fieldCol[i] = -1;
		if (fieldNameInfo[i]=="" || pTable->GetField()->existField(fieldNameInfo[i].c_str()))
		{
			DEBUG_LOG("Warn: [%s] >[%s] �ظ����ֶ�������ֶ���, ��������", pTable->GetTableName(), fieldNameInfo[i].c_str());
			continue;
		}

		AString typeData = typeInfo[i];
		AString type = typeData.SplitLeft(" ");
		AString paramData = typeData.SplitBlock();

		if  (type=="")
			type = typeInfo[i].c_str();
		//FIELD_TYPE type = FIELD_STRING;

		//if (typeInfo[i]=="INT")
		//	type = FIELD_INT;
		//else if (typeInfo[i]=="FLOAT")
		//	type = FIELD_FLOAT;
		//else if (typeInfo[i]=="BOOL")
		//	type = FIELD_BOOL;

		fieldCol[i] = count;
		if (pTable->SetField(fieldNameInfo[i].c_str(), type.c_str(), count++)!=NULL)
		{
			FieldInfo info = pTable->GetField()->getFieldInfo(fieldNameInfo[i].c_str());
			if (info!=NULL)
				info->setTypeParam(paramData);
		}		
		else
		{			
			--count;
			ERROR_LOG("[%s]��ȡCSV���ʧ��, type[%s], Name [%s]�ֶ���Ϣ��������, ��ǰ�����������е���", pTable->GetTableName(), type.c_str(), fieldNameInfo[i].c_str());
			fieldCol.resize(count);			
			colNum = count;
			break;
		}
	}		
	
	pTable->GetField()->_updateInfo();
	if (count<=0 || !pTable->GetField()->check())
	{
		ERROR_LOG("����ֶ�����Ϊ0, ���ֶμ��ʧ��");
		return false;
	}

	if (!pTable->GetMainIndex())
	{
		if (!pTable->SetMainIndex(0, true, false))
		{
			ERROR_LOG("Set main index fail, %s > %s", pTable->GetTableName(), pTable->GetField()->getFieldInfo(0)->getName().c_str());
			return false;
		}
	}

	std::vector<std::string> data;
	data.resize(colNum);
	while (fgetcsvline(data, scrStream))
	{		
		//if (data.size()<fieldCol.size())
		//{
		//	ERROR_LOG("��������С�ڱ������");
		//	return false;
		//}
		std::string key = data[0];
		if (strcmp(key.c_str(), "")==0)
			continue;

		if (pTable->GetRecord(key.c_str()) )
		{
			ERROR_LOG("��ȡCSV��� >[%s], �����ظ��ļ�ֵ >[%s], ��ǰ���Ǵ���", pTable->GetTableName(),key.c_str());
		}

		ARecord hRecord = pTable->CreateRecord(key.c_str(), true);

		for (int j=1; j<colNum; ++j)
		{
			if (fieldCol[j]>=0)
			{
				//if (hRecord->getField()->getFieldInfo(fieldCol[j])->getType()==FIELD_BOOL)
				//{		
				//	int b = atoi(data[j].c_str());
				//	if (b!=0 || data[j]=="TRUE")
				//		b = 1;
				//	hRecord->set(fieldCol[j], b);
				//}
				//else
				{
					hRecord->set(fieldCol[j], AString::getUTF8(data[j].c_str()).c_str());
				}
			}
		}
		//data.clear();
	}

	return true;

	//AutoData dataBuffer;

	//FileDataStream *pDataFile = dynamic_cast<FileDataStream*>(scrStream);
	//if (pDataFile!=NULL)
	//{
	//	if (*pDataFile)
	//	{
	//		dataBuffer = pDataFile->readAllData();
	//		dataBuffer->seek(dataBuffer->dataSize());
	//		dataBuffer->write((byte)'\0');
	//		dataBuffer->seek(0);
	//		scrStream = dataBuffer.getPtr();
	//		if (scrStream==NULL)
	//			return false;
	//	}
	//	else
	//	{
	//		DEBUG_LOG("ERROR: �ļ��򿪴���");
	//		return false;
	//	}
	//}

	//CsvData	tempCSV;
	//if (!tempCSV.LoadFromData((unsigned char*)scrStream->data(), scrStream->size()))
	//	return false;

	//int rowNum = tempCSV.GetRowNum();
	//if (rowNum<2)
	//{
	//	DEBUG_LOG( "csv �ļ�����2��");
	//	return false;
	//}

	//const std::vector<std::string> &typeInfo = tempCSV.GetData(0);
	//const std::vector<std::string> &fieldNameInfo = tempCSV.GetData(1);		

	//int colNum = typeInfo.size();

	//int count = 0;
	//std::vector<int> fieldCol;
	//fieldCol.resize(colNum);
	//for (int i=0; i<colNum; ++i)
	//{
	//	fieldCol[i] = -1;
	//	if (fieldNameInfo[i]=="" || pTable->GetField()->existField(fieldNameInfo[i].c_str()))
	//	{
	//		DEBUG_LOG("Warn: [%s] >[%s] �ظ����ֶ�������ֶ���, ��������", pTable->GetTableName(), fieldNameInfo[i].c_str());
	//		continue;
	//	}

	//	AString typeData = typeInfo[i].c_str();
	//	AString type = typeData.SplitLeft(" ");
	//	AString paramData = typeData.SplitBlock();

	//	if  (type=="")
	//		type = typeInfo[i].c_str();
	//	//FIELD_TYPE type = FIELD_STRING;

	//	//if (typeInfo[i]=="INT")
	//	//	type = FIELD_INT;
	//	//else if (typeInfo[i]=="FLOAT")
	//	//	type = FIELD_FLOAT;
	//	//else if (typeInfo[i]=="BOOL")
	//	//	type = FIELD_BOOL;

	//	fieldCol[i] = count;
	//	if (pTable->SetField(fieldNameInfo[i].c_str(), type.c_str(), count++)!=NULL)
	//	{
	//		FieldInfo info = pTable->GetField()->getFieldInfo(fieldNameInfo[i].c_str());
	//		if (info!=NULL)
	//			info->setTypeParam(paramData);
	//	}		
	//	else
	//	{
	//		DEBUG_LOG("ERROR: ��ȡCSV���ʧ��, [%s]�ֶ���Ϣ��������", fieldNameInfo[i].c_str());
	//		return false;
	//	}
	//}		
	//pTable->GetField()->_updateInfo();
	//if (count<=0)
	//	return true;

	//for (int i=2; i<rowNum; ++i)
	//{
	//	const std::vector<std::string> &data = tempCSV.GetData(i);

	//	if (strcmp(data[0].c_str(), "")==0)
	//		continue;

	//	ARecord hRecord = pTable->CreateRecord(data[0].c_str(), true);
	//	int s = data.size();

	//	if (s>(int)fieldCol.size()) 
	//		s = (int)fieldCol.size();

	//	for (int j=1; j<s; ++j)
	//	{
	//		if (fieldCol[j]>=0)
	//		{
	//			if (hRecord->getField()->getFieldInfo(fieldCol[j])->getType()==FIELD_BOOL)
	//			{		
	//				int b = atoi(data[j].c_str());
	//				if (b!=0 || data[j]=="TRUE")
	//					b = 1;
	//				hRecord->set(fieldCol[j], b);
	//			}
	//			else
	//			{
	//				hRecord->set(fieldCol[j], AString::getUTF8(data[j].c_str()).c_str());
	//			}
	//		}
	//	}
	//}

	//return true;
}

bool CSVTableLoader::SaveCSV( tBaseTable *pTable, const char *szCSVFileName, const char *szIndexField /*= NULL*/ )
{
	CSVTableLoader csvLoader;
	FileDataStream   f(szCSVFileName, FILE_CREATE_UTF8);
	if (!f)
	{
		DEBUG_LOG("ERROR: ����CSV����ļ�ʧ�� [%s]", szCSVFileName);
		return false;
	}
	return csvLoader.SaveTable(pTable, &f, szIndexField);
}

bool TxtTableLoader::SaveTable(tBaseTable *pTable, DataStream *resultData, const char *szIndexField /*= NULL*/)
{
    ARecordIt it;
    if (szIndexField!=NULL)
    {
        AutoIndex index = pTable->GetIndex(szIndexField);
        if (!index)
        {
            ERROR_LOG("����������[%s]", szIndexField);
            return false;
        }
        it = index->GetRecordIt();
    }
    else
        it = pTable->GetRecordIt();

    AutoField info = pTable->GetField();

    DataStream & f = *resultData;

    for (int i=0; i< info->getCount(); ++i)
    {
        if (i>0)
            f.writeText(TXT_TABLE_DELIMITED_STRING);
        f.writeText(info->getFieldInfo(i)->getTypeString());
        f.writeText( " {" );
        f.writeText( info->getFieldInfo(i)->getTypeParam().c_str() );
        f.writeText( "}" );
    }
    f.writeText("\r\n");

    for (int i=0; i< info->getCount(); ++i)
    {
        if (i>0)
            f.writeText(TXT_TABLE_DELIMITED_STRING);
        f.writeText(info->getFieldInfo(i)->getName().c_str());
    }
    f.writeText("\r\n");
    if (!it)
        return true;
    for ( ; *it; ++(*it))
    {
        ARecord re = *it;
        if (!re)
            continue;
        for (int i=0; i< info->getCount(); ++i)
        {
            if (i>0)
                f.writeText(TXT_TABLE_DELIMITED_STRING);

            AString val = AString::getANIS(re->get(i).string().c_str());
            f.writeText(val.c_str());
        }
        f.writeText("\r\n");
    }

    return true;
}

bool TxtTableLoader::LoadFromData(tBaseTable *pTable, DataStream *scrStream)
{
    if (scrStream==NULL)
        return false;

    pTable->Init();

    Array<AString> typeInfo;
    Array<AString> fieldNameInfo;

    AString typeStr;
    AString nameStr;
    if (!scrStream->readLine(typeStr))
        //if (!fgetcsvline(typeInfo, scrStream))
    {
        WARN_LOG("������Ϊ��, �ձ� [%s]", pTable->GetTableName());
        return true;
    }
    //if (!fgetcsvline(fieldNameInfo, scrStream))
    if (!scrStream->readLine(nameStr))
    {
        WARN_LOG("������Ϊ��, �ձ� [%s]", pTable->GetTableName());
        return true;
    }

    int colNum = AString::Split(typeStr.c_str(), typeInfo, TXT_TABLE_DELIMITED_STRING, 0);

    if (colNum<=0)
    {
        WARN_LOG("����ֶ�����Ϊ0 [%s]", pTable->GetTableName());
        return true;
    }
    AString::Split(nameStr.c_str(), fieldNameInfo, TXT_TABLE_DELIMITED_STRING, 0);
    if (fieldNameInfo.size()!=colNum)
    {
        ERROR_LOG("������������������������� [%s]", pTable->GetTableName());
        return false;
    }

    int count = 0;
    std::vector<int> fieldCol;
    fieldCol.resize(colNum);
    for (int i=0; i<colNum; ++i)
    {
        fieldCol[i] = -1;
        if (fieldNameInfo[i]=="" || pTable->GetField()->existField(fieldNameInfo[i].c_str()))
        {
            DEBUG_LOG("Warn: [%s] >[%s] �ظ����ֶ�������ֶ���, ��������", pTable->GetTableName(), fieldNameInfo[i].c_str());
            continue;
        }

        AString typeData = typeInfo[i];
        AString type = typeData.SplitLeft(" ");
        AString paramData = typeData.SplitBlock();

        if  (type=="")
            type = typeInfo[i].c_str();
        //FIELD_TYPE type = FIELD_STRING;

        //if (typeInfo[i]=="INT")
        //	type = FIELD_INT;
        //else if (typeInfo[i]=="FLOAT")
        //	type = FIELD_FLOAT;
        //else if (typeInfo[i]=="BOOL")
        //	type = FIELD_BOOL;

        fieldCol[i] = count;
        if (pTable->SetField(fieldNameInfo[i].c_str(), type.c_str(), count++)!=NULL)
        {
            FieldInfo info = pTable->GetField()->getFieldInfo(fieldNameInfo[i].c_str());
            if (info!=NULL)
                info->setTypeParam(paramData);
        }		
        else
        {			
            --count;
            ERROR_LOG("[%s]��ȡCSV���ʧ��, type[%s], Name [%s]�ֶ���Ϣ��������, ��ǰ�����������е���", pTable->GetTableName(), type.c_str(), fieldNameInfo[i].c_str());
            fieldCol.resize(count);			
            colNum = count;
            break;
        }
    }		

    pTable->GetField()->_updateInfo();
    if (count<=0)
    {
        ERROR_LOG("����ֶ�����Ϊ0");
        return false;
    }

    Array<AString> data;
    data.resize(colNum);
    AString lineString;
    while (true)
    {		
        if (!scrStream->readLine(lineString))
            break;
        data.clear();
        if (AString::Split(lineString.c_str(), data, TXT_TABLE_DELIMITED_STRING, colNum)!=colNum)
        {
            if (data.size()==colNum-1)
                data.push_back(AString());
            else
            {
                ERROR_LOG("[%s] Txt table split error >%s", pTable->GetTableName(), lineString.c_str());
                break;
            }
        }
        AString key = data[0];
        if (strcmp(key.c_str(), "")==0)
            continue;

        if (pTable->GetRecord(key.c_str()) )
        {
            ERROR_LOG("��ȡCSV��� >[%s], �����ظ��ļ�ֵ >[%s], ��ǰ���Ǵ���", pTable->GetTableName(),key.c_str());
        }

        ARecord hRecord = pTable->CreateRecord(key.c_str(), true);

        for (int j=1; j<colNum; ++j)
        {
            if (fieldCol[j]>=0)
            {
                //if (hRecord->getField()->getFieldInfo(fieldCol[j])->getType()==FIELD_BOOL)
                //{		
                //	int b = atoi(data[j].c_str());
                //	if (b!=0 || data[j]=="TRUE")
                //		b = 1;
                //	hRecord->set(fieldCol[j], b);
                //}
                //else
                {
                    hRecord->set(fieldCol[j], AString::getUTF8(data[j].c_str()).c_str());
                }
            }
        }
        //data.clear();
    }

    return true;
}
