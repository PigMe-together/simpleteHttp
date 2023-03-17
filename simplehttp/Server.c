#pragma once
#include "Server.h"
#include "threadpool.h"
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

void acceptClient(void* arg)
{
	printf("put in acceptClient.................................................................\n");
	my_events* info = (my_events*)arg;
	int cfd;
		cfd = accept(info->m_fd, NULL, NULL);
		if (cfd == -1)
		{
			perror("accept");
			return;
		}
		//printf("Xin accept ip=%s... port=%ln......\n", inet_ntop(AF_INET, &saddr.sin_addr.s_addr, ip, sizeof(ip)),
		//	ntohs(saddr.sin_port));
			//非阻塞
		int flag = fcntl(cfd, F_GETFL);
		flag |= O_NONBLOCK;
		fcntl(cfd, F_SETFL, flag);

		int i;
		for (i = 0; i < MAX_EVENTS; ++i)
		{
			if (ep_events[i].m_status == 0)
				break;
		}
		if (i >= MAX_EVENTS)
		{
			perror("ep_events[i].m_status");
		}
		eventset(&ep_events[i], cfd, info->epfd, recvHttpRequest, &ep_events[i]);
		eventadd(info->epfd, EPOLLIN | EPOLLET, &ep_events[i]);
}



const char* getFileType(const char* name)
{
	// a.jpg a.mp4 a.html
	//自由向左查找'.'字符，如不存在返回NULL
	const char* n = strrchr(name,'.');
	if (n == NULL)
		return "text/plain;charset=utf-8";
	if (strcasecmp(n, ".html") == 0 || strcasecmp(n, "htm") == 0)
		return "text/html;charset=utf-8";
	if(strcmp(n,".jpg")==0 || strcmp(n, ".jpeg") == 0)
		return "image/jpeg";
	if (strcmp(n, ".gif") == 0)
		return "image/gif";
	if (strcmp(n, ".png") == 0)
		return "image/png";
	if (strcmp(n, ".css") == 0)
		return "text/css";
	if (strcmp(n, ".au") == 0)
		return "audio/basic";
	if (strcmp(n, ".avi") == 0)
		return "video/x-msvideo";
	if (strcmp(n, ".mov") == 0 || strcmp(n,".qt")==0)
		return "video/quicktime";
	if (strcmp(n, ".bmp") == 0)
		return "application/x-bmp";
	if (strcmp(n, ".mpeg") == 0 || strcmp(n, ".mpv") == 0)
		return "video/mpg";
	if (strcmp(n, ".vrml") == 0 || strcmp(n, ".wrl") == 0)
		return "model/vrml";
	if (strcmp(n, ".mp3") == 0)
		return "audio/mpeg";
	if (strcmp(n, ".mp4") == 0)
		return "video/mpeg4";
	if (strcmp(n, ".ogg") == 0)
		return "application/ogg";
	if (strcmp(n, ".jfif") == 0)
		return "image/jpeg";
	if (strcmp(n, ".ico") == 0)
		return "image/x-icon";
	if (strcmp(n, ".pac") == 0)
		return "application/x-ns-proxy-autoconfig";
	return "text/plain;charset=utf-8";
}
/*
<html>
<head>
	<meta charset="utf-8">
	<title></title>
</head>
<body>
	<table border="1">
		<tr>
			<td>100</td>
		</tr>
	</table>
</body>
</html>
	*/
void sendDir(void* arg)
{
	printf("1\n");
	my_events* info = (my_events*)arg;
	eventdel(info->epfd, info);
	char buf[1024] = { 0 };
	sprintf(buf,"<html><head><meta http-equiv='Content-Type' content='text/html;charset=utf-8'/><title>%s</title></head><body><table>",info->fileName);
	
	struct dirent** namelist;
	int num = scandir(info->fileName, &namelist, NULL, alphasort);
	printf("2\n");
	for (int i = 0; i < num; i++)
	{
		//去除文件名 namelist指向的是一个指针数组 struct dirent* tmp[]
		char* name = namelist[i]->d_name;
		struct stat st;
		char subPath[1024] = {0};
		sprintf(subPath,"%s/%s", info->fileName,name);
		stat(subPath, &st);
		if (S_ISDIR(st.st_mode))
		{
			// a标签 <a href="">name</a>
			sprintf(buf+strlen(buf),"<tr><td><a href=\"%s/\">%s</a></td><td>%ld</td></tr>",
				name,name,st.st_size);
		}
		else
		{
			sprintf(buf + strlen(buf), "<tr><td><a href=\"%s\">%s</a></td><td>%ld</td></tr>",
				name, name, st.st_size);
		}
		send(info->m_fd, buf, strlen(buf), 0);
		memset(buf,0,sizeof(buf));
		free(namelist[i]);
	}
	printf("3\n");
	memset(buf,0,sizeof(buf));
	sprintf(buf, "</table></body></html>");
	send(info->m_fd, buf, strlen(buf),0);
	free(namelist);
	printf("fa song Dir success!....\n");
	eventset(info, info->m_fd, info->epfd, recvHttpRequest, info);
	eventadd(info->epfd, EPOLLIN | EPOLLET, info);
	return;
}

void decodeMsg(char* to, char* from)
{
	for (; *from != '\0'; ++to, ++from)
	{
		// Linux%E5%96.jpg
		if (from[0] == '%' && isxdigit(from[1]) && isxdigit(from[2]))
		{
			*to = HexToDec(from[1]) * 16 + HexToDec(from[2]);
			from += 2;
		}
		else
		{
			*to = *from;
		}
	}
}

int HexToDec(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a'+10;
	if (c >= 'A' && c <= 'F')
		return c - 'A'+10;

	return 0;
}



void parseRequestLine(void* arg)
{
	my_events* info = (my_events*)arg;
	//eventdel(info->epfd, info);
	//解析请求行 /
	char method[12];
	char path[1024];
	sscanf(info->m_buf,"%[^ ] %[^ ]",method,path);
	//memset(info->m_buf,0,sizeof(info->m_buf));
	
	printf("method=%s,.............................path=%s...................\n", method, path);
	if (strcasecmp(method, "get") != 0)
	{
		printf("this is not get\n");
		return;
	}
	printf("chu li kehuduan Request\n");
	decodeMsg(path, path);
	//处理客户端请求的静态资源（目录或文件）
	char* file = NULL;
	if (strcmp(path, "/") == 0)
	{
		file = "./";
	}else
	{
		file = path + 1;
	}
	//strcpy(info->m_buf, "");
	//获取文件属性
	struct stat st;
	int ret = stat(file,&st);
	if (ret == -1)
	{
		printf("1\n");
		//文件不存在--回复404

		/*sendHeadMsg(info->m_fd,info->epfd,404,"Not Found",getFileType(".html"),-1);
		sendFile("404.html", info->m_fd, info->epfd);*/
		eventset1(info, info->m_fd, info->epfd, sendHeadMsg, info, 404, "Not Found", 
			getFileType(".html"),
			-1, "404.html");
		//eventadd(info->epfd, EPOLLOUT | EPOLLET, info);
		sendHeadMsg(info);


		/*eventset(info, info->m_fd, info->epfd, recvHttpRequest, info);
		eventadd(info->epfd, EPOLLIN | EPOLLET, info);*/
		
	}else
	//判断文件类型
	if (S_ISDIR(st.st_mode))		//返回1为目录，返回0为文件
	{
		printf("2\n");
		//将这个目录中的内容发送给客户端
		
		eventset1(info, info->m_fd, info->epfd, sendHeadMsg, info, 200, "OK", getFileType(file),
			-1, file);
		//eventadd(info->epfd, EPOLLOUT | EPOLLET, info);
		sendHeadMsg(info);
		
		/*sendHeadMsg(info->m_fd, info->epfd, 200, "OK", getFileType(file), -1);
		sendDir(file, info->m_fd, info->epfd);*/
	}
	else
	{
		printf("3\n");
		//将文件的内容发送给客户端
		
		eventset1(info, info->m_fd, info->epfd, sendHeadMsg, info,200,"OK",getFileType(file),
			st.st_size,file);
		//eventadd(info->epfd, EPOLLOUT | EPOLLET, info);
		sendHeadMsg(info);
		
		/*sendHeadMsg(info->m_fd, info->epfd, 200, "OK", getFileType(file), st.st_size);
		printf("fa song HeadMsg wan bi!...\n");
		sendFile(file, info->m_fd, info->epfd);
		printf("Fa song wan bi!.......\n");*/
	}
}

void sendFile(void* arg)
{
	printf("fa song file\n");
	my_events* info = (my_events*)arg;
	//打开文件
	int fd = open(info->fileName,O_RDONLY);
	eventdel(info->epfd,info);
	if (fd < 0)
	{
		printf("open failed\n");
		return;
	}
	off_t offset = 0;
	off_t size = lseek(fd,0,SEEK_END);//文件大小 偏移量
	lseek(fd,0,SEEK_SET);
	int ret;
	printf("before while\n");
	while(offset<size)
	{
		ret = sendfile(info->m_fd, fd, &offset, size - offset);
		if (ret==-1 && errno == EAGAIN)
		{
			//printf("No shuju...\n");
			printf("in while errno is %d\n",errno);      //  11
			continue;
		}
	}
	printf("after while\n");
	close(fd);
	printf("fa song File success!...\n");
	eventset(info, info->m_fd, info->epfd, recvHttpRequest, info);
	eventadd(info->epfd, EPOLLIN | EPOLLET, info);
	return;
}

void sendHeadMsg(void* arg)
{
	my_events* info = (my_events*)arg;
	//状态行
	eventdel(info->epfd,info);
	char buf[4096] = { 0 };
	sprintf(buf,"http/1.1 %d %s\r\n",info->status,info->descr);

	//响应头
	sprintf(buf+strlen(buf),"Content_Type: %s\r\n",info->type);
	sprintf(buf + strlen(buf), "Content_Length: %d\r\n", info->length);
	sprintf(buf + strlen(buf), "Accept: application/json;charset = UTF-8\r\n\r\n");

	
	struct stat st;
	int ret = stat(info->fileName, &st);
	if (ret == -1)
	{
		printf("fa song HeadMsg success!.........\n");
		send(info->m_fd, buf, strlen(buf), 0);
		
		sendFile(info);
	}else if (S_ISDIR(st.st_mode))
	{
				//让内核不产生信号

		printf("fa song HeadMsg success!.........\n");
		send(info->m_fd, buf, strlen(buf), 0);
		printf("fa song dir\n");
		sendDir(info);
	}
	else
	{
		
		printf("fa song HeadMsg success!.........\n");
		send(info->m_fd, buf, strlen(buf), 0);		//让内核不产生信号
		sendFile(info);
	}
	//return;
}


int InitCon(int epfd,unsigned short port)
{
		//printf("Chu shi hua socket\n");
		int fd = socket(AF_INET, SOCK_STREAM, 0);
		if (fd == -1)
		{
			perror("socket!");
			return -1;
		}
		int opt = 1;
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

		int flag = fcntl(fd, F_GETFL);
		flag |= O_NONBLOCK;
		fcntl(fd, F_SETFL, flag);

		struct sockaddr_in saddr;
		saddr.sin_family = AF_INET;
		saddr.sin_port = htons(port);
		saddr.sin_addr.s_addr = INADDR_ANY;
		int ret = bind(fd, (struct sockaddr*)&saddr, sizeof(saddr));
		if (ret == -1)
		{
			perror("bind");
			return -1;
		}
		ret = listen(fd, 128);
		if (ret == -1)
		{
			perror("listen");
			return -1;
		}
		
		eventset(&ep_events[MAX_EVENTS - 1], fd,epfd, acceptClient, &ep_events[MAX_EVENTS-1]);
		eventadd(epfd, EPOLLIN|EPOLLET, &ep_events[MAX_EVENTS - 1]);

		return fd;
}
//int epollRun(int fd,int epfd,ThreadPool* pool)
//{
//		struct epoll_event events[1024];
//		while (1) {
//			printf("Get socket from epoll\n");
//			int num = epoll_wait(epfd, &events, 1024, -1);
//			for (int i = 0; i < num; ++i)
//			{
//				my_events* p = (my_events*)(events[i].data.ptr);
//				if (events[i].events & EPOLLIN)
//					printf("epoll_wait EPOLLIN\n");
//					threadPoolAdd(pool, p->call_back, p);
//					//p->call_back(p->m_arg);
//				if (events[i].events & EPOLLOUT && p->m_event & EPOLLOUT)
//					threadPoolAdd(pool, p->call_back, p);
//					printf("epoll_wait EPOLLOUT\n");
//					//p->call_back( p->m_arg);
//			}
//		}
//}
void recvHttpRequest(void* arg)
{
	my_events* info = (my_events*)arg;
	bzero(info->m_buf,sizeof(info->m_buf));
	printf("Begin recv HttpRequst.........................................................\n");
	int len = 0;
	int total = 0;
	char tmp[1024] = { 0 };
	eventdel(info->epfd,info);
	do {
		while ((len = recv(info->m_fd, tmp, sizeof tmp, 0)) > 0)
		{
			if (total + len < sizeof info->m_buf)
			{
				memcpy(info->m_buf, tmp, len);
			}
			printf("%s\n", info->m_buf);
			total += len;
		}
		//printf("len=%d...\n", len);
		if (len == -1 && errno == EAGAIN) //这几种错误码， 认为连接是正常的，继续接收 //EAGAIN：没有数据但是可以继续尝试
		{
			char* pt = strstr(info->m_buf, "\r\n");
			int reqLen = pt - info->m_buf;
			info->m_buf[reqLen] = '\0';
			//eventset(info, info->m_fd, info->epfd, parseRequestLine, info);
			//eventadd(info->epfd, EPOLLOUT | EPOLLET, info);
			parseRequestLine(info); 
		/*}
		else if(len==-1 && errno == EINTR)
		{
			return;*/
		}else if (len == 0)
		{
			close(info->m_fd);
			printf("client was unconnected....errno is %d\n",errno);//2 11 17
			//break;
		}
		else
		{
			perror("recv");
			break;
		}
	} while (0);
	return;
}
