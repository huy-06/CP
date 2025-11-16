/// @author Huy
/// @since  21:21:35 15/11/2025
#include "cp/init/lib.hpp"

signed main() {
    //! std::ios_base::sync_with_stdio(false);
    //! std::cin.tie(nullptr);
    //! std::cout.tie(nullptr);

    int n, a, b;
    std::cin >> n >> a >> b;

    std::string s;
    std::cin >> s;

    std::vector<int> ca(n + 1), cb(n + 1);
    for (int i = 0; i < n; ++i) {
        ca[i + 1] = ca[i] + (s[i] == 'a');
        cb[i + 1] = cb[i] + (s[i] == 'b'); 
    }

    long long ans = 0;
    for (int i = 0; i < n; ++i) {
        int ra = std::lower_bound(ca.begin(), ca.end(), ca[i] + a) - ca.begin();
        int rb = std::lower_bound(cb.begin(), cb.end(), cb[i] + b) - cb.begin();
        if (ra < rb) ans += rb - ra;
    }
    std::cout << ans << '\n';

    return 0;
}