#include <vector>
#include <cassert>
#include <algorithm>

#ifndef CP_DS_PERSISTENT_SEGMENT_TREE
#define CP_DS_PERSISTENT_SEGMENT_TREE
namespace cp {
namespace ds {

template <typename Tp>
class persistent_segment_tree {
public:
    using value_type = Tp;

    struct node {
        int l = -1;
        int r = -1;
        value_type val;
    };

    persistent_segment_tree() : n(0) {}

    persistent_segment_tree(int n, const value_type& val = value_type()) {
        init(n, val);
    }

    persistent_segment_tree(const std::vector<value_type>& data) {
        init(data);
    }

    void init(int n, const value_type& val = value_type()) {
        std::vector<value_type> data(n, val);
        init(data);
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

    // Truy vấn trên phiên bản có gốc là root
    value_type range_query(int root, int l, int r) const {
        assert(0 <= l && l <= r && r <= n);
        if (l == r) return value_type();
        return range_query(root, 0, n, l, r);
    }

    value_type point_query(int root, int p) const {
        assert(0 <= p && p < n);
        return range_query(root, 0, n, p, p + 1);
    }

    // Tìm vị trí r lớn nhất sao cho [l, r) thỏa mãn pred (trên phiên bản root)
    template <typename F>
    int max_right(int root, int l, F&& pred) const {
        assert(0 <= l && l <= n);
        assert(pred(value_type()));
        if (l == n) return n;
        value_type sm = value_type();
        return max_right(root, 0, n, l, sm, pred);
    }

    // Tìm vị trí l nhỏ nhất sao cho [l, r) thỏa mãn pred (trên phiên bản root)
    template <typename F>
    int min_left(int root, int r, F&& pred) const {
        assert(0 <= r && r <= n);
        assert(pred(value_type()));
        if (r == 0) return 0;
        value_type sm = value_type();
        return min_left(root, 0, n, r, sm, pred);
    }

    // Hàm tiện ích để reserve bộ nhớ nếu biết trước số lượng truy vấn
    void reserve(int update_count) {
        nodes.reserve(nodes.size() + update_count * (std::__lg(n) + 2));
    }

private:
    int n;
    std::vector<node> nodes;
    std::vector<int> roots;

    int copy_node(int u) {
        nodes.push_back(nodes[u]);
        return int(nodes.size()) - 1;
    }

    int new_node() {
        nodes.emplace_back();
        return int(nodes.size()) - 1;
    }

    void pull(int u) {
        if (nodes[u].l != -1 && nodes[u].r != -1) {
            nodes[u].val = nodes[nodes[u].l].val + nodes[nodes[u].r].val;
        }
    }

    int build(int l, int r, const std::vector<value_type>& data) {
        int u = new_node();
        if (l == r - 1) {
            nodes[u].val = data[l];
        } else {
            int mid = l + (r - l) / 2;
            nodes[u].l = build(l, mid, data);
            nodes[u].r = build(mid, r, data);
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
                nodes[new_u].l = modify(nodes[u].l, l, mid, p, v);
            } else {
                nodes[new_u].r = modify(nodes[u].r, mid, r, p, v);
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
        return range_query(nodes[u].l, l, mid, ql, qr) + 
               range_query(nodes[u].r, mid, r, ql, qr);
    }

    template <typename F>
    int max_right(int u, int l, int r, int ql, value_type& sm, F&& pred) const {
        if (l >= ql) {
            value_type new_sm = sm + nodes[u].val;
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
            if (res != mid) return res;
        }
        return max_right(nodes[u].r, mid, r, ql, sm, pred);
    }

    template <typename F>
    int min_left(int u, int l, int r, int qr, value_type& sm, F&& pred) const {
        if (r <= qr) {
            value_type new_sm = nodes[u].val + sm;
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
            if (res != mid) return res;
        }
        return min_left(nodes[u].l, l, mid, qr, sm, pred);
    }
};

} // namespace ds
} // namespace cp
#endif