#pragma once

#include <cstddef>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <queue>
#include <vector>
#include <tuple>

template <typename Task>
class TasksQueue {
public:
    bool Push(Task task) {
        {
            std::unique_lock<std::mutex> lk{mt_};
            if (is_closed_) {
                return false;
            }
            tasks_.push(std::move(task));
        }

        cv_.notify_one();
        return true;
    }

    bool Pop(Task& out) {
        std::unique_lock<std::mutex> lk{mt_};
        cv_.wait(lk, [this]() {
            return this->is_closed_ || !this->tasks_.empty();
        });

        if (tasks_.empty()) {
            return false;
        }

        out = std::move(tasks_.front());
        tasks_.pop();

        return true;
    }

    void Close() {
        {
            std::lock_guard<std::mutex> lk{mt_};
            is_closed_ = true;
        }

        cv_.notify_all();
    }

private:
    std::queue<Task> tasks_;
    std::condition_variable cv_;
    std::mutex mt_;
    bool is_closed_ = false;
};

class ThreadPool {
public:
    using Task = std::function<void()>;

    explicit ThreadPool(size_t workers) {
        if (workers == 0) {
            throw std::invalid_argument("workers num is 0");
        }

        workers_.reserve(workers);

        for (size_t i = 0; i < workers; ++i) {
            workers_.emplace_back([this] {
                this->WorkerLoop();
            });
        }
    }

    ThreadPool(const ThreadPool& other) = delete;
    ThreadPool(ThreadPool&& other) = delete;

    ThreadPool& operator=(const ThreadPool& other) = delete;
    ThreadPool& operator=(ThreadPool&& other) = delete;

    ~ThreadPool() {
        Shutdown();      

        for (auto& w : workers_) {
            if (w.joinable()) {
                w.join();
            }
        }
    }

    template <class F, class... Args>
    auto Enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
        using Result = std::invoke_result_t<F, Args...>;

        auto task_ptr = std::make_shared<std::packaged_task<Result()>>(
            [
                func = std::forward<F>(f),
                args_tuple = std::make_tuple(std::forward<Args>(args)...)
            ]() mutable -> Result
            {
                return std::apply(std::move(func), std::move(args_tuple));
            });

        std::future<Result> result = task_ptr->get_future();

        bool check = tasks_.Push([task_ptr]{
            (*task_ptr)();
        });

        if (check == false) {
            throw std::runtime_error("The pool is stopped");
        }

        return result;
    }

    void Shutdown() {
        tasks_.Close();
    }

private:
    void WorkerLoop() {
        while (true) {
            Task ts;
            if (tasks_.Pop(ts) == false) {
                break;
            }

            ts();
        }
    }

private:
    TasksQueue<Task> tasks_;    
    std::vector<std::jthread> workers_;
};