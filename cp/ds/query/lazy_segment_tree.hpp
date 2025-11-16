
#include <vector>
#include <cassert>

#ifndef CP_DS_QUERY_LAZY_SEGMENT_TREE
#define CP_DS_QUERY_LAZY_SEGMENT_TREE
namespace cp {
namespace ds {

template <typename Tp, typename Tg>
class lazy_segment_tree {
public:
    using value_type = Tp;
    using value_tag  = Tg;

    lazy_segment_tree() : n(0), size(0), height(0) {}

    lazy_segment_tree(int n, const value_type& val = value_type()) {
        init(n, val);
    }

    lazy_segment_tree(const std::vector<value_type>& data) {
        init(data);
    }

    void init(int n, const value_type& val = value_type()) {
        std::vector<value_type> data(n, val);
        init(data);
    }

    void init(const std::vector<value_type>& data) {
        n = static_cast<int>(data.size());
        for (size = 1, height = 0; size < n; size <<= 1, ++height);
        tree.assign(size << 1, value_type());
        lazy.assign(size << 1, value_tag());
        for (int i = 0; i < this->n; ++i) tree[size + i] = data[i];
        for (int i = size - 1; i > 0; --i) pull(i);
    }

    void modify(int p, const value_type& val) {
        assert(0 <= p && p < n);
        p += size;
        for (int i = height; i >= 1; --i) push(p >> i);
        tree[p] = val;
        for (int i = 1; i <= height; ++i) pull(p >> i);
    }

    void range_apply(int l, int r, const value_tag& t) {
        assert(0 <= l && l <= r && r <= n);
        l += size;
        r += size;
        for (int i = height; i >= 1; --i) {
            if (((l >> i) << i) != l) push(l >> i);
            if (((r >> i) << i) != r) push((r - 1) >> i);
        }
        for (int a = l, b = r; a < b; a >>= 1, b >>= 1) {
            if (a & 1) apply(a++, t);
            if (b & 1) apply(--b, t);
        }
        for (int i = 1; i <= height; ++i) {
            if (((l >> i) << i) != l) pull(l >> i);
            if (((r >> i) << i) != r) pull((r - 1) >> i);
        }
    }

    value_type point_query(int p) {
        assert(0 <= p && p < n);
        p += size;
        for (int i = height; i >= 1; --i) push(p >> i);
        return tree[p];
    }

    value_type range_query(int l, int r) {
        assert(0 <= l && l <= r && r <= n);
        value_type sml = value_type()
        value_type smr = value_type();
        l += size;
        r += size;
        for (int i = height; i >= 1; --i) {
            if (((l >> i) << i) != l) push(l >> i);
            if (((r >> i) << i) != r) push((r - 1) >> i);
        }
        while (l < r) {
            if (l & 1) sml = sml + tree[l++];
            if (r & 1) smr = tree[--r] + smr;
            l >>= 1; r >>= 1;
        }
        return sml + smr;
    }

    value_type all_query() const {
        return tree[1];
    }

    template <typename F>
    int max_right(int l, F&& pred) {
        assert(0 <= l && l <= n);
        if (l == n) return n;
        l += size;
        for (int i = height; i >= 1; --i) push(l >> i);
        value_type sm = value_type();
        do {
            while ((l & 1) == 0) l >>= 1;
            if (!pred(sm + tree[l])) {
                while (l < size) {
                    push(l);
                    l <<= 1;
                    if (pred(sm + tree[l])) sm = sm + tree[l++];
                }
                return l - size;
            }
            sm = sm + tree[l++];
        } while ((l & -l) != l);
        return n;
    }

    template <typename F>
    int min_left(int r, F&& pred) {
        assert(0 <= r && r <= n);
        if (r == 0) return 0;
        r += size;
        for (int i = height; i >= 1; --i) push((r - 1) >> i);
        value_type sm = value_type();
        do {
            --r;
            while (r > 1 && (r & 1)) r >>= 1;
            if (!pred(tree[r] + sm)) {
                while (r < size) {
                    push(r);
                    r = (r << 1) | 1;
                    if (pred(tree[r] + sm)) sm = tree[r--] + sm;
                }
                return r + 1 - size;
            }
            sm = tree[r] + sm;
        } while ((r & -r) != r);
        return 0;
    }

private:
    int n, size, height;
    std::vector<value_type> tree;
    std::vector<value_tag>  lazy;

    void pull(int i) {
        tree[i] = tree[i << 1] + tree[(i << 1) | 1];
    }

    void apply(int i, const value_tag& t) {
        tree[i].apply(t);
        lazy[i].apply(t);
    }

    void push(int i) {
       apply(i << 1, lazy[i]);
       apply((i << 1) | 1, lazy[i]);
       lazy[i] = value_tag();
    }
};

} // namespace ds
} // namespace cp
#endif