#include <iostream>
#include <stdexcept>
#define M 5
#define KV template<typename K, typename V> //K: key, V: value
#define BNT BtreeNode<K, V>
#define BT Btree<K, V>

KV
class Btree;

KV
class pair { //simplify pair key-value
public:
    K k;
    V v;

    pair() : k(), v() {} // default
    pair(const K& key, const V& value) : k(key), v(value) {}
    bool operator < (const pair& p);
    friend std::ostream& operator << (std::ostream& out, const pair& p) {
        out << "key: " << p.k << " value: " << p.v;
        return out;
    }
};

template<typename K, typename V>
pair<K, V> make_pair(const K& key, const V& value) {
    return pair<K, V>(key, value);
}

KV
class BtreeNode {
private:
    bool isLeaf;
    int n; //current count of key
    pair<K, V>* key; //max key: 4, min key: 2
    BtreeNode** children; //max child: 5, min child: 3
    int depth;

public:
    BtreeNode(bool leaf); //create node
    ~BtreeNode();

    friend class Btree<K, V>;
};

KV
class Btree {
private:
    BNT* root;

public:
    Btree();
    ~Btree();
    void insert(pair<K, V> p);
    void remove(K k);
    V search(K k);
    void traverse();

private:
    void deleteTree(BtreeNode<K, V>* node);
    BNT* search(BNT* node, K k);
    void insertToNonFull(BNT* node, pair<K, V> p);
    void sortCur(BNT* node);
    void split(BNT* parent, BNT* target_child, int i);
    void traverse(BNT* node, int d);
    void remove(BNT* node, K k);
    BNT* findParent(BNT* node, BNT* child);
    pair<K, V> getPred(BNT* node, int curIdx); //Predecessor
    pair<K, V> getSucc(BNT* node, int curIdx); //Successor
    void merge(BNT* node, int idx);
    void case1(BNT* node, K k);
    void case2(BNT* node, K k);
    void case3(BNT* node, K k);
};


/*
 * implement of pair class
 */
KV
bool pair<K, V>::operator < (const pair<K, V>& p) {
    return this->k < p.k;
}

/*
 *  implement of BtreeNode class
 *
 *  data(key): (pair<K, V>
 *  isLeaf: distinguish this node is leaf node
 *  children: the children node array
 *  n: number of data in the current data
 */
KV
BNT::BtreeNode(bool leaf) {
    isLeaf = leaf;
    n = 0;
    key = new pair<K, V>[M - 1];
    children = new BtreeNode*[M];
    depth = 0;
}

KV
BNT::~BtreeNode() {
    delete[] key;
    delete[] children;
}

/*
 *  implement of Btree class
 *
 *  this class has only root node, and searching with recursion
 */
KV
void BT::deleteTree(BtreeNode<K, V>* node) {
    if(node) {
        if(!node->isLeaf) {
            for(int i = 0; i < node->n; i++) {
                deleteTree(node->children[i]);
            }
        }
        delete node;
    }
}

KV
BT::Btree() {
    root = new BtreeNode<K, V>(true);
}

KV
BT::~Btree() {
    deleteTree(root);
}

KV
BNT* BT::search(BNT* node, K k) {
    int i = 0;
    while(i < node->n && k > node->key[i].k)
        i++;

    if(i < node->n && k == node->key[i].k)
        return node;

    if(node->isLeaf)
        return nullptr;

    return search(node->children[i], k);
}

KV
V BT::search(K k) {
    BNT* res = search(root, k);
    if(res != nullptr) {
        for(int i = 0; i < res->n; i++) {
            if(res->key[i].k == k)
                return res->key[i].v;
        }
    }

    throw std::runtime_error("Key not found");
}

KV
void BT::sortCur(BtreeNode<K, V> *node) {
    for(int i = 0; i < node->n; i++) {
        for(int j = i; j < node->n; j++) {
            if(node->key[j] < node->key[i]) {
                pair<K, V> tmp = node->key[i];
                node->key[i] = node->key[j];
                node->key[j] = tmp;
            }
        }
    }
    std::cout << "우웅?\n";
}

KV
void BT::insertToNonFull(BtreeNode<K, V> *node, pair<K, V> p) {
    int j = node->n - 1;

    if(node->isLeaf) {
        while(j >= 0 && p < node->key[j]) {
            node->key[j + 1] = node->key[j];
            j--;
        }

        node->key[j + 1] = p;
        node->n++;
    } else {
        while(j >= 0 && p < node->key[j])
            j--;
        j++;

        if(node->children[j]->n == M-1) {
            split(node, node->children[j], j);
            if(node->key[j] < p) {
                j++;
            }
        }
        insertToNonFull(node->children[j], p);
    }
}

KV
void BT::insert(pair<K, V> p) {
    if(root->n == M-1) {
        BNT* tmp = new BNT(false);
        tmp->children[0] = root;
        split(tmp, root, 0);
        root = tmp;
    }
    insertToNonFull(root, p);
}

KV
void BT::split(BNT* parent, BNT* target_child, int i) {
    int t = int(M / 2);
    BNT* z = new BNT(target_child->isLeaf);
    z->n = t - 1;

    for(int j = 0; j < t - 1; ++j) {
        z->key[j] = target_child->key[target_child->n - 1 - j];
    }
    if(!target_child->isLeaf) {
        for(int j = 0; j < t; ++j) {
            z->children[j] = target_child->children[target_child->n - 1 - j];
        }
    }

    target_child->n = t;

    for(int j = parent->n; j >= i + 1; --j) {
        parent->children[j + 1] = parent->children[j];
    }
    parent->children[i + 1] = z;

    
    for(int j = parent->n - 1; j > i; --j) {
        parent->key[j + 1] = parent->key[j];
    }
    parent->key[i] = target_child->key[t];
    parent->n++;
    // std::cout << target_child->n << "target_child->n\n";
    // std::cout << parent->n << "parent->n\n";
    // std::cout << z->n << "z->n\n";
}

KV
void BT::traverse() {
    traverse(root, 1);
}

KV
void BT::traverse(BNT* node, int d) {
    int i;
    for(i = 0; i < node->n; i++) {
        if(!node->isLeaf)
            traverse(node->children[i], d + 1);
        std::cout << "Current depth:" << d << " " << node->key[i] << "\n";
    }

    if(!node->isLeaf)
        traverse(node->children[i], d + 1);
}

KV
void BT::remove(K k) {
    if(root->n == 0) {
        std::cout << "This tree is empty\n";
        return;
    }
    BNT* node = search(root, k);
    if(node) {
        remove(node, k);
    } else {
        return;
    }

    if(root->n == 0 && !root->isLeaf) {
        BNT* oldRoot = root;
        root = root->children[0];
        delete oldRoot;
    }
}

KV
BNT* BT::findParent(BNT* node, BNT* child) {
    if(node == nullptr || node->isLeaf) {
        return nullptr;
    }

    for(int i = 0; i <= node->n; ++i) {
        if(node->children[i] == child)
            return node;

        BNT* parent = findParent(node->children[i], child);
        if(parent != nullptr)
            return parent;
    }

    return nullptr;
}

KV
pair<K, V> BT::getPred(BNT* node, int curIdx) { //left, biggest key
    BNT* tmp = node->children[curIdx];
    while(!tmp->isLeaf) {
        tmp = tmp->children[tmp->n];
    }
    return tmp->key[tmp->n - 1];
}

KV
pair<K, V> BT::getSucc(BNT* node, int curIdx) { //right, smallest key
    BNT* tmp = node->children[curIdx + 1];
    while(!tmp->isLeaf) {
        tmp = tmp->children[0];
    }
    return tmp->key[0];
}

KV
void BT::case1(BNT* node, K k) {
    //case 1-1: current node has keys more than min key 'M%2 ? ceil(M/2) - 1 : ceil(M/2)'
    //case 1-2: current node has min key, left or right node sibling has keys more than min key
    //case 1-3: current and sibling nodes have min key, parent node has keys more than min key
    //case 1-4: current, sibling, and parent nodes have min key

    int delIdx = -1;
    for (int i = 0; i < node->n; ++i) {
        if (node->key[i].k == k) {
            delIdx = i;
            break;
        }
    }
    if (delIdx == -1) return; // key not found

    for (int j = delIdx + 1; j < node->n; j++) {
        node->key[j - 1] = node->key[j];
    }
    node->n--;

    if (node->n >= int(M / 2)) { // case 1-1
        return;
    }


    BNT* parent = findParent(root, node);
    BNT* left = nullptr;
    BNT* right = nullptr;
    int siblingIdx;

    if(parent != nullptr) {
        for(int i = 0; i <= parent->n; i++) {
            if(parent->children[i] == node) {
                siblingIdx = i;
                if(i - 1 >= 0) {
                    left = parent->children[i - 1];
                }
                if(i + 1 <= parent->n) {
                    right = parent->children[i + 1];
                }
                break;
            }
        } //find left and right sibling
    } else { //it is root node
        return;
    }

    if(left && left->n > int(M / 2)) { //case 1-2 left
        //borrow from left
        for (int i = node->n; i > 0; --i)
            node->key[i] = node->key[i - 1];

        node->key[0] = parent->key[siblingIdx - 1];
        parent->key[siblingIdx - 1] = left->key[left->n - 1];

        left->n--;
        node->n++;
    } else if(right && right->n > int(M / 2)) { //case 1-2 right
        //borrow from right
        node->key[node->n] = parent->key[siblingIdx];
        parent->key[siblingIdx] = right->key[0];

        for (int i = 1; i < right->n; ++i)
            right->key[i - 1] = right->key[i];

        right->n--;
        node->n++;
    } else if((right && right->n == int(M / 2)) || (left && left->n == int(M / 2))) { //case 1-3 or case 1-4
        if(parent->n > int(M/2)) { //case 1-3
            if(left) {
                // remove the key and child from parent
                pair<K, V> tmp = parent->key[siblingIdx - 1];
                for(int i = siblingIdx - 1; i < parent->n - 1; i++) {
                    parent->key[i] = parent->key[i + 1];
                    parent->children[i + 1] = parent->children[i + 2];
                }
                parent->n--;

                // move parent's key to left sibling
                left->key[left->n] = tmp;
                left->n++;

                // move node's keys to left sibling
                for(int i = 0; i < node->n; i++) {
                    left->key[left->n + i] = node->key[i];
                }
                left->n += node->n;

                delete node;
            } else {
                // move parent's key to node
                node->key[node->n] = parent->key[siblingIdx];
                node->n++;

                // remove the key and child from parent
                for(int i = siblingIdx; i < parent->n - 1; i++) {
                    parent->key[i] = parent->key[i + 1];
                    parent->children[i + 1] = parent->children[i + 2];
                }
                parent->n--;

                // move right sibling's key to node
                for (int i = 0; i < right->n; ++i) {
                    node->key[node->n + i] = right->key[i];
                }
                node->n += right->n;

                delete right;
            }
        } else { //case 1-4
            // Handle case 1-4: current, sibling, and parent nodes have min key
            // it will implement on case 3
            if(left) {
                // Merge with left sibling
                left->key[left->n] = parent->key[siblingIdx - 1];
                left->n++;

                for(int i = 0; i < node->n; i++) {
                    left->key[left->n + i] = node->key[i];
                }
                left->n += node->n;

                for(int i = siblingIdx - 1; i < parent->n - 1; i++) {
                    parent->key[i] = parent->key[i + 1];
                    parent->children[i] = parent->children[i + 1];
                }
                parent->n--;

                delete node;
            } else if(right) {
                // Merge with right sibling
                node->key[node->n] = parent->key[siblingIdx];
                node->n++;

                for(int i = 0; i < right->n; i++) {
                    node->key[node->n + i] = right->key[i];
                }
                node->n += right->n;

                for(int i = siblingIdx; i < parent->n - 1; i++) {
                    parent->key[i] = parent->key[i + 1];
                    parent->children[i] = parent->children[i + 1];
                }
                parent->n--;

                delete right;
            }

            // If the parent now has fewer than the minimum number of keys, handle recursively
            if(parent->n <= int(M / 2)) {
                if(parent == root && root->n == 0) {
                    BNT* oldRoot = root;
                    root = root->children[0];
                    delete oldRoot;
                } else {
                    case1(parent, k);
                }
            }
        }
    }
}

KV
void BT::merge(BtreeNode<K, V> *node, int idx) {
    BNT* first = node->children[idx];
    BNT* second = node->children[idx + 1];

    first->key[first->n] = node->key[idx];
    first->n++;

    for(int i = 0; i < second->n; ++i) {
        first->key[i + first->n] = second->key[i];
        first->children[i + first->n + 1] = second->children[i];
    }
    first->n += second->n;

    for(int i = idx + 1; i < node->n; ++i) {
        node->key[i-1] = node->key[i];
        node->children[i] = node->children[i + 1];
    }
    node->n--;
}

KV
void BT::case2(BNT* node, K k) {
    int idx = -1;
    for(int i = 0; i < node->n; i++) {
        if(node->key[i].k == k)
            idx = i;
    }

    if (idx == -1) return;

    if (node->children[idx]->n > int(M / 2)) {
        pair<K, V> key = getPred(node, idx);
        node->key[idx] = key;
        remove(node->children[idx], key.k);
    } else if (node->children[idx + 1]->n > int(M / 2)) {
        pair<K, V> key = getSucc(node, idx);
        node->key[idx] = key;
        remove(node->children[idx + 1], key.k);
    } else {
        if(idx < node->n) {
            merge(node, idx);
        } else {
            merge(node, idx-1);
        }
        remove(node->children[idx], k);
    }
}

KV
void BT::case3(BNT* node, K k) {
    int idx = -1;
    for(int i = 0; i < node->n; i++) {
        if(node->key[i].k == k)
            idx = i;
    }

    if (idx == -1) return;

    if(node->children[idx]->n == int(M / 2)) {
        if(idx != 0 && node->children[idx - 1]->n > int(M / 2)) {
            //borrow from left sibling
            BNT* child = node->children[idx];
            BNT* sibling = node->children[idx - 1];

            for(int i = child->n; i > 0; --i) {
                child->key[i] = child->key[i - 1];
                child->children[i + 1] = child->children[i];
            }
            child->n++;
            child->key[0] = node->key[idx - 1];

            node->key[idx - 1] = sibling->key[sibling->n - 1];
            sibling->n--;
        } else if(idx != node->n && node->children[idx + 1]->n > int(M / 2)) {
            //borrow from right sibling
            BNT* child = node->children[idx];
            BNT* sibling = node->children[idx + 1];

            child->key[child->n] = node->key[idx];
            child->n++;
            child->children[child->n] = sibling->children[0];

            for(int i = 1; i < sibling->n; i++) {
                sibling->key[i - 1] = sibling->key[i];
                sibling->children[i - 1] = sibling->children[i];
            }
            sibling->children[sibling->n - 1] = sibling->children[sibling->n];
            sibling->n--;
        } else {
            if(idx != node->n) {
                merge(node, idx);
            } else {
                merge(node, idx - 1);
            }
        }
    }
    remove(node->children[idx], k);
}

KV
void BT::remove(BNT* node, K k) {
    if (node->isLeaf) {
        // Case 1: Current node is a leaf node
        //         - If the node is a leaf, remove the key directly.
        //         - If the node has more than the minimum number of keys, simply remove the key.
        //         - If the node has exactly the minimum number of keys:
        //             - Borrow a key from a sibling with more than the minimum number of keys, or
        //             - Merge with a sibling and reduce the height of the tree if necessary.

        case1(node, k);
    } else {
        // Case 2: Current node is not a leaf node and both current and children nodes have more than the minimum number of keys
        //         - Remove the key and ensure the tree remains balanced.

        // Case 3: Current node is not a leaf node and current node has the minimum number of keys
        //     Case 3-1: Child node has more than the minimum number of keys
        //         - Find the predecessor or successor key
        //         - Replace the key with the predecessor or successor
        //         - Remove the predecessor or successor key from the leaf node (this will lead to Case 1)

        //     Case 3-2: Child node has the minimum number of keys
        //         - Borrow a key from a sibling that has more than the minimum number of keys and exchange it with the parent key
        //         - Merge with a sibling that has the minimum number of keys, bringing down a key from the parent to merge
        //         - Apply Case 3 recursively to the parent if it has fewer than the minimum number of keys

        int idx = -1;
        for(int i = 0; i < node->n; i++) {
            if(node->key[i].k == k) {
                idx = i;
                break;
            }
        }

        if(idx != -1 && node->key[idx].k == k) {
            case2(node, k);
        } else {
            case3(node, k);
        }
    }
}