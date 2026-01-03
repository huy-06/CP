/// @author Huy
/// @since  21:20:21 25/11/2025
/// @link   https://www.spoj.com/problems/GIVEAWAY/ 
#include "cp/init/lib.hpp"
#include "cp/ds/query/fenwick_tree.hpp"

struct query {
    int i_q, type, i_a, val, sign, time;
};

bool operator<(const query& a, const query& b) {
    return a.val > b.val;
}

signed main() {
    //! std::ios_base::sync_with_stdio(false);
    //! std::cin.tie(nullptr);
    //! std::cout.tie(nullptr);

    int n;
    std::cin >> n;

    std::vector<int> a(n);
    std::vector<query> queries;

    int time_cnt = 0;

    for (int i = 0; i < n; ++i) {
        std::cin >> a[i];
        queries.push_back({-1, 0, i, a[i], 1, ++time_cnt});
    }

    int q;
    std::cin >> q;

    std::vector<char> is_query(q);

    for (int i = 0; i < q; ++i) {
        int type;
        std::cin >> type;

        if (type == 0) {
            int l, r, c;
            std::cin >> l >> r >> c;
            --l; --r;

            is_query[i] = true;
            queries.push_back({i, 1, r, c, 1, ++time_cnt});
            queries.push_back({i, 1, l - 1, c, -1, ++time_cnt});
        } else {
            int p, c;
            std::cin >> p >> c;
            --p;

            is_query[i] = false;
            queries.push_back({-1, 0, p, a[p], -1, ++time_cnt});
            queries.push_back({-1, 0, p, c, 1, ++time_cnt});

            a[p] = c;
        }
    }

    cp::ds::fenwick_tree<int> bit(n);

    std::vector<int> ans(q);

    std::function<void(int, int)> dfs = [&](int l, int r) -> void {
        if (l >= r) {
            return;
        }

        int mid = (l + r) / 2;
        dfs(l, mid);
        dfs(mid + 1, r);

        std::vector<query> left_update;
        for (int i = l; i <= mid; ++i) {
            if (queries[i].type == 0) {
                left_update.push_back(queries[i]);
            }
        }
        std::sort(left_update.begin(), left_update.end());

        std::vector<query> right_query;
        for (int i = mid + 1; i <= r; ++i) {
            if (queries[i].type == 1) {
                right_query.push_back(queries[i]);
            }
        }
        std::sort(right_query.begin(), right_query.end());

        int i = 0;
        for (const auto& q : right_query) {
            while (i < int(left_update.size()) && left_update[i].val >= q.val) {
                bit.add(left_update[i].i_a, left_update[i].sign);
                ++i;
            }
           
            ans[q.i_q] += 1ll * q.sign * bit.range_sum(0, q.i_a + 1);
        }

        for (int k = 0; k < i; ++k) {
            bit.add(left_update[k].i_a, -left_update[k].sign);
        }
    };
    dfs(0, queries.size() - 1);

    for (int i = 0; i < q; ++i) {
        if (is_query[i]) {
            std::cout << ans[i] << '\n';
        }
    }

    return 0;
}