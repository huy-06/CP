#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include "../../init/constexpr.hpp"

#ifndef CP_DS_BIT_SUBARRAY_XOR
#define CP_DS_BIT_SUBARRAY_XOR
namespace cp {
namespace ds {

template <typename Tp>
class subarray_xor {
public:
    static cp_constexpr int bits = sizeof(Tp) * 8;
    using value_type = Tp;

    subarray_xor() { 
        clear();
    }

    void clear() {
        basis.fill(0);
        rank = 0;
    }

    bool insert(value_type x) {
        for (int i = bits - 1; i >= 0; --i) {
            if (!((x >> i) & 1)) continue;
            if (!basis[i]) {
                basis[i] = x;
                ++rank;
                return true;
            }
            x ^= basis[i];
        }
        return false;
    }

    bool contains(value_type x) const {
        for (int i = bits - 1; i >= 0; --i) {
            if (!((x >> i) & 1)) continue;
            if (!basis[i]) return false;
            x ^= basis[i];
        }
        return true;
    }

    value_type max(value_type x = 0) const {
        value_type res = x;
        for (int i = bits - 1; i >= 0; --i) {
            res = std::max(res, res ^ basis[i]);
        }
        return res;
    }

    value_type min(value_type x = 0) const {
        value_type res = x;
        for (int i = bits - 1; i >= 0; --i) {
            value_type cand = res ^ basis[i];
            if (cand < res) res = cand;
        }
        return res;
    }

    int size() const {
        return rank;
    }

    template <typename U>
    void merge(const subarray_xor<U>& other) {
        for (int i = 0; i < subarray_xor<U>::bits; ++i) {
            if (other.get_basis(i)) {
                insert(other.get_basis(i));                
            }
        }
    }

    value_type get_basis(int i) const {
        return basis[i];
    }

private:
    std::array<value_type, bits> basis;
    int rank;
};

} // namespace ds
} // namespace cp
#endif