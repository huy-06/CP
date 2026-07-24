/// @author Thuw
/// @since  07:48:37 24/07/2026
#include <bits/stdc++.h>
#include <vector>
#include <cassert>
#include <algorithm>

#ifndef CP_DS_PERSISTENT_SEGMENT_TREE
#define CP_DS_PERSISTENT_SEGMENT_TREE
namespace cp {
namespace ds {

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

    int version_count() const {
        return int(roots.size());
    }

    int get_root(int version) const {
        assert(0 <= version && version < int(roots.size()));
        return roots[version];
    }

    int modify(int prev_root, int p, const value_type& val) {
        assert(0 <= p && p < n);
        int new_root = modify(prev_root, 0, n, p, val);
        roots.push_back(new_root);
        return new_root;
    }

    value_type range_query(int root, int l, int r) const {
        assert(0 <= l && l <= r && r <= n);
        if (l == r) return e();
        return range_query(root, 0, n, l, r);
    }

    value_type point_query(int root, int p) const {
        assert(0 <= p && p < n);
        return range_query(root, 0, n, p, p + 1);
    }

    template <typename F>
    int max_right(int root, int l, F&& pred) const {
        assert(0 <= l && l <= n);
        assert(pred(e()));
        if (l == n) return n;
        value_type sm = e();
        return max_right(root, 0, n, l, sm, pred);
    }

    template <typename F>
    int min_left(int root, int r, F&& pred) const {
        assert(0 <= r && r <= n);
        assert(pred(e()));
        if (r == 0) return 0;
        value_type sm = e();
        return min_left(root, 0, n, r, sm, pred);
    }

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
            nodes[u].val = op(nodes[nodes[u].l].val, nodes[nodes[u].r].val);
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

        return op(range_query(nodes[u].l, l, mid, ql, qr),
                  range_query(nodes[u].r, mid, r, ql, qr));
    }

    template <typename F>
    int max_right(int u, int l, int r, int ql, value_type& sm, F&& pred) const {
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
            if (res != mid) return res;
        }
        return max_right(nodes[u].r, mid, r, ql, sm, pred);
    }

    template <typename F>
    int min_left(int u, int l, int r, int qr, value_type& sm, F&& pred) const {
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
            if (res != mid) return res;
        }
        return min_left(nodes[u].l, l, mid, qr, sm, pred);
    }
};

} // namespace ds
} // namespace cp
#endif

signed main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    int n, q;
    std::cin >> n >> q;

    std::vector<long long> a(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> a[i];
    }

    cp::ds::persistent_segment_tree<
        long long,
        [](long long a, long long b) -> long long {
            return a + b;
        },
        []() -> long long {
            return 0;
        }
    > seg(a);
    seg.reserve(q);


    while (q--) {
        int t;
        std::cin >> t;

        if (t == 1) {
            int k, a, x;
            std::cin >> k >> a >> x;
            --k; --a;

            seg.modify(k, a, x);
        } else if (t == 2) {
            int k, a, b;
            std::cin >> k >> a >> b;
            --k; --a;

            std::cout << seg.range_query(k, a, b) << '\n';;
        } else {
            int k;
            std::cin >> k;
            --k;


        }
    }

    return 0;
}