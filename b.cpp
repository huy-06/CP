/// @author Huy
/// @since  07:17:12 14/11/2025
#include "huy/init/lib.hpp"

#include <vector>
#include <cassert>
#include <numeric> // Dùng cho std::iota trong hàm init tối ưu

#ifndef HUY_DS_QUERY_FENWICK_TREE
#define HUY_DS_QUERY_FENWICK_TREE
namespace huy {
namespace ds {

template <typename Tp>
class fenwick_tree {
public:
    using value_type = Tp;

    fenwick_tree() : n(0) {}

    fenwick_tree(int n, const value_type& val = value_type()) {
        init(n, val);
    }

    fenwick_tree(const std::vector<value_type>& data) {
        init(data);
    }

    // SỬA LỖI: Triển khai hàm init(n, val)
    void init(int n, const value_type& val = value_type()) {
        this->n = n;
        tree.assign(n, val);
        // Nếu val khác 0, cần phải xây dựng cây.
        // Cách tối ưu O(n) để xây dựng cây.
        if (val != value_type()) {
            for (int i = 1; i <= n; ++i) {
                int j = i + (i & -i);
                if (j <= n) {
                    tree[j - 1] += tree[i - 1];
                }
            }
        }
    }

    // SỬA LỖI: Triển khai hàm init(data) với cách xây dựng tối ưu O(n)
    void init(const std::vector<value_type>& data) {
        n = data.size();
        tree = data;
        for (int i = 1; i <= n; ++i) {
            int j = i + (i & -i); // Tìm cha của i trong cây
            if (j <= n) {
                tree[j - 1] += tree[i - 1];
            }
        }
    }

    // Cập nhật giá trị tại vị trí p, tăng thêm val
    void add(int p, const value_type& val) {
        assert(0 <= p && p < n);
        for (int i = p + 1; i <= n; i += i & -i) {
            tree[i - 1] += val;
        }
    }

    // Đặt giá trị tại vị trí p thành val
    void set(int p, const value_type& val) {
        assert(0 <= p && p < n);
        value_type current_val = range_sum(p, p + 1);
        add(p, val - current_val);
    }
    
    // Tính tổng trên đoạn [0, p-1]
    value_type prefix_sum(int p) const {
        assert(0 <= p && p <= n);
        value_type ans = 0;
        for (int i = p; i > 0; i -= i & -i) {
            ans += tree[i - 1];
        }
        return ans;
    }

    // Tính tổng trên đoạn [l, r-1]
    value_type range_sum(int l, int r) const {
        assert(0 <= l && l <= r && r <= n);
        return prefix_sum(r) - prefix_sum(l);
    }

    /**
     * Tìm vị trí p nhỏ nhất sao cho prefix_sum(p) >= k
     * Nếu không có vị trí nào, trả về n.
     * Yêu cầu các phần tử trong cây không âm.
     */
    int min_left(value_type k) const {
        if (k <= 0) return 0;
        value_type cur = 0;
        int x = 0;
        // Tìm lũy thừa lớn nhất của 2 không vượt quá n
        int p = 1;
        while ((p << 1) <= n) p <<= 1;

        for (int i = p; i > 0; i >>= 1) {
            if (x + i <= n && cur + tree[x + i - 1] < k) { // SỬA LỖI: Dùng tree[x + i - 1]
                cur += tree[x + i - 1]; // SỬA LỖI: Dùng tree[x + i - 1]
                x += i;
            }
        }
        return x;
    }

    /**
     * Tìm vị trí p lớn nhất sao cho prefix_sum(p) <= k
     * Yêu cầu các phần tử trong cây không âm.
     */
    int max_right(value_type k) const {
        value_type cur = 0;
        int x = 0;
        int p = 1;
        while ((p << 1) <= n) p <<= 1;

        for (int i = p; i > 0; i >>= 1) {
            if (x + i <= n && cur + tree[x + i - 1] <= k) { // SỬA LỖI: Dùng tree[x + i - 1]
                cur += tree[x + i - 1]; // SỬA LỖI: Dùng tree[x + i - 1]
                x += i;
            }
        }
        return x;
    }

private:
    int n;
    std::vector<value_type> tree;
};

} // namespace ds
} // namespace huy
#endif

signed main() {
    //! std::ios_base::sync_with_stdio(false);
    //! std::cin.tie(nullptr);
    //! std::cout.tie(nullptr);

    /* Code here */

    return 0;
}