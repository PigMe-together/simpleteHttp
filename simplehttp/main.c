#pragma once
#include "Server.h"
#include <stdio.h>
#include <unistd.h>
#include "myEvent.h"



int main(int argc,char* argv[])
{
	if (argc < 3)
	{
		printf("./a.out port path\n");
		return -1;
	}
	unsigned short port = atoi(argv[1]);
	//�л��������Ĺ���Ŀ¼
	chdir(argv[2]);
	int checkpos = 0;
	int i;

	int epfd = epoll_create(1);
	int fd=InitCon(epfd, port);
	ThreadPool* pool = threadPoolCreate(6, 100, 200);
	struct epoll_event events[1024];
	while (1) {
		/*��ʱ��֤,ÿ�β���100������,60s��û�кͷ�����ͨ����رտͻ�������*/
		//long now = time(NULL); //��ǰʱ��
		//for (i = 0; i < 100; i++, checkpos++) //һ��ѭ�����100����ʹ��checkpos���Ƽ�����
		//{
		//	if (checkpos == MAX_EVENTS - 1)
		//		checkpos = 0;
		//	if (ep_events[i].m_status != 1) //���ں������
		//		continue;

		//	long spell_time = now - ep_events[i].m_lasttime; //�ͻ��˲���Ծ��ʱ��
		//	if (spell_time >= 60) //���ʱ�䳬��60s
		//	{
		//		printf("[fd= %d] timeout \n", ep_events[i].m_fd);
		//		close(ep_events[i].m_fd); //�ر���ͻ�������
		//		eventdel(epfd, &ep_events[i]); //���ͻ��˴Ӻ����ժ��
		//	}
		//}

		int num = epoll_wait(epfd, &events, 1024, -1);
		for (i = 0; i < num; ++i)
		{
			//printf("For xun huan.....................................\n");
			my_events* p = (my_events*)(events[i].data.ptr);
			if ((events[i].events & EPOLLIN) && (p->m_event & EPOLLIN))
			{
				printf("p->callback succeed !-----EPOLLIN\n");
				threadPoolAdd(pool, p->call_back, p);
				//printf("epoll_wait EPOLLIN\n");
			}

			//p->call_back(p->m_arg);
			if ((events[i].events & EPOLLOUT) && (p->m_event & EPOLLOUT))
			{
				printf("p->callback succeed !-----EPOLLOUT\n");
				threadPoolAdd(pool, p->call_back, p);
				//printf("epoll_wait EPOLLOUT\n");
				//p->call_back(p->m_arg);
			}
			//p->call_back( p->m_arg);
		}
	}

	printf("chucuola      \n");
	threadPoolDestroy(pool);
	return 0;
}