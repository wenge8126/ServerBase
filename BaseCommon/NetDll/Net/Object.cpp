#include "Object.h"
#include "Common.h"
#include "ObjectFactory.h"
#include <stdarg.h>

int tObject::GetType(void) const
{
	return mFactory->GetType();
}

void tObject::Log(const char *szInfo, ...)
{
#if DEVELOP_MODE
	va_list va;
	va_start(va,szInfo);
	TableTool::DebugLog(va,szInfo);
#endif
}