#include "Threadpool.hpp"

//quick bit on unique lock and the thread pool
//when we reach this line, we are attemping to lock the queuemutex
// std::unique_lock lock(queueMutex);
//if we don't lock it, we will wait here until the OS chooses us
// for the thread that DID get the locK:
//that thread will check the conditional variable for if the threadpool got told to stop or for tasks to be empty
//only then will be take the task in the queue and actually do it
//then we will leave the scope, which wil destroy our lock:
//   {
//     std::unique_lock lock(queueMutex);
//     condition.wait(lock, [this] { return stop || !tasks.empty(); });

//     if (stop && tasks.empty())
//         return;

//     task = std::move(tasks.front());
//     tasks.pop();
// }   <------ when we leave this we will destroy our hold on the lock and another of our worker threads will get it

//The workerthread will be blocked until it's received
//the OS will give this mutex to one of these threads try to get the lock, you can't control it
//Rinse and repeat

ThreadPool::ThreadPool(size_t threadCount) {
    for (size_t i = 0; i < threadCount; ++i) {
        workers.emplace_back([this] { WorkerThread(); });
    }
}

ThreadPool::~ThreadPool() {
    stop = true;
    condition.notify_all();  // Wake up all threads
}

void ThreadPool::WorkerThread() {

    while (!stop) {
        std::function<void()> task;

        {
            std::unique_lock lock(queueMutex);
            condition.wait(lock, [this] { return stop || !tasks.empty(); });

            if (stop && tasks.empty())
                return;

            task = std::move(tasks.front());
            tasks.pop();
        }

        task();
    }
}

template<typename Func, typename... Args>
auto ThreadPool::Enqueue(Func&& func, Args&&... args) -> std::future<decltype(func(args...))> {
    using ReturnType = decltype(func(args...));

    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
        std::bind(std::forward<Func>(func), std::forward<Args>(args)...)
    );

    std::future<ReturnType> result = task->get_future();

    {
        std::unique_lock lock(queueMutex);
        tasks.emplace([task]() { (*task)(); });
    }

    condition.notify_one();
    return result;
}