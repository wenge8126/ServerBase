#pragma once
#ifndef _INCLUDE_SHAREMEMDBTABLE_H_
#define _INCLUDE_SHAREMEMDBTABLE_H_

#include "ShareMemCloudDB.h"
#include "IndexBaseTable.h"
#include "IndexDBRecord.h"
#include "CloudDBTable.h"
#include "DBDefine.h"
#include "LoopList.h"
#include "PoolLoop.h"

//-------------------------------------------------------------------------
// �����ڴ�DB�� (2021/4/11 ֹͣʹ��, �����޸�����)
// NOTE: �ѵ��빲������޸ı�ʶͬ������ (ȥ����¼�޸�������)
// 1 ���ʶ����ʹ�ù����ڴ�, ҵ������¼�״̬����
// 2 �޸ĺ�,ͬʱ�޸�ҵ������빲���ڴ��״̬, Timer ��ʱ 10���,�ټ��и�����Ϣ���͵��������(ֻ����ҵ����״̬����)
// 3 ״̬����������4�ֽڸ��´���, �����ж��Ƿ���ȫ�������
// 4 ҵ����̹رպ�, ������̹ر�ǰ, �ٸ��ݸ��´��������ʶ������
// 5 �����ȥ����������߳�
// ����: 1 ������Ϣ���ط��ͼ�¼���� 2 ���´������� 3 ��ǿ������ȫ���
//-------------------------------------------------------------------------

class ShareMemAO;

namespace NetCloud
{

	class ShareMemCloudDBNode;

	class ShareCloudDB_Export ShareMemDBTable : public CloudDBTable
	{
		friend class ShareMemRecord;
		friend class DBDataIndexRecord;
	public:
		ShareMemDBTable(ShareMemCloudDBNode *pDB);

		virtual void ClearAll() override;

		virtual const char* GetTableType() { return SHARE_MEMORY_TABLE; }

		virtual void Process() override {}
		virtual void LowProcess();

		virtual void InitStart(tNiceData *pSQLParam) {}

	public:
		// ������¼������Э�̺�����
		virtual ARecord GrowthNewRecord(DataStream *recordData = NULL) override;
		virtual ARecord CreateRecord(Int64 nIndex, bool bReplace) override;

		virtual ARecord CreateRecord(const char* szIndex, bool bReplace) override;

		virtual ARecord NewRecord() override;
		virtual ARecord _NewRecord() override;

		// ��DB����ɾ����¼, NOTE: ��Ҫ������Э�̺�����
		virtual bool DeleteRecord(ARecord record) override;

	public:
		// ֱ�Ӵӹ����ڴ�ػָ����м�¼
		virtual bool RestoreFromShareMem(const Array<int> &sharePtrList);

		bool _RestoreRecord(char *pRecordMemPtr);

		void* GetShareRecordMem(SHARE_PTR recordPtr);

		virtual ARecord _ReqeustMemDBCreateRecord(const AString &key, bool bGrowth = false);

		void AppendShareMem(int key, int arrayIndex);

	protected:
		Array<ShareMemAO*>			mShareMemList;

	public:
		PoolLoop<ARecord>				mUpdateRecordList;				// ��ҪUpdate�ļ�¼�б�, �����ظ�
		UInt64										mUpdateCount;
		UInt64										mAlreadyUpdateCount;		
	};

	//-------------------------------------------------------------------------
	// �ӹ����ڴ�ָ���DB��¼
	//-------------------------------------------------------------------------
	class ShareCloudDB_Export ShareMemRecord : public IndexDBRecord
	{
		friend class ShareMemDBTable;
	public:
		ShareMemRecord( AutoTable hOwnerTable )
			: IndexDBRecord(hOwnerTable)
		{}
		~ShareMemRecord()
		{
			mRecordData = NULL;
		}

	public:
		void SetShareMem(char *pMemPtr)
		{
			mRecordData = pMemPtr;
			mUpdateFlagBuffer.resize(IndexDBRecord::_getStateDataSize());
		}
		// ���½�ʱ, ���Ѿ���ȡ�������ڴ��ַ
		virtual void _alloctData(int extData) override
		{
			mUpdateFlagBuffer.resize(IndexDBRecord::_getStateDataSize());
		}

		virtual void _freeData() override
		{
			mRecordData = NULL;
		}

		virtual char* _getData() const { return mRecordData; }

		virtual Data _getData(const char *szKey) const override { return get(szKey); }
		virtual Data _getData(int field) const override { return get(field); }

		virtual DSIZE _getStateDataSize() override
		{
			return IndexDBRecord::_getStateDataSize() + sizeof(ShareUpdateInfo);
		}

		virtual ShareUpdateInfo *getUpdateInfo()
		{
			return (ShareUpdateInfo*)(_getStateData() + _getStateDataSize() - sizeof(ShareUpdateInfo));
		}

		virtual void FullAllUpdate(bool bNeed) override
		{
			IndexDBRecord::FullAllUpdate(bNeed);
			memcpy(mUpdateFlagBuffer.data(), _getStateData(), IndexDBRecord::_getStateDataSize());
			if (bNeed)
			{
				Auto<ShareMemDBTable> t = GetTable();
				t->mUpdateRecordList.insert(this);
				t->mUpdateCount++;
				getUpdateInfo()->mShareUpdateCount++;
			}
		}

		virtual bool _setNeedUpdate(int col, bool bNeed = true) override
		{			
			if (bNeed)
			{
				mUpdateFlagBuffer.openState(col);
				Auto<ShareMemDBTable> t = GetTable();
				t->mUpdateRecordList.insert(this);
				t->mUpdateCount++;
				getUpdateInfo()->mShareUpdateCount++;
			}
			else
				mUpdateFlagBuffer.closeState(col);

			return IndexDBRecord::_setNeedUpdate(col, bNeed);
		}

		bool ExistUpdateFlag()
		{
			char *sz = mUpdateFlagBuffer.data();
			for (int i = 0; i < mUpdateFlagBuffer.size(); ++i)
			{
				if (sz[i] != 0)
					return true;
			}
			return false;
		}


	public:
		//NOTE: �ڴ˴����, ���뱣֤ÿ���޸ĺ�, ��Ҫ��һ�����ʶ! �Զ����������� (���ü�������Ϊ2 ʱ���м�������)
		virtual void OnUseChanged(int useCount) override;


	public:
		// ����ҵ����̱�ʶ����״̬����
		Buffer		mUpdateFlagBuffer;
	};

	//-------------------------------------------------------------------------
	// ֻ���ڴӹ����ڴ��лָ���¼���ݵ��ֶ�����, ����DB����
	// NOTE: DB���̲��ɷ����½���¼���ڴ�, �½�ʱЭ���첽����ȴ�
	//-------------------------------------------------------------------------
	class ShareMemFieldIndex : public BaseFieldIndex
	{
	public:
		ShareMemFieldIndex()
			:BaseFieldIndex() {}

		virtual INIT_FIELD_MODE getFieldMode() const override { return eShareMemoryUseField; }
	public:
		virtual void* _allotRecordData(uint size) override { /*ERROR_LOG("Can not user _allotRecordData");  AssertEx(0, "Can not user _allotRecordData");*/  return NULL; }

		virtual void _freeRecordData(void *pRecordData) override { /*ERROR_LOG("Can not user _freeRecordData");  AssertEx(0, "Can not user _freeRecordData");*/ }


		virtual DSIZE _getStateDataSize()
		{
			return (getCount() + 7) / 8 + sizeof(ShareUpdateInfo);			
		}

		virtual DSIZE _getRecordMemSize()
		{
			return getDataLength() + _getStateDataSize() + 1;
		}
	};



}
#endif //_INCLUDE_SHAREMEMDBTABLE_H_
