/// @author Huy
/// @since  19:46:47 07/11/2025
#include "huy/init/lib.hpp"

signed main() {
    //! std::ios_base::sync_with_stdio(false);
    //! std::cin.tie(nullptr);
    //! std::cout.tie(nullptr);

    std::string s;
    std::cin >> s;

    for (int i = int(s.size()) - 1; i >= 0; --i) {
        if (s[i] == 'a' || s[i] == 'e' || s[i] == 'u' || s[i] == 'o' || s[i] == 'i') {
            std::cout << s.substr(0, i + 1) + "ntry\n";
            break;
        }
    }

    return 0;
}