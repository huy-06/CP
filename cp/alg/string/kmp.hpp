#include <vector>
#include <string>

#ifndef CP_ALG_STRING_KMP
#define CP_ALG_STRING_KMP
namespace cp {
namespace alg {

std::vector<int> kmp(const std::string& s) {
    int n = s.size();
    std::vector<int> pi(n, 0);
    for (int i = 1; i < n; ++i) {
        int j = pi[i - 1];
        while (j > 0 && s[i] != s[j])
            j = pi[j - 1];
        if (s[i] == s[j])
            ++j;
        pi[i] = j;
    }
    return pi;
}

} // namespace alg
} // namespace cp
#endif