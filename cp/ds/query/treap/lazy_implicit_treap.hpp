#include <vector>
#include <cassert>
#include <random>
#include <utility>
#include <algorithm>

#ifndef CP_DS_QUERY_LAZY_IMPLICIT_TREAP
#define CP_DS_QUERY_LAZY_IMPLICIT_TREAP
namespace cp {
namespace ds {

/**
 * @brief Cấu trúc dữ liệu Implicit Treap Lazy Propagation.
 * 
 * Cho phép Chèn, Xóa, Đảo ngược, Di chuyển đoạn, Cập nhật đoạn và Truy vấn đoạn trong O(log N).
 * Sử dụng node Pool (Mảng tĩnh) để đạt hiệu năng tối đa.
 */
template <
    class Tp, 
    Tp (*op)(Tp, Tp), 
    Tp (*e)(),
    class Tg, 
    Tp (*mapping)(Tg, Tp), 
    Tg (*composition)(Tg, Tg), Tg (*id)()
>
class lazy_implicit_treap {
public:
    using value_type = Tp;
    using lazy_type  = Tg;

    /// @brief Khởi tạo cây rỗng
    lazy_implicit_treap() : root(0) {
        clear();
    }

    /// @brief Khởi tạo cây với n phần tử val
    explicit lazy_implicit_treap(int n, const value_type& val = e()) : root(0) {
        init(n, val);
    }

    /// @brief Khởi tạo cây từ vector data sẵn có
    explicit lazy_implicit_treap(const std::vector<value_type>& data) : root(0) {
        init(data);
    }

    /// @brief Xóa toàn bộ cây và dọn dẹp bộ nhớ
    void clear() {
        nodes.clear();
        freelist.clear();
        nodes.emplace_back(); 
        nodes[0].size = 0;
        nodes[0].acc = e();
        nodes[0].tag = id();
        nodes[0].has_tag = false;
        nodes[0].rev = false;
        nodes[0].l = nodes[0].r = 0;
        root = 0;
    }

    /// @brief Khởi tạo lại cây với n phần tử val
    void init(int n, const value_type& val = e()) {
        std::vector<value_type> data(n, val);
        init(data);
    }

    /// @brief Khởi tạo lại cây từ vector data trong thời gian
    void init(const std::vector<value_type>& data) {
        clear();
        int n = static_cast<int>(data.size());
        if (n == 0) return;

        std::vector<int> st;
        st.reserve(n);

        for (int i = 0; i < n; ++i) {
            int u = alloc_node(data[i]);
            int last = 0;
            while (!st.empty() && nodes[st.back()].pri < nodes[u].pri) {
                last = st.back();
                st.pop_back();
            }
            nodes[u].l = last;
            if (!st.empty()) {
                nodes[st.back()].r = u;
            }
            st.push_back(u);
        }

        root = st[0];
        
        auto dfs_pull = [&](auto& self, int u) -> void {
            if (!u) return;
            self(self, nodes[u].l);
            self(self, nodes[u].r);
            pull(u);
        };
        dfs_pull(dfs_pull, root);
    }

    /// @brief Số lượng phần tử hiện tại
    int size() const {
        return get_size(root);
    }

    /// @brief Kiểm tra mảng rỗng
    bool empty() const {
        return size() == 0;
    }

    /// @brief Thêm phần tử vào cuối mảng
    void push_back(const value_type& val) {
        int u = alloc_node(val);
        merge(root, root, u);
    }

    /// @brief Chèn phần tử val vào vị trí p (0 <= p <= size)
    void insert(int p, const value_type& val) {
        assert(0 <= p && p <= size());
        int l, r;
        split(root, p, l, r);
        int u = alloc_node(val);
        merge(l, l, u);
        merge(root, l, r);
    }

    /// @brief Xóa phần tử tại vị trí p (0 <= p < size)
    void erase(int p) {
        assert(0 <= p && p < size());
        int l, mid, r;
        split(root, p, l, r);
        split(r, 1, mid, r);
        free_node(mid);
        merge(root, l, r);
    }

    /// @brief Thay đổi giá trị tại vị trí p thành val
    void modify(int p, const value_type& val) {
        assert(0 <= p && p < size());
        int l, mid, r;
        split(root, p, l, r);
        split(r, 1, mid, r);
        nodes[mid].val = val;
        pull(mid);
        merge(r, mid, r);
        merge(root, l, r);
    }

    /// @brief Lấy giá trị tại vị trí p
    value_type point_query(int p) {
        assert(0 <= p && p < size());
        int l, mid, r;
        split(root, p, l, r);
        split(r, 1, mid, r);
        value_type res = nodes[mid].val;
        merge(r, mid, r);
        merge(root, l, r);
        return res;
    }

    /// @brief Áp dụng tag f lên đoạn [l, r)
    void range_apply(int l, int r, const lazy_type& f) {
        assert(0 <= l && l <= r && r <= size());
        if (l == r) return;
        int le, mid, ri;
        split(root, l, le, ri);
        split(ri, r - l, mid, ri);
        apply(mid, f);
        merge(ri, mid, ri);
        merge(root, le, ri);
    }

    /// @brief Tính toán (gộp) kết quả trên đoạn [l, r)
    value_type range_query(int l, int r) {
        assert(0 <= l && l <= r && r <= size());
        if (l == r) return e();
        int le, mid, ri;
        split(root, l, le, ri);
        split(ri, r - l, mid, ri);
        value_type res = get_acc(mid);
        merge(ri, mid, ri);
        merge(root, le, ri);
        return res;
    }

    /// @brief Kết quả trên toàn mảng
    value_type all_query() const {
        return get_acc(root);
    }

    /// @brief Đảo ngược đoạn [l, r)
    void reverse(int l, int r) {
        assert(0 <= l && l <= r && r <= size());
        if (l == r) return;
        int le, mid, ri;
        split(root, l, le, ri);
        split(ri, r - l, mid, ri);
        if (mid) nodes[mid].rev ^= true;
        merge(ri, mid, ri);
        merge(root, le, ri);
    }

    /**
     * @brief Cắt đoạn [l, r) và chèn vào vị trí target_pos
     * @param l Chỉ số bắt đầu đoạn cần chuyển
     * @param r Chỉ số kết thúc đoạn cần chuyển (không gồm r)
     * @param target_pos Vị trí chèn trong mảng sau khi đã cắt đoạn [l, r) ra
     */
    void move_range(int l, int r, int target_pos) {
        assert(0 <= l && l <= r && r <= size());
        if (l == r) return;

        int le, mid, ri;
        split(root, l, le, ri);
        split(ri, r - l, mid, ri);

        int rem = 0;
        merge(rem, le, ri);

        assert(0 <= target_pos && target_pos <= get_size(rem));
        int left_part, right_part;
        split(rem, target_pos, left_part, right_part);

        merge(root, left_part, mid);
        merge(root, root, right_part);
    }

    /// @brief Trả về toàn bộ dãy dưới dạng std::vector
    std::vector<value_type> to_vector() {
        std::vector<value_type> res;
        res.reserve(size());
        auto dfs = [&](auto& self, int u) -> void {
            if (!u) return;
            push(u);
            self(self, nodes[u].l);
            res.push_back(nodes[u].val);
            self(self, nodes[u].r);
        };
        dfs(dfs, root);
        return res;
    }

private:
    struct node {
        value_type val;
        value_type acc;
        lazy_type  tag;
        bool       has_tag;
        bool       rev;
        uint32_t   pri;
        int        size;
        int        l, r;

        node(const value_type& v = e())
            : val(v), acc(v), tag(id()), has_tag(false), rev(false),
              pri(gen_pri()), size(1), l(0), r(0) {}

        static uint32_t gen_pri() {
            static std::mt19937 rng(1337);
            return rng();
        }
    };

    std::vector<node> nodes;
    std::vector<int>  freelist;
    int root;

    int alloc_node(const value_type& val) {
        int u;
        if (!freelist.empty()) {
            u = freelist.back();
            freelist.pop_back();
            nodes[u] = node(val);
        } else {
            u = static_cast<int>(nodes.size());
            nodes.emplace_back(val);
        }
        return u;
    }

    void free_node(int u) {
        if (u != 0) {
            freelist.push_back(u);
        }
    }

    int get_size(int u) const {
        return u ? nodes[u].size : 0;
    }

    value_type get_acc(int u) const {
        return u ? nodes[u].acc : e();
    }

    void pull(int u) {
        if (!u) return;
        nodes[u].size = 1 + get_size(nodes[u].l) + get_size(nodes[u].r);
        nodes[u].acc = op(get_acc(nodes[u].l), op(nodes[u].val, get_acc(nodes[u].r)));
    }

    void apply(int u, const lazy_type& f) {
        if (!u) return;
        nodes[u].val = mapping(f, nodes[u].val);
        nodes[u].acc = mapping(f, nodes[u].acc);
        if (nodes[u].has_tag) {
            nodes[u].tag = composition(f, nodes[u].tag);
        } else {
            nodes[u].tag = f;
            nodes[u].has_tag = true;
        }
    }

    void push(int u) {
        if (!u) return;
        if (nodes[u].rev) {
            std::swap(nodes[u].l, nodes[u].r);
            if (nodes[u].l) nodes[u].l[nodes.data()].rev ^= true;
            if (nodes[u].r) nodes[nodes[u].r].rev ^= true;
            nodes[u].rev = false;
        }
        if (nodes[u].has_tag) {
            if (nodes[u].l) apply(nodes[u].l, nodes[u].tag);
            if (nodes[u].r) apply(nodes[u].r, nodes[u].tag);
            nodes[u].tag = id();
            nodes[u].has_tag = false;
        }
    }

    void split(int u, int k, int& l, int& r) {
        if (!u) { l = r = 0; return; }
        push(u);
        int left_size = get_size(nodes[u].l);
        if (k <= left_size) {
            split(nodes[u].l, k, l, nodes[u].l);
            r = u;
            pull(r);
        } else {
            split(nodes[u].r, k - left_size - 1, nodes[u].r, r);
            l = u;
            pull(l);
        }
    }

    void merge(int& u, int l, int r) {
        push(l);
        push(r);
        if (!l || !r) {
            u = l ? l : r;
            return;
        }
        if (nodes[l].pri > nodes[r].pri) {
            merge(nodes[l].r, nodes[l].r, r);
            u = l;
        } else {
            merge(nodes[r].l, l, nodes[r].l);
            u = r;
        }
        pull(u);
    }
};

} // namespace ds
} // namespace cp
#endif