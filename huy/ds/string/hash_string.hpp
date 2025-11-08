#include <vector>
#include <string>
#include "../num/mod/static.hpp"

#ifndef HUY_DS_STRING_HASH_STRING
#define HUY_DS_STRING_HASH_STRING
namespace huy {
namespace ds {

class hash_string {
public:
    using value_type = static_mod_int<(1ll << 61) - 1>;

    hash_string() {
        pref.assign(1, 0);
        ensure_pow(0);
    }

    hash_string(const std::string& s) {
        init(s);
    }

    void init(const std::string& s) {
        int n = int(s.size());
        pref.assign(n + 1, 0);
        ensure_pow(n);
        for (int i = 0; i < n; ++i) {
            pref[i + 1] = pref[i] * base + s[i];
        }
    }

    // range [l : r)
    value_type get(int l, int r) const {
        assert(0 <= l && r < int(pref.size()));
        return pref[r] - pref[l] * pow[r - l];
    }
private:
    static constexpr value_type base = 1315423911ull;
    static std::vector<value_type> pow;

    std::vector<value_type> pref;

    void ensure_pow(int n) {
        if (pow.empty()) {
            pow.push_back(1);
        }
        if (int(pow.size()) <= n) {
            pow.reserve(n + 1);
            while (int(pow.size()) <= n) {
                pow.push_back(pow.back() * base);
            }
        }
    }
};
std::vector<hash_string::value_type> hash_string::pow;

} // namespace ds
} // namespace huy
#endif