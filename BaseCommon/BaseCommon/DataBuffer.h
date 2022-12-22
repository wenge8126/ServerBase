#ifndef _INCLUDE_BUFFER_H_
#define _INCLUDE_BUFFER_H_


#include "BaseCommon.h"

#include "DataStream.h"
#include "DataStream.inl"

#define GROWTH_BUFFER_LENGTH	128

//--------------------------------------------------------------------------------------
#ifndef EXCEPTION_CATCH
#define EXCEPTION_CATCH	...
#endif
//--------------------------------------------------------------------------------------
typedef unsigned short StateDataType;
//--------------------------------------------------------------------------------------
// 开关
class BaseCommon_Export StateData
{
public:
	StateData()
		: mOpenData(0)
	{
	}

	StateData(StateDataType stateData)
		: mOpenData(stateData)
	{

	}

	operator StateDataType () const { return mOpenData; }

	StateData& operator = (StateDataType stateData)
	{
		mOpenData = stateData;
		return *this;
	}

	void init(bool bAllClose)
	{
		if (bAllClose)
			mOpenData = 0;
		else
			mOpenData = 0xFFFF;
	}

	bool operator == (const StateData &other)
	{
		return mOpenData==other.mOpenData;
	}

public:
	bool isOpen(StateDataType state) const
	{
		return (mOpenData & state)!=0;
	}
	void open(StateDataType state)
	{
		mOpenData |= state;
	}

	void close(StateDataType state)
	{
		mOpenData &= ~state;
	}

	void set(StateDataType state, bool bOpen)
	{
		bOpen ? open(state) : close(state);
	}

protected:
	StateDataType	mOpenData;
};
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
class  Buffer;



//-------------------------------------------------------------------------*/
enum ZIP_OPTION
{
	STANDARD,
	MAX_SPEED,
	MAX_ZIP_RATE,
};
//--------------------------------------------------------------------------------------
// 可以像文件一件进行读写的数据缓存 [杨文鸽 2010/4/27]
//--------------------------------------------------------------------------------------
class BaseCommon_Export DataBuffer : public DataStream
{
	//typedef Hand<DataBuffer>		AutoData;

protected:
	Buffer	mData;
	DSIZE	mCurrent;
	DSIZE	mDataSize;

public:
	virtual ~DataBuffer()
	{

	}

	DataBuffer()
		: mCurrent(0)
		, mDataSize(0)
	{

	}

	DataBuffer( DSIZE lenght )
		: mData( lenght )
		, mCurrent(0)
		, mDataSize(0)
	{

	}

	void Release(void){ delete this; }
	Auto<DataStream> GetSelf(){ return Auto<DataStream>(this); }

public:
	virtual DSIZE tell(void) const { return mCurrent; }
    // NOTE: 设置数据时会改变有效数据大小: 移动数据位置时,大于数据位置后,重置数据大小到数据位置
	virtual bool seek(DSIZE absolutePosition) 
	{
		if (absolutePosition>=0 && absolutePosition<=mData.size())
		{
			mCurrent = absolutePosition;

			// 保存最大使用的数据大小
			if (mCurrent>mDataSize)
				mDataSize = mCurrent;

			return true;
		}
		return false;
	}
	virtual bool setDataSize(DSIZE dataSize)
	{ 
		if (dataSize<=size())
		{
			mDataSize = dataSize; 
			return true;
		}
		return false;
	}
	virtual DSIZE size(void) const { return mData.size(); }
	virtual DSIZE dataSize(void) const {return mDataSize; }
	virtual char* data(void) { return mData.data(); }
	virtual const char* data(void)const { return mData.data(); }
	virtual bool resize(DSIZE destSize, char fullValue = 0)
	{
		return mData.resize(GROWTH_BUFFER_LENGTH*int((destSize+GROWTH_BUFFER_LENGTH-1)/GROWTH_BUFFER_LENGTH), fullValue);
	}

	//virtual void initData(byte initVal)
	//{ 
	//	memset(data(), initVal, size()); 
	//	mCurrent = 0;
	//	mDataSize = 0; 
	//}

	virtual void clear(bool bFreeBuffer = false)
	{
		if (bFreeBuffer)
			mData._free(); 
		else
			memset(data(), 0, size());

		mCurrent = 0; 
		mDataSize = 0;
	}

public:
	virtual bool setData(const char* scrData, DSIZE scrSize)
	{
		resize(scrSize);
		seek(0);
		return _write((void*)scrData, scrSize);
	}

	//void swap(DataBuffer &other);

	DSIZE ZipData(AutoData &resultBuffer, DSIZE destDataPosition = 0, DSIZE scrDataPos = 0, DSIZE scrSize = 0, ZIP_OPTION op = STANDARD );
	bool UnZipData(AutoData &resultBuffer, DSIZE resultBeginPosition, DSIZE destSize=0, DSIZE zipDataPos = 0, DSIZE zipSize = 0 );

public:
	static DSIZE Zip(void *scrData, DSIZE scrSize, void* resultZipDataBuffer, DSIZE resultSize);
	static bool UnZip( void *scrZipData, int scrZipSize, void *resultData, int resultDataSize );


};
//#ifndef STATE_BASECOMMON_LIB
//template<>
//void Hand<DataBuffer>::FreeClass(DataBuffer *obj)
//{
//	obj->Release();
//}
//#endif

typedef Auto<DataBuffer>	HandDataBuffer;

class TempDataBuffer : public DataBuffer
{
public:
	TempDataBuffer(){}
	TempDataBuffer(void *szData, DSIZE size)
	{
		mData.m_pData = (char*)szData;
		mData.mLength = size;
		setDataSize(size);
	}

	~TempDataBuffer()
	{
		mData.m_pData = NULL;
		mData.mLength = 0;
	}

	void InitSet(void *szData, DSIZE size)
	{
		mData.m_pData = (char*)szData;
		mData.mLength = size;
		setDataSize(size);
	}

	virtual bool resize(DSIZE destSize, char fullValue = 0) override
	{
		ERROR_LOG("Not can use TempDataBuffer::resize");
		return false;
	}

	Buffer& GetBuffer(){ return mData; }

	virtual void clear(bool bFreeBuffer = false) override
	{
		//if (bFreeBuffer)
		//	mData._free();
		//else
		//	memset(data(), 0, size());

		mCurrent = 0;
		mDataSize = 0;
	}
};


#endif //  _INCLUDE_BUFFER_H_
