/// @author Huy
/// @since  17:15:11 25/10/2025
#include "huy/init/library.hpp"

signed main() {
    //! std::ios_base::sync_with_stdio(false);
    //! std::cin.tie(nullptr);
    //! std::cout.tie(nullptr);

    auto solve = [&](int tc) -> void {
        std::string s;
        std::cin >> s;
        char c[2] = {'>', '<'};
        if (s[0] == s[2]) {
            std::cout << s[0] << '=' << s[2] << '\n';
        } else {
            std::cout << s[0] << c[s[0] < s[2]] << s[2] << '\n';
        }
    };

    int t;
    std::cin >> t;

    for (int tc = 1; tc <= t; ++tc) {
        solve(tc);
    }

    return 0;
}