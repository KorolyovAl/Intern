#include <iostream>

#include "hash_table.h"

int main() {

    HashTable<std::string, double> table(10);

    std::cout << table.getSize() << '\n';

    table.insert("abc", 13.);
    table.insert("def", 543.2);
    table.insert("asd", 111);
    table.insert("str", 44321.123);

    std::cout << table.getSize() << '\n';

    table.printStats();

    return 0;
}