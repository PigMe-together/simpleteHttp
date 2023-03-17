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
	//切换服务器的工作目录
	chdir(argv[2]);
	int checkpos = 0;
	int i;

	int epfd = epoll_create(1);
	int fd=InitCon(epfd, port);
	ThreadPool* pool = threadPoolCreate(6, 100, 200);
	struct epoll_event events[1024];
	while (1) {
		/*超时验证,每次测试100个连接,60s内没有和服务器通信则关闭客户端连接*/
		//long now = time(NULL); //当前时间
		//for (i = 0; i < 100; i++, checkpos++) //一次循环检测100个，使用checkpos控制检测对象
		//{
		//	if (checkpos == MAX_EVENTS - 1)
		//		checkpos = 0;
		//	if (ep_events[i].m_status != 1) //不在红黑树上
		//		continue;

		//	long spell_time = now - ep_events[i].m_lasttime; //客户端不活跃的时间
		//	if (spell_time >= 60) //如果时间超过60s
		//	{
		//		printf("[fd= %d] timeout \n", ep_events[i].m_fd);
		//		close(ep_events[i].m_fd); //关闭与客户端连接
		//		eventdel(epfd, &ep_events[i]); //将客户端从红黑树摘下
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