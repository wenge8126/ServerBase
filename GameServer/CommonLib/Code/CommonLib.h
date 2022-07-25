#pragma once


#ifndef CommonLib_Export
#ifdef STATE_COMMON_LIB
#	define CommonLib_Export
#	define CommonLib_Export_H
#else
#		ifdef COMMONLIB_EXPORTS
#			define CommonLib_Export __declspec(dllexport)
#			define CommonLib_Export_H	__declspec(dllexport)
#		else
#			define CommonLib_Export __declspec(dllimport)
#			define CommonLib_Export_H
#		endif
#endif
#endif