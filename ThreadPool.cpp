//
// Created by Liming SHAO on 2019/12/8.
//

#include "ThreadPool.h"
#include <errno.h>
#include <stdio.h>
#include <time.h>

ThreadPool::ThreadPool(int threadsLimit) : _valid(true), _count(0), _idle(0), _quit(0), _threadsLimit(threadsLimit)
{
    _valid = true;

    if (pthread_mutex_init(&_mutex, nullptr) != 0) {
        perror("pthread_mutex_init");
        _valid = false;
    }

    if (pthread_cond_init(&_cond, nullptr) != 0) {
        perror("pthread_cond_init");
        _valid = false;
    }

    if (pthread_attr_init(&_attr) != 0) {
        perror("pthread_attr_init");
        _valid = false;
    }

    if (pthread_attr_setdetachstate(&_attr, PTHREAD_CREATE_DETACHED) != 0) {
        perror("pthread_attr_setdetachstate");
        _valid = false;
    }
}

ThreadPool::~ThreadPool()
{
    if (_quit)
        return;

    pthread_mutex_lock(&_mutex);
    _quit = 1;

    if (_count > 0) {
        if (_idle > 0) {
            printf("idle[%d] count[%d]\n", _idle, _count);

            // Wake up all threads waiting for condition variables COND.
            pthread_cond_broadcast(&_cond);
        }

        while (_count) {
            printf("count[%d] idle[%d]\n", _count, _idle);

            // Wait for condition variable COND to be signaled or broadcast.
            // MUTEX is assumed to be locked before.
            pthread_cond_wait(&_cond, &_mutex);
        }
    }

    pthread_mutex_unlock(&_mutex);

    if ((pthread_mutex_destroy(&_mutex)) != 0) {
        perror("pthread_mutex_destroy");
    }

    if ((pthread_cond_destroy(&_cond)) != 0) {
        perror("pthread_cond_destroy");
    }

    if ((pthread_attr_destroy(&_attr)) != 0) {
        perror("pthread_cond_destroy");
    }
}

void *workerThread(void *arg)
{
    printf("[%lu] --> Start\n", pthread_self());

    struct timespec abstime;
    int timeout = 0;

    ThreadPool *pool = (ThreadPool *)arg;
    for (;;) {
        pthread_mutex_lock(&pool->_mutex);

        pool->_idle++;

        //等待队列有任务到来 或者 收到线程池销毁通知
        while (pool->_tasks.empty() && !pool->_quit) {
            printf("[%lu] --> Waiting\n", pthread_self());

            clock_gettime(CLOCK_REALTIME, &abstime);
            abstime.tv_sec += 2;

            if (pthread_cond_timedwait(&pool->_cond, &pool->_mutex, &abstime) == ETIMEDOUT) {
                printf("[%lu] --> Wait timeout\n", pthread_self());
                timeout = 1;
                break;
            }
        }

        pool->_idle--;
        if (!pool->_tasks.empty()) {
            printf("[%lu] --> Running a task\n", pthread_self());

            ThreadPool::ThreadTask task = pool->_tasks.front();
            pool->_tasks.pop();
            // 解锁让其他线程运行
            pthread_mutex_unlock(&pool->_mutex);

            // 取出一个任务运行
            task.task(task.arg);

            // 任务结束，新加锁
            pthread_mutex_lock(&pool->_mutex);
        }

        if (pool->_quit && pool->_tasks.empty()) {
            printf("[%lu] --> _quit && _tasks.empty()\n", pthread_self());
            pool->_count--;

            //若线程池中没有线程，通知等待线程（主线程）全部任务已经完成
            if (pool->_count == 0) {
                pthread_cond_signal(&pool->_cond);
            }
            pthread_mutex_unlock(&pool->_mutex);
            break;
        }

        if (timeout == 1) {
            pool->_count--;  //当前工作的线程数-1
            pthread_mutex_unlock(&pool->_mutex);
            break;
        }

        pthread_mutex_unlock(&pool->_mutex);
    }

    printf("[%lu] --> Exit\n", pthread_self());
    return nullptr;
}

void ThreadPool::addTask(function task, void *arg)
{
    if (!_valid || task == nullptr)
        return;

    pthread_mutex_lock(&_mutex);

    ThreadTask t(task, arg);
    _tasks.emplace(t);

    printf("addTask tasks[%d] count[%d] idle[%d]\n", (int)_tasks.size(), _count, _idle);

    if (_idle > 0) {
        // Wake up one thread waiting for condition variable COND.
        pthread_cond_signal(&_cond);
    } else if (_count < _threadsLimit) {
        pthread_t tid;
        pthread_create(&tid, &_attr, workerThread, this);
        _count++;
    }

    pthread_mutex_unlock(&_mutex);
}
