#include <string>
#include <vector>
#include <array>
#include <type_traits>
#include <ext/pb_ds/assoc_container.hpp>

#ifndef HUY_DS_HASH_UNORDERED_MAP
#define HUY_DS_HASH_UNORDERED_MAP
namespace huy {

namespace internal {

struct custom_hash {
public:    
    template <typename Tp>
    constexpr size_t operator()(const Tp& x) const {
        return splitmix64(static_cast<size_t>(x));
    }
    
    constexpr size_t operator()(const std::string& s) const {
        size_t res = 0;
        for (auto& c : s) {
            res = (res * 131) + static_cast<size_t>(c);
        }
        return splitmix64(res);
    }
    
    template<typename T1, typename T2>
    constexpr size_t operator()(const std::pair<T1, T2>& p) const {
        size_t h1 = operator()(p.first);
        size_t h2 = operator()(p.second);
        return comb(h1, h2);
    }
    
    template<typename Tp>
    constexpr size_t operator()(const std::vector<Tp>& v) const {
        size_t res = 0;
        for (const Tp& x : v) {
            res = comb(res, operator()(x));
        }
        return splitmix64(res);
    }
    
    template<typename Tp, size_t N>
    constexpr size_t operator()(const std::array<Tp, N>& a) const {
        size_t res = 0;
        for (const Tp& x : a) {
            res = comb(res, operator()(x));
        }
        return splitmix64(res);
    }

private:
    static constexpr size_t comb(const size_t& a, const size_t& b) {
        return a ^ (b + 0x9e3779b9 + (a << 6) + (a >> 2));
    }
    
    static constexpr size_t splitmix64(size_t x) {
        x += 0x9e3779b97f4a7c15;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
        return x ^ (x >> 31);
    }
};

} // namespace internal

namespace ds {

template <class K, class V>
using unordered_map = __gnu_pbds::gp_hash_table<K, 
                                                V, 
                                                internal::custom_hash, 
                                                std::equal_to<K>, 
                                                __gnu_pbds::direct_mask_range_hashing<>,
                                                __gnu_pbds::linear_probe_fn<>,
                                                __gnu_pbds::hash_standard_resize_policy<
                                                    __gnu_pbds::hash_exponential_size_policy<>, 
                                                    __gnu_pbds::hash_load_check_resize_trigger<>, true>>;


template <typename Tp>
using unordered_set = unordered_map<Tp, __gnu_pbds::null_type>;

} // namespace ds

} // namespace huy
#endif