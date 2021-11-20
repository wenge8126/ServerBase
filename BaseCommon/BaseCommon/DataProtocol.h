#ifndef _INCLUDE_DATAPROTOCOL_H_
#define _INCLUDE_DATAPROTOCOL_H_

#include "Auto.h"
#include "BaseTable.h"
#include "TableManager.h"
/*/----------------------------------------------------------------------
NiceData �����Ż�
    1 Э����Ϣ 
    ��1������
    ��2�����
    ��3������
    ����
    mm
{
    1 INT age = 10,
        2 STRING name = "NULL",
}
2 �ֶξ�Ϊ��ѡ
    ʹ��״̬���������Ƿ����

    3 Э����غ� ����ΪTABLE��ʹ��DataTableManger �������е�Э��
    ��֧�ֱ���ΪЭ���ʽ

    4 Э��֧��Ƕ��
    ����ʱ�� ��Э������л�ȡ

    5 ����
    ��0��4�ֽ���������Э����֤KEY��ͨ��Э���������ɣ���Э��׼����ɺ����ɣ�
    ��1���Ƿ����״̬
    ��2��ʹ���ֶ���Ϣ���������ֶ�
    ��3�� ���ΪǶ��Э�飬������Э������

    6 ������
    ��1����ȡ��֤�� �Ƚ���֤�����������������
    ��2����ȡ����״̬
    ��3�����ݴ��ڵ�״̬������������ݶ���
    ��4�����Ϊ��Э�飬 ��ݹ鷴������Э��
//---------------------------------------------------------------------*/


// ʹ�� NICEDATA �ֶα�ʾΪ��Э��
// NOTE:����Э��ʱ,Ҳ��NICEDATA����, �������ж��Ƿ�Ϊ����
class BaseCommon_Export DataProtocol : public AutoBase
{
public:
    bool LoadProtocol(const char *szTxtFile);
    bool SaveProtocol(AutoTable protocol, const char *szFileName, bool bAppend = true);

    bool LoadProtocol(DataStream *scrProtocolData, bool bReplace = true);

    bool SaveProtocol(AutoTable protocol, DataStream *destData);

    AutoTable FindProtocol(const char *szProtocolName);
    AutoTable FindProtocol(const AString &strProtocolName);

    void AppendProtocol(AutoTable protocal)
    {
        mProtocolList.InsertTable(protocal->GetTableName(), protocal, true);
    }

    bool _SaveNiceData(const char *szProtocolName, NiceData *pScrData, DataStream *destData);

    bool SaveNiceData(AutoTable protocol, NiceData *pScrData, DataStream *destData);

    bool _RestoreData(const char *szProtocolName, DataStream *scrData, NiceData *pDestData);

    bool RestoreData(AutoTable protocol, DataStream *scrData, NiceData *pDestData);

public:
    AutoTable GenerateProtocol(NiceData *pScrData, const char *szProtocolName, bool bReplace = true);

    bool SaveAllProtocol(const char *szFileName, bool bAppend = false);

    bool SerializeProtocal(AutoTable protocal, DataStream *destData);

    AutoTable RestoreProtocal(DataStream *scrData);

public:
    DataTableManager        mProtocolList;
};

#endif //_INCLUDE_DATAPROTOCOL_H_