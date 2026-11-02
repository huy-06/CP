#include <string>
#include "../../../ds/num/alge/matrix.hpp"

#ifndef CP_ALG_GET_FIBONACCI
#define CP_ALG_GET_FIBONACCI
namespace cp {
namespace alg {
namespace math {

template <typename Tp>
Tp get_fibonacci(const std::string& n) {
    ds::matrix<Tp> res = ds::matrix<Tp>::identity(2);

    ds::matrix<Tp> base = {
        {Tp(1), Tp(1)},
        {Tp(1), Tp(0)}
    };

    for (char c : n) {
        int digit = c - '0';
        res = res.pow(10);
        res *= base.pow(digit);
    }

    return res[0, 1];
}

template <typename Tp>
Tp get_fibonacci(long long n) {
    return get_fibonacci<Tp>(std::to_string(n));
}

} // namespace math
} // namespace alg
} // namespace cp
#endif