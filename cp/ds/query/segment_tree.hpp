#include <vector>
#include <cassert>

#ifndef CP_DS_QUERY_SEGMENT_TREE
#define CP_DS_QUERY_SEGMENT_TREE
namespace cp {
namespace ds {

/**
 * @param Tp cách dùng Tp.
 * 
 * sturct info {
 *      int s = 0; // phải gán cho một giá trị nào đó!
 * };
 * 
 * info operator+(const info& a, const info& b) {
 *      info res;
 *      res.s = a.s + b.s;
 *      return res;
 * }
 */
template <typename Tp>
class segment_tree {
public:
    using value_type = Tp;
    
    segment_tree() : n(0), size(0) {}

    segment_tree(int n, const value_type& val = value_type()) {
        init(n, val);
    }

    segment_tree(const std::vector<value_type>& data) {
        init(data);
    }

    void init(int n, const value_type& val = value_type()) {
        std::vector<value_type> data(n, val);
        init(data);
    }

    void init(const std::vector<value_type>& data) {
        n = static_cast<int>(data.size());
        for (size = 1; size < n; size <<= 1);
        tree.assign(2 * size, value_type());
        for (int i = 0; i < n; ++i) tree[size + i] = data[i];
        for (int i = size - 1; i > 0; --i) pull(i);
    }

    void modify(int p, const value_type& val) {
        assert(0 <= p && p < n);
        p += size;
        tree[p] = val;
        for (p >>= 1; p > 0; p >>= 1) pull(p);
    }

    value_type point_query(int p) const {
        assert(0 <= p && p < n);
        return tree[p + size];
    }

    /// @note range [l, r)
    value_type range_query(int l, int r) const {
        assert(0 <= l && l <= r && r <= n);
        value_type sml = value_type();
        value_type smr = value_type();
        l += size;
        r += size;
        while (l < r) {
            if (l & 1) sml = sml + tree[l++];
            if (r & 1) smr = tree[--r] + smr;
            l >>= 1;
            r >>= 1;
        }
        return sml + smr;
    }

    value_type all_query() const {
        return tree[1];
    }

    template <typename F>
    int max_right(int l, F&& pred) const {
        assert(0 <= l && l <= n);
        if (l == n) return n;
        value_type sm = value_type();
        l += size;
        do {
            while ((l & 1) == 0) l >>= 1;
            if (!pred(sm + tree[l])) {
                while (l < size) {
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
    int min_left(int r, F&& pred) const {
        assert(0 <= r && r <= n);
        if (r == 0) return 0;
        value_type sm = value_type();
        r += size;
        do {
            --r;
            while (r > 1 && (r & 1)) r >>= 1;
            if (!pred(tree[r] + sm)) {
                while (r < size) {
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
    int n, size;
    std::vector<value_type> tree;

    void pull(int i) {
        tree[i] = tree[i << 1] + tree[(i << 1) | 1];
    }
};

} // namespace ds
} // namespace cp
#endif