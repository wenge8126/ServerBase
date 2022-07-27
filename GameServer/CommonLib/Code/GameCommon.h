#pragma once

#include "CommonLib.h"

class CommonLib_Export_H GameCommon
{
public:
	static void Analysis(NiceData &msg, const AString &requestData)
	{
		Array<AString> tempList;
		AString::Split(requestData.c_str(), tempList, "&", 100);
		for (int i = 0; i < tempList.size(); i++)
		{
			Array<AString> str;
			AString::Split(tempList[i].c_str(), str, "=", 2);

			if (str.size() == 2)
			{
				msg[str[0].c_str()] = str[1].c_str();
			}
		}
	}
};
