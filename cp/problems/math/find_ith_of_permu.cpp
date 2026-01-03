/// @author Huy
/// @since  11:20:56 25/11/2025
/// @link   https://www.spoj.com/problems/TPGA/
#include "cp/init/lib.hpp"
#include "cp/ds/num/mod/comb.hpp"
#include "cp/ds/query/fenwick_tree.hpp"

using mint = cp::ds::mint1000000007;
 
signed main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
 
    cp::ds::comb<mint> comb;
 
    auto solve = [&](int tc) -> void {
        int n;
        std::cin >> n;
 
        std::vector<int> p(n);
        for (int i = 0; i < n; ++i) {
            std::cin >> p[i];
        }
 
        cp::ds::fenwick_tree<int> ft(n + 1);
        for (int i = 1; i <= n; ++i) {
            ft.add(i, 1);
        }
 
        mint ans = 1;
        for (int i = 0; i < n; ++i) {
            mint smaller = ft.range_sum(1, p[i]);
            mint ways = smaller * comb.fact(n - 1 - i);
            ans += ways;
 
            ft.add(p[i], -1);
        }
        std::cout << ans << '\n';
    };
 
    int t;
    std::cin >> t;
 
    for (int tc = 1; tc <= t; ++tc) {
        solve(tc);
    }
 
    return 0;
} 