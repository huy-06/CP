#include <vector>
#include <string>

#ifndef CP_ALG_STRING_KMP
#define CP_ALG_STRING_KMP
namespace cp {
namespace alg {

/// @brief pi[i] là độ dài của tiền tố thực sự dài nhất của chuỗi con s[0...i] mà cũng là hậu tố của chuỗi con này.
/// Ví dụ: s = "ABAAB", pi = {0, 0, 1, 1, 2}
template <typename Tp>
std::vector<int> prefix_function(const std::vector<Tp>& s) {
    int n = static_cast<int>(s.size());
    std::vector<int> pi(n);
    for (int i = 1; i < n; ++i) {
        int j = pi[i - 1];
        while (j > 0 && s[i] != s[j]) {
            j = pi[j - 1];
        }
        if (s[i] == s[j]) {
            j++;
        }
        pi[i] = j;
    }
    return pi;
}

/// @brief pi[i] là độ dài của tiền tố thực sự dài nhất của chuỗi con s[0...i] mà cũng là hậu tố của chuỗi con này.
std::vector<int> prefix_function(const std::string& s) {
    int n = static_cast<int>(s.size());
    std::vector<int> s2(n);
    for (int i = 0; i < n; ++i) {
        s2[i] = s[i];
    }
    return prefix_function(s2);
}

// Hàm tiện ích để tìm kiếm chuỗi t trong s dùng KMP
// Trả về các vị trí bắt đầu của t trong s
template <typename Tp>
std::vector<int> kmp_search(const std::vector<Tp>& s, const std::vector<Tp>& t) {
    if (t.empty()) return {};
    std::vector<int> pi = prefix_function(t);
    std::vector<int> ans;
    
    int n = static_cast<int>(s.size());
    int m = static_cast<int>(t.size());
    int j = 0;

    for (int i = 0; i < n; ++i) {
        while (j > 0 && s[i] != t[j]) {
            j = pi[j - 1];
        }
        if (s[i] == t[j]) {
            j++;
        }
        if (j == m) {
            ans.push_back(i - m + 1);
            j = pi[j - 1];
        }
    }

    return ans;
}

// Hàm tiện ích để tìm kiếm chuỗi t trong s dùng KMP
// Trả về các vị trí bắt đầu của t trong s
std::vector<int> kmp_search(const std::string& s, const std::string& t) {
    int n = static_cast<int>(s.size());
    
    std::vector<int> s2(n);
    for (int i = 0; i < n; ++i) {
        s2[i] = s[i];
    }

    int m = static_cast<int>(t.size());

    std::vector<int> t2(m);
    for (int i = 0; i < m; ++i) {
        t2[i] = t[i];
    }

    return kmp_search(s2, t2);
}

} // namespace alg
} // namespace cp
#endif