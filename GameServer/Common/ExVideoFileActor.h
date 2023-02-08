#pragma once

#include "Actor.h"
#include "FileDataStream.h"
#include "EasyHash.h"
//-------------------------------------------------------------------------
// 外部视频文件扩展保存文件与管理, 不与SQL偶合, 可独立保存与加载
// 索引信息文件在对应的仓库目录中 (t_info.txt, t_info.db)
// NOTE: 由于此组件使用了同步保存大文件, 影响运行效率, 需要把这个单独放到一个进程里, 独立于DB
//-------------------------------------------------------------------------
class ExVideoDataComponent : public Component
{
public:
	typedef Auto< EX_VideoFileIndex> AEX_VideoFileIndex;

public:
	EasyHash<AString, AEX_VideoFileIndex>	mVideoFileInfoList;

	// 注意跨域问题
	AString mMainAddress = "nice177.com/Data/";

	AString mFileDirectory = "D:/xampp/htdocs/Data/";

	Hand<UploadCacheComponent> mBigDataCacheComponent;

public:
	AutoData TakeOutCacheData(int cacheID)
	{
		if (!mBigDataCacheComponent)
		{
			mBigDataCacheComponent = GetActor()->GetComponent<UploadCacheComponent>();
			if (!mBigDataCacheComponent)
			{
				ERROR_LOG("UploadCacheComponent no exist");
				return AutoData();
			}
		}

		return mBigDataCacheComponent->TakeOutData(cacheID);
	}

	AEX_VideoFileIndex FindOrCreateIndex(const AString &typeIndex)
	{
		AString type = typeIndex;
		type.toLower();
		AEX_VideoFileIndex index = mVideoFileInfoList.find(type);
		if (!index)
		{
			index = MEM_NEW EX_VideoFileIndex();
			index->mType = type;
			index->mMaxKey = 0;

			AString fileName = mFileDirectory;
			fileName += type;
			
			fileName += "/";
			fileName += type;
			fileName += ".index";
			FileDataStream f(fileName.c_str(), FILE_READ);
			if (f)
			{
				AutoData d = f.readAllData();
				if (d)
				{
					d->seek(0);
					if (index->restore(d.getPtr()))
					{
						mVideoFileInfoList.insert(type, index);
						return index;
					}
				}
				f.close();
			}
			index->mMaxKey = 0;
			index->mType = type;
			AString path = mFileDirectory;
			path += type;
			FileDataStream::CreateMultiDirectory(path.c_str());
			SaveIndex(index);
		}
		return index;
	}

	bool SaveIndex(AEX_VideoFileIndex index)
	{
		AString fileName;
		fileName.Format("%s%s/%s.index", mFileDirectory.c_str(), index->mType.c_str(), index->mType.c_str());
		
		FileDataStream f(fileName.c_str(), FILE_CREATE);
		if (f)
		{
			AutoData d = MEM_NEW DataBuffer();
			if (index->serialize(d.getPtr()))
			{
				f._write(d->data(), d->dataSize());
				return true;
			}
			else
				ERROR_LOG("Serialize fail : %s", fileName.c_str());
		}
		else
			ERROR_LOG("Create file fail : %s", fileName.c_str());
		return false;
	}

	AString GetVideoFileName(AEX_VideoFileIndex index, Int64 key, bool bGrowth, Int64 &newKey)
	{
		newKey = key;
		if (bGrowth)
		{
			newKey = index->mMaxKey + 1;
		}
		AString fileName;
		fileName.Format("%s%s/%s", mFileDirectory.c_str(), index->mType.c_str(), STRING(newKey));
		return fileName;
	}

public:
	void On(EX_SaveVideoData &req, EX_ResponseSaveVideo &response, UnitID, int)
	{
		if (req.mType.length() <= 0)
		{
			response.mError = eError_ResourceCheckFail;
			return;
		}

		AutoData d = TakeOutCacheData(req.mCacheID);
		if (!d)
		{
			response.mError = eError_Resource_NoExist;
			return;
		}

		AEX_VideoFileIndex index = FindOrCreateIndex(req.mType);

		// 保存到目录 
		AString fileName = GetVideoFileName(index, req.mKey, req.mbGrowth, response.mIndexKey);

		fileName += ".";
		fileName += req.mExtName;

		FileDataStream f(fileName.c_str(), FILE_CREATE);
		f._write(d->data(), d->dataSize());
		f.close();

		index->mMaxKey++;
		SaveIndex(index);

	}

	void On(EX_RequestVideoMainAddress &req, EX_ResponsetVideoMainAddress &response, UnitID, int)
	{
		response.mMainAddress = mMainAddress;
	}

	void On(TEST_BigHttpMsg &req, TEST_ResponseBigHttp &response, UnitID, int)
	{
		NOTE_LOG("DO test msg > data : %d", req.mBigData->dataSize());
		response.mResponseData = MEM_NEW DataBuffer(1024 * 1024);
		char sz[1024];
		for (int i = 0; i < 1024; ++i)
		{
			response.mResponseData->_write(sz, 1024);
		}
	}

public:
	virtual void RegisterMsg() override
	{
		REG_COMP_MSG(ExVideoDataComponent, EX_SaveVideoData, EX_ResponseSaveVideo);
		REG_COMP_MSG(ExVideoDataComponent, EX_RequestVideoMainAddress, EX_ResponsetVideoMainAddress);
		REG_COMP_MSG(ExVideoDataComponent, TEST_BigHttpMsg, TEST_ResponseBigHttp);
	}
};
//-------------------------------------------------------------------------
// 提供视频上传保存功能
//-------------------------------------------------------------------------
class ExVideoFileActor : public Actor
{
public:
	virtual void Init() override
	{
		AddComponent("ExVideoDataComponent");
		Hand<UploadCacheComponent> comp = AddComponent("UploadCacheComponent");
		comp->SetDataMaxSize(16*1024*1024);
	}

public:
	virtual void RegisterMsg() override
	{
		// 需要把这个单独放到一个进程里, 独立于DB
		REG_COMPONENT(ExVideoDataComponent);
		REG_COMPONENT(UploadCacheComponent);
	}
};
//-------------------------------------------------------------------------