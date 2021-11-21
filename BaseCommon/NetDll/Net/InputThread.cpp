#include "InputThread.h"
#include "BaseThread.h"

#include <stdio.h>
#if __WINDOWS__
#include <conio.h>
#	include <Windows.h>
#endif
void InputThread::backWorkThread(void)
{
	char Cmd[2];
	Cmd[1] = '\0';
	AString mCommString;
	while (IsActive())
	{				
		Cmd[0] = getchar(); 
		if ('`' == Cmd[0])
		{
			mpThread->RequestClose();
		}
		else if (Cmd[0] == '!')
		{
			//NOTE_LOG("Work average time %d, max loop time %d", mpThread->GetAverageLoopTime(), mpThread->GetMaxLoopTime());
		}
		else  if (Cmd[0] == '\n')
		{
			//OnInput(Cmd);			
			mpThread->ProcessCommand(mCommString.c_str());
			mCommString.setNull();
		}
		else
			mCommString += Cmd;
	}
}


void ControlThread::backWorkThread(void)
{
	char szInputString[33];
	while (IsActive())
	{
		if (mpThread->IsStoped())
		{
			if (mpThread->IsException())
				mpThread->Close();
			break;
		}
		Sleep(mOnceMilSec);

		//if (mpThread->ControlCheck()>mCheckMaxTime)		
		//{
		//	DEBUG_LOG("严重问题: 工作线程当前循环时间%d超过最大检查时间%d", mpThread->ControlCheck(), mCheckMaxTime);
		//	OnWorkThreadFault(mpThread, mpThread->ControlCheck(), mpThread->GetMaxLoopTime());
		//}
		//else
		//	DEBUG_LOG("Work average time %d, max loop time %d", mpThread->GetAverageLoopTime(), mpThread->GetMaxLoopTime());

		if (kbhit())
		{	
			fgets(szInputString, 32, stdin);
			// NOTE: 最后会包含一个回车符, 必须去掉
			if (strlen(szInputString)<=0)
				continue;
			szInputString[strlen(szInputString)-1] = '\0';
			if (strcmp(szInputString, "`")==0 )	
			{			
				mpThread->RequestClose();                
			}
			else if (strcmp(szInputString, "state")==0)
			{
				//NOTE_LOG("*Now state: work average time %d, max loop time %d", mpThread->GetAverageLoopTime(), mpThread->GetMaxLoopTime());
			}
			else
			{				
				mpThread->ProcessCommand(szInputString);
			}
		}
		
	}
}

