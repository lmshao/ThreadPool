//
// Created by lmshao on 2021/4/12.
//

#include "ThreadPool.h"
#include <iostream>

ThreadPool::ThreadPool(int threadsLimit)
  : _threadNum(threadsLimit), _service{}, _serviceWorker(new boost::asio::io_service::work(_service)), _closed(false)
{
}

ThreadPool::~ThreadPool()
{
    close();
}

void ThreadPool::start()
{
    auto thisPtr = shared_from_this();
    auto worker = [thisPtr] {
        std::ostringstream oss;
        oss << "current thread: " << std::this_thread::get_id();
        std::cout << oss.str() << std::endl;

        if (!thisPtr->_closed) {
            std::cout << oss.str() << std::endl;
            return thisPtr->_service.run();
        }
        return size_t(0);
    };

    for (int i = 0; i < _threadNum; ++i) {
        _group.add_thread(new boost::thread(worker));
    }
}

void ThreadPool::close()
{
    _closed = true;
    _serviceWorker.reset();
    _group.join_all();
    _service.stop();
}

void ThreadPool::addTask(std::function<void()> task)
{
    _service.dispatch(task);
}
