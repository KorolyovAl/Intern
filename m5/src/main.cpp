#include <iostream>
#include <syncstream>
#include <chrono>
#include <thread>
#include <random>

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
                    << "arguments: int num" << "\n";

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

    ThreadPool pool{3};

    try {
        pool.Enqueue(first_task);

        auto res = pool.Enqueue(second_task, 10);
        std::osyncstream(std::cout) << "second task result: " << res.get() << "\n\n";

        pool.Enqueue(third_task, 3.1415, "PI number");
    }
    catch (std::exception& e) {
        std::cerr << "exception: " << e.what();
        return 1;
    }    

    pool.Shutdown();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // проверка выброса исключения при добавлении задачи в остановленную очередь
    try {
        pool.Enqueue(first_task);
    }
    catch (std::runtime_error& e) {
        std::osyncstream(std::cout) << "exception after closing thread pool\n";
    }

    return 0;
}