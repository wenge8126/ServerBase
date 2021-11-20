/********************************************************************
created:	2011/07/21
created:	21:7:2011   0:12
filename: 	d:\Work\Common\DataBase\MemString.h
file path:	d:\Work\Common\DataBase
file base:	MemString
file ext:	h
author:		���ĸ�

purpose:	
*********************************************************************/
#ifndef _INCLUDE_AUTOSTRING_H_
#define _INCLUDE_AUTOSTRING_H_

#include "BaseCommon.h"
#include "AutoPtr.h"
#include "StringTool.h"

#include "Assertx.h"
#include "Array.h"

//-------------------------------------------------------------------------*/
#define TOUTF8(sz)		AString::getUTF8(sz).c_str()
#define TOANIS(sz)		AString::getANIS(sz).c_str()
//-------------------------------------------------------------------------*/
#if DEBUT_STRING
#	include <set>
#endif
//----------------------------------------------------------------------
#define		SIZE_SPACE_LENGTH	sizeof(unsigned short)
//----------------------------------------------------------------------
//#ifdef __LINUX__
class WString :  public Array<wchar_t>
{
public:
	const wchar_t* c_str() const
	{
		return Array<wchar_t>::mpArrayPtr;
	}

	wchar_t* str()
	{
		return Array<wchar_t>::mpArrayPtr;
	}

public:
	WString()
	{
		Array<wchar_t>::resize(1);
		Array<wchar_t>::mpArrayPtr[0] = '\0';
	}
};
//#else
//template<>
//void AutoPtr<wchar_t>::FreeClass()
//{
//	delete[] getPtr();
//}
//typedef AutoPtr<wchar_t>		WString;
//
//#endif

//------------------------------------------------------------------------------------
// ʹ���ַ����汣���ַ�����Ϣ���ַ�������
// mStringBuffer 
// 1 ������Ϊ0����ʾֱ�ӱ����ַ�
// 2 ���Ϊ'P', ��ʾ�����ַ�������Ŀռ䣬8�ֽں󱣴����ռ���Ա�����ַ�����, �µĿռ俪ͷ���ֽ�Ϊ���ü���,֮��Ϊ������ַ�������
//------------------------------------------------------------------------------------

#define  StringSpaceInfoSize  (sizeof(UseType)+4)


// �� DEBUT_STRING =0 >�����ܳ���Ϊ 15, ��������8��, ��Ϊ��Ҫ8���ֽڱ���ָ��
// �Ż�ADataʱ��ʹ��AData ռ�� 17���ֽڣ���һ���ֽ�Ϊ�ֶ����ͣ�ʹ�� EasyMap KeyValueռ��32���ֽ�
#define STRING_BUFFER_LENGTH		15	// ���� STRING_BUFFER_LENGTH+1 �ֽڿռ�
#define IS_USE_STRING_PTR			(mStrBuffer[STRING_BUFFER_LENGTH]==('P'))

#pragma pack(push)
#pragma pack(1)

class BaseCommon_Export AString
{
public:
	void Format(const char* pFormat, ...);
	void Format(va_list &valist, const char *pFormat);


	const char* c_str() const
	{
		if (IS_USE_STRING_PTR)
			return _strBuffer_const()+StringSpaceInfoSize;

		return mStrBuffer;
	}

	size_t length() const
	{
		return strlen(c_str());
	}

	bool empty()const
	{
		if (IS_USE_STRING_PTR)
			return NULL==_strBuffer_const();
		return mStrBuffer[0] == '\0';
	}	

	void swap(AString &other)
	{
		AString t = *this;
		*this = other;
		other = t;
	}

	void set(const char *szStr)
	{
		_set(szStr);
	}

	void replace(const char *szScrStr, const char *szDest);

	void replaceFirst(const char *szScrStr, const char *szDest);

	void replaceLast(const char *szScrStr, const char *szDest);

	void free()
	{
		setNull();
	}

public:
	AString operator + (const char *szString);
	AString operator + (const AString &str);
	AString operator + (const wchar_t *wString);

	AString operator + (int nVal);
	AString operator + (float fVal);
	AString& operator += (const char *szString)
	{
		return link(szString);
	}

	AString& operator += (const AString &str)
	{
		return link(str.c_str());
	}

	AString& operator += (const wchar_t *wString);

	AString& operator += (int nVal);
	AString& operator += (float fVal);
	AString& operator += ( double dVal );
	AString& operator += (UInt64 uValU64);
	AString& operator+=(Int64 uValU64);

	void linkInt64(BIGINT i64);

	bool getU64(UInt64  &uResult);
	UInt64 getU64(void){ UInt64 v; if (!getU64(v)) v = 0;  return v; }

	bool operator < (const AString &other) const
	{
		return strcmp(c_str(),other.c_str())<0;
	}
	bool operator > (const AString &other) const
	{
		return strcmp(c_str(), other.c_str())>0;
	}
	bool operator == (const AString &other) const
	{
		return strcmp(c_str(),other.c_str())==0;
	}	
	bool operator != (const AString &other) const
	{
		return ! (*this == other);
	}

	bool operator < (const char *szString) const
	{
		return strcmp(c_str(), szString)<0;
	}
	bool operator > (const char *szString) const
	{
		return strcmp(c_str(), szString)>0;
	}
	bool operator == (const char* szString) const
	{
		return strcmp(c_str(), szString)==0;
	}
	bool operator != (const char* szString) const
	{
		return strcmp(c_str(), szString)!=0;
	}

	AString& link(const char *szString);
	WString getWString(uint pagecode = 0) const;

	void toUpper();
	void toLower();
	AString UTF8() const;
	AString ANIS() const;

	int getKeyID() const ;

	AString sub(int startPos, int subLength = 0);
	int replace(char scrChar, char destChar);

	// �����Ƴ���ʼ����, NOTE: ��������, Ҳ��ͬʱ�ı�
	void removeBeginning(int beginningPos);

	static AString getUTF8(const char *scr, int pageCode = 0);

	static AString getANIS( const char *szUTF8 );

	static int Split(const char *scrString, Array<AString> &resultStringList, const char *delims, int maxSplits);

	AString SplitLeft(const char *splitString);

	// ��ȡ {  } ֮����ı�, ֧��Ƕ���� { {a} {b} } ���� "{a} {b}" 
	AString SplitBlock(const char s1= '{', const char s2 = '}', bool bSkipSplitChar = false, bool *pHave = NULL);

	void set(const char *sz, DSIZE len);

	void setNull()
	{
		_setnull();
	}

public:
	~AString()
	{
		_setnull();
	}

	AString()
	{
		memset(mStrBuffer, 0, STRING_BUFFER_LENGTH+1);
	}
	AString(size_t allocSize)
	{
		memset(mStrBuffer, 0, STRING_BUFFER_LENGTH + 1);
		Alloc(allocSize);
	}
	AString(const AString &other) 
	{
		memset(mStrBuffer, 0, STRING_BUFFER_LENGTH+1);
		*this = other;
	}
	AString( const char* szString ) 
	{
		memset(mStrBuffer, 0, STRING_BUFFER_LENGTH+1);
		*this = szString;
	}

	AString( const wchar_t* wString ) 
	{
		memset(mStrBuffer, 0, STRING_BUFFER_LENGTH+1);
		*this = wString;
	}

	AString& operator = (const AString &other)
	{
		if (IS_USE_STRING_PTR && _strBuffer_const()==other._strBuffer_const()) 
			return *this;

		_setnull();
		if (other._strBuffer_const()==NULL)
		{			
			memcpy(mStrBuffer, other.mStrBuffer, STRING_BUFFER_LENGTH);			
		}		
		else
		{
			_setStrBuffer((char*)other._strBuffer_const());
			SetSpace(other.Space());
			AddUse();
		}

		return *this;
	}
	AString& operator = (const char* szString)
	{
		_set(szString);
		return *this;
	}
	AString& operator = (const wchar_t* szString);

	//AString& operator = (const int x);
	//AString& operator = (const Int64 x);

	//operator const char* () const
	//{
	//	return c_str();
	//}
	//operator bool () const{ return mStringBuffer!=NULL; }
	operator size_t () const
	{ 
		const char *key = c_str();
		int len = length();
		register uint nr=1, nr2=4; 
		while (len--) 
		{ 
			nr^= (((nr & 63)+nr2)*((uint) (uchar) *key++))+ (nr << 8); 
			nr2+=3; 
		} 
		return((uint) nr); 
	}

protected:

	typedef unsigned short UseType;
	typedef unsigned int SizeType;

	friend class DataStream;

	friend AString	Binary2String(const CHAR* pIn, UINT InLength, const char *preFix);

protected:
    union 
    {
        char		mStrBuffer[STRING_BUFFER_LENGTH+1];		// ����ַ������Ϊ '\0' Ϊ 'P'ʱ����ʾ������ָ��ռ�        
        const char	*mDebugString;
    };

protected:
	UseType Use() const { if (IS_USE_STRING_PTR) return *(const UseType*)_strBuffer_const(); return 0; }

	// ����Ŀռ��С�����ڵ�9���ֽ�,ռ3���ֽ�, ���Ի�������Ϊ12�ֽ�(8+4),  ���ֱ�ӷ���������
	SizeType Space() const { if (IS_USE_STRING_PTR) return *(SizeType*)(_strBuffer_const()+2); return STRING_BUFFER_LENGTH+1; }
    //SizeType Space() const { if (IS_USE_STRING_PTR) return *(SizeType*)(&mStrBuffer[8]); return STRING_BUFFER_LENGTH+1; }

	char* _strBuffer()
	{
		if (IS_USE_STRING_PTR)
			return ((char*)( *(size_t*)mStrBuffer ))-StringSpaceInfoSize;

		return NULL; 
	}

	const char* _strBuffer_const() const
	{
		if (IS_USE_STRING_PTR)
			return ((char*)( *(size_t*)mStrBuffer ))-StringSpaceInfoSize;

		return NULL; 
	}

	void _setStrBuffer(char *strBuff)
	{		
		*(size_t*)mStrBuffer = (size_t)(strBuff+StringSpaceInfoSize);
		mStrBuffer[STRING_BUFFER_LENGTH] = 'P';
	}

	void Alloc(SizeType size)
	{				
		_setnull();
		if (size>STRING_BUFFER_LENGTH)
		{
			size_t s = size + StringSpaceInfoSize;
			//if (s<15)
			//	s = 15;

			AssertNote(s<INT_MAX, "�ַ��ռ䲻�ܴ���[%d]", INT_MAX-StringSpaceInfoSize);
			char *ptr = (char*)ALLOCT_NEW(s);
			if (ptr==NULL)
			{
				AssertEx(0, "�����ַ����ռ�ʧ��");
				return;
			}
			_setStrBuffer(ptr);

#if DEBUT_STRING
			AString::gDebugLock.lock();
			gDebugString.insert(_strBuffer()+StringSpaceInfoSize);
			AString::gDebugLock.unlock();
#endif
			memset(ptr, 0, s);
			AddUse();
			//SetSpace(s-StringSpaceInfoSize);
            *(SizeType*)(_strBuffer_const()+2) = s-StringSpaceInfoSize;
		}		
	}
	//void resize(size_t len){ Alloc(len); }

	void AddUse()
	{
		AssertNote(Use()<=0xFFFF, "�����������ɳ��� 0xFFFF");
		*(UseType*)_strBuffer() = Use()+1;
	}

	void SetSpace(int s)
	{
		//if (s<0) s = 0;
  //      *(SizeType*)(_strBuffer()+2) = s;
		////*(SizeType*)(&mStrBuffer[8]) = ((uint)s);
	}

	void _setnull();

public:
#if DEBUT_STRING
	static CsLock				gDebugLock;
	static std::set<char*>		gDebugString;
#endif


protected:
	void _set(const char* szString);

	char* _str()
	{
		if (IS_USE_STRING_PTR)
		{
			if (_strBuffer()!=NULL)
				return _strBuffer() + StringSpaceInfoSize;

			return NULL;
		}
		return mStrBuffer;
	}
	bool is() const { return _strBuffer_const()!=NULL; }

	SizeType size() const { return Space(); }

public:
	static size_t _hash(const char *szString)
	{
		int len = strlen(szString);
		register uint nr=1, nr2=4; 
		while (len--) 
		{ 
			nr^= (((nr & 63)+nr2)*((uint) (uchar) *szString++))+ (nr << 8); 
			nr2+=3; 
		} 
		return((uint) nr); 
	}
};

#pragma pack(pop)	

struct AStringHash 
{
	size_t operator()(const AString& other) const { return AString::_hash(other.c_str()); }
};

#ifdef __LINUX__
extern int	MultiByteToWideChar(
	unsigned int  CodePage,
	unsigned long    dwFlags,
	const char*   lpMultiByteStr,
	int      cbMultiByte,
	wchar_t*  lpWideCharStr,
	int      cchWideChar);

extern int	WideCharToMultiByte(
	unsigned int  CodePage,
	unsigned long    dwFlags,
	const wchar_t*  lpWideCharStr,
	int      cchWideChar,
	char*   lpMultiByteStr,
	int      cbMultiByte,
	const char*   lpDefaultChar,
	int*  lpUsedDefaultChar);
#endif
//------------------------------------------------------------------------------------
typedef Array<AString, 8> StringArray;
//------------------------------------------------------------------------------------

#endif //_INCLUDE_AUTOSTRING_H_