#include "mNewCreateShareMemFieldIndex.h"
#include "mShareMemTableData.h"


void NetCloud::NewCreateShareMemFieldIndex::_updateInfo()
{
	BaseFieldIndex::_updateInfo();
	mRecordMemPool.clear();
	mRecordMemPool.setBlockSize(getDataLength() + (getCount() + 7) / 8+sizeof(ShareUpdateInfo));

	// ��������ֶ��Ƿ����ʹ�ù���
	for (int i = 0; i < getCount(); ++i)
	{
		FieldInfo info = getFieldInfo(i);
		if (!info->CanUseShareMemory())
			ERROR_LOG("%s field %s : type [%s] can not use share memory pool", mRecordMemPool.mpMemTableData->GetTableName(), info->getName().c_str(), info->getTypeString());
	}
}
