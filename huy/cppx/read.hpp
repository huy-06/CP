#include <vector>
#include <cstdint>
#include <iostream>
#include <sstream>

#ifndef HUY_CPPX_READ
#define HUY_CPPX_READ

namespace huy {

int64_t nint() {
    int64_t x;
    if (!(std::cin >> x)) exit(0);
    return x;
}

long double nfloat() {
    long double x;
    if (!(std::cin >> x)) exit(0);
    return x;
}

std::string nstr() {
    std::string x;
    if (!(std::cin >> x)) exit(0);
    return x;
}

std::string nline() {
    std::string x;
    while (std::getline(std::cin, x) && x.empty());
    if (x.empty()) exit(0);
    return x;
}

std::pair<int64_t, int64_t> npii() {
    std::pair<int64_t, int64_t> p;
    p.first = nint();
    p.second = nint();
    return p;
}

std::vector<int64_t> nvi() {
    std::string x = nline();
    std::stringstream ss(x);
    std::vector<int64_t> v;
    int64_t n;
    while (ss >> n) {
        v.push_back(n);
    }
    return v;
}

std::vector<int64_t> nvi(const int& n, const int64_t& off = 0) {
    std::vector<int64_t> v;
    v.reserve(n);
    for (int i = 0; i < n; ++i) {
        v.push_back(nint() - off);
    }
    return v;
}

std::vector<std::string> nvs() {
    std::string x = nline();
    std::stringstream ss(x);
    std::vector<std::string> v;
    std::string s;
    while (ss >> s) {
        v.push_back(s);
    }
    return v;
}

std::vector<std::string> nvs(const int& n) {
    std::vector<std::string> v;
    v.reserve(n);
    for (int i = 0; i < n; ++i) {
        v.push_back(nstr());
    }
    return v;
}

std::vector<long double> nvf() {
    std::string x = nline();
    std::stringstream ss(x);
    std::vector<long double> v;
    long double n;
    while (ss >> n) {
        v.push_back(n);
    }
    return v;
}

std::vector<long double> nvf(const int& n) {
    std::vector<long double> v;
    v.reserve(n); 
    for (int i = 0; i < n; ++i) {
        v.push_back(nfloat());
    }
    return v;
}

std::vector<std::pair<int64_t, int64_t>> nvii(const int& n, const int64_t& off = 0) {
    std::vector<std::pair<int64_t, int64_t>> v;
    v.reserve(n);
    for (int i = 0; i < n; ++i) {
        int64_t x = nint() - off;
        int64_t y = nint() - off;
        v.push_back({x, y});
    }
    return v;
}

std::vector<std::vector<int64_t>> nvvi(const int& n) {
    std::vector<std::vector<int64_t>> v;
    v.reserve(n);
    for (int i = 0; i < n; ++i) {
        v.push_back(nvi());
    }
    return v;
}

namespace internal {
    template <typename Tp>
    void read(Tp& x) {
        if (!(std::cin >> x)) exit(0);
    }

    template <typename... Args>
    void read(Args&... args) {
    	using expander = int32_t[];
    	(void)expander{0, ((void) read(args), 0)...};
    }
} // namespace internal

} // namespace huy

#define ints(...) int64_t __VA_ARGS__; huy::internal::read(__VA_ARGS__)
#define strs(...) std::string __VA_ARGS__; huy::internal::read(__VA_ARGS__)
#define floats(...) long double __VA_ARGS__; huy::internal::read(__VA_ARGS__)

#endif
