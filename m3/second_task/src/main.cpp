#include <iostream>
#include <vector>

#include "number_array.h"

int main() {
    NumberArray arr;

    arr.GenerateNumbers();

    std::cout << "Original numbers:\n";
    arr.Print();

    arr.SortDescending();
    std::cout << "\nSorted descending:\n";
    arr.Print();

    std::vector<int> primes = arr.GetPrimeNumbers();
    std::cout << "\nPrime numbers:\n";
    for (int value : primes) {
        std::cout << value << ' ';
    }
    std::cout << '\n';

    arr.RemoveDuplicates();
    std::cout << "\nWithout duplicates:\n";
    arr.Print();

    std::cout << "\nSum of squares: " << arr.GetSumOfSquares() << '\n';

    return 0;
}