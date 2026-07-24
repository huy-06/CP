#include <bit>
#include <string>
#include <algorithm>
#include "../../init/constexpr.hpp"

#ifndef CP_ALG_BITMASK
#define CP_ALG_BITMASK
namespace cp {
namespace alg {
namespace bit {

template <typename Tp>
cp_constexpr Tp make_mask(int l, int r) noexcept {
    using UTp = typename std::make_unsigned<Tp>::type;
    if (r - l + 1 >= static_cast<int>(sizeof(Tp) * 8)) {
        return static_cast<Tp>(~static_cast<UTp>(0));
    }
    UTp mask = ((static_cast<UTp>(1) << (r - l + 1)) - 1) << l;
    return static_cast<Tp>(mask);
}

template <typename Tp>
cp_constexpr Tp pow2(int i) noexcept {
    using UTp = typename std::make_unsigned<Tp>::type;
    return static_cast<Tp>(static_cast<UTp>(1) << i);
}

template <typename Tp>
cp_constexpr bool is_pow2(Tp x) noexcept {
    using UTp = typename std::make_unsigned<Tp>::type;
    UTp ux = static_cast<UTp>(x);
    return ux > 0 && (ux & (ux - 1)) == 0;
}

template <typename Tp>
cp_constexpr int log2(Tp x) noexcept {
    using UTp = typename std::make_unsigned<Tp>::type;
    UTp ux = static_cast<UTp>(x);
    int ans = -1;
    while (ux > 0) {
        ++ans;
        ux >>= 1;
    }
    return ans;
}

template <typename Tp>
cp_constexpr int len(Tp x) {
    if (x == 0) return 0;
    return log2(x) + 1;
}

template <typename Tp>
cp_constexpr bool get(Tp x, int i) noexcept {
    using UTp = typename std::make_unsigned<Tp>::type;
    return static_cast<bool>((static_cast<UTp>(x) >> i) & 1);
}

template<typename Tp>
cp_constexpr int popcount(Tp x) noexcept {
    using UTp = typename std::make_unsigned<Tp>::type;
    UTp ux = static_cast<UTp>(x);
    int cnt = 0;
    while (ux > 0) {
        ux &= ux - 1;
        ++cnt;
    }
    return cnt;
}

template <typename Tp>
cp_constexpr int parity(Tp x) noexcept {
    return popcount(x) & 1;
}

template <typename Tp>
cp_constexpr int countl_zero(Tp x) noexcept {
    if (x == 0) return static_cast<int>(sizeof(Tp) * 8);
    return static_cast<int>(sizeof(Tp) * 8) - 1 - log2(x);
}

template <typename Tp>
cp_constexpr int countr_zero(Tp x) noexcept {
    if (x == 0) return static_cast<int>(sizeof(Tp) * 8);
    using UTp = typename std::make_unsigned<Tp>::type;
    UTp ux = static_cast<UTp>(x);
    int ans = 0;
    while ((ux & 1) == 0) {
        ++ans;
        ux >>= 1;
    }
    return ans;
}

// Tìm lũy thừa của 2 nhỏ nhất không bé hơn x (ví dụ: 5 -> 8, 8 -> 8, 0 -> 1)
template <typename Tp>
cp_constexpr Tp bit_ceil(Tp x) noexcept {
    using UTp = typename std::make_unsigned<Tp>::type;
    UTp ux = static_cast<UTp>(x);
    if (ux <= 1) return static_cast<Tp>(1);
    return static_cast<Tp>(static_cast<UTp>(1) << len(static_cast<UTp>(ux - 1)));
}

// Tìm lũy thừa của 2 lớn nhất không vượt quá x (ví dụ: 5 -> 4, 8 -> 8, 0 -> 0)
template <typename Tp>
cp_constexpr Tp bit_floor(Tp x) noexcept {
    using UTp = typename std::make_unsigned<Tp>::type;
    UTp ux = static_cast<UTp>(x);
    if (ux == 0) return static_cast<Tp>(0);
    return static_cast<Tp>(static_cast<UTp>(1) << log2(ux));
}

template <typename Tp>
cp_constexpr int topbit(Tp x) noexcept {
    return log2(x);
}

template <typename Tp>
cp_constexpr int lowbit(Tp x) noexcept {
    if (x == 0) return -1;
    return countr_zero(x);
}

template <typename Tp>
cp_constexpr Tp flip(Tp x, int i) noexcept {
    using UTp = typename std::make_unsigned<Tp>::type;
    return static_cast<Tp>(static_cast<UTp>(x) ^ (static_cast<UTp>(1) << i));
}

template <typename Tp>
cp_constexpr Tp flip(Tp x, int l, int r) noexcept {
    return x ^ make_mask<Tp>(l, r);
}

template <typename Tp>
cp_constexpr Tp on(Tp x, int i) noexcept {
    using UTp = typename std::make_unsigned<Tp>::type;
    return static_cast<Tp>(static_cast<UTp>(x) | (static_cast<UTp>(1) << i));
}

template <typename Tp>
cp_constexpr Tp on(Tp x, int l, int r) noexcept {
    using UTp = typename std::make_unsigned<Tp>::type;
    return static_cast<Tp>(static_cast<UTp>(x) | static_cast<UTp>(make_mask<Tp>(l, r)));
}

template <typename Tp>
cp_constexpr Tp off(Tp x, int i) noexcept {
    using UTp = typename std::make_unsigned<Tp>::type;
    return static_cast<Tp>(static_cast<UTp>(x) & ~(static_cast<UTp>(1) << i));
}

template <typename Tp>
cp_constexpr Tp off(Tp x, int l, int r) noexcept {
    using UTp = typename std::make_unsigned<Tp>::type;
    return static_cast<Tp>(static_cast<UTp>(x) & ~static_cast<UTp>(make_mask<Tp>(l, r)));
}

template <typename Tp>
inline std::string string(Tp x) {
    using UTp = typename std::make_unsigned<Tp>::type;
    UTp ux = static_cast<UTp>(x);
    int n = sizeof(Tp) * 8;
    std::string s(n, '0');
    for (int i = 0; i < n; ++i) {
        if ((ux >> (n - 1 - i)) & 1) {
            s[i] = '1';
        }
    }
    return s;
}

template <typename Tp>
inline Tp to_int(const std::string& s) noexcept {
    using UTp = typename std::make_unsigned<Tp>::type;
    UTp res = 0;
    for (char c : s) {
        res = (res << 1) | (c - '0');
    }
    return static_cast<Tp>(res);
}

// Trả về giá của bit nhỏ nhất được bật (ví dụ: 12 (1100) -> trả về 4 (0100))
template <typename Tp>
cp_constexpr Tp lsb_val(Tp x) noexcept {
    using UTp = typename std::make_unsigned<Tp>::type;
    UTp ux = static_cast<UTp>(x);
    return static_cast<Tp>(ux & -ux);
}

// Thuật toán Gosper's Hack: Tìm số tiếp theo lớn hơn x nhưng có cùng số bit 1
template <typename Tp>
cp_constexpr Tp next_popcount_equal(Tp x) noexcept {
    using UTp = typename std::make_unsigned<Tp>::type;
    UTp ux = static_cast<UTp>(x);
    if (ux == 0) return 0;
    UTp c = ux & -ux;
    UTp r = ux + c;
    return static_cast<Tp>(((r ^ ux) >> 2) / c | r);
}

} // namespace bit
} // namespace alg
} // namespace cp
#endif