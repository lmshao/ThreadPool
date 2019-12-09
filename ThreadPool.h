//
// Created by Liming SHAO on 2019/12/8.
//

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#include <queue>

struct ThreadTask {
    void *(*task)(void *);  // function ptr
    void *arg;              // params
    ThreadTask(void *(*task)(void *), void *arg) : task(task), arg(arg) {}
    ThreadTask() : task(nullptr), arg(nullptr){};
};

class ThreadPool
{

  public:
    ThreadPool(int threadsLimit = 100);
    ~ThreadPool();

    friend void *workerThread(void *arg);

    void addTask(void *(*task)(void *), void *arg);

  private:
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
    pthread_attr_t _attr;

    std::queue<ThreadTask> _tasks;
    int _count;
    int _idle;
    int _threadsLimit;
    int _quit;

    bool _valid;
    bool _running;
};

#endif  // THREAD_POOL_H
