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
    int        m_fd;                                         //监听的文件描述符
    int        m_event;                                      //监听的事件  
    void*      m_arg;                                       //泛型参数
    void       (*call_back)(void* arg);   //回调函数
    char       m_buf[Max];
    int        m_buf_len;
    int        m_status;                                     //是否在红黑树上, 1->在, 0->不在
    time_t     m_lasttime;                                   //最后放入红黑树的时间
    int status;
    char* descr;
    char* type;
    int length;
    char* fileName;
}my_events;


   

/*将结构体成员变量初始化*/
void eventset(my_events* my_ev, int fd,int epfd, void (*call_back)(int fd, int event, void* arg), void* event_arg);
/*向红黑树添加 文件描述符和对应的结构体*/
void eventadd(int ep_fd, int event,my_events* my_ev);
/*从红黑树上删除 文件描述符和对应的结构体*/
void eventdel(int ep_fd,my_events* ev);

void eventset1(my_events* my_ev, int fd, int epfd, void (*call_back)(void* arg),
    void* event_arg, int status, char* descr, char* type, int length, char* fileName);


