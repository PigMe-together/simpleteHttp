#include "myEvent.h"

void eventset(my_events* my_ev, int fd,int epfd, void(*call_back)(int fd, int event, void* arg), void* event_arg)
{
	my_ev->epfd = epfd;
	my_ev->m_fd = fd;
	my_ev->m_event = 0; //��ʼ��֪����ע����ʲô�¼����������Ϊ0
	my_ev->m_arg = event_arg;
	my_ev->call_back = call_back;

	my_ev->m_status = 0; //0��ʾû���ں������
	my_ev->m_lasttime = time(NULL);//����eventset�����ľ���ʱ��
	my_ev->status=0;
	my_ev->descr=NULL;
	my_ev->type=NULL;
	my_ev->length=0;
	my_ev->fileName=NULL;

	return;
}

void eventadd(int ep_fd, int event, my_events* my_ev)
{
	int op;
	struct epoll_event epv;
	epv.data.ptr = my_ev;
	epv.events = my_ev->m_event = event; //EPOLLIN��EPOLLOUT

	if (my_ev->m_status == 0)
	{
		op = EPOLL_CTL_ADD;
	}
	else
	{
		//printf("\n add error: already on tree \n");
		return;
	}

	if (epoll_ctl(ep_fd, op, my_ev->m_fd, &epv) < 0) //ʵ�����/�޸�
	{
		//printf("\n event add/mod false [fd= %d] [events= %d] \n", my_ev->m_fd, my_ev->m_event);
	}
	else
	{
		my_ev->m_status = 1;
		//printf("\n event add ok [fd= %d] [events= %d] \n", my_ev->m_fd, my_ev->m_event);
	}
	return;
}

void eventdel(int ep_fd, my_events* ev)
{
	if (ev->m_status != 1)
		return;

	epoll_ctl(ep_fd, EPOLL_CTL_DEL, ev->m_fd, NULL);
	ev->m_status = 0;

	return;

}

void eventset1(my_events* my_ev, int fd, int epfd, void(*call_back)(void* arg), void* event_arg, int status, char* descr, char* type, int length, char* fileName)
{
	my_ev->epfd = epfd;
	my_ev->m_fd = fd;
	my_ev->m_event = 0; //��ʼ��֪����ע����ʲô�¼����������Ϊ0
	my_ev->m_arg = event_arg;
	my_ev->call_back = call_back;

	my_ev->m_status = 0; //0��ʾû���ں������
	my_ev->m_lasttime = time(NULL);//����eventset�����ľ���ʱ��
	my_ev->status = status;
	my_ev->descr = descr;
	my_ev->type = type;
	my_ev->length = length;
	my_ev->fileName = fileName;

	return;
}
