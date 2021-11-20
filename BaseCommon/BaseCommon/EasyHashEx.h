#ifndef _INCLUDE_EASYHASH_H_
#define _INCLUDE_EASYHASH_H_

#include "Array.h"

#define HASH_INIT_BUCKET_COUNT		(8)

//-------------------------------------------------------------------------*/
// ר����Hash�е�ArrayList, �����ٷ��侲̬��ֵ����
// �����ڴ�ʹ��
// NOTE: mValueArray ����Ϊ��
// 2019-9-9 ÿ��Ͱ��ʼʱ����������ռ�, Ͱ�ڼ���Ԫ��ʱ��ʼ1��Ԫ��
// �����Ż�: ��һ���Ż�������Value��ֱ�Ӽ���һ��T����, ��һ��ֻ��һ��Ԫ��, ����������ռ�, ������Ч�ʸ���
//-------------------------------------------------------------------------*/
#pragma pack(push)
#pragma pack(1)
template<typename T, int bucketInitSize>
class ArrayListForHash : public  tArray<T, bucketInitSize, unsigned short>  // �ߴ�����Ϊ�޷��Ŷ�����,ֻ��Ҫ����� 65535, �� push_back����Ҫ����������    
{
    typedef tArray<T, bucketInitSize, unsigned short> parent;

private:   
    T   mFirstValue;
    bool mbUsedFirstValue;

public:
	T* operator [] (size_t pos)
	{
		if (pos<= 0)
		{
			return &mFirstValue;
		}
        AssertNote(pos - 1 < mCount, "Code error");
		return &mpArrayPtr[pos-1];
	}

	const T* operator [] (size_t pos) const
	{
        if (pos <= 0)
        {
            return &mFirstValue;
        }
        AssertNote(pos - 1 < mCount, "Code error");		
		return &mpArrayPtr[pos-1];
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
        if (pos<=0)
        {
            if (mCount>0)
            {
                mFirstValue = mpArrayPtr[mCount - 1];
                _removeAt(mCount - 1);
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

		for (size_t i=0; i<size(); ++i)
		{
			if (mpArrayPtr[i]==value)
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
        if (pos >= 0 && pos < mCount - 1)
        {
            mpArrayPtr[pos] = mpArrayPtr[mCount - 1];
            mpArrayPtr[mCount - 1] = T();
            --mCount;
            return true;
        }
        else if (pos == mCount - 1)
        {
            mpArrayPtr[mCount - 1] = T();
            --mCount;
            return true;
        }
        return false;
    }

    size_t size() const { return mCount; }
    bool empty()const { return 0 == mCount; }

    void push_back(T val) { parent::push_back(val); }
    void pop_back() { parent::pop_back(); }
    bool remove(size_t pos) { parent::remove(pos);  }
    void clear(bool bFreeBuffer = true) { parent::clear(bFreeBuffer); }
    void resize(size_t num, T def = T()) { parent::resize(num, def); }
    void swap(tArray &other) { parent::swap(other); }
    size_t _useSpaceSize() const { return parent::_useSpaceSize(); }

public:
	ArrayListForHash(): mbUsedFirstValue(false) {}
	ArrayListForHash( const ArrayListForHash &other )
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
            mpArrayPtr[i] = other.mpArrayPtr[i];
        }
        return *this;
    }
};
/*/-------------------------------------------------------------------------
���Խ���ܽ᣺
	1 EasyHash (����) ArrayListHash ʵ�ʲ��Ա�hash_map Ҫ�죬 �Ҷ�������ɾ��ʱ�Ƚ������ƣ����ռ�ռ���Զ�������
	2 EasyMapHash ��������ʱ���� hash_map, ��ʱ�Կ�
	3 EasyOneHash ���ڱ���ʹ�� string_id, ������������HASH
//-------------------------------------------------------------------------
// NOTE: 
// ���׵�Hash����, Ͱʹ��ArraylList, �ʺ�Ƶ���仯�����
// �ط���Ͱ���������� ����������Ͱ������2��
// ���ܣ�15000�����£���Ѱ150000�Σ�����31 ��Ѱ��ʱ 576�� stdext::hash_map ����31����Ѱ��ʱ564
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
		//_remove(vB, key);		// �Ż�����, ����ֱ�Ӹ��� 2017.6.20
		for (int i=0; i<vB.Count(); ++i)
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
		for (int i=0; i<vB.Count(); ++i)
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
		for (int i=0; i<vB.Count(); ++i)
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
		for (int i=0; i<vB.Count(); ++i)
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
		printf("Now dump hash array size [%llu]\r\n", mValueArray.size());
		for (size_t i=0; i<mValueArray.size(); ++i)
		{
			s += mValueArray[i].spaceSize();
			if (mValueArray[i].Count()>0)
			{
				printf("%llu = %llu [%llu]\r\n", i, mValueArray[i].Count(), mValueArray[i].spaceSize());
			}
		}
		printf("END Total use [%llu]----------------------------------------\r\n", s);
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
			for (size_t n=0; n<vB.Count(); ++n)
			{
				Value &kv = *vB[n];				
				insert(kv.mKey, kv.mVal);
			}
		}
		temp.clear(true);
	}

	bool _remove(ValueBucket &vB, const K &key)
	{
		for (int i=0; i<vB.Count(); ++i)
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
				if (vB.Count()>0)
				{
					int count = vB.Count();

					int begin = 0;
					if (i==mFastIndex)
						begin = mOnePos;
					// �����ǰΪ�գ�������Ҳһ��Ϊ��
					if (begin<vB.Count())
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
					if (mOnePos<vB.Count())
					{
						mFastIndex = (uint)i;
						return true;
					}
					if (!vB.isEmpty()&& i!=mFastIndex)
					{
						mFastIndex = (uint)i;
						mOnePos = vB.Count()-1;
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
				return mOnePos<vB.Count();			
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
			if (!have())	//NOTE: �����б�ɾ����ʽ�ǽ������Ԫ��ֱ�ӷ���ɾ��Ԫ�ص�λ�ã�����ֻ�е�ǰΪ���Ԫ��ʱ����Ҫ�Ƶ�����1���б���
				next();
			return b;
		}

		const K& key()
		{
			AssertNote(mpHash!=NULL, "HASH is NULL");
			if (mFastIndex<mpHash->mValueArray.size())
			{
				ValueBucket &vB = mpHash->mValueArray[mFastIndex];
				if (mOnePos<vB.Count())
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
				if (mOnePos<vB.Count())
					return (*vB[mOnePos]).mVal;
			}
			return mpHash->_getStatic().mVal;
		}

		Value& Value()
		{
			AssertNote(mpHash!=NULL, "HASH is NULL");
			if (mFastIndex<mpHash->mValueArray.size())
			{
				ValueBucket &vB = mpHash->mValueArray[mFastIndex];
				if (mOnePos<vB.Count())
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
				mValueArray[i].clearUp(false);
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
		for (int i=0; i<vB.Count(); ++i)
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
		return key==szKey;
	}

	iterator GetLastIterator()
	{
		for (Int64 i=mValueArray.size()-1; i>=0; --i)
		{
			if (!mValueArray[i].isEmpty())
			{
				iterator it;
				it.mFastIndex = (uint)i;
				it.mOnePos = mValueArray[i].Count()-1;
				return it;
			}
		}
		return iterator();
	}

	V& back()
	{
		for (Int64 k=mValueArray.size()-1; k>=0; --k)
		{
			if (!mValueArray[k].isEmpty())
			{				
				const ValueBucket &vB = mValueArray[k];
				return vB[vB.Count()-1]->mVal;				
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
#if FIELD_INFO_THREAD_SAFE
	mutable Value		mStatic;		// ֧�ֶ���ʱ�����ڷ��ز����ڵ�����
#else
	static Value		mStatic;
#endif
};

#if !FIELD_INFO_THREAD_SAFE
template<typename K, typename V, int INIT_COUNT, int bucketInteSize>
typename EasyHash<K, V, INIT_COUNT, bucketInteSize>::Value EasyHash<K, V, INIT_COUNT, bucketInteSize>::mStatic;
#endif

#pragma pack(pop)
//-------------------------------------------------------------------------
#endif //_INCLUDE_EASYHASH_H_