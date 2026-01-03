#include <vector>
#include <array>
#include <chrono>

#ifndef CP_DS_HASH_CUSTOM_HASH
#define CP_DS_HASH_CUSTOM_HASH
namespace cp {
namespace internal {

class custom_hash {
public:
    template <typename T>
    typename std::enable_if<std::is_integral<T>::value || std::is_enum<T>::value, size_t>::type
    operator()(T x) const {
        return static_cast<size_t>(splitmix64(static_cast<uint64_t>(x)));
    }

    template <typename T>
    typename std::enable_if<!std::is_integral<T>::value && !std::is_enum<T>::value, size_t>::type
    operator()(const T& x) const {
        static std::hash<T> hasher;
        return static_cast<size_t>(splitmix64(hasher(x)));
    }

    template <typename T1, typename T2>
    size_t operator()(const std::pair<T1, T2>& p) const {
        size_t h1 = (*this)(p.first);
        size_t h2 = (*this)(p.second);
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }

    template <typename T>
    size_t operator()(const std::vector<T>& v) const {
        size_t h = 0;
        for (const T& x : v) {
            h ^= (*this)(x) + 0x9e3779b9 + (h << 6) + (h >> 2);
        }
        return h;
    }

    template <typename T, size_t N>
    size_t operator()(const std::array<T, N>& a) const {
        size_t h = 0;
        for (const T& x : a) {
            h ^= (*this)(x) + 0x9e3779b9 + (h << 6) + (h >> 2);
        }
        return h;
    }

private:
    static uint64_t splitmix64(uint64_t x) {
        static const uint64_t FIXED_RANDOM = 
            static_cast<uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count());

        x += FIXED_RANDOM + 0x9e3779b97f4a7c15;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb;

        return x ^ (x >> 31);
    }
};

} // namespace internal
} // namespace cp
#endif