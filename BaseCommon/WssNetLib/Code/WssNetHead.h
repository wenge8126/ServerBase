#pragma once


#ifndef WssNet_Export
#ifdef STATE_WSS_NET
#define WssNet_Export
#define WssNet_Export_H	
#else
#		ifdef WSS_NET_EXPORTS
#			define WssNet_Export __declspec(dllexport)
#			define WssNet_Export_H	__declspec(dllexport)
#		else
#			define WssNet_Export __declspec(dllimport)
#			define WssNet_Export_H	
#		endif
#endif
#endif
