#pragma once

#include <stdio.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <unistd.h>
#include <assert.h>//����
#include <sys/sendfile.h>
#include <dirent.h>
#include <ctype.h>
#include "threadpool.h"
#include <pthread.h>
#include "myEvent.h"
#include <string.h>
#include <errno.h>

my_events ep_events[MAX_EVENTS];



//��ʼ���׽���
int InitCon(int epfd,unsigned short port);  //fd  epfd
//����epoll
//int epollRun(int fd,int epfd, ThreadPool* pool);					//epfd pool
//����������
void acceptClient(void* arg);				//fd  epfd
//����http����
void recvHttpRequest(void* arg);			//fd  epfd
//����������
void parseRequestLine(void* arg);
//�����ļ�
void sendFile(void* arg);		///////////
// ������Ӧͷ(״̬�к���Ӧͷ)
void sendHeadMsg(void* arg);
//����ContentType����
const char* getFileType(const char* name);
//����Ŀ¼
void sendDir(void* arg);
//����
void decodeMsg(char* to,char* from);
//charתʮ����
int HexToDec(char c);