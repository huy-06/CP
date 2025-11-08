/// @author Huy
/// @since  19:52:26 07/11/2025
#include "huy/init/lib.hpp"

signed main() {
    //! std::ios_base::sync_with_stdio(false);
    //! std::cin.tie(nullptr);
    //! std::cout.tie(nullptr);

    long long d, s, e;
    std::cin >> d >> s >> e;

    long long u = d - s - e;

    long double ans;
    if (s <= u) {
        ans = 1.0l * s * (d - s) / (d * u);
    } else {
        ans = 1.0l * (s + e) / d;
    }

    std::cout << std::fixed << std::setprecision(15) << ans << '\n';

    return 0;
}