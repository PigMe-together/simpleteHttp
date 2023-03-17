#pragma once
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "myEvent.h"
#include "Server.h"

// ����ṹ��
typedef struct Tasks
{
    void (*function)(void* arg);
    void* arg;
}Task;
// �̳߳ؽṹ��
typedef struct ThreadPools
{
    // �������
    Task* taskQ;
    int queueCapacity;  // ����
    int queueSize;      // ��ǰ�������
    int queueFront;     // ��ͷ -> ȡ����
    int queueRear;      // ��β -> ������

    pthread_t managerID;    // �������߳�ID
    pthread_t* threadIDs;   // �������߳�ID
    int minNum;             // ��С�߳�����
    int maxNum;             // ����߳�����
    int busyNum;            // æ���̵߳ĸ���
    int liveNum;            // �����̵߳ĸ���
    int exitNum;            // Ҫ���ٵ��̸߳���
    pthread_mutex_t mutexPool;  // ���������̳߳�
    pthread_mutex_t mutexBusy;  // ��busyNum����
    pthread_cond_t notFull;     // ��������ǲ�������
    pthread_cond_t notEmpty;    // ��������ǲ��ǿ���

    int shutdown;           // �ǲ���Ҫ�����̳߳�, ����Ϊ1, ������Ϊ0
}ThreadPool;
// �����̳߳ز���ʼ��

ThreadPool* threadPoolCreate(int min, int max, int queueSize);

// �����̳߳�
int threadPoolDestroy(ThreadPool* pool);

// ���̳߳��������
void threadPoolAdd(ThreadPool* pool, void(*func)(void*), void* arg);

// ��ȡ�̳߳��й������̵߳ĸ���
int threadPoolBusyNum(ThreadPool* pool);

// ��ȡ�̳߳��л��ŵ��̵߳ĸ���
int threadPoolAliveNum(ThreadPool* pool);

// �������߳�(�������߳�)������
void* worker(void* arg);
// �������߳�������
void* manager(void* arg);
// �����߳��˳�
void threadExit(ThreadPool* pool);
