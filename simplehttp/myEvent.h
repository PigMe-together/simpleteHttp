#pragma once
#include <stdio.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


#define MAX_EVENTS  1024
#define Max 4096

typedef struct my_Events {
    int        epfd;
    int        m_fd;                                         //�������ļ�������
    int        m_event;                                      //�������¼�  
    void*      m_arg;                                       //���Ͳ���
    void       (*call_back)(void* arg);   //�ص�����
    char       m_buf[Max];
    int        m_buf_len;
    int        m_status;                                     //�Ƿ��ں������, 1->��, 0->����
    time_t     m_lasttime;                                   //������������ʱ��
    int status;
    char* descr;
    char* type;
    int length;
    char* fileName;
}my_events;


   

/*���ṹ���Ա������ʼ��*/
void eventset(my_events* my_ev, int fd,int epfd, void (*call_back)(int fd, int event, void* arg), void* event_arg);
/*��������� �ļ��������Ͷ�Ӧ�Ľṹ��*/
void eventadd(int ep_fd, int event,my_events* my_ev);
/*�Ӻ������ɾ�� �ļ��������Ͷ�Ӧ�Ľṹ��*/
void eventdel(int ep_fd,my_events* ev);

void eventset1(my_events* my_ev, int fd, int epfd, void (*call_back)(void* arg),
    void* event_arg, int status, char* descr, char* type, int length, char* fileName);


