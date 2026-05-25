#include <iostream>
#include <syncstream>
#include <chrono>
#include <thread>
#include <random>
#include <memory>

#include "thread_pool.h"

int RandomNum() {
    thread_local std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<int> dist(100, 500);
    return dist(generator);
}

int main() {
    auto first_task = []() {
        std::this_thread::sleep_for(std::chrono::milliseconds(RandomNum()));
        std::osyncstream(std::cout)
                    << "thread ID: " << std::this_thread::get_id() << "\n" 
                    << "taskname: first task\n" 
                    << "arguments: none" << "\n\n";
    };

    auto second_task = [](int num) {
        std::this_thread::sleep_for(std::chrono::milliseconds(RandomNum()));
        std::osyncstream(std::cout) 
                    << "thread ID: " << std::this_thread::get_id() << "\n" 
                    << "taskname: second task\n" 
                    << "arguments: int num" << "\n\n";

        return num * num;
    };

    auto third_task = [](double num, std::string str) {
        std::this_thread::sleep_for(std::chrono::milliseconds(RandomNum()));
        std::osyncstream(std::cout) 
                    << "thread ID: " << std::this_thread::get_id() << "\n" 
                    << "taskname: third task\n" 
                    << "arguments: double num, string\n"                    
                    << "result: string is " << str 
                    << " and num is " << num << "\n\n";
    };

    std::unique_ptr<ThreadPool> pool;

    try {
        pool = std::make_unique<ThreadPool>(3);

        auto f1 = pool->Enqueue(first_task);
        auto f2 = pool->Enqueue(second_task, 10);
        auto f3 = pool->Enqueue(third_task, 3.1415, "PI number");

        f1.get();
        std::osyncstream(std::cout) << "second task result: " << f2.get() << "\n\n";
        f3.get();

        pool->Shutdown();    
    }
    catch (std::exception& e) {
        std::cerr << "exception: " << e.what();
        return 1;
    }

    // проверка выброса исключения при добавлении задачи в остановленную очередь
    try {
        pool->Enqueue(first_task);
    }
    catch (std::runtime_error& e) {
        std::osyncstream(std::cout) << "exception after closing thread pool\n";
    }

    return 0;
}