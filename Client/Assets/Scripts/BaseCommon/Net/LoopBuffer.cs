



using System;

namespace Logic
{
    public class OutputBuffer
    {
        static public int MAX_DATA_BUFFER_SIZE = 1024 * 512;

	    byte[]		mData;
	
	    public int m_Head = 0;
	    public int m_Tail = 0;
	
	    //---------------------------------------------------------------------
	
	    public OutputBuffer(int initSize)
	    {
		    mData = new byte[initSize];
	    }
	
	    public int DataSize()
	    {
		    if (m_Head<m_Tail)
			    return m_Tail - m_Head;
		else if (m_Head>m_Tail)
		{
			return mData.Length-m_Head + m_Tail;
		}
		
		return 0;
        }
	
	public int FreeSize()
	{
        if (m_Head <= m_Tail)
            return mData.Length - m_Tail + m_Head - 1;
        else
            return m_Head - m_Tail -1;
	}
	
	public bool Write(byte[] data, int beginPos, int size)
	{
        if (data.Length<beginPos+size)
            return false;

        int freeSize = FreeSize();
		if (size>=FreeSize())
        {
            if (!Resize(size - freeSize + 1))
                return false;
        }

        if (m_Head<=m_Tail)
        {
            // data at middle
            if (m_Head==0)
            {
                freeSize = mData.Length - m_Tail -1;
                Array.Copy( data, beginPos, mData, m_Tail, size );
            }
            else
            {
                freeSize = mData.Length - m_Tail;
                if (size <= freeSize)
                    Array.Copy(data, beginPos, mData, m_Tail, size);
                else
                {
                    Array.Copy(data, beginPos, mData, m_Tail, freeSize);
                    Array.Copy(data, beginPos+freeSize, mData, 0, size-freeSize);
                }
            }
        }
        else
        {
            // data at two side.
            Array.Copy(data, beginPos, mData, m_Tail, size);
        }

        m_Tail = (m_Tail+size) % mData.Length;

        return true;
	}
	

    public int _NetSend(tNetTool tNet)
    {

        int nFlushed = 0;
        int nSend = 0;
        int nLeft = 0;

        if (m_Head < m_Tail)
        {
            nLeft = m_Tail - m_Head;

            while (nLeft > 0)
            {
                nSend = tNet.tryWrite(mData, m_Head, nLeft);
                if (nSend<=0)
                    return 0;
                nFlushed += nSend;
                nLeft -= nSend;
                m_Head += nSend;                
            }

            if (nLeft!=0)
                LOG.log(" *** OutputBuffer is error.");
                
        }
        else if (m_Head > m_Tail)
        {
            nLeft = mData.Length - m_Head;

            while (nLeft>0)
            {
                nSend = tNet.tryWrite(mData, m_Head, nLeft);
                if (nSend<=0)
                    return 0;

                nFlushed += nSend;
                nLeft -= nSend;
                m_Head += nSend;
            }

            if (m_Head != mData.Length)
                LOG.log(" *** OutputBuffer is error.");

            m_Head = 0;

            while (nLeft >0)
            {
                nSend = tNet.tryWrite(mData, m_Head, nLeft);

                if (nSend<=0)
                    return 0;

                nFlushed += nSend;
                nLeft -= nSend;
                m_Head += nSend;
            }

            if (nLeft!=0)
                LOG.log(" *** OutputBuffer is error.");
            
        }

        m_Head = m_Tail = 0;

        return nFlushed;

     }

        bool Resize(int addSize)
        {
            if (addSize <= 0)
                return true;

            int newSize = mData.Length + addSize;

            if (newSize>MAX_DATA_BUFFER_SIZE)
            {
                LOG.log(" *** XXX WARN: resize send buffer over max size.(1024*512) >>> "+newSize.ToString());
                return false;
            }

            int dataSize = DataSize();

            byte[] newData = new byte[newSize];

            if (m_Head < m_Tail)
            {
                Array.Copy(mData, m_Head, newData, 0, m_Tail-m_Head);
            }
            else if (m_Head>m_Tail)
            {
                Array.Copy(mData, m_Head, newData, 0, mData.Length-m_Head);
                Array.Copy(mData, m_Tail, newData, mData.Length-m_Head, m_Tail);
            }

            mData = newData;

            m_Head = 0;
            m_Tail = dataSize;

            return true;

        }

        public void Clear()
        {
            m_Head = 0;
            m_Tail = 0;
        }

    }


    // for net receive net data

    public class InputBuffer
    {
        int m_Head;
        int m_Tail;

        byte[]    mData;

        int Size { get{return mData.Length; } }

        public InputBuffer( int initSize )
        {
            mData = new byte[initSize];

            m_Head = 0;
            m_Tail = 0;
        }

        public int DataSize()
        {
            if (m_Head<m_Tail)
                return m_Tail-m_Head;

            else if (m_Head>m_Tail)
            {
                return mData.Length - m_Head+m_Tail;
            }

            return 0;
        }

        public bool Peek(ref byte[] resultData, int indexPosition, int getDataSize)
        {
            if (resultData.Length < indexPosition + getDataSize)
                return false;

            if (getDataSize <= 0)
                return true;

            if (getDataSize > DataSize())
                return false;

            if (m_Head < m_Tail)
                Array.Copy(mData, m_Head, resultData, indexPosition, getDataSize);
            else
            {
                int rightLen = mData.Length - m_Head;
                if (getDataSize <= rightLen)
                    Array.Copy(mData, m_Head, resultData, indexPosition, getDataSize);
                else
                {
                    Array.Copy(mData, m_Head, resultData, indexPosition, rightLen);
                    Array.Copy(mData, 0, resultData, indexPosition + rightLen, getDataSize - rightLen);
                }
            }

            return true;
        }

        public bool Skip(int len)
        {
            if (len <= 0)
                return true;

            if (len > DataSize())
                return false;

            m_Head = (m_Head + len) % mData.Length;

            return true;
        }


        public int FillOnNet(tNetTool tNet)
        {
            int nFilled = 0;
            int nReceived = 0;
            int nFree = 0;

            if (m_Head <= m_Tail)
            {
                if (m_Head == 0)
                {
                    nReceived = 0;
                    nFree = mData.Length - m_Tail - 1;

                    if (nFree != 0)
                    {
                        nReceived = tNet.tryRead(ref mData, m_Tail, nFree);
                        if (nReceived <= 0)
                            return nFilled;

                        m_Tail += nReceived;
                        nFilled += nReceived;
                    }

                    if (nReceived == nFree)
                    {
                        int available = tNet.available();
                        if (available > 0)
                        {
                            if (!Resize(available + 1))
                            {
                                LOG.log(" XXX resize receive data buffer size fail");
                                return nFilled;
                            }

                            nReceived = tNet.tryRead(ref mData, m_Tail, available);

                            if (nReceived <= 0)
                                return nFilled;

                            m_Tail += nReceived;
                            nFilled += nReceived;
                        }
                    }
                }
                else
                {
                    //    H   T		LEN=10
			        // 0123456789
			        // ...abcd...
                    nFree = mData.Length - m_Tail;
                    nReceived = tNet.tryRead(ref mData, m_Tail, nFree);
                    if (nReceived <= 0)
                        return 0;

                    m_Tail = (m_Tail + nReceived) % mData.Length;
                    nFilled += nReceived;

                    if (nReceived == nFree)
                    {
                        nReceived = 0;
                        nFree = m_Head - 1;
                        if (nFree != 0)
                        {
                            nReceived = tNet.tryRead(ref mData, 0, nFree);
                            if (nReceived <= 0)
                                return nFilled;

                            m_Tail += nReceived;
                            nFilled += nReceived;
                        }

                        if (nReceived == nFree)
                        {
                            int available = tNet.available();
                            if (available > 0)
                            {
                                if (!Resize(available + 1))
                                    return nFilled;

                                nReceived = tNet.tryRead(ref mData, m_Tail, available);

                                if (nReceived <= 0)
                                    return nFilled;

                                m_Tail += nReceived;
                                nFilled += nReceived;
                            }
                        }
                    }
                }
            }
            else
            {
                //     T  H		LEN=10
                // 0123456789
                // abcd...efg

                nReceived = 0;
                nFree = m_Head - m_Tail - 1;
                if (nFree != 0)
                {
                    nReceived = tNet.tryRead(ref mData, m_Tail, nFree);
                    if (nReceived <= 0)
                        return nFilled;

                    m_Tail += nReceived;
                    nFilled += nReceived;
                }

                if (nReceived == nFree)
                {
                    int available = tNet.available();
                    if (available > 0)
                    {
                        if (!Resize(available + 1))
                            return nFilled;

                        nReceived = tNet.tryRead(ref mData, m_Tail, available);
                        if (nReceived<=0)
                            return nFilled;

                        m_Tail += nReceived;
                        nFilled += nReceived;
                    }
                }
            }

            return nFilled;
        }

        public bool Resize(int size)
        {
            if (size <= 0)
                return true;

            int newSize = mData.Length + size;

            if (newSize > OutputBuffer.MAX_DATA_BUFFER_SIZE)
            {
                LOG.log(" *** XXX WARN: resize send buffer over max size.(1024*512) >>> " + newSize.ToString());
                return false;
            }

            int dataSize = DataSize();

            byte[] newData = new byte[newSize];

            if (m_Head < m_Tail)
            {
                Array.Copy(mData, m_Head, newData, 0, m_Tail - m_Head);

            }
            else if (m_Head > m_Tail)
            {
                Array.Copy(mData, m_Head, newData, 0, mData.Length - m_Head);
                Array.Copy(mData, 0, newData, mData.Length - m_Head, m_Tail);
            }

            mData = newData;

            m_Head = 0;
            m_Tail = dataSize;

            return true;

        }

        public void Clear()
        {
            m_Head = 0;
            m_Tail = 0;
        }

        public bool empty()
        {
            return m_Head == m_Tail;
        }
    }

    
}
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

