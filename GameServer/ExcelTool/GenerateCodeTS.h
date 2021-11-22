#pragma once
#ifndef _INCLUDE_GENERATECODE_H_
#define _INCLUDE_GENERATECODE_H_

#include "BaseTable.h"
#include "TableManager.h"

#define VERSION		"1.00.00"

class GenerateCodeTS
{
public:
	// path : 生成路径, 结尾需要 "/"
	static  AString GenerateCode(TableMap &tableList, AString path, const char *szDBClassName);

	static AString GenerateConfigTS(AutoTable table, bool bDefault = false);
};


#endif //_INCLUDE_GENERATECODE_H_
