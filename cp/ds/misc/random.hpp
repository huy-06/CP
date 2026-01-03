#include <iostream>
#include <random>
#include <chrono>
#include <vector>
#include <string>
#include <algorithm>
#include <type_traits>
#include <initializer_list>
#include <limits>
#include <cassert>

#ifndef CP_DS_RANDOM
#define CP_DS_RANDOM

namespace cp {
namespace internal {

template <typename T>
using decay_t = typename std::decay<T>::type;

template <typename... T>
using common_type_t = typename std::common_type<T...>::type;

class randomizer {
private:
    static std::mt19937_64& engine() {
        static thread_local std::mt19937_64 gen([]() {
            std::random_device rd;
            auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
            std::seed_seq ss{
                static_cast<uint64_t>(rd()),
                static_cast<uint64_t>(now),
                static_cast<uint64_t>(reinterpret_cast<uintptr_t>(&now))
            };
            return std::mt19937_64(ss);
        }());
        return gen;
    }

    template <typename Tp>
    struct interval_proxy {
        Tp min_val, max_val;

        template <typename U>
        friend const interval_proxy& operator>>(const interval_proxy& proxy, U& var) {
            randomizer::distribute(proxy.min_val, proxy.max_val, var);
            return proxy;
        }
    };

    template <typename Tp>
    struct list_proxy {
        std::vector<Tp> data;

        list_proxy(std::initializer_list<Tp> list) : data(list) {}

        template <typename U>
        friend const list_proxy& operator>>(const list_proxy& proxy, U& var) {
            if (proxy.data.empty()) return proxy;
            std::uniform_int_distribution<size_t> dist(0, proxy.data.size() - 1);
            var = proxy.data[dist(engine())];
            return proxy;
        }
    };

    template <typename T, typename U>
    static void distribute(T min_val, T max_val, U& var) {
        using common = common_type_t<T, U>;
        #if __cplusplus >= 201703L
            if constexpr (std::is_integral<common>::value) {
                std::uniform_int_distribution<common> dist(min_val, max_val);
                var = static_cast<U>(dist(engine()));
            } else {
                std::uniform_real_distribution<common> dist(min_val, max_val);
                var = static_cast<U>(dist(engine()));
            }
        #else
            distribute_impl(min_val, max_val, var, std::is_integral<common>());
        #endif
    }

#if __cplusplus < 201703L
    template <typename T, typename U>
    static void distribute_impl(T min_val, T max_val, U& var, std::true_type) {
        std::uniform_int_distribution<typename std::common_type<T, U>::type> dist(min_val, max_val);
        var = static_cast<U>(dist(engine()));
    }
    template <typename T, typename U>
    static void distribute_impl(T min_val, T max_val, U& var, std::false_type) {
        std::uniform_real_distribution<typename std::common_type<T, U>::type> dist(min_val, max_val);
        var = static_cast<U>(dist(engine()));
    }
#endif

public:
    template<typename Tp = int64_t>
    Tp next_int(Tp l = std::numeric_limits<Tp>::min(), Tp r = std::numeric_limits<Tp>::max()) {
        std::uniform_int_distribution<Tp> dist(l, r);
        return dist(engine());
    }

    template<typename Tp = double>
    Tp next_float(Tp l = 0.0, Tp r = 1.0) {
        std::uniform_real_distribution<Tp> dist(l, r);
        return dist(engine());
    }

    bool next_bool() {
        std::uniform_int_distribution<int> dist(0, 1);
        return dist(engine()) == 1;
    }

    char next_char(char min_char = 'a', char max_char = 'z') {
        std::uniform_int_distribution<int> dist(min_char, max_char);
        return static_cast<char>(dist(engine()));
    }

    std::string next_string(int n, const std::string& charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz") {
        if (charset.empty()) return "";
        std::string res(n, 0);
        std::uniform_int_distribution<size_t> dist(0, charset.size() - 1);
        for (int i = 0; i < n; ++i) {
            res[i] = charset[dist(engine())];
        }
        return res;
    }

    template<typename Tp = int64_t>
    std::vector<Tp> next_vector(int n, Tp l = std::numeric_limits<Tp>::min(), Tp r = std::numeric_limits<Tp>::max()) {
        std::vector<Tp> res(n);
        std::uniform_int_distribution<Tp> dist(l, r);
        for (auto& x : res) x = dist(engine());
        return res;
    }

    template<typename Tp>
    std::vector<Tp> next_vector(int n, std::initializer_list<Tp> list) {
        std::vector<Tp> src(list);
        std::vector<Tp> res(n);
        if (src.empty()) return res;
        std::uniform_int_distribution<size_t> dist(0, src.size() - 1);
        for (auto& x : res) x = src[dist(engine())];
        return res;
    }

    template<typename Container>
    auto choice(const Container& c) -> typename Container::value_type {
        assert(!c.empty());
        std::uniform_int_distribution<size_t> dist(0, c.size() - 1);
        auto it = std::begin(c);
        std::advance(it, dist(engine()));
        return *it;
    }

    template<typename Tp>
    Tp choice(std::initializer_list<Tp> list) {
        assert(list.size() > 0);
        std::vector<Tp> vec(list);
        return choice(vec);
    }

    template<typename Container>
    void shuffle(Container& c) {
        std::shuffle(std::begin(c), std::end(c), engine());
    }

    bool prob(double p) {
        std::bernoulli_distribution dist(p);
        return dist(engine());
    }


#if __cplusplus >= 201703L
    template <typename Tp>
    randomizer& operator>>(Tp& var) {
        if constexpr (std::is_integral<Tp>::value && !std::is_same<Tp, bool>::value && !std::is_same<Tp, char>::value) {
            var = next_int<Tp>();
        } else if constexpr (std::is_floating_point<Tp>::value) {
            var = next_float<Tp>();
        } else if constexpr (std::is_same<Tp, bool>::value) {
            var = next_bool();
        } else if constexpr (std::is_same<Tp, char>::value) {
            var = next_char();
        }
        return *this;
    }
#else
    // C++11 Implementation (SFINAE)
    template <typename Tp>
    typename std::enable_if<std::is_integral<Tp>::value && !std::is_same<Tp, bool>::value && !std::is_same<Tp, char>::value, randomizer&>::type
    operator>>(Tp& var) {
        var = next_int<Tp>();
        return *this;
    }

    template <typename Tp>
    typename std::enable_if<std::is_floating_point<Tp>::value, randomizer&>::type
    operator>>(Tp& var) {
        var = next_float<Tp>();
        return *this;
    }

    template <typename Tp>
    typename std::enable_if<std::is_same<Tp, bool>::value, randomizer&>::type
    operator>>(Tp& var) {
        var = next_bool();
        return *this;
    }
    
    template <typename Tp>
    typename std::enable_if<std::is_same<Tp, char>::value, randomizer&>::type
    operator>>(Tp& var) {
        var = next_char();
        return *this;
    }
#endif

    template <typename Tp>
    interval_proxy<Tp> operator()(Tp min, Tp max) {
        return interval_proxy<Tp>{min, max};
    }

    template <typename Tp>
    list_proxy<Tp> operator()(std::initializer_list<Tp> list) {
        return list_proxy<Tp>(list);
    }
};

} // namespace internal

namespace ds {
#if __cplusplus >= 201703L
    inline internal::randomizer ran;
#else
    static internal::randomizer ran;
#endif
} // namespace ds

} // namespace cp
#endif