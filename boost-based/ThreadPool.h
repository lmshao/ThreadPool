//
// Created by lmshao on 2021/4/12.
//

#ifndef THREADPOOL_THREADPOOL_H
#define THREADPOOL_THREADPOOL_H

#include <memory>
#include <vector>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

class ThreadPool : public std::enable_shared_from_this<ThreadPool> {
  public:
    explicit ThreadPool(int threadsLimit = 4);
    ~ThreadPool();

    void addTask(std::function<void()> task);

    void close();
    void start();

  private:
    int _threadNum;
    boost::asio::io_service _service;
    std::unique_ptr<boost::asio::io_service::work> _serviceWorker;
    boost::thread_group _group;
    std::atomic<bool> _closed;
};

#endif  // THREADPOOL_THREADPOOL_H
