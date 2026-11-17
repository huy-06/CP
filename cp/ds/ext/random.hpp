#include <random>
#include <chrono>
#include <vector>
#include <algorithm>
#include <concepts>
#include <ranges>

#ifndef CP_DS_RANDOM
#define CP_DS_RANDOM

namespace cp {
namespace ds {

class random {
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
        T1 min_val;
        T2 max_val;
        
        const interval_proxy& operator>>(std::integral auto& var) const {
            using val_t = std::remove_reference_t<decltype(var)>;
            using dist_t = std::conditional_t<(sizeof(val_t) < sizeof(int)) || std::same_as<val_t, bool>, int, val_t>;
            var = static_cast<val_t>(std::uniform_int_distribution<dist_t>(min_val, max_val)(engine()));
            return *this;
        }
        
        const interval_proxy& operator>>(std::floating_point auto& var) const {
            using val_t = std::remove_reference_t<decltype(var)>;
            var = std::uniform_real_distribution<val_t>(min_val, max_val)(engine());
            return *this;
        }
    };

    template <typename Tp> 
    struct list_proxy {
        std::vector<Tp> data;
        const list_proxy& operator>>(auto& var) const {
            if (!data.empty()) {
                var = data[std::uniform_int_distribution<size_t>(0, data.size() - 1)(engine())];
            }
            return *this;
        }
    };

public:
    template<std::integral Tp = int64_t>
    static Tp nint(std::type_identity_t<Tp> l = std::numeric_limits<Tp>::min(), 
                   std::type_identity_t<Tp> r = std::numeric_limits<Tp>::max()) {
        using dist_t = std::conditional_t<(sizeof(Tp) < sizeof(int)) || std::same_as<Tp, bool>, int, Tp>;
        return static_cast<Tp>(std::uniform_int_distribution<dist_t>(l, r)(engine()));
    }

    template<std::floating_point Tp = double>
    static Tp nreal(std::type_identity_t<Tp> l = 0.0, 
                    std::type_identity_t<Tp> r = 1.0) {
        return std::uniform_real_distribution<Tp>(l, r)(engine());
    }

    static bool nbool() {
        return nint(0, 1) == 1;
    }

    static char nchar(char l = 'a', char r = 'z') {
        return static_cast<char>(nint(l, r));
    }

    static std::string nstr(int n, std::string_view charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz") {
        if (charset.empty()) return "";
        std::string res(n, '\0');
        for (char& c : res) c = charset[nint<size_t>(0, charset.size() - 1)];
        return res;
    }

    template<std::integral Tp = int64_t>
    static auto nvec(int n, 
                     std::type_identity_t<Tp> l = std::numeric_limits<Tp>::min(), 
                     std::type_identity_t<Tp> r = std::numeric_limits<Tp>::max()) {
        std::vector<Tp> res(n);
        std::ranges::generate(res, [&]{ return nint<Tp>(l, r); });
        return res;
    }

    template<typename Tp>
    static auto nvec(int n, std::initializer_list<Tp> list) {
        std::vector<Tp> src(list), res(n);
        if (src.empty()) return res;
        for (auto& x : res) x = src[nint<size_t>(0, src.size() - 1)];
        return res;
    }

    static auto choice(std::ranges::random_access_range auto const& c) {
        assert(!std::ranges::empty(c));
        return *(std::ranges::begin(c) + nint<size_t>(0, std::ranges::size(c) - 1));
    }

    template<typename Tp>
    static Tp choice(std::initializer_list<Tp> list) { 
        assert(list.size() > 0);
        return *(list.begin() + nint<size_t>(0, list.size() - 1)); 
    }
    
    static void shuffle(std::ranges::random_access_range auto& c) {
        std::ranges::shuffle(c, engine());
    }

    static bool prob(double p) {
        return std::bernoulli_distribution(p)(engine());
    }

    random& operator>>(bool& var) {
        var = nbool();
        return *this;
    }

    random& operator>>(char& var) {
        var = nchar();
        return *this;
    }
    
    random& operator>>(std::integral auto& var) { 
        var = static_cast<std::remove_reference_t<decltype(var)>>(nint());
        return *this; 
    }
    
    random& operator>>(std::floating_point auto& var) { 
        var = static_cast<std::remove_reference_t<decltype(var)>>(nreal());
        return *this; 
    }

    auto operator()(auto min, auto max) {
        return interval_proxy<decltype(min), decltype(max)>{min, max};
    }

    template <typename Tp>
    auto operator()(std::initializer_list<Tp> list) {
        return list_proxy<Tp>{list};
    }
};

} // namspace ds
} // namespace cp

#endif // CP_DS_RANDOM