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
#include <future>

class ThreadPool {
public:
    ThreadPool(int threadNum = std::thread::hardware_concurrency());
    ~ThreadPool();

    // void addTask(std::function<void()>);
    template<typename F, typename... Args>
    auto addTask(F &&f, Args &&... args) -> std::future<std::result_of_t<F(Args...)> >;

private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex mutex;
    std::condition_variable cond;
    bool stop;
};

template<typename F, typename... Args>
auto ThreadPool::addTask(F &&f, Args &&... args) -> std::future<std::result_of_t<F(Args...)> > {
    // 任务的返回值类型
    using return_type = std::result_of_t<F(Args...)>;
    // 将任务封装到 std::packaged_task 中(类似于 std::function)
    auto task = std::make_shared<std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    // 获得任务的 future
    std::future<return_type> res = task->get_future();
    {
        // 在此作用域对 mutex 上锁，离开此作用域自动解锁
        std::unique_lock<std::mutex> lock(mutex);
        if (stop) {
            throw std::runtime_error("addTask on stopped ThreadPool");
        }
        // 将任务添加到任务队列
        tasks.emplace([task]() { (*task)(); });
    }
    // 唤醒一个线程
    cond.notify_one();
    return res;
}