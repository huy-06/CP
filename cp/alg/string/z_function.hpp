#include <vector>
#include <string>

#ifndef CP_ALG_STRING_Z_FUNCTION
#define CP_ALG_STRING_Z_FUNCTION
namespace cp {
namespace alg {

/// @brief Z[i]là độ dài của tiền tố chung dài nhất giữa chuỗi S và hậu tố của S bắt đầu tại vị trí i.
template <typename Tp>
std::vector<int> z_function(const std::vector<Tp>& s) {
    int n = static_cast<int>(s.size());
    if (n == 0) return {};
    std::vector<int> z(n);
    z[0] = 0;
    for (int i = 1, j = 0; i < n; ++i) {
        int& k = z[i];
        k = (j + z[j] <= i) ? 0 : std::min(j + z[j] - i, z[i - j]);
        while (i + k < n && s[k] == s[i + k]) ++k;
        if (j + z[j] < i + z[i]) j = i;
    }
    z[0] = n;
    return z;
}

/// @brief Z[i]là độ dài của tiền tố chung dài nhất giữa chuỗi S và hậu tố của S bắt đầu tại vị trí i.
std::vector<int> z_function(const std::string& s) {
    int n = static_cast<int>(s.size());
    std::vector<int> s2(n);
    for (int i = 0; i < n; ++i) {
        s2[i] = s[i];
    }
    return z_function(s2);
}

} // namespace alg
} // namespace cp
#endif