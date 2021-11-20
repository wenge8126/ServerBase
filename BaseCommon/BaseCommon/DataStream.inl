//#include "DataStream.h"


inline bool DataStream::writeData( DataStream *scrData, DSIZE writeLength )
{
	if (writeLength==0)
		return _write(&writeLength, sizeof(DSIZE));

	if (scrData==NULL)	
	{
		writeLength = 0;
		return _write(&writeLength, sizeof(DSIZE));
	}

	if (scrData->size()<writeLength)
		return false;

	bool re = _write(&writeLength, sizeof(DSIZE));

	if (writeLength > 0 && re)
	{
		//NOTE: ���ԴΪ����Buffer, �����ȶ���, Ȼ�����д��, ֱ��д��, �п��ܻ��ȡβ��+��Ч����
		if (scrData->IsLoopBuffer())
		{
			Buffer		tempBuf(writeLength);
			if (!scrData->peek(tempBuf.data(), writeLength)) 
				return false;
			return _write(tempBuf.data(), writeLength);
		}
		return _write(scrData->data(), writeLength);
	}
	return re;
}

const int _MAX_BUFFER_READ_SIZE	= 512*1024*1024;

inline bool DataStream::readData( DataStream *destData )
{
	AssertEx(destData!=NULL, "��ȡ����Ŀ�� destData Ϊ��");

	DSIZE len = 0;
	if (read(len))
	{
		if (len>0)
		{
			if (len<=_MAX_BUFFER_READ_SIZE)
			{
				//NOTE: ���Ŀ��Ϊ����Buffer, �����ȶ���, Ȼ�����д��Ŀ��, ֱ��д��, �п��ܻ��ȡβ��+��Ч����
				if (destData->IsLoopBuffer())
				{
					Buffer		tempBuf(len);
					if (!_read(tempBuf.data(), len))
						return false;
					return destData->_write(tempBuf.data(), len);
				}

				if (destData->size()<len)
					if (!destData->resize(len))
					{
						ERROR_LOG("Resize data fail >%d", len);
						return false;
					}

				if ( _read(destData->data(), len) )
				{
					destData->seek(len);
					destData->setDataSize(len);
					return true;
				}			

				destData->clear();
				return false;
			}
			else
			{
				ERROR_LOG("DataStream::readData len more then [%d]", _MAX_BUFFER_READ_SIZE);
				return false;
			}
		}
		return true;
	}
	return false;
}

inline bool DataStream::writeString( const AString &scr )
{
	return writeString(scr.c_str());
}

//inline bool DataStream::writeString( const AString &easyString )
//{
//	return writeString(easyString.c_str());
//}

inline bool DataStream::writeString( const char* scrString)
{
	DSIZE scrLength = (DSIZE)strlen(scrString);
	AssertNote(scrLength<=0xffff, "���л��ַ������֧�ֳ���65535");
	if (scrLength>0xFFFF)
		return false;
	bool b = write((StrLenType)scrLength);
	AssertEx(b, "�����ַ�������ʧ��");
	if (scrLength>0)
		return _write((void*)scrString, scrLength);

	return true;
}

inline bool DataStream::readString( AString &resultString )
{
	StrLenType reLen = 0;
	if (!read(reLen))
		return false;

	if (reLen>0)
	{
		resultString.Alloc(reLen+1);
		if ( !_read(resultString._str(), reLen) )
		{
			resultString.setNull();
			return false;
		}
	}
	else
		resultString.setNull();

	return true;
}

inline AString DataStream::readString( void )
{
	AString resultString;

	StrLenType reLen = 0;
	if (!read(reLen))
		return resultString;

	if (reLen>0)
	{
		resultString.Alloc(reLen+1);
		bool b = _read(resultString._str(), reLen);
		AssertEx(b, "��ȡ�ַ���ʧ��");
	}

	return resultString;
}

//inline bool DataStream::readString( AString &resultString )
//{
//	StrLenType reLen = 0;
//	if (!read(reLen))
//		return false;
//
//	resultString.free();
//	if (reLen>0)
//	{
//		if (lastSize()>=reLen)
//		{
//			resultString.set((const char*)nowData(), reLen);			
//			bool re = move((int)reLen);
//			AssertEx(re, "λ�ñ�Ӧ����ȷ�ƶ�");
//			return true;
//		}		
//		else
//			return false;
//	}	
//
//	return true;
//}

//inline const char* DataStream::readString( DSIZE &resultLength )
//{
//	const char* szResult = (const char*)nowData();
//	if (szResult==NULL)
//		return NULL;
//
//	resultLength = strlen( szResult )+1;
//	// last must have '\0', so < last size.
//	if (resultLength<=lastSize())
//	{
//		move((int)resultLength);
//		return szResult;
//	}
//
//	return NULL;
//}

inline bool DataStream::_write( void *scrData, DSIZE dataSize )
{
	if (lastSize() < dataSize)
	{
		if (!resize(size() * 2))
			return false;
		if (lastSize() < dataSize)
		{
			if (!resize(tell() + dataSize))
				return false;
		}
	}
	AssertEx(lastSize()>=dataSize, "�ռ�Ӧ�÷��乻��");
	memcpy(nowData(), scrData, dataSize);
	bool re = move((int)dataSize);
	AssertEx(re, "λ�ñ�Ӧ����ȷ�ƶ�");
	return re;
}

inline bool DataStream::_read( void *destData, DSIZE readSize )
{
	if (lastDataSize()>=readSize)
	{
		memcpy(destData, nowData(), readSize);
		bool re = move((int)readSize);
		AssertEx(re, "λ�ñ�Ӧ����ȷ�ƶ�");
		return true;
	}
	return false;
}


inline void* DataStream::nowData( void )
{
	if (tell()<size()) 
		return (char*)data() + tell(); 
	else 
		return NULL;
}

inline bool DataStream::move( int offSize )
{
	if (offSize==0)
		return true;

	if (offSize<0)
	{
		if (tell() >= (DSIZE)(-offSize))
		{
			seek(tell()+offSize);
			return true;
		}
	}
	else 
	{
		if (tell()+offSize <= size() )
		{
			seek(tell()+offSize);
			return true;
		}
	}
	return false;
}


inline bool DataStream::writeWString(const wchar_t *wString)
{	
	DSIZE length = (DSIZE)wcslen(wString) + 1;
	return _write((void*)wString, length * sizeof(wchar_t));
}

inline const wchar_t* DataStream::readWString(DSIZE &resultLength)
{
	const wchar_t* szResult = (const wchar_t*)nowData();
	resultLength = (DSIZE)((wcslen( szResult )+1) * sizeof(wchar_t));
	// last must have '\0', so < last size.
	if (resultLength<=lastDataSize())
	{
		move((int)resultLength);
		return szResult;
	}

	return NULL;
}