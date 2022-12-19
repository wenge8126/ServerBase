/********************************************************************
	created:	2011/07/10
	created:	10:7:2011   16:14
	filename: 	d:\NewTable__0709\NewTable\Common\DataBase\DataStream.h
	file path:	d:\NewTable__0709\NewTable\Common\DataBase
	file base:	DataStream
	file ext:	h
	author:		Yang Wenge
	
	purpose:	
*********************************************************************/
#ifndef	_INCLUDE_DATASTREAM_H_
#define _INCLUDE_DATASTREAM_H_

#include "BaseCommon.h"
#include "AutoString.h"
#include "Auto.h"
#include "Hand.h"

#include "Common.h"
//-------------------------------------------------------------------------
#define CHECK_READ(data, dest)  { if (!data->read(dest)) ERROR_LOG("loading failed!"); }
#define READ(d, result)		\
	if (!d->read(result))	\
	{						\
		WARN_LOG("Read data fail");\
		return false;		\
	}
	
//-------------------------------------------------------------------------
class DataBuffer;
//-------------------------------------------------------------------------
class BaseCommon_Export DataStream : public AutoBase // Base<DataStream>
{
public:
	virtual ~DataStream(){}

	virtual void Release(void);
	virtual void freeClass(){ Release();}

	virtual UINT Read(char *dest, UINT len) { return _read(dest, (DSIZE)len) ? len : 0; }
	virtual UINT	Write(const CHAR* buf, UINT len) { return _write((void*)buf, (DSIZE)len) ? len:0; }

	virtual bool IsLoopBuffer() const { return false; }

	virtual AString md5();

public:
	virtual DSIZE tell(void) const = 0;
	virtual bool seek(DSIZE absolutePosition) = 0;
	virtual bool setDataSize(DSIZE dataSize) = 0;

	virtual DSIZE size(void) const = 0;
	virtual DSIZE dataSize(void) const = 0;
	virtual char* data(void) = 0;
	virtual bool resize(DSIZE destSize, char fullValue = 0) = 0;
	virtual void clear(bool bFreeBuffer = false) = 0;

	inline virtual void* nowData(void);
	inline virtual DSIZE lastSize(void){ return size()-tell(); }
	inline virtual DSIZE lastDataSize(void){ return dataSize()-tell(); }
	inline virtual bool move(int offSize);
	inline virtual bool end(void){ return tell()==size(); }
	inline virtual bool empty(void){ return size()==0; }

public:
	bool write(const int nVal){ return _write((void*)&nVal, sizeof(int)); }
	bool write(const long lVal){ return _write((void*)&lVal, sizeof(long));  }
	bool write(const float fVal){ return _write((void*)&fVal, sizeof(float)); }
	bool write(const double dVal){ return _write((void*)&dVal, sizeof(double));}
	bool write(const short sVal){ return _write((void*)&sVal, sizeof(short)); }
	bool write(const char cVal){ return _write((void*)&cVal, sizeof(char)); }
	bool write(const bool bVal){ return _write((void*)&bVal, sizeof(bool)); }
	bool write(const Int64 n64Val){ return _write((void*)&n64Val, sizeof(Int64)); }

	bool write(const unsigned int nVal){ return _write((void*)&nVal, sizeof(unsigned int)); }
	bool write(const unsigned long lVal){ return _write((void*)&lVal, sizeof(unsigned long)); }
	bool write(const unsigned short sVal){ return _write((void*)&sVal, sizeof(unsigned short)); }
	bool write(const unsigned char cVal){ return _write((void*)&cVal, sizeof(unsigned char)); }
	bool write(const UInt64 n64Val){ return _write((void*)&n64Val, sizeof(UInt64)); }

	bool write(const AString &str ){ return writeString(str); }
	bool write(const char *szStr) { return writeString(szStr); }

	
	bool read(int &nVal){ return _read(&nVal, sizeof(int)); }
	bool read(long &lVal){ return _read(&lVal, sizeof(long)); }
	bool read(float &fVal){ return _read(&fVal, sizeof(float)); }
	bool read(double &dVal){ return _read(&dVal, sizeof(double));}
	bool read(short &sVal){ return _read(&sVal, sizeof(short)); }
	bool read(char &cVal){ return _read(&cVal, sizeof(char)); }
	bool read(bool &bVal){ return _read(&bVal, sizeof(bool)); }
	bool read(Int64 &n64Val){ return _read(&n64Val, sizeof(Int64)); }

	bool read(unsigned int &nVal){ return _read(&nVal, sizeof(unsigned int)); }
	bool read(unsigned long &lVal){ return _read(&lVal, sizeof(unsigned long)); }
	bool read(unsigned short &sVal){ return _read(&sVal, sizeof(unsigned short)); }
	bool read(unsigned char &cVal){ return _read(&cVal, sizeof(unsigned char)); }
	bool read(UInt64 &n64Val){ return _read(&n64Val, sizeof(UInt64)); }

	//bool read(AString &str ){ return readString(str); }
	bool read(AString &str ){ return readString(str); }

	// NOTE: 保存源缓存数据所有分配数据, 如需要只保存dataSize, 可指定保存长度
	inline bool writeData(DataStream *scrData){ return writeData(scrData, scrData!=NULL ? scrData->dataSize() : 0); }	
	inline bool writeData(DataStream *scrData, DSIZE writeLength);	
	inline bool readData(DataStream *destData);	
	
	inline virtual bool writeString(const AString &scr);

	inline virtual bool readString(AString &resultString);

	inline virtual AString readString(void);

	//inline virtual bool writeString(const AString &easyString);

	//inline virtual bool readString(AString &resultString);

	inline virtual bool writeWString(const wchar_t *wString);
	inline virtual const wchar_t* readWString(DSIZE &resultLength);

	virtual bool writeStringArray(const StringArray &scrArray);
	virtual bool readStringArray(StringArray &resultArray);

	template <typename T>
	bool writeArray(const Array<T> &scrArray)
	{
		write((DSIZE)scrArray.size());
		for (int i=0; i<scrArray.size(); ++i)
		{
			write(scrArray[i]);
		}
		return true;
	}

	template <typename T>
	bool _writeArray(const Array<T> &scrArray)
	{
		write((DSIZE)scrArray.size());
		for (int i=0; i<scrArray.size(); ++i)
		{
			_write((void*)&scrArray[i],sizeof(T));
		}
		return true;
	}

	template <typename T>
	bool readArray(Array<T> &resultArray)
	{
		DSIZE count = 0;
		if (!read(count) || count<0)
			return false;
		resultArray.resize(count);
		for (int i=0; i<count; ++i)
		{
			if (!read(resultArray[i]))
				return false;
		}

		return true;
	}
	template <typename T>
	bool _readArray(Array<T> &resultArray)
	{
		DSIZE count = 0;	
		if (!read(count))
			return false;
		resultArray.resize(count);
		for (int i=0; i<count; ++i)
		{
			if (!_read((void*)&resultArray[i],sizeof(T)))
				return false;
		}

		return true;
	}

public:
	virtual bool _write(void *scrData, DSIZE dataSize);

	virtual bool _read(void *destData, DSIZE readSize);


	virtual bool writeString(const char* scrString);

	virtual bool writeText(const char* scrString)
	{
		return _write((void*)scrString, (DSIZE)strlen(scrString) );
	}

	bool readText( AString &resultString );

	//virtual const char* readString(DSIZE &resultLength);

	virtual bool readLine(AString &resultString);

	virtual char* GetDataPtr(int &nSize) { nSize = dataSize(); return data(); }

	virtual bool skip(DSIZE len) { return seek(tell()+len); }

	virtual bool peek(void *dest, DSIZE len)
	{
		if (dataSize() >= len)
		{
			memcpy(dest, nowData(), len);
			return true;
		}
		return false;
	}
};
//-------------------------------------------------------------------------
typedef Auto<DataStream>		AutoData;


//-------------------------------------------------------------------------
// 用于消息的循环缓存
//-------------------------------------------------------------------------
class tLoopStream :public DataStream
{
public:	
	virtual UINT	Write(const CHAR* buf, UINT len) = 0;
	virtual UINT	Read(CHAR* buf, UINT len) = 0;	
	virtual bool	skip(DSIZE len) = 0;

	virtual bool _write(void *scrData, DSIZE dataSize) = 0;
	virtual bool _read(void *destData, DSIZE readSize) = 0;

	virtual bool peek(void *dest, DSIZE len) = 0;

	virtual bool IsLoopBuffer() const { return true; }
};
//-------------------------------------------------------------------------

class BaseCommon_Export Buffer
{
	friend class TempDataBuffer;

protected:
	typedef  AutoPtr<Buffer>  AutoBuffer;

	char   *m_pData;
	DSIZE  mLength;

public:
	~Buffer()
	{
		_free();
	}

	//	const char* operator () { return buf; }
	Buffer(DSIZE lenght);
	Buffer() { m_pData = NULL; mLength = 0; }
	Buffer(const char*  szBuf, const DSIZE lenght);

	void initData(byte initVal) { memset(data(), initVal, size()); }

	inline bool isHave() { return mLength > 0; }
	void _free();

	DSIZE	size(void)const { return mLength; }

	char* data(void)const { return m_pData; }

	bool resize(const DSIZE length, char defaultFullValue = 0);

	bool reset(const char*  szBuf, const DSIZE length);

	void add(const char *szBuf, const DSIZE lenght);

	// 减去前面多少数据
	bool dele_front(const DSIZE lenght);
	// 减去后面多少数据
	bool dele_back(const DSIZE lenght);
	bool write(DSIZE pos, void* data, DSIZE length);

	// 状态数据设置,关闭,读取
	bool isOpenState(DSIZE index);
	bool openState(DSIZE index);
	bool closeState(DSIZE index);

	Buffer & operator = (const Buffer &other)
	{
		resize(other.size());
		memcpy(m_pData, other.data(), mLength);
		return *this;
	}

	void swap(Buffer &other)
	{
		char *temp = m_pData;
		DSIZE tempLen = mLength;
		m_pData = other.m_pData;
		mLength = other.mLength;
		other.m_pData = temp;
		other.mLength = tempLen;
	}
};

typedef  AutoPtr<Buffer>  AutoBuffer;
//-------------------------------------------------------------------------

#endif //_INCLUDE_DATASTREAM_H_