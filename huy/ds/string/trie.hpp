#include <array>
#include <vector>
#include <functional>

#ifndef HUY_DS_STRING_TRIE
#define HUY_DS_STRING_TRIE
namespace huy {
namespace ds {

template <int N = 26>
class Trie {
public:
    struct Node {
        int exist, cnt;
        std::array<Node*, N> child;

        Node() : exist(0), cnt(0), child(nullptr) {}
    };

    Node* root;

    Trie() : root(new Node()) {}

    template <typename Tp>
    void insert(const std::vector<Tp>& key) {
        Node *p = root;
        for (auto i : key) {
            if (p->child[i] == nullptr) p->child[i] = new Node();
            p = p->child[i];
            p->cnt++;
        }
        p->exist++;
    }

    void insert(const std::string& key) {
        int n = static_cast<int>(key.size());
        std::vector<int> v(n);
        for (int i = 0; i < n; ++i) v[i] = key[i] - 'a';
        insert(v);
    }

    template <typename Tp>
    bool find(const std::vector<Tp>& key) const {
        const Node *p = root;
        for (auto i : key) {
            if (p->child[i] == nullptr) return false;
            p = p->child[i];
        }
        return p->exist > 0;
    }

    bool find(const std::string& key) const {
        int n = static_cast<int>(key.size());
        std::vector<int> v(n);
        for (int i = 0; i < n; ++i) v[i] = key[i] - 'a';
        return find(v);
    }

    template <typename Tp>
    bool starts_with(const std::vector<Tp>& prefix) const {
        const Node *p = root;
        for (auto i : prefix) {
            if (p->child[i] == nullptr) return false;
            p = p->child[idx];
        }
        return true;
    }

    bool starts_with(const std::string& prefix) const {
        int n = static_cast<int>(prefix.size());
        std::vector<int> v(n);
        for (int i = 0; i < n; ++i) v[i] = prefix[i] - 'a';
        return starts_with(v);
    }

    template <typename Tp>
    void remove(const std::vector<Tp>& key) {
        if (!find(key)) return;
        int n = static_cast<int>(key.size());
        std::function<bool(Node*, int)> work = [&](Node* p, int i) -> bool {
            if (i < n) {
                if (work(p->child[key[i]], i + 1)) p->child[i] = nullptr;
            } else {
                p->exist--;
            }
            if (p != root) {
                p->cnt--;
                if (p->cnt == 0) {
                    delete p;
                    return true;
                }
            }
            return false;
        };
        work(root, 0);
    }

    void remove(const std::string& key) {
        int n = static_cast<int>(key.size());
        std::vector<int> v(n);
        for (int i = 0; i < n; ++i) v[i] = key[i] - 'a';
        remove(v);
    }

    ~Trie() { clear(root); }

    void clear(Node* node) {
        if (!node) return;
        for (int i = 0; i < N; ++i) clear(node->child[i]);
        delete node;
    }
};

} // namespace ds
} // namespace huy
#endif