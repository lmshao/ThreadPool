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

    ThreadPool pool(4);
    for (int i = 0; i < 20; i++) {
        std::cout << i << std::endl;
        int *arg = new int(i);
        pool.addTask(task, arg);
    }

    std::cout << "-------------" << std::endl;
}