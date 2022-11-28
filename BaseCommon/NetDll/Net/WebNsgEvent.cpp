#include "WebMsgEvent.h"

//-------------------------------------------------------------------------
void WebNetEventCenter::RegisterMsg(Logic::AutoEventFactory hFactory)
{
	int id = hFactory->GetID();
	if (id < mArrayIndex.size() && mArrayIndex[id])
		ERROR_LOG("Already exist event factory %d >%s", id, mArrayIndex[id]->GetEventName());
	RegisterEvent(hFactory->GetEventName(), hFactory);
	hFactory->SetID(id);
	if (id >= mArrayIndex.size())
		mArrayIndex.resize(id + 8);

	mArrayIndex[id] = hFactory;
}

void WebNetEventCenter::RegisterMsg(int eventIndex, Logic::AutoEventFactory factory)
{
	factory->SetID(eventIndex);
	AString name;
	name.Format("WebMsgEvent_%d", eventIndex);
	factory->SetEventName(name.c_str());
	RegisterMsg(factory);
}

bool WebNetEventCenter::SerializeMsg(AutoEvent &hEvent, DataStream *destData)
{
	int id = hEvent->GetEventFactory()->GetID();
	if (id > 0xffff >> 1)
	{
		ERROR_LOG("[%s] Event ID over 65535", hEvent->GetEventName());
		return false;
	}

	destData->write((short)id);

	if (hEvent->_Serialize(destData))
		return true;

	ERROR_LOG("get event [%s] data stream error", hEvent->GetEventNameInfo().c_str());
	return false;
}

AutoEvent WebNetEventCenter::RestoreMsg(DataStream *scrData)
{
	AutoEvent hEvent;
	short id = 0;
	if (!scrData->read(id))
	{
		ERROR_LOG("Read event  id fail");
		return AutoEvent();
	}
	if (id > 0)
	{
		hEvent = _Start(id, 0);
		if (!hEvent)
		{
			ERROR_LOG("根据ID创建事件失败>[%d], 未注册事件", id);
		}
		else
		{
			if (hEvent->_Restore(scrData))
				return hEvent;
			ERROR_LOG("Error: restor event [%s] form data stream", hEvent->GetEventName());
		}
	}
	else if (id < 0)
		ERROR_LOG("恢复事件 ID 为 %d", id);

	return hEvent;
}

AutoEvent WebNetEventCenter::_Start(ushort eventID, int eventNameIndex)
{
	if (eventID > 0 && eventID < mArrayIndex.size())
	{
		Logic::AutoEventFactory hF = mArrayIndex[eventID];
		if (hF)
		{
			return hF->StartEvent();
		}
	}
	return AutoEvent();
}

WebNetEventCenter::WebNetEventCenter()
{
	mResponseEventFactory->SetID(WEB_MSG_RESPNSE_MSG);
	if (mArrayIndex.size() <= 1)
		mArrayIndex.resize(8);
	mArrayIndex[WEB_MSG_RESPNSE_MSG] = mResponseEventFactory;
}

//----------------------------------------------------------------------