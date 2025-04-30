#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>

class ThreadPool {
    public:
        //no implicit contructors
        explicit ThreadPool(size_t threadCount = std::thread::hardware_concurrency());
        ~ThreadPool();

        template<typename Func, typename... Args>
        auto Enqueue(Func&& func, Args&&... args) -> std::future<decltype(func(args...))>;

        //decltype is implying type from the function args

    private:
        std::vector<std::jthread> workers;
        std::queue<std::function<void()>> tasks;

        std::mutex queueMutex;
        std::condition_variable condition;
        std::atomic<bool> stop = false;

        void WorkerThread();
};