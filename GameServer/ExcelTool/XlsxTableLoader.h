#pragma once
#ifndef _INCLUDE_XLSXTABLELOADER_H_
#define _INCLUDE_XLSXTABLELOADER_H_

#include "BaseTable.h"

class XlsxTableLoader : public AutoBase
{
public:
	static AutoTable LoadXlsx(const AString &xlsxFile, const AString &sheet);

public:
	static AString	msLogFileName;
	static bool		mbInitLogFile;
};


#endif //_INCLUDE_XLSXTABLELOADER_H_
