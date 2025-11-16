#include <vector>
#include <string>

#ifndef CP_ALG_STRING_MANACHER
#define CP_ALG_STRING_MANACHER
namespace cp {
namespace alg {

std::pair<std::vector<int>, std::vector<int>> manacher(const std::string& s) {
    int n = static_cast<int>(s.size());
    std::vector<int> d1(n), d2(n);

    for (int i = 0, l = 0, r = -1; i < n; ++i) {
        int k = 1;
        if (i <= r) k = std::min(d1[l + r - i], r - i + 1);
        while (0 <= i - k && i + k < n && s[i - k] == s[i + k]) ++k;
        d1[i] = k;
        if (i + k - 1 > r) {
            l = i - k + 1;
            r = i + k - 1;
        }
    }

    for (int i = 0, l = 0, r = -1; i < n; ++i) {
        int k = 0;
        if (i <= r) k = std::min(d2[l + r - i + 1], r - i + 1);
        while (0 <= i - k - 1 && i + k < n && s[i - k - 1] == s[i + k]) ++k;
        d2[i] = k;
        if (i + k - 1 > r) {
            l = i - k;
            r = i + k - 1;
        }
    }

    return {d1, d2};
}

} // namespace alg
} // namespace cp
#endif