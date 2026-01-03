#include <vector>
#include <cassert>

#ifndef CP_DS_QUERY_SEGMENT_TREE_2D
#define CP_DS_QUERY_SEGMENT_TREE_2D

namespace cp {
namespace ds {

template <typename Tp>
class segment_tree_2d {
public:
    using value_type = Tp;

    segment_tree_2d() : n(0), m(0), size_n(0), size_m(0) {}

    segment_tree_2d(int n, int m, const value_type& val = value_type()) {
        init(n, m, val);
    }

    segment_tree_2d(const std::vector<std::vector<value_type>>& data) {
        init(data);
    }

    void init(int n, int m, const value_type& val = value_type()) {
        std::vector<std::vector<value_type>> data(n, std::vector<value_type>(m, val));
        init(data);
    }

    // O(n * m)
    void init(const std::vector<std::vector<value_type>>& data) {
        if (data.empty() || data[0].empty()) return;
        n = static_cast<int>(data.size());
        m = static_cast<int>(data[0].size());

        for (size_n = 1; size_n < n; size_n <<= 1);
        for (size_m = 1; size_m < m; size_m <<= 1);

        tree.assign(4 * size_n * size_m, value_type());

        for (int i = 0; i < n; ++i) {
            int row_idx = size_n + i;
            for (int j = 0; j < m; ++j) {
                tree[id(row_idx, size_m + j)] = data[i][j];
            }
            for (int j = size_m - 1; j > 0; --j) {
                pull_y(row_idx, j);
            }
        }

        for (int i = size_n - 1; i > 0; --i) {
            for (int j = 1; j < 2 * size_m; ++j) {
                pull_x(i, j);
            }
        }
    }

    // O(log(n) * log(m))
    void modify(int x, int y, const value_type& val) {
        assert(0 <= x && x < n && 0 <= y && y < m);
        x += size_n;
        y += size_m;

        tree[id(x, y)] = val;
        for (int j = y >> 1; j > 0; j >>= 1) {
            pull_y(x, j);
        }

        for (int i = x >> 1; i > 0; i >>= 1) {
            pull_x(i, y);

            for (int j = y >> 1; j > 0; j >>= 1) {
                pull_y(i, j); 
            }
        }
    }

    value_type point_query(int x, int y) const {
        assert(0 <= x && x < n && 0 <= y && y < m);
        return tree[id(x + size_n, y + size_m)];
    }

    // O(log(n) * log(m))
    value_type rect_query(int xs, int ys, int xe, int ye) const {
        assert(0 <= xs && xs <= xe && xe <= n);
        assert(0 <= ys && ys <= ye && ye <= m);

        value_type res = value_type();
        int lx = xs + size_n;
        int rx = xe + size_n;

        while (lx < rx) {
            if (lx & 1) {
                res = res + query_y(lx++, ys, ye);
            }
            if (rx & 1) {
                res = res + query_y(--rx, ys, ye);
            }
            lx >>= 1;
            rx >>= 1;
        }
        return res;
    }

    value_type all_query() const {
        return tree[id(1, 1)];
    }

private:
    int n, m;
    int size_n, size_m;
    std::vector<value_type> tree;

    inline int id(int x, int y) const {
        return x * (2 * size_m) + y;
    }

    void pull_y(int x, int y) {
        tree[id(x, y)] = tree[id(x, y << 1)] + tree[id(x, (y << 1) | 1)];
    }

    void pull_x(int x, int y) {
        tree[id(x, y)] = tree[id(x << 1, y)] + tree[id((x << 1) | 1, y)];
    }

    value_type query_y(int row_idx, int ys, int ye) const {
        value_type sml = value_type();
        value_type smr = value_type();
        int ly = ys + size_m;
        int ry = ye + size_m;

        while (ly < ry) {
            if (ly & 1) sml = sml + tree[id(row_idx, ly++)];
            if (ry & 1) smr = tree[id(row_idx, --ry)] + smr;
            ly >>= 1;
            ry >>= 1;
        }
        return sml + smr;
    }
};

} // namespace ds
} // namespace cp
#endif
