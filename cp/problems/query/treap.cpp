/// @author Huy
/// @since  16:04:29 29/11/2025
/// @link   https://www.spoj.com/problems/TWIST/
#include "cp/init/lib.hpp"

struct node {
    int val;
    int pri;
    int siz;
    bool rev;
    node* l;
    node *r;

    node(int v) 
        : val(v), pri(rand()), siz(1), rev(false), l(nullptr), r(nullptr) {}
};

int get_size(node* t) {
    return t ? t->siz : 0;
}

void pull(node* t) {
    if (t) {
        t->siz = 1 + get_size(t->l) + get_size(t->r);
    }
}

void push(node* t) {
    if (t && t->rev) {
        t->rev = false;
        std::swap(t->l, t->r);
        if (t->l) t->l->rev ^= true;
        if (t->r) t->r->rev ^= true;
    }
}

void split(node* t, int k, node*& l, node*& r) {
    if (!t) {
        l = r = nullptr;
        return;
    }

    push(t);

    int i = get_size(t->l) + 1;
    if (i <= k) {
        split(t->r, k - i, t->r, r);
        l = t;
    } else {
        split(t->l, k, l, t->l);
        r = t;
    }

    pull(t);
}

void merge(node*& t, node* l, node* r) {
    push(l);
    push(r);

    if (!l || !r) {
        t = l ? l : r;
    } else if (l->pri > r->pri) {
        merge(l->r, l->r, r);
        t = l;
    } else {
        merge(r->l, l, r->l);
        t = r;
    }

    pull(t);
}

void reverse_range(node*& root, int l, int r) {
    node *t1, *t2, *t3;

    split(root, r, t2, t3);
    split(t2, l - 1, t1, t2);

    if (t2) t2->rev ^= true;

    merge(root, t1, t2);
    merge(root, root, t3);
}

void print_tree(node* t) {
    if (!t) return;

    push(t);
    print_tree(t->l);
    std::cout << t->val << ' ';
    print_tree(t->r);
}

signed main() {
    //! std::ios_base::sync_with_stdio(false);
    //! std::cin.tie(nullptr);
    //! std::cout.tie(nullptr);
    srand(time(nullptr));

    int n, m;
    std::cin >> n >> m;

    node* root = nullptr;

    for (int i = 1; i <= n; ++i) {
        merge(root, root, new node(i));
    }

    for (int i = 0; i < m; ++i) {
        int l, r;
        std::cin >> l >> r;
        reverse_range(root, l, r);
    }

    print_tree(root);
    std::cout << '\n';

    return 0;
}