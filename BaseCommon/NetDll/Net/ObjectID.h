
#ifndef _INCLUDE_OBJECTID_H_
#define _INCLUDE_OBJECTID_H_

typedef int	ID;
typedef	unsigned char	TD;

//union Code
//{
//public:
//	struct
//	{
//		ID mIndex       : 24;	  // ��24λ�����������   ��� 1677 7215
//		ID mThread      : 8;     // ��8λ��ʾ�߳�����    ��� 255
//	};
//	int			mCode;
//
//public:
//	Code()
//		: mCode(0)
//	{
//
//	}
//	Code(int nCode)
//		: mCode(nCode)
//	{
//	}
//
//	Code(int nIndex, int nThread)
//		: mIndex(nIndex)
//		, mThread(nThread)
//	{
//	}
//
//	Code(const Code& other)
//		: mCode(other.mCode)
//	{
//	}
//
//public:
//	operator ID () const { return mIndex; }		// ת��Ϊ�������, �����������Ϊ�޷�����,��ֱ�ӿ���ȡ���������
//	operator TD () const { return mThread; }		// �Զ������߳�
//	operator int () const { return mCode; }		// ת��Ϊ����, �����������Ϊ����, ��ֱ�ӿ���ת������ֵ
//	Code& operator = (int nCode){ mCode = nCode; return *this; }			//ȫֵ����
//	Code& operator = (ID uIndex){ mIndex = uIndex; return *this; }			//ֱ�ӽ�������
//	Code& operator = (TD uThread){ mThread = uThread; return *this; }		//ֱ�ӽ����߳�����
//};

//typedef Code		CODE;

typedef int		CODE;



//union KEY
//{
//public:
//	struct
//	{		
//		int mRight;       		  
//		int mLeft;    
//	};
//	unsigned __int64	mValue;
//
//public:
//	KEY()
//		: mValue(0)
//	{
//
//	}
//	KEY(unsigned __int64 nV)
//		: mValue(nV)
//	{
//	}
//
//	KEY(int nK1, int nK2)
//		: mRight(nK2)
//		, mLeft(nK1)
//	{
//	}
//
//	KEY(const KEY& other)
//		: mValue(other.mValue)
//	{
//	}
//
//public:
//	int DBID(){ return mLeft; }
//	int DB(){ return mRight; }
//
//	operator unsigned __int64 () { return mValue; }
//};



#endif //_INCLUDE_OBJECTID_H_