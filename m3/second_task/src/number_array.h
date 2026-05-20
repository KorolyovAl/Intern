#pragma once

#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>
#include <unordered_set>
#include <iterator>

class NumberArray {
public:
    void GenerateNumbers(std::size_t count = 100, int min = 1, int max = 1000) {
        numbers_.resize(count);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(min, max);

        std::generate(numbers_.begin(), numbers_.end(), [&]() {
            return dist(gen);
        });
    }

    void SortDescending() {
        std::sort(numbers_.begin(), numbers_.end(), std::greater<int>());
    }

    std::vector<int> GetPrimeNumbers() const {
        std::vector<int> primes;

        std::copy_if(numbers_.begin(), numbers_.end(), std::back_inserter(primes),
            [](int value) {
                return IsPrime(value);
            });

        return primes;
    }

    void RemoveDuplicates() {
        std::unordered_set<int> seen;
        std::vector<int> unique;
        unique.reserve(numbers_.size());

        std::copy_if(numbers_.begin(), numbers_.end(), std::back_inserter(unique), [&seen](int x) {            
            return seen.insert(x).second;
        });

        numbers_ = std::move(unique);
    }

    std::uint64_t GetSumOfSquares() const {
        return std::accumulate(numbers_.begin(), numbers_.end(), std::uint64_t{0},
            [](std::uint64_t sum, int value) {
                return sum + static_cast<std::uint64_t>(value * value);
            });
    }

    void Print() const {
        for (int value : numbers_) {
            std::cout << value << ' ';
        }
        std::cout << '\n';
    }

private:
    static bool IsPrime(int number) {
        if (number < 2) {
            return false;
        }

        for (int i = 2; i * i <= number; ++i) {
            if (number % i == 0) {
                return false;
            }
        }

        return true;
    }

private:
    std::vector<int> numbers_;
};
