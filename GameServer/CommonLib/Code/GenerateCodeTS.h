#pragma once
#ifndef _INCLUDE_GENERATECODE_H_
#define _INCLUDE_GENERATECODE_H_

#include "BaseTable.h"
#include "TableManager.h"
#include "CommonLib.h"

#define VERSION		"2.00.00"

class CommonLib_Export GenerateCodeTS
{
public:
	// path : 生成路径, 结尾需要 "/"
	static  AString GenerateCode(TableMap &tableList, AString path, const char *szDBClassName);

	static AString GenerateConfigTS(AutoTable table, bool bDefault = false);

	//static AString GenerateMsgProtocolTSCode(AutoNice proList, AutoNice proNotes, Array<AString> &structNameList, bool bFirst);



};


#endif //_INCLUDE_GENERATECODE_H_
