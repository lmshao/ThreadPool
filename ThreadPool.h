//
// Created by Liming SHAO on 2019/12/8.
//

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#include <queue>

class ThreadPool {
  public:
    typedef void *(*function)(void *);
    struct ThreadTask {
        function task;  // function ptr
        void *arg;      // params
        ThreadTask(function task, void *arg) : task(task), arg(arg) {}
        ThreadTask() : task(nullptr), arg(nullptr){};
    };

    explicit ThreadPool(int threadsLimit = 100);
    ~ThreadPool();

    friend void *workerThread(void *arg);

    void addTask(function task, void *arg);

  private:
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
    pthread_attr_t _attr;

    std::queue<ThreadTask> _tasks;
    int _count;  // 当前工作线程个数
    int _idle;   // 当前空闲的线程个数
    int _threadsLimit;
    int _quit;  //

    bool _valid;
};

#endif  // THREAD_POOL_H
