/// @author Huy
/// @since  07:35:00 08/11/2025
#include "huy/init/lib.hpp"
#include "huy/ds/num/mod/dynamic.hpp"
#include "huy/ds/graph/graph.hpp"
#include "huy/ds/graph/edge/edge.hpp"

using mint = huy::ds::dynamic_mod_int<int, 0>;

signed main() {
    //! std::ios_base::sync_with_stdio(false);
    //! std::cin.tie(nullptr);
    //! std::cout.tie(nullptr);

    int n, m, p;
    std::cin >> n >> m >> p;
    mint::set_mod(p);

    huy::ds::graph<huy::ds::edge> g(n, m);
    g.read_unedge(m);

    std::vector<int> vis(n);
    int comp = 0;
    for (int i = 0; i < n; ++i) {
        if (!vis[i]) {
            std::queue<int> q;
            q.push(i);

            vis[i] = 1;
            while (!q.empty()) {
                int u = q.front();
                q.pop();
                for (auto e : g[u]) {
                    int v = e.to;
                    if (vis[v]) {
                        if (vis[u] == vis[v]) {
                            std::cout << "impossible\n";
                            return 0;
                        }
                        continue;
                    }
                    vis[v] = vis[u] == 1 ? 2 : 1;
                    q.push(v);
                }
            }

            ++comp;
        }
    }

    std::cout << mint(2).pow(comp - 1) + 1 << '\n';

    return 0;
}