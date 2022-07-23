using System;


namespace Logic
{
    public abstract class BaseFieldInfo : tFieldInfo
    {
        protected string mName = null;
        protected int mPosition = 0;

        //public override string getTypeName() { return StaticDefine.ToStringType(getType()); }

        public override void setName(string name) { mName = name; }
        public override string getName() { return mName; }

        public override void setPosition(int nPos) { mPosition = nPos; }
        public override int getPosition() { return mPosition; }

        public override bool serialize(object[] dataList, ref DataBuffer resultData)
        {
            return serialize(dataList[mPosition], ref resultData);
        }
        public override bool restore(ref object[] dataList, ref DataBuffer scrData)
        {
            return restore(out dataList[mPosition], ref scrData);
        }

        public override int getIndex(object[] dataList)
        {
            Logic.EventCenter.Log(LOG_LEVEL.ERROR, "No Support this index support > " + StaticDefine.ToStringType(getType()) + " at " + getName());
            return 0;
        }
        public override int MakeIndex(int nVal) { return 0; }
        public override int MakeIndex(string strVal) { return 0; }

        public override bool get(object[] dataList, out UInt64 result) { result = 0; return false; }

        public override bool saveParam(ref DataBuffer resultData) { return true; }
        public override bool restoreParam(ref DataBuffer scrData) { return true; }
    }
}
