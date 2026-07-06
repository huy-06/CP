#include <string>
#include "../mat/matrix.hpp"

#ifndef CP_ALG_GET_FIBONACCI
#define CP_ALG_GET_FIBONACCI
namespace cp {
namespace alg {

template <typename Tp>
Tp get_fibonacci(const std::string& n) {
    std::vector<std::vector<Tp>> res = cp::alg::identity<Tp>(2);

    std::vector<std::vector<Tp>> base = {
        {Tp(1), Tp(1)},
        {Tp(1), Tp(0)}
    };

    for (char c : n) {
        int digit = c - '0';
        res = cp::alg::pow(res, 10);
        res = cp::alg::multiply(res, cp::alg::pow(base, digit));
    }

    return res[0][1];
}

template <typename Tp>
Tp get_fibonacci(long long n) {
    return get_fibonacci<Tp>(std::to_string(n));
}

} // namespace alg
} // namespace cp
#endif