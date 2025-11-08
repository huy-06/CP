/// @author Huy
/// @since  22:09:11 07/11/2025
#include "huy/init/lib.hpp"

signed main() {
    //! std::ios_base::sync_with_stdio(false);
    //! std::cin.tie(nullptr);
    //! std::cout.tie(nullptr);

    int n, s1, s2;
    std::cin >> n >> s1 >> s2;

    std::vector<int> a(s1), b(s2);
    for (int i = 0; i < s1; ++i) {
        std::cin >> a[i];
    }
    for (int i = 0; i < s2; ++i) {
        std::cin >> b[i];
    }

    std::vector<int> s;
    s.reserve(n + 1);
    for (int i = 0; i < s1; ++i) {
        s.push_back(a[i]);
    }
    for (int i = s2 - 1; i >= 0; --i) {
        s.push_back(b[i]);
    }
    debug(s);

    int pos = std::find(s.begin(), s.end(), 0) - s.begin();

    int ans = 0, mx;
    mx = 0;
    for (int i = pos - 1; i >= 0; --i) {
        if (s[i] > mx) {
            ++ans;
            mx = s[i];
        }
    }
    mx = 0;
    for (int i = pos + 1; i < n + 1; ++i) {
        if (s[i] > mx) {
            ++ans;
            mx = s[i];
        }
    }

    std::cout << ans << '\n';

    return 0;
}