#include <vector>
#include <cassert>

#ifndef CP_DS_QUERY_FENWICK_TREE
#define CP_DS_QUERY_FENWICK_TREE
namespace cp {
namespace ds {

/// @brief Tính tổng các phần tử trong mảng trong đoạn [l, r) và cập nhật giá trị tại vị trí p
/// @tparam Tp 
template <typename Tp>
class fenwick_tree {
public:
    using value_type = Tp;

    fenwick_tree() : n(0) {}

    fenwick_tree(int n, const value_type& val = value_type()) {
        init(n, val);
    }

    template <typename T>
    fenwick_tree(const std::vector<T>& data) {
        init(data);
    }

    void init(int n, const value_type& val = value_type()) {
        std::vector<value_type> data(n, val);
        init(data);
    }

    template <typename T>
    void init(const std::vector<T>& data) {
        n = static_cast<int>(data.size());
        tree.assign(n, value_type());
        
        for (int i = 0; i < n; ++i) {
            tree[i] = value_type(data[i]);
        }

        for (int i = 1; i <= n; ++i) {
            int j = i + (i & -i);
            if (j <= n) {
                tree[j - 1] += tree[i - 1];
            }
        }
    }

    void add(int p, const value_type& val) {
        assert(0 <= p && p < n);
        
        for (int i = p + 1; i <= n; i += i & -i) {
            tree[i - 1] += val;
        }
    }

    void modify(int p, const value_type& val) {
        assert(0 <= p && p < n);

        value_type cur = range_sum(p, p + 1);
        add(p, val - cur);
    }

    value_type point_sum(int p) const {
        assert(0 <= p && p < n);

        return prefix_sum(p + 1) - prefix_sum(p);
    }

    // range [l, r)
    value_type range_sum(int l, int r) const {
        assert(0 <= l && l <= r && r <= n);

        return prefix_sum(r) - prefix_sum(l);
    }

    /// @brief Tìm vị trí p nhỏ nhất sao cho prefix_sum(p) >= k
    int min_left(value_type k) const {
        if (k <= 0) return 0;

        int p = 1;
        while ((p << 1) <= n) p <<= 1;

        value_type cur = 0;
        int x = 0;
        for (int i = p; i > 0; i >>= 1) {
            if (x + i <= n && cur + tree[x + i - 1] < k) {
                cur += tree[x + i - 1];
                x += i;
            }
        }

        return x;
    }

    /// @brief  Tìm vị trí p lớn nhất sao cho prefix_sum(p) <= k
    int max_right(value_type k) const {
        int p = 1;
        while ((p << 1) <= n) p <<= 1;

        value_type cur = 0;
        int x = 0;
        for (int i = p; i > 0; i >>= 1) {
            if (x + i <= n && cur + tree[x + i - 1] <= k) {
                cur += tree[x + i - 1];
                x += i;
            }
        }

        return x;
    }

private:
    int n;
    std::vector<value_type> tree;

    value_type prefix_sum(int p) const {
        value_type ans = 0;
        for (int i = p; i > 0; i -= i & -i) {
            ans += tree[i - 1];
        }

        return ans;
    }
};

} // namespace ds
} // namespace cp
#endif