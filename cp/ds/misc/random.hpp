#include <random>
#include <chrono>
#include <vector>
#include <algorithm>
#include <concepts>
#include <ranges>

#ifndef CP_DS_RANDOM
#define CP_DS_RANDOM
namespace cp::internal {

class randomizer {
private:
    static auto& engine() {
        static thread_local std::mt19937_64 gen([]{
            std::random_device rd;
            std::seed_seq ss{ 
                static_cast<uint32_t>(rd()), 
                static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count())
            };
            return std::mt19937_64(ss);
        }());
        return gen;
    }

    template <typename T1, typename T2> 
    struct interval_proxy {
        T1 min_val; T2 max_val;
        
        const interval_proxy& operator>>(std::integral auto& var) const {
            var = std::uniform_int_distribution<std::remove_reference_t<decltype(var)>>(min_val, max_val)(engine());
            return *this;
        }
        
        const interval_proxy& operator>>(std::floating_point auto& var) const {
            var = std::uniform_real_distribution<std::remove_reference_t<decltype(var)>>(min_val, max_val)(engine());
            return *this;
        }
    };

    template <typename Tp> 
    struct list_proxy {
        std::vector<Tp> data;
        const list_proxy& operator>>(auto& var) const {
            if (!data.empty()) var = data[std::uniform_int_distribution<size_t>(0, data.size() - 1)(engine())];
            return *this;
        }
    };

public:
    template<std::integral Tp = int64_t>
    Tp nint(std::type_identity_t<Tp> l = std::numeric_limits<Tp>::min(), 
                std::type_identity_t<Tp> r = std::numeric_limits<Tp>::max()) {
        return std::uniform_int_distribution<Tp>(l, r)(engine());
    }

    template<std::floating_point Tp = double>
    Tp nreal(std::type_identity_t<Tp> l = 0.0, 
                  std::type_identity_t<Tp> r = 1.0) {
        return std::uniform_real_distribution<Tp>(l, r)(engine());
    }

    bool nbool() {
        return nint(0, 1) == 1;
    }

    char nchar(char l = 'a', char r = 'z') {
        return static_cast<char>(nint(l, r));
    }

    std::string nstr(int n, std::string_view charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz") {
        if (charset.empty()) return "";
        std::string res(n, '\0');
        for (char& c : res) c = charset[nint(0, charset.size() - 1)];
        return res;
    }

    template<std::integral Tp = int64_t>
    auto nvec(int n, 
                     std::type_identity_t<Tp> l = std::numeric_limits<Tp>::min(), 
                     std::type_identity_t<Tp> r = std::numeric_limits<Tp>::max()) {
        std::vector<Tp> res(n);
        std::ranges::generate(res, [&]{ return nint<Tp>(l, r); });
        return res;
    }

    template<typename Tp>
    auto nvec(int n, std::initializer_list<Tp> list) {
        std::vector<Tp> src(list), res(n);
        if (src.empty()) return res;
        for (auto& x : res) x = src[nint(0, src.size() - 1)];
        return res;
    }

    auto choice(std::ranges::random_access_range auto const& c) {
        assert(!std::ranges::empty(c));
        return *(std::ranges::begin(c) + nint(0, std::ranges::size(c) - 1));
    }

    template<typename Tp> Tp choice(std::initializer_list<Tp> list) { 
        return *(list.begin() + nint(0, list.size() - 1)); 
    }
    
    void shuffle(std::ranges::random_access_range auto& c) {
        std::ranges::shuffle(c, engine());
    }

    bool prob(double p) {
        return std::bernoulli_distribution(p)(engine());
    }

    randomizer& operator>>(bool& var) {
        var = nbool(); return *this;
    }

    randomizer& operator>>(char& var) {
        var = nchar(); return *this;
    }
    
    randomizer& operator>>(std::integral auto& var) { 
        var = static_cast<std::remove_reference_t<decltype(var)>>(nint()); return *this; 
    }
    
    randomizer& operator>>(std::floating_point auto& var) { 
        var = static_cast<std::remove_reference_t<decltype(var)>>(nreal()); return *this; 
    }

    auto operator()(auto min, auto max) {
        return interval_proxy<decltype(min), decltype(max)>{min, max};
    }

    template <typename Tp> auto operator()(std::initializer_list<Tp> list) {
        return list_proxy<Tp>{list};
    }
};

} // namespace cp::internal

namespace cp::ds {

inline internal::randomizer random;

} // namspace cp::ds
#endif