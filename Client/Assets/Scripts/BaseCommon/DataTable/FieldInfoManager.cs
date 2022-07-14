using System.Collections;
using System.Collections.Generic;



namespace Logic
{
    class FieldInfoManager
    {
        protected Dictionary<FIELD_TYPE, tFieldInfoFactory> mFieldInfoMap = new Dictionary<FIELD_TYPE, tFieldInfoFactory>();
        protected Dictionary<FIELD_TYPE, tFieldInfo> mFieldMap = new Dictionary<FIELD_TYPE, tFieldInfo>();

        //--------------------------------------------------------------------------------

        FieldInfoManager()
        {
            tFieldInfoFactory info = new IntFieldInfoFactory();
            _RegisterFieldInfo(info);
            info = new FloatFieldInfoFactory();
            _RegisterFieldInfo(info);
            info = new DoubleFieldInfoFactory();
            _RegisterFieldInfo(info);
            info = new StringFieldInfoFactory();
            _RegisterFieldInfo(info);
            info = new BoolFieldInfoFactory();
            _RegisterFieldInfo(info);
            info = new NiceDataFieldInfoFactory();
            _RegisterFieldInfo(info);
            //info = new TableFieldInfoFactory();
            //_RegisterFieldInfo(info);
            //info = new DataFieldInfoFactory();
            //_RegisterFieldInfo(info);

            info = new ByteFieldInfoFactory();
            _RegisterFieldInfo(info);
            info = new UINT64FieldInfoFactory();
            _RegisterFieldInfo(info);

            info = new ShortFieldInfoFactory();
            _RegisterFieldInfo(info);

            //info = new RecordFieldInfoFactory();
            //_RegisterFieldInfo(info);
        }
        //--------------------------------------------------------------------------------

        public void _RegisterFieldInfo(tFieldInfoFactory fieldInfoFactory)
        {
            mFieldInfoMap.Add(fieldInfoFactory.getType(), fieldInfoFactory);
            mFieldMap.Add(fieldInfoFactory.getType(), fieldInfoFactory.NewFieldInfo());
        }

        //--------------------------------------------------------------------------------

        static public void RegisterFieldInfo(tFieldInfoFactory fieldInfoFactory)
        {
            sFieldInfoMgr.mFieldInfoMap.Add(fieldInfoFactory.getType(), fieldInfoFactory);
        }
        //--------------------------------------------------------------------------------

        static FieldInfoManager sFieldInfoMgr = new FieldInfoManager();
        //--------------------------------------------------------------------------------

        static public tFieldInfo CreateFieldInfo(FIELD_TYPE eType)
        {
            tFieldInfoFactory fieldFactory;
            if (sFieldInfoMgr.mFieldInfoMap.TryGetValue(eType, out fieldFactory))
                return fieldFactory.NewFieldInfo();

            return null;
        }
        //--------------------------------------------------------------------------------

        static public tFieldInfo CreateFieldInfo(string strType)
        {
            FIELD_TYPE eType = StaticDefine.ToType(strType);

            return CreateFieldInfo(eType);
        }

        static public tFieldInfo GetFieldInfo(FIELD_TYPE eType)
        {
            tFieldInfo fieldInfo;
            if (sFieldInfoMgr.mFieldMap.TryGetValue(eType, out fieldInfo))
                return fieldInfo;

            return null;
        }

        static public tFieldInfo GetFieldInfo(string strType)
        {
            FIELD_TYPE eType = StaticDefine.ToType(strType);
            return GetFieldInfo(eType);
        }
    };
}
