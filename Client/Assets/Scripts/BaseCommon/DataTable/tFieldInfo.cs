using System;

namespace Logic
{
    public abstract class tFieldInfo
    {
        public abstract void setName(string name);
        public abstract string getName();

        public abstract FIELD_TYPE getType();
        //public abstract string getTypeName();

        public abstract void setPosition(int nPos);
        public abstract int getPosition();

        public abstract bool set(ref object[] dataList, object obj);

        public abstract object get(object[] dataList);

        public abstract bool get(object[] dataList, out int nVal);
        public abstract bool get(object[] dataList, out float fVal);
        public abstract bool get(object[] dataList, out string strVal);
        public abstract bool get(object[] dataList, out UInt64 result);

        public abstract bool serialize(object[] dataList, ref DataBuffer resultData);
        public abstract bool restore(ref object[] dataList, ref DataBuffer scrData);

        public abstract bool serialize(object data, ref DataBuffer resultData);
        public abstract bool restore(out object data, ref DataBuffer scrData);

        public abstract int getIndex(object[] dataList);
        public abstract int MakeIndex(int nVal);
        public abstract int MakeIndex(string strVal);

        public abstract bool saveParam(ref DataBuffer resultData);
        public abstract bool restoreParam(ref DataBuffer scrData);

        public virtual void setMaxLength(int maxLen) { }
        public virtual int getMaxLength() { return 0; }

        public virtual string getTypeParam() { return ""; }
    };

}
