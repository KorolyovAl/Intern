#include <chrono>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <list>
#include <vector>
#include <random>
#include <algorithm>

using Clock = std::chrono::steady_clock;
using mcrs = std::chrono::microseconds;

void SetCase(const std::vector<int>& nums, const std::vector<int>& other_nums) {
    // set performance
    {
        std::set<int> values;

        // insert
        auto start = Clock::now();
        for (int value : nums) {
            values.insert(value);
        }
        auto end = Clock::now();

        std::cout << "set inserting: " 
                << std::chrono::duration_cast<mcrs>(end - start).count() << " microseconds\n"; 

        // search
        start = Clock::now();
        for (auto num : other_nums) {
            values.find(num);
        }
        end = Clock::now();

        std::cout << "set searching: " 
                << std::chrono::duration_cast<mcrs>(end - start).count() << " microseconds\n"; 

        // remove
        start = Clock::now();
        for (auto num : other_nums) {
            values.erase(num);
        }
        end = Clock::now();

        std::cout << "set erasing: " 
                << std::chrono::duration_cast<mcrs>(end - start).count() << " microseconds\n"; 
    }

    // unordered_set performance
    {
        std::unordered_set<int> values;

        // insert
        auto start = Clock::now();
        for (int value : nums) {
            values.insert(value);
        }
        auto end = Clock::now();

        std::cout << "unordered_set inserting: " 
                << std::chrono::duration_cast<mcrs>(end - start).count() << " microseconds\n";
        
        // search
        start = Clock::now();
        for (auto num : other_nums) {
            values.find(num);
        }
        end = Clock::now();

        std::cout << "unordered_set searching: "
                << std::chrono::duration_cast<mcrs>(end - start).count() << " microseconds\n";

        // remove
        start = Clock::now();
        for (auto num : other_nums) {
            values.erase(num);
        }
        end = Clock::now();

        std::cout << "unordered_set erasing: "
                << std::chrono::duration_cast<mcrs>(end - start).count() << " microseconds\n";
    }

}

void MapCase(const std::vector<int>& nums, const std::vector<int>& other_nums) {
    // map performance
    {
        std::map<int, int> values;

        // insert
        auto start = Clock::now();
        for (int value : nums) {
            values.insert({value, value});
        }
        auto end = Clock::now();

        std::cout << "map inserting: " 
                << std::chrono::duration_cast<mcrs>(end - start).count() << " microseconds\n";

        // search
        start = Clock::now();
        for (auto num : other_nums) {
            values.find(num);
        }
        end = Clock::now();

        std::cout << "map searching: "
                << std::chrono::duration_cast<mcrs>(end - start).count() << " microseconds\n";

        // remove
        start = Clock::now();
        for (auto num : other_nums) {
            values.erase(num);
        }
        end = Clock::now();

        std::cout << "map erasing: " 
                << std::chrono::duration_cast<mcrs>(end - start).count() << " microseconds\n";
    }

    // unordered_map performance
    {
        std::unordered_map<int, int> values;

        // insert
        auto start = Clock::now();
        for (int value : nums) {
            values.insert({value, value});
        }
        auto end = Clock::now();

        std::cout << "unordered_map inserting: " 
                << std::chrono::duration_cast<mcrs>(end - start).count() << " microseconds\n";

        // search
        start = Clock::now();
        for (auto num : other_nums) {
            values.find(num);
        }
        end = Clock::now();

        std::cout << "unordered_map searching: "
                << std::chrono::duration_cast<mcrs>(end - start).count() << " microseconds\n";

        // remove
        start = Clock::now();
        for (auto num : other_nums) {
            values.erase(num);
        }
        end = Clock::now();

        std::cout << "unordered_map erasing: " 
                << std::chrono::duration_cast<mcrs>(end - start).count() << " microseconds\n";
    }
}

void ArrayCase(const std::vector<int>& nums, const int middle_operations_count) {    
    // vector performance
    {       
        std::vector<int> values = nums;

        // insert middle
        auto middle = values.begin() + values.size() / 2;

        auto start = Clock::now();
        for (int i = 0; i < middle_operations_count; ++i) {            
            middle = values.insert(middle, i);
        }
        auto end = Clock::now();

        std::cout << "vector insert middle: "
                << std::chrono::duration_cast<mcrs>(end - start).count() << " microseconds\n";

        // erase middle
        middle = values.begin() + values.size() / 2;

        start = Clock::now();
        for (int i = 0; i < middle_operations_count; ++i) {
            middle = values.erase(middle);
        }
        end = Clock::now();

        std::cout << "vector erase middle: "
                << std::chrono::duration_cast<mcrs>(end - start).count() << " microseconds\n";
    }

    // list performance
    {
        std::list<int> values(nums.begin(), nums.end());

        // insert middle
        auto middle = values.begin();
        std::advance(middle, values.size() / 2);

        auto start = Clock::now();
        for (int i = 0; i < middle_operations_count; ++i) {
            middle = values.insert(middle, i);
        }
        auto end = Clock::now();

        std::cout << "list insert middle: "
                << std::chrono::duration_cast<mcrs>(end - start).count() << " microseconds\n";

        // erase middle
        middle = values.begin();
        std::advance(middle, values.size() / 2);

        start = Clock::now();
        for (int i = 0; i < middle_operations_count; ++i) {
            middle = values.erase(middle);
        }
        end = Clock::now();

        std::cout << "list erase middle: "
                << std::chrono::duration_cast<mcrs>(end - start).count() << " microseconds\n";
    }
}

void GenerateNumbers(std::vector<int>& vec, std::size_t count = 100, 
                    int min = 1, int max = 100'000) {
    vec.resize(count);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(min, max);

    std::generate(vec.begin(), vec.end(), [&]() {
        return dist(gen);
    });
}

int main() {
    constexpr int data_size = 100'000;
    std::vector<int> nums;
    std::vector<int> other_nums;

    GenerateNumbers(nums, data_size);
    GenerateNumbers(other_nums);

    std::cout << "set VS unordered_set perf tests:\n\n";
    SetCase(nums, other_nums);
    std::cout << "\n";

    std::cout << "map VS unordered_map perf tests:\n\n";
    MapCase(nums, other_nums);
    std::cout << "\n";

    std::cout << "vector VS list perf tests:\n\n";
    ArrayCase(nums, 10'000);
    std::cout << "\n";

    return 0;
}