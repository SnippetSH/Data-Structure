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
    void borrowFromPrev(BNT* parent, int idx);
    void borrowFromNext(BNT* parent, int idx);
    void merge(BNT* parent, int idx);
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
void BT::insertToNonFull(BtreeNode<K, V>* node, pair<K, V> p) {
    int j = node->n - 1;

    if (node->isLeaf) {
        // 노드가 리프 노드일 때, 새로운 키를 올바른 위치에 삽입합니다.
        while (j >= 0 && p < node->key[j]) {
            node->key[j + 1] = node->key[j];
            j--;
        }
        node->key[j + 1] = p;
        node->n++;
    } else {
        // 노드가 리프 노드가 아닐 때, 올바른 자식 노드를 찾습니다.
        while (j >= 0 && p < node->key[j])
            j--;
        j++;

        // 자식 노드가 꽉 차있다면, 분할합니다.
        if (node->children[j]->n == M - 1) {
            split(node, node->children[j], j);
            if (node->key[j] < p) {
                j++;
            }
        }
        insertToNonFull(node->children[j], p);
    }
}


KV
void BT::insert(pair<K, V> p) {
    if (root->n == M - 1) {
        BNT* tmp = new BNT(false);
        tmp->children[0] = root;
        split(tmp, root, 0);
        root = tmp;
    }
    insertToNonFull(root, p);
}


KV
void BT::split(BNT* parent, BNT* target_child, int i) {
    int t = (M - 1) / 2;
    BNT* z = new BNT(target_child->isLeaf);
    z->n = M - 1 - t - 1;

    // target_child의 키를 z로 이동합니다.
    for (int j = 0; j < z->n; ++j) {
        z->key[j] = target_child->key[j + t + 1];
    }
    if (!target_child->isLeaf) {
        // target_child의 자식을 z로 이동합니다.
        for (int j = 0; j <= z->n; ++j) {
            z->children[j] = target_child->children[j + t + 1];
        }
    }

    target_child->n = t;

    // 부모 노드의 자식을 조정합니다.
    for (int j = parent->n; j >= i + 1; --j) {
        parent->children[j + 1] = parent->children[j];
    }
    parent->children[i + 1] = z;

    // 부모 노드의 키를 조정합니다.
    for (int j = parent->n - 1; j >= i; --j) {
        parent->key[j + 1] = parent->key[j];
    }
    parent->key[i] = target_child->key[t];
    parent->n++;
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
pair<K, V> BT::getPred(BNT* node, int idx) {
    BNT* cur = node->children[idx];
    while (!cur->isLeaf)
        cur = cur->children[cur->n];

    return cur->key[cur->n - 1];
}

KV
pair<K, V> BT::getSucc(BNT* node, int idx) {
    BNT* cur = node->children[idx + 1];
    while (!cur->isLeaf)
        cur = cur->children[0];

    return cur->key[0];
}

KV
void BT::remove(BNT* node, K k) {
    int idx = 0;
    while (idx < node->n && node->key[idx].k < k)
        ++idx;

    if (idx < node->n && node->key[idx].k == k) {
        if (node->isLeaf) {
            // 키를 제거
            for (int i = idx + 1; i < node->n; ++i) {
                node->key[i - 1] = node->key[i];
            }
            node->n--;
        } else {
            if (node->children[idx]->n >= (M + 1) / 2) {
                pair<K, V> pred = getPred(node, idx);
                node->key[idx] = pred;
                remove(node->children[idx], pred.k);
            } else if (node->children[idx + 1]->n >= (M + 1) / 2) {
                pair<K, V> succ = getSucc(node, idx);
                node->key[idx] = succ;
                remove(node->children[idx + 1], succ.k);
            } else {
                merge(node, idx);
                remove(node->children[idx], k);
            }
        }
    } else {
        if (node->isLeaf) {
            std::cerr << "The key " << k << " does not exist in the tree\n";
            return;
        }

        bool flag = (idx == node->n);

        if (node->children[idx]->n < (M + 1) / 2) {
            if (idx != 0 && node->children[idx - 1]->n >= (M + 1) / 2) {
                borrowFromPrev(node, idx);
            } else if (idx != node->n && node->children[idx + 1]->n >= (M + 1) / 2) {
                borrowFromNext(node, idx);
            } else {
                if (idx != node->n) {
                    merge(node, idx);
                } else {
                    merge(node, idx - 1);
                }
            }
        }

        if (flag && idx > node->n)
            remove(node->children[idx - 1], k);
        else
            remove(node->children[idx], k);
    }

    if (root->n == 0) {
        BNT* temp = root;
        if (root->isLeaf)
            root = nullptr;
        else
            root = root->children[0];
        delete temp;
    }
}

KV
void BT::borrowFromPrev(BNT* parent, int idx) {
    BNT* child = parent->children[idx];
    BNT* sibling = parent->children[idx - 1];

    for (int i = child->n - 1; i >= 0; --i)
        child->key[i + 1] = child->key[i];

    if (!child->isLeaf) {
        for (int i = child->n; i >= 0; --i)
            child->children[i + 1] = child->children[i];
    }

    child->key[0] = parent->key[idx - 1];

    if (!child->isLeaf)
        child->children[0] = sibling->children[sibling->n];

    parent->key[idx - 1] = sibling->key[sibling->n - 1];

    child->n += 1;
    sibling->n -= 1;
}

KV
void BT::borrowFromNext(BNT* parent, int idx) {
    BNT* child = parent->children[idx];
    BNT* sibling = parent->children[idx + 1];

    child->key[child->n] = parent->key[idx];

    if (!child->isLeaf)
        child->children[child->n + 1] = sibling->children[0];

    parent->key[idx] = sibling->key[0];

    for (int i = 1; i < sibling->n; ++i)
        sibling->key[i - 1] = sibling->key[i];

    if (!sibling->isLeaf) {
        for (int i = 1; i <= sibling->n; ++i)
            sibling->children[i - 1] = sibling->children[i];
    }

    child->n += 1;
    sibling->n -= 1;
}

KV
void BT::merge(BNT* parent, int idx) {
    BNT* child = parent->children[idx];
    BNT* sibling = parent->children[idx + 1];

    child->key[(M - 1) / 2] = parent->key[idx];

    for (int i = 0; i < sibling->n; ++i)
        child->key[i + (M - 1) / 2 + 1] = sibling->key[i];

    if (!child->isLeaf) {
        for (int i = 0; i <= sibling->n; ++i)
            child->children[i + (M - 1) / 2 + 1] = sibling->children[i];
    }

    for (int i = idx + 1; i < parent->n; ++i) {
        parent->key[i - 1] = parent->key[i];
    }

    for (int i = idx + 2; i <= parent->n; ++i) {
        parent->children[i - 1] = parent->children[i];
    }

    child->n += sibling->n + 1;
    parent->n--;

    delete sibling;
}

KV
void BT::case1(BNT* parent, K k) {
    int idx = 0;
    while (idx < parent->n && parent->key[idx].k < k)
        ++idx;

    BNT* child = parent->children[idx];
    BNT* sibling = nullptr;

    if (idx != 0 && parent->children[idx - 1]->n >= (M + 1) / 2) {
        sibling = parent->children[idx - 1];
        for (int i = child->n - 1; i >= 0; --i) {
            child->key[i + 1] = child->key[i];
        }
        if (!child->isLeaf) {
            for (int i = child->n; i >= 0; --i) {
                child->children[i + 1] = child->children[i];
            }
        }
        child->key[0] = parent->key[idx - 1];
        if (!child->isLeaf) {
            child->children[0] = sibling->children[sibling->n];
        }
        parent->key[idx - 1] = sibling->key[sibling->n - 1];
        child->n += 1;
        sibling->n -= 1;
    } else if (idx != parent->n && parent->children[idx + 1]->n >= (M + 1) / 2) {
        sibling = parent->children[idx + 1];
        child->key[child->n] = parent->key[idx];
        if (!child->isLeaf) {
            child->children[child->n + 1] = sibling->children[0];
        }
        parent->key[idx] = sibling->key[0];
        for (int i = 1; i < sibling->n; ++i) {
            sibling->key[i - 1] = sibling->key[i];
        }
        if (!sibling->isLeaf) {
            for (int i = 1; i <= sibling->n; ++i) {
                sibling->children[i - 1] = sibling->children[i];
            }
        }
        child->n += 1;
        sibling->n -= 1;
    } else {
        if (idx != parent->n) {
            merge(parent, idx);
        } else {
            merge(parent, idx - 1);
        }
    }
}

KV
void BT::case2(BNT* node, K k) {
    int idx = -1;
    for(int i = 0; i < node->n; i++) {
        if(node->key[i].k == k)
            idx = i;
    }

    if (idx == -1) return;

    if (node->children[idx]->n > (M + 1) / 2) {
        pair<K, V> key = getPred(node, idx);
        node->key[idx] = key;
        remove(node->children[idx], key.k);
    } else if (node->children[idx + 1]->n > (M + 1) / 2) {
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
    int idx = 0;
    while (idx < node->n && node->key[idx].k < k)
        ++idx;

    BNT* child = node->children[idx];
    BNT* sibling = nullptr;

    if (idx != 0 && node->children[idx - 1]->n >= (M + 1) / 2) {
        sibling = node->children[idx - 1];
        for (int i = child->n - 1; i >= 0; --i) {
            child->key[i + 1] = child->key[i];
        }
        if (!child->isLeaf) {
            for (int i = child->n; i >= 0; --i) {
                child->children[i + 1] = child->children[i];
            }
        }
        child->key[0] = node->key[idx - 1];
        if (!child->isLeaf) {
            child->children[0] = sibling->children[sibling->n];
        }
        node->key[idx - 1] = sibling->key[sibling->n - 1];
        child->n += 1;
        sibling->n -= 1;
    } else if (idx != node->n && node->children[idx + 1]->n >= (M + 1) / 2) {
        sibling = node->children[idx + 1];
        child->key[child->n] = node->key[idx];
        if (!child->isLeaf) {
            child->children[child->n + 1] = sibling->children[0];
        }
        node->key[idx] = sibling->key[0];
        for (int i = 1; i < sibling->n; ++i) {
            sibling->key[i - 1] = sibling->key[i];
        }
        if (!sibling->isLeaf) {
            for (int i = 1; i <= sibling->n; ++i) {
                sibling->children[i - 1] = sibling->children[i];
            }
        }
        child->n += 1;
        sibling->n -= 1;
    } else {
        if (idx != node->n) {
            merge(node, idx);
        } else {
            merge(node, idx - 1);
        }
    }
    remove(child, k);
}

