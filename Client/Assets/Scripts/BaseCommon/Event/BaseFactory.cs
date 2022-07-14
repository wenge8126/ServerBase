using System.Collections;

namespace Logic
{
    public abstract class BaseFactory : tEventFactory
    {
        protected string mName;
        public int mNameIndex;

        public tEventCenter mEventCenter;

        public BaseFactory()
        {

        }

        public override int GetNameIndex() { return mNameIndex; }

        public override void SetEventName(string name) { mName = name; mNameIndex = StaticDefine.StringID(mName); }
        public override string GetEventName() { return mName; }

        public override void SetEventCenter(tEventCenter center) { mEventCenter = center; }
        public override tEventCenter GetEventCenter() { return mEventCenter; }


    }
}
