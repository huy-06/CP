/// @author Huy
/// @since  16:50:02 07/12/2025
/// @link   https://www.spoj.com/problems/ADAMATCH/?__cf_chl_tk=fH2q.nNx5eBzvnEWfGdYlCN0aC0TIbqekFoHLLYItbM-1765099357-1.0.1.1-zChkK1pAprzZVnvX.Sl9ard8EIDOX.gJlmqa5_yAYpM
#include "cp/init/library.hpp"
#include "cp/init/constexpr.hpp"
#include "cp/alg/math/polynomial/convolution.hpp"

signed main() {
    //! std::ios_base::sync_with_stdio(false);
    //! std::cin.tie(nullptr);
    //! std::cout.tie(nullptr);

    std::string s, r;
    std::cin >> s >> r;

    int n = int(s.size());
    int m = int(r.size());

    std::vector<int> total_matches(n + m + 1, 0);
    for (char c : {'A', 'C', 'G', 'T'}) {
        std::vector<int> a(n);
        for (int i = 0; i < n; ++i) {
            a[i] = (s[i] == c);
        }

        std::vector<int> b(m);
        for (int i = 0; i < m; ++i) {
            b[i] = (r[m - 1 - i] == c);
        }

        auto res = cp::alg::convolution(a, b);
        for (int i = 0; i < int(res.size()); ++i) {
            if (i < int(total_matches.size())) {
                total_matches[i] += res[i];
            }
        }
    }

    int ans = m;
    for (int i = m - 1; i < n; ++i) {
        ans = std::min(ans, m - total_matches[i]);
    }
    std::cout << ans << '\n';

    return 0;
}