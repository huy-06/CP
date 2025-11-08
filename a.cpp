/// @author Huy
/// @since  06:43:58 08/11/2025
#include "huy/init/lib.hpp"

signed main() {
    //! std::ios_base::sync_with_stdio(false);
    //! std::cin.tie(nullptr);
    //! std::cout.tie(nullptr);

    long long n;
    std::cin >> n;

    if (n == 1) {
        std::cout << "1\n";
        return 0;
    }

    long long m = n - 1;
    if (int c = std::popcount(1ull * m); c > 2 || c == 0) {
        std::cout << "impossible\n";
    } else {
        long long x = m & -m;
        std::cout << x + 1 << '\n';
    }

    return 0;
}