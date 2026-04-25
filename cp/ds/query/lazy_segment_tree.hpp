#include <vector>
#include <cassert>

#ifndef CP_DS_QUERY_LAZY_SEGMENT_TREE
#define CP_DS_QUERY_LAZY_SEGMENT_TREE
namespace cp {
namespace ds {

/**
 * @brief Cấu trúc dữ liệu Lazy Segment Tree.
 * 
 * @tparam Tp Kiểu dữ liệu của các phần tử trong Segment Tree.
 * @tparam op Con trỏ hàm định nghĩa phép toán kết hợp 2 phần tử (ví dụ: cộng, max, min).
 *            Hàm này phải thỏa mãn tính kết hợp: op(a, op(b, c)) == op(op(a, b), c).
 * @tparam e Con trỏ hàm trả về phần tử đơn vị (Identity element) của phép toán `op`.
 *           Phần tử e phải thỏa mãn: op(a, e()) == op(e(), a) == a với mọi a.
 * @tparam Tg Kiểu dữ liệu của Lazy Tag.
 * @tparam mapping Hàm áp dụng tag Tg lên node Tp: Tp mapping(Tg f, Tp x)
 * @tparam composition Hàm gộp 2 tag Tg (tag mới f áp dụng lên tag cũ g): Tg composition(Tg f, Tg g)
 * @tparam id Hàm trả về tag rỗng (Identity Tag).
 * 
 * @par Ví dụ sử dụng (Bài toán Cộng đoạn - Truy vấn Tổng đoạn):
 * @code
 * // 1. Khai báo Node (Tp) và Lazy Tag (Tg)
 * struct Tp {
 *     long long sum;
 *     int len; // Lưu chiều dài đoạn để không cần truyền l, r
 * };
 * 
 * struct Tg {
 *     long long add;
 * };
 * 
 * // 2. Định nghĩa các thao tác cho Tp (Info)
 * Tp op(Tp a, Tp b) {
 *     return {a.sum + b.sum, a.len + b.len};
 * }
 * 
 * Tp e() {
 *     return {0, 0}; // Node đơn vị
 * }
 * 
 * // 3. Định nghĩa các thao tác cho Tg (Tag/Lazy)
 * Tp mapping(Tg f, Tp x) {
 *     // Giá trị mới = giá trị cũ + (lượng cộng thêm * độ dài đoạn)
 *     return {x.sum + f.add * x.len, x.len}; 
 * }
 * 
 * Tg composition(Tg f, Tg g) {
 *     // f là tag đến sau, g là tag có sẵn
 *     return {f.add + g.add};
 * }
 * 
 * Tg id() {
 *     return {0}; // Tag rỗng
 * }
 * 
 * // 4. Khởi tạo và sử dụng
 * // void solve() {
 * //     int n = 5;
 * //     std::vector<Tp> a(n, {0, 1}); // Khởi tạo n phần tử, mỗi lá có len = 1
 * //     
 * //     cp::ds::lazy_segment_tree<Tp, op, e, Tg, mapping, composition, id> seg(a);
 * //     
 * //     seg.range_apply(1, 4, {5});          // Cộng 5 vào đoạn[1, 4)
 * //     long long ans = seg.range_query(1, 4).sum; // Trả về tổng đoạn[1, 4)
 * // }
 * @endcode
 */
template <
    class Tp, Tp (*op)(Tp, Tp), Tp (*e)(),
    class Tg, Tp (*mapping)(Tg, Tp), Tg (*composition)(Tg, Tg), Tg (*id)()
>
class lazy_segment_tree {
public:
    using value_type = Tp;
    using lazy_type  = Tg;

    /// @brief Khởi tạo cây rỗng
    lazy_segment_tree() : n(0), size(0), height(0) {}

    /// @brief Khởi tạo cây với n phần tử, giá trị mặc định là val (nếu không truyền sẽ dùng e())
    explicit lazy_segment_tree(int n, const value_type& val = e()) {
        init(n, val);
    }

    /// @brief Khởi tạo cây từ một mảng có sẵn (vector)
    explicit lazy_segment_tree(const std::vector<value_type>& data) {
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
        for (size = 1, height = 0; size < n; size <<= 1, ++height);
        lazy.assign(2 * size, id());
        tree.assign(2 * size, e());
        for (int i = 0; i < n; ++i) tree[size + i] = data[i];
        for (int i = size - 1; i > 0; --i) pull(i);
    }

    /// @brief Gán giá trị val cho phần tử tại vị trí p
    void modify(int p, const value_type& val) {
        assert(0 <= p && p < n);
        p += size;
        for (int i = height; i >= 1; --i) push(p >> i);
        tree[p] = val;
        for (int i = 1; i <= height; ++i) pull(p >> i);
    }

    /// @brief Áp dụng tag t lên đoạn[l, r)
    void range_apply(int l, int r, const lazy_type& t) {
        assert(0 <= l && l <= r && r <= n);
        if (l == r) return;
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

    /// @brief Lấy giá trị tại vị trí p
    value_type point_query(int p) {
        assert(0 <= p && p < n);
        p += size;
        for (int i = height; i >= 1; --i) push(p >> i);
        return tree[p];
    }

    /// @brief Tính toán (gộp) kết quả trên đoạn[l, r)
    value_type range_query(int l, int r) {
        assert(0 <= l && l <= r && r <= n);
        if (l == r) return e();
        value_type sml = e();
        value_type smr = e();
        l += size;
        r += size;
        for (int i = height; i >= 1; --i) {
            if (((l >> i) << i) != l) push(l >> i);
            if (((r >> i) << i) != r) push((r - 1) >> i);
        }
        while (l < r) {
            if (l & 1) sml = op(sml, tree[l++]);
            if (r & 1) smr = op(tree[--r], smr);
            l >>= 1; r >>= 1;
        }
        return op(sml, smr);
    }

    /// @brief Lấy tổng toàn mảng
    value_type all_query() const {
        return tree[1];
    }

    /// @brief Tìm kiếm nhị phân trên Segment Tree (Trái sang Phải).
    /// Tìm r lớn nhất thỏa mãn hàm pred(op(a[l..r-1])) trả về true.
    template <typename Pred>
    int max_right(int l, Pred&& pred) {
        assert(0 <= l && l <= n);
        assert(pred(e()));
        if (l == n) return n;
        l += size;
        for (int i = height; i >= 1; --i) push(l >> i);
        value_type sm = e();
        do {
            while (l % 2 == 0) l >>= 1;
            if (!pred(op(sm, tree[l]))) {
                while (l < size) {
                    push(l);
                    l = (2 * l);
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
    template <typename Pred>
    int min_left(int r, Pred&& pred) {
        assert(0 <= r && r <= n);
        assert(pred(e()));
        if (r == 0) return 0;
        r += size;
        for (int i = height; i >= 1; --i) push((r - 1) >> i);
        value_type sm = e();
        do {
            r--;
            while (r > 1 && (r % 2)) r >>= 1;
            if (!pred(op(tree[r], sm))) {
                while (r < size) {
                    push(r);
                    r = (2 * r) + 1;
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
    int n, size, height;
    std::vector<lazy_type>  lazy;
    std::vector<value_type> tree;

    void pull(int i) {
        tree[i] = op(tree[i << 1], tree[(i << 1) | 1]);
    }

    void apply(int i, const lazy_type& t) {
        tree[i] = mapping(t, tree[i]);
        if (i < size) lazy[i] = composition(t, lazy[i]);
    }

    void push(int i) {
       apply(i << 1, lazy[i]);
       apply((i << 1) | 1, lazy[i]);
       lazy[i] = id();
    }
};

} // namespace ds
} // namespace cp
#endif