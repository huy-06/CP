#include <cassert>
#include "suffix_array.hpp"

#ifndef CP_ALG_STRING_LCP_ARRAY
#define CP_ALG_STRING_LCP_ARRAY
namespace cp {
namespace alg {

template <typename Tp>
std::vector<int> lcp_array(const std::vector<Tp>& s, const std::vector<int>& sa) {
    assert(s.size() == sa.size());
    int n = static_cast<int>(s.size());
    assert(n >= 1);
    std::vector<int> rnk(n);
    for (int i = 0; i < n; ++i) {
        assert(0 <= sa[i] && sa[i] < n);
        rnk[sa[i]] = i;
    }
    std::vector<int> lcp(n - 1);
    int h = 0;
    for (int i = 0; i < n; ++i) {
        if (h > 0) --h;
        if (rnk[i] == 0) continue;
        for (int j = sa[rnk[i] - 1]; j + h < n && i + h < n; ++h) {
            if (s[j + h] != s[i + h]) break;
        }
        lcp[rnk[i] - 1] = h;
    }
    return lcp;
}

std::vector<int> lcp_array(const std::string& s, const std::vector<int>& sa) {
    int n = static_cast<int>(s.size());
    std::vector<int> s2(n);
    for (int i = 0; i < n; ++i) {
        s2[i] = s[i];
    }
    return lcp_array(s2, sa);
}

} // namespace alg
} // namespace cp
#endif