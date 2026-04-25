/// @author Thuw
/// @since  16:30:05 25/04/2026
#include "cp/init/library.hpp"

#include <vector>
#include <cassert>

#ifndef CP_DS_QUERY_SEGMENT_TREE
#define CP_DS_QUERY_SEGMENT_TREE
namespace cp {
namespace ds {

/**
 * @brief Cấu trúc dữ liệu Segment Tree (giống AtCoder Library).
 * 
 * @tparam Tp Kiểu dữ liệu của các phần tử trong Segment Tree.
 * @tparam op Con trỏ hàm định nghĩa phép toán kết hợp 2 phần tử (ví dụ: cộng, max, min).
 *            Hàm này phải thỏa mãn tính kết hợp: op(a, op(b, c)) == op(op(a, b), c).
 * @tparam e Con trỏ hàm trả về phần tử đơn vị (Identity element) của phép toán `op`.
 *           Phần tử e phải thỏa mãn: op(a, e()) == op(e(), a) == a với mọi a.
 * 
 * @par Ví dụ sử dụng (Tính tổng - Range Sum Query):
 * @code
 * // 1. Định nghĩa kiểu dữ liệu (Tp)
 * using Tp = long long;
 * 
 * // 2. Định nghĩa hàm kết hợp (op)
 * Tp op(Tp a, Tp b) { return a + b; }
 * 
 * // 3. Định nghĩa hàm trả về phần tử đơn vị (e)
 * Tp e() { return 0; } // Phần tử đơn vị của phép cộng là 0
 * 
 * // 4. Khởi tạo cây
 * cp::ds::segment_tree<Tp, op, e> seg(10); // Cây có 10 phần tử
 * @endcode
 */
template <class Tp, Tp (*op)(Tp, Tp), Tp (*e)()>
class segment_tree {
public:
    using value_type = Tp;
    
    /// @brief Khởi tạo cây rỗng
    segment_tree() : n(0), size(0) {}

    /// @brief Khởi tạo cây với n phần tử, giá trị mặc định là val (nếu không truyền sẽ dùng e())
    explicit segment_tree(int n, const value_type& val = e()) {
        init(n, val);
    }

    /// @brief Khởi tạo cây từ một mảng có sẵn (vector)
    explicit segment_tree(const std::vector<value_type>& data) {
        init(data);
    }

    /// @brief Cấp phát lại cây với n phần tử, giá trị mặc định là val
    void init(int n, const value_type& val = e()) {
        std::vector<value_type> data(n, val);
        init(data);
    }

    /// @brief Xây dựng lại cây từ một mảng có sẵn
    void init(const std::vector<value_type>& data) {
        n = static_cast<int>(data.size());
        for (size = 1; size < n; size <<= 1);
        tree.assign(2 * size, e()); // Sử dụng phần tử đơn vị e() làm mặc định
        for (int i = 0; i < n; ++i) tree[size + i] = data[i];
        for (int i = size - 1; i > 0; --i) pull(i);
    }

    /// @brief Gán giá trị val cho phần tử tại vị trí p (0-indexed)
    void modify(int p, const value_type& val) {
        assert(0 <= p && p < n);
        p += size;
        tree[p] = val;
        for (p >>= 1; p > 0; p >>= 1) pull(p);
    }

    /// @brief Lấy giá trị tại vị trí p (0-indexed)
    value_type point_query(int p) const {
        assert(0 <= p && p < n);
        return tree[p + size];
    }

    /// @brief Truy vấn trong nửa khoảng[l, r) (0-indexed)
    value_type range_query(int l, int r) const {
        assert(0 <= l && l <= r && r <= n);
        value_type sml = e(); // Accumulator trái
        value_type smr = e(); // Accumulator phải
        l += size;
        r += size;
        while (l < r) {
            if (l & 1) sml = op(sml, tree[l++]);
            if (r & 1) smr = op(tree[--r], smr);
            l >>= 1;
            r >>= 1;
        }
        return op(sml, smr); // Gộp kết quả trái và phải
    }

    /// @brief Truy vấn trên toàn bộ mảng[0, n)
    value_type all_query() const {
        return tree[1];
    }

    /// @brief Tìm kiếm nhị phân trên Segment Tree (Trái sang Phải).
    /// Tìm r lớn nhất thỏa mãn hàm pred(op(a[l..r-1])) trả về true.
    template <typename F>
    int max_right(int l, F&& pred) const {
        assert(0 <= l && l <= n);
        assert(pred(e()));
        if (l == n) return n;
        value_type sm = e();
        l += size;
        do {
            while ((l & 1) == 0) l >>= 1;
            if (!pred(op(sm, tree[l]))) {
                while (l < size) {
                    l <<= 1;
                    if (pred(op(sm, tree[l]))) {
                        sm = op(sm, tree[l]);
                        l++;
                    }
                }
                return l - size;
            }
            sm = op(sm, tree[l]);
            l++;
        } while ((l & -l) != l);
        return n;
    }

    /// @brief Tìm kiếm nhị phân trên Segment Tree (Phải sang Trái).
    /// Tìm l nhỏ nhất thỏa mãn hàm pred(op(a[l..r-1])) trả về true.
    template <typename F>
    int min_left(int r, F&& pred) const {
        assert(0 <= r && r <= n);
        assert(pred(e()));
        if (r == 0) return 0;
        value_type sm = e();
        r += size;
        do {
            r--;
            while (r > 1 && (r & 1)) r >>= 1;
            if (!pred(op(tree[r], sm))) {
                while (r < size) {
                    r = (r << 1) | 1;
                    if (pred(op(tree[r], sm))) {
                        sm = op(tree[r], sm);
                        r--;
                    }
                }
                return r + 1 - size;
            }
            sm = op(tree[r], sm);
        } while ((r & -r) != r);
        return 0;
    }

private:
    int n, size;
    std::vector<value_type> tree;

    /// @brief Cập nhật nút i từ 2 nút con
    void pull(int i) {
        tree[i] = op(tree[i << 1], tree[(i << 1) | 1]);
    }
};

} // namespace ds
} // namespace cp
#endif

signed main() {
    //! std::ios_base::sync_with_stdio(false);
    //! std::cin.tie(nullptr);
    //! std::cout.tie(nullptr);

    int n;
    std::cin >> n;
    
    std::vector<int> a(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> a[i];
    }

    cp::ds::segment_tree<

    return 0;
}