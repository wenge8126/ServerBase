
#define USE_HASH_EX     1

#if USE_HASH_EX

#ifndef _INCLUDE_EASYHASH_H_
#define _INCLUDE_EASYHASH_H_

#include "Array.h"
#include "Common.h"

#define HASH_INIT_BUCKET_COUNT		(8)

//-------------------------------------------------------------------------*/
// 专用于Hash中的ArrayList, 区别不再分配静态空值对象
// 精减内存使用
// NOTE: mValueArray 不可为空
// 2019-9-9 每个桶初始时不分配数组空间, 桶内加入元素时初始1个元素
// 持续优化: 进一步优化可以在Value中直接加入一个T对象, 存一般只有一个元素, 不分配数组空间, 理论上效率更优
//-------------------------------------------------------------------------*/
#pragma pack(push)
#pragma pack(1)
template<typename T, int bucketInitSize>
class ArrayListForHash : public  tArray<T, bucketInitSize, unsigned short>  // 尺寸类型为无符号短整形,只需要最多是 65535, 在 push_back里需要检查最大数量    
{
    typedef tArray<T, bucketInitSize, unsigned short> parent;

private:
    T   mFirstValue;
    bool mbUsedFirstValue;

public:
    T* operator [] (size_t pos)
    {
        if (pos <= 0)
        {
            return &mFirstValue;
        }
        AssertNote(pos - 1 < parent::mCount, "Code error");
        return &parent::mpArrayPtr[pos - 1];
    }

    const T* operator [] (size_t pos) const
    {
        if (pos <= 0)
        {
            return &mFirstValue;
        }
        AssertNote(pos - 1 < parent::mCount, "Code error");
        return &parent::mpArrayPtr[pos - 1];
    }

public:
    void add(const T &value)
    {
        if (!mbUsedFirstValue)
        {
            mbUsedFirstValue = true;
            mFirstValue = value;
            return;
        }
        push_back(value);
    }

    bool removeAt(size_t pos)
    {
        if (pos <= 0)
        {
            if (parent::mCount > 0)
            {
                mFirstValue = parent::mpArrayPtr[parent::mCount - 1];
                _removeAt(parent::mCount - 1);
                mbUsedFirstValue = true;
                return true;
            }
            mFirstValue = T();
            mbUsedFirstValue = false;
            return true;
        }
        return _removeAt(pos - 1);
    }

    bool remove(const T &value)
    {
        if (mbUsedFirstValue && mFirstValue == value)
            return removeAt(0);

        for (size_t i = 0; i < size(); ++i)
        {
            if (parent::mpArrayPtr[i] == value)
            {
                return _removeAt(i);
            }
        }
        return false;
    }

    void clearUp(bool bFreeBuffer)
    {
        mbUsedFirstValue = false;
        mFirstValue = T();
        clear(bFreeBuffer);
    }

    size_t Count() const
    {
        if (mbUsedFirstValue)
            return size() + 1;
        return size();
    }

    bool isEmpty() const
    {
        if (mbUsedFirstValue)
            return false;
        return parent::empty();
    }

    size_t spaceSize() const
    {
        return sizeof(mFirstValue) + 1 + _useSpaceSize();
    }

protected:
    bool _removeAt(size_t pos)
    {
        if (pos >= 0 && pos < parent::mCount - 1)
        {
			parent::mpArrayPtr[pos] = parent::mpArrayPtr[parent::mCount - 1];
			parent::mpArrayPtr[parent::mCount - 1] = T();
            --parent::mCount;
            return true;
        }
        else if (pos == parent::mCount - 1)
        {
			parent::mpArrayPtr[parent::mCount - 1] = T();
            --parent::mCount;
            return true;
        }
        return false;
    }

	DSIZE size() const { return parent::mCount; }
    bool empty()const { return 0 == parent::mCount; }

    void push_back(T val) { parent::push_back(val); }
    void pop_back() { parent::pop_back(); }
    bool remove(size_t pos) { parent::remove(pos); }
    void clear(bool bFreeBuffer = true) { parent::clear(bFreeBuffer); }
    void resize(size_t num, T def = T()) { parent::resize(num, def); }
    void swap(parent &other) { parent::swap(other); }
    size_t _useSpaceSize() const { return parent::_useSpaceSize(); }

public:
    ArrayListForHash() : mbUsedFirstValue(false) {}
    ArrayListForHash(const ArrayListForHash &other)
        : mbUsedFirstValue(false)
    {
        *this = other;
    }

    ArrayListForHash& operator = (const ArrayListForHash &other)
    {
        clear(false);
        mbUsedFirstValue = other.mbUsedFirstValue;
        mFirstValue = other.mFirstValue;
        resize(other.size());
        for (size_t i = 0; i < other.size(); ++i)
        {
			parent::mpArrayPtr[i] = other.mpArrayPtr[i];
        }
        return *this;
    }
};
/*/-------------------------------------------------------------------------
测试结果总结：
    1 EasyHash (现名) ArrayListHash 实际测试比hash_map 要快， 且对于增加删除时比较有优势，但空间占用稍多于其他
    2 EasyMapHash 在数量多时慢于 hash_map, 少时略快
    3 EasyOneHash 由于必须使用 string_id, 慢于以上所有HASH
//-------------------------------------------------------------------------
// NOTE:
// 简易的Hash容器, 桶使用ArraylList, 适合频繁变化的情况
// 重分配桶数量条件： 当数量超过桶数量的2倍
// 性能：15000数量下，查寻150000次，创建31 查寻用时 576， stdext::hash_map 创建31，查寻用时564
//------------------------------------------------------------------------*/
template<typename K, typename V, int INIT_COUNT = 8, int bucketInitSize = 1>
class EasyHash : public MemBase
{
public:
    struct Value
    {
    public:
        K			mKey;
        mutable V   	mVal;

    public:
        Value()
            : mKey()
            , mVal()
        {

        }
    };
    typedef ArrayListForHash<Value, bucketInitSize>	ValueBucket;

public:
    V& operator [] (const K &key)
    {
        V *p = findPtr(key);
        if (p == NULL)
        {
            insert(key, V());
            p = findPtr(key);
        }
        return *p;
    }

    const V& operator [] (const K &key) const
    {
        return find(key);
    }

    // NOTE: Replace when exist key
    void insert(const K &key, const V &val)
    {
        _checkRehash();
        size_t k = _hashKey(key) % mValueArray.size();
        ValueBucket &vB = mValueArray[k];
        //_remove(vB, key);		// 优化如下, 存在直接覆盖 2017.6.20
        for (int i = 0; i < vB.Count(); ++i)
        {
            Value &kV = *vB[i];
            if (kV.mKey == key)
            {
                kV.mVal = val;
                return;
            }
        }

        Value v;
        v.mKey = key;
        v.mVal = val;
        mValueArray[k].add(v);
        ++mCount;
    }

    bool erase(const K &key)
    {
        size_t k = _hashKey(key) % mValueArray.size();
        return _remove(mValueArray[k], key);
    }

    bool erase(const K &key, V &exist)
    {
        size_t k = _hashKey(key) % mValueArray.size();
        ValueBucket &vB = mValueArray[k];
        for (int i = 0; i < vB.Count(); ++i)
        {
            if ((*vB[i]).mKey == key)
            {
                exist = (*vB[i]).mVal;
                vB.removeAt(i);
                --mCount;
                return true;
            }
        }
        return false;
    }

    V& find(const K &key) const
    {
        V* p = findPtr(key);
        if (p != NULL)
            return *p;

        return _getStatic().mVal;
    }

    V& find(const char *key) const
    {
        V* p = findPtr(key);
        if (p != NULL)
            return *p;

        return _getStatic().mVal;
    }

    V* findPtr(const K &key) const
    {
        size_t k = _hashKey(key) % mValueArray.size();
        const ValueBucket &vB = mValueArray[k];
        for (int i = 0; i < vB.Count(); ++i)
        {
            const Value &kV = *vB[i];
            if (kV.mKey == key)
            {
                return &(kV.mVal);
            }
        }
        return NULL;
    }

    V* findPtr(const char *key) const
    {
        size_t k = _hashKey(key) % mValueArray.size();
        const ValueBucket &vB = mValueArray[k];
        for (size_t i = 0; i < vB.Count(); ++i)
        {
            const Value &kV = *vB[i];
            if (kV.mKey == key)
            {
                return &(kV.mVal);
            }
        }
        return NULL;
    }

	bool NeedOptimize(int limitCount = 2)
	{
		for (size_t i = 0; i < mValueArray.size(); ++i)
		{
			if (mValueArray[i].Count() > limitCount)
			{
				return true;
			}
		}
		return false;
	}

    bool OptimizeSpeed(float rate = 1.0f)
    {
		// 如果当前不存在两个元素的桶, 则无需优化
		bool bHaveMore = false;
		for (size_t i = 0; i < mValueArray.size(); ++i)
		{
			if (mValueArray[i].Count() > 1)
			{
				bHaveMore = true;
				break;
			}
		}
		if (!bHaveMore)
			return false;

        if (mValueArray.size() < (int)(mCount * rate))
        {
            _rehash(rate);
            return true;
        }
        return false;
    }

    void Dump() const
    {
        size_t s = mValueArray._useSpaceSize();
        PRINT("Now dump hash array size [%llu]\r\n", (UInt64)mValueArray.size());
        for (size_t i = 0; i < mValueArray.size(); ++i)
        {
            s += mValueArray[i].spaceSize();
    //        if (mValueArray[i].Count() > 0)
    //        {
				//PRINT("%llu = %llu [%llu]\r\n", (UInt64)i, (UInt64)mValueArray[i].Count(), (UInt64)mValueArray[i].spaceSize());
    //        }
        }
		PRINT("END Total use [%llu]----------------------------------------\r\n", (UInt64)s);
    }

protected:
    size_t _hashKey(const K &key) const
    {
        return (size_t)key;
    }

    size_t _hashKey(const char *key) const
    {
        return AString::_hash(key);
    }

    void _checkRehash()
    {
        if ((double)(mCount + 1) / mValueArray.size() >= 2)
        {
            _rehash();
        }
    }

    virtual void _rehash(float rate = 2)
    {
        size_t count = (size_t)(mValueArray.size()*rate);
        if (count <= 2)
            return;
        Array<ValueBucket> temp(count);

        mValueArray.swap(temp);
        mCount = 0;
        for (size_t i = 0; i < temp.size(); ++i)
        {
            ValueBucket &vB = temp[i];
            for (size_t n = 0; n < vB.Count(); ++n)
            {
                Value &kv = *vB[n];
                insert(kv.mKey, kv.mVal);
            }
        }
        temp.clear(true);
    }

    bool _remove(ValueBucket &vB, const K &key)
    {
        for (int i = 0; i < vB.Count(); ++i)
        {
            const Value &kV = *vB[i];
            if (kV.mKey == key)
            {
                vB.removeAt(i);
                --mCount;
                return true;
            }
        }
        return false;
    }

    bool _remove(size_t hashIndex, size_t pos)
    {
        if (hashIndex < mValueArray.size())
        {
            ValueBucket &vB = mValueArray[hashIndex];
            if (vB.removeAt(pos))
            {
                --mCount;
                return true;
            }
        }
        return false;
    }



public:
    EasyHash()
        : mCount(0)
    {
        mValueArray.resize(INIT_COUNT);
    }

	EasyHash(int initBucketCount)
		: mCount(0)
	{
		mValueArray.resize(initBucketCount);
	}

	Value& _getStatic() const
	{
		_THREAD_LOCAL_ static Value		mStatic;
		return mStatic;
	}
public:
    class iterator
    {
        friend class EasyHash;
    public:
        iterator()
            : mFastIndex(0)
            , mOnePos(0)
            , mpHash(NULL)
        {}
        iterator(EasyHash *h)
            : mFastIndex(0)
            , mOnePos(UINT_MAX)
            , mpHash(h)
        {
            next();
        }

        bool operator ++ ()
        {
            return next();
        }

        bool operator ++ (int)
        {
            return next();
        }

        operator bool()
        {
            return have();
        }

        V& operator * ()
        {
            return get();
        }

		// NOTE: 在空闲桶比较多时, 遍历效率会降低
        bool next()
        {
            if (mpHash == NULL)
                return false;
			// 当前桶的下一个如果存在, 则直接递增返回			
			if (++mOnePos < mpHash->mValueArray[mFastIndex].Count())
			{
				return true;
			}
			mOnePos = 0;
			for (size_t i = mFastIndex+1; i < mpHash->mValueArray.size(); ++i)
			{
				ValueBucket &vB = mpHash->mValueArray[i];
				if (!vB.isEmpty())
				{
					mFastIndex = i;
					return true;
				}
			}
			mFastIndex = mpHash->mValueArray.size();
			return false;

            //++mOnePos;

            //for (size_t i = mFastIndex; i < mpHash->mValueArray.size(); ++i)
            //{
            //    ValueBucket &vB = mpHash->mValueArray[i];
            //    if (vB.Count() > 0)
            //    {
            //        int count = vB.Count();

            //        int begin = 0;
            //        if (i == mFastIndex)
            //            begin = mOnePos;
            //        // 如果当前为空，则后面的也一定为空
            //        if (begin < vB.Count())
            //        {
            //            mFastIndex = i;
            //            mOnePos = begin;
            //            return true;
            //        }
            //    }
            //}
            //return false;
        }

        bool previou()
        {
            if (mFastIndex <= 0 && mOnePos <= 0)
            {
                mFastIndex = -1;
                mOnePos = -1;
                return false;
            }

            --mOnePos;
            for (size_t i = mFastIndex; i >= 0; --i)
            {
                if (i < mpHash->mValueArray.size())
                {
                    ValueBucket &vB = mpHash->mValueArray[i];
                    if (mOnePos < vB.Count())
                    {
                        mFastIndex = (uint)i;
                        return true;
                    }
                    if (!vB.isEmpty() && i != mFastIndex)
                    {
                        mFastIndex = (uint)i;
                        mOnePos = vB.Count() - 1;
                        return true;
                    }
                }
                else
                    mOnePos = 0;
            }

            mFastIndex = -1;
            mOnePos = -1;
            return false;
        }

        bool have()
        {
            if (mpHash == NULL)
                return false;

            if (mFastIndex < mpHash->mValueArray.size())
            {
                ValueBucket &vB = mpHash->mValueArray[mFastIndex];
                return mOnePos < vB.Count();
            }
            return false;
        }

        bool reset()
        {
            if (mpHash == NULL)
                return false;

            mFastIndex = 0;
            mOnePos = 0;
            return have();
        }

        bool erase()
        {
            if (mpHash == NULL)
                return false;
            bool b = mpHash->_remove(mFastIndex, mOnePos);
            if (!have())	//NOTE: 数组列表删除方式是将最后面元素直接放在删除元素的位置，所以只有当前为最后元素时才需要移到到下1个列表内
                next();
            return b;
        }

        const K& key()
        {
            AssertNote(mpHash != NULL, "HASH is NULL");
            if (mFastIndex < mpHash->mValueArray.size())
            {
                ValueBucket &vB = mpHash->mValueArray[mFastIndex];
                if (mOnePos < vB.Count())
                    return (*vB[mOnePos]).mKey;
            }
            return mpHash->_getStatic().mKey;
        }

        V& get()
        {
            AssertNote(mpHash != NULL, "HASH is NULL");
            if (mFastIndex < mpHash->mValueArray.size())
            {
                ValueBucket &vB = mpHash->mValueArray[mFastIndex];
                if (mOnePos < vB.Count())
                    return (*vB[mOnePos]).mVal;
            }
            return mpHash->_getStatic().mVal;
        }

		EasyHash::Value& Value()
        {
            AssertNote(mpHash != NULL, "HASH is NULL");
            if (mFastIndex < mpHash->mValueArray.size())
            {
                ValueBucket &vB = mpHash->mValueArray[mFastIndex];
                if (mOnePos < vB.Count())
                    return *vB[mOnePos];
            }
            return mpHash->_getStatic();
        }

    protected:
		DSIZE mFastIndex;
		DSIZE mOnePos;
        EasyHash *mpHash;
    };

public:
    iterator begin()
    {
        return iterator(this);
    }

    void clear(bool bClearBuffer = false)
    {
        if (bClearBuffer)
        {
            mValueArray.clear(bClearBuffer);
            mValueArray.resize(INIT_COUNT);
        }
        else
        {
            for (size_t i = 0; i < mValueArray.size(); ++i)
            {
                mValueArray[i].clearUp(false);
            }
        }
        mCount = 0;
    }

    bool findExBy(const char *szKey, V &val) const
    {
        const V* p = findPtr(szKey);
        if (p != NULL)
        {
            val = *p;
            return true;
        }
        return false;
    }

    iterator findIt(const char *szKey)
    {
        size_t k = _hashKey(szKey) % mValueArray.size();
        const ValueBucket &vB = mValueArray[k];
        for (int i = 0; i < vB.Count(); ++i)
        {
            const Value &kV = *vB[i];
            if (CompareKey(kV.mKey, szKey))
            {
                iterator it(this);
                it.mOnePos = i;
                it.mFastIndex = k;
                return it;
            }
        }
        iterator it;
        it.mFastIndex = UINT_MAX;
        it.mOnePos = UINT_MAX;
        return it;
    }

    iterator findIt(const K &key)
    {
        size_t k = _hashKey(key) % mValueArray.size();
        const ValueBucket &vB = mValueArray[k];
        for (int i = 0; i < vB.Count(); ++i)
        {
            const Value &kV = *vB[i];
            if (kV.mKey == key)
            {
                iterator it(this);
                it.mOnePos = i;
                it.mFastIndex = k;
                return it;
            }
        }
        iterator it;
        it.mFastIndex = UINT_MAX;
        it.mOnePos = UINT_MAX;
        return it;
    }

    bool CompareKey(const K &key, const char *szKey)
    {
        return key == szKey;
    }

    iterator GetLastIterator()
    {
        for (size_t i = mValueArray.size() - 1; i >= 0; --i)
        {
            if (!mValueArray[i].isEmpty())
            {
                iterator it;
                it.mFastIndex = (uint)i;
                it.mOnePos = mValueArray[i].Count() - 1;
                return it;
            }
        }
        return iterator();
    }

    V& back()
    {
        for (size_t k = mValueArray.size() - 1; k >= 0; --k)
        {
            if (!mValueArray[k].isEmpty())
            {
                const ValueBucket &vB = mValueArray[k];
                return vB[vB.Count() - 1]->mVal;
            }
        }
        return _getStatic().mVal;
    }

    bool exist(const K &key) const
    {
        return findPtr(key) != NULL;
    }

    bool exist(const char *szKey) const
    {
        return findPtr(szKey) != NULL;
    }

    bool empty() const
    {
        return mCount <= 0;
    }

    int size() const { return mCount; }

    void swap(EasyHash &other)
    {
        mValueArray.swap(other.mValueArray);
        size_t temp = mCount;
        mCount = other.mCount;
        other.mCount = temp;
    }

protected:
    Array<ValueBucket>	mValueArray;
    size_t				mCount;

};


#pragma pack(pop)
//-------------------------------------------------------------------------
#endif //_INCLUDE_EASYHASH_H_

#else  //USE_HASH_EX

#ifndef _INCLUDE_EASYHASH_H_
#define _INCLUDE_EASYHASH_H_

#include "Array.h"

#define HASH_INIT_BUCKET_COUNT		(8)

//-------------------------------------------------------------------------*/
// 专用于Hash中的ArrayList, 区别不再分配静态空值对象
// 精减内存使用
// NOTE: mValueArray 不可为空
// 2019-9-9 每个桶初始时不分配数组空间, 桶内加入元素时初始1个元素
// 持续优化: 进一步优化可以在Value中直接加入一个T对象, 存一般只有一个元素, 不分配数组空间, 理论上效率更优
//-------------------------------------------------------------------------*/
#pragma pack(push)
#pragma pack(1)
template<typename T>
class ArrayListForHash : public tArray<T, 1, unsigned int>
{
public:
	T* operator [] (size_t pos)
	{
		if (pos>=mCount)
		{
			return NULL;
		}
		return &mpArrayPtr[pos];
	}

	const T* operator [] (size_t pos) const
	{
		if (pos>=mCount)
		{
			return NULL;
		}
		return &mpArrayPtr[pos];
	}

public:
	void add(const T &value)
	{
		push_back(value);
	}

	bool removeAt(size_t pos)
	{
		if (pos>=0 && pos<size()-1)
		{
			mpArrayPtr[pos] = mpArrayPtr[size()-1];
			mpArrayPtr[size()-1] = T();
			--mCount;
			return true;
		}
		else if (pos==size()-1)
		{
			mpArrayPtr[size()-1] = T();
			--mCount;
			return true;
		}
		return false;
	}

	bool remove(const T &value)
	{
		for (size_t i=0; i<size(); ++i)
		{
			if (mpArrayPtr[i]==value)
			{
				return removeAt(i);
			}
		}
		return false;
	}

public:
	ArrayListForHash(){}
	ArrayListForHash( const ArrayListForHash &other )
	{
		*this = other;
	}

	ArrayListForHash& operator = (const ArrayListForHash &other)
	{
		clear(false);
		resize(other.size());
		for (size_t i=0; i< other.size(); ++i)
		{
			mpArrayPtr[i] = other.mpArrayPtr[i];
		}
		return *this;
	}
};
/*/-------------------------------------------------------------------------
测试结果总结：
	1 EasyHash (现名) ArrayListHash 实际测试比hash_map 要快， 且对于增加删除时比较有优势，但空间占用稍多于其他
	2 EasyMapHash 在数量多时慢于 hash_map, 少时略快
	3 EasyOneHash 由于必须使用 string_id, 慢于以上所有HASH
//-------------------------------------------------------------------------
// NOTE: 
// 简易的Hash容器, 桶使用ArraylList, 适合频繁变化的情况
// 重分配桶数量条件： 当数量超过桶数量的2倍
// 性能：15000数量下，查寻150000次，创建31 查寻用时 576， stdext::hash_map 创建31，查寻用时564
//------------------------------------------------------------------------*/
template<typename K, typename V, int INIT_COUNT = 8>
class EasyHash : public MemBase
{
public:
	struct Value
	{
	public:
		K			mKey;
		mutable V   	mVal;

	public:
		Value()
			: mKey()
			, mVal()
		{

		}
	};
	typedef ArrayListForHash<Value>	ValueBucket;

public:
	V& operator [] (const K &key)
	{
		V *p = findPtr(key);
		if (p==NULL)
		{
			insert(key, V());
			p = findPtr(key);
		}
		return *p;
	}

	const V& operator [] (const K &key) const
	{
		return find(key);
	}

	// NOTE: Replace when exist key
	void insert(const K &key, const V &val)
	{
		_checkRehash();
		size_t k = _hashKey(key) % mValueArray.size();
		ValueBucket &vB = mValueArray[k];
		//_remove(vB, key);		// 优化如下, 存在直接覆盖 2017.6.20
		for (int i=0; i<vB.size(); ++i)
		{
			Value &kV = *vB[i];
			if (kV.mKey==key)
			{
				kV.mVal = val;
				return;
			}
		}

		Value v;
		v.mKey = key;
		v.mVal = val;
		mValueArray[k].add(v);
		++mCount;
	}

	bool erase(const K &key)
	{
		size_t k = _hashKey(key) % mValueArray.size();
		return _remove(mValueArray[k], key);
	}

	bool erase(const K &key, V &exist)
	{
		size_t k = _hashKey(key) % mValueArray.size();
		ValueBucket &vB = mValueArray[k];
		for (int i=0; i<vB.size(); ++i)
		{
			if ((*vB[i]).mKey==key)
			{
				exist = (*vB[i]).mVal;
				vB.removeAt(i);
				--mCount;
				return true;
			}
		}
		return false;
	}

	V& find(const K &key) const
	{
		V* p = findPtr(key);
		if (p!=NULL)
			return *p;

		return _getStatic().mVal;
	}

	V& find(const char *key) const
	{
		V* p = findPtr(key);
		if (p!=NULL)
			return *p;

		return _getStatic().mVal;
	}

	V* findPtr(const K &key) const
	{
		size_t k = _hashKey(key) % mValueArray.size();
		const ValueBucket &vB = mValueArray[k];
		for (int i=0; i<vB.size(); ++i)
		{
			const Value &kV = *vB[i];
			if (kV.mKey==key)
			{
				return &(kV.mVal);
			}
		}
		return NULL;
	}

	V* findPtr(const char *key) const
	{
		size_t k = _hashKey(key) % mValueArray.size();
		const ValueBucket &vB = mValueArray[k];
		for (int i=0; i<vB.size(); ++i)
		{
			const Value &kV = *vB[i];
			if (kV.mKey==key)
			{
				return &(kV.mVal);
			}
		}
		return NULL;
	}

	bool OptimizeSpeed(float rate = 1.0f)
	{
		if (mValueArray.size() < (int)(mCount * rate))
		{
			_rehash(rate);
			return true;
		}
		return false;
	}

	void Dump() const
	{
		size_t s = mValueArray._useSpaceSize();
		PRINT("Now dump hash array size [%llu]\r\n", mValueArray.size());
		for (size_t i=0; i<mValueArray.size(); ++i)
		{
			s += mValueArray[i]._useSpaceSize();
			if (mValueArray[i].size()>0)
			{
				PRINT("%llu = %llu [%llu]\r\n", i, mValueArray[i].size(), mValueArray[i]._useSpaceSize());
			}
		}
		PRINT("END Total use [%llu]----------------------------------------\r\n", s);
	}

protected:
	size_t _hashKey(const K &key) const
	{
		return (size_t)key;
	}

	size_t _hashKey(const char *key) const
	{	
		return AString::_hash(key);
	}

	void _checkRehash()
	{
		if ((double)(mCount+1)/mValueArray.size()>=2)
		{
			_rehash();
		}
	}

	void _rehash(float rate = 2)
	{
		size_t count = (size_t)(mValueArray.size()*rate);
		if (count<=2)
			return;
		Array<ValueBucket> temp(count);		

		mValueArray.swap(temp);
		mCount = 0;
		for (size_t i=0; i<temp.size(); ++i)
		{
			ValueBucket &vB = temp[i];
			for (size_t n=0; n<vB.size(); ++n)
			{
				Value &kv = *vB[n];				
				insert(kv.mKey, kv.mVal);
			}
		}
		temp.clear(true);
	}

	bool _remove(ValueBucket &vB, const K &key)
	{
		for (int i=0; i<vB.size(); ++i)
		{
			const Value &kV = *vB[i];
			if (kV.mKey==key)
			{
				vB.removeAt(i);
				--mCount;
				return true;
			}
		}
		return false;
	}

	bool _remove(size_t hashIndex, size_t pos)
	{
		if (hashIndex<mValueArray.size())
		{
			ValueBucket &vB = mValueArray[hashIndex];
			if (vB.removeAt(pos))
			{
				--mCount;
				return true;
			}
		}
		return false;
	}

	Value& _getStatic() const
	{
		_THREAD_LOCAL_ static Value		mStatic;
		return mStatic;
	}

public:
	EasyHash()
		: mCount(0)
	{
		mValueArray.resize(INIT_COUNT);
	}

public:
	class iterator
	{
		friend class EasyHash;
	public:		
		iterator()
			: mFastIndex(0)
			, mOnePos(0)
			, mpHash(NULL)
		{}
		iterator(EasyHash *h)
			: mFastIndex(0)
			, mOnePos(UINT_MAX)
			, mpHash(h)
		{
			next();
		}

		bool operator ++ ()
		{
			return next();
		}

		bool operator ++ (int)
		{
			return next();
		}

		operator bool ()
		{
			return have();
		}

		V& operator * ()
		{
			return get();
		}

		bool next()
		{
			if (mpHash==NULL)
				return false;

			++mOnePos;

			for (size_t i=mFastIndex; i<mpHash->mValueArray.size(); ++i)
			{
				ValueBucket &vB = mpHash->mValueArray[i];
				if (vB.size()>0)
				{
					int count = vB.size();

					int begin = 0;
					if (i==mFastIndex)
						begin = mOnePos;
					// 如果当前为空，则后面的也一定为空
					if (begin<vB.size())
					{
						mFastIndex = i;
						mOnePos = begin;
						return true;
					}
				}
			}
			return false;
		}

		bool previou()
		{
			if (mFastIndex<=0 && mOnePos<=0)
			{
				mFastIndex = -1;
				mOnePos = -1;
				return false;
			}

			--mOnePos;
			for (Int64 i=mFastIndex; i>=0; --i)
			{
				if (i<(Int64)mpHash->mValueArray.size())
				{
					ValueBucket &vB = mpHash->mValueArray[i];
					if (mOnePos<vB.size())
					{
						mFastIndex = (uint)i;
						return true;
					}
					if (!vB.empty()&& i!=mFastIndex)
					{
						mFastIndex = (uint)i;
						mOnePos = vB.size()-1;
						return true;
					}
				}				
				else
					mOnePos = 0;
			}

			mFastIndex = -1;
			mOnePos = -1;
			return false;
		}

		bool have()
		{
			if (mpHash==NULL)
				return false;

			if (mFastIndex<mpHash->mValueArray.size())
			{
				ValueBucket &vB = mpHash->mValueArray[mFastIndex];
				return mOnePos<vB.size();			
			}
			return false;
		}

		bool reset()
		{
			if (mpHash==NULL)
				return false;

			mFastIndex = 0;
			mOnePos = 0;
			return have();
		}

		bool erase()
		{
			if (mpHash==NULL)
				return false;
			bool b = mpHash->_remove(mFastIndex, mOnePos);
			if (!have())	//NOTE: 数组列表删除方式是将最后面元素直接放在删除元素的位置，所以只有当前为最后元素时才需要移到到下1个列表内
				next();
			return b;
		}

		const K& key()
		{
			AssertNote(mpHash!=NULL, "HASH is NULL");
			if (mFastIndex<mpHash->mValueArray.size())
			{
				ValueBucket &vB = mpHash->mValueArray[mFastIndex];
				if (mOnePos<vB.size())
					return (*vB[mOnePos]).mKey;
			}
			return mpHash->_getStatic().mKey;
		}

		V& get()
		{
			AssertNote(mpHash!=NULL, "HASH is NULL");
			if (mFastIndex<mpHash->mValueArray.size())
			{
				ValueBucket &vB = mpHash->mValueArray[mFastIndex];
				if (mOnePos<vB.size())
					return (*vB[mOnePos]).mVal;
			}
			return mpHash->_getStatic().mVal;
		}

		EasyHash::Value& Value()
		{
			AssertNote(mpHash!=NULL, "HASH is NULL");
			if (mFastIndex<mpHash->mValueArray.size())
			{
				ValueBucket &vB = mpHash->mValueArray[mFastIndex];
				if (mOnePos<vB.size())
					return *vB[mOnePos];
			}
			return mpHash->_getStatic();
		}

	protected:
		uint	mFastIndex;
		uint	mOnePos;
		EasyHash *mpHash;
	};

public:
	iterator begin()
	{
		return iterator(this);
	}	

	void clear(bool bClearBuffer = false)
	{
		if (bClearBuffer)
		{
			mValueArray.clear(bClearBuffer);
			mValueArray.resize(INIT_COUNT);
		}
		else
		{
			for (size_t i=0; i<mValueArray.size(); ++i)
			{
				mValueArray[i].clear(false);
			}
		}
		mCount = 0;
	}

	bool findExBy(const char *szKey, V &val) const
	{
		const V* p = findPtr(szKey);
		if (p!=NULL)
		{
			val = *p;
			return true;
		}
		return false;
	}

	iterator findIt(const char *szKey)
	{
		size_t k = _hashKey(szKey) % mValueArray.size();
		const ValueBucket &vB = mValueArray[k];
		for (int i=0; i<vB.size(); ++i)
		{
			const Value &kV = *vB[i];
			if (CompareKey(kV.mKey, szKey))
			{
				iterator it(this);
				it.mOnePos = i;
				it.mFastIndex = k;
				return it;
			}
		}
		iterator it;
		it.mFastIndex = UINT_MAX;
		it.mOnePos = UINT_MAX;
		return it;
	}

	iterator findIt(const K &key)
	{
		size_t k = _hashKey(key) % mValueArray.size();
		const ValueBucket &vB = mValueArray[k];
		for (int i = 0; i < vB.size(); ++i)
		{
			const Value &kV = *vB[i];
			if (kV.mKey == key)
			{
				iterator it(this);
				it.mOnePos = i;
				it.mFastIndex = k;
				return it;
			}
		}
		iterator it;
		it.mFastIndex = UINT_MAX;
		it.mOnePos = UINT_MAX;
		return it;
	}

	bool CompareKey(const K &key, const char *szKey)
	{
		return key==szKey;
	}

	iterator GetLastIterator()
	{
		for (Int64 i=mValueArray.size()-1; i>=0; --i)
		{
			if (!mValueArray[i].empty())
			{
				iterator it;
				it.mFastIndex = (uint)i;
				it.mOnePos = mValueArray[i].size()-1;
				return it;
			}
		}
		return iterator();
	}

	V& back()
	{
		for (Int64 k=mValueArray.size()-1; k>=0; --k)
		{
			if (!mValueArray[k].empty())
			{				
				const ValueBucket &vB = mValueArray[k];
				return vB[vB.size()-1]->mVal;				
			}
		}
		return _getStatic().mVal;
	}

	bool exist(const K &key) const
	{
		return findPtr(key)!=NULL;
	}

	bool exist(const char *szKey) const
	{
		return findPtr(szKey)!=NULL;
	}

	bool empty() const
	{
		return mCount<=0;
	}

	int size() const { return mCount; }

	void swap(EasyHash &other)
	{
		mValueArray.swap(other.mValueArray);
		size_t temp = mCount;
		mCount = other.mCount;
		other.mCount = temp;
	}

protected:
	Array<ValueBucket>	mValueArray;
	size_t				mCount;
};


#pragma pack(pop)
//-------------------------------------------------------------------------
#endif //_INCLUDE_EASYHASH_H_

#endif //USE_HASH_EX