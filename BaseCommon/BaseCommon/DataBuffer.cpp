
//#include "stdafx.h"
#include "DataBuffer.h"

#include "DataStream.h"
#include "MemBase.h"

#include "Common.h"
#if __WINDOWS__
#	include "zip/zlib.h"
#endif
#include "MemBase.h"

#define LOG_ZIP_TIME		0

#if LOG_ZIP_TIME
#	include "TimeManager.h"
#endif
//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------

DSIZE DataBuffer::ZipData( AutoData &resultBuffer, DSIZE destDataPosition, DSIZE scrDataPos /*= 0*/, DSIZE scrSize /*= 0 */, ZIP_OPTION op )
{
#if __WINDOWS__
#if LOG_ZIP_TIME
	UINT startTime = TimeManager::NowTick();
#endif

	if (scrSize<=0)
		scrSize = dataSize()-scrDataPos;

	if (scrSize>=0 && data() && size()-scrDataPos>=scrSize )
	{
		unsigned long resize = scrSize + destDataPosition;
		if (!resultBuffer)
			resultBuffer = MEM_NEW DataBuffer(resize);

		unsigned char* szResultData = ((unsigned char*)resultBuffer->data()) + destDataPosition;
		unsigned char* szScrData = ((unsigned char*)data()) + scrDataPos;

		int zOption = Z_BEST_SPEED;
		if (op==STANDARD)
			zOption = Z_DEFAULT_COMPRESSION;
		else if (op = MAX_ZIP_RATE)
			zOption = Z_BEST_COMPRESSION;
		
		while (true)
		{			
			if (resultBuffer->size()<(DSIZE)resize)
			{
				resultBuffer->resize(resize);
				szResultData = ((unsigned char*)resultBuffer->data()) + destDataPosition;
			}
			unsigned long resultZipSize = resultBuffer->size() - destDataPosition;

			int re = compress2( szResultData, &resultZipSize, szScrData, (unsigned long)scrSize,  zOption);
			if ( re==Z_OK )
			{
				resultBuffer->seek(destDataPosition+resultZipSize);
				resultBuffer->write(scrSize);
				//resultBuffer->setDataSize(destDataPosition+resultZipSize);
#if LOG_ZIP_TIME
				DEBUG_LOG(" Zip use time > [%u]", TimeManager::NowTick() - startTime);
#endif
				return resultZipSize;
			}
			else if (re==Z_BUF_ERROR)
				resize += 1028;
			else
			{
				ERROR_LOG("Zip fail %d > error : %d", scrSize, re);
				return 0;
			}
		}
	}
#else
	NOTE_LOG("Zip cannot use at linux");
#endif
	return 0;
}

bool DataBuffer::UnZipData( AutoData &resultBuffer, DSIZE resultBeginPosition, DSIZE destSize, DSIZE zipDataPos /*= 0*/, DSIZE zipSize /*= 0 */ )
{
#if __WINDOWS__
#if LOG_ZIP_TIME
	UINT startTime = TimeManager::NowTick();
#endif
	DSIZE infoSize = destSize > 0 ? 0 : sizeof(DSIZE);
	if (zipSize==0)
		zipSize = dataSize()-zipDataPos-infoSize;
	else if (zipSize > dataSize()-zipDataPos-infoSize)
		return false;

	if (destSize <= 0)
	{
		seek(zipDataPos+zipSize);
		read(destSize);
	}
	if (!resultBuffer)
		resultBuffer = MEM_NEW DataBuffer(resultBeginPosition+destSize);
	else if (resultBuffer->size()<resultBeginPosition+destSize)
		resultBuffer->resize(resultBeginPosition+destSize);

	uLongf destdatasize = (uLongf)destSize;
	int ret = uncompress((unsigned char*)resultBuffer->data() + resultBeginPosition, &destdatasize, (unsigned char*)(data() + zipDataPos), (unsigned long)zipSize);
	if (ret == Z_OK)
	{
		resultBuffer->seek(resultBeginPosition+destdatasize);
		resultBuffer->setDataSize(resultBeginPosition+destdatasize);
#if LOG_ZIP_TIME
		DEBUG_LOG(" UNZip use time > [%u]", TimeManager::NowTick() - startTime);
#endif
		return true;
	}
#else
	NOTE_LOG("Zip cannot use at linux");
#endif
	return false;
}

DSIZE DataBuffer::Zip( void *scrData, DSIZE scrSize, void* resultZipDataBuffer, DSIZE resultSize, ZIP_OPTION op)
{
#if __WINDOWS__

	int zOption = Z_BEST_SPEED;
	if (op == STANDARD)
		zOption = Z_DEFAULT_COMPRESSION;
	else if (op = MAX_ZIP_RATE)
		zOption = Z_BEST_COMPRESSION;

	uLongf resultZipSize = (uLongf)resultSize;
	int re = compress2( (unsigned char*)resultZipDataBuffer, &resultZipSize, (unsigned char*)scrData, (unsigned long)scrSize, zOption );
	if ( re==Z_OK )
		return resultZipSize;
#else
	NOTE_LOG("Zip cannot use at linux");
#endif
	return 0;
}

bool DataBuffer::UnZip( void *scrZipData, int scrZipSize, void *resultData, int resultDataSize )
{
#if __WINDOWS__
	uLongf destdatasize = (uLongf)resultDataSize;
	return ( uncompress( (unsigned char*)resultData, &destdatasize, (unsigned char*)scrZipData, (unsigned long)scrZipSize )==Z_OK );
#else
	NOTE_LOG("Zip cannot use at linux");
#endif
}

