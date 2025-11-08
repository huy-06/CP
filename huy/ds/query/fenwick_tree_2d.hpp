#include <vector>
#include <cassert>

#ifndef HUY_DS_QUERY_FENWICK_TREE_2D
#define HUY_DS_QUERY_FENWICK_TREE_2D
namespace huy {
namespace ds {

template <typename Tp>
class fenwick_tree_2d {
public:
    using value_type = Tp;

    fenwick_tree_2d(int n, int m)
        : n(n), m(m), bit((n + 1) * (m + 1), value_type()) {}

    void add(int x, int y, const value_type& v) {
        for (int i = x + 1; i <= n; i += i & -i)
            for (int j = y + 1; j <= m; j += j & -j)
                bit[idx(i, j)] += v;
    }

    value_type prefix_sum(int x, int y) const {
        value_type s = value_type();
        for (int i = x + 1; i > 0; i -= i & -i)
            for (int j = y + 1; j > 0; j -= j & -j)
                s += bit[idx(i, j)];
        return s;
    }

    value_type rect_sum(int x1, int y1, int x2, int y2) const {
        assert(0 <= x1 && x1 <= x2 && x2 <= n);
        assert(0 <= y1 && y1 <= y2 && y2 <= m);
        if (x1 == x2 || y1 == y2) return value_type();
        return prefix_sum(x2 - 1, y2 - 1) - prefix_sum(x1 - 1, y2 - 1) - prefix_sum(x2 - 1, y1 - 1) + prefix_sum(x1 - 1, y1 - 1);
    }
private:
    int n, m;
    std::vector<value_type> bit;

    int idx(int i, int j) const {
        return i * (m + 1) + j;
    }

};

} // namespace ds
} // namespace huy
#endif