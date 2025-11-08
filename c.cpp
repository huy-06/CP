/// @author Huy
/// @since  14:20:02 07/11/2025
#include "huy/init/lib.hpp"

signed main() {
    //! std::ios_base::sync_with_stdio(false);
    //! std::cin.tie(nullptr);
    //! std::cout.tie(nullptr);

    auto 🌻🌻🌻 = [&](int tc) -> void {
        int n, m;
        std::cin >> n >> m;

        std::vector<std::vector<std::pair<int, long>>> adj(n);
        for (int i = 1; i < n; ++i) {
            int u, v, w;
            std::cin >> u >> v >> w;
            --u; --v;
            adj[u].push_back({v, w});
            adj[v].push_back({u, w});
        }

        long long ans = 0;
        std::vector<long long> dp(n, -1);
        auto dfs = [&](this auto&& self, int u, int p) -> bool {
            for (auto [v, w] : adj[u]) {
                if (v == p) continue;
                bool have_cycle = sefl(v, u);
            }
        };
    };

    int 🌸;
    std::cin >> 🌸;

    for (int 🍀 = 1; 🍀 <= 🌸; ++🍀) {
        🌻🌻🌻(🍀);
    }

    return 0;
}