#include <vector>
#include <cassert>

#ifndef CP_DS_QUERY_FENWICK_TREE_2D
#define CP_DS_QUERY_FENWICK_TREE_2D
namespace cp {
namespace ds {

template <typename Tp>
class fenwick_tree_2d {
public:
    using value_type = Tp;

    fenwick_tree_2d() : n(0), m(0) {}

    fenwick_tree_2d(int n, int m, const value_type& val = value_type()) {
        init(n, m, val);
    }

    template <typename T>
    fenwick_tree_2d(const std::vector<std::vector<T>>& data) {
        init(data);
    }

    void init(int n, int m, const value_type& val = value_type()) {
        std::vector<std::vector<value_type>> data(n, std::vector<value_type>(m, val));
        init(data);
    }

    template <typename T>
    void init(const std::vector<std::vector<T>>& data) {
        n = static_cast<int>(data.size());
        if (n == 0) {
            m = 0;
            return;
        }
        
        m = static_cast<int>(data[0].size());

        tree.assign(n * m, value_type());
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                if (value_type(data[i][j]) != value_type()) {
                    add(i, j, value_type(data[i][j]));
                }
            }
        }
    }

    void add(int x, int y, const value_type& val) {
        assert(0 <= x && x < n);
        assert(0 <= y && y < m);

        for (int i = x + 1; i <= n; i += i & -i) {
            for (int j = y + 1; j <= m; j += j & -j) {
                tree[(i - 1) * m + j - 1] += val;
            }
        }
    }

    void modify(int x, int y, const value_type& val) {
        assert(0 <= x && x < n);
        assert(0 <= y && y < m);
        
        value_type cur = rect_sum(x, y, x + 1, y + 1);
        add(x, y, val - cur);
    }

    value_type point_sum(int x, int y) const {
        assert(0 <= x && x < n);
        assert(0 <= y && y < m);

        return rect_sum(x, y, x + 1, y + 1);
    }

    // range [xs, xe), [ys, ye)
    value_type rect_sum(int xs, int ys, int xe, int ye) const {
        assert(0 <= xs && xs <= xe && xe <= n);
        assert(0 <= ys && ys <= ye && ye <= m);
        if (xs == xe || ys == ye) return value_type();
        return prefix_sum(xe, ye) - prefix_sum(xs, ye) - prefix_sum(xe, ys) + prefix_sum(xs, ys);
    }

private:
    int n, m;
    std::vector<value_type> tree;

    value_type prefix_sum(int x, int y) const {
        value_type ans = 0;
        for (int i = x; i > 0; i -= i & -i) {
            for (int j = y; j > 0; j -= j & -j) {
                ans += tree[(i - 1) * m + j - 1];
            }
        }
        return ans;
    }
};

} // namespace ds
} // namespace cp
#endif