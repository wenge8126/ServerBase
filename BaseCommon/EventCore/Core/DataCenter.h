/********************************************************************
	created:	2014/01/10
	created:	10:1:2014   16:48
	filename: 	E:\HomeGame\BaseCommon\EventCore\Core\DataCenter.h
	file path:	E:\HomeGame\BaseCommon\EventCore\Core
	file base:	DataCenter
	file ext:	h
	author:		Wenge Yang
	
	purpose:	�߼���������
				����״̬,���Ե����ݶ�����ͨ���������Ĳ���
*********************************************************************/
#ifndef _INCLUDE_DATACENTER_H_
#define _INCLUDE_DATACENTER_H_

class __DataCenter
{
public:
	virtual ~__DataCenter(){}

public:
	virtual AData& get(const char *szIndex)
	{
		return mMainData.getOrCreate(szIndex);
	}

	virtual AutoNice getData(const char *szIndex)
	{
		AutoNice result;
		mMainData.get(szIndex, result);
		return result;
	}

protected:
	NiceData		mMainData;
};


#endif //_INCLUDE_DATACENTER_H_