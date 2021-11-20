
#ifndef _INCLUDE_IDNICEDATA_H_
#define _INCLUDE_IDNICEDATA_H_

#include "NiceData.h"
#include "Common.h"
#include "BaseTable.h"
#include "TableManager.h"
/*/-------------------------------------------------------------------------/
// 使用字符串唯一哈希整数索引
 NOTE: 优点: 占用空间小, KEY值整数比字符串要小很
			缺点: 在获取字符串哈希KEY时, 速度较慢
---------------------------------------------------------------------------*/
#pragma pack(push)
#pragma pack(1)

class BaseCommon_Export IDNiceData : public tNiceData
{
	typedef tNiceData base;
#if !USE_AUTOPTR_ITERATOR
	friend struct tNiceData::iterator;
#endif
public:
	IDNiceData(const tNiceData &other)
	{
		*(tNiceData*)this = other;
	}
	IDNiceData(){}

	virtual NICEDATA_TYPE getType() const { return ID_NICEDATA; }

	virtual AutoNice NewNice() override { return MEM_NEW IDNiceData(); }

    static int ToKeyID(const char *szKey) {
        return TableManager::getSingleton().mStringKeyIndex.KeyID(szKey);
    }

	virtual AData& _getStaticData() const { return mDataMap._getStaticValue().mVal; }

public:	
	Data get(const char* key) const{ return base::get(key); }

	bool set(const char* key, int nVal)
	{
		return base::set(key, nVal);
	}

	bool set(const char* key, float fVal)
	{
		return base::set(key, fVal);
	}

	bool set(const char* key, bool bVal)
	{
		return base::set(key, bVal);
	}

	bool set(const char* key, unsigned char val)
	{
		return base::set(key, val);
	}

	bool set(const char* key, const char* szVal)
	{
		return base::set(key, szVal);
	}

	bool set(const char* key, const AString &strVal)
	{
		return base::set(key, strVal);
	}

	bool set(const char* key, const Data &kVal)
	{
		return base::set(key, kVal);
	}

	bool set(const char* key, Data &kVal)
	{
		return base::set(key, kVal);
	}

	bool set(const char* key, UInt64 nValU64)
	{
		return base::set(key, nValU64);
	}

	bool set(const char* key, void *obj, const type_info &typeInfo)
	{
		return base::set(key, obj, typeInfo);
	}

	bool get(const char* key, void *obj, const type_info &typeInfo) const
	{
		return base::get(key, obj, typeInfo);
	}

	template<typename T>
	bool set(const char* key, T &obj)
	{
		return base::set(key, obj);
	}

	template<typename T>
	bool get(const char* key, T &obj)
	{
		return base::get(key, obj);
	}

public:
	virtual AData& getOrCreate(const char* key) override;
	virtual AData& getAData(const char *szKey) const override
	{
		return mDataMap.find(ToKeyID(szKey));
	}

	virtual size_t count() const
	{
		return mDataMap.size();
	}

	virtual bool serialize(DataStream *destData) const override;
	virtual bool restore(DataStream *scrData) override;

	virtual bool append(int index, const AData &data, bool bReplace = false) override
	{
		if (bReplace)
			mDataMap.erase(index);
		else if (mDataMap.exist(index))
		{
			WARN_LOG("Already exist >[%d]", index);
			return false;
		}
		mDataMap.insert(index, data);
		return true;
	}
	virtual bool append( const AString &index, const AData &data, bool bReplace = false ) override
	{
		return append(TableManager::getSingleton().mStringKeyIndex.KeyID(index.c_str()), data, bReplace);
	}

	virtual bool remove(const char* key) override { return mDataMap.erase(TableManager::getSingleton().mStringKeyIndex.KeyID(key)); }

	size_t append(const tNiceData &scrData, bool bReplace) override;

	virtual void clear(bool bFreeBuffer = true) override
	{
		mDataMap.clear(bFreeBuffer);
	}

	virtual void swap(tNiceData &other) override
	{
		IDNiceData *p = dynamic_cast<IDNiceData*>(&other);
		AssertEx (p!=NULL, "必须相同类型交换");

		mDataMap.swap(p->mDataMap);		
	}

	virtual Data _getData(int nKey) const override
	{
		AData *dest = mDataMap.findPtr(nKey);

		if (NULL==dest)
		{
			mDataMap.insert(nKey, AData());
            dest = mDataMap.findPtr(nKey);
           
		}
        
        Data temp(dest);
        
        return temp;
	}

	virtual void initData() override
	{
		for (auto it=mDataMap.begin(); it.have(); it.next())
		{
			auto &kV = it.Value();
			kV.mVal.free();
		}
		mDataMap.clear(true);
	}

	virtual AString ToJSON() override 
	{ 
		ERROR_LOG("IDNiceData not can to json, because unkwon key string");
		return AString("ERROR IDNiceData:  not can to json, because unkwon key string");
	}

protected:
	mutable EasyMap<ID_TYPE, AData, 1, true>				mDataMap;

#if USE_AUTOPTR_ITERATOR
	class Iterator : public iterator
	{
	public:
		Iterator(const IDNiceData *pNice)
		{
			mHashIt = pNice->mDataMap.begin();
			mpHash = &pNice->mDataMap;
		}

		virtual AData& operator * () override
		{
			return get();
		}

		virtual const AString& key() override
		{
			msKey = STRING(mHashIt.key());
			return msKey;
		}
		virtual AData& get()
		{
			return mHashIt.get();
		}

		virtual bool next() 
		{
			return mHashIt.next();
		}
		virtual bool have() 
		{
			return mHashIt.have();
		}

		virtual void reset() 
		{
			mHashIt.reset();
		}

	protected:
		EasyHash<int, AData>::iterator mHashIt;
		EasyHash<int, AData> *mpHash;
		AString msKey;
	};

	virtual iterator begin() const override { return MEM_NEW Iterator(this); }
#else
	virtual iterator begin() const override { return iterator((IDNiceData*)this); }
#endif
};
//-------------------------------------------------------------------------*/
#pragma pack(pop)	

#endif //_INCLUDE_IDNICEDATA_H_