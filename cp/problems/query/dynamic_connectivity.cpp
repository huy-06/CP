/// @author Huy
/// @since  14:45:57 30/11/2025
/// @link   https://codeforces.com/gym/100551/problem/A
#include "cp/init/lib.hpp"
#include "cp/ds/hash/hash_map.hpp"
#include "cp/ds/edge/edge.hpp"
#include "cp/ds/tree/dsu_rollback.hpp"

using edge = cp::ds::edge;
 
struct query {
    char op;
    int u, v;
};
 
signed main() {
    //! std::ios_base::sync_with_stdio(false);
    //! std::cin.tie(nullptr);
    //! std::cout.tie(nullptr);
    //! freopen("connect.in", "r", stdin);
    //! freopen("connect.out", "w", stdout);
 
    int n, k;
    std::cin >> n >> k;
    if (k == 0) return 0;
 
    cp::ds::unordered_map<std::pair<int, int>, int> active_edges;
    std::vector<query> f(k);
 
    std::vector<std::vector<edge>> tree(k * 4 + 10);
 
    auto add_edge = [&](this auto&& self, int p, int l, int r, int x, int y, edge e) -> void {
        if (l > y || r < x) return;
        if (x <= l && r <= y) {
            tree[p].push_back(e);
            return;
        }
 
        int m = (l + r) / 2;
        self(2 * p, l, m, x, y, e);
        self(2 * p + 1, m + 1, r, x , y, e);
    };
 
    for (int i = 0; i < k; ++i) {
        char op;
        std::cin >> op;
 
        if (op == '?') {
            f[i] = {op, 0, 0};
        } else {
            int u, v;
            std::cin >> u >> v;
            --u; --v;
            if (u > v) std::swap(u, v);
 
            f[i] = {op, u, v};
            if (op == '+') {
                active_edges[{u, v}] = i;
            } else {
                int start_time = active_edges[{u, v}];
                add_edge(1, 0, k - 1, start_time, i - 1, {u, v});
                active_edges.erase({u, v});
            }
        }
    }
 
    for (auto [key, val] : active_edges) {
        add_edge(1, 0, k - 1, val, k - 1, edge(key.first, key.second));
    }
 
    cp::ds::dsu_rollback dsu(n);
 
    [&](this auto&& self, int p, int l, int r) -> void {
        int cnt = 0;
        for (auto& e : tree[p]) {
            if (dsu.merge(e.from, e.to)) {
                ++cnt;
            }
        }
 
        if (l == r) {
            if (l < f.size() && f[l].op == '?') {
                std::cout << dsu.comps() << '\n';
            }
        } else {
            int m = (l + r) / 2;
            self(2 * p, l, m);
            self(2 * p + 1, m + 1, r);
        }
 
        while (cnt--) {
            dsu.rollback();
        }
    }(1, 0, k - 1);
 
    return 0;
}