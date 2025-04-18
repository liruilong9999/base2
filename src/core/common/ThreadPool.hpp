#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>             // 包含 vector 类
#include <queue>              // 包含 queue 类
#include <memory>             // 包含智能指针
#include <thread>             // 包含线程类
#include <mutex>              // 包含互斥锁
#include <condition_variable> // 包含条件变量
#include <future>             // 包含未来（future）类
#include <functional>         // 包含函数适配器和绑定
#include <stdexcept>          // 包含标准异常类

class ThreadPool
{
public:
    // 构造函数，接受线程数量
    ThreadPool(size_t);

    // 将任务添加到线程池的队列中
    template <class F, class... Args>
    auto enqueue(F && f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type>;

    // 析构函数
    ~ThreadPool();

private:
    std::vector<std::thread>          workers; // 用于跟踪线程，以便在析构时可以 join 这些线程
    std::queue<std::function<void()>> tasks;   // 任务队列，用于存储待执行的任务
    // 同步相关
    std::mutex              queue_mutex; // 保护任务队列的互斥锁
    std::condition_variable condition;   // 条件变量，用于线程间通信
    bool                    stop;        // 标志，指示线程池是否停止
};

// 构造函数，启动指定数量的工作线程
inline ThreadPool::ThreadPool(size_t threads)
    : stop(false) // 初始化时，线程池未停止
{
    // 创建并启动线程
    for (size_t i = 0; i < threads; ++i)
        workers.emplace_back( // 将新线程添加到 workers 向量
            [this] {          // 线程的任务是这个 lambda 表达式
                for (;;)      // 无限循环，直到显式返回
                {
                    std::function<void()> task; // 定义任务

                    // 锁住队列
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    // 等待条件变量，直到有任务可执行或线程池停止
                    this->condition.wait(lock,
                                         [this] {
                                             return this->stop || !this->tasks.empty();
                                         });
                    // 如果线程池停止并且任务队列为空，则返回
                    if (this->stop && this->tasks.empty())
                        return;

                    // 从队列中获取任务并移除
                    task = std::move(this->tasks.front());
                    this->tasks.pop();

                    // 执行任务
                    task();
                }
            });
}

// 将新任务添加到线程池中
template <class F, class... Args>
auto ThreadPool::enqueue(F && f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type; // 任务返回类型

    // 创建一个共享指针，封装任务
    auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    // 获取未来对象，以便任务完成后获取结果
    std::future<return_type> res = task->get_future();
    {
        // 锁住任务队列
        std::unique_lock<std::mutex> lock(queue_mutex);

        // 如果线程池已经停止，则抛出异常
        if (stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        // 将任务封装成 lambda 表达式并添加到任务队列中
        tasks.emplace([task]() {
            (*task)(); // 执行任务
        });
    }
    // 通知一个等待的线程
    condition.notify_one();
    return res; // 返回未来对象
}

// 析构函数，等待所有线程完成工作
inline ThreadPool::~ThreadPool()
{
    // 锁住任务队列
    std::unique_lock<std::mutex> lock(queue_mutex);
    stop = true; // 设置停止标志

    condition.notify_all(); // 通知所有线程退出
    // 等待每个工作线程结束
    for (std::thread & worker : workers)
        worker.join();
}

#endif // THREAD_POOL_H
