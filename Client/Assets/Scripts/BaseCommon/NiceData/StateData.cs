

public class StateData
{
    static public void SetState(ref int data, int state, bool bSet)
    {
        if (bSet)
            data |= state;
        else
            data &= ~state;
    }

    static public bool HasState(int data, int state)
    {
        return (data & state) > 0;
    }

    public StateData()
    {
        mStateData = new byte[1];
    }
    public StateData(uint count)
    {
        mStateData = new byte[(count + 7) / 8];
    }

    // return true only if this have other state.
    public bool Compare(StateData other)
    {
        if (other.mStateData.Length != mStateData.Length)
            return false;

        for (int i = 0; i < mStateData.Length; ++i)
        {
            byte x = mStateData[i];
            byte x2 = other.mStateData[i];
            if ((mStateData[i] & other.mStateData[i]) != 0)
                return true;
        }

        return false;
    }

    public void InitClear()
    {
        for (int i = 0; i < mStateData.Length; ++i)
        {
            mStateData[i] = 0;
        }
    }

    public void InitOpen()
    {
        for (int i = 0; i < mStateData.Length; ++i)
        {
            mStateData[i] = 0xFF;
        }
    }

    public bool Set(int index, bool bOpen)
    {
        if (bOpen)
            return Open(index);
        else
            return Close(index);
        //int pos = index / 8;
        //int bit = index % 8;
        //if (pos >= mStateData.Length)
        //    return false;
        //// free space is 1.
        //if (!bOpen)
        //    mStateData[pos] |= (byte)(1 << bit);
        //else
        //{
        //    mStateData[pos] &= (byte)~(1 << bit);
        //}
        //return true;
    }

    public bool Open(int index)
    {
        int pos = index / 8;
        if (pos >= mStateData.Length)
            return false;
        int bit = index % 8;

        mStateData[pos] |= (byte)(1 << bit);

        return true;
    }

    public bool Close(int index)
    {
        int pos = index / 8;
        if (pos >= mStateData.Length)
            return false;
        int bit = index % 8;

        mStateData[pos] &= (byte)~(1 << bit);

        return true;
    }

    public bool IsOpen(int index)
    {
        int pos = index / 8;
        int bit = index % 8;
        if (pos >= mStateData.Length)
            return false;

        return (mStateData[pos] & (1 << bit)) != 0;
    }

    public bool HasOpen()
    {
        for (int i = 0; i < mStateData.Length; ++i)
        {
            if (mStateData[i] != 0)
                return true;
        }
        return false;
    }

    //---------------------------------------------
    public byte[] mStateData;
}
