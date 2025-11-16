#ifndef CP_DS_TREAP
#define CP_DS_TREAP
namespace cp {
namespace ds {

template<typename Tp>
class Treap {
public:
    Treap() : root(nullptr) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
    }

    explicit Treap(int n) : Treap() {
        for (int i = 0; i < n; ++i)
            push_back(Tp());
    }

    Treap(int n, const Tp& val) : Treap() {
        for (int i = 0; i < n; ++i)
            push_back(val);
    }

    Treap(const Treap& o) : root(clone(o.root)) {}

    Treap& operator=(const Treap& o) {
        if (this != &o) {
        destroy(root);
        root = clone(o.root);
        }
        return *this;
    }

    Treap(Treap&& o) noexcept : root(o.root) {
        o.root = nullptr;
    }

    Treap& operator=(Treap&& o) noexcept {
        if (this != &o) {
            destroy(root);
            root = o.root;
            o.root = nullptr;
        }
        return *this;
    }

    ~Treap() {
        destroy(root);
    }

    int size() const {
        return getSize(root);
    }

    bool empty() const {
        return size() == 0;
    }

    Tp& operator[](int idx) {
        assert(0 <= idx && idx < size());
        return at(root, idx);
    }

    const Tp& operator[](int idx) const {
        return const_cast<Treap*>(this)->operator[](idx);
    }

    void push_back(const Tp& val) {
        root = insertNode(root, size(), val);
    }

    void push_back(const Treap& other) {
        root = merge(root, clone(other.root));
    }

    void push_back(Treap&& other) {
        root = merge(root, other.root);
        other.root = nullptr;
    }

    void pop_back() {
        if (!empty())
            root = eraseNode(root, size() - 1);
    }

    void insert(int pos, const Tp& val) {
        assert(0 <= pos && pos <= size());
        root = insertNode(root, pos, val);
    }

    void insert(int pos, const Treap& other) {
        assert(0 <= pos && pos <= size());
        Node *a, *b;
        split(root, pos, a, b);
        Node* c = clone(other.root);
        root = merge(merge(a, c), b);
    }
    
    void insert(int pos, Treap&& other) {
        assert(0 <= pos && pos <= size());
        Node *a, *b;
        split(root, pos, a, b);
        root = merge(merge(a, other.root), b);
        other.root = nullptr;
    }

    void erase(int pos) {
        assert(0 <= pos && pos < size());
        root = eraseNode(root, pos);
    }

    void erase(int l, int r) {
        assert(0 <= l && l <= r && r <= size());
        root = eraseRange(root, l, r);
    }

    struct Iterator {
        Treap* tv;
        int idx;

        Iterator(Treap* t, int i) : tv(t), idx(i) {}

        Tp& operator*() const {
            return (*tv)[idx];
        }

        Iterator& operator++() {
            ++idx;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++idx;
            return tmp;
        }

        Iterator& operator--() {
            --idx;
            return *this;
        }

        Iterator operator--(int) {
            Iterator tmp = *this;
            --idx;
            return tmp;
        }

        bool operator==(const Iterator& o) const {
            return tv == o.tv && idx == o.idx;
        }

        bool operator!=(const Iterator& o) const {
            return !(*this == o);
        }

        Iterator operator+(int n) const {
            return Iterator(tv, idx + n);
        }

        Iterator operator-(int n) const {
            return Iterator(tv, idx - n);
        }

        int operator-(const Iterator& o) const {
            return idx - o.idx;
        }
    };

    Iterator begin() {
        return Iterator(this, 0);
    }

    Iterator end() {
        return Iterator(this, size());
    }

private:
    struct Node {
        Tp    val;
        int   prio;
        int   sz;
        Node* l;
        Node* r;

        Node(const Tp& v) : val(v), prio(std::rand()), sz(1), l(nullptr), r(nullptr) {}
    };

    Node* root;

    static Node* clone(Node* t) {
        if (!t) return nullptr;
        Node* p = new Node(t->val);
        p->prio = t->prio;
        p->l    = clone(t->l);
        p->r    = clone(t->r);
        update(p);
        return p;
    }

    static int getSize(Node* t) {
        return t ? t->sz : 0;
    }

    static void update(Node* t) {
        if (t)
            t->sz = 1 + getSize(t->l) + getSize(t->r);
    }

    static void split(Node* t, int k, Node*& a, Node*& b) {
        if (!t) {
            a = b = nullptr;
        } else if (getSize(t->l) < k) {
            split(t->r, k - getSize(t->l) - 1, t->r, b);
            a = t;
            update(a);
        } else {
            split(t->l, k, a, t->l);
            b = t;
            update(b);
        }
    }

    static Node* merge(Node* a, Node* b) {
        if (!a || !b)
            return a ? a : b;

        if (a->prio > b->prio) {
            a->r = merge(a->r, b);
            update(a);
            return a;
        } else {
            b->l = merge(a, b->l);
            update(b);
            return b;
        }
    }

    static Node* insertNode(Node* t, int pos, const Tp& val) {
        Node* newNode = new Node(val);
        Node *a, *b;
        split(t, pos, a, b);
        return merge(merge(a, newNode), b);
    }

    static Node* eraseNode(Node* t, int pos) {
        Node *a, *b, *c;
        split(t, pos, a, b);
        split(b, 1, b, c);
        delete b;
        return merge(a, c);
    }

    static Node* eraseRange(Node* t, int l, int r) {
        Node *a, *b, *c;
        split(t, l, a, b);
        split(b, r - l, b, c);
        destroy(b);
        return merge(a, c);
    }

    static Tp& at(Node* t, int k) {
        int leftSz = getSize(t->l);
        if (k < leftSz)
            return at(t->l, k);
        else if (k == leftSz)
            return t->val;
        else
            return at(t->r, k - leftSz - 1);
    }

    static void destroy(Node* t) {
        if (!t)
            return;
        destroy(t->l);
        destroy(t->r);
        delete t;
    }
};

} // namespace ds
} // namespace cp
#endif