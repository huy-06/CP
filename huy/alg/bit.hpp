#include <bit>
#include <string>
#include <algorithm>

#ifndef HUY_ALG_BIT
#define HUY_ALG_BIT
namespace huy {
namespace alg {

namespace bit {
template<typename Tp>
using _make_unsigned = typename std::make_unsigned<Tp>::type;

template<typename Tp>
constexpr int width() noexcept {
    return static_cast<int>(sizeof(Tp) * 8);
}

template<typename Tp>
constexpr _make_unsigned<Tp> pow2(int n) noexcept {
    using up = _make_unsigned<Tp>;
    return (n < 0 || n >= width<Tp>()) ? 0 : (up(1) << n);
}

template<typename Tp>
constexpr int popcount(Tp x) noexcept {
    using up = _make_unsigned<Tp>;
    up u = static_cast<up>(x);
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_popcountll(static_cast<unsigned long long>(u));
#else
    int c = 0;
    while (u) {
        u &= (u - 1);
        ++c;
    }
    return c;
#endif
}

template<typename Tp>
constexpr int parity(Tp x) noexcept {
    return popcount(x) & 1;
}

template<typename Tp>
constexpr int clz(Tp x) noexcept {
    using up = _make_unsigned<Tp>;
    up u = static_cast<up>(x);
    if (u == 0) return width<Tp>();
#if defined(__GNUC__) || defined(__clang__)
    int w = width<Tp>();
    return __builtin_clzll(static_cast<unsigned long long>(u)) - (64 - w);
#else
    int cnt = 0, w = width<Tp>();
    for (int i = w - 1; i >= 0; --i)
        if (u >> i & 1) return cnt;
        else ++cnt;
    return cnt;
#endif
}

template<typename Tp>
constexpr int ctz(Tp x) noexcept {
    using up = _make_unsigned<Tp>;
    up u = static_cast<up>(x);
    if (u == 0) return width<Tp>();
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_ctzll(static_cast<unsigned long long>(u));
#else
    int cnt = 0;
    while ((u & 1) == 0) {
        u >>= 1;
        ++cnt;
    }
    return cnt;
#endif
}

template<typename Tp>
constexpr int log2(Tp x) noexcept {
    return x == 0 ? -1 : width<Tp>() - 1 - clz(x);
}

template<typename Tp>
constexpr int topbit(Tp x) noexcept {
    return x == 0 ? -1 : log2(x);
}

template<typename Tp>
constexpr int lowbit(Tp x) noexcept {
    return x == 0 ? -1 : ctz(x);
}

template<typename Tp>
constexpr Tp flip(Tp x, int n) noexcept {
    return (n < 0 || n >= width<Tp>()) ? x : Tp(x ^ pow2<Tp>(n));
}

template<typename Tp>
constexpr Tp on(Tp x, int n) noexcept {
    return (n < 0 || n >= width<Tp>()) ? x : Tp(x | pow2<Tp>(n));
}

template<typename Tp>
constexpr Tp off(Tp x, int n) noexcept {
    return (n < 0 || n >= width<Tp>()) ? x : Tp(x & ~pow2<Tp>(n));
}

template<typename Tp>
constexpr Tp put(Tp x, int n, bool v) noexcept {
    if (n < 0 || n >= width<Tp>()) return x;
    return (x & ~pow2<Tp>(n)) | (Tp(v) << n);
}

template<typename Tp>
inline void toggle(Tp& x, int n) noexcept {
    x = flip(x, n);
}

template<typename Tp>
inline void set(Tp& x, int n) noexcept {
    x = on(x, n);
}

template<typename Tp>
inline void clear(Tp& x, int n) noexcept {
    x = off(x, n);
}

template<typename Tp>
inline void setto(Tp& x, int n, bool v) noexcept {
    x = put(x, n, v);
}

template<typename Tp>
inline bool get(Tp x, int n) noexcept {
    return (x >> n) & 1;
}

template<typename Tp>
std::string to_string(Tp x) {
    using up = _make_unsigned<Tp>;
    up u = static_cast<up>(x);
    if (u == 0) return "0";
    std::string s;
    while (u > 0) {
        s.push_back(char('0' + (u & 1)));
        u >>= 1;
    }
    std::reverse(s.begin(), s.end());
    return s;
}

template<typename Tp>
_make_unsigned<Tp> to_int(const std::string& s, bool* overflow = nullptr) noexcept {
    using up = _make_unsigned<Tp>;
    up res = 0;
    bool of = false;
    const int w = width<Tp>();
    int pos = 0;
    for (int i = static_cast<int>(s.size()) - 1; i >= 0; --i) {
        char c = s[i];
        if (c != '0' && c != '1') continue;
        if (c == '1') {
            if (pos >= w) {
                of = true;
            } else {
                res |= (up(1) << pos);
            }
        }
        ++pos;
    }
    if (overflow) *overflow = of;
    return res;
}

} // namespace bit
} // namespace alg
} // namespace huy
#endif