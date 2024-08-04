#include "Btree.h"
#include <iostream>
#include <cassert>
#include <random>

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(1, 100);

    Btree<int, int> tree;

    // Insert test
    for (int i = 1; i <= 100; ++i) {
        tree.insert({i, 1});
    }   
    
    tree.traverse();
    std::cout << "-----------------" << std::endl;
    
    for (int i = 0; i < 20; i++) {
        int n = dis(gen);
        std::cout << "난수 : " << n << std::endl;
        tree.remove(n);
    }

    // Traversal test
    tree.traverse();
    std::cout << std::endl;

    std::cout << "All test cases passed!" << std::endl;
    return 0;
}