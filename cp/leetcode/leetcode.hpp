#include "..\init\library.hpp"
#include "..\debug\debug.h"

#ifndef LEETCODE
#define LEETCODE

struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}

    // friend std::ostream& operator<<()
};

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

class Node {
public:
    int val;
    std::vector<Node*> neighbors;
    Node() {
        val = 0;
        neighbors = std::vector<Node*>();
    }
    Node(int _val) {
        val = _val;
        neighbors = std::vector<Node*>();
    }
    Node(int _val, std::vector<Node*> _neighbors) {
        val = _val;
        neighbors = _neighbors;
    }
};

namespace leetcode {
inline void __read__(int& n) {
    std::cin >> n;
    std::cin.ignore();
}

inline void __read__(std::string& s) {
    std::string tmp;
    std::getline(std::cin, tmp);
    s = tmp.substr(1, tmp.size() - 2);
}

template<typename T>
inline void __read__(std::vector<T>& v) {
    std::string s;
    std::getline(std::cin, s);
    std::string space = "[],'\"";
    std::stringstream ss;
    for (auto it : s) {
        if (std::find(space.begin(), space.end(), it) != space.end()) ss << ' ';
        else ss << it;
    }
    v.clear();
    T val;
    while (ss >> val) {
        v.push_back(val);
    }
}

template<typename T>
inline void __read__(std::vector<std::vector<T>>& v) {
    std::string s;
    std::getline(std::cin, s);
    std::string space = "[,'\"";
    std::stringstream ss;
    for (auto it : s) {
        if (it == ']') ss << '\n';
        else if (std::find(space.begin(), space.end(), it) != space.end()) ss << ' ';
        else ss << it;
    }
    v.clear();
    while (std::getline(ss, s)) {
        std::vector<T> tmp;
        std::stringstream ss2(s);
        T val;
        while (ss2 >> val) tmp.push_back(val);
        v.push_back(tmp);
    }
    v.pop_back();
}

inline void __read__(ListNode *&node) {
    std::vector<int> v;
    __read__(v);
    if (v.empty()) {
        node = nullptr;
        return;
    }
    node = new ListNode(v[0]);
    ListNode* dummy = node;
    for (int i = 1; i < (int) v.size(); ++i) {
        dummy->next = new ListNode(v[i]);
        dummy = dummy->next;
    }
}

inline void __read__(TreeNode *&node) {
    std::vector<std::string> v;
    __read__(v);
    if (v.empty() || v[0] == "null") {
        node = nullptr;
        return;
    }
    node = new TreeNode(std::stoi(v[0]));
    auto build = [&](auto&& self, TreeNode* node, int id = 1) -> void {
        if (id * 2 - 1 < (int) v.size() && v[id * 2 - 1] != "null") {
            node->left = new TreeNode(std::stoi(v[id * 2 - 1]));
            self(self, node->left, id * 2);
        }
        if (id * 2 < (int) v.size() && v[id * 2] != "null") {
            node->right = new TreeNode(std::stoi(v[id * 2]));
            self(self, node->right, id * 2 + 1);
        }
    };
    build(build, node);
}

template<typename... Args>
void read(Args&... args) {
    ( __read__(args), ... );
}

};

using namespace std;

#endif