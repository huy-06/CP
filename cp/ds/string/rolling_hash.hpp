#include <vector>
#include <string>
#include "../../alg/utils/random.hpp"
#include "../numeric/modular/int_61.hpp"

#ifndef CP_DS_STRING_ROLLING_HASH
#define CP_DS_STRING_ROLLING_HASH
namespace cp {
namespace ds {

class rolling_hash {
public:
    using value_type = mod_int_61;

    rolling_hash() : n(0) {}

    template <typename Array>
    explicit rolling_hash(const Array& a) {
        init(a);
    }

    template <typename Array>
    void init(const Array& a) {
        n = int(a.size());

        if (base.val() == 0) {
            base = value_type(std::uniform_int_distribution<unsigned long long>(
                256, 
                value_type::mod() - 1
            )(alg::random64));
        }

        prepare_pow(n);

        pref.assign(n + 1, value_type(0));
        for (int i = 0; i < n; ++i) {
            pref[i + 1] = pref[i] * base + value_type(a[i]);
        }
    }

    // Lấy hash đoạn [l, r) (0-based)
    value_type get(int l, int r) const {
        assert(0 <= l && r <= n);
        return pref[r] - pref[l] * pow[r - l];
    }
    
    // Lấy hash cả chuỗi
    value_type get() const {
        return get(0, n);
    }
    
private:
    int n;
    std::vector<value_type> pref;

    static value_type base;
    static std::vector<value_type> pow;

    void prepare_pow(int limit) {
        if (pow.empty()) pow.push_back(value_type(1));
        if (int(pow.size()) <= limit) {
            pow.reserve(limit + 1);
            while (int(pow.size()) <= limit) {
                pow.push_back(pow.back() * base);
            }
        }
    }
};

rolling_hash::value_type rolling_hash::base = rolling_hash::value_type(0);
std::vector<rolling_hash::value_type> rolling_hash::pow = {rolling_hash::value_type(1)};

} // namespace ds
} // namespace cp
#endif