#include "ThreadPool.h"

/**
 * @brief 构造函数
 * @details 初始化线程池
 * @param threadNum 线程数
 */
ThreadPool::ThreadPool(int threadNum) : stop(false) {
    for (int i = 0; i < threadNum; ++i) {
        threads.emplace_back([this]() {
            while (true) {
                std::function<void()> task;
                {
                    // 在此作用域对 mutex 上锁，离开此作用域自动解锁
                    std::unique_lock<std::mutex> lock(mutex);
                    // 等待条件变量，直到有任务或者线程池停止
                    cond.wait(lock, [this]() {
                        return stop || !tasks.empty();
                    });
                    // 如果线程池停止，且任务队列为空，则线程退出
                    if (stop && tasks.empty()) {
                        return;
                    }
                    task = std::move(tasks.front());
                    tasks.pop();
                }
                task();
            }
        });
    }
}

/**
 * @brief 析构函数
 * @details 停止线程池
 */
ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(mutex);
        stop = true;
    }
    // 唤醒所有线程
    cond.notify_all();
    // 等待所有线程执行完毕
    for (auto &thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

/**
 * @brief 添加任务
 * @details 添加任务到任务队列
 * @param task 任务
 */
void ThreadPool::addTask(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(mutex);
        if (stop) {
            throw std::runtime_error("线程池已停止，无法添加任务!");
        }
        tasks.emplace(task);
    }
    cond.notify_one();
}