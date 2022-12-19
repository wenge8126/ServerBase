
#include "DataStream.h"
#include "Md5Tool.h"

void DataStream::Release(void)
{
	delete this;
} 

AString DataStream::md5()
{
	if (data() == NULL)
		return AString();

	MD5 md(data(), dataSize());
	return md.toString();
}

bool DataStream::readText( AString &resultString )
{
	DSIZE nowPos = tell();

	int strLen = 0;
	while (true)
	{
		char t = '\0';
		if (!_read((void*)t, 1))
			return false;

		if (t=='\0')
			break;

		++strLen;
	}
	if (strLen>0)
	{
		resultString.Alloc(strLen+1);
		seek(nowPos);
		return _read(resultString._str(), strLen+1);
	}
	return true;
}

bool DataStream::readLine(AString &resultString)
{
	if (empty())
		return false;
	char r[2];
	r[0] = 0;
	r[1] = '\0';
	char temp = 0;
	while (read(r[0]))
	{
		if (r[0]=='\r')
		{
			if (read(temp))
			{
				if (temp=='\n')
					break;
			}
		}
		else if (r[0]=='\n')
			break;

		resultString += r;
	}
	return true;
}

bool DataStream::writeStringArray(const StringArray &scrArray )
{
	write((DSIZE)scrArray.size());
	for (size_t i=0; i<scrArray.size(); ++i)
	{
		writeString(scrArray[i]);
	}
	return true;
}

bool DataStream::readStringArray( StringArray &resultArray )
{
	DSIZE count = 0;
	READ(this, count);
	resultArray.resize(count);
	for (size_t i=0; i<resultArray.size(); ++i)
	{
		if (!readString(resultArray[i]))
			return false;
	}
	return true;
}


bool Buffer::resize(const DSIZE desLen, char defaultFullValue)
{

	if (desLen == mLength)
		return true;

	if (desLen == 0)
	{
		_free();
		return true;
	}

	char *temp = m_pData;

	m_pData = (char*)ALLOCT_NEW(desLen);

	AssertEx(m_pData != NULL, "Alloct memory fail");

	if (m_pData == NULL)
		return false;

	if (desLen > mLength)
	{
		if (NULL != temp)
			memcpy(m_pData, temp, mLength);
		memset(m_pData + mLength, defaultFullValue, desLen - mLength);
	}
	else if (NULL != temp)
		memcpy(m_pData, temp, desLen);

	if (temp != NULL)
		Allot::freePtr(temp, mLength);

	mLength = desLen;

	return true;
}

bool Buffer::reset(const char* szBuf, const DSIZE length)
{
	if (length == 0)
	{
		_free();
		return true;
	}

	if (mLength != length)
	{
		_free();
		m_pData = (char*)ALLOCT_NEW(length);
		AssertEx(m_pData, "Now create data is null, create fail");
		mLength = length;
	}

	memcpy(m_pData, szBuf, length);

	return true;

}

void Buffer::add(const char *szBuf, const DSIZE lenght)
{

	char *p = (char*)ALLOCT_NEW(lenght);

	if (m_pData)
		memcpy(p, m_pData, mLength);
	memcpy(p + mLength, szBuf, lenght);
	if (m_pData)
		Allot::freePtr(m_pData, mLength);
	m_pData = p;
	mLength = mLength + lenght;
}

Buffer::Buffer(DSIZE lenght)
{
	m_pData = NULL;
	mLength = 0;
	resize(lenght);
}

Buffer::Buffer(const char* szBuf, const DSIZE lenght)
{
	m_pData = NULL;
	mLength = 0;
	reset(szBuf, lenght);
}
bool Buffer::dele_front(const DSIZE lenght)
{
	if (lenght == 0) return false;
	if (mLength <= lenght)
	{
		_free();
		return false;
	}
	else
	{
		Buffer	autoBuf(m_pData + lenght, mLength - lenght);
		//reset( autoBuf->m_pData,autoBuf->mLength );
		swap(autoBuf);
	}
	return true;
}

bool Buffer::dele_back(const DSIZE lenght)
{
	if (lenght == 0) return false;
	if (mLength <= lenght)
	{
		_free();
		return false;
	}
	else
	{
		Buffer autoBuf(m_pData, mLength - lenght);
		//reset( autoBuf->m_pData,autoBuf->mLength );
		swap(autoBuf);
	}
	return true;
}

void Buffer::_free()
{
	if (m_pData)
	{
		Allot::freePtr(m_pData, mLength);
		m_pData = NULL;
		mLength = 0;
	}
}

bool Buffer::write(DSIZE pos, void* data, DSIZE length)
{
	if (mLength - pos < length)
		return false;

	memcpy(m_pData + pos, data, length);

	return true;
}

bool Buffer::isOpenState(DSIZE index)
{
	if (index >= 0 && index < mLength * 8)
		return (m_pData[index / 8] & (1 << (index % 8))) > 0;
	return false;
}

bool Buffer::openState(DSIZE index)
{
	if (index >= 0 && index < mLength * 8)
	{
		m_pData[index / 8] |= (1 << (index % 8));
		return true;
	}
	return false;
}

bool Buffer::closeState(DSIZE index)
{
	if (index >= 0 && index < mLength * 8)
	{
		m_pData[index / 8] &= ~(1 << (index % 8));
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------