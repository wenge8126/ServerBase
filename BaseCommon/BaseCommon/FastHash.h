#pragma once

#ifndef _INCLDUE_FASTHASH_H_
#define _INCLDUE_FASTHASH_H_



#include "Array.h"
#include "EasyMap.h"
#include "Common.h"

#define HASH_INIT_BUCKET_COUNT		(8)

//-------------------------------------------------------------------------*/
//  使用EasyMap作为桶,优化查询速度
// 1 空间与Array较节省
// 2 桶的数量为总数量的平方根*2, 远少于Array方式
//-------------------------------------------------------------------------*/
#pragma pack(push)
#pragma pack(1)


template<typename K, typename V, int INIT_COUNT = 8, int bucketInitSize = 1>
class FastHash : public MemBase
{
public:
	typedef EasyMapValue<K, V> Value;
	typedef EasyMap<K, V, 2, false, false, ushort>	ValueBucket;

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
		// 优化如下, 存在直接覆盖 2017.6.20
		if (vB.erase(key) <= 0)
			++mCount;
		vB.insert(key, val);
	}

	bool erase(const K &key)
	{
		size_t k = _hashKey(key) % mValueArray.size();
		ValueBucket &vB = mValueArray[k];
		if (vB.erase(key)>0)
		{
			--mCount;
			return true;
		}
		return false;
	}

	bool erase(const K &key, V &exist)
	{
		size_t k = _hashKey(key) % mValueArray.size();
		ValueBucket &vB = mValueArray[k];
		V *pValue = vB.findPtr(key);
		if (pValue != NULL)
		{
			exist = *pValue;
			if (vB.erase(key)>0)
			{
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
		
		return vB.findPtr(key);
	}

	V* findPtr(const char *key) const
	{
		size_t k = _hashKey(key) % mValueArray.size();
		const ValueBucket &vB = mValueArray[k];

		return vB.findPtr(key);
	}


	void Dump() const
	{
		size_t s = mValueArray._useSpaceSize();
		PRINT("Now dump hash array size [%llu]\r\n", (UInt64)mValueArray.size());
		for (size_t i = 0; i < mValueArray.size(); ++i)
		{
			s += mValueArray[i]._useSpaceSize();
			//if (mValueArray[i].size() > 0)
			//{
			//	PRINT("%llu = %llu [%llu]\r\n", (UInt64)i, (UInt64)mValueArray[i].size(), (UInt64)mValueArray[i]._useSpaceSize());
			//}
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
		size_t needSize = (size_t)sqrt(mCount);
		if (mValueArray.size() < needSize)
		{
			_rehash();
		}
	}

	virtual void _rehash()
	{
		size_t count = (size_t)sqrt(mCount) * 2; // (size_t)(mValueArray.size()*rate);
		if (count <= 2)
			return;
		Array<ValueBucket> temp(count);

		mValueArray.swap(temp);
		mCount = 0;
		for (size_t i = 0; i < temp.size(); ++i)
		{
			ValueBucket &vB = temp[i];
			for (size_t n = 0; n < vB.size(); ++n)
			{
				const Value &kv = vB.getValue(n);
				insert(kv.mKey, kv.mVal);
			}
		}
		temp.clear(true);
	}

	bool _remove(size_t hashIndex, size_t pos)
	{
		if (hashIndex < mValueArray.size())
		{
			ValueBucket &vB = mValueArray[hashIndex];
			if (vB._remove(pos))
			{
				--mCount;
				return true;
			}
		}
		return false;
	}



public:
	FastHash()
		: mCount(0)
	{
		mValueArray.resize(INIT_COUNT);
	}

	FastHash(int initBucketCount)
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
		friend class FastHash;
	public:
		iterator()
			: mFastIndex(0)
			, mOnePos(0)
			, mpHash(NULL)
		{}
		iterator(FastHash *h)
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
			if (++mOnePos < mpHash->mValueArray[mFastIndex].size())
			{
				return true;
			}
			mOnePos = 0;
			for (size_t i = mFastIndex + 1; i < mpHash->mValueArray.size(); ++i)
			{
				ValueBucket &vB = mpHash->mValueArray[i];
				if (!vB.empty())
				{
					mFastIndex = i;
					return true;
				}
			}
			mFastIndex = mpHash->mValueArray.size();
			return false;
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
					if (mOnePos < vB.size())
					{
						mFastIndex = (uint)i;
						return true;
					}
					if (!vB.empty() && i != mFastIndex)
					{
						mFastIndex = (uint)i;
						mOnePos = vB.size() - 1;
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
				return mOnePos < vB.size();
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
				if (mOnePos < vB.size())
					return vB.getKey(mOnePos);
			}
			return mpHash->_getStatic().mKey;
		}

		V& get()
		{
			AssertNote(mpHash != NULL, "HASH is NULL");
			if (mFastIndex < mpHash->mValueArray.size())
			{
				ValueBucket &vB = mpHash->mValueArray[mFastIndex];
				if (mOnePos < vB.size())
					return vB.get(mOnePos);
			}
			return mpHash->_getStatic().mVal;
		}

		FastHash::Value& Value()
		{
			AssertNote(mpHash != NULL, "HASH is NULL");
			if (mFastIndex < mpHash->mValueArray.size())
			{
				ValueBucket &vB = mpHash->mValueArray[mFastIndex];
				if (mOnePos < vB.size())
					return vB.getValue(mOnePos);
			}
			return mpHash->_getStatic();
		}

	protected:
		DSIZE	mFastIndex;
		DSIZE	mOnePos;
		FastHash *mpHash;
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
				mValueArray[i].clear(false);
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

		size_t pos = vB._find(szKey);
		if (pos != NULL_POS)
		{
			iterator it(this);
			it.mOnePos = (DSIZE)pos;
			it.mFastIndex = k;
			return it;
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
		size_t pos = vB._find(key);
		if (pos != NULL_POS)
		{
			iterator it(this);
			it.mOnePos = (DSIZE)pos;
			it.mFastIndex = k;
			return it;
		}

		iterator it;
		it.mFastIndex = (DSIZE)UINT_MAX;
		it.mOnePos = (DSIZE)UINT_MAX;
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
			if (!mValueArray[i].empty())
			{
				iterator it;
				it.mFastIndex = (DSIZE)i;
				it.mOnePos = mValueArray[i].size() - 1;
				return it;
			}
		}
		return iterator();
	}

	V& back()
	{
		for (size_t k = mValueArray.size() - 1; k >= 0; --k)
		{
			if (!mValueArray[k].empty())
			{
				ValueBucket &vB = mValueArray[k];
				return vB.get(vB.size() - 1);
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

	DSIZE size() const { return mCount; }

	void swap(FastHash &other)
	{
		mValueArray.swap(other.mValueArray);
		size_t temp = mCount;
		mCount = other.mCount;
		other.mCount = temp;
	}

protected:
	Array<ValueBucket>	mValueArray;
	DSIZE				mCount;

};


#pragma pack(pop)
//-------------------------------------------------------------------------


#endif //_INCLDUE_FASTHASH_H_
