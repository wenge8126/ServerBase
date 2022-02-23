#ifndef _INCLUDE_BASEMSG_H_
#define _INCLUDE_BASEMSG_H_

#pragma once

#include "NetHead.h"

#include "PacketFactory.h"
#include "NiceData.h"
//#include "ShareMemCloudDB.h"
#include "TimeManager.h"
#include "BaseRecord.h"

#define CheckGet(d, dest) { Data v = d[#dest]; if (v.empty()) {} else dest = v; }

#define SAVE_MSG_VALUE(member, fieldType) Save(#member, fieldType, member, destData);

#define SAVE_MSG_NICE(member)  destData->write(#member); destData->write((byte)FIELD_NICEDATA); if (member) { destData->write((byte)member->getType()); member->serialize(destData); }   else destData->write((byte)NULL_NICEDATA);
#define SAVE_MSG_DATA(member)  destData->write(#member); destData->write((byte)FIELD_DATA); destData->writeData((DataStream*)member.getPtr(), member?member->dataSize():0); 

#define  SAVE_MSG_STRUCT(member) destData->write(#member);  destData->write((byte)FIELD_NICEDATA);  destData->write((byte)NICEDATA); member.serialize(destData);

#define  COPY_MSG_NICE(scr, destNice) destNice = MEM_NEW NiceData(); if (scr) destNice->append(*scr, true);
#define  COPY_MSG_DATA(scr, destData) destData = MEM_NEW DataBuffer(); if (scr) { destData->resize(scr->dataSize()); destData->_write(((DataStream*)scr.getPtr())->data(), scr->dataSize()); }

#if DEVELOP_MODE
#	define _MSG_ARRAY_LIMIT 2000
#	define _ShowBox(info) MyMessageBox(info)
#else
#	define _MSG_ARRAY_LIMIT	3000
#	define _ShowBox(info)
#endif


#define CHECK_ARRAY_SIZE(array, arrayType) if (array.size()>_MSG_ARRAY_LIMIT) {\
AString info; info.Format("Msg <%s> array size can not %llu > %d, Now give up more", arrayType, array.size(), _MSG_ARRAY_LIMIT); \
ERROR_LOG(info.c_str()); _ShowBox(info.c_str()); count = _MSG_ARRAY_LIMIT; }

namespace NetCloud
{
	class ShareMemCloudDBNode;
}

using namespace NetCloud;

class tBaseMsg : public BasePacket
{
public:
	tBaseMsg(const tBaseMsg &other)
	{
		AssertNote(0, "No use tBaseMsg(other) code");
	}

	tBaseMsg(){}

	virtual ~tBaseMsg() {}	

	virtual AData& _getStaticData() const 
	{
		thread_local static AData ms;
		return ms;
	}

public:
	int Exe(NetCloud::ShareMemCloudDBNode *pNode, const AString &tableName, AutoNice &paramData, AutoNice &resultData)
	{
		Full(paramData);
		auto now_t = TimeManager::NowTick();
		int ret = Run(pNode, tableName, paramData, resultData);
		now_t = TimeManager::NowTick() - now_t;
		if (now_t > 300)
			WARN_LOG("proto name:%s time out:%lld", GetMsgName(), now_t);
		
		return ret;
	}

	virtual int Run(NetCloud::ShareMemCloudDBNode *pNode, const AString &tableName, AutoNice &paramData, AutoNice &resultData)
	{
		ERROR_LOG("No write code"); 
		return 0;
	}

	virtual AData get(const char *szMember) const = 0;
	virtual bool set(const char *szMember, AData value) = 0;
	virtual AData operator [] (const char *szMember) const = 0;

public:
	template<typename T>
	static bool SaveArray(const char *szKey, const Array<T> &scrArray, DataStream *destData)
	{
		// key , type
		destData->write(szKey);

		destData->write((byte)FIELD_NICEDATA);

		// AData 字段保存
		destData->write((byte)ARRAY_NICEDATA);

		// ArrayNiceData:: serialize
		int count = scrArray.size();
		if (count >= 0xFFFF)
		{
			ERROR_LOG("Array count too more, %d >= %u", count, 0xFFFF);
			count = 0xFFFF;
		}
		
		CHECK_ARRAY_SIZE(scrArray, typeid(T).name());

		destData->write((ushort)count);

		tFieldInfo *info = FieldInfoManager::getFieldInfo(typeid(T));
		if (info == NULL)
		{
			ERROR_LOG("No exist type field > %s", typeid(T).name());
			return false;
		}
		for (int i = 0; i < count; ++i)
		{
			//bool AData::serialize(DataStream *destData) const
			destData->write((byte)info->getType());
			destData->write(scrArray[i]);
		}
		return true;
	}

	template<typename T>
	static bool SaveArray(const Array<T> &scrArray, DataStream *destData)
	{
		if (scrArray.size() > 1000)
		{
			ERROR_LOG("Msg array size can not %llu > 1000", scrArray.size());
			return false;
		}
		// AData 字段保存
		destData->write((byte)ARRAY_NICEDATA);

		// ArrayNiceData:: serialize
		int count = scrArray.size();
		if (count >= 0xFFFF)
		{
			ERROR_LOG("Array count too more, %d >= %u", count, 0xFFFF);
			count = 0xFFFF;
		}
		
		CHECK_ARRAY_SIZE(scrArray, typeid(T).name());

		destData->write((ushort)count);

		tFieldInfo *info = FieldInfoManager::getFieldInfo(typeid(T));
		if (info == NULL)
		{
			ERROR_LOG("No exist type field > %s", typeid(T).name());
			return false;
		}
		for (int i = 0; i < count; ++i)
		{
			//bool AData::serialize(DataStream *destData) const
			destData->write((byte)info->getType());
			destData->write(scrArray[i]);
		}
		return true;
	}

	template<typename T>
	static bool SaveMsgArray(const char *szKey, const Array<T> &scrArray, DataStream *destData)
	{

		destData->write(szKey);

		destData->write((byte)FIELD_NICEDATA);

		destData->write((byte)ARRAY_NICEDATA);

		// ArrayNiceData:: serialize
		int count = scrArray.size();
		if (count >= 0xFFFF)
		{
			ERROR_LOG("Array count too more, %d >= %u", count, 0xFFFF);
			count = 0xFFFF;
		}
		
		CHECK_ARRAY_SIZE(scrArray, typeid(T).name());

		destData->write((ushort)count);

		for (int i = 0; i < count; ++i)
		{
			//bool AData::serialize(DataStream *destData) const
			destData->write((byte)FIELD_NICEDATA);
			// AutoNiceFieldInfo::saveData
			destData->write((byte)NICEDATA);
			scrArray[i].serialize(destData);
		}
		return true;
	}

	template<typename T>
	static void Save(const char *szKey, int fieldType, const T &value, DataStream *destData)
	{
		destData->write(szKey);
		destData->write((byte)fieldType);
		destData->write(value);
	}

	template<typename T>
	static void FullArray(AutoNice scrData, Array<T> &dest, const char *szName)
	{
		AutoNice d = (tNiceData*)scrData[szName];
		if (d)
		{
			int i = 0; dest.resize(d->count());
			for (auto m = d->begin(); m; ++m)
			{
				dest[i++] = m.get();
			}
		}
	}

	template<typename T>
	static void FullArray(AutoNice scrData, Array<T> &dest)
	{
		AutoNice &d = scrData;
		if (d)
		{
			int i = 0; dest.resize(d->count());
			for (auto m = d->begin(); m; ++m)
			{
				dest[i++] = m.get();
			}
		}
	}

	template<typename T>
	static void FullStuctArray(AutoNice scrData, Array<T> &dest, const char *szName)
	{
		AutoNice d = (tNiceData*)scrData[szName];
		if (d)
		{
			int i = 0; dest.resize(d->count());
			for (auto m = d->begin(); m; ++m)
			{
				AutoNice subData; m.get().get(&subData, typeid(subData));
				if (subData) dest[i++].Full(subData); else ERROR_LOG("%s is not NiceData", m.key().c_str());
			}
		}
	}

	template<typename T>
	static void ArrayToData(AutoNice destData, Array<T> &scrArray, const char *szName)
	{
		AutoArrayData d = MEM_NEW ArrayNiceData();
		if (d)
		{
			for (int i = 0; i < scrArray.size(); ++i)
			{
				d[i] = scrArray[i];
			}
		}
		destData[szName].set(&d, typeid(AutoArrayData));
	}

	template<typename T>
	static void ArrayToData(AutoNice &destData, Array<T> &scrArray)
	{
		AutoArrayData d = MEM_NEW ArrayNiceData();
		destData = d;
		if (d)
		{
			for (int i = 0; i < scrArray.size(); ++i)
			{
				d[i] = scrArray[i];
			}
		}
	}

	template<typename T>
	static void StuctArrayToData(AutoNice destData, Array<T> &scrArray, const char *szName)
	{
		AutoArrayData d = MEM_NEW ArrayNiceData();
		if (d)
		{
			for (int i = 0; i < scrArray.size(); ++i)
			{
				AutoNice val = destData->NewNice();
				scrArray[i].ToData(val);
				d[i] = val.getPtr();
			}
		}
		destData[szName].set(&d, typeid(AutoArrayData));
	}

	template<typename T>
	static void CopyMsgArray(const Array<T> &other, Array<T> &dest)
	{
		int count = (int)other.size();
		
		CHECK_ARRAY_SIZE(other, typeid(T).name());
		
		dest.clear(false);
		dest.resize(count);

		for (int i = 0; i < count; ++i)
		{
			dest[i].copy(other[i]);
		}
	}

public:
	// 导出工具自动生成保存功能
	virtual bool serialize(DataStream *destData) const = 0;
	virtual void Full(AutoNice scrData) = 0;
	virtual void ToData(AutoNice &destData) = 0;



	virtual void clear(bool bFreeBuffer = true) = 0;
	virtual void copy(const tBaseMsg &other) {}

	virtual void FullFromRecord(ARecord scrData) 
	{
		AutoNice data = scrData->ToNiceData();
		Full(data);
	}
	virtual void SaveToRecord(ARecord &destData) 
	{
		AutoNice tempData = MEM_NEW NiceData();
		ToData(tempData);
		int count = destData->getField()->getCount();
		for (int i=0; i<count; ++i)
		{
			FieldInfo info = destData->getField()->getFieldInfo(i);
			destData->get(i).setData(tempData[info->getName()]);
		}
	}

	virtual tBaseMsg& operator = (AutoNice other)
	{
		Full(other);  return *this;
	}

	virtual tBaseMsg& operator = (const tBaseMsg &other)
	{
		copy(other);  
		return *this;
	}

public:

	// NOTE: 使用 IDNiceData 临时转换, 可以兼容之前老的结构
	// NOTE: 特别注意, 如果新的结构中不存在对应的字段, 老的数据将被丢弃!
	virtual bool restore(DataStream *scrData)
	{
		thread_local static AutoNice idNice = MEM_NEW NiceData();
		if (!idNice->restore(scrData))
		{
			ERROR_LOG("tBaseMsg restore fail");
			return false;
		}
		Full(idNice);
		return true;
	}


	virtual AString ToJSON() 
	{
		thread_local static AutoNice  tempNice = MEM_NEW NiceData();
		tempNice->clear(false);
		ToData(tempNice);
		return tempNice->ToJSON();
	}


	virtual AString dump(int sub = 0, int code = 0) const 
	{
		thread_local static AutoNice temp = MEM_NEW NiceData();
		temp->clear(false);
		((tBaseMsg*)this)->ToData(temp);
		return temp->dump();
	}

public:
	virtual UINT		GetState() const { return 0; }
	virtual VOID		SetState(UINT stateData) {}

	virtual BOOL		Read(DataStream& iStream, size_t packetSize, tNetConnect*) override
	{ 
		AutoNice temp = MEM_NEW NiceData();

		if (temp->restore(&iStream))
		{
			Full(temp);
			return TRUE;
		}
		else
			ERROR_LOG("Restore msg %s data fail", GetMsgName());
		return FALSE;
	}
	virtual BOOL		Write(DataStream& oStream) const override
	{ 
		return serialize(&oStream); 
	}

	//virtual UINT		Execute(tNetConnect* pConnect) { return 0; }


};

class tRequestMsg : public tBaseMsg
{
public:
	virtual void SetRequestID(MSG_ID id) = 0;
	virtual MSG_ID GetRequestID() = 0; // { ERROR_LOG("%s is not reqeust msg", GetMsgName());  return 0; }
};

#endif //_INCLUDE_BASEMSG_H_