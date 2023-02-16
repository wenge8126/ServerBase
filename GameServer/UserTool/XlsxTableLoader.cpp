#include "XlsxTableLoader.h"

#include "framework.h"
#include "AutoString.h"
#include "FileDataStream.h"
#include <afxdb.h>
#include <odbcinst.h>
using namespace std;

// 获取ODBC中Excel驱动
CString GetExcelDriver(const AString &excelFileType)
{
	char szBuf[2001];
	WORD cbBufMax = 2000;
	WORD cbBufOut;
	char *pszBuf = szBuf;
	CString sDriver;

	// 获取已安装驱动的名称(涵数在odbcinst.h里)
	if (!SQLGetInstalledDrivers(szBuf, cbBufMax, &cbBufOut))
		return "";

	// 检索已安装的驱动是否有Excel...
	do
	{
		if (strstr(pszBuf, excelFileType.c_str()) != 0)
		{
			//发现 !
			sDriver = CString(pszBuf);
			break;
		}
		pszBuf = strchr(pszBuf, '\0') + 1;
	} while (pszBuf[1] != '\0');

	return sDriver;
}

AString			XlsxTableLoader::msLogFileName;
bool				XlsxTableLoader::mbInitLogFile = true;

//-------------------------------------------------------------------------
// 读取Excel文件  
//格式为: 第一行 字段名和类型[NAME:TYPE] 例: INDEX:INT	NAME:STRING
// 第二行为字段说明
//-------------------------------------------------------------------------
AutoTable XlsxTableLoader::LoadXlsx(const AString &xlsxFile, const AString &sheetName)
{
	AString sheet = AString::getANIS(sheetName.c_str());
	AutoTable table = tBaseTable::NewBaseTable();

	CDatabase database;
	CString sSql;
	CString strItemData, sItem2;
	CString sDriver;
	CString sDsn;
	// 检索是否安装有Excel驱动 "Microsoft Excel Driver (*.xls)" 
	AString ext = FileDataStream::GetFileExtName(xlsxFile.c_str());
	ext.toLower();
	sDriver = GetExcelDriver(ext);
	if (sDriver.IsEmpty())
	{
		// 没有发现Excel驱动
		ERROR_LOG("没有安装Excel驱动!");
		return AutoTable();
	}

	// 创建进行存取的字符串
	sDsn.Format("ODBC;DRIVER={%s};DSN='';DBQ=%s", sDriver, xlsxFile.c_str());

	TRY;
	{
		// 打开数据库(既Excel文件)
		database.Open(NULL, false, false, sDsn);

		CRecordset recset(&database);

		// 设置读取的查询语句.
		sSql.Format("Select * from [%s$]", sheet.c_str());

		// 执行查询语句
		if (recset.Open(CRecordset::forwardOnly, sSql, CRecordset::readOnly) == FALSE)
		{
			database.Close();
			ERROR_LOG("Open xlsx file fail > %s, sheet %s > SQL %s", xlsxFile.c_str(), sheet.c_str(), (LPCTSTR)sSql);
			return AutoTable();
		}

		int fieldCount = recset.GetODBCFieldCount();
		
		if (!recset.IsEOF())
		{
			CODBCFieldInfo  fieldInfo;
			for (int i = 0; i < fieldCount; ++i)
			{
				recset.GetODBCFieldInfo(i, fieldInfo);

				if (fieldInfo.m_strName.GetLength() <= 0)
				{
					fieldCount = i;
					//WARN_LOG("Field Col %d : %s  is empty, now giveup > %s, sheet %s", i, (LPCTSTR)fieldInfo.m_strName, xlsxFile.c_str(), sheet.c_str());
					break;
				}

				StringArray strList;
				if (AString::Split((LPCTSTR)fieldInfo.m_strName, strList, ":", 3) < 2)
				{
					fieldCount = i;
					//WARN_LOG("Field Col %d : %s  is error, now giveup > %s, sheet %s", i, (LPCTSTR)fieldInfo.m_strName, xlsxFile.c_str(), sheet.c_str());
					break;
					//database.Close();
					//ERROR_LOG("Field Col %d : %s'  name format error, example 'NAME:STRING' > %s, sheet %s", i, (LPCTSTR)fieldInfo.m_strName, xlsxFile.c_str(), sheet.c_str());
					//return AutoTable();
				}

				if (table->SetField(strList[0].c_str(), strList[1].c_str(), i) == NULL)
				{
					ERROR_LOG("%s : sheet %s Now may be no exist field %s : %s", xlsxFile.c_str(), sheet.c_str(), strList[0].c_str(), strList[1].c_str());
					database.Close();
					return AutoTable();
				}
			}

			table->GetField()->_updateInfo();
			if (!table->GetField()->check())
			{
				database.Close();
				ERROR_LOG("Field check fail > %s, sheet %s", xlsxFile.c_str(), sheet.c_str());
				return AutoTable();
			}			
		}
		else
		{
			database.Close();
			ERROR_LOG("xlsx file is empty > %s, sheet %s", xlsxFile.c_str(), sheet.c_str());
			return AutoTable();
		}

		// 默认第二行为说明, 不需要加载
		recset.MoveNext();

		int lineCount = 2;
		
		// 获取查询结果, NOTE: 每一行默认为字段名, 
		while (!recset.IsEOF())
		{
			++lineCount;

			recset.GetFieldValue((short)0, strItemData);

			if (strItemData.GetLength() <= 0)
			{
				// 如果是 数值类型, 写入了字符串, 则为不合法的值, 取值会异常, 所以只能修改为文本格式, 目前忽略丢弃处理
				//CDBVariant	 val;
				//recset.GetFieldValue(i, val);
				//WARN_LOG("Line %d value type of non conformity, then discard > %s, sheet %s", lineCount, xlsxFile.c_str(), sheet.c_str());				
				recset.MoveNext();
				continue;
			}

			if (table->GetRecord(strItemData)!=NULL)
			{
				WARN_LOG("xslx file %s, sheet %s, line %d  repeat, now discard", xlsxFile.c_str(), sheet.c_str(), lineCount);
				recset.MoveNext();
				continue;
			}

			ARecord record = table->CreateRecord(strItemData, true);
			//读取Excel内部数值			
			for (short i = 1; i < fieldCount; ++i)
			{								
				recset.GetFieldValue(i, strItemData);

				if (strItemData.GetLength() >= 253)
				{
					StringArray strList;
					AString::Split((LPCSTR)strItemData, strList, "\n", 1000);
					bool bCheck = false;
					for (int i = 0; i < strList.size(); ++i)
					{
						WString  s = strList[i].getWString();
						if (strList[i].length()>254 && wcslen(s.c_str()) != strList[i].length())
						{
							bCheck = true;
							break;
						}
					}
					if (bCheck)
					{
						if (mbInitLogFile)
						{
							mbInitLogFile = false;
							::DeleteFile(msLogFileName.c_str());
							FileDataStream f(msLogFileName.c_str(), FILE_CREATE);
							f.close();
						}

						AString info;
						info.Format("%s : %s key : %s >=254, 需要检查%s日志中读取是否正确(一行内不可超过255):\r\n%s\r\n\r\n", xlsxFile.c_str(), sheet.c_str(), record[0].c_str(), msLogFileName.c_str(), (LPCSTR)strItemData);
						LOG("%s", info.c_str());

						FileDataStream f(msLogFileName.c_str(), FILE_READ_WRITE);
						AutoData data = f.readAllData();
						if (f && data)
						{
							f._write(data->data(), data->dataSize());
							f._write((void*)info.c_str(), info.length());
						}
						f.close();
					}
					
				}

				if (record->set(i, (AString::getUTF8((LPCSTR)strItemData))) )
				{
					if (table->GetField()->getFieldInfo(i)->getType() == FIELD_ARRAYDATA)
					{
						AutoNice d = (tNiceData*)record[i];
							if (d)
							{
								for (auto it = d->begin(); it; ++it)
								{
									AString x = it.get();
										if (x.length() > 0 && *x.c_str() == ' ')
										{
											x.SplitLeft(" ");
											it.get() = x;
										}
								}
							}
					}
				}
				else
				{
					if (strItemData.GetLength()==255)
						ERROR_LOG("xslx file %s, sheet %s, line %d  field %s, value length >255, %s", xlsxFile.c_str(), sheet.c_str(), lineCount, table->GetField()->getFieldInfo(i)->getName().c_str(), (LPCTSTR)strItemData)
					else
						ERROR_LOG("xslx file %s, sheet %s, line %d  field %s, value %s is error", xlsxFile.c_str(), sheet.c_str(), lineCount, table->GetField()->getFieldInfo(i)->getName().c_str(), (LPCTSTR)strItemData);
				}
			}
			
			// 移到下一行
			recset.MoveNext();
		}

		// 关闭数据库
		database.Close();
		return table;
	}
		CATCH(CDBException, e)
	{
		// 数据库操作产生异常时...
		ERROR_LOG("数据库错误: 检查目录或文件名是否正确 > " + e->m_strError);
		
	}
	END_CATCH;

	return AutoTable();
}

//创建并写入Excel文件
//void WriteToExcel()
//{
//	CDatabase database;
//	CString sDriver = "Microsoft Excel Driver (*.xls)"; // Excel安装驱动
//	CString sExcelFile = "d:\\demo.xls"; // 要建立的Excel文件
//	CString sSql;
//
//	TRY
//	{
//		//sSql = "Driver={Driver do Microsoft Excel(*.xls)}; Dbq= " + sExcelFile + ";Extended Properties='Excel 8.0; HDR=Yes; IMEX=1'";
//		// 创建进行存取的字符串
//		//sSql.Format("DRIVER={%s};DSN='';FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\"%s\";DBQ=%s",sDriver, sExcelFile, sExcelFile);
//
//	// 创建数据库 (既Excel表格文件)
//	if (database.OpenEx(sSql,CDatabase::noOdbcDialog))
//	{
//		// 创建表结构(姓名、年龄)
//		sSql = "CREATE TABLE demo (Name TEXT,Age NUMBER)";
//		database.ExecuteSQL(sSql);
//
//		// 插入数值
//		sSql = "INSERT INTO demo (Name,Age) VALUES ('qinshujin',32)";
//		database.ExecuteSQL(sSql);
//
//		sSql = "INSERT INTO demo (Name,Age) VALUES ('jiyingjun',28)";
//		database.ExecuteSQL(sSql);
//
//		sSql = "INSERT INTO demo (Name,Age) VALUES ('zhangqi',28)";
//		database.ExecuteSQL(sSql);
//	}
//
//	// 关闭数据库
//	database.Close();
//	}
//		CATCH_ALL(e)
//	{
//		TRACE1("Excel驱动没有安装: %s", sDriver);
//	}
//	END_CATCH_ALL;
//}