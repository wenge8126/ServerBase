/********************************************************************
created:	2013/02/26
created:	26:2:2013   21:21
filename: 	C:\NewGame\Common\DataBase\tNiceData.h
file path:	C:\NewGame\Common\DataBase
file base:	tNiceData
file ext:	h
author:		Wenge Yang

purpose:	数据容器
支持保存自身指针对象, 可以保存结构数据
*********************************************************************/
#ifndef _INCLUDE_NICEDATA_H_
#define _INCLUDE_NICEDATA_H_


#include "Auto.h" 
#include "EasyHash.h"
#include "EasyMap.h"

#include "AData.h"

#define USE_JSON_LIB								1		// 使用 json 0.5.0 lib 解析或生成JSON串
//-------------------------------------------------------------------------*/
#define  USE_HASH_NICEDATA				0			// 使用HASH容器保存元素(空间占用大,速度快, 用于逻辑事件数据)
#define USE_AUTOPTR_ITERATOR 			0			// 使用接口迭代对象方式, 缺点: 每次迭代需要new 迭代对象
typedef AString			DataKeyType;

#define IS_NICE			(mNiceType==NICEDATA)
#define IS_ARRAY		(mNiceType==ARRAY_NICEDATA)
#define IS_IDNICE			(mNiceType==ID_NICEDATA)
//-------------------------------------------------------------------------*/
enum NICEDATA_TYPE
{
	NULL_NICEDATA,
	NICEDATA,
	ARRAY_NICEDATA,
	ID_NICEDATA,
	DB_NICEDATA,
};
class IDNiceData;
class DataStream;
//----------------------------------------------------------------
#if USE_JSON_LIB
namespace Json { class Value; }
#endif
//----------------------------------------------------------------
class tBaseTable;
#pragma pack(push)
#pragma pack(1)
class BaseCommon_Export tNiceData : public AutoBase
{
protected:
	typedef Auto<tNiceData>		AutoNice;

public:
	AString										msKey;

public:
	tNiceData()
	{

	}

	tNiceData(const tNiceData &other)
	{
		*this = other;
	}

	virtual ~tNiceData()
	{

	}

	virtual NICEDATA_TYPE getType() const = 0;

	virtual AutoNice NewNice() = 0;

	virtual AData& _getStaticData() const = 0;

	virtual void OnClearFromRecord() { }

public:
    Data operator [] (const char *szValueName)
    {
        AData &d = getOrCreate(szValueName);
        Data temp(&d);

        return temp;
    }

	Data operator [] (const AString &szValueName)
	{		
        AData &d = getOrCreate(szValueName.c_str());
        Data temp(&d);

        return temp;
	}

	virtual Data operator [] (const int intName)
	{		
        AData &d = getOrCreate(STRING(intName));
        Data temp(&d);

        return temp;
	}

	virtual tNiceData& operator = (const tNiceData &other)
	{
		clear(false);
		append(other, true);
		return *this;
	}

public:
	bool set(const char* key, int nVal)
	{
		AData &d = getOrCreate(key);
		return d.set(nVal);
	}

	bool set(const char* key, float fVal)
	{
		AData &d = getOrCreate(key);
		return d.set(fVal);
	}

	bool set(const char* key, bool bVal)
	{
		AData &d = getOrCreate(key);
		return d.set(bVal);
	}

	bool set(const char* key, unsigned char val)
	{
		AData &d = getOrCreate(key);
		return d.set(val);
	}

	bool set(const char* key, const char* szVal)
	{
		AData &d = getOrCreate(key);
		return d.set(szVal);
	}

	bool set(const char* key, const AString &strVal)
	{
		AData &d = getOrCreate(key);
		return d.set(strVal);
	}

	bool set(const char* key, const Data &kVal)
	{
		AData &d = getOrCreate(key);
		return d.set(kVal);
	}

	bool set(const char* key, Data &kVal)
	{
		AData &d = getOrCreate(key);
		return d.set(kVal);
	}

	bool set(const char* key, Int64 nValU64)
	{
		AData &d = getOrCreate(key);
		return d.set(nValU64);
	}

	bool set(const char* key, UInt64 nValU64)
	{
		AData &d = getOrCreate(key);
		return d.set(nValU64);
	}

	Data get(const char* key) const;
	Data get(const AString &key) const { return get(key.c_str()); }

	//NOTE: Result data only temp use, for autoptr [] operate
	virtual Data _getData(const char *szKey) const override
	{
		AData &d = ((tNiceData*)this)->getOrCreate(szKey);	
        Data temp(&d);

		return temp;			
	}

	virtual Data _getData(int intName) const { return _getData(STRING(intName)); }

	tBaseTable* getTable(const char *key);

	AutoNice getNice(const char *key)
	{
		AutoNice d;
		get(key, &d, typeid(d));
		return d;
	}

	bool set(const char* key, void *obj, const type_info &typeInfo)
	{
		AData &d = getOrCreate(key);
		return d.set(obj, typeInfo);
	}

	bool get(const char* key, void *obj, const type_info &typeInfo) const;

	template<typename T>
	bool set(const char* key, T &obj)
	{
		AData &d = getOrCreate(key);
		return d.set(&obj, typeid(T));
	}

	template<typename T>
	bool get(const char* key, T &obj)
	{
		AData &d = getAData(key);		
		return d.get(&obj, typeid(T));		
	}

public:
	virtual void initData();

	AutoNice getData(const char *szIndex)
	{
		AutoNice result;
		get(szIndex, result);
		return result;
	}

	bool exist(const char *key) const { return !getAData(key).empty(); }
	bool existData(const char *keyList, AString *resultInfo = NULL);

	//NOTE: will change param string
	virtual bool FullJSON(AString scrJsonString);
	// Parse string array to tNiceData, ["KEY1", "1", "KEY2", "2"]
	virtual bool FullByStringArray(AString scrStringArray);

#if USE_JSON_LIB
	virtual bool FullJSON(const char *szKey, Json::Value*);
	virtual AString  ToJSON(Json::Value *, tNiceData *pComment, tNiceData *pMainComment, int space)
	{
		ERROR_LOG("No write code");
		return AString();
	}
	static bool FullToAData(const char *szKey, Json::Value *pScrJSON, AData &destAData);
#endif
	virtual AString ToJSON() = 0;

	virtual AString dump(int sub = 0, int code = 0) const ;


	virtual bool SetStringArray(const char *szKey, StringArray &scrArray);
	virtual bool GetStringArray(const char *szKey, StringArray &resultArray);

	template <typename T>
	bool SetArray(const char *szKey, const Array<T> &scrArray)
	{
		AutoData data = MEM_NEW DataBuffer(64);
		data->write((DSIZE)scrArray.size());
		for (int i=0; i<scrArray.size(); ++i)
		{
			data->write(scrArray[i]);
		}
		return set(szKey, data);
	}
	template <typename T>
	bool GetArray(const char *szKey, Array<T> &resultArray)
	{
		AutoData data;
		get(szKey, data);
		if (data)
		{
			DSIZE count = 0;
			data->seek(0);
			if (!data->read(count))
				return false;
			resultArray.resize(count);
			for (int i=0; i<count; ++i)
			{
				if (!data->read(resultArray[i]))
					return false;
			}

			return true;
		}
		return false;
	}

	virtual bool empty() const { return count()<=0; }

public:
	virtual AData& getOrCreate(const char* key) = 0;
	virtual AData& getAData(const char *szKey) const = 0;

	virtual bool append(int index, const AData &data, bool bReplace = false) = 0;
	virtual bool append( const AString &index, const AData &data, bool bReplace = false ) = 0;

	virtual bool serialize(DataStream *destData) const = 0;
	virtual bool restore(DataStream *scrData) = 0;

	bool remove(const AString &key){ return remove(key.c_str()); }
	virtual bool remove(const char* key) = 0;
	virtual void clear(bool bFreeBuffer = true) { AssertEx(0, "Can not run this"); }
	virtual void swap(tNiceData &other) = 0;

	virtual size_t append(const tNiceData &scrData, bool bReplace) = 0;
	virtual size_t count() const = 0;

    virtual AData& getOrCreate(int pos) { return getOrCreate(STRING(pos)); }

public:
#if USE_AUTOPTR_ITERATOR
	class _iterator : public AutoBase
	{
	public:
		_iterator(){}
		virtual ~_iterator(){}

		virtual AData& operator * () = 0;
		virtual const AString& key() = 0;
		virtual AData& get() = 0;

		virtual bool next() = 0;
		virtual bool have() = 0;
		virtual void erase() = 0;
		virtual void reset() = 0;

		operator bool (){ return have(); }
		bool operator ++(){ return next(); }
		bool operator ++(int){ return next(); }
	};

	virtual Auto<_iterator> begin() const = 0;

#else
	struct BaseCommon_Export iterator
	{
		// 兼容之前的写法
		//iterator* operator -> ()  { return this; } 
		operator bool()
		{
			return have();
		}		

		void operator ++ ()
		{
			next();
		}
		void operator ++ (int)
		{
			next();
		}

	public:
		virtual AData& operator * ()
		{
			return get();
		}
		virtual const AString& key();
		virtual AData& get()
		{
			if (IS_ARRAY)
			{
				if (mCurrentPos < mpDataList->size())
					return (*mpDataList)[mCurrentPos];
				return AData::getStatic();
			}
			else if (IS_NICE)
				return mHashIt.get();
			else
				return mIDMapIt.get();
		}

		virtual bool next() 
		{
			if (IS_ARRAY)
			{
				++mCurrentPos;
				return mCurrentPos < mpDataList->size();
			}
			else if (IS_NICE)
				return mHashIt.next();
			else
				return mIDMapIt.next();
		}
		virtual bool have() 
		{
			if (IS_ARRAY)
				return mCurrentPos < mpDataList->size();
			else if (IS_NICE)
				return mHashIt.have();
			else
				return mIDMapIt.have();
		}
		virtual void erase()
		{
			if (IS_ARRAY)
				mpDataList->remove(mCurrentPos);
			else if (IS_NICE)
				mHashIt.erase();
			else
				mIDMapIt.erase();
		}
		virtual void reset() 
		{
			if (IS_ARRAY)
				mCurrentPos = 0;
			else if (IS_NICE)
				mHashIt.reset();
			else
				mIDMapIt.reset();
		}

	public:
		iterator()
			:mNiceType(false)
			, mCurrentPos(0)
			, mpDataList(NULL)			
		{}

		iterator(NiceData *mpNice);
		iterator(ArrayNiceData *mpNice);
		iterator(IDNiceData *pIDNice);

	protected:
		int				mNiceType;
		int				mCurrentPos;
		Array<AData>	*mpDataList;

#if USE_HASH_NICEDATA
		EasyHash<DataKeyType, AData>::iterator mHashIt;
#else
		EasyMap<DataKeyType, AData, 4>::Iterator mHashIt;
#endif
		EasyMap<int, AData, 1, true>::Iterator mIDMapIt;
	};
	virtual iterator begin() const = 0;
#endif
};


typedef Auto<tNiceData>				AutoNice;
#if USE_AUTOPTR_ITERATOR
typedef Auto<tNiceData::_iterator>	NiceIt;
#endif

//-------------------------------------------------------------------------*/

class BaseCommon_Export NiceData : public tNiceData
{
	typedef tNiceData base;
	friend struct iterator;

public:
	NiceData(const tNiceData &other)
	{
		*(tNiceData*)this = other;
	}
	NiceData(){}

	virtual NICEDATA_TYPE getType() const { return NICEDATA; }

	virtual void initData() override;

	virtual AutoNice NewNice() override { return MEM_NEW NiceData(); }

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
	virtual AString ToJSON() override;

	virtual AString ToFastJSON();

#if USE_JSON_LIB
	AString ToJSONEx(tNiceData *pMainCommentInfo, tNiceData *pNowCommentInfo  =NULL);
	virtual AString  ToJSON(Json::Value *, tNiceData *pCommentInfo, tNiceData *pMainComment, int space) override;
#endif

public:
	virtual AData& getOrCreate(const char* key);
	virtual AData& getOrCreate(const AString &key){ return getOrCreate(key.c_str()); }

	virtual AData& getAData(const char *szKey) const
	{
		AData *p = (AData*)mDataMap.findPtr(szKey);

		if (p!=NULL)
			return *p;

		//static AData d;
		return ((tNiceData*)(this))->_getStaticData();
	}

	virtual bool append(int index, const AData &data, bool bReplace = false) override;
	virtual bool append( const AString &index, const AData &data, bool bReplace = false ) override;

	virtual bool serialize(DataStream *destData) const override;
	virtual bool restore(DataStream *scrData) override;

#if USE_HASH_NICEDATA
	virtual bool remove(const char* key) override { return mDataMap.erase(key); }	
#else
    virtual bool remove(const char* key) override { return mDataMap.erase(key)>0; }	
#endif
	size_t append(const tNiceData &scrData, bool bReplace);

	virtual void clear(bool bFreeBuffer = true) override
	{
		mDataMap.clear(bFreeBuffer);
	}

	virtual void swap(tNiceData &other) override
	{
		NiceData *p = dynamic_cast<NiceData*>(&other);
		AssertEx (p!=NULL, "必须相同类型交换");
	
		mDataMap.swap(p->mDataMap);		
	}

	virtual size_t count() const
	{
		return mDataMap.size();
	}

protected:
#if USE_HASH_NICEDATA
	mutable EasyHash<DataKeyType, AData>	mDataMap;	
#else
    mutable EasyMap<DataKeyType, AData, 4>	mDataMap;	
#endif
public:
#if USE_AUTOPTR_ITERATOR
	class Iterator : public _iterator
	{
	public:
		Iterator(const NiceData *pNice)
		{
			mHashIt = pNice->mDataMap.begin();
			//mpHash = &pNice->mDataMap;
		}
		virtual AData& operator * () override
		{
			return get();
		}
		virtual const AString& key() override
		{
			return mHashIt.key();
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

		virtual void erase() override
		{
			mHashIt.erase();
		}

		virtual void reset() 
		{
			mHashIt.reset();
		}

	protected:
#if USE_HASH_NICEDATA
		EasyHash<DataKeyType, AData>::iterator mHashIt;
#else
        EasyMap<DataKeyType, AData, 4>::Iterator mHashIt;
#endif //USE_HASH_NICEDATA 
		//EasyHash<DataKeyType, AData> *mpHash;
	};

	virtual Auto<_iterator> begin() const override { return MEM_NEW Iterator(this); }
#else
	virtual iterator begin() const { return iterator((NiceData*)this); }
#endif
};

typedef Auto<NiceData>	ANice;

#pragma pack(pop)		// NiceData size = 145
//-------------------------------------------------------------------------*
// 数组方式保存数据，使用枚举下标方式取值, 应用于一些常用的优化消息中
// 序列数据小，使用取值快速, NOTE: 数据最多不能超过 0xffff
//-------------------------------------------------------------------------*
class BaseCommon_Export ArrayNiceData	: public tNiceData
{
	friend struct iterator;

	static int toIndex(const char *szIndex)
	{
		int x = TOINT(szIndex);
		if (x==0)
		{
			// 如果为零，则必须第1个字符为零
			if (strlen(szIndex)>0 && szIndex[0]=='0')
				return x;
			return -1;
		}
		return x;
	}

public:
	virtual NICEDATA_TYPE getType() const { return ARRAY_NICEDATA; }

	virtual AData& _getStaticData() const { return mDataList._getStatic(); }

	// 如果不存在增长创建
	virtual Data operator [] (const int intName) override
	{				
		AssertEx(intName>=0, "数组下标不可为负数");

		if (intName>0xFFFF)
		{
			ERROR_LOG("数组下标超过 65535");
			return Data();
		}

		if (intName>=(int)mDataList.size())
			mDataList.resize(intName+1);

		AData &d = mDataList[intName];
        Data temp(&d);

        return temp;
	}

	virtual Data _getData(const char *szKey) const 
	{
		int x = toIndex(szKey);
		if (x>=0)
			return _getData(x); 
		return Data();
	}
	virtual Data _getData(int intName) const 
	{
		return (*(ArrayNiceData*)this)[intName];
	}

	virtual bool empty() const
	{
		return mDataList.empty();
	}

	virtual AString dump(int sub = 0, int code=0) const override;

public:
	virtual AData& getOrCreate(const char* key) 
	{
		int x = toIndex(key);
		if (x<0)
		{
			//static AData s;
			return _getStaticData();
		}
		return _get(x);
	}

	virtual AData& getAData(const char *szKey) const
	{
		int x = toIndex(szKey);
		if (x<0)
		{
			//static AData s;
			return ((tNiceData*)(this))->_getStaticData();
		}
		return *(AData*)&_get(x);
	}

    virtual AData& getOrCreate(int pos) override
    {
        int x = pos;
        if (x<0)
        {
            //static AData s;
            return _getStaticData();
        }
        return _get(x);
    }

    virtual AData& getAData(int pos) const
    {
        int x = pos;
        if (x<0)
        {
            //static AData s;
            return ((tNiceData*)(this))->_getStaticData();
        }
        return *(AData*)&_get(x);
    }

	virtual const AString getKeyString(size_t pos) const { return AString(STRING((int)pos)); }

	virtual bool append(int index, const AData &data, bool bReplace = false);
	virtual bool append( const AString &index, const AData &data, bool bReplace = false )
	{
		int x = toIndex(index.c_str());
		if (x<0)
			return false;
		return append(x, data, bReplace);
	}

	virtual bool serialize(DataStream *destData) const;
	virtual bool restore(DataStream *scrData);

	virtual bool remove(const char* key) 
	{
		int x = toIndex(key);
		if (x>=0 && x<mDataList.size())
		{
			mDataList[x].free();
			return true;
		}
		return false;
	}
	virtual void clear(bool bFreeBuffer = true) { mDataList.clear(bFreeBuffer); }
	virtual void swap(tNiceData &other)
	{
		ArrayNiceData *pScr = dynamic_cast<ArrayNiceData*>(&other);
		if (pScr!=NULL)
		{
			mDataList.swap(pScr->mDataList);
		}
		else
		{
			ERROR_LOG("ArrayNiceData 交换对象必须是相同类型 [%s]", typeid(other).name());
		}
	}

	virtual size_t append(const tNiceData &scrData, bool bReplace);
	virtual size_t count() const { return mDataList.size(); }

    //NOTE: 不支持子NiceData 生成JSON字符串
	virtual bool FullJSON(AString scrJsonString) override;
	virtual AString ToJSON() override;
#if USE_JSON_LIB
	virtual AString  ToJSON(Json::Value *, tNiceData *pComment, tNiceData *pMainComment, int space) override;
	virtual bool FullJSON(const char *szKey, Json::Value *pJV) override;
#endif
	virtual AutoNice NewNice() override;

protected:
	virtual AData& _get(size_t pos)const
	{
		if (pos>0xFFFF)
		{
			ERROR_LOG("数组下标超过 65535");
			//static AData s;
			return _getStaticData();
		}
		if (pos>=0)
		{
			if (pos>=mDataList.size())
				mDataList.resize(pos+1);
			return mDataList[pos];
		}
		//static AData s;
		return _getStaticData();
	}

protected:
	mutable Array<AData>	mDataList;

public:
#if USE_AUTOPTR_ITERATOR
	class Iterator : public _iterator
	{
	public:
		Iterator(const ArrayNiceData *pNice)
			: mpDataList(&pNice->mDataList)
		{
			//mHashIt = pNice->mDataMap.begin();
			mCurrentPos = 0;
		}
		virtual AData& operator * () override
		{
			return get();
		}
		virtual const AString& key() override
		{
			_THREAD_LOCAL_ static AString msKey;
			if (mCurrentPos<mpDataList->size())
				msKey = STRING(mCurrentPos);
			else
				msKey = STRING(-1);
			return msKey;
		}
		virtual AData& get()
		{
			if (mCurrentPos<mpDataList->size())
				return (*mpDataList)[mCurrentPos];

			return AData::getStatic();
		}

		virtual bool next() 
		{
			++mCurrentPos;
			return mCurrentPos<mpDataList->size();
		}
		virtual bool have() 
		{
			return mCurrentPos<mpDataList->size();
		}
		virtual void erase() override
		{
			mpDataList->remove(mCurrentPos);
		}
		virtual void reset() 
		{
			mCurrentPos = 0;
		}

	protected:
		int			mCurrentPos;
		Array<AData>	*mpDataList;

		///use THREAD_LOCAL static AData;

	};

	virtual Auto<_iterator> begin() const override { return MEM_NEW Iterator(this); }
#else
	virtual iterator begin() const { return iterator((ArrayNiceData*)this); }
#endif
};


typedef Auto<ArrayNiceData> AutoArrayData;
//-------------------------------------------------------------------------*/
//#	include "IDNiceData.h"

//-------------------------------------------------------------------------*/

#endif //_INCLUDE_NICEDATA_H_