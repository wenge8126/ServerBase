
using System;


namespace Logic
{


    public class TM_WaitToRunAction : BaseEvent
    {
        public object mObject;
        public System.Reflection.MethodInfo mFunMethod;
        public object mParam;

        public Action mPreferedAction;

        public override void InitData()
        {
            base.InitData();
            mObject = null;
            mFunMethod = null;
            mParam = null;
        }

        public override void OnTimeOver()
        {
            object[] p = new object[1];
            p[0] = mParam;
            try
            {
                if (mPreferedAction != null)
                    mPreferedAction();
                else if (mObject != null)
                    mFunMethod.Invoke(mObject, p);
                else
                    Log("ERROR: Function object is null");
            }
            catch (Exception e)
            {
                LOG.logError("Run action > " + e.ToString());
            }
        }
    }


    public class TM_WaitUpdateAction : BaseEvent
    {
        public float mNowTime = 0;
        public int mRunCount = 1;
        public float mSpaceTime = 1;
        public object mObject;
        public System.Reflection.MethodInfo mFunMethod;
        public object mParam;

        public override void InitData()
        {
            base.InitData();
            mObject = null;
            mFunMethod = null;
            mParam = null;

            mNowTime = 0;
            mRunCount = 1;
            mSpaceTime = 1;
        }


        public override void Do()
        {
            mNowTime = EventCenter.Instance.mTimeEventManager.GetNowTime();
            StartUpdate();
         
        }

        public override void Update(float t)
        {
            if (mFunMethod == null)
            {
                StopUpdate();
                return;
            }
            if (EventCenter.Instance.mTimeEventManager.GetNowTime() - mNowTime > mRunCount * mSpaceTime)
            {
                ++mRunCount;
                object[] p = new object[1];
                p[0] = mParam;
                try
                {
                    if (!(bool)mFunMethod.Invoke(mObject, p))
                    {
                        StopUpdate();
                        return ;
                    }
                }
                catch (Exception e)
                {
                    LOG.logError("Run action > " + e.ToString());
                    StopUpdate();
                    return ;
                }
            }            
        }

    }



}
