#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

#ifndef CP_DS_CHT_LI_CHAO_TREE
#define CP_DS_CHT_LI_CHAO_TREE
namespace cp {
namespace ds {

template <typename Tp = long long, bool IsMin = true>
class li_chao_tree {
public:
    using value_type = Tp;

    li_chao_tree(value_type x_min, value_type x_max) : x_min(x_min), x_max(x_max) {
        if constexpr (IsMin) {
            inf_val = std::numeric_limits<value_type>::max() / 2;
        } else {
            inf_val = std::numeric_limits<value_type>::min() / 2;
        }
        tree.push_back({{0, inf_val}, -1, -1});
    }

    // y = m * x + c
    void add_line(value_type m, value_type c) {
        add_line_rec(0, x_min, x_max, {m, c});
    }

    // y = m * x + c | x -> [l ,r)
    void add_segment(value_type m, value_type c, value_type l, value_type r) {
        if (l >= r) return;
        add_segment_rec(0, x_min, x_max, l, r - 1, {m, c});
    }

    value_type query(value_type x) {
        return query_rec(0, x_min, x_max, x);
    }

    value_type inf() const {
        return inf_val;
    }

private:
    struct line {
        value_type m, c;
        value_type eval(value_type x) const {
            return m * x + c;
        }
    };

    struct node {
        line l;
        int left = -1;
        int right = -1;
    };

    value_type x_min, x_max;
    value_type inf_val;
    std::vector<node> tree;

    inline bool is_better(const line& a, const line& b, value_type x) const {
        if (IsMin) return a.eval(x) < b.eval(x);
        return a.eval(x) > b.eval(x);
    }

    int create_node() {
        tree.push_back({{0, inf_val}, -1, -1});
        return (int)tree.size() - 1;
    }

    void add_line_rec(int cur, value_type l, value_type r, line new_line) {
        value_type mid = l + (r - l) / 2;
        bool mid_better = is_better(new_line, tree[cur].l, mid);
        bool left_better = is_better(new_line, tree[cur].l, l);

        if (mid_better) {
            std::swap(tree[cur].l, new_line);
        }

        if (new_line.c == inf_val) return;

        if (l == r) return;

        if (left_better != mid_better) {
            if (tree[cur].left == -1) {
                int next_id = create_node();
                tree[cur].left = next_id;
            }
            add_line_rec(tree[cur].left, l, mid, new_line);
        } else {
            if (tree[cur].right == -1) {
                int next_id = create_node();
                tree[cur].right = next_id;
            }
            add_line_rec(tree[cur].right, mid + 1, r, new_line);
        }
    }

    void add_segment_rec(int cur, value_type l, value_type r, value_type ql, value_type qr, line new_line) {
        if (qr < l || r < ql) return;
        if (ql <= l && r <= qr) {
            add_line_rec(cur, l, r, new_line);
            return;
        }

        value_type mid = l + (r - l) / 2;
        if (ql <= mid) {
            if (tree[cur].left == -1) {
                int next_id = create_node();
                tree[cur].left = next_id;
            }
            add_segment_rec(tree[cur].left, l, mid, ql, qr, new_line);
        }
        if (qr > mid) {
            if (tree[cur].right == -1) {
                int next_id = create_node();
                tree[cur].right = next_id;
            }
            add_segment_rec(tree[cur].right, mid + 1, r, ql, qr, new_line);
        }
    }

    value_type query_rec(int cur, value_type l, value_type r, value_type x) {
        if (cur == -1) return inf_val;
        value_type res = tree[cur].l.eval(x);
        if (l == r) return res;

        value_type mid = l + (r - l) / 2;
        if (x <= mid) {
            value_type left_res = query_rec(tree[cur].left, l, mid, x);
            return IsMin ? std::min(res, left_res) : std::max(res, left_res);
        } else {
            value_type right_res = query_rec(tree[cur].right, mid + 1, r, x);
            return IsMin ? std::min(res, right_res) : std::max(res, right_res);
        }
    }
};

} // namespace ds
} // namespace cp
#endif