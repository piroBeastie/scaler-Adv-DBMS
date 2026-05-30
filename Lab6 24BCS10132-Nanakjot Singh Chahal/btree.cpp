#include <iostream>
#include <vector>
#include <string>
using namespace std;

template <typename Key, typename Value>
class BTree {
    struct Entry {
        Key key;
        Value value;
    };

    struct Node {
        bool isLeaf;
        vector<Entry> entries;
        vector<Node*> children;
        Node(bool leaf) : isLeaf(leaf) {}
    };

    int minDegree;
    Node* root;

    void splitChild(Node* parent, int idx) {
        Node* child = parent->children[idx];
        Node* sibling = new Node(child->isLeaf);
        int t = minDegree;

        for (int j = 0; j < t - 1; j++)
            sibling->entries.push_back(child->entries[t + j]);

        if (!child->isLeaf)
            for (int j = 0; j < t; j++)
                sibling->children.push_back(child->children[t + j]);

        Entry median = child->entries[t - 1];
        child->entries.resize(t - 1);
        if (!child->isLeaf)
            child->children.resize(t);

        parent->children.insert(parent->children.begin() + idx + 1, sibling);
        parent->entries.insert(parent->entries.begin() + idx, median);
    }

    void insertNonFull(Node* node, Key key, Value value) {
        int i = node->entries.size() - 1;

        if (node->isLeaf) {
            node->entries.push_back({key, value});
            while (i >= 0 && node->entries[i].key > key) {
                node->entries[i + 1] = node->entries[i];
                i--;
            }
            node->entries[i + 1] = {key, value};
        } else {
            while (i >= 0 && node->entries[i].key > key)
                i--;
            i++;
            if ((int)node->children[i]->entries.size() == 2 * minDegree - 1) {
                splitChild(node, i);
                if (node->entries[i].key < key)
                    i++;
            }
            insertNonFull(node->children[i], key, value);
        }
    }

    void printInOrder(Node* node) {
        if (!node) return;
        for (int i = 0; i < (int)node->entries.size(); i++) {
            if (!node->isLeaf)
                printInOrder(node->children[i]);
            cout << "[" << node->entries[i].key << ": " << node->entries[i].value << "] ";
        }
        if (!node->isLeaf)
            printInOrder(node->children[node->entries.size()]);
    }

public:
    BTree(int degree) : minDegree(degree), root(new Node(true)) {}

    void insert(Key key, Value value) {
        if ((int)root->entries.size() == 2 * minDegree - 1) {
            Node* newRoot = new Node(false);
            newRoot->children.push_back(root);
            root = newRoot;
            splitChild(root, 0);
        }
        insertNonFull(root, key, value);
    }

    bool search(Key key, Value& result) {
        Node* cur = root;
        while (cur) {
            int i = 0;
            while (i < (int)cur->entries.size() && key > cur->entries[i].key)
                i++;
            if (i < (int)cur->entries.size() && key == cur->entries[i].key) {
                result = cur->entries[i].value;
                return true;
            }
            if (cur->isLeaf) break;
            cur = cur->children[i];
        }
        return false;
    }

    void display() {
        printInOrder(root);
        cout << endl;
    }
};

int main() {
    BTree<int, string> db(3);

    db.insert(10, "Alice");
    db.insert(20, "Bob");
    db.insert(5, "Charlie");
    db.insert(15, "Diana");
    db.insert(25, "Eve");
    db.insert(30, "Frank");
    db.insert(1, "Grace");

    cout << "B-Tree contents: ";
    db.display();

    string val;
    int key = 20;
    if (db.search(key, val))
        cout << "Found key " << key << ": " << val << endl;
    else
        cout << "Key " << key << " not found" << endl;

    key = 99;
    if (db.search(key, val))
        cout << "Found key " << key << ": " << val << endl;
    else
        cout << "Key " << key << " not found" << endl;

    return 0;
}
