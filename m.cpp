/// @author Huy
/// @since  04:44:22 09/11/2025
#include "huy/init/lib.hpp"

signed main() {
    //! std::ios_base::sync_with_stdio(false);
    //! std::cin.tie(nullptr);
    //! std::cout.tie(nullptr);

    auto 🌻🌻🌻 = [&]([[maybeunuse]] int tc) -> void {
        int n;
        std::cin >> n;

        std::string s;
        std::cin >> s;

        int cnt = std::count(s.begin(), s.end(), '1');
        int max = 0, c = 0;
        for (int i = 0; i < n; ++i) {
            if (s[i] == '1') ++c;
            else c = 0;
            max = std::max(max, std::min(3, c));
        }
        std::cout << cnt - (max != 0 ? max - 1 : 0) << '\n';
        
    };

    int 🌸;
    std::cin >> 🌸;

    for (int 🍀 = 1; 🍀 <= 🌸; ++🍀) {
        🌻🌻🌻(🍀);
    }

    return 0;
}