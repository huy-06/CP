#include <vector>
#include <memory>
#include <functional>
#include "../../alg/utils/random.hpp"

#ifndef CP_DS_TREAP
#define CP_DS_TREAP
namespace cp {
namespace ds {

template <typename Tp>
class treap_node {
public:
    using value_type = Tp;

    value_type* l = nullptr;
    value_type* r = nullptr;
    int prio;
    int siz = 1;

    treap_node() : prio(alg::random()) {}

    void update_size() {
        siz = 1;
        if (l) siz += l->siz;
        if (r) siz += r->siz;
    }

    static int size(value_type* t) {
        return t ? t->siz : 0;
    }
};

template <typename Tp>
class treap {
public:
    using value_type = Tp;

    // tách thành 2 cây con l, r. cây l chứa có phần tử < k
    static void split(value_type* t, int k, value_type*& l, value_type*& r) {
        if (!t) {
            l = r = nullptr;
            return;
        }
        t->push();
        int cur_key = treap_node<value_type>::size(t->l);
        if (cur_key < k) {
            split(t->r, k - cur_key - 1, t->r, r);
            l = t;
        } else {
            split(t->l, k, l, t->l);
            r = t;
        }
        t->pull();
    }

    static void merge(value_type*& t, value_type* l, value_type* r) {
        if (!l || !r) {
            t = l ? l : r;
        } else {
            l->push();
            r->push();
            if (l->prio > r->prio) {
                merge(l->r, l->r, r);
                t = l;
            } else {
                merge(r->l, l, r->l);
                t = r;
            }
            t->pull();
        }
    }

    static void insert(value_type*& root, int i, value_type* node) {
        value_type *l, *r;
        split(root, i, l, r);
        merge(root, l, node);
        merge(root, root, r);
    }

    static void erase(value_type*& root, int i) {
        value_type *l, *m, *r;
        split(root, i, l, r);
        split(r, 1, m, r);
        merge(root, l, r);
        delete m;
    }

    static value_type* get_kth(value_type* t, int k) {
        if (!t) return nullptr;
        t->push();
        int cur = treap_node<value_type>::size(t->l);
        if (cur == k) return t;
        if (cur > k) return get_kth(t->l, k);
        return get_kth(t->r, k - cur - 1);
    }

    static void traversal(value_type* t, std::function<void(value_type*)> fun) {
        if (!t) return;
        t->push();
        traversal(t->l, fun);
        fun(t);
        traversal(t->r, fun);
    }
};

} // namespace ds
} // namespace cp
#endif