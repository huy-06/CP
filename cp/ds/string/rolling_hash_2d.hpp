#include <vector>
#include <string>
#include "../../alg/utils/random.hpp"
#include "../numeric/modular/int_61.hpp"

#ifndef CP_DS_STRING_ROLLING_HASH_2D
#define CP_DS_STRING_ROLLING_HASH_2D
namespace cp {
namespace ds {

class rolling_hash_2d {
public:
    using value_type = mod_int_61;

    rolling_hash_2d() : n(0), m(0) {}

    template <typename Array>
    explicit rolling_hash_2d(const Array& a) {
        init(a);
    }

    template <typename Array>
    void init(const Array& a) {
        n = int(a.size());
        m = (n == 0) ? 0 : int(a[0].size());

        if (base_r.val() == 0) {
            auto dist = std::uniform_int_distribution<unsigned long long>(
                256, 
                value_type::mod() - 1
            );
            base_r = value_type(dist(alg::random64));
            base_c = value_type(dist(alg::random64));
            while (base_r == base_c) {
                base_c = value_type(dist(alg::random64));
            }
        }

        prepare_pow(pow_r, n, base_r);
        prepare_pow(pow_c, m, base_c);

        pref.assign((n + 1) * (m + 1), value_type(0));
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                value_type res = get_pref(i, j + 1) * base_r
                               + get_pref(i + 1, j) * base_c
                               - get_pref(i, j) * (base_r * base_c)
                               + value_type(a[i][j]);
                set_pref(i + 1, j + 1, res);
            }
        }
    }

    // Lấy hash đoạn [xs, xe), [ys, ye) (0-based)
    value_type get(int xs, int ys, int xe, int ye) const {
        assert(0 <= xs && xs <= xe && xe <= n);
        assert(0 <= ys && ys <= ye && ye <= m);
        if (xs == xe || ys == ye) return value_type(0);

        value_type res = get_pref(xe, ye)
                       - get_pref(xs, ye) * pow_r[xe - xs]
                       - get_pref(xe, ys) * pow_c[ye - ys]
                       + get_pref(xs, ys) * (pow_r[xe - xs] * pow_c[ye - ys]);
                       
        return res;
    }
    
    // Lấy hash cả chuỗi
    value_type get() const {
        return get(0, 0, n, m);
    }
    
private:
    int n, m;
    std::vector<value_type> pref;

    static value_type base_r, base_c;
    static std::vector<value_type> pow_r, pow_c;

    inline void set_pref(int i, int j, value_type val) {
        pref[i * (m + 1) + j] = val;
    }

    inline value_type get_pref(int i, int j) const {
        return pref[i * (m + 1) + j];
    }

    void prepare_pow(std::vector<value_type>& p, int limit, value_type b) {
        if (p.empty()) p.push_back(value_type(1));
        if (int(p.size()) <= limit) {
            p.reserve(limit + 1);
            while (int(p.size()) <= limit) {
                p.push_back(p.back() * b);
            }
        }
    }
};

rolling_hash_2d::value_type rolling_hash_2d::base_r = rolling_hash_2d::value_type(0);
rolling_hash_2d::value_type rolling_hash_2d::base_c = rolling_hash_2d::value_type(0);
std::vector<rolling_hash_2d::value_type> rolling_hash_2d::pow_r = {rolling_hash_2d::value_type(1)};
std::vector<rolling_hash_2d::value_type> rolling_hash_2d::pow_c = {rolling_hash_2d::value_type(1)};


} // namespace ds
} // namespace cp
#endif