#include "ThreadPool.h"
#include <unistd.h>
#include <iostream>

void *task(void *arg)
{
    int *param = (int *)arg;
    printf("[%lu] --> Working on task %d\n", pthread_self(), *param);
    sleep(2);
    delete param;
    return nullptr;
}

int main()
{
    std::cout << "-------------" << std::endl;

    std::shared_ptr<ThreadPool> pool(new ThreadPool(4));
    pool->start();
    for (int i = 0; i < 10; i++) {
        std::cout << i << std::endl;
        int *arg = new int(i);
        pool->addTask([arg] { task(arg); });
    }

    sleep(40);
    std::cout << "-------------" << std::endl;
}