#include <vector>
#include <cassert>

#ifndef CP_DS_QUERY_FENWICK_TREE_3D
#define CP_DS_QUERY_FENWICK_TREE_3D
namespace cp {
namespace ds {

template <typename Tp>
class fenwick_tree_3d {
public:
    using value_type = Tp;

    fenwick_tree_3d() : n(0), m(0), p(0) {}

    fenwick_tree_3d(int n, int m, int p, const value_type& val = value_type()) {
        init(n, m, p, val);
    }

    template <typename T>
    fenwick_tree_3d(const std::vector<std::vector<std::vector<T>>>& data) {
        init(data);
    }

    void init(int n, int m, int p, const value_type& val = value_type()) {
        std::vector<std::vector<std::vector<value_type>>> data(n, std::vector<std::vector<value_type>>(m, std::vector<value_type>(p, val)));
        init(data);
    }

    template <typename T>
    void init(const std::vector<std::vector<std::vector<T>>>& data) {
        n = static_cast<int>(data.size());
        if (n == 0) {
            m = 0;
            p = 0;
            return;
        }

        m = static_cast<int>(data[0].size());
        if (m == 0) {
            p = 0;
            return;
        }
        
        p = static_cast<int>(data[0][0].size());

        tree.assign(n * m * p, value_type());
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                for (int k = 0; k < p; ++k) {
                    if (value_type(data[i][j][k]) != value_type()) {
                        add(i, j, k, value_type(data[i][j][k]));
                    }
                }
            }
        }
    }

    void add(int x, int y, int z, const value_type& val) {
        assert(0 <= x && x < n);
        assert(0 <= y && y < m);
        assert(0 <= z && z < p);

        for (int i = x + 1; i <= n; i += i & -i) {
            for (int j = y + 1; j <= m; j += j & -j) {
                for (int k = z + 1; k <= p; k += k & -k) {
                    tree[(i - 1) * m * p + (j - 1) * p + (k - 1)] += val;
                }
            }
        }
    }

    void modify(int x, int y, int z, const value_type& val) {
        assert(0 <= x && x < n);
        assert(0 <= y && y < m);
        assert(0 <= z && z < p);
        
        value_type cur = box_sum(x, y, z, x + 1, y + 1, z + 1);
        add(x, y, z, val - cur);
    }

    value_type point_sum(int x, int y, int z) const {
        assert(0 <= x && x < n);
        assert(0 <= y && y < m);
        assert(0 <= z && z < p);

        return box_sum(x, y, z, x + 1, y + 1, z + 1);
    }

    // range [xs, xe), [ys, ye), [zs, ze)
    value_type box_sum(int xs, int ys, int zs, int xe, int ye, int ze) const {
        assert(0 <= xs && xs <= xe && xe <= n);
        assert(0 <= ys && ys <= ye && ye <= m);
        assert(0 <= zs && zs <= ze && ze <= p);
        if (xs == xe || ys == ye || zs == ze) return value_type();
        
        value_type res = 0;
        res += prefix_sum(xe, ye, ze);
        res -= prefix_sum(xs, ye, ze);
        res -= prefix_sum(xe, ys, ze);
        res -= prefix_sum(xe, ye, zs);
        res += prefix_sum(xs, ys, ze);
        res += prefix_sum(xs, ye, zs);
        res += prefix_sum(xe, ys, zs);
        res -= prefix_sum(xs, ys, zs);
        return res;
    }

private:
    int n, m, p;
    std::vector<value_type> tree;

    value_type prefix_sum(int x, int y, int z) const {
        value_type ans = 0;
        for (int i = x; i > 0; i -= i & -i) {
            for (int j = y; j > 0; j -= j & -j) {
                for (int k = z; k > 0; k -= k & -k) {
                    ans += tree[(i - 1) * m * p + (j - 1) * p + (k - 1)];
                }
            }
        }
        return ans;
    }
};

} // namespace ds
} // namespace cp
#endif