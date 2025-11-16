#include "alg.hpp"

#ifndef CP_CPPX_WRITE
#define CP_CPPX_WRITE

namespace cp {

namespace internal {

template<typename Tp>
void write(bool& ok, const Tp& v) {
    if (ok) std::cout << ' ';
    std::cout << tostr(v);
    ok = true;
}

} // namespace internal

template<typename... Args>
void out(const Args&... args) {
    bool ok = false;
    using expander = int32_t[];
    (void)expander{0, ((void) internal::write(ok, args), 0)...};
    std::cout << '\n';
}

// template<typename... Args>
// void ouf(const std::string& fmt, const Args&... args) {
//     std::vector<std::string> parts = { tostr(args)... };
//     int i = 0, ai = 0;
//     while (i < int(fmt.size())) {
//         if (fmt[i] == '{' && i + 1 < int(fmt.size()) && fmt[i + 1] == '}') {
//             if (ai < int(parts.size())) {
//                 std::cout << parts[ai++];
//             } else {
//                 std::cout << "{}";
//             }
//             i += 2;
//         } else {
//             std::cout << fmt[i++];
//         }
//     }
//     while (ai < int(parts.size())) {
//         std::cout << ' ' << parts[ai++];
//     }
//     std::cout << '\n';
// }

template<typename... Args>
void ouf(const std::string& fmt, const Args&... args) {
    std::vector<std::string> parts = { tostr(args)... };
    int ai = 0;
    for (int i = 0; i < int(fmt.size()); ++i) {
        if (fmt[i] == '$') {
            if (ai < int(parts.size())) {
                std::cout << parts[ai++];
            } else {
                std::cout << '$';
            }
        } else {
            std::cout << fmt[i];
        }
    }
    while (ai < int(parts.size())) {
        std::cout << ' ' << parts[ai++];
    }
    std::cout << '\n';
}

} // namespace cp

#endif