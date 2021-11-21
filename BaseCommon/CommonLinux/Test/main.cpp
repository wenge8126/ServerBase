#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <pthread.h>
#include <unistd.h>

#include "LibcoAsync.h"
#include "MemoryDBNode.h"

class MemDB : public AsyncMemoryDB
{

};

int main()
{
	Hand<AsyncMemoryDB> db = MEM_NEW MemDB();

	std::cout << "start" << std::endl;
	AWaiter::AWaitTime(4);

	std::cout << "end" << std::endl;
	
	int b = 2;
}


//#include "co_routine.h"
//
//void* Producer(void* arg);
//void* Consumer(void* arg);
//
//struct stPara_t
//{
//	// 条件变量
//	stCoCond_t* cond;
//	// 数据池
//	std::vector<int> vecData;
//	// 数据ID
//	int id;
//	// 协程id
//	int cid;
//	stCoRoutine_t *mpPro;
//	stCoRoutine_t *mpCon;
//	bool mbProWait;
//	bool mbConWait;
//};
//
//int CheckFinish(void *p)
//{
//	auto x = (stPara_t*)p;
//	if (x->id > 10)
//		return -1;
//	return 0;
//}
//
//int main()
//{
//	stPara_t p;
//	p.cond = co_cond_alloc();
//	p.cid = p.id = 0;
//	p.mpCon = p.mpPro = NULL;
//	p.mbConWait = p.mbConWait = false;
//
//	srand(time(NULL));
//	// 协程对象(CCB)，一个生产者，多个消费者
//	const int nConsumer = 1;
//	stCoRoutine_t* pProducerCo = NULL;
//	stCoRoutine_t* pConsumerCo[nConsumer] = { NULL };
//
//	// 创建启动生产者协程
//	// 看源码可知该函数必返回0
//	co_create(&pProducerCo, NULL, Producer, &p);
//	p.mpPro = pProducerCo;
//	//co_resume(pProducerCo);
//	std::cout << "start producer coroutine success" << std::endl;
//
//	// 创建启动消费者协程
//	for (int i = 0; i < nConsumer; i++)
//	{
//		co_create(&pConsumerCo[i], NULL, Consumer, &p);
//		p.mpCon = pConsumerCo[i];
//		co_resume(pProducerCo);
//
//		co_resume(pConsumerCo[i]);
//	}
//	std::cout << "start consumer coroutine success" << std::endl;
//
//	// 启动循环事件
//	co_eventloop(co_get_epoll_ct(), CheckFinish, &p);
//	std::cout << "end stop" << std::endl;
//	usleep(5000000);
//	return 0;
//}
//
//
//void* Producer(void* arg)
//{
//	// 启用协程HOOK项
//	co_enable_hook_sys();
//
//	stPara_t* p = (stPara_t*)arg;
//	int cid = ++p->cid;
//	while (true)
//	{
//		
//		pthread_t x = pthread_self();
//
//		// 产生随机个数据
//		for (int i = rand() % 5 + 1; i > 0; i--)
//		{
//			p->vecData.push_back(++p->id);
//			std::cout << "[" << cid << "] + add new data:" << p->id << std::endl;
//		}
//		if (p->mpCon != NULL )
//		{
//			p->mbConWait = false;
//			//co_resume(p->mpCon);
//		}
//		p->mbProWait = true;
//		//co_yield(p->mpPro);
//		// 通知消费者
//		co_cond_signal(p->cond);
//		// 必须使用poll等待
//		poll(NULL, 0, 1000);
//	}
//	return NULL;
//}
//void* Consumer(void* arg)
//{
//	// 启用协程HOOK项
//	co_enable_hook_sys();
//
//	stPara_t* p = (stPara_t*)arg;
//	int cid = ++p->cid;
//	while (true)
//	{
//		// 检查数据池，无数据则等待通知
//		if (p->vecData.empty())
//		{
//			p->mbConWait = true;
//			//co_yield(p->mpCon);
//			co_cond_timedwait(p->cond, -1);
//			continue;
//		}
//		pthread_t x = pthread_self();
//
//		// 消费数据
//		std::cout << "[" << cid << "] - del data:" << p->vecData.front() << std::endl;
//		p->vecData.erase(p->vecData.begin());
//		if (p->mpPro != NULL )
//		{
//			p->mbProWait = false;
//			//co_resume(p->mpPro);
//		}
//	}
//	return NULL;
//}
