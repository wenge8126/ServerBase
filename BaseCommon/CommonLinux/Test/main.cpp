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
//	// ��������
//	stCoCond_t* cond;
//	// ���ݳ�
//	std::vector<int> vecData;
//	// ����ID
//	int id;
//	// Э��id
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
//	// Э�̶���(CCB)��һ�������ߣ����������
//	const int nConsumer = 1;
//	stCoRoutine_t* pProducerCo = NULL;
//	stCoRoutine_t* pConsumerCo[nConsumer] = { NULL };
//
//	// ��������������Э��
//	// ��Դ���֪�ú����ط���0
//	co_create(&pProducerCo, NULL, Producer, &p);
//	p.mpPro = pProducerCo;
//	//co_resume(pProducerCo);
//	std::cout << "start producer coroutine success" << std::endl;
//
//	// ��������������Э��
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
//	// ����ѭ���¼�
//	co_eventloop(co_get_epoll_ct(), CheckFinish, &p);
//	std::cout << "end stop" << std::endl;
//	usleep(5000000);
//	return 0;
//}
//
//
//void* Producer(void* arg)
//{
//	// ����Э��HOOK��
//	co_enable_hook_sys();
//
//	stPara_t* p = (stPara_t*)arg;
//	int cid = ++p->cid;
//	while (true)
//	{
//		
//		pthread_t x = pthread_self();
//
//		// �������������
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
//		// ֪ͨ������
//		co_cond_signal(p->cond);
//		// ����ʹ��poll�ȴ�
//		poll(NULL, 0, 1000);
//	}
//	return NULL;
//}
//void* Consumer(void* arg)
//{
//	// ����Э��HOOK��
//	co_enable_hook_sys();
//
//	stPara_t* p = (stPara_t*)arg;
//	int cid = ++p->cid;
//	while (true)
//	{
//		// ������ݳأ���������ȴ�֪ͨ
//		if (p->vecData.empty())
//		{
//			p->mbConWait = true;
//			//co_yield(p->mpCon);
//			co_cond_timedwait(p->cond, -1);
//			continue;
//		}
//		pthread_t x = pthread_self();
//
//		// ��������
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
