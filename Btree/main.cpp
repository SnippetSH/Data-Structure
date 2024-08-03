#include "Btree.h"
#include <iostream>
#include <cassert>

int main() {
    Btree<int, std::string> tree;

    // Insert test
    tree.insert({10, "ten"});
    tree.traverse();
    
    tree.insert({20, "twenty"});
    tree.traverse();
   
    tree.insert({5, "five"});
    tree.traverse();
    
    tree.insert({6, "six"});
    tree.traverse();
    
    tree.insert({12, "twelve"});
    tree.traverse();
    
    tree.insert({30, "thirty"});
    tree.traverse();
    
    tree.insert({7, "seven"});
    tree.traverse();
    
    tree.insert({17, "seventeen"});
    tree.traverse();
    

    std::cout << "insert done\n";

    // Search test
    assert(tree.search(10) == "ten");
    
    assert(tree.search(20) == "twenty");
    
    assert(tree.search(5) == "five");
    

    std::cout << "search done\n";

    // Deletion test
    tree.remove(6);
    tree.traverse();
    std::cout << " ----- \n";
    tree.remove(13); // non-existing key
    tree.traverse();
    std::cout << " ----- \n";
    tree.remove(7);
    tree.traverse();
    std::cout << " ----- \n";
    tree.remove(4); // non-existing key
    tree.traverse();
    std::cout << " ----- \n";
    tree.remove(2); // non-existing key
    tree.traverse();
    std::cout << " ----- \n";
    tree.remove(10);
    tree.traverse();
    std::cout << " ----- \n";
    tree.remove(30);
    tree.traverse();
    

    std::cout << "deletion done\n";

    // Traversal test
    tree.traverse();
    std::cout << std::endl;

    std::cout << "All test cases passed!" << std::endl;
    return 0;
}