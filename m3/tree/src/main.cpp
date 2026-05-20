#include "myTree.h"

int main() {

    RBTree<int> tr;

    tr.insert(150);
    tr.insert(1);
    tr.insert(32);
    tr.insert(1111);
    tr.insert(231);

    tr.remove(231);

    tr.print();

    if (tr.find(32) != nullptr) {
        std::cout << *tr.find(32) << std::endl;
    }

    return 0;
}