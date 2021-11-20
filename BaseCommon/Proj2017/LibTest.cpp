// LibTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "ShareMemoryPool.h"
#include "FileDataStream.h"
#include "NiceData.h"
#include "WorkThread.h"
#include "TimeManager.h"
#include "ThreadHand.h"

struct XX
{
	int x;
	float y;
};

class xxx : public ThreadBase<xxx>
{

};

class TestThread : public WorkThread
{
public:
	virtual void backWorkThread(void) override
	{
		if (mpLock != NULL)
		{
			printf("Begin lock\r\n");
			mpLock->lock();
		}
		printf("End thread\r\n");
	}

public:
	CsLock *mpLock;
};

int main()
{
	ThreadHand<xxx> mmm = MEM_NEW xxx();

	auto z = mmm;

	CsLock  *pLock = new CsLock();
	TestThread  test;

	test.mpLock = pLock;

	pLock->lock();

	test.InitThread();

	TimeManager::Sleep(2000);

	printf("delete lock\r\n");

	delete pLock;

	while (true)
	{
		TimeManager::Sleep(1);
	}

    std::cout << "Hello World!\n"; 
	ShareMemoryPool *pool = new ShareMemoryPool(false, "Test", 256);
	int x3 = BLOCK_SIZE;
	int x = 100;
	int x2 = ((uint)(x + TEMP_NUM) >>3) * BASE_BLOCK_SIZE;
	int x4 = GET_BLOCK_SIZE(x);

	new ShareMemoryPool(true, "Test", 256);

	new ShareMemoryPool(true, "Test", 256);

	SHARE_PTR p = pool->NewPtr(8);

	XX *pX = (XX*)pool->ToMemPtr(p);
	pX->x = 88;
	pX->y = 99.99f;

	{
		ShareMemoryPool *pool2 = new ShareMemoryPool(false, "Test", 256);

		SHARE_PTR p2 = p;

		XX *pX2 = (XX*)pool2->ToMemPtr(p2);
		pX2->x = 288;
		pX2->y = 299.99f;
	}

	//SHARE_PTR p = ShareMemoryPool::GetMe().NewPtr(64);
	//SHARE_PTR p2 = ShareMemoryPool::GetMe().NewPtr(32);
	//ShareMemoryPool::GetMe().FreePtr(p);
	//ShareMemoryPool::GetMe().FreePtr(p2);
	//SHARE_PTR p2 = 

	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
