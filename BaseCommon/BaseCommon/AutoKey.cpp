
#include "AutoKey.h"
#include "TimeManager.h"

AutoKey::AutoKey()
	: mTimePeriod(0)
	, mCode(0)
{
	mNowSecond = TimeManager::NowTick();
}

KEY_TYPE AutoKey::AlloctKey()
{
	UInt64 milSec = TimeManager::NowTick();

	if (milSec < mNowSecond)
		++mTimePeriod;

	mNowSecond = milSec;

	++mCode;

	KEY_TYPE key = milSec;
	key = (key << 32) + (mTimePeriod << 16) + mCode;

	return key;
}
//-------------------------------------------------------------------------

void AutoKey::ReleaseKey(KEY_TYPE freeKey)
{

}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------