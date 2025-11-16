#include <vector>
#include <type_traits>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <ext/pb_ds/assoc_container.hpp>
#include "../alg/math/pow.hpp"

#ifndef CP_CPPX_ALG
#define CP_CPPX_ALG
namespace cp {
    
using std::abs;
using std::ceil;
using std::swap;

template <typename Tp>
constexpr int len(const Tp& a) {
    return int(a.size());
}
    
template <typename T1, typename T2>
constexpr auto min(const T1& a, const T2& b) -> typename std::common_type<T1, T2>::type {
    using type = typename std::common_type<T1, T2>::type;
    return a < b ? static_cast<type>(a) : static_cast<type>(b);
}

template <typename T1, typename T2, typename... Args>
constexpr auto min(const T1& a, const T2& b, const Args&... args) -> typename std::common_type<T1, T2, Args...>::type {
    return min(min(a, b), args...);
}

template <typename Tp>
constexpr Tp min(const std::vector<Tp>& a) {
    return *std::min_element(a.begin(), a.end());
}

template <typename T1, typename T2>
constexpr auto max(const T1& a, const T2& b) -> typename std::common_type<T1, T2>::type {
    using type = typename std::common_type<T1, T2>::type;
    return b < a ? static_cast<type>(a) : static_cast<type>(b);
}

template <typename T1, typename T2, typename... Args>
constexpr auto max(const T1& a, const T2& b, const Args&... args) -> typename std::common_type<T1, T2, Args...>::type {
    return max(max(a, b), args...);
}

template <typename Tp>
constexpr Tp max(const std::vector<Tp>& a) {
    return *std::max_element(a.begin(), a.end());
}

template <typename Tp>
constexpr Tp sum(const std::vector<Tp>& a) {
    return std::accumulate(a.begin(), a.end(), Tp());
}

template <typename Tp>
constexpr int count(const std::vector<Tp>& a, const Tp& x) {
    return int(std::count(a.begin(), a.end(), x));
}

constexpr int count(const std::string& a, const char& x) {
    return int(std::count(a.begin(), a.end(), x));
}

template <typename Tp>
constexpr int find(const std::vector<Tp>& a, const Tp& x) {
    return int(std::find(a.begin(), a.end(), x) - a.begin());
}

template <typename Tp>
constexpr int lower_bound(const std::vector<Tp>& a, const Tp& x) {
    return int(std::lower_bound(a.begin(), a.end(), x) - a.begin());
}

template <typename Tp>
constexpr int upper_bound(const std::vector<Tp>& a, const Tp& x) {
    return int(std::upper_bound(a.begin(), a.end(), x) - a.begin());
}

template <typename Tp>
constexpr Tp rev(Tp a) {
    std::reverse(a.begin(), a.end());
    return a;
}

template <typename Tp>
constexpr Tp sort(Tp a, bool rev = false) {
    if (rev) std::sort(a.rbegin(), a.rend());
    else std::sort(a.begin(), a.end());
    return a;
}

template <typename Tp, typename Fun>
constexpr Tp sort(Tp a, Fun f) {
    std::sort(a.begin(), a.end(), f);
    return a;
}

template <typename Tp>
constexpr Tp unique(Tp a) {
    sort(a);
    a.erase(std::unique(a.begin(), a.end()), a.end());
    return a;
}

template <typename Tp = int64_t>
constexpr std::vector<Tp> iota(const int& n, const Tp& i = 0) {
    std::vector<Tp> a(n);
    std::iota(a.begin(), a.end(), Tp(i));
    return a;
}

template <typename Tp>
constexpr std::vector<Tp> nvec() {
    return std::vector<Tp>();
}

template <typename Tp>
constexpr std::vector<Tp> nvec(const int& n, const Tp& v = Tp()) {
    return std::vector<Tp>(n, v);
}

template<typename Tp>
constexpr Tp sqrt(const Tp& n) {
    Tp l = 0, r = n, ans = 0;
    while (l <= r) {
        Tp m = l + ((r - l) >> 1);
        if (m <= n / m) {
            ans = m; l = m + 1;
        } else {
            r = m - 1;
        }
    }
    return ans;
}

template<typename Tp>
constexpr Tp gcd(Tp a, Tp b) {
    while (b) {
        Tp t = b;
        b = a % b;
        a = t;
    }
    return a;
}

template<typename Tp>
constexpr Tp lcm(Tp a, Tp b) {
    return a * b / gcd(a, b);
}

template <typename... Args>
inline void inc(Args&... args) {
    using expander = int[];
    (void)expander{0, (++args, 0)...};
}

template <typename... Args>
inline void dec(Args&... args) {
    using expander = int[];
    (void)expander{0, (--args, 0)...};
}

template <typename T1, typename T2>
bool umin(T1& a, const T2& b) {
    if (b < a) {
        a = b;
        return true;
    }
    return false;
}

template <typename T1, typename T2>
bool umax(T1& a, const T2& b) {
    if (a < b) {
        a = b;
        return true;
    }
    return false;
}

namespace internal {
class string_view {
public:
    template <typename Tp>
    std::string stringify(const Tp& v) {
        std::ostringstream oss;
        oss.copyfmt(std::cout);
        oss << v;
        return oss.str();
    }

    template<typename T1, typename T2>
    std::string stringify(const std::pair<T1, T2>& v) {
        return stringify(v.first) + ' ' + stringify(v.second);
    }

    template<typename Tp>
    std::string stringify(const std::vector<Tp>& v) {
        std::string s;
        bool ok = false;
        for (const auto& i : v) {
            if (ok) s += ' ';
            s += stringify(i);
            ok = true;
        }
        return s;
    }

    template<typename Tp>
    std::string stringify(const __gnu_pbds::tree<Tp, 
                                __gnu_pbds::null_type,
                                std::less<Tp>,
                                __gnu_pbds::rb_tree_tag,
                                __gnu_pbds::tree_order_statistics_node_update>& v){
        std::string s;
        bool ok = false;
        for (const auto& i : v) {
            if (ok) s += ' ';
            s += stringify(i);
            ok = true;
        }
        return s;
    }
} sview;
} // namespace internal

template <typename Tp>
std::string tostr(const Tp& v) {
    return internal::sview.stringify(v);
}

template <typename Tp>
Tp slice(const Tp& a, int l, int r) {
    const int n = int(a.size());
    if (l < 0) l += n;
    if (r < 0) r += n;
    l = max(l, 0);
    r = min(r, n);
    if (l >= r) return Tp();
    return Tp(std::begin(a) + l, std::begin(a) + r);
}

template <typename Tp> 
int popcount(const Tp& x) {
    return int(__builtin_popcountll(x));
}


} // namespace cp
#endif
