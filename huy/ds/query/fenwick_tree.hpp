#include <vector>
#include <cassert>

#ifndef HUY_DS_QUERY_FENWICK_TREE
#define HUY_DS_QUERY_FENWICK_TREE
namespace huy {
namespace ds {

template <typename Tp>
class fenwick_tree {
public:
    using value_type = Tp;

    fenwick_tree(int n = 0) {
        init(n);
    }

    void init(int n) {
        this->n = n;
        bit.assign(n + 1, value_type());
    }

    void add(int id, const value_type& v) {
        assert(0 <= id && id < n);
        for (int i = id + 1; i <= n; i += i & -i) {
            bit[i - 1] += v;
        }
    }

    value_type prefix_sum(int id) const {
        assert(0 <= id && id <= n);
        value_type ans = 0; 
        for (int i = id; i > 0; i -= i & -i) {
            ans += bit[i - 1];
        }
        return ans;
    }

    value_type range_sum(int l, int r) const {
        assert(0 <= l && l <= r && r <= n);
        return prefix_sum(r) - prefix_sum(l);
    }

    int min_left(const value_type& k) const {
        if (k <= 0) return 0;
        value_type cur = 0;
        int x = 0, p = 1;
        while ((p << 1) <= n) p <<= 1;
        for (int i = p; i; i >>= 1) {
            int nx = x + i;
            if (nx <= n && cur + bit[nx] < k) {
                x = nx;
                cur += bit[nx];
            }
        }
        int res = x + 1;
        if (res > n) return n;
        return res;
    }

    int max_right(const value_type& k) const {
        value_type cur = 0;
        int x = 0, p = 1;
        while ((p << 1) <= n) p <<= 1;
        for (int i = p; i; i >>= 1) {
            int nx = x + i;
            if (nx <= n && cur + bit[nx] <= k) {
                x = nx;
                cur += bit[nx];
            }
        }
        return x;
    }

private:
    int n;
    std::vector<value_type> bit;
};

} // namespace ds
} // namespace huy
#endif