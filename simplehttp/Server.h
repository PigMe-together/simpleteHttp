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
#include <assert.h>//断言
#include <sys/sendfile.h>
#include <dirent.h>
#include <ctype.h>
#include "threadpool.h"
#include <pthread.h>
#include "myEvent.h"
#include <string.h>
#include <errno.h>

my_events ep_events[MAX_EVENTS];



//初始化套接字
int InitCon(int epfd,unsigned short port);  //fd  epfd
//启动epoll
//int epollRun(int fd,int epfd, ThreadPool* pool);					//epfd pool
//建立新连接
void acceptClient(void* arg);				//fd  epfd
//接收http请求
void recvHttpRequest(void* arg);			//fd  epfd
//解析请求行
void parseRequestLine(void* arg);
//发送文件
void sendFile(void* arg);		///////////
// 发送响应头(状态行和响应头)
void sendHeadMsg(void* arg);
//返回ContentType属性
const char* getFileType(const char* name);
//发送目录
void sendDir(void* arg);
//解码
void decodeMsg(char* to,char* from);
//char转十进制
int HexToDec(char c);