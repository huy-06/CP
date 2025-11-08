/// @author Huy
/// @since  13:07:44 07/11/2025
#include "huy/init/lib.hpp"

constexpr int inf = 1e9;

signed main() {
    //! std::ios_base::sync_with_stdio(false);
    //! std::cin.tie(nullptr);
    //! std::cout.tie(nullptr);

    auto 🌻🌻🌻 = [&](int tc) -> void {
        std::string s;
        std::cin >> s;

        int n = int(s.size());

        std::vector dp(n, std::vector(n, inf));
        auto dfs = [&](this auto&& self, int l, int r) {
            if (l < 0 && r >= n) return 0;
            if (l < 0) return n - r;
            if (r >= n) return l + 1;
            if (dp[l][r] != inf) return dp[l][r];
            if (s[l] != s[r]) {
                // dp[l][r] = std::min(dp[l][r], self(l - 1, r + 1) + 2);
                dp[l][r] = std::min(dp[l][r], self(l - 1, r) + 1);
                dp[l][r] = std::min(dp[l][r], self(l, r + 1) + 1);
                dp[l][r] = std::min(dp[l][r], self(l - 1, r + 1) + 1);
            } else {
                dp[l][r] = std::min(dp[l][r], self(l - 1, r + 1));
            }
            return dp[l][r];
        };

        int ans = inf;
        for (int p = 0; p < n; ++p) {
            ans = std::min(ans, dfs(p, p));
            ans = std::min(ans, dfs(p, p + 1));
        }
        debug(dp);

        std::cout << ans << '\n';
    };

    int 🌸;
    std::cin >> 🌸;

    for (int 🍀 = 1; 🍀 <= 🌸; ++🍀) {
        🌻🌻🌻(🍀);
    }

    return 0;
}