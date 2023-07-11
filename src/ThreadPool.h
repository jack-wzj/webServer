/**
 * ThreadPool.h
 * @brief 线程池类
 * @details 线程池类，用于管理工作线程
 */
#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadPool {
public:
    ThreadPool(int threadNum = std::thread::hardware_concurrency());
    ~ThreadPool();

    void addTask(std::function<void()>);

private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex mutex;
    std::condition_variable cond;
    bool stop;
};