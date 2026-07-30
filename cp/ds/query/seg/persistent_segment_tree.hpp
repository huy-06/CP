#include <vector>
#include <cassert>
#include <algorithm>

#ifndef CP_DS_PERSISTENT_SEGMENT_TREE
#define CP_DS_PERSISTENT_SEGMENT_TREE
namespace cp {
namespace ds {

/**
 * @brief Cấu trúc dữ liệu Persistent Segment Tree.
 * 
 * @tparam Tp Kiểu dữ liệu của các phần tử trong Cây.
 * @tparam op Con trỏ hàm định nghĩa phép toán kết hợp 2 phần tử (vd: cộng, max, min).
 *            Thỏa mãn: op(a, op(b, c)) == op(op(a, b), c).
 * @tparam e Con trỏ hàm trả về phần tử đơn vị (Identity element) của phép toán `op`.
 *           Thỏa mãn: op(a, e()) == op(e(), a) == a.
 */
template <class Tp, Tp (*op)(Tp, Tp), Tp (*e)()>
class persistent_segment_tree {
public:
    using value_type = Tp;

    struct node {
        int l = -1;
        int r = -1;
        value_type val = e();
    };

    persistent_segment_tree() : n(0) {}

    explicit persistent_segment_tree(int n, const value_type& val = e()) {
        init(n, val);
    }

    explicit persistent_segment_tree(const std::vector<value_type>& data) {
        init(data);
    }

    int init(int n, const value_type& val = e()) {
        std::vector<value_type> data(n, val);
        return init(data);
    }

    int init(const std::vector<value_type>& data) {
        n = static_cast<int>(data.size());
        nodes.clear();
        nodes.reserve(2 * n);
        roots.clear();
        int root = build(0, n, data);
        roots.push_back(root);
        return root;
    }

    // Trả về số lượng phiên bản hiện có
    int version_count() const {
        return int(roots.size());
    }

    // Lấy root của một phiên bản cụ thể
    int get_root(int version) const {
        assert(0 <= version && version < int(roots.size()));
        return roots[version];
    }

    // Tạo một phiên bản mới từ root cũ, gán val vào vị trí p
    // Trả về index của root mới
    int modify(int prev_root, int p, const value_type& val) {
        assert(0 <= p && p < n);
        int new_root = modify(prev_root, 0, n, p, val);
        roots.push_back(new_root);
        return new_root;
    }

    // Truy vấn trên phiên bản có gốc là root, nửa khoảng [l, r)
    value_type range_query(int root, int l, int r) const {
        assert(0 <= l && l <= r && r <= n);
        if (l == r) return e();
        return range_query(root, 0, n, l, r);
    }

    // Truy vấn 1 điểm trên phiên bản có gốc là root
    value_type point_query(int root, int p) const {
        assert(0 <= p && p < n);
        return range_query(root, 0, n, p, p + 1);
    }

    // Tìm vị trí r lớn nhất sao cho [l, r) thỏa mãn pred (trên phiên bản root)
    template <typename F>
    int max_right(int root, int l, F&& pred) const {
        assert(0 <= l && l <= n);
        assert(pred(e()));
        if (l == n) return n;
        value_type sm = e();
        return max_right(root, 0, n, l, sm, pred);
    }

    // Tìm vị trí l nhỏ nhất sao cho [l, r) thỏa mãn pred (trên phiên bản root)
    template <typename F>
    int min_left(int root, int r, F&& pred) const {
        assert(0 <= r && r <= n);
        assert(pred(e()));
        if (r == 0) return 0;
        value_type sm = e();
        return min_left(root, 0, n, r, sm, pred);
    }

    // Tìm phần tử nhỏ thứ K giữa 2 phiên bản (root_l và root_r)
    int find_kth(int root_l, int root_r, int k) const {
        return find_kth(root_l, root_r, 0, n, k);
    }

    // Hàm tiện ích để reserve bộ nhớ nếu biết trước số lượng update
    void reserve(int update_count) {
        nodes.reserve(nodes.size() + update_count * (std::__lg(n) + 2));
    }

private:
    int n;
    std::vector<node> nodes;
    std::vector<int> roots;

    int copy_node(int u) {
        node nd = nodes[u];
        nodes.push_back(nd);
        return int(nodes.size()) - 1;
    }

    int new_node() {
        nodes.emplace_back();
        return int(nodes.size()) - 1;
    }

    void pull(int u) {
        if (nodes[u].l != -1 && nodes[u].r != -1) {
            nodes[u].val = op(nodes[nodes[u].l].val, nodes[nodes[u].r].val);
        }
    }

    int build(int l, int r, const std::vector<value_type>& data) {
        int u = new_node();
        if (l == r - 1) {
            nodes[u].val = data[l];
        } else {
            int mid = l + (r - l) / 2;
            int l_child = build(l, mid, data);
            int r_child = build(mid, r, data);
            nodes[u].l = l_child;
            nodes[u].r = r_child;
            pull(u);
        }
        return u;
    }

    int modify(int u, int l, int r, int p, const value_type& v) {
        int new_u = copy_node(u);
        if (l == r - 1) {
            nodes[new_u].val = v;
        } else {
            int mid = l + (r - l) / 2;
            if (p < mid) {
                int child = modify(nodes[u].l, l, mid, p, v);
                nodes[new_u].l = child;
            } else {
                int child = modify(nodes[u].r, mid, r, p, v);
                nodes[new_u].r = child;
            }
            pull(new_u);
        }
        return new_u;
    }

    value_type range_query(int u, int l, int r, int ql, int qr) const {
        if (ql <= l && r <= qr) {
            return nodes[u].val;
        }
        int mid = l + (r - l) / 2;
        if (qr <= mid) return range_query(nodes[u].l, l, mid, ql, qr);
        if (ql >= mid) return range_query(nodes[u].r, mid, r, ql, qr);
        
        // Kết hợp kết quả từ cây con trái và phải bằng hàm op
        return op(range_query(nodes[u].l, l, mid, ql, qr), 
                  range_query(nodes[u].r, mid, r, ql, qr));
    }

    template <typename F>
    int max_right(int u, int l, int r, int ql, value_type& sm, F&& pred) const {
        if (r <= ql) return r;
        if (l >= ql) {
            value_type new_sm = op(sm, nodes[u].val);
            if (pred(new_sm)) {
                sm = new_sm;
                return r;
            }
            if (l == r - 1) return l;
        }
        int mid = l + (r - l) / 2;
        int res = -1;
        if (ql < mid) {
            res = max_right(nodes[u].l, l, mid, ql, sm, pred);
            if (res < mid) return res;
        }
        return max_right(nodes[u].r, mid, r, ql, sm, pred);
    }

    template <typename F>
    int min_left(int u, int l, int r, int qr, value_type& sm, F&& pred) const {
        if (l >= qr) return l;
        if (r <= qr) {
            value_type new_sm = op(nodes[u].val, sm); 
            if (pred(new_sm)) {
                sm = new_sm;
                return l;
            }
            if (l == r - 1) return r;
        }
        int mid = l + (r - l) / 2;
        int res = -1;
        if (qr > mid) {
            res = min_left(nodes[u].r, mid, r, qr, sm, pred);
            if (res > mid) return res;
        }
        return min_left(nodes[u].l, l, mid, qr, sm, pred);
    }

    int find_kth(int ul, int ur, int l, int r, int k) const {
        if (l == r - 1) {
            return l;
        }
        int mid = l + (r - l) / 2;

        int cnt_l = nodes[nodes[ur].l].val - nodes[nodes[ul].l].val;
        if (k <= cnt_l) {
            return find_kth(nodes[ul].l, nodes[ur].l, l, mid, k);
        } else {
            return find_kth(nodes[ul].r, nodes[ur].r, mid, r, k - cnt_l);
        }
    }
};

} // namespace ds
} // namespace cp
#endif